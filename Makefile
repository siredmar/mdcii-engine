NAME = mdcii
BIN_DIR ?= ./bin
VERSION ?= $(shell git describe --match=NeVeRmAtCh --always --abbrev=40 --dirty)
GO_LDFLAGS = -tags 'netgo osusergo static_build'
GO_ARCH = amd64

all: proto-cod build

build: bshdump

bshdump: test
	GOOS=linux GOARCH=${GO_ARCH} go build -o ${BIN_DIR}/bshdump cmd/bshdump/main.go

proto-cod:
	@go install google.golang.org/protobuf/cmd/protoc-gen-go@v1.32.0
	@mkdir -p tools/protoc
	curl -L https://github.com/protocolbuffers/protobuf/releases/download/v25.1/protoc-25.1-linux-x86_64.zip -o tools/protoc.zip
	@unzip -o tools/protoc.zip -d tools/protoc/
	tools/protoc/bin/protoc --proto_path=proto --go_out=pkg/cod proto/cod.proto
	@mv pkg/cod/github.com/siredmar/mdcii-engine/pkg/cod/cod.pb.go pkg/cod
	@rm -r pkg/cod/github.com/

test:
	go test ./...

clean:
	rm -rf ${BIN_DIR}/bshdump

.PHONY: check test clean

