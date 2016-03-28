__author__ = 'wei'

from x527.tinyrpc.protocols.jsonrpc import *
from os import path
import csv
import re
import os

class TestItem(object):
    v_pattern = re.compile('\[\[([^\]]+)\]\]')
    p_pattern = re.compile('{{([^}]+)}}')
    a_pattern = re.compile('<<([^>]+)>>')


    def __init__(self, row):
        self.row = row.copy()
        self.raw_row = row.copy()
        self.valid_return = None # the function to validate the return value
        self.returned_val = None  #varialbe name of the return value
        self.report_to_pudding = False #if the returned value should be reported to pudding
        self.filters = None #filter to decide if this test item should run
        self.kwargs = {} #variable names to be replaced before passed to test engine
        self.tid = row['TID'].strip()
        self.group = row['GROUP'].strip()
        self.unit = row['UNIT'].strip()
        self.judge_pass = None
        self.p_attribute = None

        #parse function
        func_str = row['FUNCTION'].lower().strip()
        m = self.a_pattern.search(func_str)
        if m:
            self.function = m.group(1)
            self.call_engine = False
        else:
            self.function = func_str
            self.call_engine = True
        #parse timeout
        s_timeout = row['TIMEOUT'].strip()
        if len(s_timeout)==0:
            self.timeout=3000
        else:
            self.timeout= int(s_timeout)

        #parse VALIDATE
        if 'VALIDATE' in row:
            if len(row['VALIDATE'].strip())>0:
                self.valid_return = eval('lambda val: ' + row['VALIDATE'])
                self.row.pop('VALIDATE')

        #parse param2
        param2_str = row['PARAM2'].strip()
        if len(param2_str) == 0:
            self.row.pop('PARAM2')
        else:
            self.row['PARAM2'] = param2_str
            m = self.v_pattern.search(param2_str)
            if m:
                self.returned_val = m.group(1)
                self.row.pop('PARAM2')
            m = self.p_pattern.search(param2_str)
            if m:
                self.returned_val = m.group(1)
                self.report_to_pudding = True
                self.row.pop('PARAM2')
            m = self.a_pattern.search(row['PARAM2'])
            if m:
                self.p_attribute = m.group(1)
                self.row.pop('PARAM2')

       #parse param1
        param1_str = row['PARAM1'].strip()
        if len(param1_str) == 0:
            self.row.pop('PARAM1')
        else:
            self.row['PARAM1'] = param1_str
            m = self.v_pattern.findall(param1_str)
            if m:
                for name in m:
                    self.kwargs[name] = 0

        #Parse KEY/VALUE
        if row['KEY']:
            m = self.p_pattern.search(row['KEY'])
            self.filters = {}
            self.filters[m.group(1)] = row['VAL'].strip()

        #parse limits
        self.low_limit = self.row['LOW'].strip()
        self.high_limit = self.row['HIGH'].strip()
        if self.low_limit: #we actually have limits
            if self.low_limit==self.high_limit:
                self.judge_pass = self.match
            else:
                self.judge_pass = self.between
                try:
                    self.low_limit = float(self.low_limit)
                    self.high_limit = float(self.high_limit)
                except ValueError as e:
                    raise JSONRPCInvalidParamsError('error tyring to convert limits for test ' + self.tid
                                                    + ': ' + e.message)


    def match(self, val):
        return val == self.low_limit

    def between(self, val):
        #temporary just throwing an error
        #raise JSONRPCInvalidParamsError('error dummy tyring to judge returned value for test ' + self.tid)
        try:
            val = float(val)
        except ValueError as e:
            raise JSONRPCInvalidParamsError('error tyring to judge returned value for test ' + self.tid
                                            + ': ' + e.message)
        return val>=self.low_limit and val<= self.high_limit

    def __eq__(self, _tid):
        return self.tid == _tid

    def __getitem__(self, item):
        return self.row[item]

    def should_run(self, filter_name, filter_value):
        return filter_value == self.filters.get(filter_name)

    def _to_dict(self):
        jdata = dict(function=self.function)
        params = []
        if self.row.has_key('PARAM1'):
            param1_str = self.row['PARAM1']
            if len(self.kwargs)>0:
                for name, value in self.kwargs.iteritems():
                    param1_str = re.sub('\[\[' + name + '\]\]', str(value), param1_str)
            params.append(param1_str)
        if self.row.has_key('PARAM2'):
            params.append(self.row['PARAM2'])
        jdata['params'] = params
        return jdata


    def __str__(self):
        return str(self.raw_row)


