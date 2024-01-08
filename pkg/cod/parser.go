package cod

import (
	fmt "fmt"
	"strings"
)

func (c *Cod) Parse() error {
	for _, rawLine := range c.Lines {
		spaces := rawLine.Spaces
		line := strings.ReplaceAll(rawLine.Line, " ", "")
		if strings.Contains(line, "Pos:X,Y+56") {
			fmt.Println("found object")
		}
		if strings.Contains(line, "Nahrung:") || strings.Contains(line, "Soldat:") || strings.Contains(line, "Turm:") {
			continue
		}
		// constant assignment, examples:
		// VARIABLEA = 5000
		// VARIABLEB = Nummer
		// Nummer = 1000
		// FOO = BAR+100
		ok, err := c.handleConstants(line)
		if ok {
			if err != nil {
				fmt.Println(err)
				return err
			}
			continue
		}

		// Check relative array assignements before relative assignements
		// relative array assignment, examples:
		// example: '@Pos:       +0, +42'
		ok, err = c.handleVariableRelativeArray(line)
		if ok {
			if err != nil {
				fmt.Println(err)
				return err
			}
			continue
		}

		ok, err = c.handleArray(line)
		if ok {
			if err != nil {
				fmt.Println(err)
				return err
			}
			continue
		}
		// relative assignment, examples:
		// example: '@Pos: +42'
		ok, err = c.handleVariableRelative(line)
		if ok {
			if err != nil {
				fmt.Println(err)
				return err
			}
			continue
		}

		// example: 'Gfx:        GFXBODEN+80'
		ok, err = c.handleVariableWithConstant(line)
		if ok {
			if err != nil {
				fmt.Println(err)
				return err
			}
			continue
		}

		ok, err = c.handleVariable(line, spaces)
		if ok {
			if err != nil {
				fmt.Println(err)
				return err
			}
			continue
		}

		// Objekt: A
		ok, err = c.handleObjects(line, spaces)
		if ok {
			if err != nil {
				fmt.Println(err)
				return err
			}
			continue
		}

		// @Nummer: +1
		ok, err = c.handleRelativeNummerObjects(line, spaces)
		if ok {
			if err != nil {
				fmt.Println(err)
				return err
			}
			continue
		}

		// example: EndObj
		ok, err = c.handleEndObjects(line, spaces)
		if ok {
			if err != nil {
				fmt.Println(err)
				return err
			}
			continue
		}

		// example: ObjFill: 0, MAX
		// example: ObjFill: OBJ
		ok, err = c.handleObjFill(line, spaces)
		if ok {
			if err != nil {
				fmt.Println(err)
				return err
			}
			continue
		}
		// Nummer: 0
		ok, err = c.handleNumberObject(line, spaces)
		if ok {
			if err != nil {
				fmt.Println(err)
				return err
			}
			continue
		}

		// skip lines that we don't care about
		continue
	}
	return nil
}
