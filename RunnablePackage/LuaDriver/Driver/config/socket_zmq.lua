--- @module config.zmq
-- @alias config
--- ZMQ config file -- uses dummy sockets for the ARM UART and a ZMQ socket for the DUT UART.
--
-- It connects to the serial_master.lua script to get the UART stream.
-- @see serial_master.lua
local config = {}
local zmqport = require "utils.ZmqPortParse"
--===========================--
-- ZMQ Socket addresses
--===========================--
config.SEQUENCE_ADDRESS         = "tcp://*"
config.RESULTS_ADDRESS          = "tcp://0.0.0.0"
config.WATCHDOG_ADDRESS         = "tcp://127.0.0.1"
config.DATALOGGER_ADDRESS       = "tcp://127.0.0.1"
config.DATALOGGER_SVR_ADDRESS   = "tcp://0.0.0.0"

config.SEQUENCE_BASE_PORT       = zmqport.zmqport("TEST_ENGINE_PORT")
config.RESULTS_BASE_PORT        = "TBD"
config.WATCHDOG_BASE_PORT       = zmqport.zmqport("TEST_ENGINE_PUB")
config.DATALOGGER_BASE_PORT     = "TBD"
config.DATALOGGER_SVR_BASE_PORT = config.DATALOGGER_BASE_PORT

-- DUT UART collector address (srial_master side)
config.DUT_UART_ZMQ_ADDRESS     = "tcp://*"
config.DUT_UART_ZMQ_BASE_PORT   = zmqport.zmqport("UART_PUB")
config.FIX_UART_ZMQ_ADDRESS     = "tcp://0.0.0.0"
config.FIX_UART_ZMQ_BASE_PORT   = zmqport.zmqport("ARM_PUB")
config.UART_ZMQ_REQ_ADDRESS     = "tcp://*"
config.UART_ZMQ_REQ_BASE_PORT   = zmqport.zmqport("UART_PORT")
config.FIX_ZMQ_REQ_SVR_ADDRESS     = "tcp://0.0.0.0"
config.FIX_ZMQ_REQ_SVR_BASE_PORT   = zmqport.zmqport("FIXTURE_CTRL_PORT")

-- DUT UART collector address (test_engine side)
config.DUT_UART_ZMQ_SVR_ADDRESS     = "tcp://127.0.0.1"
config.DUT_UART_ZMQ_SVR_BASE_PORT   = config.DUT_UART_ZMQ_BASE_PORT
config.FIX_UART_ZMQ_SVR_ADDRESS     = "tcp://127.0.0.1"
config.FIX_UART_ZMQ_SVR_BASE_PORT   = config.FIX_UART_ZMQ_BASE_PORT
config.UART_ZMQ_REQ_SVR_ADDRESS     = "tcp://127.0.0.1"
config.UART_ZMQ_REQ_SVR_BASE_PORT   = config.UART_ZMQ_REQ_BASE_PORT
config.FIX_ZMQ_REQ_SVR_ADDRESS     = "tcp://127.0.0.1"
config.FIX_ZMQ_REQ_SVR_BASE_PORT   = config.FIX_UART_ZMQ_BASE_PORT


---Socket Address
-- config.FCT_ADDRESS_BASE 	= "169.254.1.32"
 config.FCT_ADDRESS_BASE 	= "127.0.0.1"
config.FIXTURE_PORT				= 7600
config.DUT_UART_PORT		    = 7601
config.SPAM_PORT                = 7602
config.DATALOGGER_PORT          = 7603


config.SNMANAGER_ADDRESS       = "tcp://127.0.0.1"
config.SNMANAGER_BASE_PORT     = zmqport.zmqport("SN_PORT") --SN manager REP port
config.SN_HEARTBEAT_ADDRESS    = "tcp://127.0.0.1"
config.SN_HEARTBEAT_BASE_PORT  = zmqport.zmqport("SN_HEARTBEAT") --SN manager Heartbeat port

