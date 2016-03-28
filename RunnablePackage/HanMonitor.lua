

--local connect_to = "tcp://localhost:5555"
local connect_to = "tcp://localhost:6250"
--local connect_to = "tcp://172.22.0.60:6250"


print("connect to: ", connect_to)
-- 6580 SM  Heartbeat
-- 6150 TE  Heartbeat
-- 6250 SEQ Heartbeat+Pub
local message_size = 100
local message_count = 15

local zmq = require"lzmq"

local ctx = zmq.init(1)--context{io_threads = 1, max_sockets = 255}  --zmq.init(1)
local s = ctx:socket(zmq.SUB)
s:subscribe("101")    --setopt(zmq.SUBSCRIBE, "");
s:connect(connect_to)

local msg = zmq.msg_init()

for i = 1, 2000 do

	-- assert(s:recv_msg(msg))
	msgs = s:recv_multipart()--(zmq.NOBLOCK)

	print("received:",  msgs[1],msgs[2],msgs[3],msgs[4],msgs[5])

end

s:close()
ctx:term()
