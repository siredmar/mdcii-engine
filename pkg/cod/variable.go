package cod

import (
	fmt "fmt"
	"regexp"
	"strconv"
	"strings"
)

// standard variable assignment
// examples: 'a:  1'
func (c *Cod) handleVariable(line string, spaces int) (bool, error) {
	line = strings.ReplaceAll(line, " ", "")
	if strings.Contains(line, "Objekt") {
		return false, nil
	}
	if strings.Contains(line, "ObjFill") {
		return false, nil
	}
	if strings.Contains(line, "Nummer") {
		return false, nil
	}
	if strings.Contains(line, ",") {
		return false, nil
	}
	matches := regexSearch(`(\w+):(\w+)`, line)
	if len(matches) > 0 {
		// if c.Intern.currentObject == nil {
		// c.Intern.unparsedLines = append(c.Intern.unparsedLines, line)
		// }
		name := matches[0]
		value := matches[1]
		index := c.ExistsInCurrentObject(name)
		variable := &Variable{}
		if index != -1 {
			variable = c.Intern.currentObject.Variables.Variable[index]
		} else {
			c.Intern.currentObject.Variables.Variable = append(c.Intern.currentObject.Variables.Variable, variable)
			variable.Name = name
		}
		if checkType(value) == INT {
			if name == "Id" {
				c.Intern.objectIdMap[Atoi(value)] = c.Intern.currentObject
			}
			variable.Value = &Variable_ValueInt{
				ValueInt: int32(Atoi(value)),
			}
			c.Intern.variableNumbers[name] = Atoi(value)
		} else {
			if c.constantExists(value) != -1 {
				v := c.GetVariableFromConstants(value)
				// handle all types of constants using reflection switch case
				switch v.Value.(type) {
				case *Variable_ValueInt:
					variable.Value = &Variable_ValueInt{
						ValueInt: v.GetValue().(*Variable_ValueInt).ValueInt,
					}
				case *Variable_ValueFloat:
					variable.Value = &Variable_ValueFloat{
						ValueFloat: v.GetValue().(*Variable_ValueFloat).ValueFloat,
					}
				case *Variable_ValueString:
					variable.Value = &Variable_ValueString{
						ValueString: v.GetValue().(*Variable_ValueString).ValueString,
					}
				}
			} else {
				variable.Value = &Variable_ValueString{
					ValueString: value,
				}
			}
		}

		return true, nil
	}
	return false, nil

}

// relative array assignment, examples:
// example: '@Pos:       +0, +42'
func (c *Cod) handleVariableRelativeArray(line string) (bool, error) {
	line = strings.ReplaceAll(line, " ", "")
	if matches := regexSearch(`@(\w+):.*(,)`, line); len(matches) > 0 {
		name := matches[0]
		offsets := []int{}
		if result := regexSearch(`:\s*(.*)`, line); len(result) > 0 {
			tokens := strings.Split(result[0], ",")
			for _, e := range tokens {
				e = strings.TrimSpace(e)
				if number := regexSearch(`([+|-]?)(\d+)`, e); len(number) > 0 {
					offset, err := strconv.Atoi(number[1])
					if err != nil {
						return true, err
					}
					if number[0] == "-" {
						offset *= -1
					}
					offsets = append(offsets, int(offset))

				}
			}
		}

		index := c.ExistsInCurrentObject(name)
		currentArrayValues := []int{}
		if index != -1 {
			c.Intern.currentObject.Variables.Variable[index].Value.(*Variable_ValueArray).ValueArray.Value = []*Value{}
		} else {
			newVar := Variable{
				Name: name,
				Value: &Variable_ValueArray{
					ValueArray: &ArrayValue{
						Value: []*Value{},
					},
				},
			}
			c.Intern.currentObject.Variables.Variable = append(c.Intern.currentObject.Variables.Variable, &newVar)
			index = len(c.Intern.currentObject.Variables.Variable) - 1
		}
		for i := 0; i < len(offsets); i++ {
			var currentValue int
			if index != -1 {
				if _, ok := c.Intern.variableNumbersArray[name]; ok {
					currentValue = c.Intern.variableNumbersArray[name][i]
					currentValue = calculateOperation(currentValue, "+", offsets[i])
					// append value to c.Intern.currentObject.Variables.Variable[index].Value that is type Variable_ValueArray
					c.Intern.currentObject.Variables.Variable[index].Value.(*Variable_ValueArray).ValueArray.Value = append(c.Intern.currentObject.Variables.Variable[index].Value.(*Variable_ValueArray).ValueArray.Value, &Value{Value: &Value_ValueInt{ValueInt: int32(currentValue)}})
				} else {
					return true, fmt.Errorf("no current object")
				}
			} else {
				if _, ok := c.Intern.variableNumbersArray[name]; ok {
					currentValue = calculateOperation(c.Intern.variableNumbersArray[name][i], "+", offsets[i])
					v := c.CreateOrReuseVariable(name)
					if v != nil {
						v.Name = name
						v.Value = &Variable_ValueInt{
							ValueInt: int32(currentValue),
						}
					}
				} else {
					return true, fmt.Errorf("no current object")
				}
			}
			currentArrayValues = append(currentArrayValues, currentValue)
		}
		c.Intern.variableNumbersArray[name] = currentArrayValues
	} else {
		return false, fmt.Errorf("line no relative array assignment")
	}
	return true, nil
}

