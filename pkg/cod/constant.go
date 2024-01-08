package cod

import (
	fmt "fmt"
	"strconv"
	"strings"

	valid "github.com/asaskevich/govalidator"
)

const (
	NUMBER_INCREMENT = "Nummer"
)

type constantType struct {
	key   string
	value string
}

// constantExists returns the index of the constant with the given key.
// returns -1 if not found.
func (c *Cod) constantExists(key string) int {
	for i, constant := range c.Intern.constants.Variable {
		if constant.Name == key {
			return i
		}
	}
	return -1
}

func (c *Cod) handleConstants(line string) (bool, error) {
	line = strings.ReplaceAll(line, " ", "")
	matches := regexSearch(`(@?)(\w+)\s*=\s*((?:\.+\d+|\+|\w+)+)`, line)
	if len(matches) > 0 {
		isMath := strings.Contains(matches[2], "+")
		constant := constantType{
			key:   matches[1],
			value: matches[2],
		}
		// HAUSWACHS = Nummer
		if constant.value == NUMBER_INCREMENT {
			if number, ok := c.Intern.variableNumbers[NUMBER_INCREMENT]; ok {
				constant.value = strconv.Itoa(number)
				i := c.constantExists(constant.key)
				variable := &Variable{
					Name: constant.key,
					Value: &Variable_ValueInt{
						ValueInt: int32(number),
					},
				}
				if i != -1 {
					c.Intern.constants.Variable[i] = variable
				} else {
					c.Intern.constants.Variable = append(c.Intern.constants.Variable, variable)
				}
			}
		} else {
			// VARIABLEA = 5000
			// Nummer = 1000
			// FOO = MYSTRING
			// FOO = 3.14
			// FOO = BAR+100
			i := c.constantExists(constant.key)
			variable, err := c.getValue(constant.key, constant.value, isMath)
			if err != nil {
				return true, err
			}
			if i != -1 {
				c.Intern.constants.Variable[i] = variable
			} else {
				c.Intern.constants.Variable = append(c.Intern.constants.Variable, variable)
			}
		}
		return true, nil
	}

	return false, fmt.Errorf("line no constant assignment")
}

// constantNumberAssignment checks if the constant in constant.key already exists
// if it exits, overwrite with current `Nummer` value
// if it doesn't exist create it wiht the current `Nummer` value
func (c *Cod) constantNumberAssignment(constant *constantType) {
	// example: 'HAUSWACHS = Nummer'
	// if constant.value == NUMBER_INCREMENT {
	if _, ok := c.Intern.variableNumbers[NUMBER_INCREMENT]; ok {
		number := c.Intern.variableNumbers[NUMBER_INCREMENT]
		i := c.constantExists(constant.key)
		if i != -1 {
			variable := c.Intern.constants.Variable[i]
			variable.Name = constant.key
			variable.Value = &Variable_ValueString{
				ValueString: fmt.Sprintf("%d", number),
			}
		} else {
			variable := &Variable{}
			variable.Name = constant.key
			variable.Value = &Variable_ValueString{
				ValueString: fmt.Sprintf("%d", number),
			}
			c.Intern.constants.Variable = append(c.Intern.constants.Variable, variable)
		}
	}
	// }
}

// // constantAssignment checks if the constant in constant.key already exists
// // if it exits, overwrite with the provided value
// // if it doesn't exist create it with the provided value
// func (c *Cod) constantAssignment(constant *constantType) {
// 	// // contant assignments, examples:
// 	// // KEY = Nummer
// 	// // KEY =   VALUE
// 	// // FOO = BAR+123
// 	// if matches := regexSearch(`(\w+)\s*=\s*((?:\d+|\+|\w+)+)`, line); len(matches) > 0 {
// 	// 	c.constantAssignment(&constantType{
// 	// 		key:   matches[1],
// 	// 		value: matches[2],
// 	// 	})
// 	// 	continue
// 	// }

// 	// KEY = Nummer
// 	// KEY =   VALUE
// 	// FOO = BAR+123

// 	// example: 'FOO =   12345'
// 	i := c.constantExists(constant.key)
// 	if i != -1 {
// 		// TODO
// 	} else {
// 		// TODO
// 	}
// }

// func (c *Cod) constantIncrementAssignment() {

// }

