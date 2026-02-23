package cod

// Stack is
type Stack []*ObjectType

func NewStack() *Stack {
	return &Stack{}
}

// IsEmpty: check if stack is empty
func (s *Stack) IsEmpty() bool {
	return len(*s) == 0
}

func (s *Stack) Reset() {
	*s = []*ObjectType{}
}

// Push a new value onto the stack
func (s *Stack) Push(e *ObjectType) {
	*s = append(*s, e) // Simply append the new value to the end of the stack
}

func (s *Stack) Size() int {
	return len(*s)
}

// Remove and return top element of stack. Return false if stack is empty.
func (s *Stack) Pop() (*ObjectType, bool) {
	if s.IsEmpty() {
		return nil, false
	} else {
		index := len(*s) - 1   // Get the index of the top most element.
		element := (*s)[index] // Index into the slice and obtain the element.
		*s = (*s)[:index]      // Remove it from the stack by slicing it off.
		return element, true
	}
}

func (s *Stack) Top() *ObjectType {
	if s.IsEmpty() {
		return nil
	}
	return (*s)[len(*s)-1]
}
