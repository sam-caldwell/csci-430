10 REM trig-multi-line-loop.bas
20 REM (c) 2025 Sam Caldwell. All Rights Reserved.
30 REM Iterate degrees 0..360 and print X, SIN, COS, TAN using a multi-line FOR loop.
40 LET P = ATN(1) * 4
45 PRINT "X    SIN(X)    COS(X)    TAN(X)"
50 FOR X = 0 TO 360
60 LET R = X * P / 180
70 PRINT X, SIN(R), COS(R), TAN(R)
80 NEXT X
90 END
