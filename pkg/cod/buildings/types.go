package buildings

// Enum values for Kind
type Kind string

const (
	KindUnset          Kind = "Unset"
	KindForrest        Kind = "Forrest"        // Wald
	KindGate           Kind = "Gate"           // Tor
	KindRuin           Kind = "Ruin"           // Ruine
	KindHeadquarters   Kind = "Headquarters"   // Hauptquartier
	KindBeachMouth     Kind = "BeachMouth"     // Strandmund
	KindBeachHouse     Kind = "BeachHouse"     // Strandhaus
	KindSurf           Kind = "Surf"           // Brandung
	KindBeachRuin      Kind = "BeachRuin"      // Strandruine
	KindSlopeCorner    Kind = "SlopeCorner"    // Hangeck
	KindTower          Kind = "Tower"          // Turm
	KindBeachCornerI   Kind = "BeachCornerI"   // Strandecki
	KindBeachCornerII  Kind = "BeachCornerII"  // Strandvari
	KindBeachCornerIII Kind = "BeachCornerIII" // Strandecka
	KindSea            Kind = "Sea"            // Meer
	KindGround         Kind = "Ground"         // Boden
	KindSlopeSpring    Kind = "SlopeSpring"    // Hangquell
	KindWall           Kind = "Wall"           // Mauer
	KindRock           Kind = "Rock"           // Fels
	KindEstuary        Kind = "Estuary"        // Muendung
	KindSlope          Kind = "Slope"          // Hang
	KindMine           Kind = "Mine"           // Mine
	KindBurnCorner     Kind = "BurnCorner"     // Brandeck
	KindTowerBeach     Kind = "TowerBeach"     // Turmstrand
	KindPier           Kind = "Pier"           // Pier
	KindBeach          Kind = "Beach"          // Strand
	KindBridge         Kind = "Bridge"         // Bruecke
	KindRiverCorner    Kind = "RiverCorner"    // Flusseck
	KindRiver          Kind = "River"          // Fluss
	KindHarbor         Kind = "Harbor"         // Hafen
	KindStreet         Kind = "Street"         // Strasse
	KindBuilding       Kind = "Building"       // Gebaeude
	KindWallBeach      Kind = "WallBeach"      // Mauerstrand
	KindPlaza          Kind = "Plaza"          // Platz
	KindWMill          Kind = "Windmill"       // WMuehle
)

// Enum values for Ruin
type Ruin string

const (
	RuinUnset            Ruin = "Unset "
	RuinWarehouseRuinsN1 Ruin = "WarehouseRuinsN1" // Ruine Kontor N1
	RuinWarehouseRuinsN2 Ruin = "WarehouseRuinsN2" // Ruine Kontor N2
	RuinWoodRuins        Ruin = "WoodRuins"        // Ruine Holz
	RuinWarehouseRuins1  Ruin = "WarehouseRuins1"  // Ruine Kontor 1
	RuinFieldRuins       Ruin = "FieldRuins"       // Ruine Feld
	RuinMarketRuins      Ruin = "MarketRuins"      // Ruine Markt
	RuinStoneRuins       Ruin = "StoneRuins"       // Ruine Stein
	RuinMineRuins        Ruin = "MineRuins"        // Ruine Mine
	RuinRoadStoneRuins   Ruin = "RoadStoneRuins"   // Ruine Road Stein
	RuinRoadFieldRuins   Ruin = "RoadFieldRuins"   // Ruine Road Feld

)

// Enum values for BuildSample
type BuildSample string

