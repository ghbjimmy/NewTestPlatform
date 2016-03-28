--- Dummy socket library simulates a fake fixture and DUT UART connection.
-- @module console.dummy
-- @alias dummy_module
print("< ".. tostring(CONFIG.ID).." console.dummy.lua > Load Dummy Socket Library...")
local time_utils = require("utils.time_utils")
local config_utils = require("utils.config_utils")
local uuid = require("utils.uuid")
-- CONFIG is present from global environment!!!!

--===========================--
-- dummy exported module
--===========================--
local dummy_module = {}

--===========================--
-- module local state
--===========================--
local InstrumentSocket = nil
local DutLogOut = config_utils.get_log_writer(CONFIG, "DUT", CONFIG.log_prefix)
local HwLogOut = config_utils.get_log_writer(CONFIG, "HW", CONFIG.log_prefix)
local TestFlowOut = config_utils.get_log_writer(CONFIG, "FLOW", CONFIG.log_prefix)

--===========================--
-- fake fixture module Public API
--===========================--

--- Connect to the fixture UART socket
-- @treturn nil
function dummy_module._Arm_Socket_Connect_()
  print("Dummy connection.")
end

--- Send command to the fixture (ARM)
-- @param command command to send to the fixture
-- @treturn string Response from the fixture
function dummy_module._ARM_Send_Cmd_(command)
  -- local cmd = string.format("[%d8888888]%s\r\n",CONFIG.ID,str)
  local command = "["..string.sub(uuid(),25,36).."]" .. command 
  local t1 = time_utils.get_unix_time_ms()
  -- FIX the logging functions here
  if(HwLogOut) then HwLogOut.write("<    send: > " .. tostring(command)) end
  time_utils.delay(1)

  local t2 = time_utils.get_unix_time_ms()
  if(HwLogOut) then HwLogOut.write("< receive: > " .. tostring("DONE") .. "\r\n\t\t--> elapsed(sec): " .. tostring(t2 - t1) .. "\r\n") end

  return "DONE"
end 

--- Close the fixture UART socket
-- @treturn nil
function dummy_module._Arm_Socket_Close_()
  print("Dummy Closed")
end

--===========================--
-- fake dut module Public API
--===========================--

--- Connect to the DUT UART socket
-- @treturn nil
function dummy_module._DUT_Socket_Connect_()
  return dummy_module._Arm_Socket_Connect_()
end

--- Send a string over the DUT UART without waiting for a response
-- @param str string to send to DUT
-- @treturn nil
function dummy_module._Dut_Send_String_(str) 
  DutLogOut.write("Dummy sent string "..str)
end

--- Set the detect string.
-- The detect string is used to determine when the system is done sending back a response.
-- @param str string to use as a detect string.
-- @treturn nil
-- @see _Dut_Wait_For_String_
function dummy_module._Dut_Set_Detect_String_(str)
  -- print("Dummy set detect string to "..str)
end

--- Buffer DUT UART responses until finding the detect string.
-- @param timeout timeout when waiting for detect string
-- @return 0
-- @treturn string status message
-- @see _Dut_Set_Detect_String_
function dummy_module._Dut_Wait_For_String_(timeout)
  time_utils.delay(100)
  -- print("Dummy wait for string.")
  return 0, "Get dummy detect."
end

--- Send a command to the fixture and wait for the response.
-- The detect string is set to the diags default of "] :-)", and used to determine when the
-- system is done sending back a response.
-- @param str command to sent to the DUT
-- @param timeout time to wait for detect string before failing
-- @return 0
function dummy_module._Dut_Send_Cmd_(str, timeout)
  dummy_module._Dut_Set_Detect_String_("%] %:%-%)")
  local s1, t1 = time_utils.get_time_string_ms()

  if(TestFlowOut) then TestFlowOut.write( "\t< DUT send: > " .. str) end
  dummy_module._Dut_Send_String_(str)
  ret = dummy_module._Dut_Wait_For_String_(_timeout)

  local s2, t2 = time_utils.get_time_string_ms()

  if(TestFlowOut) then TestFlowOut.write("\t< DUT elapsed: > (sec)".. tostring(t2 - t1)) end
  return ret
end 

--- Get the UART response from the last diags command.
-- This also erases the local UART buffer, so more calls of this function will read an empty string.
-- @treturn string DUT response from last diags command
function dummy_module._Dut_Read_String_()
  return "Dummy Response";
end

--- DEPRECATED: Do a raw read of the DUT Socket.
-- @treturn string DUT response from last diags command
function dummy_module._Dut_Socket_Receive_()
  -- local recv,_,_ = sc.select({DUTSocket},nil,1)
  -- if(recv) then
  return "DONE!"
  -- end
end

--- Close the DUT UART socket
-- @treturn nil
function dummy_module._Dut_Socket_Close_(  )
  print("Dummy Closed")
end

print("< ".. tostring(CONFIG.ID).." console.dummy.lua > Finished Load Dummy Socket Library...\r\n")

return dummy_module