#!/usr/bin/env lua
-- package.path = package.path..";".."./?.lua"

local __version = "1.0.2"

require("utils.zhelpers")
local json = require("dkjson")
local zmq = require("lzmq")
local zpoller = require("lzmq.poller")
local zthreads = require("lzmq.threads")
local config_utils = require("utils.config_utils")
local time_utils = require("utils.time_utils")
local lapp = require("pl.lapp")

local dispatcher = require "engine.engine_dispatcher"

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

print("************************************************************")
print("                    Test Engine",__version					)
print("************************************************************")

--===========================--
-- Global (!!!) config variable
--===========================--
CONFIG = config_utils.get_config(args)

--===========================--
-- Setup ZMQ sockets
--===========================--
local pub_address = config_utils.get_addr(CONFIG, "RESULTS", CONFIG.ID);
local rep_address = config_utils.get_addr(CONFIG, "SEQUENCE", CONFIG.ID);
local ret,err = dispatcher:connect(pub_address,rep_address)
---[[
--===========================--
-- Setup Fixture / DUT sockets
--===========================--

local fixture_console = require(CONFIG.FIXTURE_CONSOLE)
--fixture_console._Arm_Socket_Connect_()

--require "calibration"
local dut_console = require("tcpAndZmq")
dut_console._DUT_Socket_Connect_()
--]]

--===========================--
-- Global loggers
--===========================--
local TestFlowOut = config_utils.get_log_writer(CONFIG, "FLOW", CONFIG.log_prefix)

--===========================--
-- Global Variable Mangement
--===========================--
local global_var_table = {} -- Global variable table for conditional execution
local global_data = require ("utils.global_data") -- global data mangement library
global_data.set_logger(config_utils.get_log_writer(CONFIG, "VAR", CONFIG.log_prefix))

--===========================--
-- Function dispatch
--===========================--
local dispatch_table = require("function_table") -- name / function() mapping. Must be called after loggers are set up?

--Add server ready function to function table
local function Server_Ready()
	print("Test Engine Ready")
	return true,"true";
end
dispatch_table[SERVER_READY] = Server_Ready;

local dispatch_function = function (sequence_obj)
    local skipped = true
    local value, passing= nil, nil

    -- Get the function from the dispatch table
    local f = dispatch_table[sequence_obj["function"]]

    if f == nil then
        error("Function '"..tostring(sequence_obj["function"]).."' is not supported.")
    end

    -- If we are conditional, check with the global variable table
    if sequence_obj.globalKey == "" or sequence_obj.globalKey == nil 
            or global_data.compare_from_param(global_var_table, sequence_obj.globalKey, sequence_obj.globalValue) then
        skipped = false
        value, passing = f(sequence_obj, global_var_table)
    end

    -- Return the results
    return value, passing, skipped
end

--===========================--
-- Sequencer Loopback
--===========================--
--zthreads.set_context(zmq.context())
context =zmq.context()
local watch_dog_addr = config_utils.get_addr(CONFIG, "WATCHDOG", CONFIG.ID)
local watchdog_thread, watchdog_pipe = zthreads.fork(context, function(pipe, address)
        local zmq = require "lzmq"
        local zthreads = require "lzmq.threads"
        local sk = require "socket"
        local context = zthreads.get_parent_ctx()
        require("utils.zhelpers")

        local watchdog_zmq, err = context:socket(zmq.PUB, {connect = address})
        zassert(watchdog_zmq, err)

        local allowed_delays = 4
        local delays = 0

        while true do
            if not pipe:poll(2500) then
                delays = delays + 1
                if delays <= allowed_delays then
                    zassert(watchdog_zmq:send("EXECUTOR:PONG"))
                    print("Sent Watchdog")
                end
            else
                local msg = pipe:recv()
                if tostring(msg) == "DIE" then
                    break
                elseif tostring(msg) == "ALIVE" then
                    delays = 0
                    allowed_delays = 4
                elseif string.match(tostring(msg), "TIMEOUT:") then
                    local timeout = tonumber(string.match(tostring(msg),"TIMEOUT:(.*)"))
                    if timeout == nil then timeout = 7500 end
                    allowed_delays = (timeout / 2500) + 1
                end
            end
        end
        print("Watch Dog Stopped");
        sk.sleep(10);
    end, watch_dog_addr)
