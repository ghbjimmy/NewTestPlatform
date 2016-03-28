--- TCP Socket-based connection to DUT UART
-- @module console.fixture.socket
-- @alias tcp_module
print("< ".. tostring(CONFIG.ID).." fixture.socket_pub.lua > Load Fixture TCP Library...")

local dutsockettimeout = 1 --EOF LF/CR
local socket = require("utils.socket")
local time_utils = require("utils.time_utils")
local uuid = require("utils.uuid")
local config_utils = require("utils.config_utils")
local zthreads = require("lzmq.threads")
local zmq = require("lzmq")


local context = zthreads.context()
--===========================--
-- tcp exported module
--===========================--
local tcp_module = {}
tcp_module.zmqPub = nil

--===========================--
-- module local state
--===========================--
tcp_module.InstrumentSocket = nil
local HwLogOut = config_utils.get_log_writer(CONFIG, "HW", CONFIG.log_prefix)

--===========================--
-- module Public API
--===========================--

--- Connect to the fixture UART socket
-- @treturn nil
function tcp_module._Arm_Socket_Connect_()
  os.execute("ping -c 1 "..CONFIG.FIXTURE_ADDRESS[CONFIG.ID + 1]);  --ping device
  print("< ".. tostring(CONFIG.ID).."  fixture.socket_pub.lua > Fixture Port connect to: " .. CONFIG.FIXTURE_ADDRESS[CONFIG.ID + 1] .. ":" .. tostring(CONFIG.FIXTURE_PORT) .. "...")
  tcp_module.InstrumentSocket = assert(
    socket.timeout_connect(5, CONFIG.FIXTURE_ADDRESS[CONFIG.ID + 1], CONFIG.FIXTURE_PORT),"HW: "..tostring(CONFIG.ID).." Connect Fail")
  tcp_module.InstrumentSocket:settimeout(10)
  print("< ".. tostring(CONFIG.ID).."  fixture.socket_pub.lua > Fixture Port connect successfully")
  
  print("< ".. tostring(CONFIG.ID).."  fixture.socket_pub.lua > Fixture Set PUBLISH : " .. config_utils.get_addr(CONFIG, "FIX_UART_ZMQ_SVR", CONFIG.ID) .. "...")
  tcp_module.zmqPub, err = context:socket(zmq.PUB, {bind = config_utils.get_addr(CONFIG, "FIX_UART_ZMQ_SVR", CONFIG.ID)})
  zassert(tcp_module.zmqPub, err)
end

--- Send command to the fixture (ARM)
-- @param command command to send to the fixture
-- @treturn string Response from the fixture
function tcp_module._ARM_Send_Cmd_(command)
  local command = "["..string.sub(uuid(),25,36).."]" .. command 
  local t1 = time_utils.get_unix_time_ms()
  if(HwLogOut) then HwLogOut.write("<    send: > " .. tostring(command)) end
  tcp_module.zmqPub:send(command..'\0')
  command = command .. "\r\n"
  tcp_module.InstrumentSocket:send(command)
  local ret,rcvState= tcp_module.InstrumentSocket:receive()

  local t2 = time_utils.get_unix_time_ms()
  if(HwLogOut) then HwLogOut.write( "< receive: > " .. tostring(ret) .. "\r\n\t\t--> elapsed(sec): " .. tostring(t2 - t1) .. "\r\n") end
  if(rcvState == "timeout") then
    tcp_module.zmqPub:send(rcvState..'\0') 
    error("ARM Response Timeout") 
  end
  if(rcvState == "closed") then 
    tcp_module.zmqPub:send(rcvState..'\0')
    error("ARM Port connection closed.") 
  end
  tcp_module.zmqPub:send(ret..'\0')
  return ret
end 

function tcp_module._ARM_Send_String_(command)
  local command = "["..string.sub(uuid(),25,36).."]" .. command 
  local t1 = time_utils.get_unix_time_ms()
  if(HwLogOut) then HwLogOut.write("<    send: > " .. tostring(command)) end
  tcp_module.zmqPub:send(command..'\0')
  command = command .. "\r\n"
  return tcp_module.InstrumentSocket:send(command)
end 

function tcp_module._ARM_Socket_Receive_()
    local ret,rcvState = tcp_module.InstrumentSocket:receive()
    tcp_module.zmqPub:send(ret..'\0')
    return ret, rcvState  
end

