10 REM trig.bas
20 REM (c) 2025 Sam Caldwell. All Rights Reserved.
30 REM Iterate degrees 0..360 and print SIN, COS, TAN of each.
40 LET P = ATN(1) * 4
50 FOR X = 0 TO 360 : LET R = X * P / 180 : PRINT SIN(R) : PRINT COS(R) : PRINT TAN(R) : NEXT X
60 END
