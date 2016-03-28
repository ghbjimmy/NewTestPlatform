x527 Test Sequencer
=====

To install the python requirements do

    easy_install pip
    pip install -r requirements.txt



To launch the P2 dry run environment (dry run only)

	download Lua 5.1.4 source package
	make macosx
	make install
	install luarocks and all dependent libs in dependancies.sh (or copy all lib files)
	sudo dependancies.sh
	get tm_platform_v2 repositary from IA gitlab (P2_HW branch)
	launch LuaDriver/Driver/test_engine_C_zmq.lua -u N (0-3) as test engine
	launch tools/subscriber.lua -a tcp://127.0.0.1:680x to monitor DUT output
	modify sequencer.py to let stop_on_fail=False (for debug purpose only)
	launch python-sequencer/x527/sequencer/sequencer.py -s N (N match test engine)
	launch python-sequencer/x527/sdb.py to start sequence debugging
	load test plan csv file in sdb tool and run