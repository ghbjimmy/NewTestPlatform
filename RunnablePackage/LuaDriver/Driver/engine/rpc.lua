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


local dispatcher={};


--zmq handler
local publisher = {};
local reply = {};

function dispatcher.publisher(...)
	publihser.sendx(...);
end


-- Process messages from both sockets
print("Running.")
poller:start()