const (
	BuildSampleTriumphWave       BuildSample = "TriumphWave"       // Triumphbogen
	BuildSampleSwordConstruction BuildSample = "SwordConstruction" // Schwertbau
	BuildSamplePyramid           BuildSample = "Pyramid"           // Pyramide
	BuildSampleCathedral         BuildSample = "Cathedral"         // Kathedrale
	BuildSampleNix               BuildSample = "Nix"               // Nix
	BuildSampleCastle            BuildSample = "Castle"            // Schloss
	BuildSampleChapel            BuildSample = "Chapel"            // Kapelle
	BuildSampleResidentialHouse  BuildSample = "ResidentialHouse"  // Wohnhaus
	BuildSampleWell              BuildSample = "Well"              // Brunnen
	BuildSampleWarehouse         BuildSample = "Warehouse"         // Kontor
	BuildSampleFoundry           BuildSample = "Foundry"           // Giesserei
	BuildSampleButcher           BuildSample = "Butcher"           // Fleischer
	BuildSampleSchool            BuildSample = "School"            // Schule
	BuildSampleMonument          BuildSample = "Monument"          // Denkmal
	BuildSampleBakery            BuildSample = "Bakery"            // Baecker
	BuildSampleMine              BuildSample = "Mine"              // Mine
	BuildSampleStandard          BuildSample = "Standard"          // Standard
	BuildSampleCastleBurner      BuildSample = "CastleBurner"      // Rumbrenner
	BuildSamplePlantation        BuildSample = "Plantation"        // Plantage
	BuildSampleDoctor            BuildSample = "Doctor"            // Arzt
	BuildSampleMill              BuildSample = "Mill"              // Muehle
	BuildSamplePirates           BuildSample = "Pirates"           // Piraten
	BuildSampleHuntingLodge      BuildSample = "HuntingLodge"      // Jagdhütte
	BuildSampleChurch            BuildSample = "Church"            // Kirche
	BuildSampleMarket            BuildSample = "Market"            // Markt
	BuildSampleBathhouse         BuildSample = "Bathhouse"         // Badehaus
	BuildSampleInn               BuildSample = "Inn"               // Wirtshaus
	BuildSampleTheater           BuildSample = "Theater"           // Theater
)

// Enum values for Material
type Material string

const (
	MaterialUnset Material = "Unset"
	MaterialWood  Material = "Wood"  // Holz
	MaterialCloth Material = "Cloth" // Stoffe
)

// Enum values for OreMountain
type OreMountain string

const (
	OreMountainUnset     OreMountain = "Unset"
	OreMountainSmallMine OreMountain = "SmallMine" // Erzberg Klein
	OreMountainLargeMine OreMountain = "LargeMine" // Erzberg Gross
)

// Enum values for Prodtyp
type Prodtyp string

const (
	ProdtypUnset           Prodtyp = "Unset"
	ProdtypWell            Prodtyp = "Well"          // Brunnen
	ProdtypWarehouse       Prodtyp = "Warehouse"     // Kontor
	ProdtypUniversity      Prodtyp = "University"    // Hochschule
	ProdtypHuntingLodge    Prodtyp = "HuntingLodge"  // Jagdhaus
	ProdtypQuarry          Prodtyp = "Quarry"        // Steinbruch
	ProdtypMarket          Prodtyp = "Market"        // Markt
	ProdtypPlantation      Prodtyp = "Plantation"    // Plantage
	ProdtypMonument        Prodtyp = "Monument"      // Denkmal
	ProdtypFishery         Prodtyp = "Fishery"       // Fischerei
	ProdtypPasture         Prodtyp = "Pasture"       // Weidetier
	ProdtypCastle          Prodtyp = "Castle"        // Schloss
	ProdtypInn             Prodtyp = "Inn"           // Wirt
	ProdtypCraftsmanship   Prodtyp = "Craftsmanship" // Handwerk
	ProdtypWatchtower      Prodtyp = "Watchtower"    // Wachturm
	ProdtypChapel          Prodtyp = "Chapel"        // Kapelle
	ProdtypShipyard        Prodtyp = "Shipyard"      // Werft
	ProdtypTheater         Prodtyp = "Theater"       // Theater
	ProdtypGallows         Prodtyp = "Gallows"       // Galgen
	ProdtypUnused          Prodtyp = "Unused"        // Unused
	ProdtypWall            Prodtyp = "Wall"          // Mauer
	ProdtypResidence       Prodtyp = "Residence"     // Wohnung
	ProdtypTriumph         Prodtyp = "Triumph"       // Triumph
	ProdtypRawMaterial     Prodtyp = "RawMaterial"   // RawMaterial
	ProdtypClinic          Prodtyp = "Clinic"        // Klinik
	ProdtypBathhouse       Prodtyp = "Bathhouse"     // Badehaus
	ProdtypRawGrowth       Prodtyp = "RawGrowth"     // Rohstwachs
	ProdtypSchool          Prodtyp = "School"        // Schule
	ProdtypRawOre          Prodtyp = "RawOre"        // Rohsterz
	ProdtypMine            Prodtyp = "Mine"          // Bergwerk
	ProdtypPirateResidence Prodtyp = "Pirate"        // Piratenwohn
	ProdtypChurch          Prodtyp = "Church"        // Kirche
	ProdtypMilitary        Prodtyp = "Military"      // Militär
)

