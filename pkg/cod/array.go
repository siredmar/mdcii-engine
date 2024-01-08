package cod

import (
	"strconv"
	"strings"
)

// absolut array assignment, examples:
func (c *Cod) handleArray(line string) (bool, error) {
	if strings.Contains(line, ",") && !strings.Contains(line, "ObjFill") {
		// example: 'Arr: 5, 6'
		if strings.Contains(line, "[") {
			return c.handleArrayWithReference(line)
		}
		return c.handleStandardArray(line)
	}
	return false, nil
}

// if strings.Contains(line, "[") {
// 	if matches := regexSearch(`"(\w+)\s*:\s*(.+)`, line); len(matches) > 0 {
// 		name := matches[0]
// 		v, err := c.createOrReuseVariable(name)
// 		if err != nil {
// 			return true, err
// 		}
// 		if c.existsInCurrentObject(name) != -1 {
// 			v.Value = &Variable_ValueArray{}
// 		}
// 		v.Name = name
// 		values := matches[2]
// 		splitValues := strings.Split(values, ",")
// 		for _, e := range splitValues {
// 			tokens := regexSearch(`((\d+)([+|-])(\w+)\[(\d+)\])`, e)
// 			if len(tokens) > 0 {
// 				i := c.existsInCurrentObject(tokens[4])
// 				if i != -1 {
// 					arrayValue := int(c.Intern.currentObject.Variables.Variable[i].GetValueArray().Value[Atoi(tokens[5])].GetValueInt())
// 					newvalue := calculateOperation(Atoi(tokens[2]), tokens[3], arrayValue)
// 					v.Value.(*Variable_ValueArray).ValueArray.Value = append(v.Value.(*Variable_ValueArray).ValueArray.Value, &Variable_ValueInt{ValueInt: int32(newvalue)})
// 				}
// 			}
// 		}
// 	} else {

// 	}
// }