watchdog_thread:start(true, false)


--===========================--
-- Watchdog looper thread
--===========================--
--Request callback function,when get message in dispatcher reply server,will call this function
local function OnRequest(dispatcher,...)
	print (" < Received > ",...)--"{"jsonrpc":"V0.1","id":"1",fucntion": "relay","params":"p1,p2,timeout"}"  unit???
    local msg = {...}
    msg = table.concat(msg,"");
    
	local sendToSeqMsg="";
    
    local msg_obj, pos, err = json.decode (msg, 1, nil)
    -- If we have a JSON error, return ERROR and bail
    if err or msg_obj == nil then
        sendToSeqMsg = json.encode({status = "error", error = "json", tid = "nil"})
        if (TestFlowOut) then 
        	TestFlowOut.write("\n==TEST: Unknown\n\tERROR: JSON\n") 
        end
    else
        local result_value, sw_status = nil, nil
        local result = "FAIL"
        local t1 = time_utils.get_unix_time_ms()

        -- Setup watchdog for predicted timeout
        watchdog_pipe:send("TIMEOUT:"..tostring(msg_obj.timeout))

        -- Populate the Flowlog
        if (TestFlowOut) then 
            TestFlowOut.write(string.format("\n==TEST: %s\n", tostring(msg_obj["tid"])))
            -- TestFlowOut.write(string.format("\tDESC: %s\n", tostring(msg_obj["description"])))
        end
        if(msg_obj.params) then
            msg_obj.param1=msg_obj.params[1]
            msg_obj.param2=msg_obj.params[2]
            if (not msg_obj) then
            	msg_obj.unit=msg_obj.params[3]
            end
            if(#tostring(msg_obj.params[2])<4) then
                if (string.match(string.upper(tostring(msg_obj.params[2])),"V") or
                    string.match(string.upper(tostring(msg_obj.params[2])),"A") or
                    string.match(string.upper(tostring(msg_obj.params[2])),"HZ")
                    ) 
                then
                    msg_obj.unit=msg_obj.params[2]
                end
            end
            --when unit exist, parma2 must exist
            msg_obj.timeout=msg_obj.timeout
        else
            msg_obj.param1=nil
            msg_obj.param2=nil
            msg_obj.unit=nil
            msg_obj.timeout=3000
        end
        -- Send sequence to the action dispatch thread, and catch errors
        local dispatch_status, result_value, passfail, skipped = pcall(dispatch_function, msg_obj)
        print("Dispatch :",dispatch_status, result_value, passfail, skipped)
        
        if dispatch_status then
        	if result_value==true then
            	result_value = "--PASS--"
	        elseif result_value == false then
            	result_value = "--FAIL--"
        	end
        
        	if (dispatch_function == Server_Ready) then
        		result_value = "true"
        	end
            sendToSeqMsg = json.encode({jsonrpc = msg_obj.jsonrpc, id = msg_obj.id, result = result_value})
        else
            local errcode = string.match(result_value, "ERRCODE%[(.-)%]")
            local errmsg = string.match(result_value, "ERRCODE%[.-%](.+)")
            if (not errmsg) then
            	errcode = -999;
            	errmsg = result_value
            end
            sendToSeqMsg = json.encode({jsonrpc = msg_obj.jsonrpc, id = msg_obj.id,  error = {code=errcode,message=errmsg}})
        end
        
        TestFlowOut.write(" < Result > "..sendToSeqMsg.."\n")
        TestFlowOut.flush()
        -- If we Pass, celebrate!
        if passfail then
            result = "PASS"
        end
    end

    -- Tell the watchdog that we are alive and running sequence items
    watchdog_pipe:send("ALIVE")
    
    print(" < Result > ",sendToSeqMsg,"\n")
    return sendToSeqMsg;
end

--Set request callback
dispatcher:SetRequestCB(OnRequest);

-- Process messages from both sockets
print("Running.")
dispatcher:runloop();
print("Done.")