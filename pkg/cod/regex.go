package cod

import (
	"regexp"
	"strings"
)

// regexSearch gets a regular expressions and a string as input and returns all matches as slice of strings
func regexSearch(regex string, input string) []string {
	re, err := regexp.Compile(regex)
	if err != nil {
		return []string{}
	}

	matches := re.FindStringSubmatch(input)
	if len(matches) > 0 {
		return matches[1:]
	}
	return nil
}

func regexMatch(regex string, input string) bool {
	re, err := regexp.Compile(regex)
	if err != nil {
		return false
	}

	return re.MatchString(input)
}

// splitByDelimiter splits a string by a given delimiter and returns a slice of strings
func splitByDelimiter(input string, delimiter string) []string {
	return strings.Split(input, delimiter)
}

// trimLeadingSpaces removes leading spaces from a string
func trimLeadingSpaces(input string) string {
	return strings.TrimSpace(input)
}