class TestGroup(object):
    def __init__(self, name):
        self.name = name
        self._sequence = []

    def append(self, test_item):
        self._sequence.append(test_item)

    def __len__(self):
        return len(self._sequence)

    def __getitem__(self, idx):
        if idx >= len(self._sequence):
            raise IndexError('group test index out of range')
        else:
            return self._sequence[idx]

    def __eq__(self, _name):
        return self.name==_name

class TestSequenceItor(object):

    def __init__(self, sequence):
        self.sequence = sequence
        self.index = 0
        self.grp_index = 0
        self.ingrp_index = -1

    def __iter__(self):
        return self

    def next(self):
        group = self.sequence.groups[self.grp_index]
        self.ingrp_index +=1
        if self.ingrp_index<len(group):
            return group[self.ingrp_index]
        else:
            self.grp_index +=1
            self.ingrp_index = 0
            if self.grp_index >= len(self.sequence.groups):
                raise StopIteration
            else:
                group = self.sequence.groups[self.grp_index]
                return group[self.ingrp_index]

class TestSequence(object):

    def __init__(self, sequence_db=None):
        self.variables = {}
        self.groups = []
        self.parent = None
        self.name = ''
        self.version = ''
        if sequence_db is None:
            return
        if not isinstance(sequence_db, basestring):
            raise JSONRPCInvalidParamsError('non string as sequence db name')
        db_name = sequence_db.strip().lower()
        if 'default' == db_name:
            self._sequence = [['verb_' + str(i), 'params_' + str(i)] for i in range(20)]
        elif '.csv' == path.splitext(db_name)[1]:
            self.load_csv_sequence(db_name)
        else:
            raise JSONRPCInvalidParamsError('unsupported sequence db format: ' + sequence_db)

    def __len__(self):
        total_len = 0
        for group in self.groups:
            total_len += len(group)
        return total_len

    def __iter__(self):
        return TestSequenceItor(self)

    def __getitem__(self, item):
        if isinstance(item, int):
            dest = int(item)
            index = 0
            for group in self.groups:
                current_group_size = len(group)
                if index+current_group_size>dest:
                    return group[dest-index]
                else:
                    index += current_group_size
            raise IndexError('Test Index out of range')
        #otherwise we assume the item is a group name
        for group in self.groups:
            if group.name==item:
                return group
        #if we still don't find it, we assume it's a tid
        for group in self.groups:
            for test in group._sequence:
                if item==test.tid:
                    return test
        raise KeyError('group or tid ' + item + ' does not exist')

    def get_index(self, tid):
        if not isinstance(tid, str):
            raise KeyError('invalid tid (not a string): ' + str(tid))
        group_count = 0
        for grp_index, group in enumerate(self.groups):
            for ingrp_index, test in enumerate(group._sequence):
                if test.tid == tid:
                    return ingrp_index + group_count
            group_count += len(group)
        raise KeyError('TID ' + tid + ' does not exist')

    def append(self, group):
        self.groups.append(group)

    def parse_file_name(self, csv_file):
        fname = os.path.split(csv_file)[1]
        title = os.path.splitext(fname)[0]
        parts = title.split('__')
        if len(parts)!=2:
            raise JSONRPCInvalidParamsError('the sequence file name should be of the format [name]__[version].csv, are you using a single underscore as the separator between name and version?')
        return parts[0], parts[1]

    def load_csv_sequence(self, csv_file):
        try:
            self.name, self.version = self.parse_file_name(csv_file)
            f = open(csv_file, 'rU')
            reader = csv.DictReader(f)
            last_group_name = ''
            for row in reader:
                group_name = row['GROUP']
                if group_name != last_group_name:
                    group = TestGroup(group_name)
                    self.groups.append(group)
                    last_group_name = group_name
                else:
                    group = self.groups[-1]
                test = TestItem(row)
                group.append(test)
                if test.returned_val:
                    self.variables[test.returned_val] = None
                if test.p_attribute:
                    self.variables[test.p_attribute] = None
                if len(test.kwargs)>0:
                    for variable_name in test.kwargs.keys():
                        if variable_name not in self.variables:
                            raise KeyError('in test ' + test.tid + ' variable ' + variable_name + ' used before initialized')
            f.close()
        except IOError as e:
            raise JSONRPCInvalidParamsError(str(e))

