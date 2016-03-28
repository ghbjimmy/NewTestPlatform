require "lfs"

function CurrentDir()
	return lfs.currentdir()
end

function deleteLastPathComponent(p)
	local t = {}
	for v in string.gmatch(p,"[^/]+")do
		t[#t+1] = v
	end
	if(#t>1) then
		table.remove(t,#t)
	end
	return "/"..table.concat(t,"/")
end

function appendPath(op, np)
	local t = {}
	for v in string.gmatch(op,"[^/]+")do
		t[#t+1] = v
	end

	for v in string.gmatch(np,"[^/]+")do
		t[#t+1] = v
	end
	return "/"..table.concat(t,"/")
end

function lastPathComponent(p)
	return string.match(p, "([^/]+)$")
end

function pathComponents(p)
	local t = {}
	for v in string.gmatch(p,"[^/]+")do
		t[#t+1] = v
	end
	return t
end