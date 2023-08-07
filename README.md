# ppc2cpp
(work in progress) reverse engineering library and tool.

### Requirements
- protobuf
  - Debian: `sudo apt install -y protobuf-compiler libprotobuf-dev libz-dev`

### Building
```
cmake -B build
cmake --build build -j
```

## Troubleshooting

### CMake issues

This project uses CMake's builtin FindProtobuf module which is supposed
to locate your Protobuf library and compiler installation.  Recent
versions of Protobuf broke the versioning scheme, resulting in a CMake
error (e.g. Protobuf 23.4 and CMake 3.26.4 won't work together).

If you get an error like this ...

```
CMake Warning at /opt/homebrew/Cellar/cmake/3.26.4/share/cmake/Modules/FindProtobuf.cmake:524 (message):
  Protobuf compiler version 23.4 doesn't match library version 4.23.4
Call Stack (most recent call first):
  CMakeLists.txt:33 (find_package)


CMake Error at /opt/homebrew/Cellar/cmake/3.26.4/share/cmake/Modules/FindPackageHandleStandardArgs.cmake:230 (message):
  Could NOT find Protobuf (missing: Protobuf_LIBRARIES) (found version
  "4.23.4")
Call Stack (most recent call first):
  /opt/homebrew/Cellar/cmake/3.26.4/share/cmake/Modules/FindPackageHandleStandardArgs.cmake:600 (_FPHSA_FAILURE_MESSAGE)
  /opt/homebrew/Cellar/cmake/3.26.4/share/cmake/Modules/FindProtobuf.cmake:650 (FIND_PACKAGE_HANDLE_STANDARD_ARGS)
  CMakeLists.txt:33 (find_package)
```

... work around this issue by defining CMake build variables manually
specifying protoc and Protobuf link flags.  In the example below, we
use `pkg-config` to gather the correct link flags.

```
cmake -B build \
  -DProtobuf_PROTOC_EXECUTABLE="$(which protoc)" \
  -DProtobuf_LIBRARY="$(pkg-config --libs protobuf)"
```

If all of the above still doesn't work, sometimes it helps to delete
your CMake cache.
