local _log = {}
local stationName = "HanTempStation4Test"--require("functions.GhInfo").GhStationName()
local time_utils = require("utils.time_utils")

local stationName = string.gsub(stationName, "%s+","_")

_log.file = {}
_log.path = {}

function _log.getKey(logname)
	return string.upper(logname).."_LOG_FILENAME"
end

function _log.getPath(logname)
	local key  = _log.getKey(logname)
	print(key)
	return _log.path[key]
end

function _log.open(logname,logtype)
	if logtype==nil then logtype="txt" end
	local p = string.format("%s/%s_%d-%s_%s.%s",CONFIG.LOG_PATH,stationName,CONFIG.ID,os.date("%m-%d-%H-%M-%S",os.time()),string.upper(logname),logtype)
	local file = io.open(p,"w")
	if(file) then
		local key = _log.getKey(logname)
		_log.file[string.upper(logname)] = file
		_log.path[key] = p
	end
end

function _log.write(logname, conten, timestamp)
	local file = _log.file[string.upper(logname)]
	if file then
		if(timestamp ~= false) then
			conten = time_utils.get_time_string_ms().."\t"..tostring(conten) .."\n"
		end
		file:write(conten)
		file:flush()
	end
end

function _log.close(logname)
	local file = _log.file[string.upper(logname)]
	if(file and file ~="file (close)") then
		file:close()
		_log.file[string.upper(logname)] = nil
	end
end

return _log
