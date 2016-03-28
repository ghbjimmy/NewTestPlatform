
local zmq = require("lzmq")
local ztimer  = require "lzmq.timer"
local lapp = require("pl.lapp")

package.path = package.path..";".."./StateMachine/?.lua"
require "pathManager"
local triggerReq = nil

local args = lapp [[
X527 State Machine
    
    -c,--config     (default config.socket_zmq)   Config module to load.
    <updates...>    (default "X=Y")                    Series of X=Y pairs to update the CONFIG table with
]]

package.cpath = package.cpath..";"..(CurrentDir()).."/lib/?.dylib"
require "libSipFixture"
package.path = package.path..";"..(CurrentDir()).."/Driver/?.lua"
local config_utils = require("utils.config_utils")
args.uut=0
local c = config_utils.get_config(args)
local sub = tostring(config_utils.get_addr(c, "FIXTURE_CTRL_SUB", c.ID))
local req = tostring(config_utils.get_addr(c, "FIXTURE_CTRL_REQ", c.ID))
local trigger = tostring(config_utils.get_addr(c, "STATEMACHINE_REP", c.ID))
CreatSipFixture(); --libSipFixture API
InitialSerialPort(c.FIXTURE_CTRL_SERIAL_PORT, c.FIXTURE_CTRL_SERIAL_SETTING);--libSipFixture API
InitialZmqPort(req, sub);--libSipFixture API
InitialReqPort(trigger,"start","{\"function\":\"START\"}");
SipFixtureInitial();--libSipFixture API

while(1) do
	ztimer.sleep(10)
end
