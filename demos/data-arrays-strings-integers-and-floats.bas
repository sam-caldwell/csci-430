10 REM data-arrays-strings-integers-and-floats.bas
20 REM (c) 2025 Sam Caldwell. All Rights Reserved.
30 REM Demonstrate DATA/READ, arrays, strings, integer, and single types
40 REM Avoid array element expressions and FOR-body READs per grammar limits.
50 
60 DEFDBL D: DEFINT I: DEFSNG S: DEFSTR T
70 DEFINT N: DEFSNG R: DEFSTR Z
80 
90 REM Dimension arrays for integers and singles
100 DIM N(4)
110 DIM R(4)
120 
130 LET NSum = 0
140 LET SSum = 0
150 REM Read integers into scalars, then store to array
160 READ N0,N1,N2,N3,N4
170 LET N(0) = N0
180 LET N(1) = N1
190 LET N(2) = N2
200 LET N(3) = N3
210 LET N(4) = N4
220 REM Read singles into scalars, then store to array
230 READ Y0,Y1,Y2,Y3,Y4
240 LET R(0) = Y0
250 LET R(1) = Y1
260 LET R(2) = Y2
270 LET R(3) = Y3
280 LET R(4) = Y4
290 REM Read strings into scalars and build a combined string
300 READ Z0,Z1,Z2
310 LET Tall = Z0 + "," + Z1 + "," + Z2
320 
330 REM Print integer and single-precision sums (scalar sums only)
340 LET NSum = N0 + N1 + N2 + N3 + N4
350 PRINT NSum
360 LET SSum = Y0 + Y1 + Y2 + Y3 + Y4
370 PRINT SSum
380 
390 REM Show strings read via DATA using string variables
400 PRINT Tall
410 
420 REM Show integer vs. single behavior via DEFINT/DEFSNG
430 LET Ival = 5 / 2
440 LET Sval = 5 / 2
450 PRINT "Sval (single, 5/2) =", Sval
460 PRINT "Ival (integer, 5/2) =", Ival
470 
480 REM Confirm DEF* default types: S* single, D* double, T*/Z* string
490 LET Sone = 1 / 3
500 LET Done = 1 / 3
510 LET Diff = ABS(Done - Sone)
520 PRINT "DEFDBL vs DEFSNG delta:", Diff
530 LET Tname = "TypeOK"
540 PRINT "DEFSTR example (Tname):", Tname
550 
560 END
570 
900 DATA 1,2,3,4,5
910 DATA 1.1,2.2,3.3,4.4,5.5
920 DATA "ALPHA","BETA","GAMMA"
