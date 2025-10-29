10 REM factorial.bas
20 REM (c) 2025 Sam Caldwell. All Rights Reserved.
30 REM This program calculates the factorial of a given number from input and prints it to stdout.
40 INPUT N
50 LET F = 1
60 FOR I = 1 TO N : LET F = F * I : NEXT I
70 PRINT F
80 END
