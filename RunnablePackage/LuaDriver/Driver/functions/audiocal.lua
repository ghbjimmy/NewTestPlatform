local audiocal_module = {}
local relay = require("functions.relay").relay_from_IO_only_item
local time_utils = require("utils.time_utils")
local sequence_utils = require("utils.sequence_utils")
local global_data = require("utils.global_data")
local HWIO = require("hw.HWIO")
local MCU = require("hw.MCU")
local thdn = require("functions.thdn")

require "pathManager"
package.cpath = package.cpath..";"..deleteLastPathComponent(CurrentDir()).."/lib/?.dylib"

require "libGlobal"

audiocal_module.factor={
	CODEC_AMP_DMM_MEASURE_GAIN = 1,
    CODEC_AMP_DMM_MEASURE_OFFSET = 0,-- DMM Offset unit is mV, also DMM measurement unit is mV.

    CODEC_FPGA_AMP_MEASURE_GAIN = 1,
    CODEC_FPGA_AMP_MEASURE_OFFSET = 0,-- FPGA Offset unit is mV, also FPGA measurement unit is mV.

	CODEC_CURRENT_RESISTOR = 10,
}

function audiocal_module.audiocal(sequence,global_var_table)
    local result  = nil
	local f = {	GAIN = 1,
				OFFSET = 0,
				RESISTOR =10, }

	relay(HWIO.MeasureTable["CHANNEL_AI"])	--152,153,157 001
	relay(HWIO.DMMSwitchAITable["H"])
	relay(HWIO.MeasureTable["SPK_ALT_CURR"])	--107-112；152,153,157 111
	--time_utils.delay(1) --FIXME: Do we need to wait longer/shorter?

	-- local voltage = dmm_Volt_Cmd()
	local response = MCU.InstrumentCmd("dmm read(volt)")
    local voltage = string.match(response,"ACK%s*%((.-)mv;")
    if(voltage == nil) then
      error("ERRCODE[-3]Bad response from the MCU: "..tostring(response))
    end

    relay(HWIO.DMMSwitchAITable["DISCONNECT"])
    relay(HWIO.MeasureTable["DISCONNECT"])

    f.GAIN = audiocal_module.factor.CODEC_AMP_DMM_MEASURE_GAIN
    f.OFFSET = audiocal_module.factor.CODEC_AMP_DMM_MEASURE_OFFSET
    -- dmmv = f.GAIN * voltage + (f.OFFSET/1000)
    dmmv = f.GAIN * (tonumber(voltage)*(-1)) + f.OFFSET  -- DMM Offset unit is mV, also DMM measurement unit is mV. The calibrate circuit input is Negative.

    
    if sequence.unit and  (string.upper(sequence.unit) == "MV" or string.upper(sequence.unit) == "V" or string.upper(sequence.unit) == "UV") then
        result = sequence_utils.convert_units(dmmv, "mv", sequence.unit)
    elseif sequence.param1 == "resistor" or (string.upper(sequence.unit) == "OHM" or string.upper(sequence.unit) == "KOHM") then
        result = audiocal_module.resistor(sequence,global_var_table)
    elseif sequence.unit and  (string.upper(sequence.unit) == "MA" or string.upper(sequence.unit) == "A" or string.upper(sequence.unit) == "UA") then
            dmmc = dmmv/(audiocal_module.factor.CODEC_CURRENT_RESISTOR)
            result = sequence_utils.convert_units(dmmc, "ma", sequence.unit)
    else
        error("ERRCODE[-4]Invalid Measurement Unit: "..tostring(unit))
    end
    global_data.set_from_param(global_var_table, sequence.param2, result)
    return result,sequence_utils.check_numerical_limits(result, sequence)
end

--------------------------------local function------------------------------
function audiocal_module.resistor(sequence,global_var_table)
    local resistor = 10
    if sequence.unit == nil then
        error("ERRCODE[-5]resistor function requires a unit!")         --define errcode
    end
    if (string.upper(sequence.unit) == "OHM" or string.upper(sequence.unit) == "KOHM" ) then
        resistor = audiocal_module.factor.CODEC_CURRENT_RESISTOR
        resistor = sequence_utils.convert_units(resistor, "OHM", sequence.unit)
        return resistor
    end
end

function audiocal_module.codecamp(sequence,global_var_table)          --we can call thdn functions directly
    local result  = nil
    if sequence.unit and (string.upper(sequence.unit) == "MV" or string.upper(sequence.unit) == "V" or string.upper(sequence.unit) == "UV") then
        -- thdn.thdn(sequence,global_var_table)
        --result = tonumber(thdn.amplitude(sequence)) 
        result = (tonumber(thdn.amplitude(sequence)) * audiocal_module.factor.CODEC_FPGA_AMP_MEASURE_GAIN + audiocal_module.factor.CODEC_FPGA_AMP_MEASURE_OFFSET/1000)
        global_data.set_from_param(global_var_table, sequence.param2, result)
        return result,sequence_utils.check_numerical_limits(result, sequence)
    else
        error("ERRCODE[-5]Invalid THDN_AMP Unit: "..tostring(unit))
    end
end

function audiocal_module.calver(sequence,global_var_table)
     local codec_cal_version = 3
     local result = codec_cal_version
           global_data.set_from_param(global_var_table, sequence.param2, result)
     return result,sequence_utils.check_numerical_limits(result, sequence)
end

function audiocal_module.getrtctime(sequence,global_var_table)
    local rtctime = 20160118120000
    local  result = 0
    if (tonumber(os.date("%Y%m%d%H%M%S"))) then
       rtctime = tonumber(os.date("%Y%m%d%H%M%S"))
    end   
    result = rtctime
        global_data.set_from_param(global_var_table, sequence.param2, result)
    return result,sequence_utils.check_numerical_limits(result, sequence)
end

function audiocal_module.floattohex(sequence,global_var_table)
    local result = nil
    local ret = tonumber(sequence.param1)   --  sequence.param2 muse be [1,5]  1.6231212  SIP:0x00000001
    print("sequence_param1:"..tostring(sequence.param1))
    print("ret:"..tostring(ret))
    if(ret) then
        local str=floatConversion(ret);  --0.06456534 to 0X32142457
        print("str:"..tostring(str))
        result = str
    end    
    print("result:"..tostring(result))
    global_data.set_from_param(global_var_table, sequence.param2, result)
    return result,sequence_utils.check_numerical_limits(result, sequence)
end

return audiocal_module



