package chunks

import (
	"bytes"
	"io/ioutil"

	"github.com/HewlettPackard/structex"
)

const CHUNK_HEADER_OFFSET uint32 = 20

type chunkRaw struct {
	Id     [16]byte
	Length uint32
	Data   []byte
}

type Chunk struct {
	Id     string
	Length int
	Data   []byte
}

func ParseChunks(data []byte) ([]*Chunk, error) {
	chunks := []*Chunk{}
	currentIndex := 0
	var chunksLength int = 0
	for currentIndex < len(data) {
		c, err := NewChunk(data[currentIndex:])
		if err != nil {
			return nil, err
		}
		chunks = append(chunks, c)
		currentChunkLength := c.Length
		chunksLength += currentChunkLength
		currentIndex += int(CHUNK_HEADER_OFFSET) + chunksLength
	}

	return chunks, nil
}

func ReadChunks(file string) ([]byte, error) {

	b, err := ioutil.ReadFile(file)
	if err != nil {
		return nil, err
	}
	return b, err
}

func NewChunk(data []byte) (*Chunk, error) {
	chRaw := &chunkRaw{}

	if err := structex.Decode(bytes.NewReader(data), chRaw); err != nil {
		return &Chunk{}, err
	}

	d := []byte{}
	for i := CHUNK_HEADER_OFFSET; i < chRaw.Length+CHUNK_HEADER_OFFSET; i++ {
		d = append(d, data[i])
	}

	ch := &Chunk{
		Id: func(c []byte) string {
			n := -1
			for i, b := range c {
				if b == 0 {
					break
				}
				n = i
			}
			return string(c[:n+1])
		}(chRaw.Id[:]),
		Length: int(chRaw.Length),
		Data:   d,
	}
	return ch, nil
}
