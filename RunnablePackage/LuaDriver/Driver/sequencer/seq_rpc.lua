local zmq = require "lzmq"
local zassert = zmq.assert;
local zpoller = require("lzmq.poller")
local zthreads = require("lzmq.threads")
local times = require "utils.time_utils"
local message = require "sequencer.message"
local json = require "dkjson"
local config_utils = require("utils.config_utils")

local rpc={}

local ctx_reply,reply_zmq;				--reply zmq to respone remote action
local ctx_pub,pub_zmq;					--PUB zmq to publisher message
local ctx_sequence,sequence_zmq;		--request zmq to request test result from engine
local err;
local reply_function;					--reply callback function


rpc.poller = zpoller.new(3)
--===========================--
-- Setup Sequence socket connect to engine
--===========================--
local function CreateSeqZMQ()
	ctx_sequence = zmq.context()
	--zthreads.set_context(context)
	
	--Create zmq connect to engine.
	local server_ip = config_utils.get_addr(CONFIG, "SEQUENCE", CONFIG.ID);
	
	sequence_zmq, err = ctx_sequence:socket(zmq.REQ, {connect = server_ip})
	zassert(sequence_zmq, err)
	
	print("[Sequencer : ] : Create REQ,connnect to :",server_ip);
	
	function DoStep(item)
		local ret,err= sequence_zmq:send(item);
		assert(ret,err);
		local msg = sequence_zmq:recv();	--return
	end
end

function rpc.send_to_engine(arg)
	local msg
	if (type(arg)=="table") then
		msg = json.encode(arg)
	else
		msg = tostring(arg);
	end
	
	assert(sequence_zmq:send(msg))
	return sequence_zmq:recv();
end

--===========================--
-- Setup REP socket to reply remote action
--===========================--
local function CreateReply()
	
	ctx_reply = zmq.context()
	--zthreads.set_context(context)
	
	--Create zmq connect to engine.
	local reply_ip = "tcp://127.0.0.1:6401"
	
	reply_zmq, err = ctx_reply:socket(zmq.REP, {connect = reply_ip})
	zassert(reply_zmq, err)
	print("[Sequencer : ] : Create reply server:",reply_ip);
end

local function OnRequest()
	local msg = reply_zmq:recv();
	if (not reply_function) then
		log("Discard REQUEST:",msg);
		zassert(reply_zmq:send("[Sequencer]Discard"));
	else
		local ret = reply_function(msg);
		zassert(reply_zmq:send(ret));
	end
end

--===========================--
-- Setup PUB socket to publisher message
--===========================--
local function CreatePublisher()
	local pub_ip="tcp://127.0.0.1:6501"
	local ctx_pub = zmq.context();
	local pub_zmq,err = ctx_pub:socket(zmq.PUB,{bind = pub_ip})
	zassert(pub_zmq,err);
	print("[Sequencer : ] : Create publisher server: ",pub_ip);
	
	local function PublisherMsg(msg)
		assert(pub_zmq:send(msg))
		print("[Sequencer : ] Send Message:",msg);
	end
end

local function log(...)
	print(times.get_time_string_ms(),...);
end


function rpc.initial()
	CreateSeqZMQ();
	CreateReply();
	CreatePublisher();
end

function rpc.SetRequestCallBack(fun)
	assert(fun,"callback function can't set as nil!")
	reply_function = fun;
end


function rpc.start()
	assert(reply_function,"action reply function is nil,should set firstly");
	rpc.poller:add(reply_ip, zmq.POLLIN, OnRequest);
end

function rpc.publish_msg(arg)
	local msg;
	if (type(arg)=="table") then
		msg = json.encode(arg);
	else
		msg = tostring(args);
	end
	assert(ctx_pub:send(msg));
	log("Publisher message : ",msg);
end


--===========================--
-- Publisher Message Interface
--===========================--
function rpc.ShowTestStart()
	local arg={};
	arg.cmd = message.TEST_ON_START;
	local msg = json.encode(arg);
	rpc.publish_msg(msg);
end

function rpc.ShowTestStop()
	local arg={};
	arg.cmd = message.TEST_ON_STOP;
	arg.err = "stop"
	rpc.publish_msg(arg);
end

function rpc.ShowTestError(err)
	local arg={};
	assert(err,"invalid argument,shoule set the err message");
	arg.cmd = message.TEST_ON_ERROR;
	arg.err = arg;	--error message
	rpc.publish_msg(arg);
end

function rpc.ShowTestFinished(state)
	local arg={};
	assert(state,"invalid argument,shoule set the test state");
	arg.cmd = message.TEST_ON_FINISHED;
	arg.state = state;
	rpc.publish_msg(arg);
end

function rpc.ShowItemStart(index)
	local arg={};
	arg.cmd = message.TEST_ON_ITEM_START;
	arg.index=index;
	rpc.publish_msg(arg);
end

function rpc.ShowItemFinished(index,state,skipped)
	local arg = {};
	arg.cmd = message.TEST_ON_ITEM_FINISHED;
	arg.index = index;
	arg.state = state;
	rpc.publish_msg(arg);
end

function rpc.ShowTestPause(arg)
	arg.cmd = message.TEST_ON_PAUSE;
	local msg = json.encode(arg);
	rpc.publish_msg(msg);
end

function rpc.ShowTestResume(arg)
	arg.cmd = message.TEST_ON_RESUME;
	local msg = json.encode(arg);
	rpc.publish_msg(msg);
end

return rpc