package gam

import (
	"os"

	chunks "github.com/siredmar/mdcii-engine/pkg/chunks"
	"github.com/siredmar/mdcii-engine/pkg/cod/buildings"
	files "github.com/siredmar/mdcii-engine/pkg/files"
)

type Island struct {
	Island5     chunks.Island5
	ButtomLayer chunks.IslandHouse
	TopLayer    chunks.IslandHouse
}

type GamParser struct {
	Data     []byte
	Files    *files.Files
	Chunks   []*chunks.Chunk
	Islands5 []*chunks.Island5 // INSEL5

}

func NewParser() (*GamParser, error) {
	gamParser := &GamParser{}
	gamParser.Chunks = make([]*chunks.Chunk, 0)
	gamParser.Islands5 = make([]*chunks.Island5, 0)
	return gamParser, nil
}

func (p *GamParser) LoadPath(gamPath string) error {
	p.Files = files.Instance()
	path, err := p.Files.FindPathForFile(gamPath)
	if err != nil {
		return err
	}

	p.Data, err = os.ReadFile(path)
	if err != nil {
		return err
	}
	return nil
}

func (p *GamParser) LoadData(data []byte) error {
	p.Data = data
	return nil
}

func (p *GamParser) Parse(b *buildings.Buildings) error {
	chunksList, err := chunks.ParseChunks(p.Data)
	if err != nil {
		return err
	}

	for _, chunk := range chunksList {
		switch chunk.Id {
		case "INSEL5":
			i, err := chunks.NewIsland5(chunk, b)
			if err != nil {
				return err
			}
			p.Islands5 = append(p.Islands5, i)
		case "INSEL3":
			i3, err := chunks.NewIsland3(chunk)
			if err != nil {
				return err
			}
			i5 := i3.ToIsland5()
			p.Islands5 = append(p.Islands5, i5)
		case "INSEL4":
			i4, err := chunks.NewIsland4(chunk)
			if err != nil {
				return err
			}
			i5 := i4.ToIsland5()
			p.Islands5 = append(p.Islands5, i5)
		case "INSELHAUS":
			currentIsland5 := p.Islands5[len(p.Islands5)-1]
			i, err := chunks.NewIslandHouse(chunk, chunks.IslandDimensions{
				Width:  currentIsland5.Width,
				Height: currentIsland5.Height,
			}, b)
			if err != nil {
				return err
			}
			p.Islands5[len(p.Islands5)-1].AddIslandHouse(*i)
		}
	}

	for _, island := range p.Islands5 {
		err := island.Finalize()
		if err != nil {
			return err
		}
	}
	return nil
}
