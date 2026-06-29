#!/bin/bash
set -xeu

# This script will compile spek-xtra.exe under MSYS2 and make a ZIP archive.
# Check README.md in this directory for instructions.

# Adjust these variables if necessary.
MAKE=make
JOBS=3
ZIP=zip
STRIP=strip
WINDRES=windres
CYGPATH=cygpath
WX_CONFIG=wx-config
WX_PREFIX="$(wx-config --prefix)"

LANGUAGES="bs ca cs da de el eo es fi fr gl he hu id it ja ko lv nb nl pl pt_BR ru sk sr@latin sv th tr uk vi zh_CN zh_TW"

cd "$(dirname $0)"/../..
rm -fr dist/win/build && mkdir dist/win/build

# Test windres
"$WINDRES" dist/win/spek.rc -O coff -o dist/win/spek.res

# Run autogen.sh
CXXFLAGS="--static" \
LDFLAGS="-mwindows dist/win/spek.res" ./autogen.sh \
    --disable-shared \
    --enable-static \
    --disable-valgrind \
    --with-wx-config="$WX_CONFIG" \
    --prefix="$PWD/dist/win/build" && \
    "$MAKE" clean

# Compile PO files first
cd po && "$MAKE" && cd ..

# Compile the resource file
./dist/win/compile-rc.py "$WINDRES" "$(CYGPATH "$WX_PREFIX")" $LANGUAGES
mkdir -p src/dist/win && cp dist/win/spek.res src/dist/win/

# Compile spek-xtra.exe
"$MAKE" V=1 -j "$JOBS" && "$MAKE" install
"$STRIP" dist/win/build/bin/spek-xtra.exe

# Copy files to the bundle
cd dist/win
rm -fr Spek-Xtra && mkdir Spek-Xtra
cp build/bin/spek-xtra.exe Spek-Xtra/
cp ../../LICENSE.md Spek-Xtra/
cp ../../README.md Spek-Xtra/
mkdir Spek-Xtra/licenses
cp ../../lic/* Spek-Xtra/licenses/
for lic in Spek-Xtra/licenses/*; do
    mv "$lic" "$lic.txt"
done
rm -fr build
cd ../..

# Create a zip archive (keep the unpacked files so the standalone .exe can also
# be uploaded as a release artifact).
cd dist/win/Spek-Xtra
rm -f Spek-Xtra.zip
cd ..
rm -f Spek-Xtra.zip
"$ZIP" -r Spek-Xtra.zip Spek-Xtra
cp Spek-Xtra.zip Spek-Xtra/Spek-Xtra.zip
cd ../..

# Clean up
rm -fr src/dist
