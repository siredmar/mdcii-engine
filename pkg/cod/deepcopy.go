package cod

func copyVariable(src *Variable, dst *Variable) {
	dst.Name = src.Name
	if src.Value != nil {
		switch src.Value.(type) {
		case *Variable_ValueInt:
			dst.Value = &Variable_ValueInt{
				ValueInt: src.GetValue().(*Variable_ValueInt).ValueInt,
			}
		case *Variable_ValueString:
			dst.Value = &Variable_ValueString{
				ValueString: src.GetValue().(*Variable_ValueString).ValueString,
			}
		case *Variable_ValueArray:
			dst.Value = &Variable_ValueArray{
				ValueArray: &ArrayValue{
					Value: make([]*Value, 0),
				},
			}
			for _, e := range src.GetValue().(*Variable_ValueArray).ValueArray.Value {
				dst.Value.(*Variable_ValueArray).ValueArray.Value = append(dst.Value.(*Variable_ValueArray).ValueArray.Value, &Value{Value: e.GetValue()})

			}
		case *Variable_ValueFloat:
			dst.Value = &Variable_ValueFloat{
				ValueFloat: src.GetValue().(*Variable_ValueFloat).ValueFloat,
			}
		}
	}
}

func copyVariables(src *Variables, dst *Variables) {
	for _, v := range src.Variable {
		variable := &Variable{}
		copyVariable(v, variable)
		dst.Variable = append(dst.Variable, variable)
	}
}

func copyObject(src *Object, dst *Object) {
	dst.Name = src.Name
	dst.Variables = &Variables{}
	dst.Variables.Variable = make([]*Variable, 0)
	dst.Objects = make([]*Object, 0)

	copyVariables(src.Variables, dst.Variables)

	for _, o := range src.Objects {
		dst.Objects = append(dst.Objects, &Object{})
		copyObject(o, dst.Objects[len(dst.Objects)-1])
	}
}
