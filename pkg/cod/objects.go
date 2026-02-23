package cod

import (
	"bytes"
	"encoding/json"
	"strconv"

	"google.golang.org/protobuf/encoding/protojson"

	"strings"
)

type ObjFillRangeType struct {
	Object    Object
	Start     string
	Stop      string
	Filling   bool
	Stacksize int
}

type ObjectType struct {
	Object       *Object
	NumberObject bool
	Spaces       int
}

func NewObjectType(obj *Object) *ObjectType {
	return &ObjectType{
		Object:       obj,
		NumberObject: false,
		Spaces:       -1,
	}
}

func (c *Cod) handleObjects(line string, spaces int) (bool, error) {
	line = strings.ReplaceAll(line, " ", "")
	matches := regexSearch(`Objekt:\s*([\w,]+)`, line)
	if len(matches) > 0 {
		name := matches[0]

		o := c.CreateOrReuseObject(name, false, spaces, true)
		c.Intern.objectMap[name] = o
		c.Intern.currentObject = o
		return true, nil
	}
	return false, nil
}

// example: @Nummer: +1
func (c *Cod) handleRelativeNummerObjects(line string, spaces int) (bool, error) {
	line = strings.ReplaceAll(line, " ", "")
	matches := regexSearch(`(@)(Nummer):\s*([+|-]?)(\d+)`, line)
	if len(matches) > 0 {
		if c.topIsNumberObject() {
			c.objectFinished()
		}
		name := matches[1]
		operation := matches[2]
		value := Atoi(matches[3])

		currentNumber := c.Intern.variableNumbers[name]
		currentNumber = calculateOperation(currentNumber, operation, value)
		c.Intern.variableNumbers[name] = currentNumber
		objname := strconv.Itoa(currentNumber)

		o := c.createObject(objname, true, spaces, true)
		c.Intern.objectMap[objname] = o
		c.Intern.currentObject = o
		if (name == c.Intern.objFillRange.Stop) || (c.Intern.objectStack.Size() < c.Intern.objFillRange.Stacksize) {
			c.resetObjfillPrefill()
		}
		return true, nil
	}
	return false, nil
}

// example: EndObj
func (c *Cod) handleEndObjects(line string, spaces int) (bool, error) {
	line = strings.ReplaceAll(line, " ", "")
	matches := regexSearch(`(EndObj)`, line)
	if len(matches) > 0 {
		if c.Intern.objectStack.Size() <= c.Intern.objFillRange.Stacksize {
			c.resetObjfillPrefill()
		}
		if c.Intern.objectStack.Size() > 0 {
			if c.Intern.objectStack.Top().Spaces > spaces {
				// finish previous number object
				c.objectFinished()
			}
			c.objectFinished()
		}
		return true, nil
	}
	return false, nil
}

func (c *Cod) handleObjFill(line string, spaces int) (bool, error) {
	line = strings.ReplaceAll(line, " ", "")
	matches := regexSearch(`ObjFill:\s*(\w+)[,]?(\w+)*`, line)
	if len(matches) > 0 {
		// Check if range object fill to insert to objects from start to stop
		// Start and Stop is provided
		// example: ObjFill: 0, MAX
		if matches[1] != "" {
			c.Intern.objFillRange.Start = matches[0]
			c.Intern.objFillRange.Stop = matches[1]
			copiedTo := Object{}
			copyObject(c.Intern.objectStack.Top().Object, &copiedTo)
			c.Intern.objFillRange.Object = copiedTo
			c.Intern.objFillRange.Filling = true
			c.Intern.objFillRange.Stacksize = c.Intern.objectStack.Size()
			c.objectFinished()

			// suspect
			c.Intern.currentObject = c.Intern.objectStack.Top().Object
			c.Intern.currentObject.Objects = c.Intern.currentObject.Objects[:len(c.Intern.currentObject.Objects)-1]
		} else {
			// example: ObjFill: OBJ
			realName := c.GetVariableFromConstants(matches[0])
			numberName := realName.GetValueInt()
			obj := c.GetObjectByName(strconv.Itoa(int(numberName)))
			if obj != nil {
				if obj.Variables != nil {
					for _, v := range obj.Variables.Variable {
						variable := c.CreateOrReuseVariable(v.Name)
						copyVariable(v, variable)
					}
				}
				if len(obj.Objects) > 0 {
					for _, o := range obj.Objects {
						object := c.CreateOrReuseObject(o.Name, false, spaces, false)
						copyObject(o, object)
					}
				}
			}
		}
		return true, nil
	}
	return false, nil
}
func (c *Cod) handleNumberObject(line string, spaces int) (bool, error) {
	line = strings.ReplaceAll(line, " ", "")
	matches := regexSearch(`Nummer:\s*(\w+)`, line)
	if len(matches) > 0 {
		if c.topIsNumberObject() {
			c.objectFinished()
		}
		value := matches[0]
		o := c.createObject(value, true, spaces, true)
		c.Intern.objectMap[value] = o
		c.Intern.currentObject = o
		// if checkType(value) == INT {
		// 	c.Intern.variableNumbers["Nummer"] = Atoi(value)
		// } else {
		// 	constant := c.GetVariableFromConstants(value)
		// 	c.Intern.variableNumbers["Nummer"] = int(constant.GetValueInt())
		// }
		return true, nil
	}
	return false, nil
}

