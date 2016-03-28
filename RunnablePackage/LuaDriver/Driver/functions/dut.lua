--- DUT Function
-- @module functions.dut
-- @alias dut_module
local dut_module = {}

local sequence_utils = require("utils.sequence_utils")
local string_utils = require("utils.string_utils")
local time_utils = require("utils.time_utils")
local global_data = require("utils.global_data")
local console = require("tcpAndZmq")--CONFIG.DUT_UART_CONSOLE)
bit32 = require("bit32")
--===========================--
-- Module local variables
--===========================--
local _PDCA_KEY_MATCH_ = "{{(.-)}}"
local _last_diags_command = ""
local _last_diags_response = ""
local TemplatePatternTable = {}
-- local TemplateVariantTable = {}

--===========================--
--- Private Functions
-- @section private
--===========================--
local function addTimeStampMask(str)
  local t = {}
  local x = 0;
  local mask = "[0-9A-F]+@[RT]%d%s+"
  for v in string.gmatch(str..'\n', "(.-)[\r\n]+") do
    -- print(#v,v)
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

--- Parse diags template file and populate the match table with patterns
-- @param diags_command Diags command parsing template that should be processed
-- @return TemplatePatternTable reference
local function parse_template_file(diags_command)--par = file Name
  local fileName = diags_command..".txt"
  local file = io.open(CONFIG.TEMPLATE_PATH..fileName)
  if (file == nil) then
    file = io.open(CONFIG.TEMPLATE_ROOT_PATH..fileName)
  end

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
      -- string.gsub(matchTmp,'[\r\n]+','[\r\n]+')
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

--===========================--
--- Public API
-- @section public
--===========================--

--- Run the given smokey script on the device
-- @tparam sequence_object sequence FCT test sequence table
-- @param global_var_table table full of "global" test variables
-- @return true, or whether the returned data matches the expected value
-- @return passing status (whether the returned data matches the expected value)
-- @raise error
-- @see diags, parse
function dut_module.smokey(sequence, global_var_table)
    local old_command = sequence.param1
    sequence.param1 = "smokeyshell -f nandfs:\\AppleInternal\\Diags\\Scripts\\X527\\"..tostring(old_command)
    
    local retsult, passing = dut_module.diags(sequence, global_var_table)
    _last_diags_command = string.match(tostring(old_command),"(.-) ")

    sequence.param1 = old_command
    
    return result, passing
end

--- Run the given diags command on the device
-- @tparam sequence_object sequence FCT test sequence table
-- @param global_var_table table full of "global" test variables
-- @return true, or whether the returned data matches the expected value
-- @return passing status (whether the returned data matches the expected value)
-- @raise error
-- @see parse
function dut_module.diags(sequence, global_var_table)
  local timeout = tonumber(sequence.timeout)  --3sec
  if(timeout == nil) then 
      timeout = 5000
  end

  local command = global_data.sub(global_var_table, sequence.param1)
  _last_diags_command = command
  console._Dut_Read_String_()
  if(command) then
    if string.match(command, "sleep") ~= nil or string.match(command, "playaudio") ~= nil or string.match(command, "loopaudio") ~= nil then
      console._Dut_Send_String_(command)
    else
      local ret, errmsg = console._Dut_Send_Cmd_(command, timeout) --this function will set detect to ] :-)
      _last_diags_response = console._Dut_Read_String_()
      if(ret ~= 0) then return "--FAIL--diags timeout" end
      -- here need to replace xxxxxxx\r return format data
      _last_diags_response = string.gsub(tostring(_last_diags_response), "[0-9A-F]+@[RT]%d%s+\r", "")
      _last_diags_response = string.gsub(tostring(_last_diags_response), "[0-9A-F]+@[RT]%d%s+\n", "")
      -- print(" < DUT ReadResp: >\n",_last_diags_response)
    end
  else
    error("ERROCODE[-6]Problem with diags command string or variable substitution: "..command)
  end
  local result = sequence_utils.check_string_match_limit(_last_diags_response, sequence)
  return result, result
end

