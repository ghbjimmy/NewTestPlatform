local console = require(CONFIG.FIXTURE_CONSOLE)
console._Arm_Socket_Connect_()
-- local console = require("console.fixture.socket_pub")
-------------------------------
-- MCU exported module
-------------------------------
local MCU_module = {}
-- function MCU_module.RawInstrumentCmd(cmd)
-- function MCU_module.InstrumentCmd(cmd, argument1, argument2)


-------------------------------
-- Public API
-------------------------------
function MCU_module.RawInstrumentCmd(cmd)
  local result = console._ARM_Send_Cmd_(cmd)

  if result == nil or string.match(tostring(result),"ERR") ~= nil then
  --不能用ACK，读取电压电流以及frq等值时，socket返回的result没有ACK标志
  -- if result == nil or string.match(tostring(result),"ACK") == nil then
    error("ERRCODE[-1]Problem communicating with MCU:"..tostring(result))
  end
  return result
end

function MCU_module.InstrumentCmd(cmd, argument1, argument2)
  local full_command = ""

  if(string.match(cmd, "adc read")) then  --measure ADC (in mV)
    full_command = string.format("adc read(%s,%d)", argument1, 10);

  elseif(string.match(cmd, "io set"))then --io set, connect or disconnect
    local count = 0;
    local tps = {}
    local tpv = {}

    -- FIXME: Do not use strings, use indexed tables ("lists") instead
    for v in string.gmatch(argument1, "(%d+)") do
      count = count + 1
      table.insert(tps, v)
    end

    for v in string.gmatch(argument2, "(%d+)") do
      table.insert(tpv, v)
    end

    full_command = "io set("..tostring(count)..","
    for i = 1, count do
      full_command = full_command.."bit"..tostring(tps[i]).."="..tostring(tpv[i])..","
    end
    full_command = string.sub(full_command, 1, -2)
    full_command = full_command..")"
  
  elseif(string.match(cmd, "dac set")) then --DAC set (in mV)
    full_command = string.format("dac set(-c,%s,%.01f)", argument1, argument2);
  
  else
    full_command = cmd
  end

  return MCU_module.RawInstrumentCmd(full_command)
end

-- print("MCU's command :->"..full_command)
return MCU_module
