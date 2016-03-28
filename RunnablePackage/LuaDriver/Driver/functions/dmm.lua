--- DMM Function
-- @module functions.dmm
-- @alias dmm_module
local dmm_module = {}

local HWIO = require("hw.HWIO")
local MCU = require("hw.MCU")
local global_data = require("utils.global_data") -- global data mangement library
local sequence_utils = require("utils.sequence_utils")
local time_utils = require("utils.time_utils")
local relay = require("functions.relay").relay_from_IO_only_item

--===========================--
--- Private Functions
-- @section private
--===========================--
dmm_module.factor={
  LowCurr = {GAIN=1, OFFSET=0},
  HighCurr = {GAIN=1, OFFSET=0},
  Volt = {GAIN=1, OFFSET=0},
  AI = {GAIN=1, OFFSET=0},
}


--- Get current value from DMM (in mA)
-- @param net_name the net to measure through
-- @param read_current_in_ua whether to read the current in uA or not
-- @return current in mA
VOLT_NP = 1   -- Global param for Eload use.
function dmm_module.dmmVersionCheck()
  print("Get DMM Board Type and Version")
  local dmm_sn_str = MCU.InstrumentCmd("eeprom read string(dmm,cat08,0x00,17)")
  print("Get DMM board name from : "..tostring(dmm_sn_str))
  if not string.match(string.upper(tostring(dmm_sn_str)),"DMM") then
    error("ERRCODE[-998]Cant get DMM board name from : "..tostring(dmm_sn_str))
  end
  local dmm_version = ""
  local str = MCU.InstrumentCmd("eeprom read string(dmm,cat08,0x45,4)")
  print("Get DMM board name from : "..tostring(str))
  if(str ~= nil) then
      dmm_version = string.match(str,'ACK%(%"(.+)%"')
      -- print( " < DMM Version > : ",dmm_version )
      if(dmm_version == "D2.2") then
        VOLT_NP = 1
      else
        VOLT_NP = -1
      end
  else
    print("Cant Get DMM Version.")
    error("ERRCODE[-998]Get DMM Version Error.")
  end 
  return dmm_version.."  ( MCU Return: "..tostring(dmm_sn_str).." "..tostring(str)..")"
end

local function dmm_current(net_name, read_current_in_ua)
    local f = {GAIN=1,OFFSET=0}
    if read_current_in_ua then
        net_name = net_name .. "_UA"
        MCU.InstrumentCmd("dmm set(100 uA)")
        f.GAIN = dmm_module.factor.LowCurr.GAIN
        f.OFFSET = dmm_module.factor.LowCurr.OFFSET
    else
        net_name = net_name .. "_MA"
        MCU.InstrumentCmd("dmm set(2 mA)")
        f.GAIN = dmm_module.factor.HighCurr.GAIN
        f.OFFSET = dmm_module.factor.HighCurr.OFFSET
    end
    -- print("DMM Current Gain Offer", f.GAIN, f.OFFSET)
    relay(HWIO.DMMCurrentTable[net_name])
    --time_utils.delay(1)

    local response = MCU.InstrumentCmd("dmm read(curr)")
    local current = string.match(response,"ACK%s*%((.-)uA;")
    relay(HWIO.DMMCurrentTable["DISCONNECT"])
    if current == nil then
        error("ERRCODE[-2]Bad response from the MCU: "..tostring(response))
    end

    return f.GAIN * current + f.OFFSET
end

--- Get voltage value from DMM (in mV)
-- @param net_name the net to measure
-- @return voltage in mV
local function dmm_Volt_Cmd()
    local response = MCU.InstrumentCmd("dmm read(volt)")
    local voltage = string.match(response,"ACK%s*%((.-)mv;")
    if(voltage == nil) then
      error("ERRCODE[-3]Bad response from the MCU: "..tostring(response))
    end
    return voltage
end

local function dmm_voltage(net_name)
  local f = {GAIN=1,OFFSET=0}
  relay(HWIO.MeasureTable[net_name])
  local adc = HWIO.MeasureTable[net_name].ADC --this need to support AI measure
  if(adc) then
    relay(HWIO.MeasureTable["CHANNEL_AI"])
    relay(HWIO.DMMSwitchAITable[adc])
    f.GAIN = HWIO.MeasureTable[net_name].GAIN--dmm_module.factor.AI.GAIN
    f.OFFSET = HWIO.MeasureTable[net_name].OFFSET--dmm_module.factor.AI.OFFSET
  else
    f.GAIN = dmm_module.factor.Volt.GAIN
    f.OFFSET = dmm_module.factor.Volt.OFFSET
  end
  -- print("DMM Current Gain Offer", f.GAIN, f.OFFSET)
  --time_utils.delay(1)
  local voltage = dmm_Volt_Cmd()
  relay(HWIO.MeasureTable["DISCONNECT"]) 
  return  f.GAIN * voltage * VOLT_NP + f.OFFSET
end 

--===========================--
--- Public API
-- @section public
--===========================--

--- Use the DMM to read current or voltage for a net
-- @tparam sequence_object sequence FCT test item table
-- @param global_var_table table full of "global" test variables
-- @return value read from the DMM in the correct units
-- @return passing status (true if unit is within limits, false otherwise)
function dmm_module.dmm(sequence, global_var_table)
    local result = nil
    if(sequence.param1 == "DISCONNECT") then
        print("dmm here")
        relay(HWIO.MeasureTable["DISCONNECT"])
        return true, true
    end
    if sequence.unit and  (string.upper(sequence.unit) == "MV" or 
            string.upper(sequence.unit) == "V" or string.upper(sequence.unit) == "UV") then
        dmmv = dmm_voltage(sequence.param1)
        result = sequence_utils.convert_units(dmmv, "mV", sequence.unit)

    elseif sequence.unit and  (string.upper(sequence.unit) == "MA" or 
            string.upper(sequence.unit) == "A" or string.upper(sequence.unit) == "UA") then
        dmmc = dmm_current(sequence.param1, string.upper(sequence.unit) == "UA")
        result = sequence_utils.convert_units(dmmc, "ua", sequence.unit)
    else
        error("ERRCODE[-4]Invalid Measurement Unit: "..tostring(sequence.unit))
    end

    -- set global variables if they exist
    global_data.set_from_param(global_var_table, sequence.param2, result)

    -- Value, unit-passed
    return result, sequence_utils.check_numerical_limits(result, sequence)
end

function dmm_module.dmmmeasure(sequence,global_var_table)
  return dmm_module.dmm(sequence,global_var_table)
end

--===========================--
-- Export dmm to module users
--===========================--
-- dmmVersionCheck()

return dmm_module
