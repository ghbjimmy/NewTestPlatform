--publisher message define

local nf={}

--Nofication
nf.TEST_ON_START 			= "TEST_ON_START"
nf.TEST_ON_STOP 			= "TEST_ON_STOP"
nf.TEST_ON_ERROR 			= "TEST_ON_ERROR"
nf.TEST_ON_FINISHED 		= "TEST_ON_FINISHED"
nf.TEST_ON_ITEM_START 		= "TEST_ON_ITEM_START"
nf.TEST_ON_ITEM_FINISHED 	= "TEST_ON_ITEM_FINISH"
nf.TEST_ON_PAUSE 			= "TEST_ON_PAUSE"
nf.TEST_ON_RESUME 			= "TEST_ON_RESUME"


--Action
nf.TEST_DO_START 			= "TEST_DO_START"
nf.TEST_DO_STOP 			= "TEST_DO_STOP"
nf.TEST_DO_PAUSE 			= "TEST_DO_PAUSE"
nf.TEST_DO_RESUME 			= "TEST_DO_RESUME"
nf.TEST_DO_STEP 			= "TEST_DO_STEP"

--Request
nf.LOAD_SEQ					= "TEST_LOAD_SEQ"
nf.GET_SEQUENCE				= "GET_SEQUENCE"
nf.SET_GLOBAL				= "SET_GLOBAL"
nf.GET_GLOBAL				= "GET_GLOBAL"
nf.GET_GLOBAL				= "GET_GLOBAL"


return nf;
