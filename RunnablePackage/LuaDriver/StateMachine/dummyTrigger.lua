

local zmq = require("lzmq")
local zpoller = require "lzmq.poller"
local zthreads = require "lzmq.threads"
local json = require "dkjson"
local lapp = require("pl.lapp")
local ztimer  = require "lzmq.timer"

require "pathManager"
package.path = package.path..";"..deleteLastPathComponent(CurrentDir()).."/Driver/?.lua"
local config_utils = require("utils.config_utils")

local args = lapp [[
X527 State Machine Test
    
    -c,--config     (default config.socket_zmq)   Config module to load.
    <updates...>    (default "X=Y")                    Series of X=Y pairs to update the CONFIG table with
]]

local context = zmq.context()
zthreads.set_context(context)

	args.uut = 0
	c = config_utils.get_config(args)
	n = tostring(config_utils.get_addr(c, "STATEMACHINE_REP", c.ID))
	print("< "..tostring(c.ID).." Trigger > Set REQ : ".. n)
	local triger, err = context:socket{	zmq.REQ, connect = n}
	zmq.assert(triger, err)

while(1) do
	print("press any enter to send trigger msg...")
	local t = io.read()
	if(string.match(t,"loop")) then
		print(triger:send("{\"function\":\"START\",\"times\":\"3\"}"))
	elseif(string.match(t,"start")) then
		print(triger:send("{\"function\":\"START\"}"))
	elseif(string.match(t,"setsn")) then
		print(triger:send("{\"function\":\"SETUUTSN\",\"sn\":\"12312312312312312\",\"uut\":\"0\"}"))
	elseif(string.match(t,"break")) then
		print(triger:send("{\"function\":\"LOOPBREAK\"}"))
	elseif(string.match(t,"uutinfo")) then
		print(triger:send("{\"function\":\"START\",\"uutinfo\":[{\"uut\":0,\"sn\":\"qweqweqweqwe\"}]}"))
	end
	print(triger:recv())
end