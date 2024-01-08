package cod

import (
	"bytes"
	"strconv"
	"strings"
)

func Atoi(s string) int {
	v, err := strconv.Atoi(s)
	if err != nil {
		panic(err)
	}
	return v
}

func calculateOperation(oldValue int, operation string, op int) int {
	currentValue := oldValue
	if operation == "+" {
		currentValue += op
	} else if operation == "-" {
		currentValue -= op
	} else {
		currentValue = op
	}
	return currentValue
}

// bytesIndex finds the start index for a subslice of slice of bytes
func bytesIndex(b []byte, s []byte) int {
	if len(s) > len(b) {
		return -1
	}

	for i := 0; i <= len(b)-len(s); i++ {
		if bytes.Equal(b[i:i+len(s)], s) {
			return i
		}
	}

	return -1
}

type LineType struct {
	Line   string
	Spaces int
}

// readLines reads all lines from a file passed as slice of bytes and returns slice of strings.
// The end of line is marked with an '\r' followed by an '\n'.
// Lines beginning with `;' are ignored
func readLines(b []byte) ([]LineType, error) {
	var lines []LineType
	// remove all occurances of '\r' from the byte slice
	b = bytes.ReplaceAll(b, []byte{'\r'}, []byte{})

	for {
		idx := bytesIndex(b, []byte{'\n'})
		if idx == -1 {
			break
		}

		line := string(b[:idx])
		// trimmed := strings.TrimSpace(line)
		line = strings.ReplaceAll(line, "\t", "  ")
		trimmed := strings.ReplaceAll(line, " ", "")
		// check if line as a comment suffix like 'foo ;comment', then trim of the `;comment` part
		if matches := regexSearch(`(.*);(.*)`, trimmed); len(matches) > 0 {
			trimmed = matches[0]

		}

		if len(trimmed) > 0 && !strings.HasPrefix(trimmed, ";") {
			lines = append(lines, LineType{
				Line:   trimmed,
				Spaces: countFrontSpaces(line),
			})
		}

		b = b[idx+1:]
	}

	return lines, nil
}

// coundFrontSpaces counts the number of leading spaces in a string.
func countFrontSpaces(s string) int {
	var count int
	for _, r := range s {
		if r != ' ' {
			break
		}
		count++
	}
	return count
}
