package config

import (
	"fmt"
	"os"
	"path/filepath"

	"gopkg.in/yaml.v3"
)

type Config struct {
	MousePanFactor float64 `yaml:"mouse_pan_factor"`
	Renderer       string  `yaml:"renderer"`
}

var instance *Config
var configFilePath string

func DefaultConfig() *Config {
	return &Config{
		MousePanFactor: 0.67,
		Renderer:       "2d",
	}
}

func defaultConfigPath() string {
	home, err := os.UserHomeDir()
	if err != nil {
		return "config.yaml"
	}
	return filepath.Join(home, ".mdcii", "config.yaml")
}

// LoadFromPath loads config from specified path, with fallback logic:
// 1. If path is specified and exists, use it
// 2. If path is specified but doesn't exist, try ~/.mdcii/config.yaml
// 3. If ~/.mdcii/config.yaml doesn't exist, try ./config.yaml in cwd
// 4. If none exist, create default at ~/.mdcii/config.yaml
func LoadFromPath(path string) (*Config, error) {
	// Determine which path to use
	pathsToTry := []string{}
	if path != "" {
		pathsToTry = append(pathsToTry, path)
	}
	pathsToTry = append(pathsToTry, defaultConfigPath())
	pathsToTry = append(pathsToTry, "config.yaml")

	var loadedPath string
	var data []byte
	var err error

	for _, p := range pathsToTry {
		data, err = os.ReadFile(p)
		if err == nil {
			loadedPath = p
			break
		}
		if !os.IsNotExist(err) {
			return nil, fmt.Errorf("failed to read config file %s: %w", p, err)
		}
	}

	if loadedPath == "" {
		// None exist, create default at ~/.mdcii/config.yaml
		cfg := DefaultConfig()
		configFilePath = defaultConfigPath()
		if err := cfg.Save(); err != nil {
			return nil, fmt.Errorf("failed to create default config: %w", err)
		}
		instance = cfg
		return cfg, nil
	}

	cfg := &Config{}
	if err := yaml.Unmarshal(data, cfg); err != nil {
		return nil, fmt.Errorf("failed to parse config file %s: %w", loadedPath, err)
	}

	configFilePath = loadedPath
	instance = cfg
	return cfg, nil
}

func (c *Config) Save() error {
	path := configFilePath
	if path == "" {
		path = defaultConfigPath()
	}

	if err := os.MkdirAll(filepath.Dir(path), 0755); err != nil {
		return err
	}

	data, err := yaml.Marshal(c)
	if err != nil {
		return err
	}

	return os.WriteFile(path, data, 0644)
}

func Instance() *Config {
	if instance == nil {
		cfg, err := LoadFromPath("")
		if err != nil {
			fmt.Fprintf(os.Stderr, "Error loading config: %v\n", err)
			os.Exit(1)
		}
		instance = cfg
	}
	return instance
}
