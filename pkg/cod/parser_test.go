package cod

import (
	"testing"

	"github.com/stretchr/testify/assert"
)

// func TestNewPng(t *testing.T) {
// 	// assert := assert.New(t)
// 	data := []byte{}
// 	cod := &Cod{
// 		Content: data,
// 		Decode:  false,
// 	}
// 	cod.Parse()
// }

func TestReadLines1(t *testing.T) {
	assert := assert.New(t)
	data := "this is a test\r\nfollowed by a new line\r\nfin\r\n"
	bytes, err := readLines([]byte(data))
	assert.Nil(err)

	assert.Equal("this is a test", string(bytes[0].Line))
	assert.Equal("followed by a new line", string(bytes[1].Line))
	assert.Equal("fin", string(bytes[2].Line))
}

func TestReadLines2(t *testing.T) {
	assert := assert.New(t)
	data := "this is a test\r\nfollowed by a new line\r\nfin"
	bytes, err := readLines([]byte(data))
	assert.Nil(err)

	assert.Equal("this is a test", string(bytes[0].Line))
	assert.Equal("followed by a new line", string(bytes[1].Line))
}

func TestReadLines3(t *testing.T) {
	assert := assert.New(t)
	data := "this is a test\rfollowed by a new line\r\nfin"
	bytes, err := readLines([]byte(data))
	assert.Nil(err)

	assert.Equal("this is a test\rfollowed by a new line", string(bytes[0].Line))
}

func TestReadLines4(t *testing.T) {
	assert := assert.New(t)
	data := "this is a test\rfollowed by a new line\r\nfin\r\n"
	bytes, err := readLines([]byte(data))
	assert.Nil(err)

	assert.Equal("this is a test\rfollowed by a new line", string(bytes[0].Line))
	assert.Equal("fin", string(bytes[1].Line))
}

func TestReadLines5(t *testing.T) {
	assert := assert.New(t)
	data := "this is a test\r\nfollowed by a new line\r\n;----comment\r\nfin\r\n"
	bytes, err := readLines([]byte(data))
	assert.Nil(err)

	assert.Equal("this is a test", string(bytes[0].Line))
	assert.Equal("followed by a new line", string(bytes[1].Line))
	assert.Equal("fin", string(bytes[2].Line))
}

func TestReadLines6(t *testing.T) {
	assert := assert.New(t)
	data := "this is a test\r\nfollowed by a new line\r\n    ;----comment\r\nfin\r\n"
	bytes, err := readLines([]byte(data))
	assert.Nil(err)

	assert.Equal("this is a test", string(bytes[0].Line))
	assert.Equal("followed by a new line", string(bytes[1].Line))
	assert.Equal("fin", string(bytes[2].Line))
}

func TestCountFrontSpaces(t *testing.T) {
	assert := assert.New(t)
	data := "this is a test"
	assert.Equal(0, countFrontSpaces(data))
	data = " this is a test"
	assert.Equal(1, countFrontSpaces(data))
	data = "  this is a test"
	assert.Equal(2, countFrontSpaces(data))
	data = "   this is a test"
	assert.Equal(3, countFrontSpaces(data))
	data = "    this is a test"
	assert.Equal(4, countFrontSpaces(data))
}

func TestGetValueConstantNotExists(t *testing.T) {
	assert := assert.New(t)
	cod := Cod{
		Lines:   []LineType{},
		Objects: Objects{},
		Intern: CodIntern{
			variableNumbers:      map[string]int{},
			variableNumbersArray: map[string][]int{},
			constants:            Variables{},
		},
	}
	v, err := cod.getValue("FOO", "BAR+5", true)
	assert.Nil(err)
	assert.Equal(&Variable_ValueInt{ValueInt: 5}, v.Value)

}

