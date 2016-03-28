local _IA_Socket_ = {}

local config_utils = require("utils.config_utils")
local zthreads = require("lzmq.threads")
local zmq = require("lzmq")
local ztimer  = require "lzmq.timer"
local json =  require "dkjson"


print("< "..tostring(CONFIG.ID).." > Load IASocketLua lua...\r\n")
require "pathManager"
package.cpath = package.cpath..";"..deleteLastPathComponent(CurrentDir()).."/lib/?.dylib"

require "libIASocketLua"
print("\r\n< "..tostring(CONFIG.ID).." > Load IASocketLua lua Success...\r\n")

local InstrumentSocket =  require(CONFIG.FIXTURE_CONSOLE)
_IA_Socket_.reqZmq = nil

--part1 basic fucntion, part2, operation function

--part1
--basic function, get and set
function _IA_Socket_.GetVersion() -- get version of lib
	return IASocket_Obj:GetVersion();
end

function _IA_Socket_.InitSocket(par) -- inittial socket, par = "00,01,02,..."
	return IASocket_Obj:InitSocket(par);
end

--before run the following step,  need to InitSocket() first
function _IA_Socket_.GetSocketSN() -- get socket SN, 
	return IASocket_Obj:GetSocketSN()
end

function _IA_Socket_.GetUUTSN() -- get socket SN, 
	return IASocket_Obj:GetUUTSN()
end

function _IA_Socket_.GetElastomerSN() --get elastomer SN, 
	return IASocket_Obj:GetElastomerSN()
end


function _IA_Socket_.GetInterposerSN() --get Interposer SN
	return IASocket_Obj:GetInterposerSN()
end


function _IA_Socket_.GetElastomerCount() -- get Elastomer Count
	return IASocket_Obj:GetElastomerCount()
end


function _IA_Socket_.GetInterposerCount() --get Interposer Count
	return IASocket_Obj:GetInterposerCount()
end

function _IA_Socket_.GetSocketPressCount() --get 
	return IASocket_Obj:GetSocketPressCount()
end

function _IA_Socket_.GetSocketReleaseCount() --get 
	return IASocket_Obj:GetSocketReleaseCount()
end

function _IA_Socket_.GetPCIEPlugCount()
	return IASocket_Obj:GetPCIEPlugCount()
end

function _IA_Socket_.GetStationResult(par)
	return IASocket_Obj:GetStationResult(tonumber(par))
end

function _IA_Socket_.GetError(par)
	return IASocket_Obj:GetError(tonumber(par))
end


--get info from handle, and then write the info to eeprom
function _IA_Socket_.GetInfo()--return: ",00,01,02..."
	return IASocket_Obj:GetInfo()
end

--following is set, can modify the info of socket handle
function _IA_Socket_.IncreaseElastmoerCount()
	return IASocket_Obj:IncreaseElastmoerCount()
end

function _IA_Socket_.IncreaseSocketPressCount()
	return IASocket_Obj:IncreaseSocketPressCount()
end

function _IA_Socket_.IncreaseSocketReleaseCount()
	return IASocket_Obj:IncreaseSocketReleaseCount()
end

function _IA_Socket_.IncreaseSocketPCIEPlugCount()
	return IASocket_Obj:IncreaseSocketPCIEPlugCount()
end

function _IA_Socket_.SetStationResult(stationId, result)
	return IASocket_Obj:SetStationResult(stationId,result)
end

function _IA_Socket_.SetUUTSN(par)
	return IASocket_Obj:SetUUTSN(tostring(par))
end

function _IA_Socket_.SetElastomerSN(par)
	return IASocket_Obj:SetElastomerSN(tostring(par))
end

function _IA_Socket_.SetSocketSN(par)
	return IASocket_Obj:SetSocketSN(tostring(par))
end

function _IA_Socket_.SetInterPoserSN(par)
	return IASocket_Obj:SetInterPoserSN(tostring(par))
end

function _IA_Socket_.FormatHandle()
	return IASocket_Obj:selfFormat()
end

