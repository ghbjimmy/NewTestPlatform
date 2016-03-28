--- Speaker/Alert THDN Analysis Module
-- @module functions.thdn
-- @alias thdn_module

--===========================--
-- THDN exported module
--===========================--
local thdn_module = {}

local dutConsole = require(CONFIG.DUT_UART_CONSOLE)
local time_utils = require("utils.time_utils")
local fixtureConsole = require(CONFIG.FIXTURE_CONSOLE)
local sequence_utils = require("utils.sequence_utils")
local global_data = require("utils.global_data") -- global data mangement library
-- function relay_module.thdn(sequence_or_connections)

--===========================--
-- THDN Public API
--===========================--
local _thdn = nil
local amp = nil
local fre = nil
local errMsg = nil

local Left_Data={};
local Right_Data={};

local function Shift(NBits,Data,Flag,SBits)
	local NewData=0;
	if (Flag == "L") then --Left
		NewData = Data*(2^SBits);
	elseif (Flag == "R") then --Right
		NewData = math.floor(Data/(2^SBits));
	else
		assert(false,"Error Command!");
	end
	return NewData;
end

local function  RShift(Data,SBits)
	local NewData=0;
	NewData = math.floor(Data/(2^SBits));
	return NewData;
end

local function  LShift(Data,SBits)
	local NewData=0;
	NewData = Data*(2^SBits);
	return NewData;
end

local function  And(Bits,Data1,Data2)
	local NewData=0;
	local Temp = 2^Bits-1;
	
	if Temp < Data1 then
		assert(false,"Bits num too small!");
	end
	if Temp < Data2 then
		assert(false,"Bits num too small!");
	end
	for i=Bits-1,0,-1 do
		Temp=2^i;
		if Data1>=Temp then 
			Data1=Data1-Temp; 
			if Data2>=Temp then
				Data2=Data2-Temp; 
				NewData=NewData+Temp;
			end
		elseif Data2>=Temp then
			Data2=Data2-Temp;
		end
	end
	return NewData;
end

local function  Or(Bits,Data1,Data2)
	local NewData=0;
	local Flag=false;
	local Temp = 2^Bits-1;
	if Temp < Data1 then
		assert(false,"Bits num too small!");
	end
	if Temp < Data2 then
		assert(false,"Bits num too small!");
	end
	for i=Bits-1,0,-1 do
		Flag=false;
		Temp=2^i;
		if Data1>=Temp then 
			Data1=Data1-Temp; 
			Flag=true;	
		end
		if Data2>=Temp then
			Data2=Data2-Temp; 
			Flag=true;
		end
		if (Flag==true) then
			NewData=NewData+Temp;
		end
	end
	return NewData;
end

local function  Not(Bits,Data)
	local NewData;
	local Temp = 2^Bits-1;
	if Temp>= Data then
		NewData = 2^Bits-1-Data;
	else
		assert(false,"Bits num too small!");
	end
	return NewData;
end

local function OneByte (Byte_Data)
	local Type;
	if ( RShift(Byte_Data,7)  == 0x01) then
		if ( RShift(Byte_Data,6) == 0x02) then
			Type = "Header";
		else
			Type = "Error";
		end
	else
		Type = "Normal";
	end
	return Type;
end

