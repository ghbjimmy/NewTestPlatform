#!/usr/bin/env lua
require "pathManager"
package.path = package.path..";"..deleteLastPathComponent(CurrentDir()).."/Driver/?.lua"
local config_utils = require("utils.config_utils")
require "utils.zhelpers"
local zmq = require("lzmq")
local zthreads = require("lzmq.threads")
local json = require("dkjson")
local lapp = require("pl.lapp")

local args = lapp [[
X527 SN Manager

This is the SN Manager, user can set SN or Get SN
Set SN Msg format : "SET,xxxxxx" xxxxx is the SN
Get SN Msg format : "GET", SN will reture
Clear SN MSG fromat : "CLEAR" 
    
    -c,--config     (default config.socket_zmq)   Config module to load.
    -u,--uut        (default 1)                     UUT slot number (used for IP/Port selection)
    <updates...>    (default "X=Y")                    Series of X=Y pairs to update the CONFIG table with
]]

local c = config_utils.get_config(args)

print("< "..tostring(c.ID).." SNTest > Set REQ : "..tostring(config_utils.get_addr(c, "SNMANAGER", c.ID)))
local context = zmq.context()
zthreads.set_context(context)
local sntest, err = context:socket(zmq.REQ, {connect = tostring(config_utils.get_addr(c, "SNMANAGER", c.ID))})
zassert(sntest, err)

io.read()
local t={}
t["function"]="set"
t.sn="`12123123"
sntest:send(json.encode(t))
print(sntest:recv())
io.read()
t={}
t["function"]="get"
sntest:send(json.encode(t))
print(sntest:recv())
io.read()
t={}
t["function"]="clear"
sntest:send(json.encode(t))
print(sntest:recv())
io.read()
t={}
t["function"]="get"
sntest:send(json.encode(t))
print(sntest:recv())