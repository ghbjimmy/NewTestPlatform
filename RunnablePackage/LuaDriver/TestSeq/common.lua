Module = "X473F_D2.3_20150728";
Version = "20150728";

if(tc.AppDir()) then
    package.path = package.path..";"..tostring(tc.AppDir()).."/Driver/?.lua"
else
    package.path = package.path..";".."./?.lua"
end

SQLITE_FILE = "development.sqlite3"

require "Global"
require "HWIO"
require "dut_tcp"
require "dut"
-- require "dut_uart"
require "fixture"
require "eload"
require "dmm"
require "uuid"
bit32 = require "bit"
require "callback"
require "pdca"
require "calibration"
-- require "AirPressure"
require "Thdn_Data"

if(tc.StationType() ~= "PREFCT" and tc.StationType()~="PREFCT2") then--sip station
	_carrier = require "IASocketLua"
end

armdl = require "ArmDL"
require "sql"
--require "test"