--ZMQ Part
local function connectZmq()
	print("< "..tostring(CONFIG.ID).." IASocketLua > Set REQ For SN: "..tostring(config_utils.get_addr(CONFIG, "SNMANAGER", CONFIG.ID)))
	local context = zthreads.context()
	local err = nil
	_IA_Socket_.reqZmq, err = context:socket(zmq.REQ, {connect = tostring(config_utils.get_addr(CONFIG, "SNMANAGER", CONFIG.ID))})
	_IA_Socket_.reqZmq:set_rcvtimeo(1000)
	zassert(_IA_Socket_.reqZmq, err)
end

local function SetSnToManager(sn)
	local t={}
	t["function"]="set"
	t["sn"] = tostring(sn)
	_IA_Socket_.reqZmq:send(json.encode(t))
	ztimer.sleep(10)
	local msg = _IA_Socket_.reqZmq:recv()
end

--part2
--operation fucntion
function _IA_Socket_.ReadSnFromCarrier()
	local len = 1024
	local loop = math.ceil(len/100)
	local info = ""
	local tmpData = ""
	local starAdd = 100
	local lenRead = 100

	for i=1,loop do
		starAdd = (i-1)*100
		if(i==loop) then lenRead = len-100*(i-1) end
		local cmd = string.format("eeprom read(socket,cat08,%02X,%d)",starAdd, lenRead)
  		InstrumentSocket._ARM_Send_String_(cmd)
  		local ret = InstrumentSocket._ARM_Socket_Receive_();
		-- local ret = InstrumentCmd(string.format("eeprom read(socket,cat08,%02X,%d)",starAdd, lenRead))
		if(ret) then
			tmpData = string.match(ret, "%((.-);")
			if(string.match(tmpData, "ERR")) then
				return nil end
			if(tmpData) then
				info = info ..",".. tmpData
			end
		else
			return -1, "read carrier fail"
		end
	end
	if(info) then info = string.sub(info,2,#info) end
	-- print("read:\r\n"..info)
	_IA_Socket_.InitSocket(info);
	print("Socket:" .. tostring(_IA_Socket_.GetSocketSN()).." UUT:"..tostring( _IA_Socket_.GetUUTSN()).." Ela:"..tostring(_IA_Socket_.GetElastomerSN()).." Int:"..tostring(_IA_Socket_.GetInterposerSN()))
	-- print("ElaCt:" .. tostring(_IA_Socket_.GetElastomerCount()).. " InC:"..tostring(_IA_Socket_.GetInterposerCount()).." SRC:"..tostring(_IA_Socket_.GetSocketReleaseCount()).." SPC:"..tostring(_IA_Socket_.GetSocketPressCount()))
	-- print("M:"..tostring(_IA_Socket_.GetInfo()))
	local usn = _IA_Socket_.GetUUTSN()
	local csn = _IA_Socket_.GetSocketSN()
	SetSnToManager(usn)
	return usn,csn;
end

--before execute this function need to set your action, as _IA_Socket_.SetUUTSN("12324...")
function _IA_Socket_.WriteToCarrier()
	local total = 1024
	local str = _IA_Socket_.GetInfo()..","
	print("Write:\r\n"..str)
	local tbl = {}
	for v in string.gmatch(str, "(.-),") do
		table.insert(tbl, v)
	end
	local loop = math.ceil(#tbl/100)
	local starIndex = 1
	local endIndex = 100
	local wLen = 100
	local str = ""
	for i=1, loop do
		starIndex = 1+(i-1)*100
		if(i==loop) then 
			wLen = #tbl - 100 * (loop-1) 
			endIndex = total
		else
			endIndex = i*100;
		end
		str =  string.format("eeprom write(socket,cat08,%x,%d,",starIndex-1, wLen)
		str = str .. table.concat(tbl, ",", starIndex, endIndex) .. ")"
		-- print(str)
		local ret = InstrumentSocket._ARM_Send_Cmd_(str);
		local t = string.match(ret, "DONE")
		if(t == nil) then return -1,"write carrier fail" end
	end
	return 0
end
connectZmq();
return _IA_Socket_;
