#!/bin/sh
mkdir -p build
cmake -B build/ -DCMAKE_BUILD_TYPE=Release -GNinja -DCMAKE_INSTALL_PREFIX=build/AppDir/usr
cmake --build build/ --config Release
cmake --install build/
linuxdeploy --appdir build/AppDir --output appimage