--- Close the fixture UART socket
-- @treturn nil
function tcp_module._Arm_Socket_Close_()
  tcp_module.InstrumentSocket:close()
  tcp_module.InstrumentSocket = nil
end

--_Arm_Socket_Connect_();

print("< ".. tostring(CONFIG.ID).." fixture.socket_pub.lua > Finished Load Fixture  TCP Library...\r\n")

-- local function codec(params)
--   local testKey
--   local gVariant = nil
--   if(type(params) == 'table') then
--     testKey = params[1]
--     gVariant = string.match(params[2],_PDCA_KEY_MATCH_)--{{spkr_nz_floor}}, delete {{}}
--   else
--     testKey = params
--   end

--   local cmd=""
--   local strMatch = "%d+"
--   testKey = string.lower(tostring(testKey))
--   if( testKey == "fft") then

--   elseif(testKey == "amplitude") then

--   elseif(testKey == "TCXO_OUT_CLK32K") then  --TCXO_TO_PMU_AOPRT_CLK32K
          
--           InstrumentCmd("vref set(900)")
--           cmd = "frequency measure(LSM,100,0)"
--           strMatch = "ACK%s*%((.-)Hz" 
--   elseif(testKey == "TP_PT_TEST_CLKIO") then
          
--           InstrumentCmd("vref set(900)")
--           cmd = "frequency measure(LSM,100,1000)"
--           strMatch = "ACK%s*%((.-)Hz"  
--   elseif(testKey == "PT_TO_PMU_AOPRT_REQUEST_PWM") then
        
--         InstrumentCmd("vref set(900)")
--         cmd = "frequency measure(LSM,100,0)"
--         strMatch = "ACK%s*%((.-)Hz" 
--   elseif(testKey == "AP_TO_DEBUG_TST_CLKOUT") then
       
--         InstrumentCmd("vref set(900)")
--         cmd = "frequency measure(LSM,100,0)"
--         strMatch = "ACK%s*%((.-)Hz" 
--   elseif(testKey == "SPEAKER_FREQ") then
        
--         InstrumentCmd("vref set(900)")
--         cmd = "frequency measure(LSM,100,0)"
--         strMatch = "ACK%s*%((.-)Hz" 
--   elseif(testKey == "ALERT_FREQ") then
        
--         InstrumentCmd("vref set(900)")
--         cmd = "frequency measure(LSM,100,0)"
--         strMatch = "ACK%s*%((.-)Hz"   
--   elseif(testKey == "thdn") then

--   elseif(testKey == "vrms") then

--   elseif(testKey == "duty") then
--     cmd = "width measure(1000)"
--     strMatch = "duty:(.-)%%"
--   end

--   local ret = InstrumentCmd(cmd);
--   local rslt = string.match(tostring(ret),strMatch)
--     -- Variant_Table[gVariant] = rslt
--   _Add_Global_Variant(gVariant, rslt)
--   return rslt
-- end


-- local function version(params) -- params = {"ARM" or "FPGA","G"}
--     local cmd = "[321321]version(0)\r\n"
--     local rslt = ""
--     local version_str = ""
--     local t1 = os.time()
--     tcp_module.InstrumentSocket:send(cmd)
--     while(1) do
--       local str,rcvState= tcp_module.InstrumentSocket:receive()
--       -- if(rcvState == "timeout") then assert(nil,"ARM Response Timeout") end
--       -- if(rcvState == "closed") then assert(nil,"ARM Port connection closed.") end
--       if(str) then 
--         version_str = version_str .. str
--         if(string.match(str,"Author")) then
--           break
--         end
--         local t2 = os.time()
--         if(t2-t1 > 3) then 
--            timeoutFlag = 1
--           break 
--         end
--       else
--         _Add_Global_Variant(gVariant, nil)
--         return nil,"Get a nil result from FPGA."    
--       end
--     end
--     if(timeoutFlag) then 
--       _Add_Global_Variant(gVariant, nil)
--       return nil,"time out" 
--     end
--     if(version_str) then
--       local  arm_version = string.match(version_str,"MCU Software version:(.-)%$")
--       local  fpga_version = string.match(version_str,"FPGA version:(.-)%$")
--       rslt = arm_version
--       if(string.upper(tostring(params[1])) == "FPGA") then 
--          rslt = fpga_version
--       end

--       local gVariant = string.match(params[2], _PDCA_KEY_MATCH_)
--       _Add_Global_Variant(gVariant, rslt)
--       return rslt
--     else
--         return "Fixture send cmd get a nil value."
--     end
-- end
return tcp_module