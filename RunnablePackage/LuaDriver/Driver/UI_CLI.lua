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


