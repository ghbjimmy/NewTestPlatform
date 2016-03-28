#!/usr/bin/env python
# -*- coding: utf-8 -*-

from .. import RPCBatchProtocol, RPCRequest, RPCResponse, RPCErrorResponse,\
               InvalidRequestError, MethodNotFoundError, ServerError,\
               InvalidReplyError, RPCError, RPCBatchRequest, RPCBatchResponse

from uuid import uuid1

import json

SERVER_READY = '_my_rpc_server_is_ready'

class FixedErrorMessageMixin(object):
    def __init__(self, *args, **kwargs):
        if not args:
            args = [self.message]
        super(FixedErrorMessageMixin, self).__init__(*args, **kwargs)

    def error_respond(self):
        response = JSONRPCErrorResponse()

        response.error = self.message
        response.unique_id = None
        response._jsonrpc_error_code = self.jsonrpc_error_code
        return response


class JSONRPCParseError(FixedErrorMessageMixin, InvalidRequestError):
    jsonrpc_error_code = -32700
    message = 'Parse error'

class JSONRPCInvalidRequestError(FixedErrorMessageMixin, InvalidRequestError):

    jsonrpc_error_code = -32600
    message = 'Invalid Request: '
    def __init__(self, msg=''):
        super(JSONRPCInvalidRequestError, self).__init__()
        if msg:
            self.message = msg

class JSONRPCMethodNotFoundError(FixedErrorMessageMixin, MethodNotFoundError):
    jsonrpc_error_code = -32601
    message = 'Method not found'
    def __init__(self, msg=''):
        super(JSONRPCMethodNotFoundError, self).__init__()
        if msg:
            self.message = msg


class JSONRPCInvalidParamsError(FixedErrorMessageMixin, InvalidRequestError):
    jsonrpc_error_code = -32602
    message = 'Invalid params'
    def __init__(self, msg=''):
        super(JSONRPCInvalidParamsError, self).__init__()
        if msg:
            self.message = msg

class JSONRPCInternalError(FixedErrorMessageMixin, InvalidRequestError):
    jsonrpc_error_code = -32603
    message = 'Internal error'

class JSONRPCServerStopError(FixedErrorMessageMixin, ServerError):
    jsonrpc_error_code = -32604
    message = "Server Stop Requested"

class JSONRPCTimeoutError(FixedErrorMessageMixin, RPCError):
    jsonrpc_error_code = -32610
    message = 'Timed out'
    def __init__(self, msg=''):
        super(JSONRPCTimeoutError, self).__init__()
        if msg:
            self.message = msg

class JSONRPCServerError(FixedErrorMessageMixin, InvalidRequestError):
    jsonrpc_error_code = -32000
    message = ''
    def __init__(self, msg=''):
        super(JSONRPCServerError, self).__init__()
        if msg:
            self.message = msg

class JSONRPCPluginError(FixedErrorMessageMixin, InvalidRequestError):
    jsonrpc_error_code = -32620
    message = ''
    def __init__(self, msg=''):
        super(JSONRPCPluginError, self).__init__()
        if msg:
            self.message = msg

class JSONRPCSuccessResponse(RPCResponse):
    def _to_dict(self):
        return {
            'jsonrpc': self.version,
            'id': self.unique_id,
            'result': self.result,
        }

    def serialize(self):
        return json.dumps(self._to_dict())

class JSONRPCErrorResponse(RPCErrorResponse):

    def _to_dict(self):
        return {
            'jsonrpc': self.version,
            'id': self.unique_id,
            'error': {
                'message': str(self.error),
                'code': self._jsonrpc_error_code,
            }
        }

    def serialize(self):
        return json.dumps(self._to_dict())

def _get_code_and_message(error):
    assert isinstance(error, (Exception, str))
    if isinstance(error, Exception):
        if hasattr(error, 'jsonrpc_error_code'):
            code = error.jsonrpc_error_code
            msg = str(error)
        elif isinstance(error, InvalidRequestError):
            code = JSONRPCInvalidRequestError.jsonrpc_error_code
            msg = JSONRPCInvalidRequestError.message
        elif isinstance(error, MethodNotFoundError):
            code = JSONRPCMethodNotFoundError.jsonrpc_error_code
            msg = JSONRPCMethodNotFoundError.message
        else:
            # allow exception message to propagate
            code = JSONRPCServerError.jsonrpc_error_code
            msg = str(error)
    else:
        code = -32000
        msg = error

    return code, msg

class JSONRPCRequest(RPCRequest):
    def error_respond(self, error):
        if not self.unique_id:
            return None

        response = JSONRPCErrorResponse()

        code = error.jsonrpc_error_code

        response.error = error.message
        response.unique_id = self.unique_id
        response._jsonrpc_error_code = code
        return response

    def respond(self, result):
        response = JSONRPCSuccessResponse()

        if not self.unique_id:
            return None

        response.result = result
        response.unique_id = self.unique_id
        response.version = self.version

        return response

    def __eq__(self, other):
        if self.function!= other.function:
            return False
        if self.params!=other.params:
            return False
        if self.version!=other.version:
            return False
        return True

    def serialize(self):
        return json.dumps(self._to_dict())


class JSONRPCProtocol(RPCBatchProtocol):

    #change all unicode data back to ascii data
    def load_json(self, data):
        val = json.loads(data)
        return_val = {}
        for k, v in val.items():
            return_val[str(k)] = str(v)
        return return_val

    def _get_unique_id(self):
        return uuid1().hex

    def parse_request(self, data):
        try:
            print "test log"
            print data
            req = json.loads(data)
        except Exception as e:
            raise JSONRPCParseError()

        if isinstance(req, list):
            # batch request
            requests = None
            for subreq in req:
                try:
                    requests.append(self._parse_subrequest(subreq))
                except RPCError as e:
                    requests.append(e)
                except Exception as e:
                    requests.append(JSONRPCInvalidRequestError())

            if not requests:
                raise JSONRPCInvalidRequestError()
            return requests
        else:
            return self._parse_subrequest(req)


        return request

    def stop_respond(self):
        response = JSONRPCSuccessResponse()
        response.version = self.version
        response.unique_id = '0'
        response.result = 'PASS'
        return response
