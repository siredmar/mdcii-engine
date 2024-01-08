package files

import (
	"errors"
	"io/ioutil"
	"os"
	"path/filepath"
	"strings"
	"sync"
)

type Files struct {
	Path string
	Tree []string
}

var instance *Files
var once sync.Once

func CreateInstance(path string) *Files {
	once.Do(func() {
		tree, err := generateTree(path)
		if err != nil {
			panic(err)
		}

		instance = &Files{
			Path: path,
			Tree: tree,
		}
	})
	return instance
}

func Instance() *Files {
	if instance == nil {
		panic("Files instance not created")
	}
	return instance
}

func StringToLowerCase(str string) string {
	return strings.ToLower(str)
}

func CheckFile(filename string) bool {
	_, err := os.Stat(filename)
	return !os.IsNotExist(err)
}

// func generateTree(path string) ([]string) {
// 	tree []string{}
// 	err := filepath.Walk(path, func(path string, info os.FileInfo, err error) error {
// 		if err != nil {
// 			panic(err)
// 		}
// 		tree = append(tree, path)
// 	})
// 	if err != nil {
// 	 panic(err)
// 	}
// 	return tree
// }

func generateTree(path string) ([]string, error) {
	tree := []string{}
	err := filepath.Walk(path, func(path string, info os.FileInfo, err error) error {
		if err != nil {
			return err
		}
		tree = append(tree, path)
		return nil
	})
	if err != nil {
		return nil, err
	}
	return tree, nil
}

func GetDirectoryFiles(directory string) ([]string, error) {
	files, err := ioutil.ReadDir(directory)
	if err != nil {
		return nil, err
	}
	var fileNames []string
	for _, file := range files {
		fileNames = append(fileNames, file.Name())
	}
	return fileNames, nil
}

func (f *Files) GrepFiles(search string) []string {
	var list []string
	lcaseSearch := StringToLowerCase(search)
	for _, t := range f.Tree {
		treeFile := StringToLowerCase(t)
		if strings.Contains(treeFile, lcaseSearch) {
			list = append(list, t)
		}
	}
	return list
}

func (f *Files) FindPathForFile(file string) (string, error) {
	lcaseFile := strings.ToLower(file)
	// Search for the file as substring in the lowercased directory tree
	for _, t := range f.Tree {
		treeFile := strings.ToLower(t)
		if strings.Contains(treeFile, lcaseFile) {
			return t, nil
		}
	}
	return "", errors.New("[ERR] cannot find file: " + file)
}
