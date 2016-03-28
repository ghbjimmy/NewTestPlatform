-- This is for GH info
-- This should cor-work with libGhInfo.dylib
local ghInfoTable = {}

require "pathManager"
package.cpath = package.cpath..";"..deleteLastPathComponent(CurrentDir()).."/lib/?.dylib"

require "libGhInfo"
local global_data = require("utils.global_data") -- global data mangement library

local function getGhInfo(par)
	if(par) then
		return GetInfo(par)
	else
		return nil
	end
end

local _gh_info_table_ = {}
local _gh_info_name_ = {
						"StationName",--"X527 COMBINE-FCT"
						-- "StationID",
						-- "LineNumber",
						-- "StationNumber",
						-- "LineName",
						"StationType",--"COMBINE-FCT"
						-- "PDCA_Server",
						-- "SFC_server",
						-- "SFC_URL",
						-- "CBAuthStationNameToCheck",
						-- "CBAuthNumberToCheck",
						-- "CBAuthMaxFailForStation",
						-- "CBAuthToClearOnPass",
						-- "CBAuthToClearOnFail",
						-- "CBAuthStationSetControlBit"
					}
local _name__ = nil
Initial()
for i=1, #_gh_info_name_ do
	_name__ = _gh_info_name_[i]
	_gh_info_table_[_name__] = getGhInfo(_name__)
	-- print(_name__, _gh_info_table_[_name__])
end
-- Dealloc()
function ghInfoTable.GhStation(sequence, global_var_table)
	local ret = _gh_info_table_["StationType"]
	-- global_data.set_from_param(global_var_table, sequence.param2, ret)
	return ret,true;
end

function ghInfoTable.GhStationName(sequence, global_var_table)
	local ret = _gh_info_table_["StationName"] --"X527 COMBINE-FCT"
	-- global_data.set_from_param(global_var_table, sequence.param2, ret)
	return ret,true
end

function ghInfoTable.station(sequence, global_var_table)
	local ret = nil
 	local tmp = _gh_info_table_["StationName"]
    if(tmp == "X527 PREFCT" or tmp == "X527 PREFCT2" or tmp == "X527 FCT") then
        ret = "PRESMT2"
    else
-- if(tmp == nil or tmp=="SMT-QT0" or tmp == "COMBINE-FCT") then
        ret = "POSTSMT2"
    end
    -- global_data.set_from_param(global_var_table, sequence.param2, ret)
	return ret,true;
end

function ghInfoTable.fixturetype(sequence, global_var_table)
	local ret = nil
 	local tmp = _gh_info_table_["StationType"]
    if string.match(tmp, "PREFCT") then --or PREFCT2
        ret = "PANELFIXTURE"
    else
        ret = "SIPFIXTURE"
    end
	return ret,true;
end

function ghInfoTable.fixturename(sequence, global_var_table)
 	local tmp = _gh_info_table_["StationName"]
 	local production = string.match(tmp,"(.-)%s")
 	local ret = ghInfoTable.fixturetype()
 	if production then
 		ret = production.."_"..ret--X827_SIPFIXTURE
 	end
 	return ret
end

-- local p = ghInfoTable.fixturename() 
local tmp = _gh_info_table_["StationName"]
local p = string.match(tostring(tmp),"(.-)%s")
if p and CONFIG then
	local path = CONFIG.TEMPLATE_PATH..string.upper(p).."/"
	local file = io.open(CurrentDir().."/"..path)
	--if product folder is exist, change the path to match/product/, e.g. match/x527
	-- or will mot chagne , will be match/
	if(file) then
		CONFIG.TEMPLATE_PATH = path 
		file:close()
	end
	print("\ntemplate path : ",CONFIG.TEMPLATE_PATH)
end

return ghInfoTable