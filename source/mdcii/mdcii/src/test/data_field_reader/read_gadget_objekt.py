#!/usr/bin/python3
import re
import sys

# Input is file that is generated from output of cod_parser_test
filepath = sys.argv[1]
with open(filepath) as fp:
    line = fp.readline()
    name = ""
    dictlist = [dict() for x in range(3)]
    dictindex = 0
    haus_found = False

    while line:
        if("GADGET" in line):
            haus_found = True
        if(haus_found == True):
            if("{" not in line and "}" not in line and "value" not in line):
                set = True
                m = re.search('name:\s*\"(\w+)+\"', line)
                
                if(m != None):
                    name=m.group(1)

                if(name.isdigit() == True):
                    dictindex = 0
                    set = False
                    
                if(set == True):
                    dictlist[dictindex][name] = 0

        line = fp.readline()

    print("\nObject:")
    for key, value in dictlist[0].items():
        print(key)

