package.path = package.path.."./?.lua"
require "log"
require "flow"
require "public"
-------------------------------
-- uart log
--The FCT software should insert time stamp at the beginning of each write to the log file
--The name of this log should be in this format: [SerialNumber]_[station id]_[site id]_[timestamp]_uart.txt, timestamp should be of the format: mm-dd-hh-mm-ss
--The logging software needs to look for line breaks in the response returned from diags to make sure indentation are added even though there is no line change in the logging code
--Format of file content is “yyyy/mm/dd hh:mm:ss:ddddd”
--Between the timestamp and the actual log content on the same line, there should be and only be spaces. No comma, colon or any other delineators
-------------------------------
UARTLog = Log:new()
uart_log = UARTLog:new(
{	
	log_type="uart",
	log_dir="/vault/Intelli_log/",
	mlbsn="",
	station_id="",
	site_id="",
	msg_count=0,
	
	poller=nil,
	uart_sub=nil,
	f=nil,
})

function uart_log:reset()
	self.file_content = ""
	self.log_dir="/vault/Intelli_log/"
	self.mlbsn=""
	self.station_id=""
	self.site_id=""
	self.msg_count=0
	
	self.file_name = ""
	
	self.f:close()
	f=nil
end

function uart_log:init(_poller, _sub)
	self.poller = _poller
	self.uart_sub = _sub
	self.uart_sub:connect()
	poller:add(self.uart_sub.sock, zmq.POLLIN, function ()	
		local msg = self.uart_sub:recv()
		if msg then
			--print("<hw_sub:recv> ",msg)
			msg = string.gsub(msg, "\r\n$","")
			msg = string.gsub(msg, "\r$","")
			msg = string.gsub(msg, "\n$","")
		
		end
		self:dipatch_msg(msg)
	end
	)
end

function uart_log:gen_name()
	local time_stamp = os.date("%m-%d-%H-%M-%S")
	self.site_id = get_siteid()
	--[SerialNumber]_[station id]_[site id]_[timestamp]_uart.txt
	local uart_name = string.format("%s%s_%s_%s_%s_uart.txt",self.log_dir,self.mlbsn,self.station_id,self.site_id,time_stamp)
	self.file_name = uart_name
	print("uart log file name = "..self.file_name)
end

function uart_log:gen_uart_log()
	self:format()
	self:to_file()
end

function uart_log:on_uart_msg(_msg)
	if _msg then
		self:write_msg(_msg)
	end
end

function uart_log:push_report(_msg, _data)
	if _msg=="on_seq_start" then
		self.station_id=get_stationid()
		self.site_id=get_siteid()
		self.log_dir=get_logdir()
		
		self.f = io.open(self.file_name, "w")
		
	elseif _msg=="on_seq_end" then	
		--self:gen_uart_log()
		self:reset()
		
	elseif _msg=="on_uart_msg" then
		self:on_uart_msg(_data)
	end
end

function uart_log:dipatch_msg(_msg)
	self:push_report("on_uart_msg", _msg)
	flow_log:push_report("on_uart_msg", _msg)----flow log need to collect hw msg
end

function uart_log:parse_timestamp()
	if self.file_content and self.file_content~="" then		
		local new = string.gsub(self.file_content, "%x%x%x%x%x%x%x%x%x%x%x%x@R1",function(sub)
			--print(sub)
			local s = string.match(sub, "(%x%x%x%x%x%x%x%x)%x%x%x%x@R1")
			local ms = string.match(sub, "%x%x%x%x%x%x%x%x(%x%x%x%x)@R1")
			if s then s = tonumber(s, 16) end
			if ms then ms = tonumber(ms, 16) end
			if s and ms then
				--return string.format("\n%6d.%03d           ",s,ms)
				local date = os.date("%Y/%m/%d %H:%M:%S.", tonumber(s))
				return string.format("\n%s%03d%s",date, ms,"          ")
			else
				return "xxxxxxxxxx"
			end 
		end)
		self.file_content = new
	end
end

function uart_log:format()
	self.file_content = string.gsub(self.file_content, "\n\n", "\r\n")
	self:parse_timestamp()
end

function uart_log:write_msg(_msg)
	local formatted = string.gsub(_msg, "%x%x%x%x%x%x%x%x%x%x%x%x@R1",function(sub)
			--print(sub)
		local s = string.match(sub, "(%x%x%x%x%x%x%x%x)%x%x%x%x@R1")
		local ms = string.match(sub, "%x%x%x%x%x%x%x%x(%x%x%x%x)@R1")
		if s then s = tonumber(s, 16) end
		if ms then ms = tonumber(ms, 16) end
		if s and ms then
			local date = os.date("%Y/%m/%d %H:%M:%S.", tonumber(s))
			return string.format("\n%s%03d%s",date, ms,"          ")
		else
			return "xxxxxxxxxx"
		end 
	end)
	if formatted then string.gsub(formatted, "\n\n", "\r\n") end
	if self.f then
		self.f:write(formatted)
		self.f:flush()
	else
		print("<error> unable to open uart file: ", self.file_name)
	end
end
