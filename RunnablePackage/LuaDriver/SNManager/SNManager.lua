#!/usr/bin/env lua

local zmq = require("lzmq")
local zthreads = require("lzmq.threads")
local lapp = require("pl.lapp")
local json = require("dkjson")
require "pathManager"
package.path = package.path..";"..deleteLastPathComponent(CurrentDir()).."/Driver/?.lua"
local config_utils = require("utils.config_utils")

local args = lapp [[
X527 SN Manager

This is the SN Manager, user can set SN or Get SN
Set SN Msg format : "SET,xxxxxx" xxxxx is the SN
Get SN Msg format : "GET", SN will reture
Clear SN MSG fromat : "CLEAR" 
    
    -c,--config     (default config.socket_zmq)   Config module to load.
    -u,--uut        (default 1)                     UUT slot number (used for IP/Port selection)
    <updates...>    (default "X=Y")                    Series of X=Y pairs to update the CONFIG table with
]]

local c = config_utils.get_config(args)

print("< "..tostring(c.ID).." SNManager > Set REP : "..tostring(config_utils.get_addr(c, "SNMANAGER", c.ID)))
local context = zmq.context()
zthreads.set_context(context)

local scTable = {}
local sn_zmq, err = context:socket(zmq.REP, {bind = config_utils.get_addr(c, "SNMANAGER", c.ID)})

zmq.assert(sn_zmq, err)

local _SN = ""

local function setSN(_sn)
	_SN = tostring(_sn)
	return "Set Success"
end

local function getSN()
	if _SN == "" then 
	 	return "Empty SN"
	else
		return "Get Success"
	end
end

local function clearSN()
	_SN = ""
	return "Clear Success"
end

local SNHeadBeat, pipe = zthreads.fork(context, function(pipe, address)
        local zmq = require "lzmq"
        local zthreads = require "lzmq.threads"
        local context = zthreads.get_parent_ctx()
        local  t = require "lzmq.timer"

        print(" < SN MANAGER > Set HeartBeat PUB : ", address)
        local watchdog_zmq, err = context:socket(zmq.PUB, {bind = address})
        zmq.assert(watchdog_zmq, err)

        while(1) do
            t.sleep(5000)
            watchdog_zmq:send_all{"101",os.date("%m-%d %H:%M:%S",os.time()),3,"SNManager","HEARTBEAT"}
        end
    end, config_utils.get_addr(c, "SN_HEARTBEAT", c.ID))
SNHeadBeat:start(true, false)

while(1) do
	local ret ={}
	local sn_tmp = sn_zmq:recv()
	print("< SNMANAGER receive > "..tostring(sn_tmp))
	if(sn_tmp) then
		local msg_obj, pos, err = json.decode (sn_tmp, 1, nil)
		ret["function"] = msg_obj["function"]
		if(err or msg_obj==nil) then
			ret.status="Error Json Format"
		else
			if string.match(string.upper(tostring(msg_obj["function"])), "SET" ) then
				ret.status = setSN(msg_obj.sn)
			elseif string.match(string.upper(tostring(msg_obj["function"])), "GET" ) then
				ret.status =getSN()
				ret.sn = _SN
			elseif string.match(string.upper(tostring(msg_obj["function"])), "CLEAR" ) then
				ret.status =clearSN()
			else
				ret.status = "unknow Msg"
			end
		end
		sn_zmq:send(json.encode(ret))
	end
end
