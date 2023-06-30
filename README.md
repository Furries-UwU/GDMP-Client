# prebuilt-protobuf
Protobuf version: v21.12

## Compile

### Windows

cmake . -T host=x64 -A win32 -DCMAKE_CXX_STANDARD=20 -Dprotobuf_INSTALL=0 -Dprotobuf_BUILD_TESTS=0 -Dprotobuf_BUILD_SHARED_LIBS=0
cmake --build . --config Release -j 16

### macOS

cmake . -DCMAKE_OSX_ARCHITECTURES=x86_64 -DCMAKE_CXX_STANDARD=20 -Dprotobuf_INSTALL=0 -Dprotobuf_BUILD_TESTS=0 -Dprotobuf_BUILD_SHARED_LIBS=0
cmake --build . --config Release -j 16