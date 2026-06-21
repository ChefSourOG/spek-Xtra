#!/bin/bash

if [ "$1" != "i_am_ci" ]
then
    cat << EOF

!!! DO NOT RUN THIS SCRIPT ON YOUR COMPUTER !!!
-----------------------------------------------
This script is used to build Spek-Xtra Apple Silicon packages on Intel macOS CI.
It will break your dependencies if not executed in a container.

EOF
    exit 1
fi

LANGUAGES="bs ca cs da de el eo es fi fr gl he hu id it ja ko lv nb nl pl pt_BR ru sk sr@latin sv th tr uk vi zh_CN zh_TW"
LOCALEDIR="/usr/local/share/locale/"
[ ! -d "$LOCALEDIR" ] && [ -d "$HOMEBREW_PREFIX/share/locale/" ] && LOCALEDIR="$HOMEBREW_PREFIX/share/locale/"

cd $(dirname $0)/../../..

rm -f src/spek-xtra

sed 's/ --host=.*"/"/' "$HOMEBREW_PREFIX"/share/wx/*/aclocal/wxwin.m4 > wxwin.m4
echo "m4_include([wxwin.m4])" > acinclude.m4

./autogen.sh CC='clang -arch arm64' CXX='clang++ -arch arm64' --host=arm64-apple-darwin && make -j2 || exit 1

cd dist/osx
rm -fr "Spek-Xtra.app"
mkdir -p "Spek-Xtra.app/Contents/MacOS"
mkdir -p "Spek-Xtra.app/Contents/Frameworks"
mkdir -p "Spek-Xtra.app/Contents/Resources"
mv ../../src/spek-xtra "Spek-Xtra.app/Contents/MacOS/Spek-Xtra"
cp Info.plist "Spek-Xtra.app/Contents/"
cp Spek.icns "Spek-Xtra.app/Contents/Resources/"
cp *.png "Spek-Xtra.app/Contents/Resources/"
cp ../../LICENCE.md "Spek-Xtra.app/Contents/Resources/"
cp ../../README.md "Spek-Xtra.app/Contents/Resources/"
mkdir "Spek-Xtra.app/Contents/Resources/lic"
cp ../../lic/* "Spek-Xtra.app/Contents/Resources/lic/"

for lang in $LANGUAGES; do
    mkdir -p "Spek-Xtra.app/Contents/Resources/$lang.lproj"
    cp -v ../../po/"$lang".gmo "Spek-Xtra.app/Contents/Resources/$lang.lproj/spek.mo"
    cp -v "$LOCALEDIR""$lang"/LC_MESSAGES/wxstd*.mo "Spek-Xtra.app/Contents/Resources/$lang.lproj/wxstd.mo"
done
mkdir -p "Spek-Xtra.app/Contents/Resources/en.lproj"

BINS="Spek-Xtra.app/Contents/MacOS/Spek-Xtra"
while [ ! -z "$BINS" ]; do
    NEWBINS=""
    for bin in $BINS; do
        echo "Updating dependendies for $bin."
        LIBS=$(otool -L $bin | grep -e /usr/local/ -e /opt/ | tr -d '\t' | awk '{print $1}')
        for lib in $LIBS; do
            reallib=$(realpath $lib)
            libname=$(basename $reallib)
            install_name_tool -change $lib @executable_path/../Frameworks/$libname $bin
            if [ ! -f "Spek-Xtra.app/Contents/Frameworks/$libname" ]; then
                echo "\tBundling $reallib."
                cp $reallib "Spek-Xtra.app/Contents/Frameworks/"
                chmod +w "Spek-Xtra.app/Contents/Frameworks/$libname"
                install_name_tool -id @executable_path/../Frameworks/$libname "Spek-Xtra.app/Contents/Frameworks/$libname"
                NEWBINS="$NEWBINS Spek-Xtra.app/Contents/Frameworks/$libname"
            fi
        done
    done
    BINS="$NEWBINS"
done

# Sign the app
codesign -fs - "./Spek-Xtra.app" --deep

# Create a gzip tar archive
rm -f Spek-Xtra.tgz
tar cvzf Spek-Xtra.tgz "./Spek-Xtra.app"

# Clean up
cd ../..
rm wxwin.m4 acinclude.m4
