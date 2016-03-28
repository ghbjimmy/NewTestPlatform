
local AirPressureTable = {}
local mid = 0;
if(CONFIG and CONFIG.ID) then
  mid = tonumber(CONFIG.ID)
end
-----------------Socket init
print("< ".. tostring(mid).." > Load AirPressure Lua...")

local sc = require "socket"

local pressureIp = "169.254.99.65"--"169.254.160.170"
local pressurePort = 6661

local AirPressureSocket = nil

--socket function
local function _Pressure_Socket_Connect_()
  print("connect to: "..pressureIp ..":"..tostring(pressurePort).."...")
  AirPressureSocket = sc.timeout_connect(5,tostring(pressureIp), tonumber(pressurePort))
  if AirPressureSocket == nil then 
    print("< Reference > : Connect Reference Server fail.")
  else
    AirPressureSocket:settimeout(1)
    print("< Reference > : connect successfully")
  end
end


local function _Pressure_Get_()
  if AirPressureSocket then
    cmd = "read\r\n"
    AirPressureSocket:send(cmd)
    local ret,rcvState= AirPressureSocket:receive()-- return:  1   101.721
    AirPressureSocket:receive()
    print(ret)
    if(rcvState == "timeout") then error("Pressure Server Response Timeout") end
    if(rcvState == "closed") then error("Pressure Server Port connection closed.") end
    if(ret) then
      return string.match(ret, "%d%s*([^\r\n]*)")
    end
  else
    error("Pressure Server connect fail")
  end
end 

local function _Pressure_Socket_Close_()
  AirPressureSocket:close()
  AirPressureSocket = nil
end

_Pressure_Socket_Connect_()


function AirPressureTable.reference(sequence, global_var_table) --par={"params2", "unit"}
  local pressure = _Pressure_Get_()
  local result = tonumber(pressure)

  if(result) then
    return result, true
  else
    error("Got nil value")
  end
end

return AirPressureTable
