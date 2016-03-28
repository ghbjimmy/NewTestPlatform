local fixture = require "fixture"

require "pathManager"
package.path = package.path..";"..deleteLastPathComponent(CurrentDir()).."/Driver/?.lua"
local config_utils = require("utils.config_utils")

local json = require "dkjson"
local zmq = require("lzmq")
local zthreads = require "lzmq.threads"
require("utils.zhelpers")

local sm_pub;
local context = zmq.context()
-- zthreads.set_context(context)

local _State = {}
_State.EVENT = {
	ABORT = "ABORT",
	LOADED = "UUT_LOAD",
	START = "TEST_START",
	FINISH = "TEST_FINISH",
	ERROR = "ERROR",
	WILL_UNLOAD = "UUT_WILL_UNLOAD",
	REMOVED = "UUT_REMOVED"
}

_State.STATE = {
	IDLE = "SM_IDLE",
	LOADED = "SM_UUT_LOADED",
	TESTING = "SM_TESTING",
	DONE ="SM_TESTING_DONE",
	UNLOAD = "SM_UUT_UNLOADED",
	ERROR = "SM_ERROR"
}

function _State.setLoopTestFlag(par)
	print("set loop test flag to : ", tostring(par))
	fixture.loopTest = par
end

function _State.fixtureConnect(arg)
	fixture.connect(arg)

	c = config_utils.get_config(arg)
	n = config_utils.get_addr(c, "STATEMACHINE_PUB", c.ID)
	print("< StateMachine Set PUB > : ".. n)
	sm_pub, err = context:socket(zmq.PUB, {bind=n})
	zassert(sm_pub,err)
end

local function pubMsg(par)
	local t = {}
	t["function"] = tostring(par)
	local f = json.encode(t)
	print(par, sm_pub:send(f))
end

_State.state = _State.STATE.IDLE;

function _State.StateSwitch(event)
	if _State.state == _State.STATE.IDLE then
		if event == _State.EVENT.LOADED then
			fixture.close()
			_State.state = _State.STATE.LOADED
			pubMsg(_State.STATE.LOADED)
		elseif event == _State.EVENT.START then
			fixture.close()
			fixture.press()
			_State.state = _State.STATE.TESTING
			pubMsg(_State.STATE.TESTING)
		end
	elseif _State.state == _State.STATE.LOADED then
		if event == _State.EVENT.START then
			fixture.close()
			fixture.press()
			_State.state = _State.STATE.TESTING
			pubMsg(_State.STATE.TESTING)
		elseif event == _State.EVENT.ABORT then
			_State.state = _State.STATE.IDLE
			pubMsg(_State.STATE.IDLE)
		end
	elseif _State.state == _State.STATE.TESTING then
		if event == _State.EVENT.FINISH then
			fixture.release()
			_State.state = _State.STATE.DONE
			pubMsg(_State.STATE.DONE)
		end
	elseif _State.state == _State.STATE.DONE then
		if event == _State.EVENT.WILL_UNLOAD then
			fixture.open()
			_State.state = _State.STATE.UNLOAD
			pubMsg(_State.STATE.UNLOAD)
		elseif event == _State.EVENT.START then
			fixture.press()
			_State.state = _State.STATE.TESTING
			pubMsg(_State.STATE.TESTING)
		end
	elseif _State.state == _State.STATE.UNLOAD then
		if event == _State.EVENT.REMOVED then
			_State.state = _State.STATE.IDLE
			pubMsg(_State.STATE.IDLE)
		elseif event == _State.EVENT.ABORT then
			fixture.close()
			_State.state = _State.STATE.DONE
			pubMsg(_State.STATE.DONE)
		end
	elseif _State.state == _State.STATE.ERROR then
		pubMsg(_State.STATE.ERROR)
	end
	return _State.state
end

function _State.SendCmd(cmd)
	return fixture.SendCmd(cmd)
end
return _State