local function NBytes (NByte_Datas)
	local LeftData = 0;
    local RightData = 0;
    errorOccur = false;
    Ret = true;
	Length = #NByte_Datas;
    for i=1,Length,1 do
        errorOccur = false;
        if("Header" == OneByte(NByte_Datas[i])) then --left data
            LeftData = 0;
            RightData = 0;
            if i+6<=Length then --whole frame data --7Bytes          
                for k=i+1,i+6,1 do   --Check Data             
                    if("Error" == OneByte (NByte_Datas[k])) then                                 
                        errorOccur = true;
                        Ret = false;
                        i=k;
                        break;
                    end
                end					 --Check Data 
                if (errorOccur == false) then
					LeftData  =  LShift( And(8,NByte_Datas[i],0x3F),18) +  LShift( And(8,NByte_Datas[i+1],0x7F),11) +  LShift( And(8,NByte_Datas[i+2],0x7F),4) +  RShift( And(8,NByte_Datas[i+3],0x78),4);     
					RightData =  LShift( And(8,NByte_Datas[i+3],0x07),21) +  LShift( And(8,NByte_Datas[i+4],0x7F),14) +  LShift( And(8,NByte_Datas[i+5],0x7F),7) +  And(8,NByte_Datas[i+6],0x7F);
					Left_Data[#Left_Data + 1] = LeftData;
					Right_Data[#Right_Data + 1] = RightData;
				end
            end
        end
	end
    return Ret
end

local function Get_Data(thdn)
	local str=tostring(thdn)
	local block = {};
	local table1 = {};
	local x = #block;
	local conver_data = ""; 
	for k in string.gmatch(str, "ACK%(.-,.-,(.-)%)") do 				--抓取数据源内（）中第三个到最后一个数据到block
		table.insert(block,k);
	end
	
    local x = #block;
	for j = 1 ,x-2 do 													--删除最后table中最后一组数据“时间”,并拼成一个string
		conver_data = conver_data .. block[j] .. ",";
	end
	conver_data = conver_data .. block[x-1];

	for i in string.gfind(conver_data,"%w+") do 						--将字符串convert_data中数据插入table1
		table1[#table1+1]=tonumber(i,16);
	end

	return table1;
end 

local function Save_Data(path)
	local final="";
	local left=""--"Left"..",";
	local right=""--Right"..",";
	-- local f=assert(io.open(path,"r"),"open file error");
	for i in ipairs(Left_Data) do
		left=left..string.format("0x%06x",Left_Data[i])..",";
	end

	for j in ipairs(Right_Data) do
		right=right..string.format("0x%06x",Right_Data[j])..",";
	end
	-- final=left..right.."\r\n";	
	-- local NewTHDN=tostring(final);
	-- return NewTHDN;			--返回字符串NewTHDN
	return tostring(right),tostring(left)
end

local function Data_Operation(str)
	Left_Data={};
	Right_Data={};
	-- value=Get_Data(str);
	NBytes(Get_Data(str));
	return Save_Data();
end


--- NOT IMPLEMENTED: Measure the THDN of the [param1] net.
-- @tparam sequence_object sequence FCT test sequence table
-- @param global_var_table table full of "global" test variables
-- @return measured THDN
-- @return passing status (whether the returned value is within the expected range)
-- @raise error
function thdn_module.thdn_Old(sequence, global_var_table)
	--FIXME: Implement THDN
	-- error("THDN Not implemented yet.")
	local timeoutFlag = nil
	local cmd_thdn = "playaudio -b socmca -p ap-mca0 --len 6000 —freq 1000\r\n"
	if(sequence.param1 and sequence.param1~= "") then
	   cmd_thdn = string.format("playaudio -b socmca -p ap-mca0 --len 6000 —freq %s\r\n",sequence.param1)
	end   
	print("cmd_thdn__________:"..tostring(cmd_thdn))
	dutConsole._Dut_Send_String_(cmd_thdn)
	time_utils.delay(500)
	local thdn_cmd = "thdn measure()\r\n"
	fixtureConsole._ARM_Send_String_(thdn_cmd)
	local dt_str = ""
	local t1 = os.time()
	while(1) do
	 	local dt,rcvState= fixtureConsole._ARM_Socket_Receive_()
		-- if(rcvState == "timeout") then assert(nil,"ARM Response Timeout") end
		-- if(rcvState == "closed") then assert(nil,"ARM Port connection closed.") end
		if(dt) then 
		  dt_str = dt_str .. dt
		  if(string.match(dt,"DONE")) then
			  break
		  end
		  local t2 = os.time()
		  if(t2-t1 > 100) then 
			  timeoutFlag = 1
			  break 
		  end
		else
			-- return nil,false  
			error("ERRCODE[-36]No response from  MCU") 
		end
	end
	if(timeoutFlag) then 
		global_data.set_from_param(global_var_table, sequence.param2, nil)
		error("ERRCODE[-37]Timeout with communicate with MCU between two response")
		-- return nil,"time out" 
	end
	local r,l = Data_Operation(dt_str)
	r = string.sub(r,1,#r-1)
	l = string.sub(l,1,#l-1)
	-- print(r,l)
	local ret = THDN(r,l)
	global_data.set_from_param(global_var_table, sequence.param2, ret)
	return ret, true
end

function thdn_module.thdn(sequence, global_var_table)
	if(sequence.param1 and sequence.param1~= "") then
		errMsg = fixtureConsole._ARM_Send_Cmd_("thdn measure(-nf,dual,20000,5,8000)\r\n")     --use this command when FPGA test codec board D5.0 70HZ frequency
		print("70HZ")
		print("thd errMsg:"..tostring(errMsg))
	else
		errMsg = fixtureConsole._ARM_Send_Cmd_("thdn measure(-nf)\r\n")
	end  
	_thdn, fre, amp = string.match(errMsg,"ACK%((.-)dB,(.-)Hz,(.-)mV")
	if(_thdn == nil) then
		error(tostring(errMsg))
	end
	return _thdn
end

function thdn_module.amplitude(sequence, global_var_table)
	if sequence.unit == nil then
    	error("ERRCODE[-39]amplitude function requires a unit!")
  	end
	if(amp) then
		return sequence_utils.convert_units(amp, "mV", sequence.unit)
	else
		error(errMsg)
	end
end

function thdn_module.frequency(sequence, global_var_table)
	if sequence.unit == nil then
    	error("ERRCODE[-40]frequency function requires a unit!")
  	end
	if(fre) then
		return sequence_utils.convert_units(fre, "HZ", sequence.unit)
	else
		error(errMsg)
	end
end

return thdn_module