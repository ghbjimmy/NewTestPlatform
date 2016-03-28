local config_utils = require("utils.config_utils")
local zthreads = require("lzmq.threads")
local zmq = require("lzmq")
local ztimer  = require "lzmq.timer"
local json =  require "dkjson"

require "pathManager"
package.cpath = package.cpath..";"..deleteLastPathComponent(CurrentDir()).."/lib/?.dylib"
local mid = 0;
local ip = "169.254.1.32"
local port = 7603
local logpath = "/vault/Intelli_log"

if(CONFIG) then
	mid = CONFIG.ID
	ip = CONFIG.FIXTURE_ADDRESS[CONFIG.ID + 1]
	port = CONFIG.DATALOGGER_PORT
	logpath = CONFIG.LOG_PATH
end
print("< "..tostring(mid).." > Load ArmDL lua...")
require "libArmDL"
print("< "..tostring(mid).." > Load ArmDL lua Success...")

local _armdl_ = {}
_armdl_.dl = nil
_armdl_.reqZmq = nil
_armdl_.factor = {GAIN=1,OFFSET=0}

local function connectZmq()
	print("< "..tostring(CONFIG.ID).." ARMDL > Set REQ For SN: "..tostring(config_utils.get_addr(CONFIG, "SNMANAGER", CONFIG.ID)))
	local context = zthreads.context()
	local err = nil
	_armdl_.reqZmq, err = context:socket(zmq.REQ, {connect = tostring(config_utils.get_addr(CONFIG, "SNMANAGER", CONFIG.ID))})
	_armdl_.reqZmq:set_rcvtimeo(1000)
	zassert(_armdl_.reqZmq, err)
end

local function GetSnFromManager()
	local t={}
	t["function"]="get"
	_armdl_.reqZmq:send(json.encode(t))
	ztimer.sleep(10)
	local msg = _armdl_.reqZmq:recv()
	if(msg ~= nil) then
		local msg_obj, pos, err = json.decode (msg, 1, nil)
		if(err or msg_obj==nil) then
			return nil
		else
			if string.match(string.lower(tostring(msg_obj.status)),"success") then
				return msg_obj.sn
			else
				return nil
			end
		end
	else
		return nil
	end
end

function _armdl_.connect()
	_armdl_.dl = CArmDL:new()
	-- dl:updateConfig(gain,refVolt,resistor,allData?);
	_armdl_.dl:updateConfig(3.9117647059,2.5,0.5,0);
	_armdl_.dl:CreateTCPClient("ARMDL"..tostring(mid), ip, port)
end

function _armdl_.updateFactor()
	_armdl_.dl:updateCalFactor(_armdl_.factor.GAIN, _armdl_.factor.OFFSET)
end

function _armdl_.start()--this will call in start_test (test_OnEntry())
	-- local path = logpath .."/Datalogger"
	-- local file =io.open(path)
	-- if(file == nil) then
	-- 	os.execute("mkdir "..path)
	-- else
	-- 	file:close()
	-- end
	-- local sn = GetSnFromManager()
	-- if sn == nil then sn = "NULL" end
	-- print(sn)
	-- path = path.."/"..sn..os.date("%Y-%m-%d %H-%M-%S ",os.time())..".csv"
	_armdl_.dl:startDataLogger(CONFIG.ARM_DL_FILENAME)
end

function _armdl_.stop() --this will call in end_test (test_OnDone())
	_armdl_.dl:stopDataLogger()
end

_armdl_.connect()--connect to ARM when load.
-- connectZmq()

return _armdl_

