10 REM alg-simplify.bas
20 REM (c) 2025 Sam Caldwell. All Rights Reserved.
30 REM Exercise constant folding and algebraic identities in expressions
40 LET X = 7
50 LET Y = 3

60 ' constant folding (nested)
70 PRINT "E1 ", ((2+3)*(4-1)) + ((6/2)*(8+0))

80 ' identity: + 0 (both sides)
90 PRINT "E2 ", X + 0
100 PRINT "E3 ", 0 + X

110 ' identity: - 0
120 PRINT "E4 ", Y - 0

130 ' identity: * 1 (both sides)
140 PRINT "E5 ", X * 1
150 PRINT "E6 ", 1 * X

160 ' identity: / 1
170 PRINT "E7 ", (X + Y) / 1

180 ' annihilator: * 0 (both sides, nested)
190 PRINT "E8 ", 0 * (X + 10)
200 PRINT "E9 ", (X + 10) * 0

210 ' nested zero inside addition
220 PRINT "E10 ", 3 + 0 * (X + 5)

230 ' unary plus elimination (on constants)
240 PRINT "E11 ", +(+5)

250 ' unary minus on folded constant
260 PRINT "E12 ", -(2+3)

270 ' double negation after inner fold
280 PRINT "E13 ", -(-(2+3))

290 ' constant folding of arithmetic chain
300 PRINT "E14 ", ((1+2)+(3+4))*(5-2)

310 ' combine multiple identities
320 PRINT "E15 ", (((1*(X+Y)) + 0) * 1)

330 ' zero from sub-expression annuls product, add zero falls out
340 PRINT "E16 ", ((1-1)*X) + 42

350 ' division by one with trailing minus zero
360 PRINT "E17 ", (100/1) - 0

370 ' deep zero multiply with trailing addition
380 PRINT "E18 ", (2*0) + 99

390 ' parentheses-heavy constant expression
400 PRINT "E19 ", (((((3+(7-7)) * 5))))

410 ' comparisons that fold to 1 or 0
420 PRINT "C1 ", (2+2) = (6-2)
430 PRINT "C2 ", (3*4) <> (10+1)
440 PRINT "C3 ", (10/5) > (2*1)
450 PRINT "C4 ", (1+1) <= (2)
460 PRINT "C5 ", (5-3) >= (10/5)

470 ' multiple zero products summed, then plus constant
480 PRINT "E20 ", ((X * 0) + (0 * Y)) + 5

490 ' grouped identities around division by one
500 PRINT "E21 ", (((8+2) * (3+3)) / 1)

510 ' unary plus in larger expression with *1
520 PRINT "E22 ", +( ((+2) + (+(+3))) * 1 )

530 ' combine -0, +0 and *1 in nested form
540 PRINT "E23 ", ( ((X-0) + (0+Y)) * (1) )

550 ' deep nest of 0 and 1 identities
560 PRINT "E24 ", ( (0 + (1 * ( ( (0*X) + (Y*1) ) ))) )

570 ' long composite reduces to constant
580 PRINT "E25 ", ( ((2+3) * (4-1) + 0) - ( (6/3) * 1 ) )

590 END

