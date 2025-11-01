10 REM trig.bas
20 REM (c) 2025 Sam Caldwell. All Rights Reserved.
30 REM Iterate degrees 0..360 and print X, SIN, COS, TAN.
40 LET P = ATN(1) * 4
45 PRINT "X    SIN(X)    COS(X)    TAN(X)"
50 FOR X = 0 TO 360
55   LET R = X * P / 180 : PRINT X, SIN(R), COS(R), TAN(R)
60 NEXT X
70 END
