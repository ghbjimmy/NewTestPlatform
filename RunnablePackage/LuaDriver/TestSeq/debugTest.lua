Module = "X473F_D2.3_20150618_Ray";
Version = "20150618";

-- APP_PATH = "/Users/ivangan/Desktop/Debug/"
if(tc and tc.AppDir()) then
    package.path = package.path..";"..tostring(tc.AppDir()).."Driver/?.lua"
else
    package.path = package.path..";".."./?.lua"
end

require "Global"

local  boot_sub= 
{
  {tid="01",  name="connect UART",              lower=0,      upper=4,    unit="",   entry=connect,      parameter=IO_TABLE.UUT_UART_CONTROL},
  {tid="A1",  name="Reset_Fixture",             lower="",    upper="",    unit="",    entry=reset,        parameter=nil,                visible=1},
  {tid="A2a",  name="Connect_Uart_To_DUT",       lower="",    upper="",    unit="",    entry=relay,        parameter=IO_TABLE.UUT_UART_CONTROL,   visible=1},
  {tid="A4",  name="Enable_BATT_Power",         lower="",    upper="",    unit="",    entry=relay,        parameter=IO_TABLE.BATT_EN,            visible=1},
  {tid="A5",  name="Connect_BATT_Voltage_Relay",lower="",    upper="",    unit="",    entry=relay,        parameter=IO_TABLE.PP_VBATT,           visible=1},
  {tid="A3",  name="Supply_BATT_Voltage_4.3V",  lower="",    upper="",    unit="",    entry=supply,       parameter={IO_TABLE.PP_VBATT,4.3},    visible=1},
  {tid="A2x", name="delay",                   lower="",    upper="",    unit="",    entry=delay,        parameter=1000,   visible=1},
  {tid="A2ab",  name="connect button",            lower="",    upper="",    unit="",    entry=relay,        parameter=IO_TABLE.BUTTON1_L,   visible=1},
  {tid="A2c",  name="delay",                    lower="",    upper="",    unit="",    entry=delay,        parameter=500,   visible=1},
  {tid="A2d",  name="disconnect button",         lower="",    upper="",    unit="",    entry=disconnect,   parameter=IO_TABLE.BUTTON1_L,   visible=1},
  {tid="A8",  name="Ready To Diags",            lower="",    upper="",    unit="",    entry=ready,        parameter=nil,              visible=1},
  {tid="A9",  name="version",                   lower="",    upper="",    unit="",    entry=diags,        parameter={"version", "1J0195ax"},   visible=1},
  {tid="A10", name="Measure_BATT_Voltage",      lower=-10,    upper=10,    unit="",   entry=measure,      parameter=VT.PP_BATT_VCC_FB,  visible=1},
};