func TestGetValueConstantExists(t *testing.T) {
	assert := assert.New(t)
	cod := Cod{
		Lines:   []LineType{},
		Objects: Objects{},
		Intern: CodIntern{
			variableNumbers:      map[string]int{},
			variableNumbersArray: map[string][]int{},
			constants:            Variables{},
		},
	}

	cod.Intern.constants.Variable = append(cod.Intern.constants.Variable,
		&Variable{
			Name:  "BAR",
			Value: &Variable_ValueInt{ValueInt: 101},
		},
	)
	cod.Intern.constants.Variable = append(cod.Intern.constants.Variable,
		&Variable{
			Name:  "BAZ",
			Value: &Variable_ValueInt{ValueInt: 100},
		},
	)
	v, err := cod.getValue("FOO", "BAR+5", true)
	assert.Nil(err)
	assert.Equal(&Variable_ValueInt{ValueInt: 106}, v.Value)
	v, err = cod.getValue("FOO", "BAZ-5", true)
	assert.Nil(err)
	assert.Equal(&Variable_ValueInt{ValueInt: 95}, v.Value)
}

func TestParseRelativeAssignment(t *testing.T) {
	assert := assert.New(t)
	objectStack := NewStack()
	objectStack.Push(&ObjectType{
		Object: &Object{Name: "FOO",
			Variables: &Variables{
				Variable: []*Variable{
					{Value: &Variable_ValueInt{ValueInt: 10}},
					{Value: &Variable_ValueInt{ValueInt: 20}},
				},
			},
		},
		NumberObject: false,
		Spaces:       0,
	})

	cod := Cod{
		Lines: []LineType{{
			Line:   "FOO[0] = 100",
			Spaces: 0,
		},
			{
				Line:   "@FOO: +2,+5",
				Spaces: 0,
			},
		},
		Objects: Objects{},
		Intern: CodIntern{
			variableNumbers:      map[string]int{"A": 10},
			variableNumbersArray: map[string][]int{"FOO": {100, 200}},
			constants:            Variables{},
			currentObjectIndex:   -1,
			currentObject:        &Object{},
			objectStack:          objectStack,
		},
	}

	err := cod.Parse()
	assert.Nil(err)
	cod.DumpVariables()
	cod.DumpArrayVariables()
}

func TestParseConstantAssignment(t *testing.T) {
	assert := assert.New(t)
	objectStack := NewStack()

	cod := Cod{
		Lines: []LineType{
			{
				Line:   "FOO = BAR",
				Spaces: 0,
			},
			{
				Line:   "BAR=FOO",
				Spaces: 0,
			},
			{
				Line:   "A = 5000",
				Spaces: 0,
			},
			{
				Line:   "B = VARIABLE+10",
				Spaces: 0,
			},
			{
				Line:   "C = A + 10",
				Spaces: 0,
			},
			{
				Line:   "D = 3.14",
				Spaces: 0,
			},
		},
		Objects: Objects{},
		Intern: CodIntern{
			variableNumbers:      map[string]int{},
			variableNumbersArray: map[string][]int{},
			constants:            Variables{},
			currentObjectIndex:   -1,
			currentObject:        &Object{},
			objectStack:          objectStack,
		},
	}

	err := cod.Parse()
	assert.Nil(err)
	assert.Equal(6, len(cod.Intern.constants.Variable))
	assert.Equal("FOO", cod.Intern.constants.Variable[0].Name)
	assert.Equal("BAR", cod.Intern.constants.Variable[0].GetValueString())

	assert.Equal("BAR", cod.Intern.constants.Variable[1].Name)
	assert.Equal("BAR", cod.Intern.constants.Variable[1].GetValueString())

	assert.Equal("A", cod.Intern.constants.Variable[2].Name)
	assert.Equal(int32(5000), cod.Intern.constants.Variable[2].GetValueInt())

	assert.Equal("B", cod.Intern.constants.Variable[3].Name)
	assert.Equal(int32(10), cod.Intern.constants.Variable[3].GetValueInt())

	assert.Equal("C", cod.Intern.constants.Variable[4].Name)
	assert.Equal(int32(5010), cod.Intern.constants.Variable[4].GetValueInt())

	assert.Equal("D", cod.Intern.constants.Variable[5].Name)
	assert.Equal(float32(3.14), cod.Intern.constants.Variable[5].GetValueFloat())

}