// relative assignment, examples:
// '@Pos: +42'
// '@Gfx:       -36'
func (c *Cod) handleVariableRelative(line string) (bool, error) {
	line = strings.ReplaceAll(line, " ", "")
	if matches := regexSearch(`(@)(\w+)\s*:\s*([+|-]?)(\d+)`, line); len(matches) > 0 {
		name := matches[1]
		if name == NUMBER_INCREMENT {
			return false, fmt.Errorf("line no relative assignment")
		}
		index := c.ExistsInCurrentObject(name)
		operand, err := strconv.Atoi(matches[3])
		if err != nil {
			return true, err
		}
		currentValue := 0
		if index != -1 {
			currentValue = c.Intern.variableNumbers[c.Intern.currentObject.Variables.Variable[index].Name]
			currentValue = calculateOperation(currentValue, matches[2], operand)
			c.Intern.currentObject.Variables.Variable[index].Value = &Variable_ValueInt{
				ValueInt: int32(currentValue),
			}
		} else {
			currentValue = c.Intern.variableNumbers[matches[1]]
			currentValue = calculateOperation(currentValue, matches[2], operand)
			v := c.CreateOrReuseVariable(name)
			if v != nil {
				v.Name = name
				v.Value = &Variable_ValueInt{
					ValueInt: int32(currentValue),
				}
			}
		}
		c.Intern.variableNumbers[name] = currentValue
	} else {
		return false, fmt.Errorf("line no relative assignment")
	}
	return true, nil
}

// assignment with constant, examples:
// 'Gfx:        GFXBODEN+80'
func (c *Cod) handleVariableWithConstant(line string) (bool, error) {
	line = strings.ReplaceAll(line, " ", "")
	if strings.Contains(line, "Nummer") {
		return false, nil
	}
	line = strings.ReplaceAll(line, " ", "")
	if matches := regexSearch(`(\w+):(\w+)([+|-])(\d+)`, line); len(matches) > 0 {
		name := matches[0]
		constant := matches[1]
		operation := matches[2]

		index := c.ExistsInCurrentObject(name)

		operand, err := strconv.Atoi(matches[3])
		if err != nil {
			return true, err
		}
		currentValue := -1
		if c.constantExists(constant) != -1 {
			v := c.GetVariableFromConstants(constant)
			currentValue = int(v.GetValue().(*Variable_ValueInt).ValueInt)
		}
		if currentValue != -1 {
			currentValue = calculateOperation(currentValue, operation, operand)
		} else {
			currentValue = 0
		}
		c.Intern.variableNumbers[name] = currentValue

		if index != -1 {
			c.Intern.currentObject.Variables.Variable[index].Value = &Variable_ValueInt{
				ValueInt: int32(currentValue),
			}
		} else {
			v := c.CreateOrReuseVariable(name)
			if v != nil {
				v.Name = name
				v.Value = &Variable_ValueInt{
					ValueInt: int32(currentValue),
				}
			}
		}
		return true, nil
	}
	return false, fmt.Errorf("line no assignment with constant")
}

type CodValueType int

const (
	INT CodValueType = iota
	FLOAT
	STRING
	KEYVALUE
)

func checkType(s string) CodValueType {
	intRegex := regexp.MustCompile(`^[-|+]?[0-9]+$`)
	floatRegex := regexp.MustCompile(`^[-|+]?[0-9]+\.[0-9]+$`)
	keyValueRegex := regexp.MustCompile(`^([A-Z\-_a-z]+),\s*((-?\d+(\.\d+)?)+)$`)

	if intRegex.MatchString(s) {
		return INT
	} else if floatRegex.MatchString(s) {
		return FLOAT
	} else if keyValueRegex.MatchString(s) {
		return KEYVALUE
	} else {
		return STRING
	}
}

func (c *Cod) ExistsInCurrentObject(variablename string) int {
	if c.Intern.currentObject != nil {
		// Check if variable already exists in currentObject (e.g. copied from ObjFill)
		for index, v := range c.Intern.currentObject.Variables.Variable {
			if v.Name == variablename {
				return index
			}
		}
	}
	return -1
}

func (c *Cod) CreateOrReuseVariable(name string) *Variable {
	if c.Intern.currentObject != nil {
		optionalVar := c.GetVariableFromObject(c.Intern.currentObject, name)
		if optionalVar != nil {
			return optionalVar
		}
	}
	variable := &Variable{}
	c.Intern.currentObject.Variables.Variable = append(c.Intern.currentObject.Variables.Variable, variable)
	return variable
}

func (c *Cod) GetVariableFromConstants(key string) Variable {
	for i := 0; i < len(c.Intern.constants.Variable); i++ {
		if c.Intern.constants.Variable[i].Name == key {
			return *c.Intern.constants.Variable[i]
		}
	}
	return Variable{}
}

func (c *Cod) GetVariableFromObject(obj *Object, name string) *Variable {
	if obj.Variables != nil {
		for i := 0; i < len(obj.Variables.Variable); i++ {
			if obj.Variables.Variable[i].Name == name {
				return obj.Variables.Variable[i]
			}
		}
	}
	return nil
}
