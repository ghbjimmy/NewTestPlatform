package.path = package.path.."./?.lua"
require "log"
require "flow"
require "public"

-------------------------------
-- hardware log...
-------------------------------
HWLog = Log:new()
hw_log = HWLog:new(
{	
	log_type="hw",
	log_dir="/vault/Intelli_log/",
	mlbsn="",
	station_id="",
	site_id="",
	msg_count=0,
	
	poller=nil,
	hw_sub=nil,
	f=nil,
})

function hw_log:reset()
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

function hw_log:init(_poller, _sub)
	self.poller = _poller
	self.hw_sub = _sub
	self.hw_sub:connect()
	poller:add(self.hw_sub.sock, zmq.POLLIN, function()	
		local msg = self.hw_sub:recv()
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

function hw_log:gen_name()
	local time_stamp = os.date("%m-%d-%H-%M-%S")
	self.site_id = get_siteid()
	--[SerialNumber]_[station id]_[site id]_[timestamp]_uart.txt
	local filename = string.format("%s%s_%s_%s_%s_hw.txt",self.log_dir,self.mlbsn,self.station_id,self.site_id,time_stamp)
	self.file_name = filename
	print("hw log file name = "..self.file_name)
end

function hw_log:gen_log()
	self:to_file()
end

function hw_log:write_msg(_msg)
	local time_stamp=gen_timestamp() 
	if self.msg_count>0 then 
		time_stamp="\n"..time_stamp
	end
	local msg=nil
	if _msg then
		msg=_msg
		msg = string.gsub(msg,"\r\n","format_mark                              ")--30 spaces
		msg = string.gsub(msg,"\r+","\r                              ")--30 spaces
		msg = string.gsub(msg,"\n+","\n                              ")--30 spaces
		msg = string.gsub(msg,"format_mark","\r\n")
		msg = time_stamp.."     "..msg
	end
	if msg and self.f then
		self.f:write(msg)
		self.f:flush()
	else
		print("<error> unable to open file: ", self.file_name)
	end
end

function hw_log:on_hw_msg(_msg)
--[[
	local time_stamp=gen_timestamp() 
	if self.msg_count>0 then 
		time_stamp="\n"..time_stamp
	end
	local msg=nil
	if _msg then
		msg=_msg
		msg = string.gsub(msg,"\r\n","format_mark                              ")--30 spaces
		msg = string.gsub(msg,"\r+","\r                              ")--30 spaces
		msg = string.gsub(msg,"\n+","\n                              ")--30 spaces
		msg = string.gsub(msg,"format_mark","\r\n")
		self.file_content = self.file_content..time_stamp.."     "..msg
		self.msg_count = self.msg_count + 1
	end
	--]]
	self:write_msg(_msg)
end

function hw_log:push_report(_msg, _data)
	if _msg=="on_seq_start" then
		self.f = io.open(self.file_name, "w")
		self.station_id=get_stationid()
		self.site_id=get_siteid()
		self.log_dir=get_logdir()
		
	elseif _msg=="on_seq_end" then
		--self:gen_log()
		self:reset()
		
	elseif _msg=="on_hw_msg" then
		self:on_hw_msg(_data)
	end
end

function hw_log:dipatch_msg(_msg)
	hw_log:push_report("on_hw_msg", _msg)
	flow_log:push_report("on_hw_msg", _msg)----flow log need to collect hw msg
end