# All AOO specific zlib build parameters in one place
AOO_ZLIB_CMAKE_ARGS = [
    "-DBUILD_SHARED_LIBS=OFF",
    "-DZLIB_BUILD_EXAMPLES=OFF",
    "-DCMAKE_POSITION_INDEPENDENT_CODE=ON",     # needed for AOO shared libs
    "-DCMAKE_BUILD_TYPE=Release",
]

AOO_ZLIB_CACHE_ENTRIES = {
    "CMAKE_C_FLAGS": "-fno-strict-aliasing",    # AOO required flag
    "ZLIB_COMPAT": "ON",
}

# Platform specific overrides
AOO_ZLIB_CMAKE_ARGS_WINDOWS  = AOO_ZLIB_CMAKE_ARGS + [
    "-DCMAKE_WINDOWS_EXPORT_ALL_SYMBOLS=ON",
]

AOO_ZLIB_CMAKE_ARGS_MACOS  = AOO_ZLIB_CMAKE_ARGS + [
    "-DCMAKE_OSX_DEPLOYMENT_TARGET=10.15",
]
