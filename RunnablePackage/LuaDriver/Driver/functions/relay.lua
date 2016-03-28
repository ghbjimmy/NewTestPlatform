--- Relay Functions Module
-- @module functions.relay
-- @alias relay_module

local HWIO = require("hw.HWIO")
local MCU = require("hw.MCU")
local time_utils = require("utils.time_utils")

-------------------------------
-- Relay exported module
-------------------------------
local relay_module = {}
-- function relay_module.relay(sequence)
-- function relay_module.disconnect(sequence)
-- function relay_module.relay_from_connections(connections)
-- function relay_module.disconnect_from_connections(connections)
-- function relay_module.button(sequence)
-- function relay_module.tbat(sequence)

----------------------------------
-- Raw Relay Connections function
----------------------------------

--- Control the fixture relays
-- @param hwio_table table of connection-to-IO mappings
-- @param[opt=true] disconnect true if disconnecting the relays
-- @param[optchain="CONNECT"] connect_to specify a non-default connection
-- @return true
-- @return passing status (whether the eload was sucessfully applied)
-- @raise error

local function relay_from_connections(hwio_table, disconnect, connect_to)
  -- print(tostring (hwio_table)..";"..tostring(disconnect)..";"..tostring(connect_to))
  if hwio_table == nil then
    error("ERRCODE[-24]Incorrect connection Table")
  end

  if disconnect == true then
    connect_to = "DISCONNECT"
  elseif connect_to == nil or connect_to == "" then
    connect_to = "CONNECT"
  end

  local connections = hwio_table[connect_to]
  if connections == nil then
    error("ERRCODE[-25]Invalid connection for relay: " .. connect_to,2)
  end

  local cmd = {}
  local io = {}
  local value = {}
  local current_state = -1
  local next_state = -1

  local count = #connections.IO
  for n=1, count do
    io[#io+1] = connections.IO[n][1]
    value[#value+1] = connections.IO[n][2]
    
    if string.match(HWIO.critical, tostring(connections.IO[n][1])) then
      current_state = 1
    else
      current_state = 0
    end
    if n+1 <= count then
      if string.match(HWIO.critical, tostring(connections.IO[n+1][1])) then
        next_state = 1
      else
        next_state = 0
      end
    else
      next_state = 2
    end

    if current_state ~= next_state or next_state == 2 then
      cmd[1] = table.concat(io, ",")
      cmd[2] = table.concat(value,",")
      ret = MCU.InstrumentCmd("io set", cmd[1], cmd[2])
      if string.match(tostring(ret),"DONE") == nil then
        error("ERRCODE[-26]Problem communicating with MCU: "..tostring(ret))
      end
      time_utils.delay(2)
      io = {}
      value = {}
      cmd = {}
    end
  end
  
  return true
end


-------------------------------
-- Public Relay Functions
-------------------------------

--- Connect net at [param1] to default fixture HW (or the fixture net specified in [param2])
-- @tparam sequence_object sequence FCT test sequence table
-- @return result (true)
-- @return passing status (true)
-- @raise error

function relay_module.relay(sequence)
  local testpoint = sequence.param1
  local connect_to = sequence.param2
  -- For nets with mutiple connections, we need to specify which one.

  -- FIXME: Special case for the discharge resistors for now
  -- if testpoint == "DISCHARGER" then
  --   -- Disconnect all the supplies
  --   for _, connections in HWIO.SupplyTable do
  --     relay_from_connections(connections, true, connect_to)
  --   end
  -- end
  local result = relay_from_connections(HWIO.RelayTable[testpoint], false, connect_to)
  return result, true
end

--- Disconnect net at [param1] from fixture HW
-- @tparam sequence_object sequence FCT test sequence table
-- @return result (true)
-- @return passing status (true)
-- @raise error

function relay_module.disconnect(sequence)
  local testpoint = sequence.param1

  local result = relay_from_connections(HWIO.RelayTable[testpoint], true)
  return result, true
end
--- Convenience function to connect testpoints for Measure, DMM, and Eload
-- This only exists because these HWIO table items only have "IO" entries 
-- (instead of "CONNECT" and "DISCONNECT" entries)
-- @param HWIO_table_item measure, dmm, or eload HWIO table items
-- @return passing status (true)
-- @raise error
-- @within ConvenienceFunctions
function relay_module.relay_from_IO_only_item(HWIO_table_item)
  return relay_from_connections({CONNECT = HWIO_table_item}, false)
end

--- Convenience function to connect testpoints for Measure, DMM, and Eload
-- This only exists because these HWIO table items only have "IO" entries 
-- (instead of "CONNECT" and "DISCONNECT" entries)
-- @param HWIO_table_item measure, dmm, or eload HWIO table items
-- @return passing status (true)
-- @raise error
-- @within ConvenienceFunctions

--- Convenience function to expose raw connections API to outside users
-- @param connections standard HWIO table mapping
-- @return passing status (true)
-- @raise error
-- @within ConvenienceFunctions

function relay_module.relay_from_connections(connections)
  return relay_from_connections(connections, false)
end

-- function relay_module.relay_from_IO_only_item(HWIO_table_item)
--   return relay_from_connections({CONNECT = HWIO_table_item},false)
-- end

--- Convenience function to expose raw connections API to outside users
-- @param connections standard HWIO table mapping
-- @return passing status (true)
-- @raise error
-- @within ConvenienceFunctions
function relay_module.disconnect_from_connections(connections)
  return relay_from_connections(connections, true)
end

--- Connect the net [param1] for two seconds.
-- Currently only supports BUTTON1_L.
-- @tparam sequence_object sequence FCT test sequence table
-- @return result (true)
-- @return passing status (true)
-- @raise error

function relay_module.button(sequence)
  -- this is button1. just press the button.
  if sequence and sequence.param1 ~= "BUTTON1_L" then
    error("ERRCODE[-27]Button command only works for BUTTON1_L")
  end

  relay_from_connections(HWIO.RelayTable["BUTTON1_L"], true)
  --time_utils.delay(2000)
  relay_from_connections(HWIO.RelayTable["BUTTON1_L"], false)
  time_utils.delay(2000)
  relay_from_connections(HWIO.RelayTable["BUTTON1_L"], true)

  return true, true;
end

--- Set the battery NTC to a given resistance
-- Automatically connects the NTC to the DUT.
-- @tparam sequence_object sequence FCT test sequence table
-- @return result (true)
-- @return passing status (true)
-- @raise error

function relay_module.tbat(sequence)
    if sequence.param1 == nil then
      error("ERRCODE[-28]Resistance must be specified.")
    end

    local _net = sequence.param1;
    local _r = tonumber(_net)
    if(_r) then
      if(_r == 3000) then
        _net = "RES_3K"
      elseif(_r == 13000) then
        _net = "RES_13K"
      elseif(_r == 16000) then
        _net = "RES_16K"
      elseif(_r == 20000) then
        _net = "RES_20K"
      elseif(_r == 25000) then
        _net = "RES_25K"
      elseif(_r == 30000) then
        _net = "RES_30K"
      else
        error("ERRCODE[-29] Resistor value Invalid: "..tostring(_net))
      end
    end
    relay_from_connections(HWIO.RelayTable["PMU_TBAT"], false, _net)
    return true, true
end

-----------------------------------
-- Export relay to module users
-----------------------------------
return relay_module
