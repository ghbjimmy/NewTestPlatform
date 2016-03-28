require("utils.zhelpers")
local json = require("dkjson")
local zmq = require("lzmq")
local zpoller = require("lzmq.poller")
local zthreads = require("lzmq.threads")
local config_utils = require("utils.config_utils")
local time_utils = require("utils.time_utils")
local lapp = require("pl.lapp")
require "dkjson"

print("Dummy Sequencer start...")


local args = lapp [[
X527 Test Engine

This is the interface between the test sequencer and the test fixture HW and DUT.
It does all of the required limits checking and conditional execution. Options (such
as IP address, ports, log file names, etc.) are set in config files. These files
are, by default, stored in the config/ directory.
    
    -c,--config     (default config.pathfinder)   Config module to load.
    -u,--uut        (default 1)                     UUT slot number (used for IP/Port selection)
    <updates...>    (default "X=Y")                    Series of X=Y pairs to update the CONFIG table with
]]

--===========================--
-- Global (!!!) config variable
--===========================--
CONFIG = config_utils.get_config(args)

local db_parser = require"sequencer.sqlite2lua"
--===========================--
-- Setup ZMQ sockets
--===========================--
local context = zmq.context()
zthreads.set_context(context)


local server_ip = config_utils.get_addr(CONFIG, "SEQUENCE", CONFIG.ID);
print("< " ..tostring(CONFIG.ID).. " > Connnect ZMQ REQUEST :",server_ip);
local sequence_zmq, err = context:socket(zmq.REQ, {connect = server_ip})
zassert(sequence_zmq, err)


print(json.encode({status = "error", error = "json", tid = "nil"}))


local cmd = [[{"function":"diags"}]]

local db_file = "development.sqlite3"
local items = db_parser.ParseDB(db_file)
-- db_parser.printItems(items)

local function DoItems(it)
	local t = os.time()
	for i =1, #it do
		item = it[i]
		if(item.sub and type(item.sub)=="table") then
			for j=1, #item.sub do
				-- cmd=json.encode(item.sub[j])
				local t = {}
				t.jsonrpc="V0.1"
				t.id = item.sub[j].tid
				t["function"]=item.sub[j]["function"]
				t.params={}
				t.params[1]=item.sub[j].param1
				t.params[2]=item.sub[j].param2
				t.params[3]=item.sub[j].unit
				t.params[4]=item.sub[j].timeout
				cmd = json.encode(t)
				print("send : ",sequence_zmq:send(cmd),cmd)
				print("receive : ",sequence_zmq:recv())
			end
		end
	end
	print("\r\n","ex :"..tostring((os.time()-t)/1000))
end

DoItems(items)
-- print("send : ",sequence_zmq:send(cmd))
-- print("receive : ",sequence_zmq:recv())