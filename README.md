# ppc2cpp
(work in progress) reverse engineering library and tool.

## Requirements
- protobuf

### Basic

```
cmake -B build
```

### Ubuntu 22.04

```
sudo apt install build-essential make cmake protobuf-compiler libprotobuf-dev libz-dev clang libc++-dev libc++abi-dev
CC=clang CXX=clang++ CXXFLAGS=-stdlib=libc++ cmake -B build
```

### Building
```
cmake --build build -j
```
