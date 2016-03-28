
print("< ".. tostring(CONFIG.ID).." fixture.spam.lua > Load Fixture TCP Library...")

local socket = require("utils.socket")
local time_utils = require("utils.time_utils")
local uuid = require("utils.uuid")
local config_utils = require("utils.config_utils")
local zthreads = require("lzmq.threads")
local TestEngineLog = require("utils.LogFile")
local zmqPub = require("console.fixture.socket_pub").zmqPub

--===========================--
-- tcp exported module
--===========================--
local spam_socket = {}

--===========================--
-- module local state
--===========================--
spam_socket.InstrumentSocket = nil

--===========================--
-- module Public API
--===========================--

--- Connect to the fixture UART socket
-- @treturn nil
function spam_socket._Arm_Socket_Connect_()
  TestEngineLog.write(EngineLogName,"< ".. tostring(CONFIG.ID).."  fixture.spam.lua > Fixture Port connect to: " .. CONFIG.FIXTURE_ADDRESS[CONFIG.ID + 1] .. ":" .. tostring(CONFIG.SPAM_PORT) .. "...", true)
  print("< ".. tostring(CONFIG.ID).."  fixture.spam.lua > Fixture Port connect to: " .. CONFIG.FIXTURE_ADDRESS[CONFIG.ID + 1] .. ":" .. tostring(CONFIG.SPAM_PORT) .. "...")
  spam_socket.InstrumentSocket = assert(
    socket.timeout_connect(5, CONFIG.FIXTURE_ADDRESS[CONFIG.ID + 1], CONFIG.SPAM_PORT),"HW: "..tostring(CONFIG.ID).." Connect Fail")
  TestEngineLog.write(EngineLogName, "HW: "..tostring(CONFIG.ID).." Connect Fail", true)
  spam_socket.InstrumentSocket:settimeout(10)
  TestEngineLog.write(EngineLogName, "  fixture.spam.lua > Fixture Port connect successfully", true)
  print("< ".. tostring(CONFIG.ID).."  fixture.spam.lua > Fixture Port connect successfully")
end

--- Send command to the fixture (ARM)
-- @param command command to send to the fixture
-- @treturn string Response from the fixture

function spam_socket.Clr_spam_buff()
  local recvt, sendt, status = socket.select({spam_socket.InstrumentSocket},nil,1)
  while #recvt>0 do
    local ret,rcvState= spam_socket.InstrumentSocket:receive()
    if rcvState ~= "closed" then
      if ret then
        recvt, sendt, status = socket.select({spam_socket.InstrumentSocket},nil,1)
      end
    else
      break
    end
  end
end

function spam_socket._ARM_Send_Cmd_(command)
  local command = tostring(command)
  local t1 = time_utils.get_unix_time_ms()
  if zmqPub then zmqPub:send(command..'\0') end
  command = command .. "\r\n"
  spam_socket.InstrumentSocket:send(command)
  local ret,rcvState= spam_socket.InstrumentSocket:receive()

  local t2 = time_utils.get_unix_time_ms()

  if(rcvState == "timeout") then
    if zmqPub then zmqPub:send(rcvState..'\0') end
    error("ARM Response Timeout") 
  end
  if(rcvState == "closed") then 
    if zmqPub then zmqPub:send(rcvState..'\0') end
    error("ARM Port connection closed.") 
  end
  if zmqPub then zmqPub:send(ret..'\0') end
  return ret
end 

function spam_socket._ARM_Send_String_(command)
  local command = tostring(command)
  local t1 = time_utils.get_unix_time_ms()
  if zmqPub then zmqPub:send(command..'\0') end
  command = command .. "\r\n"
  return spam_socket.InstrumentSocket:send(command)
end 

function spam_socket._ARM_Socket_Receive_()
    local ret,rcvState = spam_socket.InstrumentSocket:receive()
    if zmqPub then zmqPub:send(ret..'\0') end
    return ret, rcvState  
end

--- Close the fixture UART socket
-- @treturn nil
function spam_socket._Arm_Socket_Close_()
  if spam_socket.InstrumentSocket then
    spam_socket.InstrumentSocket:close()
    spam_socket.InstrumentSocket = nil
  end
end

print("< ".. tostring(CONFIG.ID).." fixture.spam.lua > Finished Load Fixture  TCP Library...\r\n")

return spam_socket