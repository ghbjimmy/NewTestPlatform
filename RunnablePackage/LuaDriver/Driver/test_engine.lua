#!/usr/bin/env lua
package.path = package.path..";".."/Users/Ryan/Project/TM_Platform_v2/LuaDriver/Driver/?.lua"

require("utils.zhelpers")
local json = require("dkjson")
local zmq = require("lzmq")
local zpoller = require("lzmq.poller")
local zthreads = require("lzmq.threads")
local config_utils = require("utils.config_utils")
local time_utils = require("utils.time_utils")
local lapp = require("pl.lapp")


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

--===========================--
-- Global (!!!) config variable
--===========================--
CONFIG = config_utils.get_config(args)

--===========================--
-- Setup ZMQ sockets
--===========================--
local context = zmq.context()
zthreads.set_context(context)

local sequence_zmq, err = context:socket(zmq.REP, {bind = config_utils.get_addr(CONFIG, "SEQUENCE", CONFIG.ID)})
zassert(sequence_zmq, err)

local results_zmq, err = context:socket(zmq.PUB, {bind = config_utils.get_addr(CONFIG, "RESULTS", CONFIG.ID)})
zassert(results_zmq, err)
print("< "..tostring(CONFIG.ID).." test_engine.lua > Connect SEQUENCE ZMQ REP "..tostring(config_utils.get_addr(CONFIG, "SEQUENCE", CONFIG.ID)))
print("< "..tostring(CONFIG.ID).." test_engine.lua > Connect RESULTS ZMQ PUB "..tostring(config_utils.get_addr(CONFIG, "RESULTS", CONFIG.ID)))
---[[
--===========================--
-- Setup Fixture / DUT sockets
--===========================--
local dut_console = require(CONFIG.DUT_UART_CONSOLE)
dut_console._DUT_Socket_Connect_()

local fixture_console = require(CONFIG.FIXTURE_CONSOLE)
fixture_console._Arm_Socket_Connect_()
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
-- Watchdog looper thread
--===========================--
local watchdog_thread, watchdog_pipe = zthreads.fork(context, function(pipe, address)
        local zmq = require "lzmq"
        local zthreads = require "lzmq.threads"
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
    end, config_utils.get_addr(CONFIG, "WATCHDOG", CONFIG.ID))
watchdog_thread:start(true, false)

--===========================--
-- Sequence poller
--===========================--
local poller = zpoller.new(1)
poller:add(sequence_zmq, zmq.POLLIN, function() 
    local msg = sequence_zmq:recv()
    print ("Received " .. tostring(msg))--"{"jsonrpc":"V0.1","id":"1",fucntion": "relay","params":"p1,p2,timeout"}"  unit???
    local msg_obj, pos, err = json.decode (msg, 1, nil)
    
    -- print("Process sequence item.")

    -- If we have a JSON error, return ERROR and bail
    if err or msg_obj == nil then
        sequence_zmq:send(json.encode({status = "error", error = "json", tid = "nil"}))
        if (TestFlowOut) then TestFlowOut.write("\n==TEST: Unknown\n\tERROR: JSON\n") end
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
            msg_obj.unit=msg_obj.params[3]
            msg_obj.timeout=msg_obj.params[4]
        else
            msg_obj.param1=nil
            msg_obj.param2=nil
            msg_obj.unit=nil
            msg_obj.timeout=5000
        end
        -- Send sequence to the action dispatch thread, and catch errors
        local dispatch_status, result_value, passfail, skipped = pcall(dispatch_function, msg_obj)
        if skipped == nil then skipped = false end
        -- Check and see if we have any SW errors
        local sendToSeqMsg;
        if dispatch_status then
            sendToSeqMsg = json.encode({jsonrpc = msg_obj.jsonrpc, id = msg_obj.id, result = result_value})
        else
            sendToSeqMsg = json.encode({jsonrpc = msg_obj.jsonrpc, id = msg_obj.id,  error = {code=1,message=result_value}})
            sw_status = result_value
            result_value = "N/A"
        end
        print(" < Result > ",sendToSeqMsg)
        sequence_zmq:send(sendToSeqMsg)
        
        -- If we Pass, celebrate!
        if passfail then
            result = "PASS"
        end
--[[
        -- Populate the Flowlog
        t1 = time_utils.get_time_delta_ms(t1)
        if (TestFlowOut) then
            TestFlowOut.write(string.format("\tSKIPPED: %s\n", tostring(skipped)))
            TestFlowOut.write(string.format("\tLOW: %s\n\tHIGH: %s\n", tostring(msg_obj["low"]), tostring(msg_obj["high"])))
            TestFlowOut.write(string.format("\tVAL: %s\n", tostring(result_value)))
            TestFlowOut.write(string.format("\tRESULT: %s\n", result))
            TestFlowOut.write(string.format("\tTIME: %.3f\n", t1/1000))
            if sw_status then TestFlowOut.write(string.format("\tERROR: %s\n", sw_status)) end
            TestFlowOut.write(string.format("==END: %s\n", tostring(msg_obj["tid"])))
        end

        -- Prepare results to be sent
        local result_msg = {
            tid = msg_obj["tid"],
            result = result,
            value = result_value,
            sw_status = sw_status,
            sequence = msg_obj
        }

        -- But don't actually send if we skipped it due to conditional execution
        if not skipped then
            zassert(results_zmq:send(json.encode(result_msg)))
            -- print("Results sent.")
        end

        TestFlowOut.flush()
--]]
    end

    -- Tell the watchdog that we are alive and running sequence items
    watchdog_pipe:send("ALIVE")
end)

-- Process messages from both sockets
print("Running.")
poller:start()

--- test_engine.lua manages fixture interactions for the FCT test fixture.
-- It receives a series of <code>sequence_object</code> from the sequencer over a ZMQ socket,
-- executes the test item, sends a reply back to the sequencer over ZMQ, and sends the result 
-- over ZMQ to any subscribers.
--
-- All of the test items are run using the lua <code>pcall(...)</code> function, so the 
-- top level code will continue running even in the case of unchecked errors. Any errors
-- will be reported to the sequencer and sent out on the result socket.
--
-- A seperate thread publishes a ping on a watchdog ZMQ socket while waiting on timeouts (to
-- allow for detecting hung code).
--
-- It uses the following ZMQ Sockets:
-- <ul>
--      <li><span class="parameter">SEQUENCE</span> REP socket</li>
--      <li><span class="parameter">RESULTS</span> PUB socket</li>
--      <li><span class="parameter">WATCHDOG</span> PUB socket</li>
-- </ul>
-- @script test_engine.lua
-- <h2 class="section-header "><a name="Types"></a>Types</h2>
-- <dl class="function">
--     <dt>
--     <a name="sequence_object"></a>
--     <strong>sequence_object</strong>
--     </dt>
--      <dd>
--         Example of sequence object, generated from JSON string sent by the sequencer
--         <h3>Fields:</h3>
--         <ul>
--             <li><span class="parameter">id</span>
--              ID of the test in the source database
--             </li>
--             <li><span class="parameter">tid</span>
--              Test ID string
--             </li>
--             <li><span class="parameter">description</span>
--              Description of the sequence item
--             </li>
--             <li><span class="parameter">function</span>
--              Test function (see: <a href="../modules/function_table.html">function_table</a>)
--             </li>
--             <li><span class="parameter">param1</span>
--              Param1 from test sequence
--             </li>
--             <li><span class="parameter">param2</span>
--              Param2 from test sequence
--             </li>
--             <li><span class="parameter">unit</span>
--              Unit for function result and for the test limits
--             </li>
--             <li><span class="parameter">low</span>
--              Lower limit
--             </li>
--             <li><span class="parameter">high</span>
--              Upper limit
--             </li>
--             <li><span class="parameter">global_key</span>
--              Global Key for conditional execution
--             </li>
--             <li><span class="parameter">global_val</span>
--              Global Value to match for conditional execution
--             </li>
--             <li><span class="parameter">timeout</span>
--              Timeout for the test item to execute (diags commands only)
--             </li>
--         </ul>
--     </dd>
--     <dt>
--     <a name="sequence_response"></a>
--     <strong>sequence_response</strong>
--     </dt>
--      <dd>
--         Example of a response object sent back to the sequencer
--         <h3>Fields:</h3>
--         <ul>
--             <li><span class="parameter">status</span>
--              "success" or "error"
--             </li>
--             <li><span class="parameter">tid</span>
--              test ID string
--             </li>
--             <li><span class="parameter">error</span>
--              (optional) error string
--             </li>
--         </ul>
--      </dd>
--     <dt>
--     <a name="result_object"></a>
--     <strong>result_object</strong>
--     </dt>
--      <dd>
--         Example of a result object sent as a JSON string over the result socket
--         <h3>Fields:</h3>
--         <ul>
--             <li><span class="parameter">tid</span>
--              test ID string
--             </li>
--             <li><span class="parameter">result</span>
--              "PASS" or "FAIL"
--             </li>
--             <li><span class="parameter">value</span>
--              value returned from the test function, or nil, or "N/A" on error
--             </li>
--             <li><span class="parameter">sw_status</span>
--              nil, or error string on error
--             </li>
--             <li><span class="parameter">sequence</span>
--              original <code>sequence_object</code>
--             </li>
--         </ul>
--      </dd>
-- </dl>
--<dl class="function"><dt></dt></dl>
-- @usage test_engine.lua [-c config file] [-u UUT #] [OPT=VAL]
-- @usage test_engine.lua -c config.dummy -u 0 UNIT_STAGE=PRESMT2