// Enum values for Ware
type Ware string

const (
	// WareGrain        // Getreide
	WareUnset        Ware = "Unset"
	WareFlour        Ware = "Flour"        // Mehl
	WareGold         Ware = "Gold"         // Gold
	WareCocoa        Ware = "Cocoa"        // Kakao
	WareGrapes       Ware = "Grapes"       // Weintrauben
	WareAll          Ware = "All"          // Allware
	WareTobacco      Ware = "Tobacco"      // Tabak
	WareFish         Ware = "Fish"         // Fische
	WareCloth        Ware = "Cloth"        // Stoffe
	WareAlcohol      Ware = "Alcohol"      // Alkohol
	WareOre          Ware = "Ore"          // Erze
	WareBrick        Ware = "Brick"        // Ziegel
	WareGrass        Ware = "Grass"        // Gras
	WareTobaccoPlant Ware = "TobaccoPlant" // Tabakbaum
	WareIronOre      Ware = "IronOre"      // Eisenerz
	WareNoWare       Ware = "NoWare"       // Noware
	WareJewelry      Ware = "Jewelry"      // Schmuck
	WareClothing     Ware = "Clothing"     // Kleidung
	WareSwords       Ware = "Swords"       // Schwerter
	WareWood         Ware = "Wood"         // Holz
	WareTree         Ware = "Tree"         // Baum
	WareStones       Ware = "Stones"       // Steine
	WareGrain        Ware = "Grain"        // Korn
	WareCocoaPlant   Ware = "CocoaPlant"   // Kakaobaum
	WareIron         Ware = "Iron"         // Eisen
	WareCannons      Ware = "Cannons"      // Kanonen
	WareMeat         Ware = "Meat"         // Fleisch
	WareSpices       Ware = "Spices"       // Gewürze
	WareTobaccoGoods Ware = "TobaccoGoods" // Tabakwaren
	WareSugar        Ware = "Sugar"        // Zucker
	WareFood         Ware = "Food"         // Nahrung
	WareWool         Ware = "Wool"         // Wolle
	WareCotton       Ware = "Cotton"       // Baumwolle
	WareTool         Ware = "Tool"         // Werkzeug
	WareSpicePlant   Ware = "SpicePlant"   // Gewürzbaum
	WareSugarCane    Ware = "SugarCane"    // Zuckerrohr
	WareMusket       Ware = "Musket"       // Musketen
)

// Enum values for RawMaterial
type RawMaterial string

