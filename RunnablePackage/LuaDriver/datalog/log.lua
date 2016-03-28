-------------------------------
-- Log base
-------------------------------
 Log = {
 file_name="", 
 file_content=""
 }
 
 function Log:new(o)
	o = o or {}
	setmetatable(o, self)
	self.__index = self
	return o
end

 function Log:to_file()
	local f = io.open(self.file_name, "w")
	assert(f, "Failed to open file: "..tostring(self.file_name))
	local ret = f:write(self.file_content)
	f:flush()
	f:close()
	return ret
end

 function Log:append(info)
	if type(info)=="string" then
		self.file_content = self.file_content .. info
	end
	
end

 function Log:overwrite(info)
	if type(info)=="string" then
		self.file_content = info
	end
end

function Log:exist_file()
	local f = io.open(self.file_name, "r")
	if f then
		f:close()
		return true
	else
		return false
	end
end

function Log:set_filename(filename)
	self.file_name = filename
end