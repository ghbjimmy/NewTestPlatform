--- io set Module this table is for Debug Panel
-- @module functions.ioset
-- @alias io_module

local MCU = require("hw.MCU")
local time_utils = require("utils.time_utils")

-------------------------------
-- Relay exported module
-------------------------------
local io_module = {}

--- Control the fixture relays
-- @param hwio_table table of connection-to-IO mappings
-- @param[opt=true] disconnect true if disconnecting the relays
-- @param[optchain="CONNECT"] connect_to specify a non-default connection
-- @return true
-- @return passing status (whether the eload was sucessfully applied)
-- @raise error

function io_module.ioset(sequence)
  if(sequence.param1 and #sequence.param1>0 and sequence.param2 and #sequence.param2>0) then
    ret = MCU.InstrumentCmd("io set", sequence.param1, sequence.param2)
    if string.match(tostring(ret),"DONE") == nil then
      error("ERRCODE[-18]Problem communicating with MCU: "..tostring(ret))
    end
    time_utils.delay(2) --FIXME: REquested by intelligent
    return true, true
  else
   	error("ERRCODE[-19]Invalide param1 or param2: "..tostring(sequence.param1)..","..tostring(sequence.param2))
  end
end




-----------------------------------
-- Export relay to module users
-----------------------------------
return io_module