const (
	// RawMaterialGrain                     // Getreide
	RawMaterialUnset        RawMaterial = "Unset"
	RawMaterialGrain        RawMaterial = "Grain"        // Korn
	RawMaterialGold         RawMaterial = "Gold"         // Gold
	RawMaterialGrapes       RawMaterial = "Grapes"       // Weintrauben
	RawMaterialSugar        RawMaterial = "Sugar"        // Zucker
	RawMaterialTobacco      RawMaterial = "Tobacco"      // Tabak
	RawMaterialFish         RawMaterial = "Fish"         // Fische
	RawMaterialCloth        RawMaterial = "Cloth"        // Stoffe
	RawMaterialGrass        RawMaterial = "Grass"        // Gras
	RawMaterialTobaccoPlant RawMaterial = "TobaccoPlant" // Tabakbaum
	RawMaterialFlour        RawMaterial = "Flour"        // Mehl
	RawMaterialIronOre      RawMaterial = "IronOre"      // Eisenerz
	RawMaterialNoWare       RawMaterial = "NoWare"       // Noware
	RawMaterialWood         RawMaterial = "Wood"         // Holz
	RawMaterialTree         RawMaterial = "Tree"         // Baum
	RawMaterialAll          RawMaterial = "All"          // Allware
	RawMaterialCocoaPlant   RawMaterial = "CocoaPlant"   // Kakaobaum
	RawMaterialIron         RawMaterial = "Iron"         // Eisen
	RawMaterialMeat         RawMaterial = "Meat"         // Fleisch
	RawMaterialStones       RawMaterial = "Stones"       // Steine
	RawMaterialWool         RawMaterial = "Wool"         // Wolle
	RawMaterialCotton       RawMaterial = "Cotton"       // Baumwolle
	RawMaterialWild         RawMaterial = "Wild"         // Wild
	RawMaterialSpicePlant   RawMaterial = "SpicePlant"   // Gewürzbaum
	RawMaterialSugarCane    RawMaterial = "SugarCane"    // Zuckerrohr
)

// Enum values for Maxprodcnt
type Maxprodcnt string

const (
	MaxprodcntUnset      Maxprodcnt = "Unset"
	MaxprodcntMaxprodcnt Maxprodcnt = "Maxprodcnt" // Maxprodcnt
)

// Enum values for BuildInfra
type BuildInfra string

const (
	BuildInfraUnset      BuildInfra = "Unset"
	BuildInfraLevel3F    BuildInfra = "Level3F"    // Infra Stufe 3F
	BuildInfraLevel1A    BuildInfra = "Level1A"    // Infra Stufe 1A
	BuildInfraLevel3B    BuildInfra = "Level3B"    // Infra Stufe 3B
	BuildInfraLevel3C    BuildInfra = "Level3C"    // Infra Stufe 3C
	BuildInfraCathedral  BuildInfra = "Cathedral"  // Infra Kathedrale
	BuildInfraTheater    BuildInfra = "Theater"    // Infra Theater
	BuildInfraChurch     BuildInfra = "Church"     // Infra Kirche
	BuildInfraSchool     BuildInfra = "School"     // Infra Schule
	BuildInfraLevel5A    BuildInfra = "Level5A"    // Infra Stufe 5A
	BuildInfraGallows    BuildInfra = "Gallows"    // Infra Galgen
	BuildInfraCastle     BuildInfra = "Castle"     // Infra Schloss
	BuildInfraTriumph    BuildInfra = "Triumph"    // Infra Triumph
	BuildInfraLevel2E    BuildInfra = "Level2E"    // Infra Stufe 2E
	BuildInfraLevel2D    BuildInfra = "Level2D"    // Infra Stufe 2D
	BuildInfraLevel2G    BuildInfra = "Level2G"    // Infra Stufe 2G
	BuildInfraLevel2F    BuildInfra = "Level2F"    // Infra Stufe 2F
	BuildInfraLevel2A    BuildInfra = "Level2A"    // Infra Stufe 2A
	BuildInfraLevel2C    BuildInfra = "Level2C"    // Infra Stufe 2C
	BuildInfraLevel2B    BuildInfra = "Level2B"    // Infra Stufe 2B
	BuildInfraDoctor     BuildInfra = "Doctor"     // Infra Arzt
	BuildInfraLevel4A    BuildInfra = "Level4A"    // Infra Stufe 4A
	BuildInfraTavern     BuildInfra = "Tavern"     // Infra Wirt
	BuildInfraBathhouse  BuildInfra = "Bathhouse"  // Infra Bade
	BuildInfraMonument   BuildInfra = "Monument"   // Infra Denkmal
	BuildInfraUniversity BuildInfra = "University" // Infra Hochschule
)

