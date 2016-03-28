--- Fixture functions
-- @module functions.fixture
-- @alias fixture_module
local time_utils = require("utils.time_utils")
local sequence_utils = require("utils.sequence_utils")

--===========================--
-- Supply exported module
--===========================--
local fixture_module = {}
-- function fixture_module.delay(sequence)

--===========================--
-- Public API
--===========================--

--- Delay the sequence for [param1] milliseconds
-- @tparam sequence_object sequence FCT test sequence table
-- @return true if there were no errors
-- @return passing status (true if no errors)
-- @raise error
function fixture_module.delay(sequence)
	if not tonumber(sequence.param1) then
    	error("ERRCODE[-16]Invalid delay: "..tostring(sequence.param1))
    elseif tonumber(sequence.param1) < 0 then
    	error("ERRCODE[-17]Can't have a negative delay: "..tostring(sequence.param1))
  	end

  	local delay_ms = sequence_utils.convert_units(sequence.param1, sequence.unit, "ms")

  	local result = time_utils.delay(delay_ms)
  	return result, result
end

return fixture_module