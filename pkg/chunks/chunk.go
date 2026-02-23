package chunks

import (
	"errors"
	"io"
	"os"

	"github.com/ghostiam/binstruct"
)

func (d *chunkRaw) NullTerminatedString(r binstruct.Reader) (string, error) {
	var b []byte

	var readCount int32
	for {
		readByte, err := r.ReadByte()
		if errors.Is(err, io.EOF) {
			break
		}
		if err != nil {
			return "", err
		}

		readCount++

		if readByte == 0x00 {
			break
		}

		b = append(b, readByte)
	}

	// d.IdLen -= readCount

	return string(b), nil
}

const CHUNK_HEADER_OFFSET uint32 = 20

type chunkRaw struct {
	Id     []uint8 `bin:"len:16"`
	Length uint32  `bin:"len:4"`
	Data   []uint8 `bin:"len:Length"`
}

type Chunk struct {
	Id     string
	Length int
	Data   []byte
}

func ParseChunks(data []byte) ([]*Chunk, error) {
	var chunks = []*Chunk{}
	currentIndex := 0
	// var chunksLength int = 0
	for currentIndex < len(data) {
		c, err := NewChunk(data[currentIndex:])
		if err != nil {
			return nil, err
		}
		chunks = append(chunks, c)
		// currentChunkLength := c.Length
		// chunksLength += currentChunkLength
		currentIndex += int(CHUNK_HEADER_OFFSET) + c.Length
	}

	return chunks, nil
}

func NewChunksFromFile(path string) ([]*Chunk, error) {
	b, err := os.ReadFile(path)
	if err != nil {
		return nil, err
	}
	return ParseChunks(b)
}

func NewChunk(data []byte) (*Chunk, error) {
	chRaw := chunkRaw{}

	err := binstruct.UnmarshalLE(data, &chRaw)
	if err != nil {
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

func FindChunkById(chunks []*Chunk, id string) (*Chunk, error) {
	for _, chunk := range chunks {
		if chunk.Id == id {
			return chunk, nil
		}
	}
	return nil, errors.New("chunk not found")
}
