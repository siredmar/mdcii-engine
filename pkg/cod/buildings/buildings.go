package buildings

import (
	"encoding/json"
	"fmt"
	"os"

	"github.com/siredmar/mdcii-engine/pkg/cod"
)

type BuildingSize struct {
	W int `json:"w"`
	H int `json:"h"`
}

type HouseProductionType struct {
	BGruppe        int         `json:"bgruppe"`
	Anicontflg     int         `json:"anicontflg"`
	MakLagFlg      int         `json:"maklagflg"`
	Nativflg       int         `json:"nativflg"`
	LagAniFlg      int         `json:"laganiflg"`
	Doerrflg       int         `json:"doerrflg"`
	NoMoreWork     int         `json:"nomorework"`
	Workmenge      int         `json:"workmenge"`
	NoLagVoll      int         `json:"nolagvoll"`
	Radius         int         `json:"radius"`
	Rohmenge       int         `json:"rohmenge"`
	Prodmenge      int         `json:"prodmenge"`
	Randwachs      int         `json:"randwachs"`
	Maxlager       int         `json:"maxlager"`
	Maxnorohst     int         `json:"maxnorohst"`
	Arbeiter       int         `json:"arbeiter"`
	Figuranz       int         `json:"figuranz"`
	Interval       int         `json:"interval"`
	Workstoff      Material    `json:"workstoff"`
	Erzbergnr      OreMountain `json:"erzbergnr"`
	Kind           Kind        `json:"kind"`
	Ware           Ware        `json:"ware"`
	Rohstoff       RawMaterial `json:"rohstoff"`
	Maxprodcnt     Maxprodcnt  `json:"maxprodcnt"`
	Bauinfra       BuildInfra  `json:"bauinfra"`
	Figurnr        Figure      `json:"figurnr"`
	Rauchfignr     Smoke       `json:"rauchfignr"`
	Maxware        []int       `json:"maxware"`
	OperationCosts []int       `json:"operation_costs"` // kosten
}
type HouseBuildCosts struct {
	Money  int `json:"money"`
	Tools  int `json:"tools"`
	Wood   int `json:"wood"`
	Stone  int `json:"stone"`
	Canons int `json:"canons"`
}

// Building struct represents the building information.
type Building struct {
	Id              int                 `json:"id"`
	Gfx             int                 `json:"gfx"`
	Block           int                 `json:"block"`
	Kind            Kind                `json:"kind"`
	PositionOffset  int                 `json:"position_offset"`
	PathSpeeds      []int               `json:"path_speeds"`
	HighFlag        int                 `json:"high_flag"`
	Einhoffs        int                 `json:"einhoffs"`
	BuildSample     BuildSample         `json:"build_sample"`
	Ruin            Ruin                `json:"ruin"`
	MaxEnergy       int                 `json:"max_energy"`
	MaxFire         int                 `json:"max_fire"`
	Size            BuildingSize        `json:"size"`
	Rotate          int                 `json:"rotate"`
	RandomAmount    int                 `json:"random_amount"`
	AnimationTime   int                 `json:"animation_time"`
	AnimationFrame  int                 `json:"animation_frame"`
	AnimationAdd    int                 `json:"animation_add"`
	AnimationAmount int                 `json:"animation_amount"`
	BuildGfx        int                 `json:"build_gfx"`
	PlaceFlag       int                 `json:"place_flag"`
	CrossBase       int                 `json:"cross_base"`
	NoShotFlag      int                 `json:"no_shot_flag"`
	BeachFlag       int                 `json:"beach_flag"`
	UpgradeFlag     int                 `json:"upgrade_flag"`
	DoorFlag        int                 `json:"door_flag"`
	RandomGrowth    int                 `json:"random_growth"`
	RandomAdd       int                 `json:"random_add"`
	BeachOffset     int                 `json:"beach_offset"`
	DestroyFlag     int                 `json:"destroy_flag"`
	HouseProduction HouseProductionType `json:"house_production"`
	HouseBuildCosts HouseBuildCosts     `json:"house_build_costs"`
}