local LVBOOST_Sub = {
  {tid="A02a",name="Initial enable LVBoost",   lower="OK",     upper="OK",     unit=nil,   entry=diags,parameter={"socgpio --pin 18 --port 1 --output 1","OK"}},
  {tid="A04",name="Measure PP5V0_LVBOOST",    lower=5.2*0.98, upper=5.2*1.02, unit=nil,   entry=measure,parameter=VT.PP5V0_LVBOOST},
  {tid="A05",name="Disable LVBoost1",         lower="OK",     upper="OK",                 entry=diags,parameter={"socgpio --pin 18 --port 1 --output 0","OK"}},
  {tid="A07",name="Check LVBoost GPIO1",      lower="",       upper="",        unit=nil,  entry=diags,parameter={"socgpio --port 1 --pin 18 --get","SoC GPIO%[1,18%]%s*=%s*(%d)"}},
  {tid="A09",name="Turn off PP1V8_SW3B_DIS",  lower="",       upper="",        unit=nil,  entry=diags,parameter={"pmuset --switch buck3 3b --off","Done"}},
  {tid="A09a",name="delay",             lower="",       upper="",        unit=nil,  entry=delay,parameter=100,visible=0},
  {tid="A10",name="Measure PP1V8_SW3B_DIS",   lower=-1,       upper=1,        unit=nil,  entry=measure,parameter=VT.PP1V8_SW3B_DIS}, 
  {tid="A12",name="Turn on eload",            lower="",       upper="",        unit=nil,  entry=eload,parameter={IO_TABLE.E_LOAD_PP5V0_LVBOOST,10},visible=0},
  -- {tid="A13",name="Connect ELOAD to LVBOOST", lower="",       upper="",        unit=nil,  entry=fct.ELOAD_TO_PP5V0_LVBOOST,parameter=nil,visible=0},
  -- {tid="A14",name="Set eload 10mA",           lower="",       upper="",        unit=nil,  entry=fct.SetELoadCC,parameter=10,visible=0},
  {tid="A15",name="delay 100ms",              lower="",       upper="",        unit=nil,  entry=delay,parameter=100,visible=0},
  {tid="A16",name="Set eload 0mA",            lower="",       upper="",        unit=nil,  entry=eload,parameter=0,visible=0},
  {tid="A17",name="Turn off eLoad",           lower="",       upper="",        unit=nil,  entry=relay,parameter=IO_TABLE.E_LOAD_DISCONNECT,visible=0},
  {tid="A18",name="delay 100ms",              lower="",       upper="",        unit=nil,  entry=delay,parameter=100,visible=0},  
  {tid="A19",name="PP5V0_LVBOOST@off",        lower=-0.05,    upper=0.5,       unit="V",  entry=measure,parameter=VT.PP5V0_LVBOOST},
  {tid="A20",name="AMUX PP5V0_LVBOOST@off",   lower=0,        upper=200,       unit="mV", entry=diags,parameter={"pmuadc --read appmux_in2","appmux_in2:%s*(%d+.?%d*)"}},
  {tid="A21",name="PPVDD_MAIN@off",           lower=4.3*0.9,  upper=4.3*1.1,   unit="V",  entry=measure,parameter=VT.PPVDD_MAIN_FB},
  {tid="A21a",name="Vbatt@off",               lower=4.3*0.99, upper=4.3*1.01,  unit="V",  entry=measure,parameter=VT.PPBATT_PWRBOARD_S},
  {tid="A22",name="Turn on eload",            lower=nil,      upper=nil,       unit=nil,  entry=eload,parameter=={IO_TABLE.E_LOAD_PP5V0_LVBOOST,0},visible=0},
  {tid="A24",name="Enable LVBoost",           lower="OK",     upper="OK",      unit=nil,  entry=diags,parameter={"socgpio --pin 18 --port 1 --output 1","OK"}},
  {tid="A25",name="Check LVBoost GPIO",       lower=1,        upper=1,         unit=nil,  entry=diags,parameter={"socgpio  --port 1 --pin 18 --get","SoC GPIO%[1,18%]%s*=%s*(%d)"}},
  {tid="A26",name="Set eload 20mA",           lower=nil,      upper=nil,       unit="V",  entry=eload,parameter=20,visible=0},
  {tid="A27",name="delay 100ms",              lower=nil,      upper=nil,       unit=nil,  entry=delay,parameter=100,visible=0},
  {tid="A28",name="PP5V0_LVBOOST@20mA",       lower=5.2*0.98, upper=5.2*1.02,  unit="V",  entry=measure,parameter=VT.PP5V0_LVBOOST},
  {tid="A29",name="AMUX PP5V0_LVBOOST@20mA",  lower=2000,     upper=3000,      unit="mV", entry=diags,parameter={"pmuadc --read appmux_in2","appmux_in2:%s*(%d+.?%d*)"}},
  {tid="A30",name="PPVDD_MAIN@20mA",          lower=4.29*0.9, upper=4.29*1.1,  unit="V",  entry=measure,parameter=VT.PPVDD_MAIN_FB},
  {tid="A31",name="Vbatt@20mA",               lower=4.3*0.99, upper=4.3*1.01,  unit="V",  entry=measure,parameter=VT.PPBATT_PWRBOARD_S},
  {tid="A32",name="Set eload 100mA",          lower=nil,      upper=nil,       unit="V",  entry=eload,parameter=100,visible=0},
  {tid="A33",name="delay 100ms",              lower=nil,      upper=nil,       unit=nil,  entry=delay,parameter=100,visible=0},
  {tid="A34",name="PP5V0_LVBOOST@100mA",      lower=5.2*0.98, upper=5.2*1.02,  unit="V",  entry=measure,parameter=VT.PP5V0_LVBOOST},
  {tid="A35",name="AMUX PP5V0_LVBOOST@100mA", lower=2000,     upper=3000,      unit="mV", entry=diags,parameter={"pmuadc --read appmux_in2","appmux_in2:%s*(%d+.?%d*)"}},
  {tid="A36",name="PPVDD_MAIN@100mA",         lower=4.25*0.9, upper=4.25*1.1,  unit="V",  entry=measure,parameter=VT.PPVDD_MAIN_FB},
  {tid="A37",name="Vbatt@100mA",              lower=4.3*0.99, upper=4.3*1.01,  unit="V",  entry=measure,parameter=VT.PPBATT_PWRBOARD_S},
  {tid="A38",name="Set eload 200mA",          lower=nil,      upper=nil,       unit=nil,  entry=eload,parameter=200,visible=0},
  {tid="A39",name="delay 100ms",              lower=nil,      upper=nil,       unit=nil,  entry=delay,parameter=100,visible=0},
  {tid="A39a",name="Eload_Current",           lower=200*0.9,  upper=200*1.1,   unit=nil,  entry=iload,parameter=nil},
  {tid="A40",name="PP5V0_LVBOOST@200mA",      lower=5.2*0.98, upper=5.2*1.02,  unit="V",  entry=measure,parameter=VT.PP5V0_LVBOOST},
  {tid="A42",name="AMUX PP5V0_LVBOOST@200mA", lower=2000,     upper=3000,      unit="mV", entry=diags,parameter={"pmuadc --read appmux_in2","appmux_in2:%s*(%d+.?%d*)"}},
  {tid="A43",name="PPVDD_MAIN@200mA",         lower=4.2*0.9,  upper=4.2*1.1,   unit="V",  entry=measure,parameter=VT.PPVDD_MAIN_FB},
  {tid="A44",name="Vbatt@200mA",              lower=4.3*0.99, upper=4.3*1.01,  unit="V",  entry=measure,parameter=VT.PPBATT_PWRBOARD_S},
  {tid="A45",name="Disable LVBoost",          lower="OK",     upper="OK",      unit=nil,  entry=diags,parameter={"socgpio --pin 18 --port 1 --output 0","OK"}},
  {tid="A46",name="delay 100ms",              lower=nil,      upper=nil,       unit=nil,  entry=delay,parameter=100,visible=0},
  {tid="A47",name="Disable eLoad",            lower=nil,      upper=nil,       unit=nil,  entry=eload,parameter=0,visible=0},
  {tid="A48",name="Turn off eload",           lower=nil,      upper=nil,       unit=nil,  entry=relay,parameter=IO_TABLE.E_LOAD_DISCONNECT,visible=0},
};

