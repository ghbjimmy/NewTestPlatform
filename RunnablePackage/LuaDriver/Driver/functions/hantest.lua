

local socket = require("socket")

local _hanFun = {}

_hanFun.info = "Hello the world! --------"

function _hanFun.sleep(n) -- n: second
   socket.select(nil, nil, n)
end




function _hanFun.SayHello()
	print "-------------"
	print("-------------",os.date() )
	_hanFun.sleep(2)
	print("------------- now return",os.date())
	return _hanFun.info
end

function _hanFun.errCode()
	_hanFun.sleep(1)
	error("ERRCODE[-999]error test")
end

function _hanFun.failItem()
	_hanFun.sleep(1)
	return 123
end


return _hanFun
