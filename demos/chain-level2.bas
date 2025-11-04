10 PRINT "chain-level2.bas starting"
20 PRINT "initial state: X:",X,",Y:",Y,",Z:",Z
30 Y=Y-1
40 PRINT "modified state: X:",X,",Y:",Y,",Z:",Z
50 COMMON X,Y
60 CHAIN "chain-level3.bas"
70 PRINT" chain-level2.bas resumed"
80 CHAIN "chain-test.bas", 60