--- Eload Function Module
-- @module functions.eload
-- @alias eload_module

local _ELOAD_MEAS_RESISTOR_ = 3.1
local _ILOAD_MEAS_RESISTOR_ = 0.1 * 100 -- sample resistor = 0.1, amp =100
local AMP_GAIN=10--amplifier gain
local NOR_RES = 0.1--Simple resistor unit R
-- _TEST_ELOAD_ = VT.ELOAD_CURRENT  

local HWIO = require("hw.HWIO")
local MCU = require("hw.MCU")
local global_data = require("utils.global_data") -- global data mangement library
local sequence_utils = require("utils.sequence_utils")
local time_utils = require("utils.time_utils")
local relay = require("functions.relay").relay_from_IO_only_item
local measure = require("functions.measure").measure_testpoint
local dmm = require("functions.dmm").dmm

--===========================--
-- Eload exported module
--===========================--
local eload_module = {}
-- function eload_module.eload(sequence)
-- function eload_module.iload(sequence, global_var_table)

--===========================--
-- Public Eload Functions
--===========================--

--- Apply an E-load of [param2] mA to the [param1] net
-- @tparam sequence_object sequence FCT test sequence table
-- @return true
-- @return passing status (whether the eload was sucessfully applied)
-- @raise error
function eload_module.eload(sequence)
  local current = tonumber(sequence.param2)
  if current == nil or current < 0 then
    error("ERRCODE[-12]Invalid Current! "..tostring(current))
  end

  if sequence.unit == nil or string.upper(sequence.unit) ~= "MA" then
    error("ERRCODE[-13]Invalid unit -- only mA is allowed: "..tostring(sequence.unit))
  end
  -- dac_value = sequence.param2
  -- -- First, we should be draining nothing...
  -- MCU.InstrumentCmd("dac set", HWIO.EloadTable[sequence.param1].DAC, dac_value)

  local f = {GAIN=1, OFFSET=0}
  if HWIO.EloadTable[sequence.param1].GAIN then
    f.GAIN = HWIO.EloadTable[sequence.param1].GAIN
    f.OFFSET = HWIO.EloadTable[sequence.param1].OFFSET
  end

  MCU.InstrumentCmd("dac set", HWIO.EloadTable[sequence.param1].DAC, -1000) -- Set Eload negetive current, make sure Load must be Closed!
  time_utils.delay(30)

  if(current == 0 ) then
     sequence.param1 = "DISCONNECT"       -- When supply eload 0 mA then disconnect Eload.
  end 

  relay(HWIO.EloadTable[sequence.param1])
  --time_utils.delay(1)
  -- print("Eload Gain Offer", f.GAIN, f.OFFSET)
  local dac_value = f.GAIN * current*_ELOAD_MEAS_RESISTOR_ + f.OFFSET
  -- if(dac_value<0) then dac_value=0 end
  local ret = MCU.InstrumentCmd("dac set", HWIO.EloadTable[sequence.param1].DAC, dac_value) --set
  -- return true, true
  return eload_module.iload(sequence)
end

--- Measure the actual current through the eload
-- @tparam sequence_object sequence FCT test sequence table
-- @param global_var_table table full of "global" test variables
-- @return measured current
-- @return passing status (whether the returned value is within the expected range)
-- @raise error
function eload_module.iload(sequence, global_var_table)
  if sequence.unit and not string.match(string.upper(sequence.unit), "A") then
      error("ERRCODE[-14]Invalid Measurement Unit: "..tostring(sequence.unit))
  end
  relay(HWIO.MeasureTable["ELOAD_CURRENT"])
  relay(HWIO.MeasureTable["CHANNEL_AI"])
  relay(HWIO.DMMSwitchAITable["G"])
  --time_utils.delay(1)
--measure volt
  local response = MCU.InstrumentCmd("dmm read(volt)")
  local result = string.match(response,"ACK%s*%((.-)mv;")
  if(result == nil) then
    error("ERRCODE[-15]Invalid measurement result: "..tostring(result))
  end
  result = (tonumber(result)/_ILOAD_MEAS_RESISTOR_)
  local f = {GAIN=1,OFFSET=0}
  if HWIO.MeasureTable["ELOAD_CURRENT"].GAIN then
    f.GAIN = HWIO.MeasureTable["ELOAD_CURRENT"].GAIN
    f.OFFSET = HWIO.MeasureTable["ELOAD_CURRENT"].OFFSET
  end
  result = (result * VOLT_NP) * f.GAIN + f.OFFSET
  global_data.set_from_param(global_var_table, sequence.param2, result)
  result=sequence_utils.convert_units(result,"mA",sequence.unit)
  return result, sequence_utils.check_numerical_limits(result, sequence)
end

function eload_module.current(sequence, global_var_table) 
        --ret == para[1] PP_VRECT,PP_CON_DET_EN_PIN,PP_VDD_MAIN
        --For the PP_BATT_VCC, Please use datalogger measure instead.
    if sequence.param1 == "PP_VRECT" or sequence.param1 == "PP_CON_DET_EN_PIN" or sequence.param1 == "PP_VDD_MAIN" then
         local net_name=tostring(sequence.param1).."_FIXTURE"
         if net_name == nil then
              error("ERROR param"..tostring(sequence.param1))
         end
      --switch relay
         relay(HWIO.MeasureTable[net_name])
         relay(HWIO.MeasureTable["CHANNEL_AI"])
         local adc = HWIO.MeasureTable[net_name].ADC --this need to support AI measure
         relay(HWIO.DMMSwitchAITable[adc])

         --time_utils.delay(1)
      --measure volt
         local response = MCU.InstrumentCmd("dmm read(volt)")
         local voltage = string.match(response,"ACK%s*%((.-)mv;")
         if(voltage == nil) then
           error("ERRCODE[-3]Bad response from the MCU: "..tostring(response))
         end
      --convert to current
         local result=((voltage/1000)/AMP_GAIN)/NOR_RES 
      --get gain
         local f = {GAIN=1,OFFSET=0}
         if HWIO.MeasureTable[net_name].GAIN then
           f.GAIN = HWIO.MeasureTable[net_name].GAIN
           f.OFFSET = HWIO.MeasureTable[net_name].OFFSET
         end
      -- linear fit
         result = (result * VOLT_NP) * f.GAIN + f.OFFSET
         result=sequence_utils.convert_units(result,"A",sequence.unit)
         return result
    else
        error("This function only support measure (PP_VRECT/PP_CON_DET_EN_PIN/PP_VDD_MAIN) current.")
    end  
end

return eload_module