// Enum values for Figure
type Figure string

const (
	FigureUnset        Figure = "Unset"
	FigureCarrier2     Figure = "Carrier2"     // Träger2
	FigureWoodcutter   Figure = "Woodcutter"   // Holzfäller
	FigureCannonTower  Figure = "CannonTower"  // Kanonturm
	FigureFisher       Figure = "Fisher"       // Fischer
	FigureStoneBreaker Figure = "StoneBreaker" // Steinklopfer
	FigureHarvester    Figure = "Harvester"    // Pflücker
	FigureDoctor       Figure = "Doctor"       // Arzt
	FigureHunter       Figure = "Hunter"       // Jäger
	FigureReaper       Figure = "Reaper"       // Mäher
	FigurePirateTower  Figure = "PirateTower"  // Piratturm
	FigureHarvester2   Figure = "Harvester2"   // Pflücker2
	FigureButcher      Figure = "Butcher"      // Fleischer
	FigureCart         Figure = "Cart"         // Karren
	FigureFirefighter  Figure = "Firefighter"  // Lösch
	FigureCattle       Figure = "Cattle"       // Rind
	FigureCarrier      Figure = "Carrier"      // Träger
	FigureSheep        Figure = "Sheep"        // Schaf
	FigurePackMule     Figure = "PackMule"     // Packesel
	FigureSpearman1    Figure = "Spearman1"    // Speer1
)

// Enum values for Smoke
type Smoke string

const (
	SmokeUnset          Smoke = "Unset"
	SmokeFlagTower3     Smoke = "FlagTower3"     // Fahneturm3
	SmokeSmokeTools     Smoke = "SmokeTools"     // Rauchwerkzeug
	SmokeSmokeCannons   Smoke = "SmokeCannons"   // Rauchkanonen
	SmokeSmokeGold      Smoke = "SmokeGold"      // Rauchgold
	SmokeFlagWarehouse  Smoke = "FlagWarehouse"  // Fahnekontor
	SmokeSmokeBaker     Smoke = "SmokeBaker"     // Rauchbäck
	SmokeFlagWarehouse3 Smoke = "FlagWarehouse3" // Fahnekontor3
	SmokeFlagWarehouse2 Smoke = "FlagWarehouse2" // Fahnekontor2
	SmokeFlagWarehouse1 Smoke = "FlagWarehouse1" // Fahnekontor1
	SmokeSmokeSword     Smoke = "SmokeSword"     // Rauchschwert
	SmokeSmokeSmelter   Smoke = "SmokeSmelter"   // Rauchschmelz
	SmokeFlagWarehouse4 Smoke = "FlagWarehouse4" // Fahnekontor4
	SmokeFlagMarket     Smoke = "FlagMarket"     // FahneMarkt
	SmokeSmokeBurner    Smoke = "SmokeBurner"    // Rauchbrenner
	SmokeFlagTower2     Smoke = "FlagTower2"     // Fahneturm2
	SmokeSmokeMusket    Smoke = "SmokeMusket"    // Rauchmuskete
	SmokeFlagTower1     Smoke = "FlagTower1"     // Fahneturm1
)

