#!/usr/bin/lua
--- serial_master.lua is an optional script to manage the DUT UART connection and publish the log over ZMQ.
-- It continuously publishes the UART traffic to a ZMQ socket (<code>DUT_UART_ZMQ</code>), and takes requests 
-- to send text over UART to the DUT using another ZMQ socket (<code>UART_ZMQ_REQ</code>).
--
-- It uses the following ZMQ Sockets:
-- <ul>
--      <li><span class="parameter">DUT_UART_ZMQ</span> PUB socket</li>
--      <li><span class="parameter">UART_ZMQ_REQ</span> REP socket</li>
-- </ul>
-- @script serial_master.lua
-- @usage serial_master.lua [-c config file] [-u DUT #] [OPT=VAL]
-- @usage serial_master.lua -c config.zmq -u 0 UNIT_STAGE=PRESMT2
require("utils.zhelpers")
local zmq = require("lzmq")
local zpoller = require("lzmq.poller")
local socket = require("utils.socket")
local time_utils = require("utils.time_utils")
local config_utils = require("utils.config_utils")
local lapp = require("pl.lapp")

local args = [[
X527 Serial Master

This is a logger that sits between the DUT UART and a ZMQ socket. It publishes the UART log over ZMQ,
and also writes the log to the text file specified in the config.
    
    -c,--config     (default "config.zmq")          Config module to load.
    -u,--uut        (default 0)                     UUT slot number (used for IP/Port selection)
    <updates...>    (default "")                    Series of X=Y pairs to update the CONFIG table with
]]



--===========================--
-- Global (!!!) config variable
--===========================--
CONFIG = config_utils.get_config(args)
local serial
if CONFIG.SERIAL_PORT == true then
    serial = require("socket.serial")
end

--===========================--
-- Local Buffer
--===========================--
local recv_buffer = ""
local start_time = time_utils.get_unix_time_ms()
local max_send_timeout = 500

--===========================--
-- Setup ZMQ sockets
--===========================--
local context = zmq.context()
local serial_cmd_zmq, err = context:socket(zmq.REP, {bind = config_utils.get_addr(CONFIG, "UART_ZMQ_REQ", CONFIG.ID)})
zassert(serial_cmd_zmq, err)

local dut_serial_output_zmq, err = context:socket(zmq.PUB, {bind = config_utils.get_addr(CONFIG, "DUT_UART_ZMQ", CONFIG.ID)})
zassert(dut_serial_output_zmq, err)

-- local fixture_serial_output_zmq, err = context:socket(zmq.PUB, {bind = CONFIG.get_addr("FIX_UART_ZMQ", CONFIG.ID)})
-- zassert(fixture_serial_output_zmq, err)


--===========================--
-- Open Log Files
--===========================--
local DutLogOut = config_utils.get_log_writer(CONFIG, "DUT", CONFIG.log_prefix)

--===========================--
-- Setup real sockets
--===========================--
local dut_socket = nil
if CONFIG.SERIAL_PORT == true then
    dut_socket = assert(serial(CONFIG.DUT_UART_ADDRESS[CONFIG.ID + 1]))
    dut_socket:settimeout(1)
else
    dut_socket = assert(
        socket.timeout_connect(5,CONFIG.DUT_UART_ADDRESS[CONFIG.ID + 1], CONFIG.DUT_UART_PORT),"HW: "..tostring(CONFIG.ID).." Connect Fail")
    dut_socket:settimeout(1)
    dut_socket:setoption("keepalive",true)
end

--===========================--
-- Serial loop function
--===========================--
local function serial_loop()
    -- Rebroadcast Serial
    local ret, rcvState = dut_socket:receive(1)

    if(rcvState == "closed") then
        error("DUT port Connection Closed.")
    end

    if ret and ret ~= "\0" then
        recv_buffer = recv_buffer .. tostring(ret)
    end

    if recv_buffer:match("\n") or recv_buffer:match("\r") then
        dut_serial_output_zmq:send(recv_buffer)
        if (DutLogOut) then DutLogOut.write(recv_buffer, nil, false) end

        recv_buffer = ""
        start_time = time_utils.get_unix_time_ms()
    end

    if time_utils.get_time_delta_ms(start_time) > max_send_timeout then
        dut_serial_output_zmq:send(recv_buffer)
        if (DutLogOut) then DutLogOut.write(recv_buffer, nil, true) end

        recv_buffer = ""
        start_time = time_utils.get_unix_time_ms()
    end

    -- Process any commands
    local msg = nil
    if serial_cmd_zmq:poll(0) then
        msg = serial_cmd_zmq:recv()
    end
    if msg ~= nil then
        print("MSG: ".. msg)
        command = msg:match("DUT:(.*)")
        dut_socket:send(string.format("%s\r", command))
        serial_cmd_zmq:send("SENT:"..command)
    end
end

while true do
    serial_loop()
end