func (c *Cod) getValue(key string, value string, isMath bool) (*Variable, error) {
	ret := &Variable{
		Name:  key,
		Value: nil,
	}

	if isMath {
		// Searching for some characters followed by a + or - sign and some digits.
		// example: VALUE+20
		matches := regexSearch(`(\w+)(\+|\-)(\d+)`, value)
		if len(matches) > 0 {
			key := matches[0]
			if len(matches) > 0 {
				oldValue := Variable{
					Name:  key,
					Value: nil,
				}
				i := c.constantExists(oldValue.Name)
				if i != -1 {
					oldValue.Value = c.Intern.constants.Variable[i].Value
				} else {
					oldValue.Value = &Variable_ValueInt{
						ValueInt: 0,
					}
				}
				switch v := oldValue.Value.(type) {

				// Special case handling for `RUINE_KONTOR_1`
				case *Variable_ValueString:
					if v.ValueString == "RUINE_KONTOR_1" {
						ret.Value = &Variable_ValueInt{
							ValueInt: 424242,
						}
						return ret, nil
					}
				}
				if matches[1] == "+" {
					switch v := oldValue.Value.(type) {
					case *Variable_ValueInt:
						i, err := strconv.Atoi(matches[2])
						if err != nil {
							return ret, err
						}
						ret.Value = &Variable_ValueInt{
							ValueInt: v.ValueInt + int32(i),
						}
						return ret, nil
					case *Variable_ValueFloat:
						i, err := strconv.ParseFloat(matches[2], 64)
						if err != nil {
							return ret, err
						}
						ret.Value = &Variable_ValueFloat{
							ValueFloat: v.ValueFloat + float32(i),
						}
						return ret, nil
					case *Variable_ValueString:
						i, err := strconv.Atoi(matches[2])
						if err != nil {
							return ret, err
						}
						old, err := strconv.Atoi(v.ValueString)
						if err != nil {
							return ret, err
						}
						ret.Value = &Variable_ValueInt{
							ValueInt: int32(old) + int32(i),
						}
						return ret, nil
					}
				}
				if matches[1] == "-" {
					switch v := oldValue.Value.(type) {
					case *Variable_ValueInt:
						i, err := strconv.Atoi(matches[2])
						if err != nil {
							return ret, err
						}
						ret.Value = &Variable_ValueInt{
							ValueInt: v.ValueInt - int32(i),
						}
						return ret, nil
					case *Variable_ValueFloat:
						i, err := strconv.ParseFloat(matches[2], 64)
						if err != nil {
							return ret, err
						}
						ret.Value = &Variable_ValueFloat{
							ValueFloat: v.ValueFloat - float32(i),
						}
						return ret, nil
					case *Variable_ValueString:
						i, err := strconv.Atoi(matches[2])
						if err != nil {
							return ret, err
						}
						old, err := strconv.Atoi(v.ValueString)
						if err != nil {
							return ret, err
						}
						ret.Value = &Variable_ValueInt{
							ValueInt: int32(old) - int32(i),
						}
						return ret, nil
					}
				}
			}
		}
	}

	{
		// Handle ints as value
		match := valid.IsInt(value)
		if match {
			value, err := strconv.Atoi(value)
			if err != nil {
				return ret, err
			}
			ret.Value = &Variable_ValueInt{

				ValueInt: int32(value),
			}
			return ret, nil
		}
	}

	{
		// Handls floats as value
		match := valid.IsFloat(value)
		if match {
			value, err := strconv.ParseFloat(value, 32)
			if err != nil {
				return ret, err
			}
			ret.Value = &Variable_ValueFloat{

				ValueFloat: float32(value),
			}
			return ret, nil
		}
	}

	{
		// Handls non-empty strings as value
		result := regexSearch("([A-Za-z0-9_]+)", value)
		if len(result) > 0 {
			i := c.constantExists(result[0])
			if i != -1 {
				oldValue := c.Intern.constants.Variable[i]
				ret.Name = key
				switch v := oldValue.Value.(type) {
				case *Variable_ValueInt:
					ret.Value = &Variable_ValueInt{
						ValueInt: v.ValueInt,
					}
				case *Variable_ValueFloat:
					ret.Value = &Variable_ValueFloat{
						ValueFloat: v.ValueFloat,
					}
				case *Variable_ValueString:
					ret.Value = &Variable_ValueString{
						ValueString: v.ValueString,
					}
				}
			} else {
				ret.Value = &Variable_ValueString{
					ValueString: value,
				}
			}
			return ret, nil
		}
	}

	return nil, fmt.Errorf("Not a valid constant assignment: %s = %s", key, value)
}