// Buildings struct manages the buildings information.
type Buildings struct {
	IdOffset        int               `json:"id_offset"`
	Cod             *cod.Cod          `json:"cod"`
	Buildings       map[int]*Building `json:"buildings"`
	BuildingsVector []*Building       `json:"buildings_vector"`
}

// NewBuildings creates a new Buildings object.
func NewBuildings(cod *cod.Cod) (*Buildings, error) {
	b := &Buildings{
		IdOffset:        20000,
		Cod:             cod,
		Buildings:       make(map[int]*Building),
		BuildingsVector: make([]*Building, 0),
	}
	err := b.GenerateBuildings()
	if err != nil {
		return nil, err
	}
	return b, nil
}

// LazyImport imports just the json structure of the buildings structs. You can consider this as a read-only mode.
// Changes to the buildings will not be saved.
func LazyImport(path string) ([]*Building, error) {
	// Open the file and read the byes
	d, err := os.ReadFile(path)
	if err != nil {
		return nil, err
	}
	imported := []*Building{}
	err = json.Unmarshal(d, &imported)
	if err != nil {
		return nil, err
	}

	return imported, nil
}

// GenerateBuildings populates the buildings map.
func (b *Buildings) GenerateBuildings() error {
	for _, obj := range b.Cod.Objects.Object {
		if obj.Name == "HAUS" {
			for _, o := range obj.Objects {
				haus := b.GenerateBuilding(o)
				b.Buildings[haus.Id] = haus
				b.BuildingsVector = append(b.BuildingsVector, b.Buildings[haus.Id])
			}
		}
	}
	return nil
}

// GenerateBuilding creates a Building object from the given Object.
func (b *Buildings) GenerateBuilding(obj *cod.Object) *Building {
	var h Building

	// Populating Building fields based on the Object
	if obj.Variables.Variable != nil {
		for _, variable := range obj.Variables.Variable {
			switch variable.Name {
			case "Id":
				h.Id = b.processId(int(variable.GetValueInt()))
			case "Gfx":
				h.Gfx = int(variable.GetValueInt())
			case "Blocknr":
				h.Block = int(variable.GetValueInt())
			case "Kind":
				h.Kind = KindMap[variable.GetValueString()]
			case "Posoffs":
				h.PositionOffset = int(variable.GetValueInt())
			case "Wegspeed":
				h.PathSpeeds = processWegspeed(variable.GetValueArray())
			case "Highflg":
				h.HighFlag = int(variable.GetValueInt())
			case "Einhoffs":
				h.Einhoffs = int(variable.GetValueInt())
			case "Bausample":
				h.BuildSample = BuildSampleMap[variable.GetValueString()]
			case "Ruinenr":
				h.Ruin = RuinMap[variable.GetValueString()]
			case "Maxenergy":
				h.MaxEnergy = int(variable.GetValueInt())
			case "Maxbrand":
				h.MaxFire = int(variable.GetValueInt())
			case "Size":
				h.Size.W = int(variable.GetValueArray().Value[0].GetValueInt())
				h.Size.H = int(variable.GetValueArray().Value[1].GetValueInt())
			case "Rotate":
				h.Rotate = int(variable.GetValueInt())
			case "RandAnz":
				h.RandomAmount = int(variable.GetValueInt())
			case "AnimTime":
				h.AnimationTime = processAnimTime(variable)
			case "AnimFrame":
				h.AnimationFrame = int(variable.GetValueInt())
			case "AnimAdd":
				h.AnimationAdd = int(variable.GetValueInt())
			case "Baugfx":
				h.BuildGfx = int(variable.GetValueInt())
			case "PlaceFlg":
				h.PlaceFlag = int(variable.GetValueInt())
			case "AnimAnz":
				h.AnimationAmount = int(variable.GetValueInt())
			case "KreuzBase":
				h.CrossBase = int(variable.GetValueInt())
			case "NoShotFlg":
				h.NoShotFlag = int(variable.GetValueInt())
			case "Strandflg":
				h.BeachFlag = int(variable.GetValueInt())
			case "Ausbauflg":
				h.UpgradeFlag = int(variable.GetValueInt())
			case "Tuerflg":
				h.DoorFlag = int(variable.GetValueInt())
			case "Randwachs":
				h.RandomGrowth = int(variable.GetValueInt())
			case "RandAdd":
				h.RandomAdd = int(variable.GetValueInt())
			case "Strandoff":
				h.BeachOffset = int(variable.GetValueInt())
			case "Destroyflg":
				h.DestroyFlag = int(variable.GetValueInt())
			}
		}
	}

	if len(obj.Objects) > 0 {
		for _, o := range obj.Objects {
			nestedObj := o
			if nestedObj.Name == "HAUS_PRODTYP" {
				h.HouseProduction = processHouseProdTyp(nestedObj.Variables)
			} else if nestedObj.Name == "HAUS_BAUKOST" {
				h.HouseBuildCosts = processHouseBauKost(nestedObj.Variables)
			}
		}
	}

	return &h
}

