Aufbau einer .gad-Datei
=======================

```c
typedef gadget_t = struct {
  int id;
  int blocknr;
  int gfxnr;
  int kind;
  int slidverflg;
  int noselflg;
  point pos;
  point size;
  point color;
  point posoffs;
  int pressoff;
  int flipflg;
};
```

- nummer: Pointer
- objfill: Objekt(e), mit denen das aktuelle Objekt gefüllt wird

- vorangestelltes @: Wert kann relativ sein
