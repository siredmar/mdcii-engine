FROM ubuntu:18.04

RUN apt-get update && \
    apt-get install -y --no-install-recommends \
    build-essential \
    cmake \
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
    libprotobuf-dev \
    libprotobuf-c-dev \
    protobuf-compiler \
    protobuf-c-compiler \
    gcc-8 \
    g++-8 \
    gosu && \
    update-alternatives --install /usr/bin/gcc gcc /usr/bin/gcc-8 60 --slave /usr/bin/g++ g++ /usr/bin/g++-8 && \
    update-alternatives --config gcc && \
    apt-get clean && \
    rm -rf /var/lib/apt/lists/*

WORKDIR /build

COPY docker/entrypoint.sh /usr/local/bin/entrypoint.sh
RUN chmod +x /usr/local/bin/entrypoint.sh

ENTRYPOINT ["/usr/local/bin/entrypoint.sh"]
