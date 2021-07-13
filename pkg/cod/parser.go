package cod

type Cod struct {
}

func NewCod(file string, decode bool) (*Cod, error) {
	return &Cod{}, nil
}

func (c *Cod) Parse()
