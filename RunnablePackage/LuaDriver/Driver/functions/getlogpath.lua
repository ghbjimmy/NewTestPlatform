local config_utils = require("utils.config_utils")
local zmq = require("lzmq")
local json = require "dkjson"
local ztimer  = require "lzmq.timer"

local _getPath = {}
local req = nil

function _getPath.connect()
	local r = tostring(config_utils.get_addr(CONFIG, "GET_LOG_PATH_REQ", CONFIG.ID))

	print("< Get Log Path Client > Set REQ : ".. r)
	local context = zmq.context()
	req, err = context:socket{zmq.REQ, connect = r}
	req:set_rcvtimeo(1500)
	zmq.assert(req, err)
end

function _getPath.getLogPath()
	local r = {}
	local t = {}
	t["function"]="get_path"
	req:send(json.encode(t))
	local msg = req:recv()
	-- print(msg)
	if(msg) then
		local c = json.decode(msg)
		for v,k in pairs(c) do
			if v~="csv_log" then
				r[#r+1] = k
			end
		end	
	end
	return table.concat(r,",")
end

_getPath.connect()

return _getPath