var KindMap = map[string]Kind{
	"BODEN":       KindGround,
	"BRANDECK":    KindBurnCorner,
	"BRANDUNG":    KindSurf,
	"BRUECKE":     KindBridge,
	"FELS":        KindRock,
	"FLUSS":       KindRiver,
	"FLUSSECK":    KindRiverCorner,
	"GEBAEUDE":    KindBuilding,
	"HAFEN":       KindHarbor,
	"HANG":        KindSlope,
	"HANGECK":     KindSlopeCorner,
	"HANGQUELL":   KindSlopeSpring,
	"HQ":          KindHeadquarters,
	"MAUER":       KindWall,
	"MAUERSTRAND": KindWallBeach,
	"MEER":        KindSea,
	"MINE":        KindMine,
	"MUENDUNG":    KindEstuary,
	"PIER":        KindPier,
	"PLATZ":       KindPlaza,
	"RUINE":       KindRuin,
	"STRAND":      KindBeach,
	"STRANDECKA":  KindBeachCornerIII,
	"STRANDECKI":  KindBeachCornerI,
	"STRANDHAUS":  KindBeachHouse,
	"STRANDMUND":  KindBeachMouth,
	"STRANDRUINE": KindBeachRuin,
	"STRANDVARI":  KindBeachCornerII,
	"STRASSE":     KindStreet,
	"TOR":         KindGate,
	"TURM":        KindTower,
	"TURMSTRAND":  KindTowerBeach,
	"WALD":        KindForrest,
	"WMUEHLE":     KindWMill,
}

var RuinMap = map[string]Ruin{
	"RUINE_KONTOR_N1":  RuinWarehouseRuinsN1,
	"RUINE_KONTOR_N2":  RuinWarehouseRuinsN2,
	"RUINE_HOLZ":       RuinWoodRuins,
	"RUINE_KONTOR_1":   RuinWarehouseRuins1,
	"RUINE_FELD":       RuinFieldRuins,
	"RUINE_MARKT":      RuinMarketRuins,
	"RUINE_STEIN":      RuinStoneRuins,
	"RUINE_MINE":       RuinMineRuins,
	"RUINE_ROAD_STEIN": RuinRoadStoneRuins,
	"RUINE_ROAD_FELD":  RuinRoadFieldRuins,
}

var BuildSampleMap = map[string]BuildSample{
	"WAV_TRIUMPH":    BuildSampleTriumphWave,
	"WAV_SCHWERTBAU": BuildSampleSwordConstruction,
	"WAV_PYRAMIDE":   BuildSamplePyramid,
	"WAV_KATHETRALE": BuildSampleCathedral,
	"WAV_NIX":        BuildSampleNix,
	"WAV_SCHLOSS":    BuildSampleCastle,
	"WAV_KAPELLE":    BuildSampleChapel,
	"WAV_WOHNHAUS":   BuildSampleResidentialHouse,
	"WAV_BRUNNEN":    BuildSampleWell,
	"WAV_KONTOR":     BuildSampleWarehouse,
	"WAV_GIESSEREI":  BuildSampleFoundry,
	"WAV_FLEISCHER":  BuildSampleButcher,
	"WAV_SCHULE":     BuildSampleSchool,
	"WAV_DENKMAL":    BuildSampleMonument,
	"WAV_BAECKER":    BuildSampleBakery,
	"WAV_MINE":       BuildSampleMine,
	"WAV_STANDARD":   BuildSampleStandard,
	"WAV_BURG":       BuildSampleCastleBurner,
	"WAV_RUMBRENNER": BuildSamplePlantation,
	"WAV_ARZT":       BuildSampleDoctor,
	"WAV_MUEHLE":     BuildSampleMill,
	"WAV_PIRATEN":    BuildSamplePirates,
	"WAV_JAGDHUETTE": BuildSampleHuntingLodge,
	"WAV_KIRCHE":     BuildSampleChurch,
	"WAV_MARKT":      BuildSampleMarket,
	"WAV_BADEHAUS":   BuildSampleBathhouse,
	"WAV_WIRTSHAUS":  BuildSampleInn,
	"WAV_THEATER":    BuildSampleTheater,
}

var MaterialMap = map[string]Material{
	"HOLZ":   MaterialWood,
	"STOFFE": MaterialCloth,
}

var OreMountainMap = map[string]OreMountain{
	"ERZBERG_KLEIN": OreMountainSmallMine,
	"ERZBERG_GROSS": OreMountainLargeMine,
}