--- Spin until timeout or we detect a string (given in the sequence)
-- @tparam sequence_object sequence FCT test sequence table
-- @return true
-- @return passing status (true)
-- @raise error
function dut_module.detect(sequence) --par = "xxx"
  -- local det = string_utils.replace_special_char(sequence.param1)
  local det = sequence.param1
  local timeout = tonumber(sequence.timeout)
  if timeout==nil then timeout=30000 end
  print("< Note > : wait to detect...'"..det.."'")
  -- console._Dut_Read_String_()
  console._Dut_Set_Detect_String_(det)

  local st, msg = console._Dut_Wait_For_String_(timeout)
  if st ~= 0 then
    error("ERRCODE[-7]Timeout when waiting for ".. tostring(det) ..". Time out due to: "..tostring(msg))
  end

  local ret = console._Dut_Read_String_()
  if string.match(det, "%:%-%)") then
    local times = 0;
    for v in string.gmatch(ret, "(%:%-%))") do  --search how many :-) got
      times = times + 1
    end
    local firstLine = string.match(ret, "(.-)[\r\n]+")
    if(string.match(firstLine, "%:%-%)") and times==1) then --only on :-) and in the first line,detect again
      st, msg = console._Dut_Wait_For_String_(timeout)
      if st ~= 0 then
        error("ERRCODE[-7]Timeout when waiting for ".. tostring(det) ..". Time out due to: "..tostring(msg))
      end
    end
  end

  console._Dut_Read_String_()
  console._Dut_Set_Detect_String_("%:%-%)")
  --time_utils.delay(1000)

  return true, true
end


--- Parse serial output from the previous diags command and extract values
--  using a parsing template.
-- @tparam sequence_object sequence FCT test sequence table
-- @param global_var_table table full of "global" test variables
-- @return result of parsing the serial 
-- @return passing status (whether the returned data matches the expected value or is within the expected range)
-- @see diags, smokey
-- @raise error
function dut_module.parse(sequence, global_var_table) --params = {fileName, keyName} --In keyName, including {{}}
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
    
    -- for i=1, #_last_diags_response do
    --   io.write(string.format("%02X",string.byte(string.sub(_last_diags_response,i,i))))
    --   io.write(" ")
    -- end
    -- print("*********************\n")
  local value = string.match(_last_diags_response, TemplatePatternTable[diags_command][keyName])
  if value == nil then 
    global_data.set_from_param(global_var_table, sequence.param2, "NULL")
    --error("ERRCODE[-11]Could not match pattern for '"..keyName.."' for command: ".._last_diags_command)
    value = "--FAIL--ERRCODE[-11]Could not match pattern for '"..keyName.."' for command: ".._last_diags_command .." DUT Got:"..string.gsub(tostring(_last_diags_response),"[\r\n]+"," ")
    print("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~DUT parse failed~~~~~~~~~~~~~~~~~~~~~~~~~~")
    print("DUT response:");
    print(_last_diags_response);
    print("Match String:")
    print(TemplatePatternTable[diags_command][keyName])
    print("Diags Command:")
    print(_last_diags_command)
    print("KeyName:")
    print(keyName)
    print("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~DUT parse~~~~~~~~~~~~~~~~~~~~~~~~~~")
  end

  value = string.gsub(tostring(value), "[\r\n]", "") 
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

--- Evaluate the given lua expression, substituting any variables in {{}} braces.
-- @tparam sequence_object sequence FCT test sequence table
-- @param global_var_table table full of "global" test variables
-- @return result of executing the lua expression
-- @return true
-- @see diags, smokey
-- @raise error
function dut_module.calculate(sequence, global_var_table) 
  -- local command = global_data.sub(global_var_table, sequence.param1)
  local command = global_data.sub(global_var_table, string.match(sequence.param1, "(%(.+%))"))
  if string.match(string.upper(tostring(command)),"NONE") then
    return("--FAIL--ERRCODE[-997]None is invalide for calculate function : "..tostring(command))
  elseif string.match(string.upper(tostring(command)),"ERRCODE[") then
    return("--FAIL--ERRCODE[-996]Invalide parameter for calculate function : " .. tostring(command))
  end
  -- local result = loadstring("return ("..command..")")()
  local f,err = loadstring("return ("..command..")")
  assert(f,err)
  local result = f()
  
  global_data.set_from_param(global_var_table, sequence.param2, result)
  
  return result, sequence_utils.check_numerical_limits(result, sequence)
end

return dut_module
