-- ldoc configuration file
title = "X527 Test Engine Documentation"
project = "X527 Test Engine"
not_luadoc = true
prettify_files = true
all = true

file = {".", exclude = {"utils/zhelpers.lua", "calibration.lua", "callback.lua", "Global.lua", "hw/ArmDL.lua"}}

full_description = [[
This code interfaces between the test sequencer (using ZMQ) and the raw fixture interfaces (using one of the console modules). It conditionally executes tests and publishes the results over ZMQ after completion.
]]