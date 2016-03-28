#!/usr/bin/env lua
package.path = package.path..";".."./?.lua"

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
    -e,--echo       (default 1)                     echo receive and result in terminal
    <updates...>    (default "X=Y")                    Series of X=Y pairs to update the CONFIG table with
]]

local __ENGINE_VERSION = "1.2"
print("*********************************************")
print("			  Load Engine ","V"..__ENGINE_VERSION);
print("*********************************************")

--===========================--
-- Global (!!!) config variable
--===========================--
CONFIG = config_utils.get_config(args)

--===========================--
-- Setup ZMQ sockets
--===========================--
local context = zmq.context()
zthreads.set_context(context)

print("< "..tostring(CONFIG.ID).." test_engine.lua > Connect SEQUENCE ZMQ REP "..tostring(config_utils.get_addr(CONFIG, "SEQUENCE", CONFIG.ID)))
local sequence_zmq, err = context:socket(zmq.REP, {bind = config_utils.get_addr(CONFIG, "SEQUENCE", CONFIG.ID)})
zassert(sequence_zmq, err)

-- print("< "..tostring(CONFIG.ID).." test_engine.lua > Connect RESULTS ZMQ PUB "..tostring(config_utils.get_addr(CONFIG, "RESULTS", CONFIG.ID)))
-- local results_zmq, err = context:socket(zmq.PUB, {bind = config_utils.get_addr(CONFIG, "RESULTS", CONFIG.ID)})
-- zassert(results_zmq, err)

--===========================--
-- Setup Fixture / DUT sockets
--===========================--

local fixture_console = require(CONFIG.FIXTURE_CONSOLE)
--fixture_console._Arm_Socket_Connect_()

local dut_console = require("tcpAndZmq")
--dut_console._DUT_Socket_Connect_()

-- require "functions.calibration"

local TestEngineLog = require("utils.LogFile")
EngineLogName = "ENGINE"
local EngieLogType = "txt"

--===========================--
-- Global loggers
--===========================--

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
        local  t = require "lzmq.timer"

        print(" < TE > Set HeartBeat PUB : ", address)
        local watchdog_zmq, err = context:socket(zmq.PUB, {bind = address})
        zassert(watchdog_zmq, err)

        while(1) do
            t.sleep(5000)
            watchdog_zmq:send_all{"101",os.date("%m-%d %H:%M:%S",os.time()),3,"TestEngine","FCT_HEARTBEAT"}
        end
    end, config_utils.get_addr(CONFIG, "WATCHDOG", CONFIG.ID))
watchdog_thread:start(true, false)

--===========================--
-- Sequence poller
--===========================--
local poller = zpoller.new(1)
poller:add(sequence_zmq, zmq.POLLIN, function()
    local msg = sequence_zmq:recv()
    if args.echo~=0 then print (" < Received > ",tostring(msg)) end--"{"jsonrpc":"V0.1","id":"1",fucntion": "relay","params":"p1,p2,timeout"}"  unit???
    local receiveString = time_utils.get_time_string_ms().."\t < Received > \t"..tostring(msg)
    local msg_obj, pos, err = json.decode (msg, 1, nil)
    -- If we have a JSON error, return ERROR and bail
    if err or msg_obj == nil then
        local tmp = json.encode({status = "error", error = "json", tid = "nil"})
        print(tmp)
        sequence_zmq:send(tmp)
    else
        local result_value, sw_status = nil, nil
        local result = "FAIL"

        -- Setup watchdog for predicted timeout
        -- watchdog_pipe:send("TIMEOUT:"..tostring(msg_obj.timeout))

        if(msg_obj.params) then
            msg_obj.param1=msg_obj.params[1]
            msg_obj.param2=msg_obj.params[2]
        else
            msg_obj.param1=nil
            msg_obj.param2=nil
            msg_obj.unit=nil
            msg_obj.timeout=3000
        end
        -- Send sequence to the action dispatch thread, and catch errors
        local dispatch_status, result_value, passfail, skipped
        if msg_obj["function"] == "_my_rpc_server_is_ready" then
            dispatch_status = true
            result_value = "--PASS--"
            passfail = true
        else
            -- print("@@@@@ dispatch_function ")
            if(msg_obj["function"] == "start_test") then
                TestEngineLog.close(EngineLogName)
                TestEngineLog.open(EngineLogName,EngieLogType)
            end

            dispatch_status, result_value, passfail, skipped = pcall(dispatch_function, msg_obj)
            if result_value==true then
                result_value = "--PASS--"
            elseif result_value == false then
                result_value = "--FAIL--"
            end

            if result_value==nil then result_value = "" end
        end
        local sendToSeqMsg;
        if dispatch_status then
            sendToSeqMsg = json.encode({jsonrpc = msg_obj.jsonrpc, id = msg_obj.id, result = result_value})
        else
            local errcode = string.match(tostring(result_value), "ERRCODE%[(.-)%]")
            local errmsg = string.match(tostring(result_value), "ERRCODE%[.-%](.+)")
            if errcode == nil or errmsg == nil then
                errcode = "-999"
                errmsg = tostring(result_value)
            end
            sendToSeqMsg = json.encode({jsonrpc = msg_obj.jsonrpc, id = msg_obj.id,  error = {code=errcode,message=errmsg}})
        end
        if args.echo~=0 then print(" < Result > ",sendToSeqMsg,"\n") end
        sequence_zmq:send(sendToSeqMsg)
        TestEngineLog.write(EngineLogName,receiveString.."\n",false)
        TestEngineLog.write(EngineLogName,time_utils.get_time_string_ms().."\t < Result > \t"..sendToSeqMsg.."\n\n",false)
    end
    -- Tell the watchdog that we are alive and running sequence items
    -- watchdog_pipe:send("ALIVE")
    collectgarbage("collect")
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