local LVBOOST_Item={name="[LV]",entry=nil,parameter=nil,sub=LVBOOST_Sub};

local  carbon_Init_sub= 
{
  {tid="01",  name="mwasure VDD",               lower=0,      upper=4,    unit="",    entry=measure,      parameter=VT.PP1V8_HALT_LDO1}, 
  {tid="01a",  name="connect",                   lower=0,      upper=4,    unit="",    entry=measure,      parameter=VT.PP1V8_HALT_SW3A}, 
  {tid="A8",name="Connectivity Test_init",      lower="",     upper="",   unit="",    entry=diags,parameter={"sensor --sel accel --init","OK"},visible=1},
  {tid="A9",name="Connectivity Test_check",     lower="",     upper="",   unit="",    entry=diags,parameter={"sensor --sel accel --c","passed"},visible=1},
  {tid="A10",name="Connectivity Test_Turn off", lower="",     upper="",   unit="",    entry=diags,parameter={"sensor --sel accel --turnoff","OK"},visible=1},
  {tid="A11",name="delay1",                      lower=nil,    upper=nil,  unit="",    entry=delay,parameter=50,sub=nil,visible=1},
  {tid="A12",name="Accel Self-test_init",       lower="",     upper="",   unit="",    entry=diags,parameter={"sensor --sel accel --init","OK"},visible=1},
  {tid="A13",name="delay",                      lower=nil,    upper=nil,  unit="",    entry=delay,parameter=50,sub=nil,visible=1},
  {tid="A14",name="Accel Self-test_exectest",   lower="",     upper="",   unit="",    entry=diags,parameter={"sensor --sel accel --exectest selftest","passed"},visible=1},
  {tid="A15",name="Accel Self-test_turnoff",    lower="",     upper="",   unit="",    entry=diags,parameter={"sensor --sel accel --turnoff","OK"},visible=1},
};

