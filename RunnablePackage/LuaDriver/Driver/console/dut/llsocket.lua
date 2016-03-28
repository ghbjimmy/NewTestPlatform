--- TCP Socket-based connection to DUT UART
-- @module console.dut.socket
-- @alias tcp_module

local dutsockettimeout = 1 --EOF LF/CR
local socket = require("utils.socket")
local time_utils = require("utils.time_utils")
local config_utils = require("utils.config_utils")
-- CONFIG is present from global environment!!!!

--===========================--
-- tcp exported module
--===========================--
local tcp_module = {}

--===========================--
-- module local state
--===========================--
local _dut_detect_ = "%] %:%-%)"
local _dut_response_ = ""
local _dut_sent_command_ = false
tcp_module.DUTSocket =  nil
local DutLogOut = config_utils.get_log_writer(CONFIG, "DUT", CONFIG.log_prefix)
local TestFlowOut = config_utils.get_log_writer(CONFIG, "FLOW", CONFIG.log_prefix)

--===========================--
-- module Public API
--===========================--
--- Connect to the DUT UART socket
-- @treturn nil
function tcp_module.Open( )
	if CONFIG.SERIAL_PORT == true then
		return tcp_module._DUT_Serial_Socket_Connect_()
	end
	print("connect to: " .. CONFIG.DUT_UART_ADDRESS[CONFIG.ID + 1] .. ":" .. tostring(CONFIG.DUT_UART_PORT) .. "...")
	tcp_module.DUTSocket = assert(
		socket.timeout_connect(5,CONFIG.DUT_UART_ADDRESS[CONFIG.ID + 1], CONFIG.DUT_UART_PORT),"HW: "..tostring(CONFIG.ID).." Connect Fail")
	tcp_module.DUTSocket:settimeout(dutsockettimeout)
	tcp_module.DUTSocket:setoption("keepalive",true)
	print("connect successfully")
end

--- Send a string over the DUT UART without waiting for a response
-- @param str string to send to DUT
-- @treturn nil
function tcp_module.WriteString(str) --""
	_dut_sent_command_ = true
	return tcp_module.DUTSocket:send(string.format("%s\r", str))
end


--- DEPRECATED: Do a raw read of the DUT Socket.
-- @treturn string DUT response from last diags command
function tcp_module.ReadString()
	return tcp_module.DUTSocket:receive()
end

--- Close the DUT UART socket
-- @treturn nil
function tcp_module.Close(  )
	tcp_module.DUTSocket:close()
	tcp_module.DUTSocket = nil;
end

print("< ".. tostring(CONFIG.ID).." > Finished Load Low level socket io Library...\r\n")

return tcp_module