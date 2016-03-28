
local mid = 0
if(CONFIG and CONFIG.ID) then
    mid = CONFIG.ID
end

print("< "..tostring(mid).." > Load Calibration Factor...")
local hwio = require("hw.HWIO")
local VT = hwio.MeasureTable
local SUPPLY = hwio.SupplyTable
local ELOAD = hwio.EloadTable

local MCU = require("hw.MCU")
local dmm = require("functions.dmm")
local dl = require("functions.ArmDL")
local datalogger = require("functions.datalogger")
local audiocal = require("functions.audiocal")

require "pathManager"
package.cpath = package.cpath..";"..deleteLastPathComponent(CurrentDir()).."/lib/?.dylib"

require "libGlobal"

local log = require "utils.LogFile"

local origionData = {}
local factorData = {}
factorData[#factorData+1] = "Name,Gain,Offset"
------------------------------------------CAL FUNCTION----------------------------------------------
--Voltage Supply, Eload Supply will update factor to HWIO
--measure fucntion, will update to HWIO
--DMM, Datalogger, will update to dmm.lua and datalogger.lua

local AI_CAL_VT = {}
local DATE_STARTADDR=0x80
local USBRECT_STARTADDR=DATE_STARTADDR+0X20--RECT address
local USBMAIN_STARTADDR=USBRECT_STARTADDR+0x10--Main address
local BATT_STARTADDR=USBMAIN_STARTADDR+0x10--battery address
local USB_CURR_STARTADDR=BATT_STARTADDR+0x10--Current  address
local VDDMIAN_CURR_STARTADDR=USB_CURR_STARTADDR+0x10--Current  address
local BATT_CURR_STARTADDR=VDDMIAN_CURR_STARTADDR+0x10--Current  address
local ELOAD_CURR_STARTADDR=BATT_CURR_STARTADDR+0x10--Current  address
local ELOAD_MEAS_CURR_STARTADDR=ELOAD_CURR_STARTADDR+0x10--Current  address
local AI_STARTADDR=ELOAD_MEAS_CURR_STARTADDR+0x10--AI address


local DMM_100uA_CURR_ADD = AI_STARTADDR+0x70   --0X1A0
local DMM_2mA_CURR_ADD = DMM_100uA_CURR_ADD+0x10  --0X1B0
local Datalogger_CURR_ADD = DMM_2mA_CURR_ADD+0x10  --0X1C0

local f = nil;
local function Check_Project_type()
		local logtype1 = "/Users/gdlocal/Desktop/Restore Info.txt";
		local fi = io.open(logtype1,"r")
		f = fi:read("*all")
		fi:close()
		assert(f, "Failed to open file: "..tostring(logtype1))
		if (string.find(f,"PRODUCT=X527")) then
			 	AI_CAL_VT = {
				"PP_VRECT",--Devide
				"PMU_TO_AOP_RESET_L",
				"PP_CON_DET_EN_PIN",--Devide
				"ALL_AI",--"PP3V0_SLEEP3_LDO2", --ALL AI
				"PMU_TO_DISP_DEBUG_AWAKE_INDICATOR",
				"ALERT_VOLT",
				"DEBUG_TO_PMU_RESET",
				"PMU_TO_AOP_AP_SLEEP1_PWRGOOD",
				"DIRECT_POINT",--"PP_BATT_VCC",--separate to DMM, 7 poine
				};
				print("switch X527 mode!")
		elseif (string.find(f,"PRODUCT=X827")) then
				AI_CAL_VT = {
				"PP8V0_DISP",
				"PP_VRECT",--Devide
				"PMU_TO_AOP_RESET_L",
				"PP_CON_DET_EN_PIN",--Devide
				"ALL_AI",--"PP3V0_SLEEP3_LDO2", --ALL AI
				"ALERT_VOLT",
				"DIRECT_POINT",--"PP_BATT_VCC",--separate to DMM, 7 poine
				print("switch X827 mode!")
				};
		else
				print("< error > can't find product type!")
		end
end


--READ
-- 0: success
-- -1: connect error -2: timeout -3:cmd error 1: thread cancel
-- nil: Can't find fac
--get Factor from eeprom
local function lua_string_split(str, split_char)
    local sub_str_tab = {};
    while (true) do
        local pos = string.find(str, split_char);
        if (not pos) then
            sub_str_tab[#sub_str_tab + 1] = str;
            break;
        end
        local sub_str = string.sub(str, 1, pos - 1);
        sub_str_tab[#sub_str_tab + 1] = sub_str;
        str = string.sub(str, pos + 1, #str);
    end

    return sub_str_tab;
end

------------combine two tables--------
local function combine2Tables(table1, table2)
	local tmpTable = {}--= table1;
	for i=1, #table1 do
		tmpTable[#tmpTable+1] = table1[i]
	end

	for i=1,#table2 do
		tmpTable[#tmpTable+1] = table2[i]
	end
	return tmpTable
end

local function MCUSendCommand(cmd)
	print(cmd)
	origionData[#origionData+1] = "< Send > : "..cmd
	local rec =MCU.InstrumentCmd(cmd);	
	print(rec)
	origionData[#origionData+1] = "< Receive > : "..rec
	return rec
end

local function ReadFactorFromARMSub(base,addrr)
	local len=0;
	local legth=4;
	local recevicestr;

	--read flag
	local flagcmd=string.format("eeprom read(%s,at08,%X,%d)",base,addrr+len,legth);
	local recevicestr = MCUSendCommand(flagcmd)
	if(string.find(tostring(recevicestr),"ACK")==nil)then
		origionData[#origionData+1] = "< Error > : Cant get Full return with ACK"
		return	recevicestr	
	end				-- -1: connect error -2: timeout, 1: thread cancel
	if(string.find(recevicestr,"ERR"))then
		origionData[#origionData+1] = "< Error > : Error Occur"
		return -3 
	end

	recevicestr=string.match(recevicestr,"ACK%s*%((.-);");
	local strtable=lua_string_split(recevicestr,",");
	if(#strtable<4) then 
		origionData[#origionData+1] = "< Error > : table length less than 4 "..tostring(recevicestr)
		return -4
	end
	local flagnum=string.format("0x%02X%02X%02X%02X",tonumber(strtable[4],16),tonumber(strtable[3],16),tonumber(strtable[2],16),tonumber(strtable[1],16));
	if(string.find(flagnum,"0x00000001")) then	
		local krt={};
		for i=1,2 do	
			len=len+4;
			local flagcmd=string.format("eeprom read(%s,at08,%x,%d)",base,addrr+len,legth);
			recevicestr=MCUSendCommand(flagcmd);	
			if(string.find(tostring(recevicestr),"ACK")==nil)then 
				origionData[#origionData+1] = "< Error > : Cant get Full return with ACK"
				return	recevicestr	
			end				-- -1: connect error -2: timeout, 1: thread cancel
			if(string.find(recevicestr,"ERR")) then
				origionData[#origionData+1] = "< Error > : Get Error"
				return recevicestr
			end

			recevicestr=string.match(recevicestr,"ACK%s*%((.-);");
			local strtable=lua_string_split(recevicestr,",");	
			if(#strtable<4) then 
				origionData[#origionData+1] = "< Error > : table length less than 4 "..tostring(recevicestr)
				return -4
			end
			local krnum=string.format("0x%02X%02X%02X%02X",tonumber(strtable[4],16),tonumber(strtable[3],16),tonumber(strtable[2],16),tonumber(strtable[1],16));
			krt[i]=krnum;
		end
		origionData[#origionData+1] = string.format("< Success > : %s & %s",krt[1], krt[2])
		return {krt[1],krt[2]};
	else
		origionData[#origionData+1] = "< Error > : Can't find 0x00000001"
		return -2
	end
end

--------------------------------------------update fac function-------------------------------------
local function GetFacorFromArm(addr)
		local fac = {1,0}
		local flag=0;
		local krtable=ReadFactorFromARMSub("translation",addr)
		local basetable=ReadFactorFromARMSub("testbase",addr)
		if(type(krtable)=="table" and type(basetable)=="table") then
			if(krtable[1]~=basetable[1] or krtable[2]~=basetable[2]) then
				origionData[origionData+1]=string.format("< Error > : Data From ARM Different %s & %s VS %s & %s",krtable[1],krtable[2],basetable[1],basetable[2])
			end
			fac[1]=floatToNumber(krtable[1]);
			fac[2]=floatToNumber(krtable[2]);
		else
			origionData[#origionData+1]="< Warning > : Error occur when get data from ARM, set Factor to (1:0)"
			fac[1]=1;
			fac[2]=0;
			flag=1;			
			krtable=nil;
		end
		return flag,fac;
end
--update VT
local function checkDevidADC(tbl, adc)
	local flag = false
	for key, _adc in pairs(tbl) do
		if _adc == adc then
			flag = true
			break
		end
	end
	return flag
end

local function updateVTFactor()
	print("\nupdateVTFactor...")
	origionData[#origionData+1] = "\nupdate VT Factor..."
	local flag=0;
	local flagnum=1;
	local keyTable = AI_CAL_VT

	local factorTable = {}
	local offsetTable= {}
--read from ARM
	local addr=AI_STARTADDR;
	local itime=0;
	for i=1,#keyTable do
		itime = 1+itime
		local retflag,krtable = GetFacorFromArm(addr);
		flagnum=flagnum*2;
		factorTable[i]=krtable[1];
		offsetTable[i]=krtable[2];
		if (itime==7) then
			addr=addr+0x40;
		else
			addr=addr+0x10;
		end
	end
--print gain and offset
	local g = {}
	for i=1, #factorTable do
		local key = keyTable[i]
		local t = {}
		t.GAIN  = factorTable[i]
		t.OFFSET = offsetTable[i]
		g[key] = t
	end
--Update HWIO MeasureTable
	origionData[#origionData+1] = "< update > : ALL AI"
	for key, tbl in pairs(VT) do
		tbl.GAIN = g.ALL_AI.GAIN
		tbl.OFFSET = g.ALL_AI.OFFSET
		-- print(key, tbl.GAIN, tbl.OFFSET)
	end
	factorData[#factorData+1] = "ALL_AI,"..tostring(g.ALL_AI.GAIN)..","..tostring(g.ALL_AI.OFFSET)
--update sepecial key In Measure Table
	for i=1, #AI_CAL_VT do
		local key = AI_CAL_VT[i]
		if(VT[key]) then
			origionData[#origionData+1] = "< update > : "..key
			VT[key].GAIN = g[key].GAIN
			VT[key].OFFSET = g[key].OFFSET
			factorData[#factorData+1] = key..","..tostring(VT[key].GAIN)..","..tostring(VT[key].OFFSET)
		end
	end
--Update Direct Point	
	origionData[#origionData+1] = "< update > : DMM_Direct_Point"
	dmm.factor.Volt.GAIN = g.DIRECT_POINT.GAIN
	dmm.factor.Volt.OFFSET = g.DIRECT_POINT.OFFSET
	factorData[#factorData+1] = "DIRECT_Volt,"..tostring(g.DIRECT_POINT.GAIN)..","..tostring(g.DIRECT_POINT.OFFSET)
	-- for k,v in pairs(VT) do
	-- 	print(k,VT[k].GAIN,VT[k].OFFSET)
	-- end
	return flag;
end




--update Volt fac
local function updateVoltFactor()
	print("update Volt Factor...")
	origionData[#origionData+1] = "\nupdate Volt(supply) Factor..."
	local flag=0;
	local flagnum=1;
	local retflag;
	local add = {BATT_STARTADDR, USBMAIN_STARTADDR, USBRECT_STARTADDR, ELOAD_CURR_STARTADDR}
	local name = {"BATT","USBMAIN","USBRECT","ELOAD"}
	local fac = {{1,0},{1,0},{1,0},{1,0}}

	for i=1, #add do
		retflag,fac[i] = GetFacorFromArm(add[i]);
		factorData[#factorData+1] = name[i]..","..tostring(fac[i][1])..","..tostring(fac[i][2])
		-- if(retflag==1) then flag=flag+flagnum return flag end
		flagnum=flagnum*2;
	end	
	for key, tbl in pairs(SUPPLY) do
		if tbl.DAC then
			local index = string.byte(tbl.DAC)-65 --"B"batt,"C"main,"D"rect
			tbl.GAIN = fac[index][1]
			tbl.OFFSET = fac[index][2]
		end
	end
	for key, tbl in pairs(ELOAD) do
		if(tbl.DAC) then
			tbl.GAIN = fac[4][1]
			tbl.OFFSET = fac[4][2]
		end
	end
	return flag;
end

local function updateCurrFactor(par)
	print("update curr factor...")
	origionData[#origionData+1] = "\nupdate curr factor..."
	local retflag;
	local add = {VDDMIAN_CURR_STARTADDR, USB_CURR_STARTADDR, ELOAD_MEAS_CURR_STARTADDR}
	local name = {"VDD_MAIN_CURRENT","PP_VRECT_CURRENT","ELOAD_CURRENT"}
	local fac = {1,0}

	for i=1, #add do
		retflag,fac = GetFacorFromArm(add[i]);
		factorData[#factorData+1] = name[i]..","..tostring(fac[1])..","..tostring(fac[2])
		if(VT[name[i]]) then
			origionData[#origionData+1] = "< update > : "..name[i]
			VT[name[i]].GAIN = fac[1]
			VT[name[i]].OFFSET = fac[2]
		end
	end	
end

local function Update_Datalogger_Factor()
	print("Update_Datalogger_Factor...")
	origionData[#origionData+1] = "\nUpdate_Datalogger_Factor..."
	local flag, f = GetFacorFromArm(Datalogger_CURR_ADD)
	dl.factor.GAIN = f[1]
	dl.factor.OFFSET = f[2]/1000
	dl.updateFactor()
	datalogger.factor.GAIN = f[1]
	datalogger.factor.OFFSET = f[2]/1000
	factorData[#factorData+1] = "Datalogger,"..tostring(datalogger.factor.GAIN)..","..tostring(datalogger.factor.OFFSET)
end

local function Update_DMM_Low_Curr_Factor()
	print("Update_DMM_Low_Curr_Factor...")
	origionData[#origionData+1] = "\nUpdate_DMM_Low_Curr_Factor..."
	local flag, f = GetFacorFromArm(DMM_100uA_CURR_ADD)
	dmm.factor.LowCurr.GAIN = f[1]
	dmm.factor.LowCurr.OFFSET = f[2]
	factorData[#factorData+1] = "DMM_Low_Curr,"..tostring(f[1])..","..tostring(f[2])
end

local function Update_DMM_High_Curr_Factor()
	print("Update_DMM_High_Curr_Factor...")
	origionData[#origionData+1] = "\nUpdate_DMM_High_Curr_Factor..."
	local flag, f = GetFacorFromArm(DMM_2mA_CURR_ADD)
	dmm.factor.HighCurr.GAIN = f[1]
	dmm.factor.HighCurr.OFFSET = f[2]
	factorData[#factorData+1] = "DMM_High_Curr,"..tostring(f[1])..","..tostring(f[2])
end

local function GetCalFactor(cmd1, cmd2)
	local fac = {GAIN=1,OFFSET=0}
    local Response1 = MCU.InstrumentCmd(cmd1)
    print("codec Response1 is :"..Response1)
    if(Response1 ~= nil)then
        local length=string.match(Response1,"length:(%d*)")
        if(length ~= nil) then
            if(tonumber(length)>0 and tonumber(length)<100)then
                local cmd2=string.format(cmd2,tostring(length));  
                local Response2 = MCU.InstrumentCmd(cmd2)
                if(Response2 ~= nil)then
                    local offset=string.match(tostring(Response2),"offset:(.-)k")
                    local gain=string.match(tostring(Response2),"gain:(.-)r")
                    if((offset ~= nil) and (gain ~= nil))then
						fac.OFFSET = tonumber(offset)
                        fac.GAIN = tonumber(gain)
                    else  
						--log.write(logtype,"Get Factor Error! Message is:"..tostring(offset)..","..tostring(gain).."\r")
						origionData[#origionData+1] = "Get Factor Error! Message is:"..tostring(offset)..","..tostring(gain).."\r"
                    end  
                else
                    --log.write(logtype,"Calibration data get nil string!")
                    origionData[#origionData+1] = "Calibration data get nil string!"
                end 
            else
                --log.write(logtype,"Calibration length is not within 0-100") 
                 origionData[#origionData+1] = "Calibration length is not within 0-100"
            end 
        else
        	--log.write(logtype,"Wrong string length from EEPROM!")
        	 origionData[#origionData+1] = "Wrong string length from EEPROM!"
        end            
    else 
        --log.write(logtype,"No response from ARM")
         origionData[#origionData+1] = "No response from ARM"
    end 
--    log.write(logtype,"\tGain, Offset : "..tostring(fac.GAIN)..", "..tostring(fac.OFFSET))
	origionData[#origionData+1] = "  <Gain> "..tostring(fac.GAIN).."   <Offset> "..tostring(fac.OFFSET)
    return fac
end


local function GetCodecResistor(cmd1, cmd2)
	local codec_resistor =10
    local Response1 = MCU.InstrumentCmd(cmd1)
    if(Response1 ~= nil)then
        local length=string.match(Response1,"length:(%d*)")
        if(length ~= nil) then
            if(tonumber(length)>0 and tonumber(length)<100)then
                local cmd2=string.format(cmd2,tostring(length));  
                local Response2 = MCU.InstrumentCmd(cmd2)
                if(Response2 ~= nil)then
                    codec_resistor = tonumber(string.match(tostring(Response2),"resistor:(.-)ohm"))
                    if(tonumber(codec_resistor) ~= nil)then
                    	--log.write(logtype,"\t codec_resistor : "..tostring(codec_resistor))
                    	origionData[#origionData+1] = "  <codec_resistor> "..tostring(codec_resistor)
                    	return codec_resistor
                    else  
						--log.write(logtype,"Get Factor Error! Message is:"..tostring(offset)..","..tostring(gain).."\r")
						origionData[#origionData+1] = "Get Factor Error! Message is:"..tostring(offset)..","..tostring(gain).."\r"
                    end  
                else
                    --log.write(logtype,"Calibration data get nil string!")
                    origionData[#origionData+1] = "Calibration data get nil string!"
                end 
            else
                --log.write(logtype,"Calibration length is not within 0-100")
                 origionData[#origionData+1] = "Calibration length is not within 0-100"
            end 
        else
        	--log.write(logtype,"Wrong string length from EEPROM!")
        	 origionData[#origionData+1] = "Wrong string length from EEPROM!"
        end            
    else 
        --log.write(logtype,"No response from ARM")
        origionData[#origionData+1] = "No response from ARM"
    end 
end

local function Codec_DMM_Volt_Factor()
--	log.write(logtype,"Update Codec Volt Factor, please waiting ... ")
	origionData[#origionData+1] = "\t<Update Codec Volt Factor . please waiting ...>"
	local cmd1 = "eeprom read string(codec,cat08,0x200,9)"
	local cmd2 = "eeprom read string(codec,cat08,0x209,%s)"
	local f = GetCalFactor(cmd1,cmd2)
	audiocal.factor.CODEC_AMP_DMM_MEASURE_GAIN = f.GAIN
	audiocal.factor.CODEC_AMP_DMM_MEASURE_OFFSET = f.OFFSET

end


local function Codec_FPGA_AMP_Factor()
--	log.write(logtype,"Update Codec FPGA AMP Factor, please waiting ... ")
	origionData[#origionData+1] = "\t<Update Codec FPGA AMP Factor . please waiting ...>"
	local cmd1 = "eeprom read string(codec,cat08,0x80,9)"
	local cmd2 = "eeprom read string(codec,cat08,0x89,%s)"
	local f = GetCalFactor(cmd1,cmd2)
	audiocal.factor.CODEC_FPGA_AMP_MEASURE_GAIN = f.GAIN
	audiocal.factor.CODEC_FPGA_AMP_MEASURE_OFFSET = f.OFFSET
end

local function Codec_DMM_Resistor()
--	log.write(logtype,"Update Codec Resistor , please waiting ... ")
	origionData[#origionData+1] = "\t<Update Codec Resistor . please waiting ...>"
	local cmd1 = "eeprom read string(codec,cat08,0x280,9)"
	local cmd2 = "eeprom read string(codec,cat08,0x289,%s)"
	local f = GetCodecResistor(cmd1,cmd2)
	audiocal.factor.CODEC_CURRENT_RESISTOR = f

end


-- local function Codec_AP_AGILENT_Factor()
-- 	log.write(logtype,"Update Codec AP&Agilent Factor, please waiting ... ")
-- 	local cmd1 = "eeprom read string(codec,cat08,0x300,9)"
-- 	local cmd2 = "eeprom read string(codec,cat08,0x309,%s)"
-- 	local f = GetCalFactor(cmd1,cmd2)
-- 	audiocal.factor.CODEC_AP_AGILENT_MEASURE_GAIN = f.GAIN
-- 	audiocal.factor.CODEC_AP_AGILENT_MEASURE_OFFSET = f.OFFSET
-- end



local function ReadDateFromARM()
	local len=0x20
	local strARM=string.format("eeprom read(translation,cat08,%x,%d)",DATE_STARTADDR,len);
	local ret=MCUSendCommand(strARM)
	local recevicestr=string.match(ret,"ACK%((.-);");

	local endnum=string.find(recevicestr,",ff");
	recevicestr=string.sub(recevicestr,1,endnum-1);
	-- local strtable=M.lua_string_split(recevicestr,",");
	local strtable = {}
	for v in string.gmatch(recevicestr..",","(.-),") do
		strtable[#strtable+1] = v
	end

	local retstr="";
	for i=1,#strtable do
		retstr=retstr..string.char(tonumber(strtable[i],16));
	end
	calibrationstr=retstr
	factorData[#factorData+1] = "Date,"..tostring(retstr)
	return retstr;

end

--SN check
local Fun_Board_SN ={
					"arm",
					"testbase",	
					"translation",
					"dmm",
					"datalogger",
					"power",
					}
local Board_num=6
local Fun_Board_SN_ADDR=0X300;
local SN_LEN=17;

local function Read_SN_From_EEPROM(name, count)
	local str
    local cmd = string.format("eeprom read string(%s,cat08,00,%s)",name,count)
    local ret=MCUSendCommand(cmd)
    if(ret ~= nil) then
        str = string.match(tostring(ret),"ACK%s*%(\"(.-)\";")
        if(str) then 
        	origionData[#origionData+1] = "< Result > : "..str
        else 
            str = "< Error > : Cant Match by ACK%s*%("
            origionData[#origionData+1] = str
        end
    else
        str = "< Error > : Empty return From MCU"  
        origionData[#origionData+1] = str
    end 
    return str   
end

local function ReadRAMSN()
	local cmd=string.format("system read(-sn)");
	local ret=MCUSendCommand(cmd);
	
	local ret = ""
	if(string.find(tostring(ret),"473F")) then
		ret = string.match(ret,"\"(.-)\"");
		origionData[#origionData+1] = "< Result > : "..ret
	else
		ret = "< Error > : Cant find .4.7.3.F";
		origionData[#origionData+1] = ret
	end
	return ret
end

local function ReadBoardSN(board)
	local snstr;
	
	if(board=="arm") then
		snstr=ReadRAMSN()
	else
		snstr=Read_SN_From_EEPROM(board, 17)
	end
	
	if(string.len(snstr)==17 and string.find(snstr,"473"))	then
		return snstr	
	else
		return nil;
	end	
end
local function CheckBoardSN()
	print("Check Board SN...")
	origionData[#origionData+1] = "\nCheck Board SN..."
	local snlen1=((Board_num)-3)*17;
	local snlen2=Board_num*17-snlen1;
	local n=0;

	local eerortable={};
	local boardsn="";
 	local cmd1= string.format("eeprom read string(translation,cat08,%x,%d)",Fun_Board_SN_ADDR,snlen1);
	local storesnstr1 = MCUSendCommand(cmd1)
 	local cmd2= string.format("eeprom read string(translation,cat08,%x,%d)",Fun_Board_SN_ADDR+snlen1,snlen2);
	local storesnstr2 = MCUSendCommand(cmd2)

    if ((string.match(storesnstr1,"ERR")==nil) and string.match(storesnstr2,"ERR")==nil) then
    	local StoreSNStr=string.match(storesnstr1,"\"(.*)\"")..string.match(storesnstr2,"\"(.*)\"");
		origionData[#origionData+1] = "< Result > : "..tostring(StoreSNStr)
		local m=0
		for i,board in pairs(Fun_Board_SN) do
			boardsn=ReadBoardSN(board);
			
			local startsub=m*17+1;
			local endsub=(m+1)*17;
			checksn=string.sub(StoreSNStr,startsub,endsub);
		
			if(tostring(checksn)~=tostring(boardsn)) then
				eerortable[n]=board;
				n=n+1;
			end
			m=m+1;
		end
    else
    	origionData[#origionData+1] = "< Error > : Error occur."
		return -1;
	end	
 
  	if(n ~= 0) then
  		origionData[#origionData+1] = "< Error > : SN CODE of board  is diffrent with calibration: "..tostring(table.concat(eerortable,"& "))
  	else
  		origionData[#origionData+1] = "< Success > : Congratulations!"
  	end
  
end
--------------------------------------------update fac function-------------------------------------
local function updatefactor()
	Check_Project_type()
	local flag = 0;
	if (string.find(f,"PRODUCT=X827")) then
			local f = {
				updateVTFactor(), 
				updateVoltFactor(), 
				updateCurrFactor(),
				Update_Datalogger_Factor(),
				Update_DMM_Low_Curr_Factor(),
				Update_DMM_High_Curr_Factor(),
				-- Update_DMM_Volt_Factor(),
				-- Update_DMM_AI_Factor(),
				ReadDateFromARM(),
				CheckBoardSN(),
				print("Upload X827 factor!")
			}
	else
				local f = {
				Codec_DMM_Resistor(),
				Codec_DMM_Volt_Factor(),
				Codec_FPGA_AMP_Factor(),
				updateVTFactor(), 
				updateVoltFactor(), 
				updateCurrFactor(),
				Update_Datalogger_Factor(),
				Update_DMM_Low_Curr_Factor(),
				Update_DMM_High_Curr_Factor(),
				-- Update_DMM_Volt_Factor(),
				-- Update_DMM_AI_Factor(),
				ReadDateFromARM(),
				CheckBoardSN(),
				-- Codec_AP_AGILENT_Factor(),
				print("Upload X527 factor!")
			}
	end
	for i=1, #f do
		local t = f[i]
	end
	-- log.write(logtype,"Test")
end

updatefactor();

local logtype = "Calibration"
log.open(logtype,"csv")
log.write(logtype,table.concat( factorData, "\n"),false)
log.write(logtype,"\n.....................................\n",false)
log.write(logtype,table.concat( origionData, "\n"),false)
local dmm_version = dmm.dmmVersionCheck()
log.write(logtype, "\n< DMM Version > : " .. tostring(dmm_version),false)
log.close(logtype)

print("< "..tostring(mid).." > Load Calibration Factor finished...")  
