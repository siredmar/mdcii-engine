# MDCII docker builder image

This image contains everything to build mdcii within a docker environment for ubuntu 18.04.

## Precondition

Clone the repository somewhere

    cd /projects
    git clone https://github.com/siredmar/mdcii-engine
    cd mdcii-engine

## How to build the image

Go to the projects root directory

    cd /projects/mdcii-engine/docker
    docker build --tag mdcii-builder .

## Building mdcii with the builder image

    # define the build directory and thus where to store the compiled artifacts
    MDCII_OUTPUT_DIR=/tmp/mdcii

    # build it!
    cd /projects/mdcii-engine
    docker run --rm -it -v /projects/mdcii-engine:/source -v ${MDCII_OUTPUT_DIR}:/build siredmar/mdcii-builder bash -c "cmake -DCMAKE_BUILD_TYPE=Debug /source && make -j$(nproc)"
