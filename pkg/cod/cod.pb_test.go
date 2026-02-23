package cod

import (
	"encoding/json"
	fmt "fmt"
	"testing"

	"github.com/stretchr/testify/assert"
)

func TestCodObjects(t *testing.T) {
	assert := assert.New(t)
	objects := Objects{}
	objects.Object = append(objects.Object, &Object{
		Name: "foo",
		Variables: &Variables{
			Variable: []*Variable{
				&Variable{
					Name: "bar",
					Value: &Variable_ValueInt{
						ValueInt: 1,
					},
				},
			},
		},
		Objects: []*Object{},
	})
	jsonData, err := json.Marshal(objects)
	assert.Nil(err)
	assert.NotEmpty(jsonData)
	fmt.Println(string(jsonData))
}
