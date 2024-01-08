package cod

import (
	"testing"

	"github.com/stretchr/testify/assert"
)

func TestVariableRegex(t *testing.T) {
	assert := assert.New(t)

	assert.Equal(checkType("1.0"), FLOAT)
	assert.Equal(checkType("1"), INT)
	assert.Equal(checkType("foo"), STRING)
	assert.Equal(checkType("foo, bar"), KEYVALUE)
	assert.Equal(checkType("foo, 1"), KEYVALUE)
	assert.Equal(checkType("foo, 1.0"), KEYVALUE)
	assert.Equal(checkType("foo, -1.0"), KEYVALUE)
	assert.Equal(checkType("foo, 1.0, 1.0"), STRING)
	assert.True(true)
}
