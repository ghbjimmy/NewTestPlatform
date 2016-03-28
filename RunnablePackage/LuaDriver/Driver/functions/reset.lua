--- Reset Function
-- @module functions.reset
-- @alias reset_module

local MCU = require("hw.MCU")
local HWIO = require("hw.HWIO")
local time_utils = require("utils.time_utils")
local relay = require("functions.relay").relay_from_connections
local disconnect = require("functions.relay").disconnect_from_connections

--===========================--
-- Reset exported module
--===========================--
local reset_module = {}
-- function reset_module.reset(sequence)
-- function reset_module.reset_raw()

--===========================--
-- Public Reset Functions
------------------------------

-- FIXME: Maybe encapsulate this better.
function reset_module.reset_raw()

disconnect(HWIO.EloadTable)               --180,179,178,177=0,0,0,0
relay(HWIO.RelayTable.DISCHARGER)         --121
disconnect(HWIO.RelayTable.SEAJAY_BOX)    --104,143
disconnect(HWIO.RelayTable.PP_VDD_MAIN)   --101,141
disconnect(HWIO.RelayTable.PP_VRECT)      --122,123,124=0,0,0
disconnect(HWIO.RelayTable.PP_BATT_VCC)   -- 100,142

MCU.RawInstrumentCmd("dac set(A,0)")
MCU.RawInstrumentCmd("dac set(B,0)")
MCU.RawInstrumentCmd("dac set(C,0)")
MCU.RawInstrumentCmd("dac set(D,0)")

time_utils.delay(800)

-- MCU.RawInstrumentCmd("datalogger close()")
-- MCU.RawInstrumentCmd("codec close pdm()")

disconnect(HWIO.RelayTable.ELOAD_CTRL)             --098 //For power sequence only
relay(HWIO.RelayTable.DUT_ARM_UART)           --097
disconnect(HWIO.RelayTable.FREQUENCY_EXTERNAL)     --099
disconnect(HWIO.RelayTable.SPAM_BOX)               --103,105,106

disconnect(HWIO.RelayTable.BAUX_DEBUG_TO_PMU_VBUS_DET_L)   --113
disconnect(HWIO.RelayTable.LISA_BTN_L)                 --114
disconnect(HWIO.RelayTable.BUTTON1_L)                  --115
disconnect(HWIO.RelayTable.DEBUG_CON_DET_L)            --116
-- disconnect(HWIO.RelayTable.PMU_UWAKE_L)                --117
-- disconnect(HWIO.RelayTable.SOCKET_I2C_WP)                --127
disconnect(HWIO.RelayTable.SPEAKER)                --138,139,140
-- disconnect(HWIO.RelayTable.STOCKHOLM_UART)                --144
relay(HWIO.RelayTable.MIPI_OVP) 
time_utils.delay(1)
disconnect(HWIO.RelayTable.MIPI_OVP)                --146
-- disconnect(HWIO.RelayTable.PDM)                --147
disconnect(HWIO.RelayTable.PP5V0_BAUX_VMID)                --156
disconnect(HWIO.RelayTable.POWER_SEQUENCE_TRIGGER)                --160

-- disconnect(HWIO.RelayTable.DEBUG_BI_BAUX_PWR_INH_L)                --175
disconnect(HWIO.RelayTable.BAUX_TCK_GPIO6)                --176
-- disconnect(HWIO.RelayTable.PMU_SHDN)                --192
disconnect(HWIO.RelayTable.PP1V5_BAUX_VSRC)                --207
-- disconnect(HWIO.RelayTable.CRIKET_1V8)                --208

-- relay(HWIO.THDNAmplitudeTable.DISCONNECT)            --118,119,120=0,0,0
disconnect(HWIO.RelayTable.THDNAmplitudeTable)            --118,119,120=0,0,0

disconnect(HWIO.RelayTable.TP_TCXO_CAL_CLK10M)               --181 126=1,0
disconnect(HWIO.RelayTable.GRAPE_PULL_UP_RES_TEST)               --206,128=0,0
disconnect(HWIO.DMMCurrentTable)               --134,133,132=0,0,0

disconnect(HWIO.DMMSwitchAITable)               --151 = 0 --无150,149,148=0,0,0,0
disconnect(HWIO.RelayTable.DMMSwitchGNDTable)               --157,153,152=0,0,0
disconnect(HWIO.RelayTable.CITRINE)               --154,155=0,0
disconnect(HWIO.RelayTable.PMU_NTC3)               --164,159=0,0
disconnect(HWIO.RelayTable.AP_BI_GG_TO_PMU_HDQ_CONN)               --161,162=0,0
disconnect(HWIO.RelayTable.PMU_TBAT)               --166,165,163=0,0,0
disconnect(HWIO.RelayTable.NTC_MEASURE)               --170,169,168,167=0,0,0,0
-- disconnect(HWIO.RelayTable.LED_CTRL)               --173,172,171=1,1,1

-- disconnect(HWIO.RelayTable.ALS_TO_AOP_INT_L)               --185,184,183,182=0,0,0,0
disconnect(HWIO.THDNFrequencyTable)               --190,189,188,187=0,0,0,0
disconnect(HWIO.RelayTable.PP4V8_HV_ANODE)               --195,194,193=0,0,0
disconnect(HWIO.MeasureTable)               --202,201,200,199,198,197=0,0,0,0
disconnect(HWIO.RelayTable.PLAT_TIA01)               --205,204=0,0


  --------------------------------------------------------------------------------------------------------
  --------------------------------------------------------------------------------------------------------
  -- time_utils.delay(1000)
  -- disconnect(HWIO.RelayTable.PP_BATT_VCCDISCHARGERES)
  return true, true;
end

--- NOT IMPLEMENTED: Reset the fixture to its default state
-- @tparam sequence_object sequence FCT test sequence table
-- @param global_var_table table full of "global" test variables
-- @return result (0)
-- @return passing status (true)
-- @raise error
function reset_module.reset(sequence, global_var_table)
  return reset_module.reset_raw(), true
end

-----------------------------------
-- Export eload to module users
-----------------------------------
return reset_module
