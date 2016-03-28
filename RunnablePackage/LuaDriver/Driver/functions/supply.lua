--- Supply Functions
-- @module functions.supply
-- @alias supply_module
local HWIO = require("hw.HWIO")
local MCU = require("hw.MCU")
local sequence_utils = require("utils.sequence_utils")
local time_utils = require("utils.time_utils")
local relay = require("functions.relay").relay_from_connections
local disconnect = require("functions.relay").disconnect_from_connections
local measure = require("functions.measure").measure_testpoint
local dmm = require("functions.dmm").dmm

--===========================--
-- Supply exported module
--===========================--
local supply_module = {}
-- function supply_module.supply(sequence)

--===========================--
-- Global variables
--===========================--
_TOLERANCE_ = 0.01

--===========================--
-- Support functions
--===========================--
local function disconnect_discharge_resistors()
  disconnect(HWIO.RelayTable.DISCHARGER)
end

local function is_calibrated()
  --FIXME: Check calibration is set?
  return true
end

local function check_calibration(seq, param2)
  local voltage = dmm(seq)
  -- print("< measure Voltage > : ", voltage)
  if voltage == nil then
    return false
  end

  percent = (voltage - param2) / param2
  if(math.abs(percent) > _TOLERANCE_) then
    return false,voltage
  end

  return true,voltage
end


--===========================--
-- Private Supply Function
--===========================--

--- Supply a voltage of [param2] to the net [param1].
-- @tparam sequence_object sequence FCT test sequence table
-- @return fixture UART response
-- @raise error
local function supply_voltage(sequence)
  local f = {GAIN=1,OFFSET=0}
  if(HWIO.SupplyTable[sequence.param1].GAIN) then
    f.GAIN = HWIO.SupplyTable[sequence.param1].GAIN
    f.OFFSET = HWIO.SupplyTable[sequence.param1].OFFSET
  end
  -- print("Supply , Gain, Offset", f.GAIN, f.OFFSET)
  relay(HWIO.RelayTable[sequence.param1])
  local voltage = sequence_utils.convert_units(sequence.param2, sequence.unit, "mV")
  local voltTar = voltage
  voltage = f.GAIN * voltage + f.OFFSET * 1000
  if voltage < 0 then voltage = 0 end

  if HWIO.SupplyTable[sequence.param1].DIVIDE then
    voltage = voltage/HWIO.SupplyTable[sequence.param1].DIVIDE
  end
  local result = MCU.InstrumentCmd("dac set", HWIO.SupplyTable[sequence.param1].DAC, voltage) --set dac to param2

  -- If calibration check is bad, fail!
  local flag, vol = check_calibration(sequence, voltTar)
  --Disable adjust as Mark's requirement. 
  -- if(not flag) then 
  --   error("ERRCODE[-30]Calibration check failed! :(measure,set)"..tostring(vol)..","..tostring(voltage))
  -- end
  return vol--result
end


--- DEPRECATED: Supply a frequency of [param2] to the net [param1].
-- Use the relay command to connect to a frequency source instead.
-- @tparam sequence_object sequence FCT test sequence table
-- @raise error
-- @see functions.relay.relay
local function supply_frequency(sequence)
  local frequency = sequence_utils.convert_units(sequence.param2, sequence.unit, "Hz")
  --FIXME: This should replace with relay function
  relay(HWIO.RelayTable[sequence.param1][sequence.param2])
  return true,true
  -- error("Frequency supply not implemented -- use relay!")
end

--===========================--
-- Public Supply Function
--===========================--

--- Supply a voltage of [param1] to the [param1] net
-- @tparam sequence_object sequence FCT test sequence table
-- @return result (true)
-- @return passing status (true)
-- @raise error
function supply_module.supply(sequence)
  if not tonumber(sequence.param2) then
    error("ERRCODE[-31]Invalid supply voltage")
  elseif tonumber(sequence.param2) < 0 then
    error("ERRCODE[-32]Can't supply negative voltages.")
  end
  
  disconnect_discharge_resistors()
  --time_utils.delay(1)

  -- Check for voltage or frequency
  local result = nil
  if sequence.unit == nil then
    error("ERRCODE[-33]Unit column can not be empty.")

  elseif string.match(sequence.unit, "V") then
    -- If not calibrated, FAIL
    if not is_calibrated(sequence.param1) then 
      error("ERRCODE[-34]Supply is not calibrated.")
    end
    result = supply_voltage(sequence)
  elseif string.match(sequence.unit, "Hz") then
    result = supply_frequency(sequence)

  else
    error("ERRCODE[-35]Invalid Supply Unit!")
  end

  -- Value, passing-status
  return result, true
end

--===========================--
-- Export supply to module users
--===========================--
return supply_module
