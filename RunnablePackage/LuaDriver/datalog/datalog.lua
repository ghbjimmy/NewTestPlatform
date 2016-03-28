#!/usr/bin/env lua
package.path = package.path.."./?.lua"

socket = require "socket"
zmq = require "lzmq"
json = require "dkjson"
lapp = require "pl.lapp"
zpoller = require("lzmq.poller")
zthreads = require("lzmq.threads")
llthreads = require"llthreads"

require "public"
require "subscriber"
require "hw"
require "uart"
require "flow"
require "csv"

require "pathManager"
package.path = package.path..";"..deleteLastPathComponent(CurrentDir()).."/Driver/?.lua"
ghinfo = require "functions.GhInfo"

instance_id = 0
context = zmq.context()
poller = zpoller.new(4)
datalog_mlbsn = ""

local args = lapp [[tool subscriber:
    -i,--id        (default 0)                     instance id
    ]]

for k,v in pairs(args) do
	if k=="id" then
		if instance_id then instance_id = tonumber(v) end
	end
end
print("site id: ", instance_id)

station_type = ghinfo.GhStation()
station_name = ghinfo.GhStationName()
print("station_type=", station_type)
print("station_name=", station_name)

--csv subscriber
csv_sub = LogSubscriber:new({port=6250,})
function csv_sub:cleanup()
	self.sock:close()
end
function csv_sub:connect()
	self.sock = context:socket(zmq.SUB) 
	--self.sock:set_identity("xxx")--not mandatory
	assert(self.sock:set_subscribe("101")) --if needed
	assert(self.sock:set_rcvtimeo(0))
	--assert(self.sock:connect("tcp://127.0.0.1:6250"))
	local p = string.format("%s:%d",self.url,self.port+instance_id)
	assert(self.sock:connect(p))
	print("connect to ", p)
	socket.sleep(0.01)
end
function csv_sub:recv()
	return self.sock:recv_all()	
end


--hw subscriber
hw_sub = LogSubscriber:new({port=6990,})
function hw_sub:cleanup()
	self.sock:close()
end
function hw_sub:connect()
	self.sock = context:socket(zmq.SUB) 
	--self.sock:set_identity("xxx")--not mandatory
	assert(self.sock:set_subscribe("")) --if needed
	assert(self.sock:set_rcvtimeo(0))
	--assert(self.sock:connect("tcp://127.0.0.1:6250"))
	local p = string.format("%s:%d",self.url,self.port+instance_id)
	assert(self.sock:connect(p))
	print("connect to ", p)
	socket.sleep(0.01)
end
function hw_sub:recv()
	return self.sock:recv()	
end


--uart subscriber
uart_sub = LogSubscriber:new(
{	
	port=6800,
})
function uart_sub:cleanup()
	self.sock:close()
end
function uart_sub:connect()
	self.sock = context:socket(zmq.SUB) 
	--self.sock:set_identity("xxx")--not mandatory
	assert(self.sock:set_subscribe("")) --if needed
	assert(self.sock:set_rcvtimeo(0))
	--assert(self.sock:connect("tcp://127.0.0.1:6250"))
	local p = string.format("%s:%d",self.url,self.port+instance_id)
	assert(self.sock:connect(p))
	print("connect to ", p)
	socket.sleep(0.01)
end
function uart_sub:recv()
	return self.sock:recv()	
end

local sn_url = string.format("tcp://%s:%d","127.0.0.1",6330+instance_id)


--[[
--change method to get mlbsn from sequencer event 4 mlbsn_verify
print("connect to ", sn_url)
sn_sock, err = context:socket(zmq.REQ, {connect=sn_url})--6300
sn_sock:set_rcvtimeo(0)
function read_mlbsn()
	local t={}
	t["function"]="get"
	sn_sock:send(json.encode(t))
	local msg = sn_sock:recv()
	if msg then
		local msg_obj, pos, err = json.decode (msg, 1, nil)
		local sn = msg_obj["sn"]
		datalog_mlbsn = tostring(sn)
	end	
end
--]]


local context_rep = zmq.context()
local datalog_rep, err = context_rep:socket(zmq.REP, {bind=string.format("tcp://%s:%d","127.0.0.1",7990+instance_id)})
print("connected to ",string.format("tcp://%s:%d","127.0.0.1",7990+instance_id))
socket.sleep(0.1)

local function cleanup()
	csv_sub:cleanup()
	hw_sub:cleanup()
	uart_sub:cleanup()
	context:destroy()
end


-------------------------------
-- heart beat...
-------------------------------
local heatbeat_thread_code = 
[[
local socket = require "socket"
zmq = require "lzmq"
local context = zmq.context()
local instance_id = ...
local hb_req_url = string.format("tcp://%s:%d","127.0.0.1",7990+instance_id)
local hb_req, err = context:socket(zmq.REQ, {connect=hb_req_url})
hb_req:set_rcvtimeo(0)
print("connect to ", hb_req_url)
socket.sleep(0.5)

local hb_pub_port_base = 6780
local hb_req_url = string.format("tcp://%s:%d","127.0.0.1",hb_pub_port_base+instance_id)
local heartbeat_pub, err = context:socket(zmq.PUB, {bind = hb_req_url})
zmq.assert(heartbeat_pub, err)
print("bind to ", hb_req_url)
socket.sleep(0.5)

local function check_main_thread()
	hb_req:send("hi")
	local msg = hb_req:recv()
	if msg=="hello" then
		--pub heartbeat
		heartbeat_pub:send_all{"101",os.date("%m-%d %H:%M:%S",os.time()),3,"Datalog", "FCT_HEARTBEAT"}
	end	
end

local startTime = 0
while true do
	check_main_thread()
	socket.sleep(5)
end
]]
local thread = llthreads.new(heatbeat_thread_code, instance_id)
assert(thread:start(true))



csv_log:init(poller, csv_sub)
hw_log:init(poller, hw_sub)
uart_log:init(poller, uart_sub)
flow_log:init()

poller:add(datalog_rep, zmq.POLLIN, function()
		local msg = datalog_rep:recv()
		-- print("<Rep Recv> ", msg)
		if string.match(tostring(msg),"get_path") then
			local t={}
			t["csv_log"]=csv_log.file_name
			t["hw_log"]=hw_log.file_name
			t["uart_log"]=uart_log.file_name
			t["flow_log"]=flow_log.file_name
			local plain_flow_log = ""
			if flow_log.file_name and flow_log.file_name~="" then
				plain_flow_log = string.gsub(flow_log.file_name, "flow", "flow_plain")
			end
			t["plain_log"]=plain_flow_log
			-- print("< send > : ",json.encode(t))
			datalog_rep:send(json.encode(t))
		else
			datalog_rep:send("hello")
		end
	end
	)
	
poller:start()
print("data log stopped")