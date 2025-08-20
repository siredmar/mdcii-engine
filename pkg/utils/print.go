package utils

import (
	"encoding/json"
	"fmt"
)

func PrettyPrint(in any) {
	b, err := json.MarshalIndent(in, "", "  ")
	if err != nil {
		fmt.Println(err)
	}
	fmt.Println(string(b))
}
