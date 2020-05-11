# Bitflags

## Bits in the 'flags' attribute of the 'settler' structure

Still to be classified:

-   Health/disease
-   Food
-   Alcohol
-   Tobacco products
-   Spices
-   Clothing
-   Cocoa
-   Jewellery

Unknown bits used are: 6, 8, 9, 11, 16, 17, 19

| bit | meaning          |        |     |
| --: | :--------------- | ------ | --- |
|   0 |                  |        |     |
|   1 |                  |        |     |
|   2 |                  |        |     |
|   3 |                  |        |     |
|   4 |                  |        |     |
|   5 |                  |        |     |
|   6 |                  |        |     |
|   7 | Cloth            |        |     |
|   8 |                  |        |     |
|   9 |                  |        |     |
|  10 |                  |        |     |
|  11 |                  |        |     |
|  12 |                  |        |     |
|  13 |                  |        |     |
|  14 |                  |        |     |
|  15 |                  |        |     |
|  16 |                  |        |     |
|  17 |                  |        |     |
|  18 |                  |        |     |
|  19 |                  |        |     |
|  20 |                  |        |     |
|  21 | Satisfied        |        |     |
|  22 | Market           |        |     |
|  23 | Church/Cathedral |        |     |
|  24 |                  | tavern |     |
|  25 | bathhouse        |        |     |
|  26 | theatre          |        |     |
|  27 |                  |        |     |
|  28 | school           |        |     |
|  29 | college          |        |     |
|  30 | chapel           |        |     |
|  31 |                  | doctor |     |

## Bits in the 'mode' attribute of the 'Prodlist' structure

-   bit 0 = active
-   bit 6 = do not pick up

## Bits in the 'fertility' attribute of the 'island5' structure

The bits indicate what is growing at 100% instead of 50%.

| bit | meaning         |
| --: | :-------------- |
|   0 | (always 1)      |
|   1 | 100% tobacco    |
|   2 | 100% spices     |
|   3 | 100% sugar cane |
|   4 | 100% cotton     |
|   5 | 100% wine       |
|   6 | 100% cocoa      |
|   7 | (always 1)      |

## Bits in the 'enabled' attribute of the 'player' structure

A set bit means that the corresponding buildings can be constructed.

|        bit | condition¹       | meaning                                                                                                                                                  |           |
| ---------: | :--------------- | :------------------------------------------------------------------------------------------------------------------------------------------------------- | --------- |
|          0 | (ignores)        |                                                                                                                                                          |           |
|          1 | (ignores)        |                                                                                                                                                          |           |
|          2 | 100 settlers     | school                                                                                                                                                   |           |
|          3 | 50 settlers      | tavern                                                                                                                                                   |           |
|          4 | 150 citizens     | church                                                                                                                                                   |           |
|          5 | 210 citizens     | bathhouse                                                                                                                                                |           |
|          6 | 300 merchants    | theatre                                                                                                                                                  |           |
|          7 | 250 merchants    |                                                                                                                                                          |           |
| university |                  |                                                                                                                                                          |           |
|          8 | 50 citizens      | doctor                                                                                                                                                   |           |
|          9 | 100 citizens     | gallows                                                                                                                                                  |           |
|         10 | 1500 aristocrats | castle                                                                                                                                                   |           |
|         11 | 2500 aristocrats | cathedral                                                                                                                                                |           |
|            | 12               |                                                                                                                                                          | (nothing) |
|            | 13               |                                                                                                                                                          | (nothing) |
|         14 | 30 pioneers      | butchery, cattle farm                                                                                                                                    |           |
|         15 | 15 settlers      | cobblestone road, stone bridge, quarry, stonemason, fire brigade                                                                                         |           |
|         16 | 30 Settlers      | Palisade, Wooden Gate, Warehouse II                                                                                                                      |           |
|         17 | 40 settlers      | tobacco shop, rum distillery, vineyard, vines, sugar cane plantation, sugar cane field, tobacco plantation, tobacco field, spice plantation, spice field |           |
|         18 | 75 settlers      | windmill, bakery, grain farm, cornfield                                                                                                                  |           |
|         19 | 100 settlers     | tool smithy                                                                                                                                              |           |
|         20 | 120 settlers     | ore mine, shipyard, ore smelting                                                                                                                         |           |
|         21 | 200 settlers     | small castle, stone wall, stone gate, watchtower, city gate, square I, swordsmiths                                                                       |           |
|         22 | 100 citizens     | Kontor III                                                                                                                                               |           |
|         23 | 150 citizens     | gold mine                                                                                                                                                |           |
|         24 | 200 citizens     | tailoring, weaving mill, cotton plantation, cotton field, cocoa plantation, cocoa field                                                                  |           |
|         25 |                  | (nothing)a                                                                                                                                               |           |
|         26 | 400 citizens     | cannon foundry                                                                                                                                           |           |
|         27 | 450 citizens     | deep ore mine                                                                                                                                            |           |
|         28 | 250 merchants    | Place II, Place III, ornamental tree, goldsmiths, office IV                                                                                              |           |
|         29 | 400 merchants    | great castle, musket maker                                                                                                                               |           |
|         30 | 500 merchants    | large shipyard                                                                                                                                           |           |
|         31 | 600 aristocrats  | fortress                                                                                                                                                 |           |

1) Source: [AnnoWiki 1602](http://1602.annowiki.de/). The condition must have been fulfilled only once.
