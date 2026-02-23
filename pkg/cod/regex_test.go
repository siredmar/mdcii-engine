package cod

import (
	fmt "fmt"
	"strconv"
	"testing"

	"github.com/stretchr/testify/assert"
)

func TestRegexSearchConstantAssignments(t *testing.T) {
	assert := assert.New(t)

	matches := regexSearch(`(@?)(\w+)\s*=\s*((?:\d+|\+|\w+)+)`, "HAUSWACHS = Nummer")
	assert.Equal(matches[1], "HAUSWACHS")
	assert.Equal(matches[2], "Nummer")
}

func TestRegexSearchReferenceIncrement(t *testing.T) {
	assert := assert.New(t)
	offsets := []int{}
	matches := regexSearch(`@(\w+):.*(,)`, "@Pos:       +0, +42")
	assert.Equal(matches[0], "Pos")
	matches = regexSearch(`:\s*(.*)`, "@Pos:       +0, +42")
	assert.Equal(matches[0], "+0, +42")
	tokens := splitByDelimiter(matches[0], ",")
	for _, e := range tokens {
		e = trimLeadingSpaces(e)
		number := regexSearch(`([+|-]?)(\d+)`, e)
		offset, err := strconv.Atoi(number[1])
		assert.Nil(err)
		if number[0] == "-" {
			offset *= -1
		}
		offsets = append(offsets, offset)
	}
	assert.Equal(offsets, []int{0, 42})
}

func TestRegularExpression(t *testing.T) {

	// finds matching groups in string by regular expression
	// assert := assert.New(t)
	matches := regexSearch(`(\w+)\s*=\s*(\d+)`, "name = 123")
	fmt.Println(matches)
	// assert.Equal(matches, []string{"name", "value"})
}
