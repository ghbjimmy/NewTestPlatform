local llthreads = require"llthreads2"

local thread_code = 
[=====[
local    SE_ERR_NONE=0
local    SE_ERR_OVERFLOW=1
local    SE_ERR_NOBUF=2
local    SE_ERR_ID=3
local    SE_ERR_CMD=4
local    SE_ERR_FORMT=5
local    SE_ERR_UNSURPPORT=6
local    SE_ERR_CRC=7
local    SE_ERR_PERM=8
local    SE_ERR_RUNNING=9
local    SE_ERR_CONFIRM=192
local    SE_ERR_PARA=193
local    SE_ERR_DOING=224
local    SE_ERR_DONE=255

local term_string = "\r\n"
local err_code = SE_ERR_NONE
local uuid = nil

function WriteFile(__file_path, __content)
	local ret = nil;
	local path = __file_path;
	local f = io.open(path, "a+");
	if f == nil then return nil, "Failed to open file: "..path; end
	if (__content) then
		ret = f:write(__content);
	end
	f:flush();
	f:close();
	return ret;
end

function DbgOut(__content)
	if (__content) then
		WriteFile("/tmp/LuaDebug.txt", __content.."\n")
	end
end

--command list format: command = {"command", "args format", "legal args", "command description"}
local ARMCommands = 
{

	{"command" 			,"?", "None", ""},
	{"help" 				,"", "None", "List the help information for commands"},
	
	{"version" 			,"function number", "Any_Number", "get the version information for software and hardware"},
	{"reset" 			,"module name, function number", "args=2 p1=enum(MCU,FPGA,System) p2=enum(0, 1) ", "reset MCU, or FPGA"},
	{"update" 			,"module name, function number", "args=2 p1=enum(MCU,FPGA,System) p2=enum(0, 1) ", "update MCU or FPGA firmware "},
	{"system set" 		,"-option,content","args=2 p1=enum(-time,-mac,-ip) p2=Any_String ", ""},
	{"system read" 		,"-option", "args=1 p1=enum(-time,-ip,-mac,-hardware,-board) ", "report all of the setting about hardware(IO/ADC/DAC)"},
	{"echo" 				,"any information", "Any_String", "echo the command"},
	{"vref set" 			,"value", "args=1 p1=range[0.0, 5000.0] ", "set reference voltage for comparator"},
	{"vref read" 		,"", "None", "read the set value of reference voltage"},
	{"frequency measure" ,"signal name", "args=1 p1=enum(tcxo) ", "measure the input signal frequency,pulse width,etc"},
	{"thdn measure" 		,"", "None", "measure codec ADC input signal THDN"},
	{"codec open pdm" 	,"", "None", "output PDM "},
	{"codec close pdm" 	,"", "None", ""},
	{"datalogger open" 	,"samplerate", "args=1 p1=range[1, 10] ", "start datalogger; set datalogger mode samplerate=(1-10): 1K to 10K, default=1K"},
	{"datalogger close" ,"", "None", "stop datalogger"},
	
	--{"fpga loopback" 	,"mode,value", "args=2 p1=Any_Number p2=Any_Number ", "loopback the UART/Codec signal loopback the UART/Codec signal "},
	{"fpga read" 		,"address,count", "args=2 p1=range[0x0, 0xFFFF] p2=range[1, 60] ", "read FPGA register only for debug"},
	{"fpga write" 		,"address,count,content", "args=3 p1=range[0x0, 0xFFFF] p2=range[1, 60] p3=[0x0, 0xFF] ", "write FPGA register  eg. fpga write(00,1,a) only for debug"},
	{"io set" 			,"number, content", "args=v p1=range[1, 32] ", "set the IO (inclcude the extending IO)"},
	{"io read" 			,"number, content", "args=v p1=range[1, 32] ", "report the setting value about IO"},
	{"io set chip" 		,"serial number, state", "args=2 p1=range[1, 6, 255] p2=range[0x0, 0xFFFF] ", "set chip output state: chip 1, state=0xeeee io chip set(1,eeee)  set all chip output state: io chip set(255,aaff)"},
	{"io read chip" 	,"serial number", "args=1 p1=range[1, 6, 255] ", "report the setting value about IO Send: io chip read(255) ACK: ACK(eeee,aaee,bbdd,ccdd,bbee,aaff)"},
	{"adc set mode" 	,"oversample, range", "args=2 p1=enum(0,2,4,8,16,32,64) p2=enum(5, 10) ", "set the mode of ADC(oversample/range) adc set mode(8,5)"},
	{"adc read mode" 	,"", "None", "read the mode of ADC(oversample/range)"},
	{"adc read" 		,"{-option},channel, count", "args=3 p1=otp(-r) p2=enum(A,B,C,D,E,F,G,H) p3=range[1, 100] ", "read the ADC value, the result can be raw data of register,or can be translated to be voltage Send: adc read(-r,A,5) ACK:  ACK(ffee,ffed,ffec,fff6,fff5)  Send: adc read(A,4) ACK:  ACK(10.02 mv,10.02 mv, 11.00 mv,10.8 mv)"},
	{"dac set" 			,"{-option},channel, data", "args=3 p1=otp(-r) p2=enum(A,B,C,D) p3=range[0.0, 5000.0] or [0x0, 0xFFFF] ", "set DAC output,the data can be raw data of register,or can be translated to be voltage dac set(A,300.9) dac set(-r,A,ffee)"},
	{"dac read"			,"{-option},channel", "args=2 p1=otp(-r) p2=enum(A,B,C,D) ", "report the setting value about dac, the result can be raw data of register,or can be translated to be voltage Send: dac read(A) ACK:  ACK(300.9 mv)"},
	{"dmm set" 			,"range", "args=1 p1=enum(0 uA, 100 uA, 2 mA) ", "set DMM mode(range/sample rate)"},
	--{"dmm check" 		,"", "None", "check DMM status online"},
	{"dmm read" 		,"channel", "args=1 p1=enum(volt, curr) ", "read the current or voltage value"},
	{"eeprom write" 	,"PCB name, address,length,content", "args=4 p1=enum(testbase,dmm,codec,datalogger,power,translation) p2=range[0x0, 0x3FF] or [0x0, 0xFF] p3=range[1, 100] p4=range[0x0, 0xFF]*100 ", "write EEPROM eeprom write(testbase,00,5,1a,2b,3c,33,4c)"},
	{"eeprom read" 		,"(PCB name,address,length", "args=3 p1=enum(testbase,dmm,codec,datalogger,power,translation) p2=range[0x0, 0x3FF] or [0x0, 0xFF] p3=range[1, 100] ", "read EEPROM"},
	{"eeprom write string" ,"PCB name, address,\"string\"", "args=2 p1=enum(testbase,dmm,codec,datalogger,power,translation) p2=range[0x0, 0x3FF] or [0x0, 0xFF] ", "write EEPROM   eeprom write(testbase,00,\"hello,board\")"},
	{"eeprom read string" 	,"PCB name, address,length", "args=3 p1=enum(testbase,dmm,codec,datalogger,power,translation) p2=range[0x0, 0x3FF] or [0x0, 0xFF] p3=range[1, 200] ", "read EEPROM Send: eeprom read(testbase Board,00,11) ACK: ACK(\"hello,board\")"},
	--{"flash write" 			,"-address,-size,-length,-content", "", "write Flash/EEPROM/DDR"},
	--{"flash read" 			,"-address,-size,-length", "", "-address,-size,-length"},
	--{"ddr write" 			,"-address,-size,-length,-content", "", "write Flash/EEPROM/DDR"},
	--{"ddr read" 				,"-address,-size,-length", "", "-address,-size,-length"},
	--{"power set" 			,"battery name, voltage", "args=2 p1=enum(vrect,vddmain,batt,pp_con_det) p2=Any_Number ", "set the parameter for controlling power voltage: mv"},
	--{"power read" 			,"battery name", "args=1 p1=enum(vrect,vddmain,batt,pp_con_det) ", "read the parameter for controlling power"},
	--]]
}

