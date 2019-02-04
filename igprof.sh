#!/bin/bash

igprof -pp -d -z -o igprof.pp.gz ${1}
igprof-analyse --sqlite -d -v -g igprof.pp.gz | sqlite3 igprof.pp.sql3 >& /dev/null
cp igprof.pp.sql3 ~/public_html/cgi-bin/data/
echo "http://${HOSTNAME}/~${USER}/cgi-bin/igprof-navigator.py/igprof.pp/"
