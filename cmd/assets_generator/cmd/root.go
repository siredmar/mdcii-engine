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

	"github.com/spf13/cobra"

	"github.com/siredmar/mdcii-engine/pkg/cod"
	"github.com/siredmar/mdcii-engine/pkg/cod/buildings"
	files "github.com/siredmar/mdcii-engine/pkg/files"
	"github.com/spf13/viper"
)

var cfgFile string

var (
	outputDir     string
	inputDir      string
	buildingsPath string
)

// rootCmd represents the base command when called without any subcommands
var rootCmd = &cobra.Command{
	Use: "assets_generator",
	Run: func(cmd *cobra.Command, args []string) {
		f := files.CreateInstance(inputDir)
		p, err := f.FindPathForFile("haeuser.cod")
		if err != nil {
			fmt.Println(err)
			os.Exit(1)
		}
		fmt.Println(p)
		buildingsCod, err := cod.NewCod(p, true)
		if err != nil {
			fmt.Println(err)
			os.Exit(1)
		}
		err = buildingsCod.Parse()
		if err != nil {
			fmt.Println(err)
			os.Exit(1)
		}

		b, err := buildings.NewBuildings(buildingsCod)
		if err != nil {
			fmt.Println(err)
			os.Exit(1)
		}
		// todo: dump stadtfld mgfx, sgfx and gfx bsh files
		// todo: create texture atlas for each bsh file
		// atlas, err := atlas.CreateTextureAtlas()
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
	rootCmd.Flags().StringVarP(&inputDir, "dir", "d", ".", "Game directory")
	rootCmd.Flags().StringVarP(&outputDir, "output", "o", ".", "Output directory")
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
