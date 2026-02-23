package cod

import (
	"encoding/json"
	fmt "fmt"
	"os"
)

type CodIntern struct {
	variableNumbers      map[string]int
	variableNumbersArray map[string][]int
	constants            Variables
	currentObjectIndex   int
	currentObject        *Object
	objectStack          *Stack
	objectMap            map[string]*Object
	objectIdMap          map[int]*Object
	objFillRange         *ObjFillRangeType
}

type Cod struct {
	Lines   []LineType
	Objects Objects
	Intern  CodIntern
}

func NewCod(file string, decode bool) (*Cod, error) {
	b, err := os.ReadFile(file)
	if err != nil {
		return &Cod{}, err
	}
	if decode {
		for i, v := range b {
			b[i] = -v
		}
	}
	lines, err := readLines(b)
	if err != nil {
		return &Cod{}, err
	}
	return &Cod{
		Lines:   lines,
		Objects: Objects{},
		Intern: CodIntern{
			variableNumbers:      map[string]int{},
			variableNumbersArray: map[string][]int{},
			constants: Variables{
				Variable: []*Variable{},
			},
			currentObjectIndex: -1,
			objectStack:        NewStack(),
			currentObject:      nil,
			objectMap:          map[string]*Object{},
			objectIdMap:        map[int]*Object{},
			objFillRange:       &ObjFillRangeType{},
		},
	}, nil
}

func (cod *Cod) DumpConstants() error {
	jsonBytes, err := json.MarshalIndent(cod.Intern.constants.Variable, "", "    ")
	if err != nil {
		return err
	}
	fmt.Println("Constants:", string(jsonBytes))
	return nil
}

func (cod *Cod) DumpVariables() error {
	fmt.Println("Variables:", cod.Intern.variableNumbers)
	return nil
}

func (cod *Cod) DumpArrayVariables() error {
	fmt.Println("Array Variables:", cod.Intern.variableNumbersArray)
	return nil
}