local Carbon_8G_Sub=
{
  {tid="B01",name="Set Init ODR_8g",  lower="",    upper="",    entry=diags,  parameter={"sensor --sel accel --init","OK"}},
  {tid="B02",name="Set rate ODR_8g",  lower="",    upper="",    entry=diags,  parameter={"sensor --sel accel --set rate 100","OK"}},  
  {tid="B03",name="Set rang ODR_8g",  lower="",    upper="",    entry=diags,  parameter={"sensor --sel accel --set dynamic_range 8","OK"}}, 
  {tid="B04",name="Set samp ODR_8g",  lower="",    upper="",    entry=diags,  parameter={"sensor --sel accel --sample 500ms --stats","OK"}}, 

  {tid="B1",name="samplea",             lower=0,     upper=110,    entry=parse,  parameter={"accel_gyro_200Hz-IvanGan.txt","{{CM_accel_captured_sample_count_200Hz}}"}},  
  {tid="B1a",name="invalidCNTa",         lower=0,     upper=110,    entry=parse,  parameter={"accel_gyro_200Hz-IvanGan.txt","{{CM_accel_invalid_sample_count_200Hz}}"}}, 
  {tid="B1b",name="orda",                lower=90,     upper=110,    entry=parse,  parameter={"accel_gyro_200Hz-IvanGan.txt","{{CM_accel_odr_200Hz}}"}}, 
  {tid="B2",name="accel_average_8g_x",lower=-0.198, upper=0.198,  entry=parse,  parameter={"accel_gyro_200Hz-IvanGan.txt","{{CM_accel_average_x_200Hz}}"}},
  {tid="B3",name="accel_average_8g_y",lower=-0.198, upper=0.198,  entry=parse,  parameter={"accel_gyro_200Hz-IvanGan.txt","{{CM_accel_average_y_200Hz}}"}},
  {tid="B4",name="accel_average_8g_z",lower=0.82,   upper=1.18,   entry=parse,  parameter={"accel_gyro_200Hz-IvanGan.txt","{{CM_accel_average_z_200Hz}}"}},
  {tid="B5",name="accel_std_8g_x",    lower=0,      upper=0.05,   entry=parse,  parameter={"accel_gyro_200Hz-IvanGan.txt","{{CM_accel_stdev_x_200Hz}}"}},  
  {tid="B6",name="accel_std_8g_y",    lower=0,      upper=1.05,   entry=parse,  parameter={"accel_gyro_200Hz-IvanGan.txt","{{CM_accel_stdev_y_200Hz}}"}},
  {tid="B7",name="accel_std_8g_z",    lower=0,      upper=0.05,   entry=parse,  parameter={"accel_gyro_200Hz-IvanGan.txt","{{CM_accel_stdev_z_200Hz}}"}},
  {tid="B0",name="Set accel ODR_8g",  lower="",    upper="",    entry=diags,  parameter={"sensor --sel accel --set dynamic_range 8","OK"}}, 
}

