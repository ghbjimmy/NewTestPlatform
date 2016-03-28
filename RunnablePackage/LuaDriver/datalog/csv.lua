package.path = package.path.."./?.lua"
require "public"
require "protocol"
require "log"
require "flow"
require "uart"
require "hw"


-------------------------------
-- csv log
--If module_ver is changed, should reset file_content and create a new csv log file
--The test id must be one word so no space is allowed
-------------------------------
CSVLog = Log:new()
csv_log = CSVLog:new(
{
	--file
	log_type="csv",
	index_test=1,
	index_item=1,
	vendor="Intelligent",
	sw_ver="v2.1",
	module_id="ia473",
	module_ver="v1.2",
	line_number="01",
	station_number="01",
	time_stamp="",--used for csv file name
	log_dir="/vault/Intelli_log/",
	need_reset=false,

	--test
	ict_os_items="",
	ict_mda_items="",
	rail_short_items="",
	panel_sn={},
	mlbsn={},
	config={},
	station_id={},
	site_id={},
	state={},
	err_msg={},
	failed_list={},
	start_time={},
	total_time={},

	--item
	items_name={},
	items_upper={},
	items_lower={},
	items_unit={},
	items_val={},--[t1:1,2,...n,t2:1,2,...n,]
	
	poller = nil,
	seq_sub = nil,
})

function csv_log:reset()
	--file
	self.index_test=1
	self.index_item=1
	self.vendor="Intelligent"
	self.sw_ver="v2.1"
	self.module_id="ia473"
	self.module_ver="v1.2"
	self.line_number="01"
	self.station_number="01"
	self.time_stamp=""
	self.log_dir="/vault/Intelli_log/"
	self.need_reset=false

	--test
	self.ict_os_items=""
	self.ict_mda_items=""
	self.rail_short_items=""
	self.panel_sn={}
	self.mlbsn={}
	self.config={}
	self.station_id={}
	self.site_id={}
	self.state={}
	self.err_msg={}
	self.failed_list={}
	self.start_time={}
	self.total_time={}

	--item
	self.items_name={}
	self.items_upper={}
	self.items_lower={}
	self.items_unit={}
	self.items_val={}
	
	self.file_name = ""
end

function csv_log:init(_poller, _sub)
	self.poller = _poller
	self.seq_sub = _sub
	self.seq_sub:connect()
	poller:add(self.seq_sub.sock, zmq.POLLIN, function ()	
		local msg = self.seq_sub:recv()
		if msg then
			if type(msg)=="table" then
				--for k,v in pairs(msg) do print("<csv_sub:recv> ",k,v) end
			elseif type(msg)=="string" then
				--print("<csv_sub:recv> ",msg)
			end
			self:dipatch_msg(msg)
		end
	end
	)
	--print("finshed csv_log init.")
end

function csv_log:clear_content()
	self.file_content = ""
end

function csv_log:do_csv_header()
	--first header line
	local csv_header = self.vendor..","..self.sw_ver..","..self.module_ver;
	csv_header = csv_header..string.rep(",", 10-2)
	csv_header = csv_header.."\r\n"
	
	--second line
	csv_header = csv_header.."Panel_SN,SerialNumber,Config,Station_ID,Site_ID,PASS/FAIL,Error_Message,Failed_List,Test Start Time,Test Stop Time,";
	csv_header = csv_header..self.ict_os_items;--open/short
	csv_header = csv_header..self.ict_mda_items;--mda
	csv_header = csv_header..self.rail_short_items;--rail short
	csv_header = csv_header..table.concat(self.items_name,",").."\r\n";--functional test names
	
	csv_header = csv_header.."Upper Limited----------->,,,,,,,,,,"..table.concat(self.items_upper,",").."\r\n";--third line
	csv_header = csv_header.."Lower Limited----------->,,,,,,,,,,"..table.concat(self.items_lower,",").."\r\n";--fourth line
	csv_header = csv_header.."Measurement unit------>,,,,,,,,,,"..table.concat(self.items_unit,",").."\r\n";--fifth line
	
	self.file_content = csv_header
end

