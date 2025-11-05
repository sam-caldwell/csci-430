10 PRINT "chain-test.bas starting"
20 LET X=42
30 LET Y=1337
40 LET Z=-1
50 COMMON X,Y,Z
60 CHAIN "chain-level2.bas"
70 PRINT "chain-test.bas resumed"
80 END