local Carbon_16G_Sub=
{
  {tid="01",name="Set Init ODR_16g",  lower="",    upper="",    entry=diags,  parameter={"sensor --sel accel --init","OK"}},
  {tid="02",name="Set rate ODR_16g",  lower="",    upper="",    entry=diags,  parameter={"sensor --sel accel --set rate 100","OK"}},  
  {tid="03",name="Set rang ODR_16g",  lower="",    upper="",    entry=diags,  parameter={"sensor --sel accel --set dynamic_range 16","OK"}}, 
  {tid="04",name="Set samp ODR_16g",  lower="",    upper="",    entry=diags,  parameter={"sensor --sel accel --sample 500ms --stats","OK"}}, 

  {tid="1x",name="sample",             lower=0,     upper=110,    entry=parse,  parameter={"accel_gyro_200Hz-IvanGan.txt","{{CM_accel_captured_sample_count_200Hz}}"}},  
  {tid="1c",name="invalidCNT",         lower=0,     upper=110,    entry=parse,  parameter={"accel_gyro_200Hz-IvanGan.txt","{{CM_accel_invalid_sample_count_200Hz}}"}}, 
  {tid="1v",name="ord",                lower=90,     upper=110,    entry=parse,  parameter={"accel_gyro_200Hz-IvanGan.txt","{{CM_accel_odr_200Hz}}"}}, 
  {tid="2",name="accel_average_16g_x",lower=-0.198, upper=0.198,  entry=parse,  parameter={"accel_gyro_200Hz-IvanGan.txt","{{CM_accel_average_x_200Hz}}"}},
  {tid="3",name="accel_average_16g_y",lower=-0.198, upper=0.198,  entry=parse,  parameter={"accel_gyro_200Hz-IvanGan.txt","{{CM_accel_average_y_200Hz}}"}},
  {tid="4",name="accel_average_16g_z",lower=0.82,   upper=1.18,   entry=parse,  parameter={"accel_gyro_200Hz-IvanGan.txt","{{CM_accel_average_z_200Hz}}"}},
  {tid="5",name="accel_std_16g_x",    lower=0,      upper=0.05,   entry=parse,  parameter={"accel_gyro_200Hz-IvanGan.txt","{{CM_accel_stdev_x_200Hz}}"}},  
  {tid="6",name="accel_std_16g_y",    lower=0,      upper=0.05,   entry=parse,  parameter={"accel_gyro_200Hz-IvanGan.txt","{{CM_accel_stdev_y_200Hz}}"}},
  {tid="7",name="accel_std_16g_z",    lower=0,      upper=0.05,   entry=parse,  parameter={"accel_gyro_200Hz-IvanGan.txt","{{CM_accel_stdev_z_200Hz}}"}},
  {tid="0",name="Set accel ODR_8ga",  lower="",    upper="",    entry=diags,  parameter={"sensor --sel accel --set dynamic_range 8","OK"}}, 
  {tid="0a",name="Turn off accel",  lower="",    upper="",    entry=diags,  parameter={"sensor --sel accel --turnoff","OK"}}, 
}


local Boot_Item  = {name="[BOOT]", entry=nil,parameter=nil,sub=boot_sub};

local lvboost_Item  = {name="[LB]", entry=nil,parameter=nil,sub=LVBOOST_Sub};
local Init_Item  = {name="[CBIN]", entry=nil,parameter=nil,sub=carbon_Init_sub};
local Carbon_8G_Item  = {name="[CB8G]", entry=nil,parameter=nil,sub=Carbon_8G_Sub};
local Carbon_16G_Item  = {name="[CB16G]", entry=nil,parameter=nil,sub=Carbon_16G_Sub};

items = {
  Boot_Item,
  lvboost_Item,
  Init_Item,
  Carbon_8G_Item,
  Carbon_16G_Item
};   
return items;    
