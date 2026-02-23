/*
Copyright © 2023 NAME HERE <EMAIL ADDRESS>

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
	"os"
	"path/filepath"
	"strconv"

	"github.com/siredmar/mdcii-engine/pkg/cod"
	"github.com/siredmar/mdcii-engine/pkg/cod/buildings"
	"github.com/siredmar/mdcii-engine/pkg/files"
	"github.com/siredmar/mdcii-engine/pkg/gam"
	"github.com/spf13/cobra"

	"github.com/spf13/viper"
	// cod "github.com/siredmar/mdcii-engine/pkg/cod"
)

var cfgFile string

var (
	gamePath string
	gamFile  string
)

// rootCmd represents the base command when called without any subcommands
var rootCmd = &cobra.Command{
	Use:   "islandpng",
	Short: "generate ",
	// Uncomment the following line if your bare application
	// has an action associated with it:
	Run: func(cmd *cobra.Command, args []string) {
		absPath, err := filepath.Abs(gamePath)
		if err != nil {
			fmt.Println(err)
			os.Exit(1)
		}

		dirPath := filepath.Dir(absPath)

		files.CreateInstance(dirPath)
		buildingsCodPath, err := files.Instance().FindPathForFile("haeuser.cod")
		if err != nil {
			fmt.Println(err)
			os.Exit(1)
		}
		haeuserCod, err := cod.NewCod(buildingsCodPath, true)
		if err != nil {
			fmt.Println(err)
			os.Exit(1)
		}
		err = haeuserCod.Parse()
		if err != nil {
			fmt.Println(err)
			os.Exit(1)
		}

		buildings, err := buildings.NewBuildings(haeuserCod)
		if err != nil {
			fmt.Println(err)
			os.Exit(1)
		}
		// jsonBytes, err := json.MarshalIndent(buildings.GetBuildings(), "", "    ")
		// if err != nil {
		// 	fmt.Println(err)
		// 	os.Exit(1)
		// }

		gamParser, err := gam.NewParser()
		if err != nil {
			fmt.Println(err)
			os.Exit(1)
		}
		err = gamParser.LoadPath(gamFile)
		if err != nil {
			fmt.Println(err)
			os.Exit(1)
		}
		err = gamParser.Parse(buildings)
		if err != nil {
			fmt.Println(err)
			os.Exit(1)
		}

		uniqueIds := make(map[int]string)
		for _, island5 := range gamParser.Islands5 {
			for _, islandHouse := range island5.Layers.IslandHouse {
				for _, field := range islandHouse.Fields {
					uniqueIds[field.Id] = strconv.Itoa(field.Id)
				}
			}
		}
		cnt := 32
		for key, _ := range uniqueIds {
			uniqueIds[key] = string(rune(cnt))
			cnt++
		}

		for _, island5 := range gamParser.Islands5 {
			for _, islandHouse := range island5.Layers.Final {
				for i, field := range islandHouse.Fields {
					x := i % islandHouse.Size.Width
					// y := i / islandHouse.Size.Width
					switch field.Id {
					case 65535:
						fmt.Printf("  ")
					default:
						fmt.Printf(" %s", uniqueIds[field.Id])
					}
					if x%islandHouse.Size.Width == 0 {
						fmt.Println()
					}
					// fmt.Printf("x: %d, y: %d: ", x, y)
					// fmt.Println(field)
				}
			}
		}
	},
}

// Execute adds all child commands to the root command and sets flags appropriately.
// This is called by main.main(). It only needs to happen once to the rootCmd.
func Execute() {
	if err := rootCmd.Execute(); err != nil {
		fmt.Println(err)
		os.Exit(1)
	}

}

func init() {
	cobra.OnInitialize(initConfig)

	// Cobra also supports local flags, which will only run
	// when this action is called directly.
	// rootCmd.Flags().BoolVarP(&decrypt, "decrypt", "d", false, "decrypt true/false")
	rootCmd.Flags().StringVarP(&gamePath, "path", "p", ".", "Path to game")

	rootCmd.Flags().StringVarP(&gamFile, "gam", "g", "", "gam file path")
}

// initConfig reads in config file and ENV variables if set.
func initConfig() {
	if cfgFile != "" {
		// Use config file from the flag.
		viper.SetConfigFile(cfgFile)
	}

	viper.AutomaticEnv() // read in environment variables that match

	// If a config file is found, read it in.
	if err := viper.ReadInConfig(); err == nil {
		fmt.Println("Using config file:", viper.ConfigFileUsed())
	}
}
