#!/usr/bin/python3
import re
import sys  

# Input is file that is generated from output of cod_parser_test
filepath = sys.argv[1]
match = sys.argv[2]

with open(filepath) as fp:
    line = fp.readline()
    name = ""
    dictlist = [dict() for x in range(3)]
    dictindex = 0
    set = False
    haus_found = False
    while line:
        if("HAUS" in line):
            haus_found = True
        if(haus_found == True):
            if("{" not in line and "}" not in line):
                m = re.search('name:\s*\"(\w+)+\"', line)
                if(m is not None):
                    name=m.group(1)
                    if(name.isdigit() == True):
                        dictindex = 0
                    if("HAUS_PRODTYP" in line):
                        dictindex = 1
                    if("HAUS_BAUKOST" in line):
                        dictindex = 2
                
                if set == False:
                    m = re.search('\w+:\s*\"(\w+)+\"', line)
                    if(m != None and m.group(1) == match):
                        set = True
                        line = fp.readline()
                        continue

                if set == True:
                    m = re.search('\w+:\s*\"(\w+)+\"', line)
                    if(m != None):
                        dictlist[dictindex][m.group(1)] = 0
                    set = False
                    continue
        line = fp.readline()
        
    print("\nObject:")
    for key, value in dictlist[0].iteritems():
        print(key)
    print("\nHAUS_PRODTYP:")
    for key, value in dictlist[1].iteritems():
        print(key)
    print("\nHAUS_BAUKOST:")
    for key, value in dictlist[2].iteritems():
        print(key)
    