package.path = package.path..";"..tostring("xxxxxxxxxxx").."/Driver/?.lua"

local functions = require "function_table"

local uuid = require "uuid"
local socket = require "socket"
local port = 7600
local connection = nil

local global_var_table = {}

--[[
{"id":1680,"description":"Delay 2000ms","function":"delay","param1":"2000","param2":"","low":null,"high":null,"unit":null,"globalKey":"","globalValue":"","timeout":0,"tid":"CV_SETUP_145_DELA","test_block_id":1775,"created_at":"2015-09-18T09:19:18.055Z","updated_at":"2015-09-18T09:19:18.055Z"}
--]]


local driver_commands = 
{
	{name="supply", param={param1="VDD_MAIN", param2=4.3,unit="V"}},
	{name="disconnect", param={param1="BUTTON1_L", param2="",unit=""}},
	{name="relay", param={param1="BUTTON1_L", param2="",unit=""}},
	--{name="button", param={param1="", param2="",unit=""}},
	--{name="ready", param={param1="", param2="",unit=""}},
	{name="diags", param={param1="sensor --sel accel --init", param2=":-)",unit=""}},
	{name="measure", param={param1="BUTTON1_L", param2="VOLT",unit="mV"}},
	{name="delay", param={param1=1000, param2="",unit=""}},
	{name="parse", param={param1="access_10KHZ", param2="{VAL_ACCESS_100",unit=""}},
	{name="eload", param={param1=100, param2="E_LOAD_PP8V0_DISP",unit=""}},
	{name="seajay", param={param1="", param2="",unit=""}},
	{name="iload", param={param1="ILOAD_100", param2="",unit="mA"}},
	{name="reference", param={param1="PRESSURE", param2="",unit="mPa"}},
	{name="codec", param={param1="TCXO_OUT_CLK32K", param2="TCXO_32K",unit=""}},
	{name="reset", param={param1="", param2="",unit=""}},
	{name="datalogger", param={param1=300, param2="BATT_CURR",unit="mA"}},
	{name="dataloggerstart", param={param1="", param2="",unit=""}},
	{name="dataloggerend", param={param1="", param2="BATT_CURR",unit=""}},
	{name="dmm", param={param1="VDD_MAIN", param2="VDD_MAIN_CURR",unit="mA"}},
	{name="smokey", param={param1="xxxx", param2="",unit=""}},
	{name="detect", param={param1=":-)", param2="",unit=""}},
	{name="calculate", param={param1="", param2="",unit=""}},
	{name="tbat", param={param1=30000, param2="",unit=""}},
	{name="channel", param={param1="", param2="channel",unit=""}},
	{name="getUnitSN", param={param1="", param2="SN",unit=""}},
	{name="getBuildEvent", param={param1="", param2="EVENT",unit=""}},
	{name="getPanelSN", param={param1="", param2="PANELSN",unit=""}},
	{name="getSBUILD", param={param1="", param2="BUILD",unit=""}},
	{name="station", param={param1="", param2="STATION",unit=""}},
	{name="GetBootTime", param={param1="", param2="BOOTTIME",unit=""}},
	{name="GetBootCount", param={param1="", param2="BOOTCOUNT",unit=""}},
	{name="version", param={param1="ARM", param2="ARMVERSION",unit=""}},
	--{name="thdn", param={param1="", param2="",unit=""}},
}

local thread_code = 
[=====[
main(...)
]=====]

local function init_client(__port)
	local host = "127.0.0.1"
	local port = 7601
	if __port then
		port = __port
	end
	local connection = assert(socket.connect(host, port))
	connection:settimeout(0)
	return connection
end

local function do_send(msg)
	if msg then
		assert(connection:send(msg.."\r\n"))
	end
end

local function do_recv(__timeout)
	local timeout = 1
	if __timeout then
		timeout = __timeout
	end
	local recvt, sendt, status = socket.select({connection}, nil, timeout)
	if #recvt > 0 then
		local response, receive_status = connection:receive()
		if receive_status ~= "closed" then
			if response then
				print(response)
				return response
			end
		end
	end
	return nil
end

local function close_client()
	connection:close()
end

local function gen_uuid()
	return string.sub(string.gsub(uuid(), "-", ""),17,32)
end

local function exec_fn(name, param)
	if name=="supply" then
		functions.name(param)
		
	elseif name=="disconnect" then
		functions.name(param)
		
	elseif name=="relay" then
		functions.name(param)
	
	elseif name=="button" then
		functions.name(param)
	
	--elseif name=="ready" then
	elseif name=="diags" then
		--functions.name(param)
		do_send(param.param1)
		local ret = do_recv(0.1)
		print(ret)
		
	elseif name=="measure" then
		functions.name(param, global_var_table)
		
	elseif name=="delay" then
		functions.name(param)
		
	elseif name=="parse" then
		functions.name(param)
		
	elseif name=="eload" then
		functions.name(param)
		
	elseif name=="seajay" then
		functions.name(param)
		
	elseif name=="iload" then
		functions.name(param)
		
	elseif name=="reference" then
		functions.name(param)
		
	elseif name=="codec" then
		functions.name(param)
		
	elseif name=="reset" then
		functions.name(param,global_var_table)
		
	elseif name=="datalogger" then
		functions.name(param)
		
	elseif name=="dataloggerstart" then
		functions.name(param)
		
	elseif name=="dataloggerend" then
		functions.name(param)
		
	elseif name=="dmm" then
		functions.name(param)
		
	elseif name=="smokey" then
		functions.name(param)
		
	elseif name=="detect" then
		functions.name()
		
	elseif name=="calculate" then
		functions.name(param)
		
	elseif name=="tbat" then
		functions.name(param)
		
	elseif name=="channel" then
		functions.name(param)
		
	elseif name=="getUnitSN" then
		functions.name(param)
		
	elseif name=="getBuildEvent" then
		functions.name(param)
		
	elseif name=="getPanelSN" then
		functions.name(param)
		
	elseif name=="getSBUILD" then
		functions.name(param)
		
	elseif name=="station" then
		functions.name(param)
		
	elseif name=="GetBootTime" then
		functions.name(param)
		
	elseif name=="GetBootCount" then
		functions.name(param)
		
	elseif name=="version" then
		functions.name(param)
		
	--elseif name=="thdn" then
	end
end

local function driver_test()
	print("starting driver test...")
		
	for i=1, #driver_commands do
		exec_fn(driver_commands[i].name, driver_commands[i].param)
	end
	
	print("driver test finished.")
end

local function main()
	connection = init_client(port)
	
	driver_test()
	
	close_client()

end

main()