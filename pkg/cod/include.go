package cod

import (
	"os"
	"slices"
	"strings"

	"github.com/siredmar/mdcii-engine/pkg/files"
)

func (c *Cod) handleInclude(line string, linenumber int) (bool, error) {
	line = strings.ReplaceAll(line, " ", "")
	matches := regexSearch(`^(Include):\s*\"([\a-zA-Z\-_.]+)\"$`, line)
	if len(matches) > 0 {
		file := matches[1]
		path, err := files.Instance().FindPathForFile(file)
		if err != nil {
			return false, err
		}
		b, err := os.ReadFile(path)
		if err != nil {
			return false, err
		}

		lines, err := readLines(b)
		if err != nil {
			return false, err
		}
		c.Lines = slices.Insert(c.Lines, linenumber, lines...)
		return true, nil
	}
	return false, nil
}
