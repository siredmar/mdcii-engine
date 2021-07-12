/*
Copyright © 2021 Armin Schlegel armin.schlegel@gmx.de

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
*/
package cmd

import (
	"fmt"
	"log"
	"os"
	"runtime"
	"strconv"
	"sync"

	"github.com/siredmar/mdcii-engine/pkg/bsh"
	"github.com/siredmar/mdcii-engine/pkg/palette"
	"github.com/spf13/cobra"
)

var (
	bshFile      string
	palFile      string
	index        string
	outputFolder string
)

var rootCmd = &cobra.Command{
	Use:   "bshdump",
	Short: "Dumps one or all bsh files to png files.",
	Long:  `Dumps one or all bsh files to png files.`,
	Run: func(cmd *cobra.Command, args []string) {
		if _, err := os.Stat(outputFolder); os.IsNotExist(err) {
			log.Fatal(err)
		}

		palette, err := palette.NewPalette(palFile)
		if err != nil {
			log.Fatal(err)
		}
		b, err := bsh.NewPng(bshFile, palette)
		if err != nil {
			log.Fatal(err)
		}
		if index != "all" {
			i, err := strconv.Atoi(index)
			if err != nil {
				log.Fatal(err)
			}
			err = b.Convert(i, fmt.Sprintf("%s/%d.png", outputFolder, i))
			if err != nil {
				log.Fatal(err)
			}
		} else {
			numCpu := runtime.NumCPU() / 2
			numberOfImages := len(b.Bsh)
			numberPerRoutine := numberOfImages / numCpu
			var wg sync.WaitGroup
			for c := 0; c < numCpu; c++ {
				wg.Add(1)
				go func(start int, stop int, c int, wg *sync.WaitGroup) {
					for i := range b.Bsh[start:stop] {
						err = b.Convert(i+c*numberPerRoutine, fmt.Sprintf("%s/%d.png", outputFolder, i+c*numberPerRoutine))
						if err != nil {
							log.Fatal(err)
						}
					}
					wg.Done()
				}(c*numberPerRoutine, (c+1)*numberPerRoutine, c, &wg)
			}
			wg.Wait()
		}
	},
}

func Execute() {
	if err := rootCmd.Execute(); err != nil {
		fmt.Println(err)
		os.Exit(1)
	}
}

func init() {
	rootCmd.Flags().StringVarP(&outputFolder, "output", "o", ".", "Output folder for images (default: .)")
	rootCmd.Flags().StringVarP(&bshFile, "bsh", "b", "stadtfld.bsh", "BSH file input")
	rootCmd.Flags().StringVarP(&palFile, "pal", "p", "stadtfld.col", "palette file input")
	rootCmd.Flags().StringVarP(&index, "index", "i", "all", "index (default: all)")
}
