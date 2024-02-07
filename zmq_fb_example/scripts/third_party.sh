#!/bin/bash

: '
This file is used to install the dependencies for the GreenFunction example.
'

# Get the absolute path to this file
ROOT_SCRIPT_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" >/dev/null 2>&1 && pwd )"

# Check if the third-party dependencies folder exists
if [ ! -d "$ROOT_SCRIPT_DIR/../third_party" ]; then
    mkdir "$ROOT_SCRIPT_DIR/../third_party"
else
    echo "${YELLOW}Third-party dependencies folder already exists${NORMAL}"
    echo "${YELLOW}If you want to reinstall the dependencies, please delete the folder${NORMAL}"
fi

# Download, compile, and install the third-party dependencies
cd "$ROOT_SCRIPT_DIR/../third_party"

# libzmq
if [ ! -d "libzmq" ]; then
    echo "${YELLOW}Downloading libzmq${NORMAL}"
    git clone --branch v4.3.5 --depth 1 https://github.com/zeromq/libzmq.git
    cd libzmq
    echo "${YELLOW}Compiling libzmq${NORMAL}"
    mkdir build
    cd build
    cmake -DCMAKE_BUILD_TYPE=Release \
          -DBUILD_TESTS=OFF          \
          -DWITH_DOCS=OFF            \
          -DWITH_TLS=OFF             \
          -DZMQBUILD_TESTS=OFF       \
          -DBUILD_SHARED=OFF         \
          -DBUILD_DRAFTS=ON          \
          -DBUILD_STATIC=ON          \
          -DCMAKE_INSTALL_PREFIX=../ \
          ..
    make -j$(nproc)
    echo "${YELLOW}Installing libzmq${NORMAL}"
    make install
    cd ../..
else
    echo "${YELLOW}libzmq already installed${NORMAL}"
fi

# FlatBuffers
if [ ! -d "flatbuffers" ]; then
    echo "${YELLOW}Downloading FlatBuffers${NORMAL}"
    git clone --branch v23.5.26 --depth 1 https://github.com/google/flatbuffers.git
    echo "${YELLOW}Compiling FlatBuffers${NORMAL}"
    cd flatbuffers
    mkdir build
    cd build
    cmake -DCMAKE_BUILD_TYPE=Release \
          -DFLATBUFFERS_BUILD_TESTS=OFF \
          -DCMAKE_INSTALL_PREFIX=../ \
          ..
    make -j$(nproc)
    echo "${YELLOW}Installing FlatBuffers${NORMAL}"
    make install
    cd ../..
else
    echo "${YELLOW}FlatBuffers already installed${NORMAL}"
fi
