# GDMP-Client
This is where she makes a mod.

## Build Instructions

- Install [Geode](https://docs.geode-sdk.org/installation)

### Windows
```shell
git clone https://github.com/Furries-UwU/GDMP-Client.git --recursive
cmake . -A Win32
cmake --build . --config Release
```

### Linux
- follow [this guide by mat](https://gist.github.com/matcool/abb65ee59ded3766717c673014c3a2a7) and make sure your environment is properly set up
```shell
cmake \
  -DCMAKE_TOOLCHAIN_FILE=$TOOLCHAIN_REPO/clang-cl-msvc.cmake \
  -DMSVC_BASE=$MSVC_BASE \
  -DWINSDK_BASE=$WINSDK_BASE \
  -DWINSDK_VER=$WINSDK_VER \
  -DGEODE_DISABLE_FMT_CONSTEVAL=true \
  -DCMAKE_BUILD_TYPE=Release -B build

cmake --build build --config Release
```

### MacOS
- idk
- <img src="https://github.com/Furries-UwU/GDMP-Client/assets/78933889/f52682ee-ac69-4d4f-b7a1-a06c9873d470" width="420px">

## Credits
**Thanks to**
- [Geode](), the modding framework this mod uses <3
- [Protocol Buffers](https://github.com/protocolbuffers/protobuf) for (de)serialization of data and shared data structures
- [ENet](https://github.com/lsalzman/enet), the networking library
- [CPM.cmake](https://github.com/cpm-cmake/CPM.cmake) for making CMake dependency management easier
- CMake, for making me suffer
