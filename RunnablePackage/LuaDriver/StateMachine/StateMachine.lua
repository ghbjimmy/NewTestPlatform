
local zmq = require("lzmq")
local zpoller = require "lzmq.poller"
local zthreads = require "lzmq.threads"
local json = require "dkjson"
local lapp = require("pl.lapp")
local ztimer  = require "lzmq.timer"

require "pathManager"
package.path = package.path..";"..deleteLastPathComponent(CurrentDir()).."/Driver/?.lua"
local config_utils = require("utils.config_utils")
local fixturetype = require("functions.GhInfo").fixturetype()--need to distinguish Panel, SIP, or SipOptilcal

local args = lapp [[
X527 State Machine
    
    -c,--config     (default config.socket_zmq)   		Config module to load.
    -e,--enable     (default all) 						Enable slot to test(start from 0)
    -s,--slots		(default 6) 							set slots of the fixture
    <updates...>    (default "X=Y")                    	Series of X=Y pairs to update the CONFIG table with
]]

local sm = require "StateSwitch"
print(fixturetype)
args.fixture = fixturetype

local context = zmq.context()
zthreads.set_context(context)

local seqSubTable = {}--to detect SEQUENCE_END
local seqReqTable = {} -- to send start testing
local flag = {}
local testingFlag = false
local uutEnable = {}
local uutSN = {}
local poller = zpoller.new(args.slots)
local triggerRep = nil
local smEnd = {}
smEnd["function"]="smfinishtest"
local smStart = {}
smStart["function"]="smstarttest"
local xx={}
xx["status"] = "OK"
local ok = json.encode(xx)
local seqStartCmd = {
		params = "",
		jsonrpc = "1.0",
		id = "f143164097ff11e58c6d3c15c2dab3ba",
}
seqStartCmd["function"] = "run"

local loopTimes = 0
local stopLoopFlag = false
local currentLoopTimes = 0


local function checkFlag()
	for i=1, args.slots do
		if(flag[i]==false) then
			return false
		end
	end
	return true
end

local function clearFlag()
	for i,v in pairs(uutEnable) do
		if(v == true) then
			flag[i] = false -- uut enable, set flag to false, need to wait seq pub
		end
	end
end

local function clearSNTable()
	for i=1, args.slots do
		local  s = "sn"..tostring(i-1)
		uutSN[s] = nil
	end
end

local function getSN(index)
	local s = "sn"..tostring(index)
	return uutSN[s]
end

local function setSN(index,sn)
	local s = "sn"..tostring(index)
	if #tostring(sn) > 0 then
		uutSN[s] = sn
	end
end

function setZmq()
	local c, n, sc, err = nil,nil,nil,nil
	print("state machine",args.slots)
	for i=1, args.slots do
		args.uut = i-1
		c = config_utils.get_config(args)
	--Sequencer Publisher
		n = tostring(config_utils.get_addr(c, "STATEMACHINE_SUB", c.ID))
		print("< "..tostring(c.ID).." StateMachine > Sequencer SUB : ".. n)
		sc, err = context:socket{	zmq.SUB, connect = n, subscribe = ""}
		zmq.assert(sc, err)
		table.insert(seqSubTable, sc)
	--Sequencer cmd Req
		n = tostring(config_utils.get_addr(c, "STATEMACHINE_REQ", c.ID))
		print("< "..tostring(c.ID).." StateMachine > Sequencer REQ : ".. n)
		sc, err = context:socket{zmq.REQ, connect = n}
		zmq.assert(sc, err)
		table.insert(seqReqTable, sc)
		table.insert(flag, false)
		table.insert(uutEnable, true)
		setSN(args.uut, nil)
	end
	--StateMachine Trigger Rep, GUI, Fixture...
	args.uut=0
	c = config_utils.get_config(args)
	n = tostring(config_utils.get_addr(c, "STATEMACHINE_REP", c.ID))
	print("< "..tostring(c.ID).." StateMachine >Trigger REP : ".. n)
	triggerRep, err = context:socket{zmq.REP, bind = n}
	zmq.assert(triggerRep, err)
end

local function sendStartToSeq()
	print("\t< "..tostring(currentLoopTimes).." > Send Cmd to sequencer Start Test...")
	testingFlag = true
	sm.StateSwitch(sm.EVENT.START)
	for i=1, args.slots do--send start msg to sequencer
		if(uutEnable[i]) then
			local s = getSN(i-1)
			if(s) then 
				seqStartCmd.params = {{attributes={MLBSN=s}}}
			else 
				seqStartCmd.params = ""
			end
			print("< REQ send >",json.encode(seqStartCmd) )
			seqReqTable[i]:send(json.encode(seqStartCmd))
			local resp = seqReqTable[i]:recv()
			print("< REQ receive >", resp)
			if string.match(resp, "error") then--error occur, discard this slot to detect finished msg
				if uutEnable[i] == true then
					flag[i] = true
				end
			end
		end
	end
end

