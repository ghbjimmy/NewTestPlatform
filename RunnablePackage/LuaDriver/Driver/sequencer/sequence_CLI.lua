local thread = require "llthreads2"

local seq_cli = {}


local cli_entry = ---[===[
	local zmq = require"lzmq"
	local zassert = zmq.assert;
	local reply_ip = "tcp://127.0.0.1:6401"
	local cxt = zmq.context();
	local socket,err = cxt:socket(zmq.REQ,{connect=reply_ip});
	zassert(socket,err);
	
	while true do
		print("INT>")
		local cmd = io.read("*line");
		if (#cmd==0) then
			continue;
		else
		end
	end

--]===]



return seq_cli;