func (c *Cod) getSubObject(obj *Object, name string) *Object {
	for _, o := range obj.Objects {
		if o.Name == name {
			return o
		}
	}
	return nil
}

func (c *Cod) objfillPrefill(obj *Object) *Object {
	name := obj.Name

	copyObject(&c.Intern.objFillRange.Object, obj)
	obj.Name = name
	return obj
}

func (c *Cod) resetObjfillPrefill() {
	c.Intern.objFillRange.Start = ""
	c.Intern.objFillRange.Stop = ""
	c.Intern.objFillRange.Filling = false
	c.Intern.objFillRange.Stacksize = 0
}

func (c *Cod) topIsNumberObject() bool {
	if c.Intern.objectStack.Size() > 0 {
		return c.Intern.objectStack.Top().NumberObject
	}
	return false
}

func (c *Cod) objectFinished() {
	if c.Intern.objectStack.Size() > 0 {
		c.Intern.objectStack.Pop()
	}
	if c.Intern.objectStack.Size() == 0 {
		c.Intern.objectIdMap = map[int]*Object{}
		c.Intern.objectMap = map[string]*Object{}
	}
}

func (c *Cod) addToStack(o *ObjectType) {
	c.Intern.objectStack.Push(o)
}

func (c *Cod) CreateOrReuseObject(name string, numberObject bool, spaces int, addToStack bool) *Object {
	if c.Intern.currentObject != nil {
		if optionalObj := c.getSubObject(c.Intern.currentObject, name); optionalObj != nil {
			obj := &ObjectType{
				Object:       optionalObj,
				NumberObject: numberObject,
				Spaces:       spaces,
			}
			if addToStack {
				c.addToStack(obj)
			}
			return optionalObj
		}
	}
	o := c.createObject(name, numberObject, spaces, addToStack)
	return o
}

func (c *Cod) GetObjectByName(name string) *Object {
	if c.Intern.objectMap[name] != nil {
		return c.Intern.objectMap[name]
	}
	return nil
}

func (c *Cod) GetObjectById(id int) *Object {
	if c.Intern.objectIdMap[id] != nil {
		return c.Intern.objectIdMap[id]
	}
	return nil
}

func (c *Cod) createObject(name string, numberObject bool, spaces int, addToObjectStack bool) *Object {
	ret := &Object{
		Name: name,
	}
	if c.Intern.objectStack.IsEmpty() {
		c.Objects.Object = append(c.Objects.Object, ret)
	} else {
		c.Intern.objectStack.Top().Object.Objects = append(c.Intern.objectStack.Top().Object.Objects, ret)
	}
	obj := NewObjectType(ret)
	obj.NumberObject = numberObject
	obj.Spaces = spaces
	obj.Object.Variables = &Variables{}
	obj.Object.Variables.Variable = make([]*Variable, 0)
	obj.Object.Objects = make([]*Object, 0)
	if c.Intern.objFillRange.Filling && numberObject {
		ret = c.objfillPrefill(ret)
	}
	if addToObjectStack {
		c.addToStack(obj)
	}
	return ret
}

func (c *Cod) DumpObjectsToJSON() (string, error) {
	jsonData, err := protojson.Marshal(&c.Objects)

	buf := new(bytes.Buffer)
	json.Indent(buf, []byte(jsonData), "", "  ")
	if err != nil {
		return "", err
	}
	return buf.String(), nil
}