function csv_log:do_serialize()
	local item_count_of_a_test = self.index_item -1
	self:do_csv_header()
	for i=1,self.index_test do
		local values = {}
		for k=1, item_count_of_a_test do
			--print(i, k)
			local index = (i-1)*item_count_of_a_test+k
			if index<=#self.items_val then
				values[#values+1] = self.items_val[index]
			else
				values[#values+1] = ""
			end
		end
		self.file_content = self.file_content..string.format("%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s\r\n", self.panel_sn[i],self.mlbsn[i],self.config[i],self.station_id[i],self.site_id[i],self.state[i],self.err_msg[i],self.failed_list[i],self.start_time[i],self.total_time[i],table.concat(values,","))
	end
end

function csv_log:gen_csv_log()
	if self:exist_file() then
		self:append_line_to_file()
	else
		self:do_serialize()
		self:to_file()
	end
end

function csv_log:append_line_to_file()
	local item_count_of_a_test = self.index_item -1
	local current_test_index = self.index_test
	local values = {}
	for k=1, item_count_of_a_test do
		local index = (current_test_index-1)*item_count_of_a_test+k
		if index<=#self.items_val then
			values[#values+1] = self.items_val[index]
		else
			values[#values+1] = ""
		end
	end
	
	local i = current_test_index
	local new_content = string.format("%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s\r\n", self.panel_sn[i],self.mlbsn[i],self.config[i],self.station_id[i],self.site_id[i],self.state[i],self.err_msg[i],self.failed_list[i],self.start_time[i],self.total_time[i],table.concat(values,","))

	local f = io.open(self.file_name, "a+")
	assert(f, "Failed to open file: "..tostring(self.file_name))
	local ret = f:write(new_content)
	f:flush()
	f:close()
end

function csv_log:gen_name()
	local date = os.date("%Y-%m-%d")
	if date~=self.time_stamp then
		if self.time_stamp=="" then
			--do nothing
		else
			self.need_reset = true
		end
		self.time_stamp = date
	end
	self:set_filename(string.format("%s%s_%s@%s_%s@%s.csv",self.log_dir,self.module_id,self.module_ver,self.line_number,self.station_number,self.time_stamp))
	print("csv file name = ",self.file_name)
end

function csv_log:push_test(_stationid,_siteid,_logdir)
	if self.index_test == 1 then
		if _logdir then self.log_dir=_logdir end
	end
	if _stationid then self.station_id[self.index_test]=_stationid end
	if _siteid then self.site_id[self.index_test]=_siteid end
end

function csv_log:push_item(_group,_tid,_unit,_low,_high,_pdca)
	local upper = "N/A"
	local lower = "N/A"
	local unit = "N/A"
	if _high~=nil and _high~="" and _high~="null" and _high~="nil" then upper = _high end
	if _low~=nil and _low~="" and _low~="null" and _low~="nil" then lower = _low end
	if _unit~=nil and _unit~="" and _unit~="null" and _unit~="nil" then unit = _unit end
	
	if self.index_test == 1 then		
		if _tid then self.items_name[self.index_item]=_tid end
		self.items_upper[self.index_item]=upper
		self.items_lower[self.index_item]=lower
		self.items_unit[self.index_item]=unit
	end
end

function csv_log:push_item_result(_tid,_value,_result,_error,_pdca)
	--print(string.format("_tid=%s, _value=%s, _result=%s, _error=%s",tostring(_tid),tostring(_value),tostring(_result),tostring(_error)))
	local value = _value
	if value then
		value = string.gsub(value, ",", "")
		value = string.gsub(value, "\r", "")
		value = string.gsub(value, "\n", "")
		value = string.gsub(value, "\'", "")
		value = string.gsub(value, "\"", "")
		value = string.gsub(value, "%-%-PASS%-%-", "PASS")
		value = string.gsub(value, "%-%-FAIL%-%-", "FAIL")
		self.items_val[#self.items_val+1]=value
	else
		self.items_val[#self.items_val+1]=""
	end
	
	if _pdca=="true" then
	end
	
	if self.index_test == 1 then
		self.index_item = self.index_item + 1 --collect all items header for first unit
	end
	
	if tostring(_result)=="false" or tostring(_result)=="0" then
		self.failed_list[self.index_test] = self.failed_list[self.index_test]..tostring(_tid).." ; "
	end
	
	if tostring(_result)=="-1" then
		self.err_msg[self.index_test] = self.err_msg[self.index_test]..tostring(_error).." ; "
	end
end

function csv_log:push_report(_msg, _data)
	if _msg=="on_seq_start" then
			
		---********OPP can not parse the csv with this kind of timestamp
		--self.start_time[self.index_test]=_data.st --save time stamp
		local date = os.date("%Y-%m-%d %H:%M:%S", os.time())
		self.start_time[self.index_test]=date
		self:push_test(get_stationid(),get_siteid(),get_logdir())
		
		self.failed_list[self.index_test]=""
		self.err_msg[self.index_test]=""
		
	elseif _msg=="on_item_start" then
		--{"data": {"group": "INTELLIGENT", "to_pdca": false, "high": "", "low": "", "tid": "INTEL_HOG_100_STAT_UNITSTAGE", "unit": ""}, "event": 2}
		self:push_item(_data.group,_data.tid,_data.unit,_data.low,_data.high,_data.to_pdca)
	
	elseif _msg=="on_item_end" then
		--{"data": {"tid": "INTEL_HOG_100_STAT_UNITSTAGE", "to_pdca": false, "result": true, "value": "POSTSMT2"}, "event": 3}
		self:push_item_result(_data.tid,_data.value,_data.result,_data["error"],_data.to_pdca)
		
	elseif _msg=="on_seq_end" then
		local result --overall pass/fail result of the sequence. 0 is fail, 1 is pass, -1 is aborted
		if _data.result == 1 then
			result="PASS"
		elseif _data.result == 0 then
			result="FAIL"
		elseif _data.result == -1 then 
			result="aborted"
		end
		local err = _data["error"] --this field only exists if the result -1.
		if err then
			err = string.gsub(err, ",", "")
			err = string.gsub(err, "\r", "")
			err = string.gsub(err, "\n", "")
			err = string.gsub(err, "\'", "")
			err = string.gsub(err, "\"", "")
		end
		
		local logs = _data.logs --A list of log paths from this run
		
		local mlbsn = get_mlbsn()
		local panel_sn = get_panelsn()
		local config = get_config()
		
		if panel_sn then self.panel_sn[self.index_test]=panel_sn else self.panel_sn[self.index_test]="" end
		if mlbsn then self.mlbsn[self.index_test]=mlbsn else self.mlbsn[self.index_test]="" end
		if config then self.config[self.index_test]=config else self.config[self.index_test]="" end
		if result then self.state[self.index_test]=result else self.state[self.index_test]="" end
		--if err then self.err_msg[self.index_test]=err else self.err_msg[self.index_test]="" end
		--if fail_list then self.failed_list[self.index_test]=fail_list else self.failed_list[self.index_test]="" end
		
		---********OPP can not parse the csv with this kind of timestamp
		--local t0 = seq_date_to_sec(self.start_time[self.index_test])
		--local t = seq_date_to_sec(_data.st)
		--self.total_time[self.index_test]= t-t0 --total test time changes to stop time
		
		local date = os.date("%Y-%m-%d %H:%M:%S", os.time())
		self.total_time[self.index_test] = date
		
		if self.file_name=="" then self:gen_name() end
		self:gen_csv_log()
		self.index_test = self.index_test + 1
	end
end


-- ************item event check for sequencer bug************
local previous_item_event = ""
local function reset_item_event()
	previous_item_event = ""
end
local function check_item_event(_current_item_event)
	local ret = true
	if _current_item_event == "ITEM_START" then
		if previous_item_event=="" or previous_item_event=="ITEM_END" then
			ret = true
		else
			ret = false
		end
		
	elseif _current_item_event == "ITEM_END" then
		if previous_item_event=="ITEM_START" then
			ret = true
		else
			ret = false
		end
	end
	previous_item_event = _current_item_event
	return ret
end

function csv_log:dipatch_msg(_msg)
	local msg=_msg
	if msg then
		local frame = {channel="",time_stamp="",level="",identity="",data="",}
		if type(msg) == "table" then
			if msg[1] then frame.channel = msg[1] end
			if msg[2] then frame.time_stamp = msg[2] end
			if msg[3] then frame.level = msg[3] end
			if msg[4] then frame.identity = msg[4] end
			if msg[5] then frame.data = msg[5] end
		
			if tonumber(frame.level)==REPORTER then
				local obj, pos, err = json.decode(frame.data, 1, nil)
				if err then
					print("<error>: ",err) 
				else 
					local event = tonumber(obj.event)
					local data = obj.data
					data["st"] = frame.time_stamp
					--for k,v in pairs(data) do print(k,v) end
					if event==SEQUENCE_START then
						print("<SEQUENCE_START>")
						clear_mlbsn()	
						reset_item_event()
						
						flow_log:gen_name()
						uart_log:gen_name()
						hw_log:gen_name()
						self:gen_name()
						
						flow_log:to_file()
						uart_log:to_file()
						hw_log:to_file()
						
						--generate csv file name only for sequencer's log path querying
						--self.file_name = string.gsub(self.file_name, "%.csv", "_default.csv")
						--self:to_file()
						
						self:push_report("on_seq_start",data)

						flow_log:push_report("on_seq_start",data)
						uart_log:push_report("on_seq_start",data)
						hw_log:push_report("on_seq_start", _data)
						
						
					elseif event==SEQUENCE_END then
						print("<SEQUENCE_END>")
						self:push_report("on_seq_end",data)
						
						flow_log:push_report("on_seq_end",data)
						uart_log:push_report("on_seq_end",data)
						hw_log:push_report("on_seq_end", _data)
						
						if self.need_reset==true then
							self:reset()
						end
						
						collectgarbage("collect")
						
					elseif event==ITEM_START then
						print("<ITEM_START>")
						if check_item_event("ITEM_START")==false then
							print("<warning>: Received more than 1 ITEM_START event continuously.")
							local dummy_item_end_data = {tid="item_start_event_error",value="item_start_event_error",result="item_start_event_error",error="item_start_event_error",to_pdca="false"}
							self:push_report("on_item_end",data)
						end
						self:push_report("on_item_start",data)
						
						flow_log:push_report("on_item_start",data)
						
						
					elseif event==ITEM_END then
						print("<ITEM_END>")
						if check_item_event("ITEM_END")==false then
							print("<warning>: Received more than 1 ITEM_END event continuously.")
							local dummy_item_start_data = {group="item_end_event_error",tid="item_end_event_error",unit="item_end_event_error",low="item_end_event_error",high="item_end_event_error",to_pdca="false"}
							self:push_report("on_item_start",data)
						end
						
						self:push_report("on_item_end",data)
						flow_log:push_report("on_item_end",data)
						
						
					elseif event==ATTRIBUTE_FOUND then
						--print("<ATTRIBUTE_FOUND>")
						local name = string.upper(tostring(data.name))
						if name=="MLBSN" or name=="SERIALNUMBER" or name=="SERIAL NUMBER" or name=="SERIAL_NUMBER" or name=="SERIAL-NUMBER" then
							local sn=tostring(data.value)
							set_mlbsn(sn)
							flow_log.mlbsn=sn
							uart_log.mlbsn=sn
							hw_log.mlbsn=sn
							
							--local dft_csv_name = self.file_name
							local dft_flow_name = flow_log.file_name
							local dft_uart_name = uart_log.file_name
							local dft_hw_name = hw_log.file_name
							
							--self:gen_name()
							flow_log:gen_name()
							uart_log:gen_name()
							hw_log:gen_name()
							
							--os.execute(string.format("mv %s %s", dft_csv_name, self.file_name))
							os.execute(string.format("mv %s %s", dft_flow_name, flow_log.file_name))
							os.execute(string.format("mv %s %s", dft_uart_name, uart_log.file_name))
							os.execute(string.format("mv %s %s", dft_hw_name, hw_log.file_name))
						end
	
					else
						print("<error>: unknow report event") 
					end 
				end
			end
		end
	end
end
