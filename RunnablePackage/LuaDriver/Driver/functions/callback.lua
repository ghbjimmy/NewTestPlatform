local time_utils = require("utils.time_utils")
local HWIO = require("hw.HWIO")
--local relay = require("functions.relay").relay_from_connections
--local led = require("functions.relay").relay
--local fixtureConsole = require(CONFIG.FIXTURE_CONSOLE)
--local dutConsole = require(CONFIG.DUT_UART_CONSOLE)
--local global_data = require("utils.global_data")
--local reset = require("functions.reset").reset
--local dut = require("tcpAndZmq")
--local carrier = require("functions.IASocketLua").ReadSnFromCarrier
--local stationInfo = require("functions.GhInfo").fixturetype
--local stationName = require("functions.GhInfo").GhStationName()
--local logpath = require("functions.getlogpath").getLogPath
--local soc = require("utils.socket")
--local sipsn = require("functions.SN")

--local armdl = require("functions.ArmDL")
--local tmStartLog = require "utils.LogFile"
local _callback_ = {}

local function getUtcTime()
	--[[
	if(soc) then
	   local utcTime = soc.gettime()
	   if(utcTime) then
		    local utcSec = string.match(utcTime,"%d+")
		    local utcMilsec = string.match(utcTime,"%d+%.(%d+)")
		    local utcMsec = tonumber(utcMilsec)/10
            if(math.ceil(utcMsec) ~= utcMsec ) then
                utcMsec = math.ceil(utcMsec) - 1
            end	
		    if(utcSec and utcMsec) then
	           return string.format("system set(-time,%s,%s)",utcSec,utcMsec)
		    end
   	  end
	end	
	]]
	return "."
end

function _callback_.Test_OnEntry(par)    --Initial function for startup test,you can add test initial code in here
	-- reset()
	-- time_utils.delay(500)

	--[[
	dut._CLear_Response_Buf_()
	relay(HWIO.RelayTable.DUT_ARM_UART)
	fixtureConsole._ARM_Send_Cmd_("datalogger open(ADC_NORMAL)")--open datalogger
	fixtureConsole._ARM_Send_Cmd_("codec open pdm(1000,3300)")--open datalogger
	local t = getUtcTime()
	if(t) then fixtureConsole._ARM_Send_Cmd_(t) end
	local sn, csn
	stationName = string.gsub(stationName, "%s+","_")
	if(stationInfo()=="SIPFIXTURE") then
		time_utils.delay(2000)  -- TODO: check if can use PRESS instead of IN/DOWN for automation control otherwise move delay to control board
		sn, csn = carrier()
		print(sn, csn)
		sipsn.mlbSN = sn
		sipsn.carrierSN = csn
	end
	-- CONFIG.ENGINE_LOG_FILENAME = string.format("%s/%s_%d-%s_Engine.txt",CONFIG.LOG_PATH,stationName,CONFIG.ID,os.date("%m-%d-%H-%M-%S",os.time()))
	CONFIG.ARM_DL_FILENAME = string.format("%s/%s_%d-%s_Datalogger.csv",CONFIG.LOG_PATH,stationName,CONFIG.ID,os.date("%m-%d-%H-%M-%S",os.time()))
	if(armdl) then armdl.start() end
	led({param1="LED_CTRL",param2="YELLOW_LED"})

	local enginelog = CONFIG.ARM_DL_FILENAME..","..tmStartLog.getPath("Calibration") .. "," .. tmStartLog.getPath(EngineLogName)
	local p = logpath()
	tmStartLog.write(EngineLogName,"< get log path from datalog > : "..tostring(p).."\n")
	if(p and p~="") then 
		p = p..","..enginelog
	else
		-- p = enginelog
		error("ERRCODE[NA]: Critical logs failed to initialize. Test aborted.")
	end

	return p, true
	--]]--
	return ".",true
end

function _callback_.Test_OnFail(par)   --Clear function for test failed,you can add clear function code in here when test failed.

end

function _callback_.Test_OnDone(par)   --Clear function for normal test finish.you can add clear function code in there when test normally finish.
	--[[
	fixtureConsole._ARM_Send_Cmd_("datalogger close()")
	fixtureConsole._ARM_Send_Cmd_("codec close pdm()")
	reset()
	if(armdl) then armdl.stop() end
	local enginelog = CONFIG.ARM_DL_FILENAME..","..tmStartLog.getPath("Calibration") .. "," .. tmStartLog.getPath(EngineLogName)
	local p = logpath()
	if(p and p~="") then 
		p = p..","..enginelog
	else 
		p = ""
	end

	if par and par.param1==1 then 
		led({param1="LED_CTRL",param2="GREEN_LED"})
	else
		led({param1="LED_CTRL",param2="RED_LED"})
	end

	return p, true
	--]]--
	return ".", true
end

function _callback_.Test_OnAbort(par)    --Clear function for normal test finish.you can add clear function code in there when test normally finish.

end

function _callback_.Step_Finish(par)
end

return _callback_
