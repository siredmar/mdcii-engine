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
	"strings"

	"github.com/spf13/cobra"

	"github.com/spf13/viper"

	"github.com/siredmar/mdcii-engine/pkg/cod/buildings"
	atlas "github.com/siredmar/mdcii-engine/pkg/texture/atlas"
)

var cfgFile string

var (
	outputDir     string
	inputDir      string
	buildingsPath string
)

// rootCmd represents the base command when called without any subcommands
var rootCmd = &cobra.Command{
	Use: "atlas",
	Run: func(cmd *cobra.Command, args []string) {
		b, err := buildings.LazyImport(buildingsPath)
		if err != nil {
			fmt.Println(err)
			os.Exit(1)
		}
		m := buildings.GetObjectKindGfxMap(b)
		fmt.Println(m)

		// filenames := []string{"1.png", "2.png", "3.png"} // Add your filenames here
		files, err := os.ReadDir(inputDir)
		if err != nil {
			fmt.Println(err)
			os.Exit(1)
		}
		atlasWidth := 4096
		atlasHeight := 4096
		filenames := []string{}
		for _, file := range files {
			if !file.IsDir() {
				if strings.HasSuffix(file.Name(), ".png") {
					filenames = append(filenames, fmt.Sprintf("%s/%s", inputDir, file.Name()))
				}
			}
		}
		fmt.Println(filenames)

		atlas, err := atlas.CreateTextureAtlas(atlasWidth, atlasHeight, atlas.WithSkipFileEnding(), atlas.WithName("texture-atlas"), atlas.WithFiles(filenames))
		if err != nil {
			fmt.Println("Error:", err)
			return
		}

		if err := atlas.Export(); err != nil {
			fmt.Println("Error exporting texture atlas:", err)
			return
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
	rootCmd.Flags().StringVarP(&inputDir, "dir", "d", ".", "Input directory")
	rootCmd.Flags().StringVarP(&outputDir, "output", "o", ".", "Output directory")
	rootCmd.Flags().StringVarP(&buildingsPath, "buildings", "b", "", "Path to buildings file")
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