var WareMap = map[string]Ware{
	"MEHL":        WareFlour,
	"GETREIDE":    WareGrain,
	"GOLD":        WareGold,
	"KAKAO":       WareCocoa,
	"WEINTRAUBEN": WareGrapes,
	"ALLWARE":     WareAll,
	"TABAK":       WareTobacco,
	"FISCHE":      WareFish,
	"STOFFE":      WareCloth,
	"ALKOHOL":     WareAlcohol,
	"ERZE":        WareOre,
	"ZIEGEL":      WareBrick,
	"GRAS":        WareGrass,
	"TABAKBAUM":   WareTobaccoPlant,
	"EISENERZ":    WareIronOre,
	"NOWARE":      WareNoWare,
	"SCHMUCK":     WareJewelry,
	"KLEIDUNG":    WareClothing,
	"SCHWERTER":   WareSwords,
	"HOLZ":        WareWood,
	"BAUM":        WareTree,
	"STEINE":      WareStones,
	"KORN":        WareGrain,
	"KAKAOBAUM":   WareCocoaPlant,
	"EISEN":       WareIron,
	"KANONEN":     WareCannons,
	"FLEISCH":     WareMeat,
	"GEWUERZE":    WareSpices,
	"TABAKWAREN":  WareTobaccoGoods,
	"ZUCKER":      WareSugar,
	"NAHRUNG":     WareFood,
	"WOLLE":       WareWool,
	"BAUMWOLLE":   WareCotton,
	"WERKZEUG":    WareTool,
	"GEWUERZBAUM": WareSpicePlant,
	"ZUCKERROHR":  WareSugarCane,
	"MUSKETEN":    WareMusket,
}

var RawMaterialMap = map[string]RawMaterial{
	"KORN":        RawMaterialGrain,
	"GETREIDE":    RawMaterialGrain,
	"GOLD":        RawMaterialGold,
	"WEINTRAUBEN": RawMaterialGrapes,
	"ZUCKER":      RawMaterialSugar,
	"TABAK":       RawMaterialTobacco,
	"FISCHE":      RawMaterialFish,
	"STOFFE":      RawMaterialCloth,
	"GRAS":        RawMaterialGrass,
	"TABAKBAUM":   RawMaterialTobaccoPlant,
	"MEHL":        RawMaterialFlour,
	"EISENERZ":    RawMaterialIronOre,
	"NOWARE":      RawMaterialNoWare,
	"HOLZ":        RawMaterialWood,
	"BAUM":        RawMaterialTree,
	"ALLWARE":     RawMaterialAll,
	"KAKAOBAUM":   RawMaterialCocoaPlant,
	"EISEN":       RawMaterialIron,
	"FLEISCH":     RawMaterialMeat,
	"STEINE":      RawMaterialStones,
	"WOLLE":       RawMaterialWool,
	"BAUMWOLLE":   RawMaterialCotton,
	"WILD":        RawMaterialWild,
	"GEWUERZBAUM": RawMaterialSpicePlant,
	"ZUCKERROHR":  RawMaterialSugarCane,
}

var MaxprodcntMap = map[string]Maxprodcnt{
	"MAXPRODCNT": MaxprodcntMaxprodcnt,
}

var FigureMap = map[string]Figure{
	"TRAEGER2":     FigureCarrier2,
	"HOLZFAELLER":  FigureWoodcutter,
	"KANONTURM":    FigureCannonTower,
	"FISCHER":      FigureFisher,
	"STEINKLOPFER": FigureStoneBreaker,
	"PFLUECKER":    FigureHarvester,
	"ARZT":         FigureDoctor,
	"JAEGER":       FigureHunter,
	"MAEHER":       FigureReaper,
	"PIRATTURM":    FigurePirateTower,
	"PFLUECKER2":   FigureHarvester2,
	"FLEISCHER":    FigureButcher,
	"KARREN":       FigureCart,
	"LOESCH":       FigureFirefighter,
	"RIND":         FigureCattle,
	"TRAEGER":      FigureCarrier,
	"SCHAF":        FigureSheep,
	"PACKESEL":     FigurePackMule,
	"SPEER1":       FigureSpearman1,
}

