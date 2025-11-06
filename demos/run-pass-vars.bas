5  COMMON X
10 LET X = 99
20 LET Y = 42
30 PRINT "[run-pass-vars] initial state: X= ", X, " Y= ", Y
40 CHAIN "use-passed-params.bas"
50 REM CHAIN does not return to this program