// Helper functions to process specific fields
func (b *Buildings) processId(value int) int {
	if value == 0 {
		return 0
	}
	return value - b.IdOffset
}

func processWegspeed(valueArray *cod.ArrayValue) []int {
	var result []int
	for _, v := range valueArray.Value {
		result = append(result, int(v.GetValueInt()))
	}
	return result
}

func processAnimTime(value *cod.Variable) int {
	if value.GetValueString() == "TIMENEVER" {
		return -1
	}
	return int(value.GetValueInt())
}

func processHouseProdTyp(vars *cod.Variables) HouseProductionType {
	var prodType HouseProductionType

	for _, variable := range vars.Variable {
		switch variable.Name {
		case "Kind":
			prodType.Kind = KindMap[variable.GetValueString()]
		case "Ware":
			prodType.Ware = WareMap[variable.GetValueString()]
		case "Workstoff":
			prodType.Workstoff = MaterialMap[variable.GetValueString()]
		case "Erzbergnr":
			prodType.Erzbergnr = OreMountainMap[variable.GetValueString()]
		case "Rohstoff":
			prodType.Rohstoff = RawMaterialMap[variable.GetValueString()]
		case "MAXPRODCNT":
			prodType.Maxprodcnt = MaxprodcntMap[variable.GetValueString()]
		case "Bauinfra":
			prodType.Bauinfra = BuildInfraMap[variable.GetValueString()]
		case "Figurnr":
			prodType.Figurnr = FigureMap[variable.GetValueString()]
		case "Rauchfignr":
			prodType.Rauchfignr = SmokeMap[variable.GetValueString()]
		case "Maxware":
			prodType.Maxware = processIntOrArray(variable)
		case "Kosten":
			prodType.OperationCosts = processIntOrArray(variable)
		case "BGruppe":
			prodType.BGruppe = int(variable.GetValueInt())
		case "LagAniFlg":
			prodType.LagAniFlg = int(variable.GetValueInt())
		case "NoMoreWork":
			prodType.NoMoreWork = int(variable.GetValueInt())
		case "Workmenge":
			prodType.Workmenge = int(variable.GetValueInt())
		case "Doerrflg":
			prodType.Doerrflg = int(variable.GetValueInt())
		case "Anicontflg":
			prodType.Anicontflg = int(variable.GetValueInt())
		case "MakLagFlg":
			prodType.MakLagFlg = int(variable.GetValueInt())
		case "Nativflg":
			prodType.Nativflg = int(variable.GetValueInt())
		case "NoLagVoll":
			prodType.NoLagVoll = int(variable.GetValueInt())
		case "Radius":
			prodType.Radius = int(variable.GetValueInt())
		case "Rohmenge":
			prodType.Rohmenge = int(variable.GetValueInt())
		case "Prodmenge":
			prodType.Prodmenge = int(variable.GetValueInt())
		case "Randwachs":
			prodType.Randwachs = int(variable.GetValueInt())
		case "Maxlager":
			prodType.Maxlager = int(variable.GetValueInt())
		case "Maxnorohst":
			prodType.Maxnorohst = int(variable.GetValueInt())
		case "Arbeiter":
			prodType.Arbeiter = int(variable.GetValueInt())
		case "Figuranz":
			prodType.Figuranz = int(variable.GetValueInt())
		case "Interval":
			prodType.Interval = int(variable.GetValueInt())
		}
	}
	return prodType
}

