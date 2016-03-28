--- Measure Functions
-- @module functions.measure
-- @alias measure_module
local HWIO = require("hw.HWIO")
local MCU = require("hw.MCU")
local global_data = require("utils.global_data") -- global data mangement library
local sequence_utils = require("utils.sequence_utils")
local time_utils = require("utils.time_utils")
local relay = require("functions.relay").relay_from_IO_only_item

--===========================--
-- Measure exported module
--===========================--
local measure_module = {}
-- function measure_module.measure(sequence_or_testpoint, global_var_table)
-- function measure_module.measure_frequency(sequence_or_testpoint, global_var_table)

--===========================--
-- Private measure functions
--===========================--

--- Measure an AI channel
-- @param channel AI channel to measure
-- @return voltage in mV
-- @fixme What is going on here?
local function fctMeasure(channel)
  local cnt = 0
  local val = 0

  local ret = MCU.InstrumentCmd("adc read", channel);

  for v in string.gmatch(tostring(ret), "([-]?%d+%.?[%d]+)%s*mv") do
    val = val+tonumber(v)
    cnt = cnt+1
  end

  if(cnt==0) then 
    error("ERRCODE[-20]Bad response from the MCU: "..tostring(ret))
    return nil 
  end

  return val/cnt;
end

--===========================--
-- Public measaure Functions
--===========================--

--- Convenience function to measure voltage of a testpoint by name
-- @param testpoint testpoint to measure
-- @return voltage in mV
-- @raise error
-- @within ConvenienceFunctions
function measure_module.measure_testpoint(testpoint)
  if(HWIO.MeasureTable[testpoint]==nil or HWIO.MeasureTable[testpoint].ADC==nil) then
    error("ERRCODE[-38]Invalide test point or Invalide ADC key: ")
    return nil
  end

  local f = {GAIN=1,OFFSET=0}
  if HWIO.MeasureTable[testpoint].GAIN then
    f.GAIN = HWIO.MeasureTable[testpoint].GAIN
    f.OFFSET = HWIO.MeasureTable[testpoint].OFFSET
  end
  -- print("Measure Gain. OFFSET\n",f.GAIN, f.OFFSET)
  
  relay(HWIO.MeasureTable[testpoint])
  --time_utils.delay(1) --FIXME: Do we need to wait longer/shorter?

  local result = fctMeasure(HWIO.MeasureTable[testpoint].ADC)
  result = result * f.GAIN + f.OFFSET
  relay(HWIO.MeasureTable["DISCONNECT"])

  return result
end

--- Convenience function to measure frequency of a signal by name
-- @param testpoint testpoint to measure
-- @return frequency in Hz
-- @raise error
-- @within ConvenienceFunctions
function measure_module.measure_frequency_testpoint(testpoint)
  relay(HWIO.THDNFrequencyTable[testpoint])
  --time_utils.delay(1) --FIXME: Do we need to wait longer/shorter?

  -- FIXME: Update instrument command with new HWIO table format
  -- MCU.InstrumentCmd(string.format("vref set(%s)", HWIO.THDNFrequencyTable[testpoint].VREF))
  local result = MCU.InstrumentCmd(string.format("frequency measure(%s)", HWIO.THDNFrequencyTable[testpoint].SETTINGS))
  result = string.match(tostring(result),"ACK%s*%((.-)Hz")
  relay(HWIO.THDNFrequencyTable["DISCONNECT"])

  if result == nil then
    error("ERRCODE[-21]Bad response from the MCU: "..tostring(result))
  end
  return result
end

--- Convenience function to measure amplitude of a signal by name
-- @param testpoint testpoint to measure
-- @return amplitude in mVpp
-- @raise error
-- @within ConvenienceFunctions
function measure_module.measure_amplitude_testpoint(testpoint)
  error("Amplitude measurements not implemented yet.")
end

--- Measure the net specified by [param1].
-- Depending on the [unit] supplied in the sequence object, this function can measure either 
-- voltage, frequency, and amplitude.
-- @tparam sequence_object sequence FCT test sequence table
-- @param global_var_table table full of "global" test variables
-- @return measurement result
-- @return passing status (whether the returned value is within the expected range)
-- @raise error
function measure_module.measure(sequence, global_var_table)
  if sequence.unit == nil then
    error("ERRCODE[-22]Measure function requires a unit!")
  end

  -- Measure the actual result
  local result = nil
  if string.upper(sequence.unit) == "V" or string.upper(sequence.unit) == "MV" then
    result = sequence_utils.convert_units(measure_module.measure_testpoint(sequence.param1), "mV", sequence.unit)

  elseif string.match(string.upper(sequence.unit), "HZ") ~= nil then
    result = sequence_utils.convert_units(measure_module.measure_frequency_testpoint(sequence.param1), "Hz", sequence.unit)

  -- FIXMEL Amplitude is not implemented yet.
  elseif string.match(string.upper(sequence.unit), "VPP") ~= nil then
    result = sequence_utils.convert_units(measure_module.measure_amplitude_testpoint(sequence.param1), "mVpp", sequence.unit)

  else
    error("ERRCODE[-23]Invalid Unit for Measure: "..tostring(sequence.unit))
  end

  -- set global variables if they exist
  global_data.set_from_param(global_var_table, sequence.param2, result)

  return result, sequence_utils.check_numerical_limits(result, sequence)
end

-----------------------------------
-- Export measure to module users
-----------------------------------
return measure_module
