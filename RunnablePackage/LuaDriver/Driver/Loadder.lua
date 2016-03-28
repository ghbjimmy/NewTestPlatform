local dut = require "functions.dut"

for v in pairs(dut) do
	print(v,dut[v])
end