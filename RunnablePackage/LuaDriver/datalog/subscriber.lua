-------------------------------
-- subscriber base
-------------------------------
Subscriber = {url="tcp://127.0.0.1", port=0, identity="", sock = nil,}
function Subscriber:new(o)
	o = o or {}
	setmetatable(o, self)
	self.__index = self
	return o
end
LogSubscriber = Subscriber:new()
