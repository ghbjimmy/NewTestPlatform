--- @module config.zmq
-- @alias config
--- ZMQ config file -- uses dummy sockets for the ARM UART and a ZMQ socket for the DUT UART.
--
-- It connects to the serial_master.lua script to get the UART stream.
-- @see serial_master.lua
local config = {}

--===========================--
-- ZMQ Socket addresses
--===========================--
config.SEQUENCE_ADDRESS         = "tcp://0.0.0.0"
config.RESULTS_ADDRESS          = "tcp://0.0.0.0"
config.WATCHDOG_ADDRESS         = "tcp://17.80.39.233"
config.DATALOGGER_ADDRESS       = "tcp://127.0.0.1"
config.DATALOGGER_SVR_ADDRESS   = "tcp://0.0.0.0"

config.SEQUENCE_BASE_PORT       = 6300
config.RESULTS_BASE_PORT        = 6700
config.WATCHDOG_BASE_PORT       = 6500
config.DATALOGGER_BASE_PORT     = 6600
config.DATALOGGER_SVR_BASE_PORT = config.DATALOGGER_BASE_PORT

-- DUT UART collector address (srial_master side)
config.DUT_UART_ZMQ_ADDRESS     = "tcp://0.0.0.0"
config.DUT_UART_ZMQ_BASE_PORT   = 6800
config.FIX_UART_ZMQ_ADDRESS     = "tcp://0.0.0.0"
config.FIX_UART_ZMQ_BASE_PORT   = 6900
config.UART_ZMQ_REQ_ADDRESS     = "tcp://0.0.0.0"
config.UART_ZMQ_REQ_BASE_PORT   = 7000
config.FIX_ZMQ_REQ_SVR_ADDRESS     = "tcp://0.0.0.0"
config.FIX_ZMQ_REQ_SVR_BASE_PORT   = 7100

-- DUT UART collector address (test_engine side)
config.DUT_UART_ZMQ_SVR_ADDRESS     = "tcp://127.0.0.1"
config.DUT_UART_ZMQ_SVR_BASE_PORT   = config.DUT_UART_ZMQ_BASE_PORT
config.FIX_UART_ZMQ_SVR_ADDRESS     = "tcp://127.0.0.1"
config.FIX_UART_ZMQ_SVR_BASE_PORT   = config.FIX_UART_ZMQ_BASE_PORT
config.UART_ZMQ_REQ_SVR_ADDRESS     = "tcp://127.0.0.1"
config.UART_ZMQ_REQ_SVR_BASE_PORT   = config.UART_ZMQ_REQ_BASE_PORT
config.FIX_ZMQ_REQ_SVR_ADDRESS     = "tcp://127.0.0.1"
config.FIX_ZMQ_REQ_SVR_BASE_PORT   = config.FIX_UART_ZMQ_BASE_PORT

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

function get_ip_address()
	
end

config.SERIAL_PORT 		= false
config.DUT_UART_ADDRESS = get_kanzi_ports()

config.FIXTURE_ADDRESS          = config.DUT_UART_ADDRESS
config.DATALOGGER_CMD_ADDRESS   = config.DUT_UART_ADDRESS

config.DUT_UART_CONSOLE = "console.dut.zmq"
config.FIXTURE_CONSOLE  = "console.dummy"


--===========================--
-- DUT/Station info
--===========================--
config.STATION      = "DEV"
config.UNIT_STAGE   = "POSTSMT2"


--===========================--
-- DUT/Station info
--===========================--
config.TEMPLATE_PATH    = "match/"


--===========================--
-- Log Filenames
--===========================--
config.DUT_LOG_FILENAME     = ".uart.txt"
config.HW_LOG_FILENAME      = ".hw_log.txt"
config.VAR_LOG_FILENAME     = ".variable.txt"
config.FLOW_LOG_FILENAME    = ".flow.txt"

return config