local function triggerMesseageHandle(cmd)
	local msg_obj, pos, err = json.decode(tostring(cmd))
	if err or msg_obj == nil then
		local t = {}
		t["error"] = "get error format msg:"..tostring(cmd)
		triggerRep:send(json.encode(t))
		return 0
	end
	if msg_obj["function"] == "LOOPBREAK" then
		stopLoopFlag = true
		triggerRep:send(ok)
	elseif testingFlag == true then
		triggerRep:send(json.encode({error="Testing"}))
	else
		print(msg_obj["function"])
		if msg_obj["function"] == "START" then--{function":"START"}
			-- triggerRep:send(ok)
			print(msg_obj["times"])
			if msg_obj.times then 
				print("loop")
				loopTimes = tonumber(msg_obj.times) 
				sm.setLoopTestFlag(true)--this flag is to control fixture
			else
				print("start")
				loopTimes = 1
				sm.setLoopTestFlag(false)
			end
			if(msg_obj.uutinfo) then
				for i=1,args.slots do
					uutEnable[i] = false
					flag[i] = true;
				end
				for i,v in ipairs(msg_obj.uutinfo) do
					uutEnable[i] = true
					flag[i] = false
					setSN(msg_obj.uutinfo.uut, msg_obj.uutinfo.sn)
				end
			end
			sendStartToSeq() --send start message to Sequencer
		elseif msg_obj["function"] == "UUTENABLE" then
			--{"enable":1,"uut":0,"function":"UUTENABLE"}
			local f,e = false, true
			local u = tonumber(msg_obj.uut)
			if tonumber(msg_obj.enable)<1 then f = true; e = false end--uut disable, flag set to true, dnot need to wait seq pub.
			flag[u+1] = f
			uutEnable[u+1] = e
			if (msg_obj.sn and #msg_obj.sn~=0) then setSN(msg_obj.uut,msg_obj.sn) end
		elseif msg_obj["function"] == "SETUUTSN" then
			--{"sn":"xxx","uut":0,"function":"SETUUTSN"}
			setSN(msg_obj.uut,msg_obj.sn)
		end
		triggerRep:send(ok)
	end
end

function addPoller()
	for i=1, args.slots do
		poller:add(seqSubTable[i], zmq.POLLIN, function()
  			local msg = seqSubTable[i]:recv()
  			-- print(i-1, "< Sub receive >", msg)
  			if string.match(msg,"\"event\": 1") then--"event": 4 ->"SEQUENCE_END"
  				print(string.match(msg,"\"event\": 1"))
  				flag[i] = true
  			end
  			if checkFlag() then
  				print("< End Test > testing finished ... \n")
  				sm.StateSwitch(sm.EVENT.FINISH)
  				sm.StateSwitch(sm.EVENT.WILL_UNLOAD)
  				sm.StateSwitch(sm.EVENT.REMOVED)
  				-- poller:stop()
  				clearFlag()
  				clearSNTable()
  				testingFlag = false
  				ztimer.sleep(1000)
  				currentLoopTimes = currentLoopTimes + 1
  				if(currentLoopTimes == loopTimes or stopLoopFlag==true) then
  					print("< Stop Test > testing finished ...\n < Wait > Wait for Trigger ...")
  					currentLoopTimes = 0;
  				else 
  					ztimer.sleep(5000)
  					print(string.format("Loop %d start...", currentLoopTimes))
  					sendStartToSeq()
  				end
  			end
		end)
	end
	poller:add(triggerRep,zmq.POLLIN, function()
		local cmd = triggerRep:recv()
		print("< REP Receive > : ",cmd)
		if(cmd) then 
			triggerMesseageHandle(cmd)
		end
	end)
end

local function printUUTEnable()
	for i=1, #uutEnable do
		io.write("UUT"..tostring(i-1).." > " .. tostring(uutEnable[i]).." \n")
	end
end

local function setDebug(indexTable)
	for i=1, #uutEnable do
		uutEnable[i]=false
		flag[i]=true
	end
	for i=1,#indexTable do
		if(indexTable[i]+1<=#uutEnable) then--start from 0,
			uutEnable[indexTable[i]+1]=true
			flag[indexTable[i]+1]=false
		end
	end
	printUUTEnable()
end

setZmq()
addPoller()
sm.fixtureConnect(args)

if(args.enable ~= "all") then
	local t = {}
	for v in string.gmatch(args.enable, "%d+") do
		print(v)
		t[#t+1]  =tonumber(v)
	end
	setDebug(t) --enable slot for debug
end

args.uut=0
local c = config_utils.get_config(args)
local SMHeadBeat, pipe = zthreads.fork(context, function(pipe, address)
        local zmq = require "lzmq"
        local zthreads = require "lzmq.threads"
        local context = zthreads.get_parent_ctx()
        local  t = require "lzmq.timer"

        print(" < StateMachine > Set HeartBeat PUB : ", address)
        local watchdog_zmq, err = context:socket(zmq.PUB, {bind = address})
        zmq.assert(watchdog_zmq, err)

        while(1) do
            t.sleep(5000)
            watchdog_zmq:send_all{"101",os.date("%m-%d %H:%M:%S",os.time()),3,"StateMachine","FCT_HEARTBEAT"}
        end
    end, config_utils.get_addr(c, "STATEMACHINE_HEARTBEAT", c.ID))
SMHeadBeat:start(true, false)

print("< StateMachine Started > : Wait for Trigger...")
poller:start()--poller receive Msg from Sequencer Pub


