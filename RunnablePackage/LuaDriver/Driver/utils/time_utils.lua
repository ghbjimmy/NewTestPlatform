--- Time utils
-- @module utils.time_utils
-- @alias time_utils_module
local zpoller = require "lzmq.poller"
local ztimer = require "lzmq.timer"

local time_utils_module = {}
-- function time_utils.delay(sequence)
-- function time_utils.get_unix_time_ms()
-- function time_utils.get_time_delta_ms(start_time)
-- function time_utils.get_time_string_ms()

local poller = zpoller.new(1)

--- Delay (not a busywait)
-- @param ms
-- @return status (false if there was an error)
function time_utils_module.delay(ms)
	local result, err = poller:poll(ms)
	return err == nil
end

--- Get unix time in MS
-- @return time in ms
function time_utils_module.get_unix_time_ms()
	return ztimer.absolute_time()
end

--- Get difference between a unix timestamp and the current time
-- @param start_time unix timestamp(in ms)
-- @return time difference in ms
function time_utils_module.get_time_delta_ms(start_time)
	return ztimer.absolute_elapsed(start_time)
end

--- Get a formatted time string for the current time.
-- Format will be used across all log files and print outputs. It is currently YEAR-MONTH-DAY 24HOUR:MIN:SEC.MS
-- @return formatted time string
-- @return unix time in ms
function time_utils_module.get_time_string_ms()
	print('Han here1')
	local t = time_utils_module.get_unix_time_ms()
	print('Han here2  t:', t)
	local t_ms = t - 1000 * math.floor(t/1000)
	local something = os.date("%Y-%m-%d %H:%M:%S.", t/1000)
	--local something = os.date(t/1000)
	print('Han here3', something, 't_ms', t_ms)
	--something = something..string.format("%03d",t_ms)
	print('Han here4', something)

	--return os.date("%Y-%m-%d %H:%M:%S.", t/1000)..string.format("%03d",t_ms), t
	t = os.time()
	return os.date("%Y-%m-%d %H:%M:%S.", t)..string.format("%03d",t_ms), t*1000
end

return time_utils_module