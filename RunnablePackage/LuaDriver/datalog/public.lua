function get_stationid()
	--return "123"
	local name = tostring(station_name)
	name = string.gsub(name, " ", "_")
	return name
end

function get_siteid()
	return tostring(instance_id)
end

function get_logdir()
	return "/vault/Intelli_log/"
end

function get_mlbsn()
	--return "12345678901234567"
	return datalog_mlbsn
end

function set_mlbsn(_sn)
	datalog_mlbsn=_sn
end

function clear_mlbsn()
	datalog_mlbsn = ""
end

function get_panelsn()
	return "nil"
end

function get_config()
	local config = tostring(station_name)
	config = string.gsub(config, " ", "_")
	return config
end
-- Generate time stamp in micro second
function gen_timestamp(_precision)
	local precision = 5
	if _precision then precision=_precision end
	local format = string.format("%%.%df", precision)
	local t = string.format(format, socket.gettime())
	local s = string.match(t,"(%d+)%.?")
	local u = string.match(t,"%.(%d%d%d%d%d)") 
	return os.date("%Y/%m/%d %H:%M:%S.", tonumber(s))..u
end

function split(str, pat)
   local t = {}
   local fpat = "(.-)" .. pat
   local last_end = 1
   local s, e, cap = str:find(fpat, 1)
   while s do
      if s ~= 1 or cap ~= "" then
     table.insert(t,cap)
      end
      last_end = e+1
      s, e, cap = str:find(fpat, last_end)
   end
   if last_end <= #str then
      cap = str:sub(last_end)
      table.insert(t, cap)
   end
   return t
end

--r format: 2015-12-07 18:46:11
function get_time_by_date(r)
    local a = split(r, " ")
    local b = split(a[1], "-")
    local c = split(a[2], ":")
    local t = os.time({year=b[1],month=b[2],day=b[3], hour=c[1], min=c[2], sec=c[3]})
    return t
end

--date format: 12-04_10:12:01.516137
function seq_date_to_sec(date)
	local t = date
	t = string.match(t,"%d%d%-%d%d_%d%d:%d%d:%d%d")
	if t then
		t = string.gsub(t,"_"," ")
		local y = os.date("%Y")
		t = y.."-"..t
		t = get_time_by_date(t)
	else
		t = 0
	end
	return t
end
