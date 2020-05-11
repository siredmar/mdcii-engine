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

The main structure is `SceneRanking`.

| SceneRanking | Type  | Description                                 |
| ------------ | ----- | ------------------------------------------- |
| ranking      | int32 | The ranking of the missions (stars): 0 to 3 |

## SZENE

The main structure is `SceneSaveData` and contains all other structures and enums listed below.

### Enums

| ClimateType | Type  | Enum   | Value |
| ----------- | ----- | ------ | ----- |
|             | uint8 | North  | 0     |
|             |       | South  | 1     |
|             |       | Random | 2     |

| SizeType | Type  | Enum  | Value |
| -------- | ----- | ----- | ----- |
|          | uint8 | Size0 | 0     |
|          |       | Size1 | 1     |
|          |       | Size2 | 2     |
|          |       | Size3 | 3     |
|          |       | Size4 | 4     |

| NativeFlag | Type  | Enum      | Value |
| ---------- | ----- | --------- | ----- |
|            | uint8 | NoNatives | 0     |
|            |       | Natives   | 1     |

| GoodsHouseId | Type   | Enum             | Value |
| ------------ | ------ | ---------------- | ----- |
|              | uint16 | None             | 0     |
|              |        | Treasure         | 533   |
|              |        | OreMine          | 2401  |
|              |        | GoldMine         | 2405  |
|              |        | TabacoPlantation | 1336  |
|              |        | WinePlantation   | 1344  |
|              |        | SugarPlantation  | 1340  |
|              |        | CacaoPlantation  | 1338  |
|              |        | WoolPlantation   | 1332  |
|              |        | SpicesPlantation | 1342  |

### Structs

| RandomGood | Type         | Description                               |
| ---------- | ------------ | ----------------------------------------- |
| houseId    | GoodsHouseId | specifies the houseId that can build this |
| amount     | uint16       | specifies how many resources there are    |
| -          | union uint32 | unused? kind, price, percent              |

| RandomOre    | Type       | Description                          |
| ------------ | ---------- | ------------------------------------ |
| smallOreMine | RandomGood | specifies the random small ore mines |
| greatOreMine | RandomGood | specifies the random great ore mines |
| goldMine     | RandomGood | specifies the random gold mines      |
| empty        | RandomGood | unused                               |

| RandomRawMaterials | Type          | Description                                                    |
| ------------------ | ------------- | -------------------------------------------------------------- |
| tabaco             | RandomGood    | specifies the distribution for random tabaco 100% growth rates |
| spices             | RandomGood    | specifies the distribution for random spices 100% growth rates |
| sugar              | RandomGood    | specifies the distribution for random sugar 100% growth rates  |
| wool               | RandomGood    | specifies the distribution for random wool 100% growth rates   |
| wine               | RandomGood    | specifies the distribution for random wine 100% growth rates   |
| cacao              | RandomGood    | specifies the distribution for random cacao 100% growth rates  |
| empty              | RandomGood[6] | empty                                                          |

| RandomGoodies | Type          | Description                          |
| ------------- | ------------- | ------------------------------------ |
| treasure      | RandomGood    | specifies amount of random treasures |
| empty         | RandomGood[3] | empty                                |

| RandomHardware | Type          | Description |
| -------------- | ------------- | ----------- |
| empty          | RandomGood[8] | unused?     |

| RandomNativeVillages | Type      | Description                                |
| -------------------- | --------- | ------------------------------------------ |
| strawRoofCount       | uint32    | specifies the count of straw roof villages |
| incasCount           | uint32    | specifies the count of incas villages      |
| empty                | uint32[3] | empty                                      |

| Position | Type   | Description                   |
| -------- | ------ | ----------------------------- |
| x        | uint32 | X position of island in world |
| y        | uint32 | Y position of island in world |

| RandomIslands | Type        | Description                                                                                                                                                                              |
| ------------- | ----------- | ---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- |
| climateType   | ClimateType | specifies the climate for "random" island generation                                                                                                                                     |
| sizenr        | SizeType    | specifies the size for "random" island generation                                                                                                                                        |
| nativflg      | NativeFlag  | unused?                                                                                                                                                                                  |
| islandNumber  | uint8       | mixed increment with this number and the number field from ISLAND5 islands                                                                                                               |
| filenr        | uint16      | 0xFF means this island shall be choosen randomly between all present island files with the given size. Other values say to chose the specific island in combination with the climateType |
| empty         | uint16      | empty                                                                                                                                                                                    |
| pos           | Position    | position of the island in the world                                                                                                                                                      |

| SceneSaveData  | Type                 | Description                                                 |
| -------------- | -------------------- | ----------------------------------------------------------- |
| name           | char\[]              | the name of the scenario                                    |
| nativeVillages | RandomNativeVillages | amount of random native villages                            |
| empty1         | int32                | empty                                                       |
| rohstmax       | int32                | amount of 100% growing raw materials for islands? Always 2? |
| islandsCount   | int32                | overall count of islands                                    |
| goldminsizenr  | int32                | unused? always 1?                                           |
| goldmaxsizenr  | int32                | unused? always 2?                                           |
| empty2         | int32                | empty                                                       |
| empty3         | RandomGood\[]        | empty                                                       |
| ores           | RandomOre            | random ores                                                 |
| rawMaterials   | RandomRawMaterials   | random raw materials                                        |
| goodies        | RandomGoodies        | random goodies                                              |
| hardware       | RandomHardware       | random hardware, unused?                                    |
| islands        | RandomIsland\[]      | random islands definitions                                  |
