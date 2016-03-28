

local connect_to = "tcp://localhost:6200"--"tcp://localhost:5555"

local zmq = require"lzmq"

local ctx = zmq.init(1)
local s = ctx:socket(zmq.REQ)

print("Connect_to: ",connect_to)
s:connect(connect_to)


local listCmd = zmq.msg_init_data("{\"function\": \"list\", \"params\": [\"10\"], \"jsonrpc\": \"1.0\", \"id\": \"cmd1\"} ")
local loadCmd = zmq.msg_init_data("{\"function\": \"load\", \"params\": [\"/Hantest_plan__0322_11h.csv\"], \"jsonrpc\": \"1.0\", \"id\": \"cmd1\"} ")

assert(s:send_msg(loadCmd))
local msg = zmq.msg_init()
assert(s:recv_msg(msg))
print("received:",  msg)


s:close()
ctx:term()
