
local zmq = require("lzmq")
local zpoller = require "lzmq.poller"
local zthreads = require "lzmq.threads"
local json = require "dkjson"
local lapp = require("pl.lapp")
local ztimer  = require "lzmq.timer"

package.path = package.path..";".."./StateMachine/?.lua"
require "pathManager"
package.path = package.path..";"..CurrentDir().."/Driver/?.lua"
local config_utils = require("utils.config_utils")

local args = lapp [[
X527 State Machine Test
    
    -c,--config     (default config.socket_zmq)   Config module to load.
    -s,--slots        (default 0)                     UUTs (used for IP/Port selection)
    <updates...>    (default "X=Y")                    Series of X=Y pairs to update the CONFIG table with
]]

local context = zmq.context()
zthreads.set_context(context)

local flag = {}
local c, n, sub, req = nil,nil,nil,nil
	args.uut = 0
	c = config_utils.get_config(args)
	n = tostring(config_utils.get_addr(c, "FIXTURE_CTRL_SUB", c.ID))
	print("< "..tostring(c.ID).." FixtureTest > Set SUB : ".. n)
	sub, err = context:socket{	zmq.SUB, connect = n, subscribe = ""}
	zmq.assert(sub, err)
	n = tostring(config_utils.get_addr(c, "FIXTURE_CTRL_REQ", c.ID))
	print("< "..tostring(c.ID).." FixtureTest > Set REQ : ".. n)
	req, err = context:socket{zmq.REQ, connect = n}
	zmq.assert(req, err)

local poller = zpoller.new(1)
	poller:add(sub, zmq.POLLIN, function()
  		local msg = sub:recv()
  		print(i, "< sub receive >", msg)
	end)
-- poller:start()
while(1) do

	io.write(">>")
	local m = io.read()
	-- print(" < io read > ",m)
	print(req:send(m))
	print("< REQ > ",req:recv())
	print("< PUB > ",sub:recv())
end