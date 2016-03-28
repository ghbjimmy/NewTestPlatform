require("utils.zhelpers")
local zmq = require("lzmq")
local zpoller = require("lzmq.poller")
local socket = require("utils.socket")
local time_utils = require("utils.time_utils")
local config_utils = require("utils.config_utils")
local lapp = require("pl.lapp")

local thread = require "llthreads2"
local __print = print;
local print=log

local args = lapp[[
X527 DUT Console

This is a logger that sits between the DUT UART and a ZMQ socket. It publishes the UART log over ZMQ,
and also writes the log to the text file specified in the config.
    
    -c,--config     (default "config.socket_zmq")          Config module to load.
    -u,--uut        (default 0)                     UUT slot number (used for IP/Port selection)
    <updates...>    (default "")                    Series of X=Y pairs to update the CONFIG table with
]]


local thread_string = 
[=====[
require("utils.zhelpers")
local zmq = require("lzmq")
local zpoller = require("lzmq.poller")
local socket = require("utils.socket")
local time_utils = require("utils.time_utils")
local config_utils = require("utils.config_utils")
local lapp = require("pl.lapp")

local thread = require "llthreads2"

local __print = print;
local print=log

local ip={...}
print("SUB : ",ip[1])
local context = zmq.context();
local dut_serial_output_zmq, err = context:socket(zmq.SUB,{subscribe = "",connect   = ip[1]})
zassert(dut_serial_output_zmq, err)

local poller = zpoller.new(2)
function OnSubsriber(socket)
  	local buf = socket:recv()
	--io.write(buf);
	if(buf and #buf>0) then
		print(buf)
	end
end

poller:add(dut_serial_output_zmq, zmq.POLLIN, function()
	OnSubsriber(dut_serial_output_zmq)
end)

print("Start Reading...")

poller:start();
while true do
	local buf = dut_serial_output_zmq:recv()
	io.write(buf);
end
--]=====]

--===========================--
-- Global (!!!) config variable
--===========================--
CONFIG = config_utils.get_config(args)

--Create ZMQ Request and Subscriber
local context = zmq.context()
local ip = config_utils.get_addr(CONFIG, "UART_ZMQ_REQ", CONFIG.ID) --"tcp://127.0.0.1",
print("REQ : ",ip)
local serial_cmd_zmq, err = context:socket(zmq.REQ, {connect =ip })
zassert(serial_cmd_zmq, err)



CONFIG = config_utils.get_config(args)
local ip = config_utils.get_addr(CONFIG, "DUT_UART_ZMQ", CONFIG.ID)
print("sub ip:",ip)
local th= thread.new(thread_string,ip)
th:start();

while true do
--	print("INT>");
	local buf = io.read("*line")
	local ret,err = serial_cmd_zmq:send("DUT:"..buf)
	print("[SEND]".."DUT:"..buf);
	if (not ret) then
		print(err)
	end
	ret= serial_cmd_zmq:recv();
end