

local zmq = require("lzmq")
local zpoller = require "lzmq.poller"
local zthreads = require "lzmq.threads"
local json = require "dkjson"
local lapp = require("pl.lapp")
local ztimer  = require "lzmq.timer"

require "pathManager"
package.path = package.path..";"..deleteLastPathComponent(CurrentDir()).."/Driver/?.lua"
local config_utils = require("utils.config_utils")

local args = lapp [[
X527 State Machine Test
    
    -c,--config     (default config.socket_zmq)   Config module to load.
    -e,--enable     (default all)  enable slots
    <updates...>    (default "X=Y")                    Series of X=Y pairs to update the CONFIG table with
]]
local cfg = dofile(deleteLastPathComponent(CurrentDir()).."/StartUp.su")
args.slots=cfg.slot

local context = zmq.context()
zthreads.set_context(context)

local seqPubTable = {}--to detect SEQUENCE_END
local seqRepTable = {} -- to send start testing
local poller = zpoller.new(args.slots)
local flag = {}
local uutEnable = {}
local c, n, sc, err = nil,nil,nil,nil

for i=1, args.slots do
	args.uut = i-1
	c = config_utils.get_config(args)
	n = tostring(config_utils.get_addr(c, "STATEMACHINE_SUB", c.ID))
	print("< "..tostring(c.ID).." StateMachine > Set PUB : ".. n)
	sc, err = context:socket{	zmq.PUB, bind = n}
	zmq.assert(sc, err)
	table.insert(seqPubTable, sc)

	n = tostring(config_utils.get_addr(c, "STATEMACHINE_REQ", c.ID))
	print("< "..tostring(c.ID).." StateMachine > Set REP : ".. n)
	sc, err = context:socket{zmq.REP, bind = n}
	zmq.assert(sc, err)
	table.insert(seqRepTable, sc)
	table.insert(flag, false)
	table.insert(uutEnable, true)
end

local function pubMsg()

end

local function checkFlag()
	for i=1, args.slots do
		if(flag[i]==false) then
			return false
		end
	end
	return true
end

local function clearFlag()
		for i=1,args.slots do
			if(uutEnable[i]==true) then
				flag[i]=false
			end
		end
	-- end
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
end

for i=1, args.slots do
	poller:add(seqRepTable[i], zmq.POLLIN, function()
		-- if(uutEnable[i]) then
  			local msg = seqRepTable[i]:recv()
  			print(os.time(),i, "< receive >", msg)
  			seqRepTable[i]:send("xxx")
  			if string.match(msg,"run") then
  				flag[i] = true
  			end
  		-- end
  			if checkFlag() then
  				print("poller:stop()")
  				poller:stop()
  			end
	end)
end


local cmd = {"SEQUENCE_START","ATTRIBUTE_FOUND","ITEM_START","ITEM_END","SEQUENCE_END","\"event\": 4"}

if(args.enable ~= "all") then
	local t = {}
	for v in string.gmatch(args.enable, "%d+") do
		t[#t+1]  =tonumber(v)
	end
	setDebug(t) --enable slot for debug
end
while(1) do
	print("wait run trigger...")
	poller:start()
	while(1) do
		if checkFlag() then
			clearFlag()
			break
		else
			ztimer.sleep(500)
		end
	end
	print("dummy testing and pub msg...")
	for i=1, #cmd do
		for j=1, args.slots do
			if(uutEnable[j]) then
				print(j-1,"< Pub >",cmd[i])
				seqPubTable[j]:send(cmd[i])
				ztimer.sleep(5)
			end
		end
	end
end