local json  = require "dkjson"
local str = [[
{
	"tid": "tid_1",
	"description": "description_1",
	"fucntion": "function_1",
	"param1": "param_1",
	"param2": "param_2",
	"low": "low_1",
	"high": "high_1",
	"unit": "unit_1",
	"globalKey": "globalKey_1",
	"globalValue": "globalValue_1"
	"timeout": "timeout_1"
}
]]

local obj, pos,err = json.decode(str,1, nil)
if(err) then 
	print("Error: ",err)
else
	print("tid",obj.tid)
	print(tostring(obj)..";"..tostring(pos)..";"..tostring(err))
end