--- Table of implemented functions
-- @module function_table

--- Function table, maps function name -> actual lua function
-- @table function_table
local function_table = {
    han_sayhello    = require("functions.hantest").SayHello,
    han_error       = require("functions.hantest").errCode,
    han_fail        = require("functions.hantest").failItem,
    get_version     = require("functions.version").GetVersion,

    start_test      = require("functions.callback").Test_OnEntry,
    end_test        = require("functions.callback").Test_OnDone,

    --[[
    datalogger      = require("functions.datalogger").datalogger, -- Get current from the datalogger
    dmm             = require("functions.dmm").dmm, -- Measure current and voltage from the DMM
    eload           = require("functions.eload").eload, -- Set a current load on the specified net
    iload           = require("functions.eload").iload, -- Measure actual current on Eload for specified net
    delay           = require("functions.fixture").delay, -- Delay execution for the given amount of time
    measure         = require("functions.measure").measure, -- Measure voltage / frequency on the given net
    relay           = require("functions.relay").relay, -- Connect specified external relays on the fixture
    disconnect      = require("functions.relay").disconnect, -- Disconnect specified external relays on the fixture
    button          = require("functions.relay").button, -- "Press" button 1
    tbat            = require("functions.relay").tbat, -- Set the TBatt digipot to a given resistance
    supply          = require("functions.supply").supply, -- Supply voltage to the given net
    thdn            = require("functions.thdn").thdn, -- Measure the THDN of a PDM audio signal
    amplitude       = require("functions.thdn").amplitude, -- Measure the THDN of a PDM audio signal
    frequency       = require("functions.thdn").frequency, -- Measure the THDN of a PDM audio signal
    
    reset           = require("functions.reset").reset,

    diags           = require("functions.dut").diags, -- Execute a diags command
    parse           = require("functions.dut").parse, -- Parse text from a diags command and store in a global variable
    detect          = require("functions.dut").detect, -- Detect characters in the DUT serial output
    calculate       = require("functions.dut").calculate, -- Run lua code using global variables and store in another global variable
    smokey          = require("functions.dut").smokey,
    reference        = require("functions.reference").reference,

    station         = require("functions.GhInfo").GhStation,
    fixturetype     = require("functions.GhInfo").fixturetype,
    fixturename     = require("functions.GhInfo").fixturename,
    channel         = require("functions.Global").channel,
    getboottime     = require("functions.Global").GetBootTime,--TBD
    getbootcount    = require("functions.Global").GetBootCount,--TBD
    fatalerror      = require("functions.fetalerror").CheckFetalError,
    
    -- setsn           = require("functions.SN").setSN,
    getsn           = require("functions.SN").getSN,--this fucntion only work for SIP
    comparesn       = require("functions.SN").compareSN,
    spamloopback    = require("functions.spam").spamloopback,

    --For Debug Panel
    ioset           = require("functions.ioset").ioset,
    dmmmeasure      = require("functions.dmm").dmmmeasure, --this function will instead ADC AI measure
    audiocal        = require("functions.audiocal").audiocal,  --this function get alert vrms after dmm calibratioj
    resistor        = require("functions.audiocal").resistor,  --this function get alert load resistor value
    codecamp        = require("functions.audiocal").codecamp,  --this function get alert vrms after fpga calibration value
    calver          = require("functions.audiocal").calver,  --this function get codec calibration version
    getrtctime      = require("functions.audiocal").getrtctime,  --this function get alert calibration RTC time
    floattohex      = require("functions.audiocal").floattohex,  --Change float to Hex
    --]]--
}

return function_table