var SmokeMap = map[string]Smoke{
	"FAHNETURM3":    SmokeFlagTower3,
	"RAUCHWERKZEUG": SmokeSmokeTools,
	"RAUCHKANONEN":  SmokeSmokeCannons,
	"RAUCHGOLD":     SmokeSmokeGold,
	"FAHNEKONTOR":   SmokeFlagWarehouse,
	"RAUCHBAECK":    SmokeSmokeBaker,
	"FAHNEKONTOR3":  SmokeFlagWarehouse3,
	"FAHNEKONTOR2":  SmokeFlagWarehouse2,
	"FAHNEKONTOR1":  SmokeFlagWarehouse1,
	"RAUCHSCHWERT":  SmokeSmokeSword,
	"RAUCHSCHMELZ":  SmokeSmokeSmelter,
	"FAHNEKONTOR4":  SmokeFlagWarehouse4,
	"FAHNEMARKT":    SmokeFlagMarket,
	"RAUCHBRENNER":  SmokeSmokeBurner,
	"FAHNETURM2":    SmokeFlagTower2,
	"RAUCHMUSKET":   SmokeSmokeMusket,
	"FAHNETURM1":    SmokeFlagTower1,
}

var ProdtypKindMap = map[string]Prodtyp{
	"BRUNNEN":    ProdtypWell,
	"KONTOR":     ProdtypWarehouse,
	"HOCHSCHULE": ProdtypUniversity,
	"JAGDHAUS":   ProdtypHuntingLodge,
	"STEINBRUCH": ProdtypQuarry,
	"MARKT":      ProdtypMarket,
	"PLANTAGE":   ProdtypPlantation,
	"DENKMAL":    ProdtypMonument,
	"FISCHEREI":  ProdtypFishery,
	"WEIDETIER":  ProdtypPasture,
	"SCHLOSS":    ProdtypCastle,
	//"WIERT":      ProdtypInn,
	"BADEHAUS":  ProdtypBathhouse,
	"WIRTSHAUS": ProdtypInn,
	"THEATER":   ProdtypTheater,
}

var BuildInfraMap = map[string]BuildInfra{
	"INFRA_STUFE_3F":   BuildInfraLevel3F,
	"INFRA_STUFE_1A":   BuildInfraLevel1A,
	"INFRA_STUFE_3B":   BuildInfraLevel3B,
	"INFRA_STUFE_3C":   BuildInfraLevel3C,
	"INFRA_KATHETRALE": BuildInfraCathedral,
	"INFRA_THEATER":    BuildInfraTheater,
	"INFRA_KIRCHE":     BuildInfraChurch,
	"INFRA_SCHULE":     BuildInfraSchool,
	"INFRA_STUFE_5A":   BuildInfraLevel5A,
	"INFRA_GALGEN":     BuildInfraGallows,
	"INFRA_SCHLOSS":    BuildInfraCastle,
	"INFRA_TRIUMPH":    BuildInfraTriumph,
	"INFRA_STUFE_2E":   BuildInfraLevel2E,
	"INFRA_STUFE_2D":   BuildInfraLevel2D,
	"INFRA_STUFE_2G":   BuildInfraLevel2G,
	"INFRA_STUFE_2F":   BuildInfraLevel2F,
	"INFRA_STUFE_2A":   BuildInfraLevel2A,
	"INFRA_STUFE_2C":   BuildInfraLevel2C,
	"INFRA_STUFE_2B":   BuildInfraLevel2B,
	"INFRA_ARZT":       BuildInfraDoctor,
	"INFRA_STUFE_4A":   BuildInfraLevel4A,
	"INFRA_WIRT":       BuildInfraTavern,
	"INFRA_BADE":       BuildInfraBathhouse,
	"INFRA_DENKMAL":    BuildInfraMonument,
	"INFRA_HOCHSCHULE": BuildInfraUniversity,
}
