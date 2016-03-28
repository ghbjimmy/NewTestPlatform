myvalue = 3

def func_from_plugin(params, unit, timeout):
    global myvalue
    myvalue = params[0]
    return int(params[0])*2

def exc_from_plugin(params, unit, timeout):
    raise Exception('exception from plugin')