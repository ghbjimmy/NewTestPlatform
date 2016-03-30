require "lfs"
local path = require("pl.path")

function UserHome()
	return path.expanduser '~'
end

function CurrentDir()
	return lfs.currentdir()
end

function CurrentPathSeparator()

	local tempStr = path.join("1","2")
	return string.sub(tempStr,2,2)
end

function JoinPath(p1, p2)
	return path.join(p1, p2)
end

function DeleteLastPathComponentUnified(pt)
	local t = {}
	local s = CurrentPathSeparator();
	local matchPattern = "[^"..s.."]+" --"[^/]+"
	for v in string.gmatch(pt,matchPattern)do
		t[#t+1] = v
	end
	if(#t>1) then
		table.remove(t,#t)
	end
	local newp = table.concat(t,s)
	local pos = string.find(pt,s)
	print("pos:",pos)
	if( pos == 1) then
		newp = s .. newp
	end
	return newp
end

function DeleteLastPathComponentTricky(pt)
	return path.join(pt,"..")
end


function DeleteLastPathComponent_Deprecated(p)
	local t = {}
	for v in string.gmatch(p,"[^/]+")do
		t[#t+1] = v
	end
	if(#t>1) then
		table.remove(t,#t)
	end
	return "/"..table.concat(t,"/")
end

function AppendPath_Deprecated(op, np)
	local t = {}
	for v in string.gmatch(op,"[^/]+")do
		t[#t+1] = v
	end

	for v in string.gmatch(np,"[^/]+")do
		t[#t+1] = v
	end
	return "/"..table.concat(t,"/")
end

function LastPathComponent_Deprecated(p)
	return string.match(p, "([^/]+)$")
end

function PathComponents_Deprecated(p)
	local t = {}
	for v in string.gmatch(p,"[^/]+")do
		t[#t+1] = v
	end
	return t
end
