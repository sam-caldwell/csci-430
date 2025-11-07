0 '==============================================================
1 ' Four-Species Life Variant in GW-BASIC (Text/Color Mode)
2 ' Grid shows A/B/C/D; color = weight (0..15). 0 weight = removed.
3 ' Rules implemented:
4 '  • Eating (directional pairs): A eats C, B eats D, D eats B, C eats A.
5 '    On eat: eater +1 weight (cap 15), hunger reset. Eaten is removed.
6 '  • Spawning (pair adjacency): A next to B spawns random A/B; C next to D spawns random C/D.
7 '    Child weight = 8. Parents that spawn do NOT move that cycle.
8 '  • Starvation: if an automaton fails to eat for 2 consecutive cycles,
9 '    weight -= 1; if weight <= 0, it is deleted; starvation counter resets.
10 '  • Movement: each automaton that did NOT spawn may move one step
11 '    into a random empty neighboring cell (8-neighborhood).
12 ' Display: SCREEN 0, 80x25 text; grid area = GW x GH.
13 '==============================================================
15 PRINT "starting..."
20 RANDOMIZE
30 GW = 60: GH = 22        ' grid width/height (leave one line for status)
40 DEFINT A-Z
45 OPTION BASE 1

50 ' Species encoding: 0=empty, 1=A, 2=B, 3=C, 4=D
60 DIM S(60, 22)      ' species
70 DIM W(60, 22)      ' weight (0..15)
80 DIM HN(60, 22)     ' starvation half-cycles since last eat (0..2)
90 DIM DEAD(60, 22)   ' marks for removals within cycle
100 DIM SPAWNED(60, 22) ' mark parents that spawned this cycle
110 DIM ATE(60, 22)     ' mark who ate this cycle
120 DIM NS(60, 22): DIM NW(60, 22): DIM NH(60, 22) ' next-state buffers

130 '----- Initialization: random seeding
140 FOR Y = 1 TO GH
150   FOR X = 1 TO GW
160     R = INT(RND * 100)
170     IF R < 12 THEN
180       S(X, Y) = 1 + INT(RND * 4) ' A..D
190       W(X, Y) = 4 + INT(RND * 7) ' weight 4..10
200       HN(X, Y) = 0
210     ELSE
220       S(X, Y) = 0: W(X, Y) = 0: HN(X, Y) = 0
230     END IF
240   NEXT X
250 NEXT Y

260 COLOR 7: PRINT "T=", T, "  A eats C, B eats D, D eats B, C eats A | AB or CD spawn child (wt=8) | No-eat 2 cycles => wt-1"

270 '==================== Main Loop ==============================
280 T = 0
290 WHILE INKEY$ = "" AND T < 10
300   T = T + 1
310   ' clear per-cycle flags
320   FOR Y = 1 TO GH
330     FOR X = 1 TO GW
340       DEAD(X, Y) = 0: SPAWNED(X, Y) = 0: ATE(X, Y) = 0
350     NEXT X
360   NEXT Y

370   '---------- Phase 1: Eating ----------
380   FOR Y = 1 TO GH
390     FOR X = 1 TO GW
400       IF S(X, Y) <> 0 AND DEAD(X, Y) = 0 THEN
410         ES = S(X, Y)
420         ' pick one edible neighbor at random among those allowed
430         CX = 0: CY = 0: FOUND = 0
440         TRY = 0
450         ' attempt up to 8 times to find a random neighbor to eat
460         WHILE TRY < 8 AND FOUND = 0
470           DX = INT(RND * 3) - 1: DY = INT(RND * 3) - 1
480           IF DX <> 0 OR DY <> 0 THEN
490             NX = X + DX: NY = Y + DY
500             IF NX >= 1 AND NX <= GW AND NY >= 1 AND NY <= GH THEN
510               IF DEAD(NX, NY) = 0 THEN
520                 PS = S(NX, NY)
530                 IF PS <> 0 THEN
540                   ' Eating rules: A->C, B->D, D->B, C->A
550                   EATS = 0
560                   IF ES = 1 AND PS = 3 THEN EATS = 1
570                   IF ES = 2 AND PS = 4 THEN EATS = 1
580                   IF ES = 4 AND PS = 2 THEN EATS = 1
590                   IF ES = 3 AND PS = 1 THEN EATS = 1
600                   IF EATS = 1 THEN
610                     FOUND = 1: CX = NX: CY = NY
620                   END IF
630                 END IF
640               END IF
650             END IF
660           END IF
670           TRY = TRY + 1
680         WEND
690         IF FOUND = 1 THEN
700           ' perform eat
710           DEAD(CX, CY) = 1: S(CX, CY) = 0: W(CX, CY) = 0: HN(CX, CY) = 0
720           W(X, Y) = W(X, Y) + 1: IF W(X, Y) > 15 THEN W(X, Y) = 15
730           HN(X, Y) = 0: ATE(X, Y) = 1
740         END IF
750       END IF
760     NEXT X
770   NEXT Y

