# -*- coding: utf-8 -*-
#!/bin/env python

import sys

f = open(sys.argv[1])

lines = [ l.strip() for l in f.readlines() ]

# for line in lines:
#     if "Bin#" in line:
#         line = "".join(["	"] + line.split()[3:])
#         line = line.replace("|", "			")
#         print line
#     if "Bin" in line:
#         line = "".join(["	"] + line.split()[3:])
#         line = line.replace("|", "	")
#         line = line.replace(u"\u00B1".encode("utf-8"), "	" + u"\u00B1".encode("utf-8") + "	")
#         print line

# delim=","
delim=","

for line in lines:
    if "Bin#" in line:
        line = "".join(["	"]  + ["binname"+delim] + line.split()[3:])
        line = line.replace("|", "{}{}{}".format(delim, delim, delim))
        print line
    elif "|" in line:
        if len(line.split()) > 1:
            title = line.split()[1] + delim
        else:
            title = delim
        line = "".join(["	"] + [title] + line.split()[3:])
        line = line.replace("|", delim)
        line = line.replace(u"\u00B1".encode("utf-8"), delim + u"\u00B1".encode("utf-8") + delim)
        print line[:-1]
