#!/bin/bash
g++ -Wall -O2 -fPIC -ggdb3 -I/data/work_svn/study/include -I/usr/include/mysql -o codestat main.cpp -L../../lib -lBaseCode -lNetWork -lmysqlclient -lhiredis -lDatabase -lssl -lcrypto
