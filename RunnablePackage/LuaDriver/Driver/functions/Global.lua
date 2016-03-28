globalTable = {}

local dut = require("tcpAndZmq")--CONFIG.DUT_UART_CONSOLE)

local function get_bit(data,bit)
	if(bit == 0) then
		return math.mod(data,2)
	else
		return math.mod(math.floor((data/math.pow(2,bit))),2)
	end
end


PRINT_LOG = 1
local function _Print_Log_(par,...)
	if(PRINT_LOG) then
		local t = {...}
		if(t[1]) then
			print(string.format(par,...))
		else
			print(par)
		end
	end
end

function globalTable.delay(sequence)
	local t = tonumber(sequence.param1)
	if(t) then
		os.execute("sleep ".. tostring(t/1000))
	end
	return true, true
end

function globalTable.channel(par)
	local r = CONFIG.ID
	if(r==nil) then
		r = 0
	end  
	return tostring(CONFIG.ID), true
end

function globalTable.getUnitSN(par)
	return "xxx", true
end

function globalTable.getPanelSN(par)
	return "xxx", true
end

function globalTable.getBuildEvent(par)
	return "xxx", true
end

function globalTable.getSBUILD(par)
	return "xxx", true;
end

function globalTable.GetBootTime(sequence, global_var_table)
	local str = dut._Get_All_Response_()
	local arrTime = {};
	for time in string.gmatch(str, "BG_Init:Clog Read%-Only %- timestamp : (%d+) ms") do
		arrTime[#arrTime+1] = tonumber(time);
	end
	if (#arrTime == 0) then return 0; end
	table.sort(arrTime);
	time = arrTime[#arrTime];
	return time, true
end

function globalTable.GetBootCount(sequence, global_var_table)
	local iBoot_count = 0
	local str = dut._Get_All_Response_()
	for time in string.gmatch(str, "BG_Init:Clog Read%-Only %- timestamp : (%d+) ms") do
		iBoot_count = iBoot_count + 1;
	end
	return iBoot_count
end

return globalTable
