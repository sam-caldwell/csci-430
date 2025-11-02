10 REM fibonacci.bas
20 REM (c) 2025 Sam Caldwell. All Rights Reserved.
30 REM Print Fibonacci numbers up to 100 using a multi-line WHILE...WEND loop.
40 LET A = 1
50 LET B = 1
60 WHILE A <= 100
70 PRINT A
80 LET C = A + B
90 LET A = B
100 LET B = C
110 WEND
120 END
