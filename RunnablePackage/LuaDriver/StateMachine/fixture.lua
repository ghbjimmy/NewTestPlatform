local config_utils = require("utils.config_utils")
local zmq = require("lzmq")
local zpoller = require "lzmq.poller"
local zthreads = require "lzmq.threads"
local json = require "dkjson"
--require "pathManager"
require "PathManager_New"
--here, need to modify location.
local needZmq = false
local _fixture = {}
_fixture.sub = nil
_fixture.req = nil
_fixture.loopTest = false

function _fixture.connect(arg)
	if(string.upper(tostring(arg.fixture)) == "SIPFIXTURE") then
		needZmq = true
		arg.uut=0
		local c = config_utils.get_config(arg)
		local sub = tostring(config_utils.get_addr(c, "FIXTURE_CTRL_SUB", c.ID))
		local req = tostring(config_utils.get_addr(c, "FIXTURE_CTRL_REQ", c.ID))

		print("< Fixture Client > Set SUB : ".. sub)
		local context = zmq.context()
		_fixture.sub, err = context:socket{zmq.SUB, connect = sub, subscribe = ""}
		zmq.assert(_fixture.sub, err)

		print("< Fixture Client > Set REQ : ".. req)
		_fixture.req, err = context:socket{zmq.REQ, connect = req}
		zmq.assert(_fixture.req, err)
		_fixture.req:set_rcvtimeo(3000)
	end
end

local function sendMsg(msg)
	if(needZmq) then
		_fixture.req:send(msg..'\0')
		return _fixture.req:recv()
	else
		return nil
	end
end

function _fixture.SendCmd(msg)
	return sendMsg(msg)
end

function _fixture.close()
	sendMsg("in")
end

function _fixture.press()
	sendMsg("down")
end

function _fixture.release()
	print("< loop mode ? >", tostring(_fixture.loopTest))
	if _fixture.loopTest==true then
		sendMsg("out")
	else
		sendMsg("release")
	end
end

function _fixture.open()
	sendMsg("out")
end

function _fixture.fixtureid()
	local r = sendMsg("readid")
	return string.match(r, "ReadID[\s\t](.+)[\r\n])")
end

return _fixture
