package.path = package.path.."./?.lua"
require "log"
require "public"

-------------------------------
-- flow log
--Each tester action line needs to have a timestamp
--It’s marked with the “[Tester]:” label
--There is at least one space between the colon after “[Tester]” and the rest of the line
--The marker lines don’t need timestamps
--The marker lines begin with "==Test:" or "==SubTest:"
--The test name and subtest name should be consistent with the test plan. At least one of them need to include the unique test id.
--If the test is numerical and have upper and lower limits, this line should have this format: “lower:[low limit]; higher:[high limit] ;value:[value]”
--If the test is only pass/fail with certain expected values, this line should have this format: “expected:[all possible values separated by | ]; value:[value]"
--Limit line doesn’t need to have a timestamp
--On the lines that don’t begin with a time stamp, the logging software should add indentation so the content of all lines are aligned
-------------------------------
FLOWLog = Log:new()
flow_log = FLOWLog:new(
{	
	log_type="flow",
	log_dir="/vault/Intelli_log/",
	mlbsn="",
	station_id="",
	site_id="",
	--need to buffer below item
	unit="",
	low="",
	high="",
	f=nil,
})

function flow_log:reset()
	self.file_content = ""
	self.log_dir="/vault/Intelli_log/"
	self.mlbsn=""
	self.station_id=""
	self.site_id=""
	self.unit=""
	self.low=""
	self.high=""
	
	self.file_name = ""
	
	self.f:close()
	f=nil
end

function flow_log:init()
end

function flow_log:gen_name()
	local time_stamp = os.date("%m-%d-%H-%M-%S")
	self.site_id = get_siteid()
	--[SerialNumber]_[station id]_[site id]_[timestamp]_flow.txt
	local flow_name = string.format("%s%s_%s_%s_%s_flow.txt",self.log_dir,self.mlbsn,self.station_id,self.site_id,time_stamp)
	self.file_name = flow_name
	print("flow log file name = "..self.file_name)
end

local flow_plain_buf = ""

function flow_log:gen_flow_log()
	self:parse_timestamp()
	self:to_file()
	
	--Here writes flow log without uart data as Martin required
	self.file_name = string.gsub(self.file_name, "flow", "flow_plain")
	self.file_content = flow_plain_buf
	self:to_file()
	flow_plain_buf = ""
end

function flow_log:push_uart_msg(_msg)
	local msg=nil
	if _msg then
		msg=_msg
		local time_stamp=gen_timestamp() 
		if self.file_content~="" then 
			time_stamp="\n"..time_stamp
		end
		msg = string.gsub(msg,"\r\n","format_mark                              ")--30 spaces
		msg = string.gsub(msg,"\r+","\r                              ")--30 spaces
		msg = string.gsub(msg,"\n+","\n                              ")--30 spaces
		msg = string.gsub(msg,"format_mark","\r\n")
		self.file_content = self.file_content..time_stamp.."     "..msg
	end
end

function flow_log:push_test(_station_id,_site_id,_log_dir)
	if _station_id then self.station_id=_station_id end
	if _site_id then self.site_id=_site_id end
	if _log_dir then self.log_dir=_log_dir end
end

function flow_log:push_item(_group,_tid,_unit,_low,_high,_pdca_flag)
	if _group then
		if self.file_content~="" then
			self.file_content=self.file_content.."\n==Test: ".._group
			flow_plain_buf = string.format("%s%s%s",flow_plain_buf,"\n==Test: ",tostring(_group))
		else
			self.file_content="==Test: ".._group
			flow_plain_buf = string.format("%s%s%s",flow_plain_buf,"==Test: ",tostring(_group))
		end
	end
	
	if _tid then
		if self.file_content~="" then
			self.file_content=self.file_content.."\n==SubTest: ".._tid
			flow_plain_buf = string.format("%s%s%s",flow_plain_buf,"\n==SubTest: ",tostring(_tid))
		else
			self.file_content="==SubTest: ".._tid
			flow_plain_buf = string.format("%s%s%s",flow_plain_buf,"==SubTest: ",tostring(_tid))
		end
	end
	
	if _high then self.high=_high end
	if _low then self.low=_low end
	if _unit then self.unit=_unit end
	
end

function flow_log:push_tester_action(_tester_action)
	if _tester_action then
		local time_stamp=gen_timestamp() 
		if self.file_content~="" then 
			time_stamp="\n"..time_stamp
			self.file_content=self.file_content..time_stamp.." [Tester]: ".._tester_action
		else
			self.file_content=time_stamp.." [Tester]: ".._tester_action
		end
	end	
end

function flow_log:push_item_numeric_val(_val)
	if _val then
		if self.file_content~="" then
			self.file_content=self.file_content.."\n                          lower:"..self.low.."; upper:"..self.high.."; value:".._val
			flow_plain_buf = flow_plain_buf.."\n                          lower:"..self.low.."; upper:"..self.high.."; value:".._val
		else
			self.file_content="lower:"..self.low.."; upper:"..self.high.."; value:".._val
			flow_plain_buf = "lower:"..self.low.."; upper:"..self.high.."; value:".._val
		end
	end	
end

function flow_log:push_item_expect_val(_expected, _val)
	if _expected and _val and type(_val)=="table" then
		local val=table.concat(_val," | ")
		if self.file_content~="" then
			self.file_content=self.file_content.."\n                          expected:".._expected.."; value:"..val
			flow_plain_buf=flow_plain_buf.."\n                          expected:".._expected.."; value:"..val
		else
			self.file_content=time_stamp.." expected:".._expected.."; value:"..val
			flow_plain_buf=time_stamp.." expected:".._expected.."; value:"..val
		end
	end
