Grafikreihenfolge
=================

Es folgt eine Visualisierung der Reihenfolge der Grafiken für Gebäude mit einer Größe von mehr als einem Feld.
Zusätzlich sind die jeweiligen Koordinaten der einzelnen Tiles relativ zum Start (Feld Nr. 1) dargestellt.

## 2x2

### Rotation 0

```
1 2
3 4
        0     0
    1      1       1
 y     3        2      x
           4
```

### Rotation 1

```
3 1
4 2

        0     -1
    1      3       0
 y     4        1      x
           2
```

### Rotation 2

```
4 3
2 1

       -1     -1
    0      4       0
 y     2        3      x
           1
```

### Rotation 3

```
2 4
1 3

       -1     0
    0      2       1
 y     1        4      x
           3
```





## 3x3

### Rotation 0
```
1 2 3
4 5 6
7 8 9

           0     0
       1      1      1
   2      4       2     2
y     7       5      3      x
          8       6
              9
```

### Rotation 1
```
7 4 1
8 5 2
9 6 3

           0     -2
       1      7      -1
   2      8       4      0
y     9       5       1      x
          6       2
              3
```

### Rotation 2
```
9 8 7
6 5 4
3 2 1

          -2     -2
      -1      9      -1
   0      6       8      0
y     3       5       7      x
          2       4
              1
```

### Rotation 3
```
3 6 9
2 5 8
1 4 7

          -2     0
      -1      3      1
   0      2       6      2
y     1       5       9      x
          4       8
              7
```

## 5x7

### Rotation 0
```
 1  2  3  4  5  6  7
 8  9 10 11 12 13 14
15 16 17 18 19 20 21
22 23 24 25 26 27 28
29 30 31 32 33 34 35

                             0       0
                         1       1       1
                     2       8       2       2
                 3       15      9       3        3
             4       22      16      10      4        4
         y      29      23      17      11      5         5
                     30      24      18       12      6        6
                         31      25      19        13      7       x
                             32      26       20       14
                                 33      27        21
                                     34       28
                                         35
```

### Rotation 1
```
29 22 15  8  1
30 23 16  9  2
31 24 17 10  3
32 25 18 11  4
33 26 19 12  5
34 27 20 13  6
35 28 21 14  7

                              0     -4
                          1     29      -3
                      2      30     22      -2
                  3      31     23      15      -1
              4      32      24      16     8        0
          5      33      25      17      9      1        x
      6      34      26      18      10      2
  y      35      27      19      11       3
             28      20      12       4
                 21      13       5
                     14       6
                          7
```
