package errors

// Assume MDCII_ASSERT is defined somewhere in your codebase
func MDCII_ASSERT(condition bool, message string) {
	if !condition {
		panic(message)
	}
}