func processHouseBauKost(vars *cod.Variables) HouseBuildCosts {
	var bauKost HouseBuildCosts

	for _, variable := range vars.Variable {
		switch variable.Name {
		case "Money":
			bauKost.Money = int(variable.GetValueInt())
		case "Werkzeug":
			bauKost.Tools = int(variable.GetValueInt())
		case "Holz":
			bauKost.Wood = int(variable.GetValueInt())
		case "Ziegel":
			bauKost.Stone = int(variable.GetValueInt())
		case "Kanon":
			bauKost.Canons = int(variable.GetValueInt())
		}
	}
	return bauKost
}

func processIntOrArray(value *cod.Variable) []int {
	var result []int
	// type assert between Variable_ValueArray and Variable_ValueInt using switch case
	switch v := value.GetValue().(type) {
	case *cod.Variable_ValueArray:
		for _, v := range v.ValueArray.Value {
			result = append(result, int(v.GetValueInt()))
		}
	case *cod.Variable_ValueInt:
		result = append(result, int(v.ValueInt))
	}

	return result
}

func (b *Buildings) GetBuilding(id int) (*Building, error) {
	building, ok := b.Buildings[id]
	if !ok {
		return nil, fmt.Errorf("Building with ID %d not found", id)
	}
	return building, nil
}

func (b *Buildings) GetBuildingByIndex(index int) (*Building, error) {
	if index < 0 || index >= len(b.BuildingsVector) {
		return nil, fmt.Errorf("Index %d out of bounds", index)
	}
	return b.BuildingsVector[index], nil
}

func (b *Buildings) GetBuildingsSize() int {
	return len(b.BuildingsVector)
}

func (b *Buildings) GetBuildings() []*Building {
	return b.BuildingsVector
}

func GetObjectKindGfxMap(m []*Building) map[Kind][]int {
	result := make(map[Kind][]int)

	for _, building := range m {
		// result[building.Kind] = append(result[building.Kind], buildig.Gfx)
		baseGfx := building.Gfx
		baseFramesPerRotation := building.Rotate
		rotations := func() int {
			if baseFramesPerRotation == 0 {
				return 1
			}
			return 8
		}()
		numSteps := building.AnimationAmount
		gfxPerStep := building.AnimationAdd

		for rotationIdx := 0; rotationIdx < rotations; rotationIdx++ {
			gfx := baseGfx + rotationIdx*baseFramesPerRotation
			for step := 0; step < numSteps; step++ {
				result[building.Kind] = append(result[building.Kind], gfx)
				gfx += gfxPerStep
			}
		}

		// for (let rotationIdx = 0; rotationIdx < rotations; rotationIdx++) {
		// 	const spritesForRotation: AnimatedSprite[] = [];

		// 	let gfx =
		// 	  baseGfx + animation.AnimOffs + rotationIdx * framesPerRotation;

		// 	const frames: Texture[] = [];
		// 	for (let step = 0; step < numSteps; step++) {
		// 	  frames.push(textures.get(gfx)!);
		// 	  gfx += gfxPerStep;
		// 	}

	}

	return result
}
