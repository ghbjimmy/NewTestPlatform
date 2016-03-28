require "pathManager"

package.path = package.path..";"..deleteLastPathComponent(CurrentDir()).."/?.lua"
package.path = package.path..";"..deleteLastPathComponent(CurrentDir()).."/Test/?.lua"

print(package.path)
local lapp = require "pl.lapp"
local string_utils = require("utils.string_utils")
local global_data = require("utils.global_data")

local args = lapp [[
parse test 
    
    -t,--tempel     (default bootcfg.txt)   get temple file from mach folder.
    -s,--source     (default xxx)   		Source file from DUT respose
    <updates...>    (default "X=Y")         xxx
]]

local _PDCA_KEY_MATCH_ = "{{(.-)}}"
local _last_diags_command = ""
local _last_diags_response = ""
local TemplatePatternTable = {}

local function addTimeStampMask(str)
  local t = {}
  local x = 0;
  local mask = "[0-9A-F]+@[RT]%d%s+"
  for v in string.gmatch(str..'\n', "(.-)[\r\n]+") do
    print(#v,v)
    if(x ==0 and #v==0) then 
      t[#t+1] = v--the first line, if value is empty, do not add time stamp
    else
      t[#t+1] = mask .. v
    end
    x=x+1
  end
  if string.match(string.sub(str,-1),"[\r\n]") then
      t[#t+1]=''
    end
  return table.concat(t,"[\r\n]+")
end

local function parse_template_file(diags_command)--par = file Name
  local fileName = diags_command..".txt"
  local file = io.open(deleteLastPathComponent(CurrentDir()).."/match/"..fileName)

  if(file) then
    local keyTable = {}
    local patternTable = {}
    local str = file:read("*a")
    file:close()

    for key in string.gmatch(str, _PDCA_KEY_MATCH_) do--{{Phosphorus_ASIC_odr}}, delete {{}}
      table.insert(keyTable, key)
    end

    -- TemplateVariantTable[fileName] = keyTable--{"123.txt"={key1,key2,...}},key has no {{}}

    local strMatch = ""
    local sig = 1
    for i=1, #keyTable do
      st, ed = string.find(str, "{{"..keyTable[i].."}}", sig)
      txt = string.sub(str, sig, st-1)
      txt = string_utils.replace_special_char(txt)

      local endStr = string.sub(str, ed+1, ed+1)
      endStr = string_utils.replace_special_char(endStr)
      local endStr2 = string.sub(str, ed+2, ed+2)

      if(endStr2 ~= "{") then
        endStr2 = string_utils.replace_special_char(endStr2)
      else
        endStr2 = ""
      end

      matchTmp = strMatch .. txt .. "(.-)" .. endStr .. endStr2--string.sub(str, ed+1, ed+1)
      strMatch = strMatch .. txt .. ".-" .. endStr--string.sub(str, ed+1, ed+1)
      -- matchTmp = string.gsub(matchTmp,'[\r\n]+','[\r\n]+')
      matchTmp = addTimeStampMask(matchTmp)
      patternTable[keyTable[i]] = matchTmp--patternTable={"ABC"="xxxx(.-)",...}, "ABC", there is no "{{}}""
      
      sig = ed + 2
      -- print(keyTable[i], matchTmp)
    end

    TemplatePatternTable[diags_command] = patternTable--{"file.txt" = {key="..."}, ...} key has no {{}}

  else
    error("ERRCODE[-5]Failed to open template file: "..fileName)
  end

  return TemplatePatternTable
end

local function parse(sequence, global_var_table) --params = {fileName, keyName} --In keyName, including {{}}
  local diags_command = _last_diags_command --_last_diags_command global variant, changed by diags function
  local keyName = string.match(sequence.param1, _PDCA_KEY_MATCH_) --{{Phosphorus_ASIC_odr}},delete {{}}

  if keyName == nil then
    error("ERRCODE[-8]Invalid match key name or format: "..sequence.param1)
  end

  if TemplatePatternTable[diags_command] == nil then
    --scan file, and stor key. if the file is scaned, it will not scan again.
    --Also, cant do this when Engine start due to dont know which file need to scan
    parse_template_file(diags_command)
  end

  if TemplatePatternTable[diags_command] == nil then
    global_data.set_from_param(global_var_table, sequence.param2, "NULL")
    error("ERRCODE[-9]Parsing file does not exist for: ".._last_diags_command)
  end

  if TemplatePatternTable[diags_command][keyName] == nil then
    global_data.set_from_param(global_var_table, sequence.param2, "NULL")
    error("ERRCODE[-10]Match key '"..keyName.."' does not exist for command: ".._last_diags_command)
  end
    -- print("*************************")
    -- print("<  DUT returen > : \n"..tostring(_last_diags_response))
    -- print("<  KeyName  > : \n"..tostring(keyName))
    -- print("<  match parttern  > : \n"..tostring(TemplatePatternTable[diags_command][keyName]))
    -- print("<  ASCII  > : ")
    -- for i=1, #_last_diags_response do
    --   io.write(string.format("%02X",string.byte(string.sub(_last_diags_response,i,i))))
    --   io.write(" ")
    -- end
    -- print("*********************\n")
    --here need to replace xxxxxxx\r return format data
    _last_diags_response = string.gsub(_last_diags_response, "[0-9A-F]+@[RT]%d%s+\r", "")
    _last_diags_response = string.gsub(_last_diags_response, "[0-9A-F]+@[RT]%d%s+\n", "")
    -- for i=1, #_last_diags_response do
    --   io.write(string.format("%02X",string.byte(string.sub(_last_diags_response,i,i))))
    --   io.write(" ")
    -- end
    -- print("*********************\n")
  local value = string.match(_last_diags_response, TemplatePatternTable[diags_command][keyName])
  if value == nil then 
    global_data.set_from_param(global_var_table, sequence.param2, "NULL")
    error("ERRCODE[-11]Could not match pattern for '"..keyName.."' for command: ".._last_diags_command)
  end

  value = string.gsub(value, "[\r\n]", "") 
  global_data.set_from_param(global_var_table, sequence.param2, value)

  -- If we have numerical limits
  local passing = true
  -- if tonumber(sequence.low) or tonumber(sequence.high) then
  --   passing = sequence_utils.check_numerical_limits(value, sequence)
  -- else
  --   passing = sequence_utils.check_string_low_high_limit(value, sequence)
  -- end
  return value, passing
end


function getKeyFromTemp(par)
	local tempFile = deleteLastPathComponent(CurrentDir()).."/match/"..par
	print(tempFile)
	local f = io.open(tempFile)
	if(f==nil) then
		print("Open Tempel file error")
		return
	end
	local str = f:read("*a")
	f:close()
	local t = {}
	for v in string.gmatch(str,"{{(.-)}}") do
		t[#t+1] = v
	end
	return t
end

function readSourceFile(par)
	local file = io.open(par,"r")
	if(file==nil) then
		print("open file Error : "..tostring(par))
	end
	local str = file:read("*a")
	file:close()
	return str;
end

local sf = args.source
local tf = args.tempel
local cmd = string.match(tf,("(.-)%."))
local key = getKeyFromTemp(tf)
if(sf == "xxx") then
	sf = cmd..".txt"
end

local seq = {}
_last_diags_command =  cmd
_last_diags_response = readSourceFile(sf)
for i=1, #key do
	seq.param1 = '{{'..key[i]..'}}'
	print(key[i], "=",tostring(parse(seq)))
end

