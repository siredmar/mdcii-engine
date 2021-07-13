
NAME = mdcii
BIN_DIR ?= ./bin
VERSION ?= $(shell git describe --match=NeVeRmAtCh --always --abbrev=40 --dirty)
GO_LDFLAGS = -tags 'netgo osusergo static_build'
GO_ARCH = amd64

all: check test build

bshdump:
	GOOS=linux GOARCH=${GO_ARCH} go build -o ${BIN_DIR}/bshdump cmd/bshdump/main.go
	GOOS=windows GOARCH=${GO_ARCH} go build -o ${BIN_DIR}/bshdump.exe cmd/bshdump/main.go

# proto-cod:
# 	cd ../ && ./dobi.sh generate-client-sources

# build:
# 	GOOS=linux GOARCH=${GO_ARCH} go build $(GO_LDFLAGS) -o ${BIN_DIR}/${NAME} main.go
# 	GOOS=windows GOARCH=${GO_ARCH} go build $(GO_LDFLAGS) -o ${BIN_DIR}/${NAME}.exe main.go

test:
	go test ./...

clean:
	rm -rf ${BIN_DIR}/bshdump ${BIN_DIR}/bshdump.exe

.PHONY: check test clean

