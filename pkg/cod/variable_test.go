package cod

// import (
// 	fmt "fmt"
// 	"regexp"
// 	"strings"
// 	"testing"

// 	"github.com/stretchr/testify/assert"
// )

// func TestVariableRegex(t *testing.T) {
// 	assert := assert.New(t)
// 	if strings.Contains(line, "Objekt") {
// 		return false, nil
// 	}
// 	re := regexp.MustCompile(`\w+:\w+`)
// 	strs := []string{
// 		"vara:123",
// 		"Nummer:23",
// 		"ObjFill:234",
// 		"asdf:234",
// 		"asdf:SDFGG",
// 		"Objekt:234",
// 		"Vara:VARIABLE",
// 	}

// 	for _, str := range strs {
// 		matches := re.FindStringSubmatch(str)
// 		if matches != nil {
// 			fmt.Println(matches[0])
// 		}
// 	}
// 	assert.True(true)
// }
