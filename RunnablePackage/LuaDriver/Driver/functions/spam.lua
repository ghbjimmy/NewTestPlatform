--- DMM Function
-- @module functions.dmm
-- @alias dmm_module
local dmm_module = {}

local console = require("tcpAndZmq")--CONFIG.DUT_UART_CONSOLE)
local spam = require(CONFIG.SPAM_CONSOLE)
local time_utils = require("utils.time_utils")
require "pathManager" 
local templeteFile = CurrentDir().."/.templete.txt"
print(templeteFile)

-- spam._Arm_Socket_Connect_()

local spam_module = {}

local TestEngineLog = require("utils.LogFile")

local function  random(n,m)
  math.randomseed(os.clock()*math.random(1000000,90000000) + math.random(1000000,90000000))
  return math.random(n,m)
end

local function randomNumber(len)
  local rt = ""
  for i=1, len do 
    if(i==1) then
      rt = rt .. random(1,9)
    else
      rt = rt .. random(0,9)
    end
  end
  return rt
end

local function randomLower(len)
  local rt = ""
  for i=1, len do
    rt = rt .. string.char(random(97,122))
  end
end

local function randomUpper(len)
  local rt = ""
  for i=1, len do
    rt = rt .. string.char(random(69,90))
  end
end 

local RandomMode = {
  UPPER = 1,--default
  LOWER = 2,
  UPPER_LOWER = 3,
  NUMBER = 4,
  UPPER_NUMBER = 5,
  LOWER_NUMBER =6,
  ALL = 7,
}

local function randomString(len, mode)
  local upper = "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
  local lower = "abcdefghijklmnopqrstuvwxyz"
  local number = "0123456789"
  local maxlen = 0
  local templete = ""

  
  if mode==RandomMode.LOWER then
    templete = lower
    maxlen = 26
  elseif mode==RandomMode.UPPER_LOWER then
    templete = upper .. lower
    maxlen = 52
  elseif mode==RandomMode.NUMBER then
    templete = number
    maxlen = 10
  elseif mode==RandomMode.UPPER_NUMBER then
    templete = upper .. number
    maxlen = 36
  elseif mode==RandomMode.LOWER_NUMBER then
    templete = lower .. number
    maxlen = 36
  elseif mode==RandomMode.ALL then
    templete = upper .. lower .. number
    maxlen = 62
  else
    templete = upper
    maxlen = 26
  end
  local str = {}
  for i=1, len do
    local  index = random(1,maxlen)
    str[i] = string.sub(templete, index, index)
  end
  return table.concat(str,"")
end 

local function compareStr(ori, comp)
  local mismatch = 0
  for i=1, #ori do
    local oriStr = string.sub(ori, i, i)
    local readStr = ""
    if(#comp >= i) then
      readStr = string.sub(comp, i, i)
    end
    if(oriStr ~= readStr) then
      mismatch = mismatch + 1
    end
  end

  if(#comp> #ori) then
    mismatch = mismatch + (#comp - #ori)
  end
  return mismatch
end

local function writeTemplete(p, content)
  local f = io.open(p, "w")
  if(f) then
    f:write(content)
    f:flush()
    f:close()
  end
end

local function readTemplete( p )
  local f = io.open(p, "r")
  local str = nil
  if(f) then
    str = f:read("*a")
    f:close()
  end
  return str
end

function spam_module.spamloopback(sequence, global_var_table)
  spam._Arm_Socket_Connect_()
  local len = 10
  if(sequence.param1 and tonumber(sequence.param1)) then
    len = math.ceil(tonumber(sequence.param1))
  end
  if(len < 1) then len = 10 end
  TestEngineLog.write(EngineLogName,"\t Clear SPAM buffer", true)
  -- spam.Clr_spam_buff()

  local str = "asdfgh" .. randomString(len, RandomMode.ALL)
  TestEngineLog.write(EngineLogName,"\t Clear UART buffer", true)
  console._Dut_Read_String_()

  TestEngineLog.write(EngineLogName,"\t < SPAM Send > \t"..str, true)
  local ret = spam._ARM_Send_Cmd_(str)  
  TestEngineLog.write(EngineLogName,"\t < SPAM Receive > \t"..ret, true)

  --time_utils.delay(500)
  local ret_uart = console._Dut_Read_String_()
  TestEngineLog.write(EngineLogName,"\t < UART Receive > \t"..ret_uart, true)

  spam._Arm_Socket_Close_()

  local err = compareStr(str, ret)--spam return
  if(err > 0) then
    return false, false
  end

  if string.match(ret_uart, str) then
    return true, true
  end
  
  return false, false
end

return spam_module
