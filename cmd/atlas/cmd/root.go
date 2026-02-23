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

	"github.com/spf13/cobra"

	"github.com/spf13/viper"

	"github.com/siredmar/mdcii-engine/pkg/bsh"
	"github.com/siredmar/mdcii-engine/pkg/cod"
	"github.com/siredmar/mdcii-engine/pkg/files"
	atlas "github.com/siredmar/mdcii-engine/pkg/texture/atlas"

	buildingsCod "github.com/siredmar/mdcii-engine/pkg/cod/buildings"
)

var cfgFile string

var (
	outputDir     string
	gamePath      string
	inputBSH      string
	buildingsPath string
)

// rootCmd represents the base command when called without any subcommands
var rootCmd = &cobra.Command{
	Use: "atlas",
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

		buildings, err := buildingsCod.NewBuildings(haeuserCod)
		if err != nil {
			fmt.Println(err)
			os.Exit(1)
		}

		gfxStadtfldBshPath, err := files.Instance().FindPathForFile("gfx/stadtfld.bsh")
		if err != nil {
			fmt.Println(err)
			os.Exit(1)
		}
		gfxStadtfldBsh, err := bsh.NewPng(bsh.WithFile(gfxStadtfldBshPath), bsh.WithConvertAll())
		if err != nil {
			fmt.Println(err)
			os.Exit(1)
		}

		atlasWidth := 4096
		atlasHeight := 4096
		atlasJsonPath := filepath.Join(outputDir, "texture-atlas.json")

		if _, err := os.Stat(atlasJsonPath); os.IsNotExist(err) {
			fmt.Println("Atlas does not exist, creating new atlas...")
			atlasObj, err := atlas.New(atlasWidth, atlasHeight, buildings, atlas.WithName("texture-atlas"), atlas.WithImages(gfxStadtfldBsh), atlas.WithOutputDir(outputDir))
			if err != nil {
				fmt.Println("Error:", err)
				os.Exit(1)
			}
			if err := atlasObj.Export(); err != nil {
				fmt.Println("Error exporting texture atlas:", err)
				os.Exit(1)
			}
			fmt.Println("Atlas created and exported.")
		} else {
			fmt.Println("Loading existing atlas...")
			loadedAtlas, err := atlas.LoadAtlasFromJSON(atlasJsonPath)
			if err != nil {
				fmt.Println("Error loading atlas:", err)
				os.Exit(1)
			}
			fmt.Printf("Atlas loaded: %s (%dx%d)\n", loadedAtlas.AtlasMeta.Name, loadedAtlas.AtlasMeta.Width, loadedAtlas.AtlasMeta.Height)
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
	// rootCmd.Flags().StringVarP(&inputDir, "dir", "d", ".", "Input directory")
	rootCmd.Flags().StringVarP(&gamePath, "game", "g", ".", "game path")
	rootCmd.Flags().StringVarP(&outputDir, "output", "o", ".", "Output file")
	rootCmd.Flags().StringVarP(&buildingsPath, "buildings", "b", "", "Path to buildings file")
	// rootCmd.Flags().StringVarP(&inputBSH, "bsh", "b", ".", "input BSH")
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
