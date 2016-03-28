local json = require "dkjson"
--local path = "echo ~/testerconfig/zmqports.json"
local path = "/zmqports.json"
print("path::::::::::: ",path)
--local f = io.popen(path);
--path = f:read();
--f:close()
print("Read zmq port from :",path);
local config = {}
local port = {}
local file = io.open(path,"r")
local str = nil
if(file==nil) then
	print("Can't open json file : ",path)
else
	str = file:read("*a")
	file:close()
end
if(str) then
	port = json.decode(str)
	if(port == nil) then
		print("json format error from file : ",path)
	end
else
	print("Got nil from file : ",path)
end

function config.zmqport(portname)
	return port[portname]
end

return config;