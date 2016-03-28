#!/usr/bin/env lua
-- package.path = package.path..";".."./?.lua"

require("utils.zhelpers")
local json = require("dkjson")
local zmq = require("lzmq")
local zpoller = require("lzmq.poller")
local zthreads = require("lzmq.threads")
local config_utils = require("utils.config_utils")
local time_utils = require("utils.time_utils")
local lapp = require("pl.lapp")
local zassert = zmq.assert;

local SERVER_READY = "_my_rpc_server_is_ready"

local args = lapp [[
X527 Test Engine

This is the interface between the test sequencer and the test fixture HW and DUT.
It does all of the required limits checking and conditional execution. Options (such
as IP address, ports, log file names, etc.) are set in config files. These files
are, by default, stored in the config/ directory.
    
    -c,--config     (default config.socket_zmq)   Config module to load.
    -u,--uut        (default 1)                     UUT slot number (used for IP/Port selection)
    <updates...>    (default "X=Y")                    Series of X=Y pairs to update the CONFIG table with
]]

--default callback function
local function __OnRequest(patcher,...)
	print("Get Request:",...)
	return "OK"
end

--dispatcher class
local dispatcher={cat="",RequestCB=__OnRequest};
function dispatcher:new(o)
	o=o or {};
	setmetatable(o,self);
	self.__index=self;
	return o;
end





function dispatcher:connect(pub,rep)
	self.pub_address = pub;
	self.rep_address = rep;

	--create publihser
	local ctx_pub = zmq.context();
	local pub_zmq,err = ctx_pub:socket(zmq.PUB,{bind = pub})
	if (not pub_zmq) then
		return false,err;
	else
		self.pub_zmq = pub_zmq
	end
	
	--create subscriber
	self.rep_adderss = rep
	local ctx_rep = zmq.context();
	local rep_zmq, err = ctx_rep:socket(zmq.REP, {bind = rep})
	if (not rep_zmq) then
		return false,err;
	else
		self.rep_zmq = rep_zmq;
	end
	
	print(time_utils.get_time_string_ms(),"[PUBLISHER] : Create publisher server:",pub);
	print(time_utils.get_time_string_ms(),"[REPLY] : Create reply server:",rep);
	
	self.poller = zpoller.new(1)
	self.poller:add(self.rep_zmq, zmq.POLLIN,function()
		local ret = self.RequestCB(self,self.rep_zmq:recvx());
		self.rep_zmq:send(ret);
	end);
	return true,"OK"
end


function dispatcher:publisher(...)
	self.pub_zmq.sendx(...);
end

function dispatcher:setcat(cat)
	if cat then
		self.cat = cat;
	end
end

--set reply message callback funtion
function dispatcher:SetRequestCB(fn)
	self.RequestCB = fn;
end

function dispatcher:runloop()
	self.poller:start();
end

function dispatcher:recvx()
	return self.rep_zmq:recvx();
end

do return dispatcher end

--debug message
local d = dispatcher:new();
for v in pairs(d) do
	print(v)
end
ret,err = d:connect("tcp://*:6600","tcp://*:9999");
print(ret,err)
assert(ret,err);
print("running")
d.poller:start();
