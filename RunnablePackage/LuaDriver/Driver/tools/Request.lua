#!/usr/bin/env lua
package.path = package.path..";".."/Users/Ryan/Project/TM_Platform_v2/LuaDriver/Driver/?.lua"

require("utils.zhelpers")
local json = require("dkjson")
local zmq = require("lzmq")
local zassert = zmq.assert
local zpoller = require("lzmq.poller")
local zthreads = require("lzmq.threads")
local config_utils = require("utils.config_utils")
local time_utils = require("utils.time_utils")
local lapp = require("pl.lapp")


local args = lapp [[
X527 Test Engine

This is the interface between the test sequencer and the test fixture HW and DUT.
It does all of the required limits checking and conditional execution. Options (such
as IP address, ports, log file names, etc.) are set in config files. These files
are, by default, stored in the config/ directory.
    
    -a,--address    (default "tcp://127.0.0.1:6666")   Config module to load.
    -u,--uut        (default 1)                     UUT slot number (used for IP/Port selection)
    <updates...>    (default "")                    Series of X=Y pairs to update the CONFIG table with
]]

local address = args.address;

local context = zmq.context();	
local request_zmq, err = context:socket(zmq.REQ, {connect = address})
zassert(request_zmq, err)
	
print(time_utils.get_time_string_ms(),"[Sequencer] : Create REQ,connnect to :",address);
	
	function DoStep(item)
		local ret,err= sequence_zmq:send(item);
		assert(ret,err);
		local msg = sequence_zmq:recv();	--return
	end
while true do
	io.write("REQUEST>");
	local cmd = io.read("*line")
	if (cmd and #cmd>0) then 
		local ret,err=request_zmq:send(cmd.."\r\n")
		if (ret) then
			print(time_utils.get_time_string_ms(),"[SEND]:",cmd);
			local buf = request_zmq:recv();
			print(time_utils.get_time_string_ms(),"[RECV]:",buf);
		else
			print("[ERROR]:",err)
		end
	end
end