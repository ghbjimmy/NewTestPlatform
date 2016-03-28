local config = {}

-------------------------------
-- ZMQ Socket addresses
-------------------------------
-- Sequence/results/watchdog addresses (test_engine side)
config.SEQUENCE_ADDRESS         = "tcp://0.0.0.0"
config.SEQUENCE_BASE_PORT       = 6300
config.RESULTS_ADDRESS          = "tcp://0.0.0.0"
config.RESULTS_BASE_PORT        = 6700
config.WATCHDOG_ADDRESS         = "tcp://127.0.0.1"
config.WATCHDOG_BASE_PORT       = 6500

-- Datalogger collector address (datalogger_master side)
config.DATALOGGER_ADDRESS       = "tcp://127.0.0.1"
config.DATALOGGER_BASE_PORT     = 6600
-- Datalogger collector address (test_engine side)
config.DATALOGGER_SVR_ADDRESS   = "tcp://0.0.0.0"
config.DATALOGGER_SVR_BASE_PORT = config.DATALOGGER_BASE_PORT

-- DUT UART collector address (srial_master side)
config.DUT_UART_ZMQ_ADDRESS     = "tcp://0.0.0.0"
config.DUT_UART_ZMQ_BASE_PORT   = 6800
config.FIX_UART_ZMQ_ADDRESS     = "tcp://0.0.0.0"
config.FIX_UART_ZMQ_BASE_PORT   = 6900
config.UART_ZMQ_REQ_ADDRESS     = "tcp://0.0.0.0"
config.UART_ZMQ_REQ_BASE_PORT   = 7000

-- DUT UART collector address (test_engine side)
config.DUT_UART_ZMQ_SVR_ADDRESS     = "tcp://127.0.0.1"
config.DUT_UART_ZMQ_SVR_BASE_PORT   = config.DUT_UART_ZMQ_BASE_PORT
config.FIX_UART_ZMQ_SVR_ADDRESS     = "tcp://127.0.0.1"
config.FIX_UART_ZMQ_SVR_BASE_PORT   = config.FIX_UART_ZMQ_BASE_PORT
config.UART_ZMQ_REQ_SVR_ADDRESS     = "tcp://127.0.0.1"
config.UART_ZMQ_REQ_SVR_BASE_PORT   = config.UART_ZMQ_REQ_BASE_PORT

-------------------------------
-- DUT Socket addresses
-------------------------------
config.SERIAL_PORT      = false
config.DUT_UART_ADDRESS = {
                            "169.254.1.32", -- DUT 0
                            "169.254.1.33", -- DUT 1
                            "169.254.1.34", -- DUT 2
                            "169.254.1.35", -- DUT 3
                            "169.254.1.36", -- DUT 4
                            "169.254.1.37", -- DUT 5
                            }

config.FIXTURE_ADDRESS          = config.DUT_UART_ADDRESS
config.DATALOGGER_CMD_ADDRESS   = config.DUT_UART_ADDRESS

config.DUT_UART_PORT        = 7601
config.FIXTURE_PORT         = 7600
config.DATALOGGER_CMD_PORT  = 7600
config.DATALOGGER_PORT      = 7603

config.DUT_UART_CONSOLE = "console.dummy"
config.FIXTURE_CONSOLE  = "console.dummy"

-------------------------------
-- DUT/Station info
-------------------------------
config.STATION      = "DEV"
config.UNIT_STAGE   = "POSTSMT2"


-------------------------------
-- DUT/Station info
-------------------------------
config.TEMPLATE_PATH    = "match/"


-------------------------------
-- Log Filenames
-------------------------------
config.UART_LOG_FILENAME    = ".uart.txt"
config.HW_LOG_FILENAME      = ".hw_log.txt"
config.VAR_LOG_FILENAME     = ".variable.txt"
config.FLOW_LOG_FILENAME    = ".flow.txt"

return config