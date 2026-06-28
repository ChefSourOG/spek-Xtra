#!/bin/bash
set -xeu

LANGUAGES="bs ca cs da de el eo es fi fr gl he hu id it ja ko lv nb nl pl pt_BR ru sk sr@latin sv th tr uk vi zh_CN zh_TW"
PROJDIR=$(realpath "$(dirname $0)/../..")

[ -z "${ARCH-}" ] && ARCH=$(uname -m)
if [ "$ARCH" = "x86_64" ]; then
    echo "Target architecture: $ARCH"
    MACOSX_DEPLOYMENT_TARGET=10.10
elif [ "$ARCH" = "arm64" ]; then
    echo "Target architecture: $ARCH"
    MACOSX_DEPLOYMENT_TARGET=12.0
else
    echo "Unsupported architecture: $ARCH" >&2
    exit 1
fi

DEPSDIR="$(realpath "$(dirname $0)")/deps.$ARCH"
if [ ! -e "$DEPSDIR" ]; then
    echo "Dependency not found. Running ./install_deps.sh ..."
    env ARCH="$ARCH" ./install_deps.sh
fi

export MACOSX_DEPLOYMENT_TARGET
export PATH="$DEPSDIR/bin:$PATH"
export ACLOCAL="aclocal -I $DEPSDIR/share/aclocal"
export PKG_CONFIG_PATH="$DEPSDIR/lib/pkgconfig"
export CXX="/usr/bin/clang++ -arch $ARCH"

cd "$PROJDIR"

rm -f src/spek-xtra

mkdir -p builddir
cd builddir
BUILDDIR=$(pwd)
../autogen.sh --host="$ARCH-apple-darwin" && make clean && make -j$(nproc) || exit 1

cd "$PROJDIR/dist/osx"
rm -fr "Spek-Xtra.app" "Spek-Xtra-$ARCH.app"
mkdir -p "Spek-Xtra.app/Contents/MacOS"
mkdir -p "Spek-Xtra.app/Contents/Frameworks"
mkdir -p "Spek-Xtra.app/Contents/Resources"
mv "$BUILDDIR"/src/spek-xtra "Spek-Xtra.app/Contents/MacOS/Spek-Xtra"
cp "$BUILDDIR"/dist/osx/Info.plist "Spek-Xtra.app/Contents/"
cp "$PROJDIR"/dist/osx/Spek.icns "Spek-Xtra.app/Contents/Resources/"
cp "$PROJDIR"/dist/osx/*.png "Spek-Xtra.app/Contents/Resources/"
cp "$PROJDIR"/LICENSE.md "Spek-Xtra.app/Contents/Resources/"
cp "$PROJDIR"/README.md "Spek-Xtra.app/Contents/Resources/"
mkdir "Spek-Xtra.app/Contents/Resources/lic"
cp "$PROJDIR"/lic/* "Spek-Xtra.app/Contents/Resources/lic/"

for lang in $LANGUAGES; do
    mkdir -p "Spek-Xtra.app/Contents/Resources/$lang.lproj"
    cp "$BUILDDIR"/po/"$lang".gmo "Spek-Xtra.app/Contents/Resources/$lang.lproj/spek.mo"
    mo=("$DEPSDIR"/share/locale/"$lang"/LC_MESSAGES/wxstd*.mo)
    test -f "$mo" && cp "$mo" "Spek-Xtra.app/Contents/Resources/$lang.lproj/wxstd.mo"
done
unset lang mo
mkdir -p "Spek-Xtra.app/Contents/Resources/en.lproj"

bin="Spek-Xtra.app/Contents/MacOS/Spek-Xtra"
echo "Updating dependendies for $bin."
cp -a "$DEPSDIR"/lib/*.dylib "Spek-Xtra.app/Contents/Frameworks/"
install_name_tool -add_rpath "@executable_path/../Frameworks" "$bin"

# Sign the app
codesign -fs - "./Spek-Xtra.app" --deep

# Create a gzip tar archive
rm -f "Spek-Xtra.$ARCH.tgz"
tar cvzf "Spek-Xtra.$ARCH.tgz" "Spek-Xtra.app"
