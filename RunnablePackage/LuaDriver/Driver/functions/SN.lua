-- local config_utils = require("utils.config_utils")
-- local zmq = require("lzmq")
-- local json = require "dkjson"
-- local ztimer  = require "lzmq.timer"

local sncenter = {}
local req = nil

sncenter.carrierSN = nil
sncenter.mlbSN = nil

function sncenter.connect()
	-- local r = tostring(config_utils.get_addr(CONFIG, "SNMANAGER", CONFIG.ID))

	-- print("< SN Client > Set REQ : ".. r)
	-- local context = zmq.context()
	-- req, err = context:socket{zmq.REQ, connect = r}
	-- req:set_rcvtimeo(1000)
	-- zmq.assert(req, err)
end

function sncenter.setSN(sequence)
	-- sncenter.carrierSN = nil
	-- local s = "SerialNumber"
	-- if(sequence.param1) then s = sequence.param1 end
	-- local t = {}
	-- t["function"]="SET"
	-- t["sn"]=s
	-- req:send(json.encode(t))
	-- local msg = req:recv()
	-- print(smg)
	-- if(msg) then
	-- 	local msg_obj = json.decode (msg)
	-- 	if(string.match(string.upper(tostring(msg_obj["status"])),"SUCCESS")) then return true end
	-- end
	-- return false
end

function sncenter.getSN()
	-- sncenter.carrierSN = nil
	-- local t = {}
	-- t["function"]="GET"
	-- req:send(json.encode(t))
	-- local msg = req:recv()
	-- print(smg)
	-- if(msg) then
	-- 	local msg_obj = json.decode (msg)
	-- 	sncenter.carrierSN = msg_obj["sn"]
	-- end

	-- if(sncenter.carrierSN) then 
	-- 	return sncenter.carrierSN
	-- else 
	-- 	return false
	-- end
	return sncenter.carrierSN
end

function sncenter.compareSN(sequence)
	if(sncenter.mlbSN == sequence.param1) then
	  return sncenter.mlbSN
	else
	  if(sncenter.mlbSN ~= "") then
	  	return "--FAIL--"..sncenter.mlbSN
	  else
	  	return "--FAIL--"..sequence.param1
	  end
	end
end

sncenter.connect()

return sncenter