// example: 'Var: 1, 2, 3' and KeyValue Pairs like `Var: foo, bar`
func (c *Cod) handleStandardArray(line string) (bool, error) {
	line = strings.ReplaceAll(line, " ", "")
	if strings.Contains(line, "Objekt") {
		return false, nil
	}
	if matches := regexSearch(`(\w+)\s*:\s*(.+)`, line); len(matches) > 0 {
		name := matches[0]
		valueRaw := matches[1]
		// if c.Intern.objectStack.Size() == 0 {
		// 	c.Intern.unparsedLines = append(c.Intern.unparsedLines, line)
		// 	return false, nil
		// }
		currentArrayValues := []int{}
		values := strings.Split(valueRaw, ",")
		varExists := c.ExistsInCurrentObject(name)
		variable := c.CreateOrReuseVariable(name)
		variable.Name = name
		keyValue := false
		if checkType(valueRaw) == KEYVALUE {
			keyValue = true
		}
		if varExists == -1 {
			variable.Value = &Variable_ValueArray{}
			variable.Value.(*Variable_ValueArray).ValueArray = &ArrayValue{}
			variable.Value.(*Variable_ValueArray).ValueArray.Value = []*Value{}
		} else {
			if !keyValue {
				variable.Value.(*Variable_ValueArray).ValueArray.Value = []*Value{}
			}
		}
		arr := variable.Value.(*Variable_ValueArray).ValueArray
		// Handle KeyValue Pairs
		// Example: 'Var: foo, bar'
		if keyValue {
			if checkType(values[1]) != STRING {
				arr.Value = append(arr.Value, &Value{Value: &Value_ValueKeyValue{ValueKeyValue: &KeyValue{Key: values[0], Value: &KeyValue_ValueString{ValueString: values[1]}}}})
			} else if checkType(values[1]) == INT {
				arr.Value = append(arr.Value, &Value{Value: &Value_ValueKeyValue{ValueKeyValue: &KeyValue{Key: values[0], Value: &KeyValue_ValueInt{ValueInt: int32(Atoi(values[1]))}}}})
			} else if checkType(values[1]) == FLOAT {
				f, err := strconv.ParseFloat(values[1], 32)
				if err != nil {
					return true, err
				}
				arr.Value = append(arr.Value, &Value{Value: &Value_ValueKeyValue{ValueKeyValue: &KeyValue{Key: values[0], Value: &KeyValue_ValueFloat{ValueFloat: float32(f)}}}})
			}
		} else {
			// Example: 'Arr: x, y+56'
			for _, v := range values {
				if strings.Contains(v, "+") || strings.Contains(v, "-") {
					matches := regexSearch(`(\w+)([+|-])(\d+)`, v)
					if len(matches) > 0 {
						index := c.constantExists(matches[0])
						if index != -1 {
							variable := c.Intern.constants.Variable[index]
							if variable.Value != nil {
								newValue := calculateOperation(int(variable.GetValueInt()), matches[1], Atoi(matches[2]))
								arr.Value = append(arr.Value, &Value{Value: &Value_ValueInt{ValueInt: int32(newValue)}})
								currentArrayValues = append(currentArrayValues, int(newValue))
							}
						}
					}
				} else {
					// Example: 'Arr: 5, 6'
					// Example: 'Arr: X, Y'
					if checkType(v) == INT {
						arr.Value = append(arr.Value, &Value{Value: &Value_ValueInt{ValueInt: int32(Atoi(v))}})
						currentArrayValues = append(currentArrayValues, Atoi(v))
					} else if checkType(v) == FLOAT {
						f, err := strconv.ParseFloat(v, 32)
						if err != nil {
							return true, err
						}
						arr.Value = append(arr.Value, &Value{Value: &Value_ValueFloat{ValueFloat: float32(f)}})
					} else {
						i := c.constantExists(v)
						if i != -1 {
							variable := c.GetVariableFromConstants(v)
							if variable.Value != nil {
								if _, ok := variable.GetValue().(*Variable_ValueInt); ok {
									arr.Value = append(arr.Value, &Value{Value: &Value_ValueInt{ValueInt: variable.GetValueInt()}})
									currentArrayValues = append(currentArrayValues, int(variable.GetValueInt()))
								} else if _, ok := variable.GetValue().(*Variable_ValueFloat); ok {
									arr.Value = append(arr.Value, &Value{Value: &Value_ValueFloat{ValueFloat: variable.GetValueFloat()}})
								} else {
									arr.Value = append(arr.Value, &Value{Value: &Value_ValueString{ValueString: variable.GetValueString()}})
								}
							}
						} else {
							arr.Value = append(arr.Value, &Value{Value: &Value_ValueString{ValueString: v}})
						}
					}
				}
			}
			c.Intern.variableNumbersArray[name] = currentArrayValues
		}
		return true, nil
	}
	return false, nil
}

// Var: 10-Arr[0], 20+Arr[1]
func (c *Cod) handleArrayWithReference(line string) (bool, error) {
	line = strings.ReplaceAll(line, " ", "")
	if strings.Contains(line, "[") {
		if matches := regexSearch(`(\w+):(.+)`, line); len(matches) > 0 {
			name := matches[0]
			variable := c.CreateOrReuseVariable(name)
			if c.ExistsInCurrentObject(name) == -1 {
				variable.Value = &Variable_ValueArray{}
				variable.Value.(*Variable_ValueArray).ValueArray = &ArrayValue{}
			}
			variable.Name = name
			values := matches[1]
			splitValues := strings.Split(values, ",")
			for _, v := range splitValues {
				tokens := regexSearch(`((\d+)([+|-])(\w+)\[(\d+)\])`, v)
				if len(tokens) > 0 {
					index := c.ExistsInCurrentObject(tokens[3])
					if index != -1 {
						arrayValue := int(c.Intern.currentObject.Variables.Variable[index].GetValueArray().Value[Atoi(tokens[4])].GetValueInt())
						newValue := calculateOperation(Atoi(tokens[1]), tokens[2], arrayValue)
						if variable.Value.(*Variable_ValueArray).ValueArray.Value == nil {
							variable.Value.(*Variable_ValueArray).ValueArray.Value = []*Value{}
						}
						variable.Value.(*Variable_ValueArray).ValueArray.Value = append(variable.Value.(*Variable_ValueArray).ValueArray.Value, &Value{Value: &Value_ValueInt{ValueInt: int32(newValue)}})
					}
				}
			}
		}
		return true, nil
	}
	return false, nil
}