end

function flow_log:push_item_result(val, _result,expected)
	if expected and expected~="" and expected~="N/A" and expected~="nil" then
		self:push_item_expect_val(expected, val)
	else
		self:push_item_numeric_val(val)
	end
	
	if _result and type(_result)=="string" then
		local time_stamp=gen_timestamp() 
		if self.file_content~="" then 
			time_stamp="\n"..time_stamp
			self.file_content=self.file_content..time_stamp.." ".._result
			flow_plain_buf=flow_plain_buf..time_stamp.." ".._result
		else
			self.file_content=time_stamp.." ".._result
			flow_plain_buf=time_stamp.." ".._result
		end
	end
end

function flow_log:push_report(_msg, _data)
	if _msg=="on_seq_start" then
		self.f = io.open(self.file_name, "w")
		self:push_test(get_stationid(),get_siteid(),get_logdir())
		
	elseif _msg=="on_item_start" then
		--self:push_item(tostring(_data.group),tostring(_data.tid),tostring(_data.unit),tostring(_data.low),tostring(_data.high),tostring(_data.to_pdca))
		self:write_item_start_msg(tostring(_data.group),tostring(_data.tid),tostring(_data.unit),tostring(_data.low),tostring(_data.high),tostring(_data.to_pdca))
	
	elseif _msg=="on_item_end" then
		--self:push_item_result(_data.value,_data.result,nil)
		local r = tostring(_data.result)
		if string.lower(r) == "true" then
			r = "PASS"
		elseif string.lower(r) == "false" then
			r = "FAIL"
		elseif string.lower(r) == "-1" then
			r = "ERROR"
		elseif string.lower(r) == "2" then
			r = "SKIP"
		else
			r = "UNKNOWN"
		end
		--self:push_item_result(tostring(_data.value),r,nil)
		self:write_item_end_msg(tostring(_data.value),r,nil)
		
		
	elseif _msg=="on_seq_end" then
		local result = _data.result --overall pass/fail result of the sequence. 0 is fail, 1 is pass, -1 is aborted
		local err = _data["error"] --this field only exists if the result -1.
		local logs = _data.logs --A list of log paths from this run
		
		--self:gen_flow_log()
		self.file_name = string.gsub(self.file_name, "flow", "flow_plain")
		self.file_content = flow_plain_buf
		self:to_file()
		flow_plain_buf = ""
		
		self:reset()
		
	elseif _msg=="on_uart_msg" then
		--self:push_uart_msg(_data)
		self:write_uart_msg(_data)
	
	elseif _msg=="on_hw_msg" then
		--self:push_tester_action(_data)
		self:write_hw_msg(_data)
	end
end

function flow_log:parse_timestamp()
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
				return "error on parsing time stamp"
			end 
		end)
		self.file_content = new
	end
end

function flow_log:write_uart_msg(_msg)
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
	if formatted and self.f then
		self.f:write(formatted)
		self.f:flush()
	else
		print("<error> unable to open uart file: ", self.file_name)
	end
end

function flow_log:write_hw_msg(_tester_action)
	local msg=nil
	if _tester_action then
		local time_stamp=gen_timestamp() 
		time_stamp="\n"..time_stamp
		msg = time_stamp.." [Tester]: ".._tester_action
	end	
	if msg and self.f then
		self.f:write(msg)
		self.f:flush()
	else
		print("<error> unable to open file: ", self.file_name)
	end
end

function flow_log:write_item_end_msg(_val, _result,expected)
	local msg=nil
	if _val then
		msg="\n                          lower:"..self.low.."; upper:"..self.high.."; value:".._val
		flow_plain_buf = flow_plain_buf.."\n                          lower:"..self.low.."; upper:"..self.high.."; value:".._val
	else
		msg="\n                          lower:"..self.low.."; upper:"..self.high.."; value:"
		flow_plain_buf = flow_plain_buf.."\n                          lower:"..self.low.."; upper:"..self.high.."; value:"
	end
	
	if _result and type(_result)=="string" then
		if msg==nil then msg="" end
		local time_stamp=gen_timestamp() 
		time_stamp="\n"..time_stamp
		msg=msg..time_stamp.."   ".._result
		flow_plain_buf=flow_plain_buf..time_stamp.." ".._result
	end
	
	if msg and self.f then
		self.f:write(msg)
		self.f:flush()
	else
		print("<error> unable to open file: ", self.file_name)
	end
end


function flow_log:write_item_start_msg(_group,_tid,_unit,_low,_high,_pdca_flag)
	local msg=nil
	if _group then
		msg="\n==Test: ".._group
		flow_plain_buf = string.format("%s%s%s",flow_plain_buf,"\n==Test: ",tostring(_group))
	end
	
	if _tid then
		if msg==nil then msg="" end
		msg=msg.."\n==SubTest: ".._tid
		flow_plain_buf = string.format("%s%s%s",flow_plain_buf,"\n==SubTest: ",tostring(_tid))
	end
	
	if msg and self.f then
		self.f:write(msg)
		self.f:flush()
	else
		print("<error> unable to open file: ", self.file_name)
	end
	
	if _high then self.high=_high end
	if _low then self.low=_low end
	if _unit then self.unit=_unit end
	
end