config.STATEMACHINE_REP_ADDRESS    = "tcp://127.0.0.1"
config.STATEMACHINE_REP_BASE_PORT  = zmqport.zmqport("SM_PORT") --REP, with GUI for start test or enable UUT, with fixture to start test
config.STATEMACHINE_REQ_ADDRESS    = "tcp://127.0.0.1"
config.STATEMACHINE_REQ_BASE_PORT  =  zmqport.zmqport("SEQUENCER_PORT") --sequencer command REQ port
config.STATEMACHINE_SUB_ADDRESS    = config.STATEMACHINE_REQ_ADDRESS
config.STATEMACHINE_SUB_BASE_PORT  =  zmqport.zmqport("SEQUENCER_PUB") --state machine spy to sequencer
config.STATEMACHINE_PUB_ADDRESS    = "tcp://*"--config.STATEMACHINE_REQ_ADDRESS
config.STATEMACHINE_PUB_BASE_PORT  =  zmqport.zmqport("SM_PUB") --state machine spy to sequencer
config.STATEMACHINE_HEARTBEAT_ADDRESS    = "tcp://127.0.0.1"
config.STATEMACHINE_HEARTBEAT_BASE_PORT  = zmqport.zmqport("SM_HEARTBEAT") --SN manager Heartbeat port

config.FIXTURE_CTRL_SUB_ADDRESS    = "tcp://127.0.0.1"--fixture PUB port.
config.FIXTURE_CTRL_SUB_BASE_PORT  = zmqport.zmqport("FIXTURE_CTRL_PUB")
config.FIXTURE_CTRL_REQ_ADDRESS    = "tcp://127.0.0.1"--fixture control REQ
config.FIXTURE_CTRL_REQ_BASE_PORT  = zmqport.zmqport("FIXTURE_CTRL_PORT")

config.FIXTURE_CTRL_SERIAL_PORT    = "/dev/cu.usbserial-F-CTR"--fixture control serial port
config.FIXTURE_CTRL_SERIAL_SETTING = "115200,n,8,1"

config.GET_LOG_PATH_REQ_ADDRESS    = "tcp://127.0.0.1"--en_test need to get log path send to sequencer
config.GET_LOG_PATH_REQ_BASE_PORT  = zmqport.zmqport("LOG_PATH_PORT")
--===========================--
-- DUT Socket addresses
--===========================--
function get_kanzi_ports()
    local i, t, popen = 0, {}, io.popen
    for filename in popen('ls -a /dev'):lines() do
        if string.match(filename, "cu.kanzi-") then
        	t[#t + 1] = "/dev/"..filename
        	print(string.format("DUT %d is %s", i, t[i + 1]))
        	i = i + 1
        end
    end

    return t, i
end


function get_socket_address(base)
	local t = {}
	local ip_header,ip_tail = base:match("(.*)%.(%d+)")
	print("base :",base,"header :",ip_header,"tail:",ip_tail)
	for i=0,9 do
		t[#t+1]=string.format("%s.%d",ip_header,ip_tail+i);
	end
	return t;
end

config.SERIAL_PORT 		= false
config.DUT_UART_ADDRESS = get_socket_address(config.FCT_ADDRESS_BASE);

config.FIXTURE_ADDRESS          = config.DUT_UART_ADDRESS
config.DATALOGGER_CMD_ADDRESS   = config.DUT_UART_ADDRESS

config.DUT_UART_CONSOLE = "console.dut.zmq"
-- config.FIXTURE_CONSOLE  = "console.dummy"
config.FIXTURE_CONSOLE  = "console.fixture.socket_pub"
config.SPAM_CONSOLE  = "console.fixture.spam_socket"


--===========================--
-- DUT/Station info
--===========================--
config.STATION      = "DEV"
config.UNIT_STAGE   = "POSTSMT2"


--===========================--
-- DUT/Station info
--===========================--
config.TEMPLATE_PATH    = "match/"
config.TEMPLATE_ROOT_PATH = "match/"
config.LOG_PATH = "/vault/Intelli_log"

--===========================--
-- Log Filenames
--===========================--
config.DUT_LOG_FILENAME     = ".uart.txt"
config.HW_LOG_FILENAME      = ".hw_log.txt"
config.VAR_LOG_FILENAME     = ".variable.txt"
config.FLOW_LOG_FILENAME    = ".flow.txt"
config.ENGINE_LOG_FILENAME  = nil
config.ARM_DL_FILENAME      = nil

return config
