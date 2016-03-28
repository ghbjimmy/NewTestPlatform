--- ZMQ-based connection to DUT UART
-- Depends on a running serial_master.lua
-- @module console.dut.zmq
-- @alias zmq_module
require("utils.zhelpers")
local zmq = require("lzmq")
local zthreads = require("lzmq.threads")
local time_utils = require("utils.time_utils")
local config_utils = require("utils.config_utils")

--===========================--
-- tcp exported module
--===========================--
local zmq_module = {}

--===========================--
-- module local state
--===========================--
local _dut_detect_ = "%] %:%-%)"
local _dut_response_ = ""

local context = zthreads.context()
zmq_module.DUTSocket = nil

--===========================--
-- module Private API
--===========================--
-- We need to only subscribe to messages while we need the data, otherwise we might overflow our buffer?
-- FIXME: Not sure if that is actually true in reality.

--- Enable or disable subscribing to DUT UART messages
-- @lfunction _DUT_ZMQ_Set_Listen_
-- @tparam boolean should_listen should we subscribe to ZMQ UART messages
function zmq_module._DUT_ZMQ_Set_Listen_(should_listen)
	if should_listen then
		zmq_module.uart_socket:subscribe("")
	else
		zmq_module.uart_socket:subscribe("#IGNORE#ALL#MSG#")
	end
end

--===========================--
-- module Public API
--===========================--

--- Connect to the DUT UART socket
-- @treturn nil
function zmq_module._DUT_Socket_Connect_( )
	print("< " ..tostring(CONFIG.ID).. " dut.zmq.lua > connecting to ZMQ SUBSCRIBE ",config_utils.get_addr(CONFIG, "DUT_UART_ZMQ_SVR", CONFIG.ID))
	zmq_module.uart_socket, err = context:socket(zmq.SUB, {connect = config_utils.get_addr(CONFIG, "DUT_UART_ZMQ_SVR", CONFIG.ID), 
														  subscribe = "#IGNORE#ALL#MSG#"})
	zassert(zmq_module.uart_socket, err)

	print("< " ..tostring(CONFIG.ID).. " dut.zmq.lua > connecting to ZMQ REQUEST ",config_utils.get_addr(CONFIG, "UART_ZMQ_REQ_SVR", CONFIG.ID))
	zmq_module.req_socket, err = context:socket(zmq.REQ, {connect = config_utils.get_addr(CONFIG, "UART_ZMQ_REQ_SVR", CONFIG.ID)})
	zassert(zmq_module.req_socket, err)
	print("< " ..tostring(CONFIG.ID).. " dut.zmq.lua > connect successfully")
end

--- Send a string over the DUT UART without waiting for a response
-- @param str string to send to DUT
-- @treturn nil
function zmq_module._Dut_Send_String_(str) --""
	zmq_module.req_socket:send("DUT:"..string.format("%s\r", str))
	return zmq_module.req_socket:recv()
end

--- Set the detect string.
-- The detect string is used to determine when the system is done sending back a response.
-- @param str string to use as a detect string.
-- @treturn nil
-- @see _Dut_Wait_For_String_
function zmq_module._Dut_Set_Detect_String_(str) --":-)"
	_dut_detect_ = tostring(str)
end

--- Buffer DUT UART responses until finding the detect string.
-- @param timeout timeout when waiting for detect string
-- @return 0
-- @treturn string status message
-- @see _Dut_Set_Detect_String_
function zmq_module._Dut_Wait_For_String_(timeout)--msec
	local start_time = time_utils.get_unix_time_ms()
	local loop_counter = 1
	-- Make sure we are listening
	zmq_module._DUT_ZMQ_Set_Listen_(true)

	while true do
		-- Non-blocking receive loop
		if zmq_module.uart_socket:poll(20) then
			local msg, err = zmq_module.uart_socket:recv(zmq.DONTWAIT)
			zmq.assert(msg, err)

			msg = tostring(msg)

			-- Not sure what this null detection is for, but keeping it from the old code.
			if msg ~= "\0" then
				-- Log the data here!
				-- print(ret)
	--			 io.write(ret)
				-- FIXME: What is this DUT Host thing??
				-- __dut_host:SendMsg(tostring(ret));
				_dut_response_ = _dut_response_ .. tostring(msg)
			end

			-- local _t, _v = string.find(_dut_response_ , _dut_detect_)
			if string.match(_dut_response_, _dut_detect_) then
				return 0, "Get ".._dut_detect_
			end

			-- Look for power on failures
			-- FIXME: More general hang detection?
			if string.match(_dut_response_, "Power on canceled") then
				error("gas guage timed out")
			end

			-- If we timeout, then EJECT! with an error message
			if time_utils.get_time_delta_ms(start_time) > timeout then
	            error("Timed out waiting for UART on loop "..loop_counter..". Got: ".._dut_response_:sub(0,150).."...")
			end

			-- FIXME: Do we still need this
			loop_counter = loop_counter + 1
		end
	end

	-- Stop listening to uart
	zmq_module._DUT_ZMQ_Set_Listen_(false)
end

--- Send a command to the fixture and wait for the response.
-- The detect string is set to the diags default of "] :-)", and used to determine when the
-- system is done sending back a response.
-- @param str command to sent to the DUT
-- @param timeout time to wait for detect string before failing
-- @return 0
function zmq_module._Dut_Send_Cmd_(str,timeout) --""
  zmq_module._Dut_Set_Detect_String_("%] %:%-%)")
  local s1, t1 = time_utils.get_time_string_ms()
  -- if(DutLogOut) then DutLogOut(s1 .. "< send: > " .. str) end
  if(TestFlowOut) then TestFlowOut( "\t< DUT send: > " .. str) end

  -- we need to listen to Serial data when sending stuff
  zmq_module._DUT_ZMQ_Set_Listen_(true)

  zmq_module._Dut_Send_String_(str)
  local ret = zmq_module._Dut_Wait_For_String_(timeout)

  zmq_module._DUT_ZMQ_Set_Listen_(false)

  local s2, t2 = time_utils.get_time_string_ms()
  -- if(DutLogOut) then DutLogOut("\t\t--> elapsed(sec): ".. tostring(t2-t1)) end
  if(TestFlowOut) then TestFlowOut("\t< DUT elapsed: > (sec)".. tostring(t2 - t1)) end
  return ret;
end 

--- Get the UART response from the last diags command.
-- This also erases the local UART buffer, so more calls of this function will read an empty string.
-- @treturn string DUT response from last diags command
function zmq_module._Dut_Read_String_()
  local tmp =  _dut_response_
  _dut_response_ = ""
  return tmp;
end

--- DEPRECATED: Do a raw read of the DUT Socket.
-- @treturn string DUT response from last diags command
function zmq_module._Dut_Socket_Receive_()
	-- local recv,_,_ = sc.select({zmq_module.DUTSocket},nil,1)
	-- if(recv) then
		return zmq_module.DUTSocket:receive()
	-- end
end

--- Close the DUT UART socket
-- @treturn nil
function zmq_module._Dut_Socket_Close_(  )
	zmq_module.DUTSocket:close()
	zmq_module.DUTSocket = nil;
end

print("< ".. tostring(CONFIG.ID).." > Finished Load DUT TCP Library...\r\n")

return zmq_module