--- TCP Socket-based connection to DUT UART
-- @module console.dut.socket
-- @alias tcp_module

local dutsockettimeout = 1 --EOF LF/CR
local socket = require("utils.socket")
local time_utils = require("utils.time_utils")
local config_utils = require("utils.config_utils")
-- CONFIG is present from global environment!!!!
local serial
if CONFIG.SERIAL_PORT == true then
	serial = require("socket.serial")
end

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

--- Connect to the DUT UART using a Kanzi
-- @lfunction _DUT_Serial_Socket_Connect_
-- @treturn nil
-- @raise error
function tcp_module._DUT_Serial_Socket_Connect_( )
	local serial_address = CONFIG.DUT_UART_ADDRESS[tonumber(CONFIG.ID) + 1]

	if serial_address == nil then
		error("Bad UUT ID")
	end

	print("< ".. tostring(CONFIG.ID).." dut.socket.lua > DUT Port connect to: " .. serial_address)
	tcp_module.DUTSocket = assert(serial(serial_address))
	tcp_module.DUTSocket:settimeout(dutsockettimeout)
	print("< ".. tostring(CONFIG.ID).." dut.socket.lua > DUT Port connect successfully")
end

--- Connect to the DUT UART socket
-- @treturn nil
function tcp_module._DUT_Socket_Connect_( )
	if CONFIG.SERIAL_PORT == true then
		return tcp_module._DUT_Serial_Socket_Connect_()
	end
	print("< ".. tostring(CONFIG.ID).." dut.socket.lua > DUT Port connect to: " .. CONFIG.DUT_UART_ADDRESS[CONFIG.ID + 1] .. ":" .. tostring(CONFIG.DUT_UART_PORT) .. "...")
	tcp_module.DUTSocket = assert(
		socket.timeout_connect(5,CONFIG.DUT_UART_ADDRESS[CONFIG.ID + 1], CONFIG.DUT_UART_PORT),"HW: "..tostring(CONFIG.ID).." Connect Fail")
	tcp_module.DUTSocket:settimeout(dutsockettimeout)
	tcp_module.DUTSocket:setoption("keepalive",true)
	print("< ".. tostring(CONFIG.ID).." dut.socket.lua > DUT Port connect successfully")
end

--- Send a string over the DUT UART without waiting for a response
-- @param str string to send to DUT
-- @treturn nil
function tcp_module._Dut_Send_String_(str) --""
	_dut_sent_command_ = true
	return tcp_module.DUTSocket:send(string.format("%s\r", str))
end

--- Set the detect string.
-- The detect string is used to determine when the system is done sending back a response.
-- @param str string to use as a detect string.
-- @treturn nil
-- @see _Dut_Wait_For_String_
function tcp_module._Dut_Set_Detect_String_(str) --":-)"
	_dut_detect_ = tostring(str)
end

--- Buffer DUT UART responses until finding the detect string.
-- @param timeout timeout when waiting for detect string
-- @return 0
-- @treturn string status message
-- @see _Dut_Set_Detect_String_
function tcp_module._Dut_Wait_For_String_(timeout)--msec
	local start_time = time_utils.get_unix_time_ms()
	local loop_counter = 1

	while true do
		local ret, rcvState = tcp_module.DUTSocket:receive(1)

		if(rcvState == "closed") then
			print("connection closed")
			if(DutLogOut) then DutLogOut.write(_dut_response_) end
			error("DUT port Connectiond Closed.")
		end

		if(ret and ret ~= "\0") then
			-- Log the data here!
			-- print(ret)
			io.write(ret)
			-- FIXME: What is this DUT Host??
			-- __dut_host:SendMsg(tostring(ret));
			_dut_response_ = _dut_response_ .. tostring(ret)
		end

		-- local _t, _v = string.find(_dut_response_ , _dut_detect_)
		if(string.match(_dut_response_, _dut_detect_)) then
			if(DutLogOut) then DutLogOut.write(_dut_response_, nil, _dut_sent_command_) end
			_dut_sent_command_ = false
			
			return 0, "Get ".._dut_detect_
		end

		if(string.match(_dut_response_, "Power on canceled")) then
			error("gas guage timed out, will terminate testing")
		end

		if time_utils.get_time_delta_ms(start_time) > timeout then
			if(DutLogOut) then  DutLogOut.write( _dut_response_, nil, _dut_sent_command_) end
			_dut_sent_command_ = false

			-- Return error plus first 100 characters of response.
            error("Timed out waiting for UART on loop "..loop_counter..". Got: ".._dut_response_:sub(1,100))
		end

		loop_counter = loop_counter + 1
	end	
end

--- Send a command to the fixture and wait for the response.
-- The detect string is set to the diags default of "] :-)", and used to determine when the
-- system is done sending back a response.
-- @param str command to sent to the DUT
-- @param timeout time to wait for detect string before failing
-- @return 0
function tcp_module._Dut_Send_Cmd_(str,timeout) --""
  tcp_module._Dut_Set_Detect_String_("%] %:%-%)")
  local s1, t1 = time_utils.get_time_string_ms()
  -- if(DutLogOut) then DutLogOut(s1 .. "< send: > " .. str) end
  if(TestFlowOut) then TestFlowOut.write( "\t< DUT send: > " .. str) end
  tcp_module._Dut_Send_String_(str)
  ret = tcp_module._Dut_Wait_For_String_(timeout)

  local s2, t2 = time_utils.get_time_string_ms()
  -- if(DutLogOut) then DutLogOut("\t\t--> elapsed(sec): ".. tostring(t2-t1)) end
  if(TestFlowOut) then TestFlowOut.write("\t< DUT elapsed: > (sec)".. tostring((t2 - t1)/1000)) end
  return ret;
end 

--- Get the UART response from the last diags command.
-- This also erases the local UART buffer, so more calls of this function will read an empty string.
-- @treturn string DUT response from last diags command
function tcp_module._Dut_Read_String_()
  local tmp =  _dut_response_
  _dut_response_ = ""
  return tmp;
end

--- DEPRECATED: Do a raw read of the DUT Socket.
-- @treturn string DUT response from last diags command
function tcp_module._Dut_Socket_Receive_()
	-- local recv,_,_ = sc.select({tcp_module.DUTSocket},nil,1)
	-- if(recv) then
		return tcp_module.DUTSocket:receive()
	-- end
end

--- Close the DUT UART socket
-- @treturn nil
function tcp_module._Dut_Socket_Close_(  )
	tcp_module.DUTSocket:close()
	tcp_module.DUTSocket = nil;
end

-- print(_ARM_Send_Cmd_("io set(1,bit140=0)"))
-- _Arm_Close_()
-- _Dut_Close_()
--_DUT_Socket_Connect_()

print("< ".. tostring(CONFIG.ID).." dut.socket.lua> Finished Load DUT TCP Library...\r\n")

return tcp_module