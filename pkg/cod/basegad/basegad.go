package basegad

import (
	"fmt"
	"sync"

	"github.com/siredmar/mdcii-engine/pkg/cod"
)

type BaseGadKindType int

const (
	UNSET BaseGadKindType = iota
	GAD_GFX
)

type BaseGadPos struct {
	X int `json:"x"`
	Y int `json:"y"`
}

type BaseGadSize struct {
	H int `json:"h"`
	W int `json:"w"`
}

type BaseGadGadget struct {
	Id       int             `json:"id"`
	Blocknr  int             `json:"blocknr"`
	Gfxnr    int             `json:"gfxnr"`
	Kind     BaseGadKindType `json:"kind"`
	Noselflg int             `json:"noselflg"`
	Pressoff int             `json:"pressoff"`
	Pos      BaseGadPos      `json:"pos"`
	Size     BaseGadSize     `json:"size"`
}

type Basegad struct {
	idOffset      int
	gadgets       map[int]*BaseGadGadget
	gadgetsVector []*BaseGadGadget
	cod           *cod.Cod
	kindMap       map[string]BaseGadKindType
	mu            sync.Mutex
}

func NewBasegad(cod *cod.Cod) (*Basegad, error) {
	b := &Basegad{
		idOffset:      30000,
		gadgets:       make(map[int]*BaseGadGadget),
		gadgetsVector: nil,
		cod:           cod,
		kindMap: map[string]BaseGadKindType{
			"GAD_GFX": GAD_GFX,
		},
	}
	err := b.GenerateGadgets()
	if err != nil {
		return nil, err
	}
	return b, nil
}

func (bg *Basegad) GetGadgets() []*BaseGadGadget {
	return bg.gadgetsVector
}

func (bg *Basegad) GetGadget(id int) (*BaseGadGadget, error) {
	bg.mu.Lock()
	defer bg.mu.Unlock()

	gadget, ok := bg.gadgets[id]
	if !ok {
		return nil, fmt.Errorf("Gadget with ID %d not found", id)
	}
	return gadget, nil
}

func (bg *Basegad) GetGadgetsSize() int {
	bg.mu.Lock()
	defer bg.mu.Unlock()

	return len(bg.gadgetsVector)
}

func (bg *Basegad) GetGadgetsByIndex(index int) (*BaseGadGadget, error) {
	bg.mu.Lock()
	defer bg.mu.Unlock()

	if index < 0 || index >= len(bg.gadgetsVector) {
		return nil, fmt.Errorf("Index %d out of bounds", index)
	}

	return bg.gadgetsVector[index], nil
}

func (bg *Basegad) GenerateGadgets() error {
	bg.mu.Lock()
	defer bg.mu.Unlock()

	for _, obj := range bg.cod.Objects.Object {
		if obj.Name == "GADGET" {
			for i := 0; i < len(obj.Objects); i++ {
				gadget := bg.GenerateGadget(obj.Objects[i])
				bg.gadgets[gadget.Id] = &gadget
				bg.gadgetsVector = append(bg.gadgetsVector, &gadget)
			}
			return nil
		}
	}
	return fmt.Errorf("GADGET object not found")
}

func (bg *Basegad) GenerateGadget(obj *cod.Object) BaseGadGadget {
	var h BaseGadGadget
	if obj.Variables != nil {
		for _, variable := range obj.Variables.Variable {
			switch variable.Name {
			case "Id":
				if variable.GetValueInt() == 0 {
					h.Id = 0
				} else {
					h.Id = int(variable.GetValueInt()) - bg.idOffset
				}
			case "Blocknr":
				h.Blocknr = int(variable.GetValueInt())
			case "Gfxnr":
				h.Gfxnr = int(variable.GetValueInt())
			case "Kind":
				h.Kind = bg.kindMap[variable.GetValueString()]
			case "Noselflg":
				h.Noselflg = int(variable.GetValueInt())
			case "Pressoff":
				h.Pressoff = int(variable.GetValueInt())
			case "Pos":
				h.Pos.X = int(variable.GetValueArray().Value[0].GetValueInt())
				h.Pos.Y = int(variable.GetValueArray().Value[1].GetValueInt())
			case "Size":
				h.Size.W = int(variable.GetValueArray().Value[0].GetValueInt())
				h.Size.H = int(variable.GetValueArray().Value[1].GetValueInt())
			}
		}
	}
	return h
}
