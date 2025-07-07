#!/bin/bash
rm -fr AppDir
mkdir -p build
pushd build
cmake .. -GNinja -DCMAKE_BUILD_TYPE=Release
cmake --build . --config Release
popd
linuxdeploy-x86_64.AppImage -e build/proxy-master -d misc/proxy-master.desktop -i misc/proxy-master.png --appdir AppDir/
cp build/dark.txt AppDir/usr/bin/
linuxdeploy-x86_64.AppImage --appdir AppDir --output appimage
