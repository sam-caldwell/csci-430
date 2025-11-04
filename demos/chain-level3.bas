10 PRINT "chain-level3.bas starting"
20 PRINT "initial state: X:",X,",Y:",Y,",Z:",Z
30 COMMON X,Y,Z: REM Z should pass to chain-test4 as 0
40 CHAIN "chain-level4.bas"
50 PRINT" chain-level3-bas resumed"
60 CHAIN "chain-level2.bas",70