780   '---------- Phase 2: Spawning (AB or CD adjacency) ----------
790   FOR Y = 1 TO GH
800     FOR X = 1 TO GW
810       IF S(X, Y) <> 0 AND DEAD(X, Y) = 0 THEN
820         ES = S(X, Y)
830         ' find an eligible partner (AB) or (CD)
840         PART = 0: PX = 0: PY = 0
850         FOR DY = -1 TO 1
860           FOR DX = -1 TO 1
870             IF DX <> 0 OR DY <> 0 THEN
880               NX = X + DX: NY = Y + DY
890               IF NX >= 1 AND NX <= GW AND NY >= 1 AND NY <= GH THEN
900                 IF DEAD(NX, NY) = 0 THEN
910                   PS = S(NX, NY)
920                   IF (ES = 1 AND PS = 2) OR (ES = 2 AND PS = 1) THEN PART = 12
930                   IF (ES = 3 AND PS = 4) OR (ES = 4 AND PS = 3) THEN PART = 34
940                   IF PART <> 0 THEN PX = NX: PY = NY
950                 END IF
960               END IF
970             END IF
980           NEXT DX
990         NEXT DY
1000        ' if a pair found, try to place child into a random empty neighbor of either parent
1010        IF PART <> 0 THEN
1020          SPAWN_SPEC = 0
1030          IF PART = 12 THEN
1040            IF RND < 0.5 THEN SPAWN_SPEC = 1 ELSE SPAWN_SPEC = 2
1050          ELSE
1060            IF RND < 0.5 THEN SPAWN_SPEC = 3 ELSE SPAWN_SPEC = 4
1070          END IF
1080          ' choose which parent's neighborhood to attempt first
1090          PSEL = 0: IF RND < 0.5 THEN PSEL = 1 ELSE PSEL = 2
1100          IF PSEL = 1 THEN SX1 = X: SY1 = Y: SX2 = PX: SY2 = PY ELSE SX1 = PX: SY1 = PY: SX2 = X: SY2 = Y
1110          ' attempt around primary parent
1120          PLACED = 0
1130          FOR ATT = 1 TO 16
1140            DX = INT(RND * 3) - 1: DY = INT(RND * 3) - 1
1150            IF DX <> 0 OR DY <> 0 THEN
1160              NX = SX1 + DX: NY = SY1 + DY
1170              IF NX >= 1 AND NX <= GW AND NY >= 1 AND NY <= GH THEN
1180                IF DEAD(NX, NY) = 0 AND S(NX, NY) = 0 THEN
1190                  S(NX, NY) = SPAWN_SPEC: W(NX, NY) = 8: HN(NX, NY) = 0
1200                  PLACED = 1: ATT = 16
1210                END IF
1220              END IF
1230            END IF
1240          NEXT ATT
1250          ' if not placed, attempt around the other parent
1260          IF PLACED = 0 THEN
1270            FOR ATT = 1 TO 16
1280              DX = INT(RND * 3) - 1: DY = INT(RND * 3) - 1
1290              IF DX <> 0 OR DY <> 0 THEN
1300                NX = SX2 + DX: NY = SY2 + DY
1310                IF NX >= 1 AND NX <= GW AND NY >= 1 AND NY <= GH THEN
1320                  IF DEAD(NX, NY) = 0 AND S(NX, NY) = 0 THEN
1330                    S(NX, NY) = SPAWN_SPEC: W(NX, NY) = 8: HN(NX, NY) = 0
1340                    PLACED = 1: ATT = 16
1350                  END IF
1360                END IF
1370              END IF
1380            NEXT ATT
1390          END IF
1400          ' mark both parents as having spawned (so they won't move this cycle)
1410          SPAWNED(X, Y) = 1: SPAWNED(PX, PY) = 1
1420        END IF
1430      END IF
1440    NEXT X
1450  NEXT Y

1460  '---------- Phase 3: Movement (only those that did NOT spawn) ----------
1470  FOR Y = 1 TO GH
1480    FOR X = 1 TO GW
1490      IF S(X, Y) <> 0 AND DEAD(X, Y) = 0 THEN
1500        IF SPAWNED(X, Y) = 0 THEN
1510          ' attempt to move into a random empty neighbor
1520          MOVED = 0
1530          FOR ATT = 1 TO 8
1540            DX = INT(RND * 3) - 1: DY = INT(RND * 3) - 1
1550            IF DX <> 0 OR DY <> 0 THEN
1560              NX = X + DX: NY = Y + DY
1570              IF NX >= 1 AND NX <= GW AND NY >= 1 AND NY <= GH THEN
1580                IF S(NX, NY) = 0 AND DEAD(NX, NY) = 0 THEN
1590                  ' move
1600                  S(NX, NY) = S(X, Y): W(NX, NY) = W(X, Y): HN(NX, NY) = HN(X, Y)
1610                  S(X, Y) = 0: W(X, Y) = 0: HN(X, Y) = 0
1620                  MOVED = 1: ATT = 8
1630                END IF
1640              END IF
1650            END IF
1660          NEXT ATT
1670        END IF
1680      END IF
1690    NEXT X
1700  NEXT Y

1710  '---------- Phase 4: Apply deaths (already zeroed when eaten) ----------
1720  ' (No extra action needed beyond clearing DEAD marks here)

1730  '---------- Phase 5: Starvation / weight decay ----------
1740  FOR Y = 1 TO GH
1750    FOR X = 1 TO GW
1760      IF S(X, Y) <> 0 THEN
1770        IF ATE(X, Y) = 0 THEN
1780          HN(X, Y) = HN(X, Y) + 1
1790          IF HN(X, Y) >= 2 THEN
1800            W(X, Y) = W(X, Y) - 1
1810            HN(X, Y) = 0
1820            IF W(X, Y) <= 0 THEN
1830              S(X, Y) = 0: W(X, Y) = 0: HN(X, Y) = 0
1840            END IF
1850          END IF
1860        ELSE
1870          ' already reset when eating
1880        END IF
1890      END IF
1900    NEXT X
1910  NEXT Y

1920  '---------- Draw ----------
1930  COLOR 7: PRINT "T=", T, "  A eats C, B eats D, D eats B, C eats A | AB or CD spawn child (wt=8) | No-eat 2 cycles => wt-1"

1940  ' small delay for readability
1950  FOR D = 1 TO 200: NEXT D
1960 WEND

1990 END
