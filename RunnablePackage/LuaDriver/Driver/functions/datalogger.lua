--- This function communicates over MCU command
-- datalogger -- it requests an RMS current over a specific number of samples.
--
-- It uses the following ZMQ Sockets:
-- <ul>
--      <li><span class="parameter">DATALOGGER</span> REQ socket</li>
-- </ul>
-- 
--
-- It assumes that there is another program running that responds to requests on
-- the <code>DATALOGGER</code> socket.
-- @module functions.datalogger
-- @alias datalogger_module


local HWIO = require("hw.HWIO")
local MCU = require("hw.MCU")
local time_utils = require("utils.time_utils")
local global_data = require("utils.global_data")
local sequence_utils = require("utils.sequence_utils")

--===========================--
-- Datalogger exported module
--===========================--
local datalogger_module = {}

--===========================--
-- DUT public API
--===========================--
datalogger_module.factor = {GAIN=1,OFFSET=0}
--- Request the RMS of the last [param1] samples from the datalogger.
-- @tparam sequence_object sequence FCT test sequence table
-- @param global_var_table table full of "global" test variables
-- @return measured current
-- @return passing status (whether the returned value is within the expected range)
-- @raise error
function datalogger_module.datalogger(sequence, global_var_table)--par = {count, “globalkey”, "unit"}
	MCU.InstrumentCmd("datalogger open(ADC_NORMAL)")--open datalogger, support sequencer step function 
	local timeout = 3000 -- We will timout if we don't get an answer after 1 seconds. FIXME: Do we need to add the count int?
	if tonumber(sequence.timeout) then timeout=tonumber(sequence.timeout) end
	local count = 300
	if tonumber(sequence.param1) ~= nil then
		count = tonumber(sequence.param1)
	end
	local msg = MCU.InstrumentCmd(string.format("datalogger current measure(%d,%d)",count, timeout))
	local result, rms = string.match(msg, "ACK%(([-]?%d+),(0x%x+);DONE")
	if(result) then
		result = (2*2.5*tonumber(result)/(tonumber(count)*(2^31)*3.9117647059))
		result = datalogger_module.factor.GAIN * result + datalogger_module.factor.OFFSET
		result = sequence_utils.convert_units(result, "A", sequence.unit)
	end
	
     -- set global variables if they exist
  	global_data.set_from_param(global_var_table, sequence.param2, result)

  	return result, sequence_utils.check_numerical_limits(result, sequence)
end


return datalogger_module
