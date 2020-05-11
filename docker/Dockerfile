FROM ubuntu:18.04

RUN apt update && \
    apt install -y \
    build-essential \
    cmake \
    libprotobuf-dev \
    libsdl2-dev \
    libsdl2-image-2.0-0 \
    libsdl2-image-dev \
    libsdl2-ttf-2.0-0 \
    libsdl2-ttf-dev \
    libboost-system1.65.1 \
    libboost-system1.65-dev \
    libboost-regex1.65.1 \
    libboost-regex1.65-dev \
    libboost-program-options1.65.1 \
    libboost-program-options1.65-dev \
    libboost-iostreams1.65.1 \
    libboost-iostreams1.65-dev \
    libboost-filesystem1.65.1 \
    libboost-filesystem1.65-dev \
    libprotobuf-c-dev \
    protobuf-compiler \
    protobuf-c-compiler \
    gosu && \
    apt clean

WORKDIR /build

COPY entrypoint.sh /usr/local/bin/entrypoint.sh
RUN chmod +x /usr/local/bin/entrypoint.sh

ENTRYPOINT ["/usr/local/bin/entrypoint.sh"]
