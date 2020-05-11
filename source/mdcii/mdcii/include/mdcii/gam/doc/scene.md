# SCENE Readme

Unlike a savegame file (`.gam`) a scenario (`.szs`, `.szm`) file contains several other chunks.
The following chunks are present in a sceneario file:

-   SZENE_PLAYERMIN
-   SZENE_PLAYERMAX
-   SZENE_GAMEID
-   SZENE_RANKING
-   SZENE

The following sections describe the chunk structures that are unique to the scenario file format. All other chunks are documented in the readme for the `.gam` file format.

When the game loads a scenario file it uses the islands defined as `ISLAND5` chunk and the islands that shall be randomly created. `ISLAND5` chunks only appear if the islands are rotated. Island numbers are counted sequenially in a mixed mode. This means that the Scene section can define e.g. `id 0` and `id 2` where an `ISLAND5` chunk defines `id 1`.
The final result after loading the scene file is a savegame file (`.gam`). 

## SZENE_PLAYERMIN

## SZENE_PLAYERMAX

## SZENE_GAMEID

## SZENE_RANKING

### Structs

| SceneRanking | Type  | Size | Description                                 |
| ------------ | ----- | ---- | ------------------------------------------- |
| ranking      | int32 | 4    | The ranking of the missions (stars): 0 to 3 |

## SZENE

### Enums

| ClimateType | Type  | Values    |
| ----------- | ----- | --------- |
| ClimateType | uint8 | North : 0 |
|             |       | South: 1  |
|             |       | Random: 2 |

| SizeType | Type  | Values   |
| -------- | ----- | -------- |
| SizeType | uint8 | Size0: 0 |
|          |       | Size1: 1 |
|          |       | Size2: 2 |
|          |       | Size3: 3 |
|          |       | Size4: 4 |

| NativeFlag | Type  | Values        |
| ---------- | ----- | ------------- |
| NativeFlag | uint8 | NoNatives : 0 |
|            |       | Natives: 1    |

### Structs

| RandomIslands | Type        | Description                                                                                                                                                                              |
| ------------- | ----------- | ---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- |
| climateType   | ClimateType | specifies the climate for "random" island generation                                                                                                                                     |
| sizenr        | SizeType    | specifies the size for "random" island generation                                                                                                                                        |
| nativflg      | NativeFlag  | unused?                                                                                                                                                                                  |
| islandNumber  | uint8_t     | mixed increment with this number and the number field from ISLAND5 islands                                                                                                               |
| filenr        | uint16_t    | 0xFF means this island shall be choosen randomly between all present island files with the given size. Other values say to chose the specific island in combination with the climateType |
| empty         | uint16_t    | empty                                                                                                                                                                                    |
| posx          | uint32_t    | position of the island in x direction                                                                                                                                                    |
| posy          | uint32_t    | position of the island in y direction                                                                                                                                                    |