local diags_msg = [==[
Loaded UtilDM  revision =  8. 1
Attempt to read past end of firmware
image 0x4600ba80: bdev 0x4600b5c0 type ibot offset 0x0 len 0x93602
image 0x4600bb00: bdev 0x4600b5c0 type bat0 offset 0x93602 len 0xbe81
image 0x4600bb80: bdev 0x4600b5c0 type bat1 offset 0x9f483 len 0x1c21
image 0x4600bc00: bdev 0x4600b5c0 type dali offset 0xa10a4 len 0x244da
image 0x4600bc80: bdev 0x4600b5c0 type glyP offset 0xc557e len 0x2391
image 0x4600bd00: bdev 0x4600b5c0 type batF offset 0xc790f len 0x15c84
image 0x4600bd80: bdev 0x4600b5c0 type chg0 offset 0xdd593 len 0x3a21
image 0x4600be00: bdev 0x4600b5c0 type recm offset 0xe0fb4 len 0x13564
image 0x4600be80: bdev 0x4600b5c0 type dtre offset 0xf4518 len 0x12660
image 0x4600bf00: bdev 0x4600b5c0 type logo offset 0x106b78 len 0x3e11
image 0x4600bf80: bdev 0x4600b5c0 type chg1 offset 0x10a989 len 0x9ce1
battery voltage 4270 mV
power supply type batt
ANS: firmware: s7002ans-release AppleStorageProcessor-609.3.4
[ANS syslog] user: s7002ans-release 9.0.0 on 7002.A1 - AppleStorageProcessor-609.3.4
Using paddr 0x9db24000, setbase message 0x00009db240001180
[ANS syslog] nand: Bus 0 Chip ID: 50 50 4E 01 05 08
[ANS syslog] nand: Bus 0 Manufacturer ID: 45 82 68 D7 50 02
[ANS syslog] nand: Bus 0 PPN FW version: 052308P_SD15_Z86
[ANS syslog] nand: BG_Init:Init - timestamp : 0 ms
[ANS syslog] nand: UtilDM_PostProcess:[ 0, 0, 0] Adopting reason=1 version=0x0000002f numDefects= 3 numSlots=17
[ANS syslog] nand: UtilDM_PostProcess:Util last formatted on ( 1425645997 ) epoch
[ANS syslog] nand: UtilDM_PostProcess:[ 5, 8, 0] Adopting reason=8 version=0x00000033 numDefects= 3 numSlots= 9
[ANS syslog] nand: UtilDM_PostProcess:[ 6, 9, 0] Adopting reason=8 version=0x00000033 numDefects= 3 numSlots= 9
[ANS syslog] nand: Util_Read:[ 7,10, 0] encountered err:NAND_UNC
[ANS syslog] nand: UtilDM_PostProcess:[ 7,10, 0] UNC! 
[ANS syslog] nand: Util_Read:[ 8,11, 0] encountered err:NAND_UNC
[ANS syslog] nand: UtilDM_PostProcess:[ 8,11, 0] UNC! 
[ANS syslog] nand: Util_Find_Fast:Location [ 6, 0] for element UTILDM  
[ANS syslog] nand: Util_Find_Fast:Location [ 5, 1] for element DM      
[ANS syslog] nand: Util_Find_Fast:Location [ 5,44] for element CTRLBITS
[ANS syslog] nand: Util_Find_Fast:Location [ 5, 4] for element EFFACE  
[ANS syslog] nand: Util_Find_Fast:Location [ 5,19] for element NVRAM   
[ANS syslog] nand: Util_Read:[ 7,10, 0] encountered err:NAND_UNC
[ANS syslog] nand: DM_Decompress:Decompressing DM version:100
[ANS syslog] nand: DM_Decompress:Found stats in Util region
[ANS syslog] nand: DM_Decompress:      .vDisables    :    1
[ANS syslog] nand: DM_Decompress:      .firstUserBand:    6
[ANS syslog] nand: DM_Decompress:      .lastUserBand :  628
[ANS syslog] nand: DM_Decompress:      .hostCapacity :    4 GB
[ANS syslog] nand: DM_Decompress:      .coreCapacity :    5 GB
[ANS syslog] nand: DM_Decompress:      .indMemory    : 2048 KB
[ANS syslog] nand: DM_Decompress:      .numDefect    :   29
[ANS syslog] nand: DM_Decompress:      .hopIdx       :   32
[ANS syslog] nand: DM_Decompress:      .numProb      :    0
[ANS syslog] nand: BG_Init:Util - timestamp : 124 ms
ASP Block Device 1152000 lbas, 4096 bytes per lba, utilFormatted:1 lbaFormatted:0
setting asp to high power mode
[ANS syslog] nand: Cmd_SetPhotoFlowMode:PhotoFlow Mode: Previous mode = 0 New mode = 0
SysCfg: version 0x00020002 with 7 entries using 164 of 131072 bytes
BDEV: protecting 0x2000-0x8000
image 0x9db20dc0: bdev 0x9db16dc0 type ibot offset 0x0 len 0x93602
image 0x9db20e40: bdev 0x9db16dc0 type bat0 offset 0x93602 len 0xbe81
image 0x9db20ec0: bdev 0x9db16dc0 type bat1 offset 0x9f483 len 0x1c21
image 0x9db20f40: bdev 0x9db16dc0 type dali offset 0xa10a4 len 0x244da
image 0x9db23b80: bdev 0x9db16dc0 type glyP offset 0xc557e len 0x2391
image 0x9db23c00: bdev 0x9db16dc0 type batF offset 0xc790f len 0x15c84
image 0x9db23c80: bdev 0x9db16dc0 type chg0 offset 0xdd593 len 0x3a21
image 0x9db23d00: bdev 0x9db16dc0 type recm offset 0xe0fb4 len 0x13564
image 0x9db23d80: bdev 0x9db16dc0 type dtre offset 0xf4518 len 0x12660
image 0x9db23e00: bdev 0x9db16dc0 type logo offset 0x106b78 len 0x3e11
image 0x9db23e80: bdev 0x9db16dc0 type chg1 offset 0x10a989 len 0x9ce1
summit_init command mode()
mipi_dsim_init()
mipi: timing out
summit_init(): read of summit panel id failed
mipi_dsim_quiesce()
=======================================
::
:: iBoot for n27a, Copyright 2007-2014, Apple Inc.
::
::	BUILD_TAG: iBoot-2261.5.19.1.4
::
::	BUILD_STYLE: DEVELOPMENT
::
::	USB_SERIAL_NUMBER: CPID:7002 CPRV:01 CPFM:03 SCEP:01 BDID:02 ECID:000001340DAF7D74 IBFL:1D SRNM:[DV35106049HFT2WA]
::
=======================================
[ANS syslog] nand: BG_Init:Clog Read-Only - timestamp : 885 ms
ASP Block Device 1152000 lbas, 4096 bytes per lba, utilFormatted:1 lbaFormatted:1
Boot Failure Count: 0	Panic Fail Count: 0
Entering recovery mode, starting command prompt
[m] diags
HFSInitPartition: 0x9db55580
[ANS management] IOP state changed to 0x1 (mgmt state=1)
ANS: in suspend to RAM
Console router buffer allocated @ 0x400009EE1C010, size = 11528210455883857936 bytes
AppleCSI ANS - loading firmware @ 0x9E3C1000 (0x9E3C1000), size= 0x60420/0xDB100, heap @ physical 0x9E49D000 (remapped 0xDC000), size= 0x400000
s7002ans-release AppleStorageProcessor-609.1.10
[NAND ASPCommandInterface] AppleASPCmdInterfaceDriverBindingSupported:186 (0x9E8BF910 - 0x9ED3BD10) - coprocId=ANS, epid=6
[NAND ASPCommandInterface] AppleASPCmdInterfaceDriverBindingStart:463 (0x9E8BF910 - 0x9ED3BD10) - driverStarted=0
[NAND ASPCommandInterface] AppleASPCmdInterfaceDriverBindingStart:508 registered callback: 0x9ED30571 with arg NULL
ANS up and running with version 9.0 (3)
[NAND ASPCommandInterface] AppleASPCmdInterfaceDriverBindingStart:528  ++--++ AppleASPCmdInterfaceDriverBindingStart started successfully ++--++
Datas: 6
[NAND ASPCommandInterface] AppleASPCmdInterfaceDriverBindingSupported:186 (0x9E8BF910 - 0x9ED3BD10) - coprocId=ANS, epid=6
[NAND ASPCommandInterface] AppleASPCmdInterfaceDriverBindingStart:463 (0x9E8BF910 - 0x9ED3BD10) - driverStarted=1
[NAND ASPCommandInterface] AppleASPCmdInterfaceDriverBindingSupported:186 (0x9E8BF910 - 0x9ED3BD10) - coprocId=ANS, epid=6
[NAND ASPCommandInterface] AppleASPCmdInterfaceDriverBindingStart:463 (0x9E8BF910 - 0x9ED3BD10) - driverStarted=1
[NAND ASPCommandInterface] AppleASPCmdInterfaceDriverBindingSupported:186 (0x9E8BF910 - 0x9ED3BD10) - coprocId=ANS, epid=6
[NAND ASPCommandInterface] AppleASPCmdInterfaceDriverBindingStart:463 (0x9E8BF910 - 0x9ED3BD10) - driverStarted=1
[2J [0;0HN27 Diag (factory_n27_dvt)
BuildEng build N27Casaval1J0195dv (1J0195dv). Revision 52a8a92.
Built at 2014/12/08 20:36:56
script: pmurw -r 0x040 2
Read 2 bytes:	 0x00  0x00 
script: alias dbg_pwr_disable=pmugpio --pin 13 --pushpull --source ldo9 --output 1
script: alias dbg_pwr_enable=pmugpio --pin 13 --pushpull --source ldo9 --output 0
script: alias bblib="smokeyshell -p -e 'if BBLib_cfg then print [[no init]] else require [[BBLib.12A]];BB_Init() end'"
[00000134:0DAF7D74] :-)]==]

local function CheckMessageFrame(_msg)
	local msg = _msg
	local payload = nil
	err_code = SE_ERR_NONE
	
	--MessageBox("msg length == "..tostring(#msg))
	for i=1,#msg do
		--MessageBox(string.byte(msg, i))
	end
	
	if (#msg > 16+2-1) then
		if (string.sub(msg, 1, 1)== "[" and string.sub(msg, 18, 18)== "]" and string.sub(msg, #msg-1, #msg)==term_string) then
			uuid = string.sub(msg, 2, 17)
			--MessageBox("uuid = ".. tostring(uuid))
			if (#msg > 16+2+2-1) then
				payload = string.sub(msg, 19, #msg-#term_string)--cmd + args
			else
				err_code = SE_ERR_FORMT
			end
		else
			err_code = SE_ERR_FORMT
		end
	else
		err_code = SE_ERR_ID
	end
	
	return payload
end

local function CheckCmd(_payload)
	local payload = _payload
	local r = 0
	
	if (type(payload)== "string" and #payload > 2) then
		if (string.match(payload, "%(.*%)$")) then
			local cmd = string.match(payload, "(.+)%(.*%)")
			if (cmd) then
				local cmd_index = 0
				for i,v in ipairs(ARMCommands) do 
					if (type(v)=="table" and #v>1) then
						if(v[1] == cmd) then
							cmd_index = i
							break
						end
					end
				end
				if (cmd_index > 0) then
					r = cmd_index
					return r
				else
					err_code = SE_ERR_UNSURPPORT
				end
			else
				err_code = SE_ERR_FORMT
			end
		else
			err_code = SE_ERR_FORMT
		end
	else
		err_code = SE_ERR_FORMT
	end

	return r
end

local function CheckArg(_cmd_index, _payload)
	local cmd_index = _cmd_index
	local payload = _payload
	local ack = "";
	
	if (cmd_index < 1 or cmd_index>#ARMCommands or payload==nil) then
		ack = tostring(SE_ERR_FORMT)
		return ack
	end
	
	local args = string.match(payload, "%((.*)%)")--either empty string, or a string contained chars
	--MessageBox("args == "..tostring(args))
	if (args) then
		local cmd = ARMCommands[cmd_index][1]
		if (cmd=="command") then
			if (args) then
				local index_cmd_queried = nil
				for i,v in ipairs(ARMCommands) do 
					if (type(v)=="table" and #v>1) then
						if(v[1] == args) then
							index_cmd_queried = i
							break
						end
					end
				end
				if (index_cmd_queried) then
					ack = ARMCommands[index_cmd_queried][4]
				else
					ack = "ERR_"..tostring(SE_ERR_UNSURPPORT)
				end
				
			else
				ack = "ERR_"..tostring(SE_ERR_FORMT)
			end
			
		
		elseif(cmd=="help") then
			if (args=="") then
				ack = ARMCommands[cmd_index][4]
			else
				ack = "ERR_"..tostring(SE_ERR_FORMT)
			end
		
		elseif(cmd=="version") then
			if (string.match(args, "%d+")) then
				ack = "V1.0.0"
			else
				ack = "ERR_"..tostring(SE_ERR_FORMT)
			end
			
		elseif(cmd=="reset") then
			--"args=2 p1=enum(MCU,FPGA,System) p2=enum(0, 1) "
			if (string.match(args, "MCU%s*,%s*[01]") or 
				string.match(args, "FPGA%s*,%s*[01]") or 
				string.match(args, "System%s*,%s*[01]")) then
				ack = "V1.0.0"
			else
				ack = "ERR_"..tostring(SE_ERR_FORMT)
			end
			
		elseif(cmd=="update") then
			--args=2 p1=enum(MCU,FPGA,System) p2=enum(0, 1)
			if (string.match(args, "MCU%s*,%s*[01]") or 
				string.match(args, "FPGA%s*,%s*[01]") or 
				string.match(args, "System%s*,%s*[01]")) then
				ack = "DONE"
			else
				ack = "ERR_"..tostring(SE_ERR_FORMT)
			end
				
		elseif(cmd=="system set") then
			--{"system set" 		,"-option,content","args=2 p1=enum(-time,-mac,-ip) p2=Any_String ", ""},
			if (string.match(args, "%-time%s*,%s*.+") or 
				string.match(args, "%-mac%s*,%s*.+") or 
				string.match(args, "%-ip%s*,%s*.+")) then
				ack = "DONE"
			else
				ack = "ERR_"..tostring(SE_ERR_FORMT)
			end
		
		
		elseif(cmd=="system read") then
			--MessageBox(tostring(cmd))
		--	{"system read" 		,"-option", "args=1 p1=enum(-time,-ip,-mac,-hardware,-board) ", "report all of the setting about hardware(IO/ADC/DAC)"},
			if (args == "-time") then
				ack = tostring(os.date())
			elseif (args == "-mac") then
				ack = "3c:15:c2:cb:d4:ca"
			elseif (args == "-ip") then
				ack = "127.0.0.1"
			elseif (args == "-hardware") then
				ack = "seeing arm"
			elseif (args == "-board") then
				ack = "seeing arm board"
			else
				ack = "ERR_"..tostring(SE_ERR_FORMT)
			end
		
		elseif(cmd=="echo") then
		--{"echo" 				,"any information", "Any_String", "echo the command"},
			ack = args

		elseif(cmd=="vref set") then
		--	{"vref set" 			,"value", "args=1 p1=range[0.0, 5000.0] ", "set reference voltage for comparator"},
			local v = tonumber(args)
			if (v) then
				if (v>=0 and v<=5000.0) then
					ack = "DONE"
				else
					ack = "ERR_"..tostring(SE_ERR_PARA)
				end
				
			else
				ack = "ERR_"..tostring(SE_ERR_FORMT)
			end
		
		elseif(cmd=="vref read") then
		--	{"vref read" 		,"", "None", "read the set value of reference voltage"},
			ack = "0.000000 mv"

		elseif(cmd=="frequency measure") then
		--{"frequency measure" ,"signal name", "args=1 p1=enum(tcxo) ", "measure the input signal frequency,pulse width,etc"},
			ack = "ERR_194"
		
		elseif(cmd=="thdn measure") then
		--{"thdn measure" 		,"", "None", "measure codec ADC input signal THDN"},
			ack = "ERR_10"
			
		elseif(cmd=="codec open pdm") then
		--"codec open pdm" 	,"", "None", "output PDM "
			ack = "ERR_11"
			
		elseif(cmd=="codec close pdm") then
		--{"codec close pdm" 	,"", "None", ""},
			ack = "DONE"
		
		elseif(cmd=="datalogger open") then
		--"datalogger open" 	,"samplerate", "args=1 p1=range[1, 10] "
			ack = "DONE"
		
		elseif(cmd=="datalogger close") then
		--"datalogger close" 	,"", "None", "stop datalogger"
			ack = "DONE"
		
		elseif(cmd=="fpga read") then
		--"fpga read" 		,"address,count", "args=2 p1=range[0x0, 0xFFFF] p2=range[1, 60] "
			local p1, p2 = string.match(args, "0?[xX]?(%x+)[%s%t]*,[%s%t]*(%d+)")
			if (p1 and p2) then
				p1 = tonumber(p1, 16)
				p2 = tonumber(p2)
				if (p1>=0 and p1<=0xffff and p2>=1 and p2<=60) then
					if(p2==1) then
						ack = "0x0"
					else
						ack = "0x0"..string.rep(",0x0",p2-1)
					end
				else
					ack = "ERR_"..tostring(SE_ERR_PARA)
				end
			else
				ack = "ERR_"..tostring(SE_ERR_FORMT)
			end
			
		elseif(cmd=="fpga write") then
		--"fpga write" 		,"address,count,content", "args=3 p1=range[0x0, 0xFFFF] p2=range[1, 60] p3=[0x0, 0xFF] "
			local p1, p2 = string.match(args, "0?[xX]?(%x+)[%s%t]*,[%s%t]*(%d+)")
			if (p1 and p2) then
				p1 = tonumber(p1, 16)
				p2 = tonumber(p2)
				if (p1>=0 and p1<=0xffff and p2>=1 and p2<=60) then
					if (string.match(args, "0?[xX]?%x+[%s%t]*,[%s%t]*%d+"..string.rep("[%s%t]*,[%s%t]*0?[xX]?%x+",p2))) then
						ack = "DONE"
					else
						ack = "ERR_"..tostring(SE_ERR_PARA)
					end
				else
					ack = "ERR_"..tostring(SE_ERR_PARA)
				end
			else
				ack = "ERR_"..tostring(SE_ERR_FORMT)
			end
			
		elseif(cmd=="io set") then
		--"io set" 	,"number, content", "args=v p1=range[1, 32] p2=bit range[97,192]" io set(3,bit105=0,bit117=0,bit120=0)
			local p1, p2 = string.match(args, "(%d+)[%s%t]*,[%s%t]*bit(%d+)[%s%t]*=[%s%t]*[01]")
			if (p1 and p2) then
				p1 = tonumber(p1)
				p2 = tonumber(p2)
				if (p1>=1 and p1<=32 and p2>=97 and p2<=192) then
					if (p1>1) then
						if (string.match(args, "%d+[%s%t]*,[%s%t]*bit(%d+)[%s%t]*=[%s%t]*[01]"..string.rep("[%s%t]*,[%s%t]*bit(%d+)[%s%t]*=[%s%t]*[01]",p1-1))) then
							ack = "DONE"
						else
							ack = "ERR_"..tostring(SE_ERR_PARA)
						end
					else
						ack = "DONE"
					end
				else
					ack = "ERR_"..tostring(SE_ERR_PARA)
				end
			else
				ack = "ERR_"..tostring(SE_ERR_FORMT)
			end
		
		elseif(cmd=="io read") then
		--"io read" 			,"number, content", "args=v p1=range[1, 32] ", "report the setting value about IO"
			local p1, p2 = string.match(args, "(%d+)[%s%t]*,[%s%t]*bit(%d+)")
			if (p1 and p2) then
				p1 = tonumber(p1)
				p2 = tonumber(p2)
				if (p1>=1 and p1<=32 and p2>=97 and p2<=192) then
					if (p1>1) then
						if (string.match(args, "%d+[%s%t]*,[%s%t]*bit%d+"..string.rep("[%s%t]*,[%s%t]*bit%d+",p1-1))) then
							ack = ""
							for v in string.gmatch(s, "(bit%d+)") do
								ack = ack..v.."=0," 
							end
							ack = string.gsub(ack, ",$", "")
						else
							ack = "ERR_"..tostring(SE_ERR_PARA)
						end
					else
						ack = "DONE"
					end
				else
					ack = "ERR_"..tostring(SE_ERR_PARA)
				end
			else
				ack = "ERR_"..tostring(SE_ERR_FORMT)
			end
		
		elseif(cmd=="io set chip") then
		--"io set chip" 		,"serial number, state", "args=2 p1=range[1, 6, 255] p2=range[0x0, 0xFFFF] " io chip set(255,aaff)
			local p1, p2 = string.match(args, "(%d+)[%s%t]*,[%s%t]*0?[xX]?(%x+)")
			if (p1 and p2) then
				p1 = tonumber(p1)
				p2 = tonumber(p2,16)
				if ((p1>=1 and p1<=6 or p1==255) and p2>=0 and p2<=0xffff) then
					ack = "DONE"
				else
					ack = "ERR_"..tostring(SE_ERR_PARA)
				end
			else
				ack = "ERR_"..tostring(SE_ERR_FORMT)
			end
			
		elseif(cmd=="io read chip") then
		--"io read chip" 		,"serial number", "args=1 p1=range[1, 6, 255] "
			local p1 = string.match(args, "[%s%t]*(%d+)[%s%t]*")
			if (p1) then
				p1 = tonumber(p1)
				if (p1>=1 and p1<=6) then
					ack = "ffff"
				elseif (p1==255) then
					ack = "ffff,ffff,ffff,ffff,ffff"
				else
					ack = "ERR_"..tostring(SE_ERR_PARA)
				end
			else
				ack = "ERR_"..tostring(SE_ERR_FORMT)
			end
			
		elseif(cmd=="adc set mode") then
		--"adc set mode" 		,"oversample, range", "args=2 p1=enum(0,2,4,8,16,32,64) p2=enum(5, 10) "
			local p1, p2 = string.match(args, "(%d+)[%s%t]*,[%s%t]*(%d+)[%s%t]*")
			if (p1 and p2) then
				p1 = tonumber(p1)
				p2 = tonumber(p2)
				if ((p1==0 or p1==2 or p1==4 or p1==8 or p1==16 or p1==32 or p1==64) and (p2==5 or p2==10)) then
					ack = "DONE"
				else
					ack = "ERR_"..tostring(SE_ERR_PARA)
				end
			else
				ack = "ERR_"..tostring(SE_ERR_FORMT)
			end
		
		elseif(cmd=="adc read mode") then
		--"adc read  mode" 	,"", "None", "read the mode of ADC(oversample/range)"
			ack = "0, 5 V"
			
		elseif(cmd=="adc read") then
		--"adc read" 		,"{-option},channel, count", "args=3 p1=otp(-r) p2=enum(A,B,C,D,E,F,G,H) p3=range[1, 100] "
			local p1 = string.match(args, "^[%s%t]*%-r[%s%t]*,")
			if (p1) then--hex format
				local p2, p3 = string.match(args, "^[%s%t]*%-r[%s%t]*,[%s%t]*([A-H])[%s%t]*,[%s%t]*(%d+)[%s%t]*")
				if (p2 and p3) then
					p3 = tonumber(p3)
					if (p3>=1 and p3<=100) then
						if (p3>1) then
							ack = "0x1ffff"
							ack = ack..string.rep(",0x1ffff",p3-1)
						else
							ack = "0x1ffff"
						end
					else
						ack = "ERR_"..tostring(SE_ERR_PARA)
					end
				else
					ack = "ERR_"..tostring(SE_ERR_FORMT)
				end
			else--dec format
				local p2, p3 = string.match(args, "^[%s%t]*([A-H])[%s%t]*,[%s%t]*(%d+)[%s%t]*")
				if (p2 and p3) then
					p3 = tonumber(p3)
					if (p3>=1 and p3<=100) then
						if (p3>1) then
							ack = "4999.96 mv"
							ack = ack..string.rep(",4999.96 mv",p3-1)
						else
							ack = "4999.96 mv"
						end
					else
						ack = "ERR_"..tostring(SE_ERR_PARA)
					end
				else
					ack = "ERR_"..tostring(SE_ERR_FORMT)
				end
			end
			
			
		elseif(cmd=="dac set") then
		--"dac set" ,"{-option},channel, data", "args=3 p1=otp(-r) p2=enum(A,B,C,D) p3=range[0.0, 5000.0] or [0x0, 0xFFFF] "
			local p1 = string.match(args, "^[%s%t]*%-r[%s%t]*,")
			if (p1) then--hex format
				local p2, p3 = string.match(args, "^[%s%t]*%-r[%s%t]*,[%s%t]*([A-D])[%s%t]*,[%s%t]*0?[xX]?(%x+)[%s%t]*")
				if (p2 and p3) then
					p3 = tonumber(p3, 16)
					if (p3>=0 and p3<=0xffff) then
						ack = "DONE"
					else
						ack = "ERR_"..tostring(SE_ERR_PARA)
					end
				else
					ack = "ERR_"..tostring(SE_ERR_FORMT)
				end
			else--dec format
				local p2, p3 = string.match(args, "^[%s%t]*([A-H])[%s%t]*,[%s%t]*(%d+[%.]?%d*)[%s%t]*")
				if (p2 and p3) then
					p3 = tonumber(p3)
					if (p3>=0 and p3<=5000) then
						ack = "DONE"
					else
						ack = "ERR_"..tostring(SE_ERR_PARA)
					end
				else
					ack = "ERR_"..tostring(SE_ERR_FORMT)
				end
			end
			
		elseif(cmd=="dac read") then
		--"dac read"			,"{-option},channel", "args=2 p1=otp(-r) p2=enum(A,B,C,D) "
			local p1 = string.match(args, "^[%s%t]*%-r[%s%t]*,")
			if (p1) then--hex format
				local p2 = string.match(args, "^[%s%t]*%-r[%s%t]*,[%s%t]*([A-D])[%s%t]*")
				if (p2) then
					ack = "0xffff"
				else
					ack = "ERR_"..tostring(SE_ERR_FORMT)
				end
			else--dec format
				local p2 = string.match(args, "^[%s%t]*([A-D])[%s%t]*")
				if (p2) then
					ack = "2500.000000 mv"
				else
					ack = "ERR_"..tostring(SE_ERR_FORMT)
				end
			end
		
		elseif(cmd=="dmm set") then
		--{"dmm set" 			,"range", "args=1 p1=enum(0 uA, 100 uA, 2 mA) ", "set DMM mode(range/sample rate)"},
			local p1, p2 = string.match(args, "^[%s%t]*(%d+)[%s%t]*([um]A)[%s%t]*")
			if (p1 and p2) then
				if (p1=="0" and p2=="uA") then
					ack = "DONE"
				elseif (p1=="100" and p2=="uA") then
					ack = "DONE"
				elseif (p1=="2" and p2=="mA") then
					ack = "DONE"
				else
					ack = "ERR_"..tostring(SE_ERR_PARA)
				end
			else
				ack = "ERR_"..tostring(SE_ERR_FORMT)
			end
			
		elseif(cmd=="dmm read") then
		--"dmm read" 		,"channel", "args=1 p1=enum(volt, curr) "
			local pv = string.match(args, "^[%s%t]*%volt[%s%t]*")
			local pc = string.match(args, "^[%s%t]*%curr[%s%t]*")
			if (pv) then
				ack = "-0.222921 mv"
			elseif (pc) then
				ack = "119.047605 uA"
			else
				ack = "ERR_"..tostring(SE_ERR_FORMT)
			end
		
		elseif(cmd=="eeprom write") then
		--{"eeprom write" 	,"PCB name, address,length,content", "args=4 p1=enum(testbase,dmm,codec,datalogger,power,translation) p2=range[0x0, 0x3FF] or [0x0, 0xFF] p3=range[1, 100] p4=range[0x0, 0xFF]*100 ", "write EEPROM eeprom write(testbase,00,5,1a,2b,3c,33,4c)"},
			local p1, p2, p3, p4 = string.match(args, "^[%s%t]*(%w+)[%s%t]*,[%s%t]*0?[xX]?(%x+)[%s%t]*,[%s%t]*(%d+)[%s%t]*,[%s%t]*0?[xX]?(%x+)[%s%t]*")
			if (p1 and p2 and p3 and p4) then
				if (p1=="testbase" or p1=="dmm" or p1=="codec" or p1=="datalogger" or p1=="power" or p1=="translation") then
					p2 = tonumber(p2, 16)
					p3 = tonumber(p3)
					p4 = tonumber(p4, 16)
					if (p2>=0 and p2<=0x3ff and p3>=1 and p3<=100 and p4) then
						if (p3>1) then
							if (string.match(args, "^[%s%t]*%w+[%s%t]*,[%s%t]*0?[xX]?%x+[%s%t]*,[%s%t]*%d+[%s%t]*,[%s%t]*0?[xX]?%x+[%s%t]*"..string.rep(",[%s%t]*0?[xX]?%x+[%s%t]*",p3-1))) then
								ack = "DONE"
							else
								ack = "ERR_"..tostring(SE_ERR_PARA)
							end
						else
							ack = "DONE"
						end
					else
						ack = "ERR_"..tostring(SE_ERR_PARA)
					end
				else
					ack = "ERR_"..tostring(SE_ERR_PARA)
				end
			else
				ack = "ERR_"..tostring(SE_ERR_FORMT)
			end
		
		elseif(cmd=="eeprom read") then
		--{"eeprom read" 		,"(PCB name,address,length", "args=3 p1=enum(testbase,dmm,codec,datalogger,power,translation) p2=range[0x0, 0x3FF] or [0x0, 0xFF] p3=range[1, 100] ", "read EEPROM"},
			local p1, p2, p3 = string.match(args, "^[%s%t]*(%w+)[%s%t]*,[%s%t]*0?[xX]?(%x+)[%s%t]*,[%s%t]*(%d+)[%s%t]*")
			if (p1 and p2 and p3) then
				if (p1=="testbase" or p1=="dmm" or p1=="codec" or p1=="datalogger" or p1=="power" or p1=="translation") then
					p2 = tonumber(p2, 16)
					p3 = tonumber(p3)
					if (p2>=0 and p2<=0x3ff and p3>=1 and p3<=100) then
						if (p3>1) then
							ack = "0xff"
							ack = ack..string.rep(",0xff",p3-1)
						else
							ack = "0xff"
						end
					else
						ack = "ERR_"..tostring(SE_ERR_PARA)
					end
				else
					ack = "ERR_"..tostring(SE_ERR_PARA)
				end
			else
				ack = "ERR_"..tostring(SE_ERR_FORMT)
			end
		
		elseif(cmd=="eeprom write string") then
		--{"eeprom write  string" ,"PCB name, address,\"string\"", "args=3 p1=enum(testbase,dmm,codec,datalogger,power,translation) p2=range[0x0, 0x3FF] or [0x0, 0xFF] ", "write EEPROM   eeprom write(testbase,00,\"hello,board\")"},
			local p1, p2, p3 = string.match(args, "^[%s%t]*(%w+)[%s%t]*,[%s%t]*0?[xX]?(%x+)[%s%t]*,[%s%t]*(\".*\")[%s%t]*")
			if (p1 and p2 and p3) then
				if (p1=="testbase" or p1=="dmm" or p1=="codec" or p1=="datalogger" or p1=="power" or p1=="translation") then
					p2 = tonumber(p2, 16)
					if (p2>=0 and p2<=0x3ff) then
						ack = "DONE"
					else
						ack = "ERR_"..tostring(SE_ERR_PARA)
					end
				else
					ack = "ERR_"..tostring(SE_ERR_PARA)
				end
			else
				ack = "ERR_"..tostring(SE_ERR_FORMT)
			end
			
			
		elseif(cmd=="eeprom read string") then
		--{"eeprom read  string" 	,"PCB name, address,length", "args=3 p1=enum(testbase,dmm,codec,datalogger,power,translation) p2=range[0x0, 0x3FF] or [0x0, 0xFF] p3=range[1, 200] ", "read EEPROM Send: eeprom read(testbase Board,00,11) ACK: ACK(\"hello,board\")"},
			local p1, p2, p3 = string.match(args, "^[%s%t]*(%w+)[%s%t]*,[%s%t]*0?[xX]?(%x+)[%s%t]*,[%s%t]*(%d+)[%s%t]*")
			if (p1 and p2 and p3) then
				if (p1=="testbase" or p1=="dmm" or p1=="codec" or p1=="datalogger" or p1=="power" or p1=="translation") then
					p2 = tonumber(p2, 16)
					p3 = tonumber(p3)
					if (p2>=0 and p2<=0x3ff and p3>=1 and p3<=200) then
						ack = "\""
						if (p3>1) then
							ack = ack..string.rep("x",p3)
						else
							ack = "x"
						end
						ack = ack.."\""
					else
						ack = "ERR_"..tostring(SE_ERR_PARA)
					end
				else
					ack = "ERR_"..tostring(SE_ERR_PARA)
				end
			else
				ack = "ERR_"..tostring(SE_ERR_FORMT)
			end
		
		end
	
	else
		err_code = SE_ERR_FORMT
		ack = "ERR_"..tostring(SE_ERR_FORMT)
	end
	
	return ack
	
end


local function HandleArmMessage(__msg)
	
	--MessageBox("about to start ARM simulator")
	
	local msg = __msg
	local r = nil
	--do return "debug" end
		
	if (msg and #msg > 0) then
		--MessageBox(tostring(msg))		
		local payload = CheckMessageFrame(msg)
		--MessageBox("payload = "..tostring(payload))
		if(payload) then --valid msg frame
			local cmd_index = CheckCmd(payload)
			--MessageBox("cmd index = "..tostring(cmd_index))
			if (cmd_index > 0) then
				local ack = CheckArg(cmd_index, payload)
				r = ack
			else
				r = "ERR_"..tostring(SE_ERR_FORMT)
			end
		else
			r = "ERR_"..tostring(err_code)
		end	
	end
	
	if (uuid and r) then
		r = string.format("[%s]ACK(%s)", uuid, r)
		r = r..term_string
	end
	
	return r
end

local function EncodeBytes(__string)
	local r = ""
	if (type(__string)=="string" and #__string>0) then
		for n=1, #__string do
			r = r..string.format("%02X", tonumber(string.byte(__string, n)))
		end
	end
	return r
end

--dut command handling
local function HandleDutMessage(__msg)	
	local msg = __msg
	local r = nil	
	if (msg and #msg > 0) then
		--print("DutMessage ["..EncodeBytes(msg).."]")				
		if (string.match(msg, "[\r\n]$")) then
			if (string.match(msg, "diags")) then
				r = diags_msg
			else
				r = "[00000134:0DAF7D74] :-)"
			end
		else
			return nil;
		end
	end
	
	if (r) then
		r = r.."\r\n"
	end
	
	return r
end





local socket = require("socket")

local echo_client = nil
local function echo_7605(__cmd)
	echo_client:send(__cmd)	
end

local function do_reply(__cmd,connected_clients, client_index, port)
	local cmd = __cmd.."\r\n"
	if (port=="7600") then
		local ack = HandleArmMessage(cmd)
		if (ack) then
			assert(connected_clients[client_index]:send(ack))
		end
		echo_7605(cmd)
		
	elseif (port=="7601") then
		local ack = HandleDutMessage(cmd)
		if (ack) then
			assert(connected_clients[client_index]:send(ack))
		end
	elseif (port=="7605") then
		
	end

end
 
local function on_connect(connected_clients,a_connect, port)
    connected_clients[#connected_clients+1] = a_connect
    print(string.format("Server[%s][%d] client connected successfully!\n", port, #connected_clients)) 
end

local function on_disconnect(connected_clients,client_index, port)
	connected_clients[client_index]:close()
	table.remove(connected_clients, client_index)
	print(string.format("Server[%s][%d] client disconnected!\n", port, client_index))  
end

local function on_data(connected_clients, client_index, msg, port)
	if msg then
		print(string.format("Server[%s][%d] client recv: ", port, client_index)..msg.."\n")
		do_reply(msg,connected_clients, client_index, port)
	end
end

local function start_server(server,connected_clients,port)
	while 1 do
		local t0 
		--t0 = socket.gettime()
		
		local a_connect = server:accept()
		
		--if port=="7600" then
		--	print(string.format("[DEBUG 7600 SVR] accept() taken time [%f] ms", (socket.gettime()-t0)*1000)) 
		--end
		
		
		if a_connect then on_connect(connected_clients,a_connect,port) end
		for client_index, a_client in pairs(connected_clients) do
			--t0 = socket.gettime()
			local recvt, sendt, status = socket.select({a_client}, nil, 0)
			--if port=="7600" then
			--	print(string.format("[DEBUG 7600 SVR] select() taken time [%f] ms", (socket.gettime()-t0)*1000)) 
			--end
			
			
			if #recvt > 0 then
				--t0 = socket.gettime()
				local msg, receive_status = a_client:receive()
				--if port=="7600" then
				--	print(string.format("[DEBUG 7600 SVR] receive() taken time [%f] ms", (socket.gettime()-t0)*1000)) 
				--end
				
				
				if receive_status ~= "closed" then
					on_data(connected_clients,client_index,msg,port)
				else
					on_disconnect(connected_clients,client_index,port)
				end
			end
		end
		--socket.sleep(0.0001)
	end
end

local function start_deubg_server(server,connected_clients,port)
	while 1 do
		local a_connect = nil
		if (#connected_clients~=2) then
			a_connect = server:accept()
			if a_connect then
		   		on_connect(connected_clients,a_connect,port) 
			end
		end
		
		local client_0 = connected_clients[1]
		local recvt, sendt, status = socket.select({client_0}, nil, 1)
		if #recvt > 0 then
			local msg, receive_status = client_0:receive()
			if receive_status ~= "closed" then
				if #connected_clients==2 then
					assert(connected_clients[2]:send(msg.."\r\n"))
				end
			end
		end
		if (#connected_clients==0) then
			socket.sleep(0.01)
		end
		--socket.sleep(0.0001)
	end
end

local function init_server(host,port)
	local server = assert(socket.bind(host, port, 1024))
	server:settimeout(0)
	return server
end

local function new_server(__port)
	local host = "127.0.0.1"
	local port = __port
	local server = nil
	local connected_clients = {}
	print("Create TCP server for port: "..port.."\n")
	server = init_server(host,port)	
	
	if (port=="7605") then
		start_deubg_server(server,connected_clients,port)
	elseif (port=="7600") then
		if (not echo_client) then
			echo_client = assert(socket.connect(host, 7605))
			echo_client:settimeout(0)
		end
		start_server(server,connected_clients,port)
	else
		start_server(server,connected_clients,port)
	end
end

new_server(...)
]=====]


--local socket = require("socket")
--function sleep(n)
--  socket.select(nil, nil, n)
-- end

local ltimer = require("lzmq.timer")

local thread = llthreads.new(thread_code, "7605")
assert(thread:start(true))

local thread = llthreads.new(thread_code, "7600")
assert(thread:start(true))

local thread = llthreads.new(thread_code, "7601")
assert(thread:start(true))

ltimer.sleep(10000000000000000)
--sleep(10000000000000)
--os.execute("sleep 10000000000000")