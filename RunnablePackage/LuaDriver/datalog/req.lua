-------------------------------
-- subscriber base
-------------------------------
Zmqreq = {url="tcp://127.0.0.1", port=0, identity="", sock = nil,}
function Zmqreq:new(o)
	o = o or {}
	setmetatable(o, self)
	self.__index = self
	return o
end
LogZmqreq = Zmqreq:new()

function LogZmqreq()
end
