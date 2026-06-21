#!/bin/bash
set -xeu

cd "$(dirname $0)"

if [ "${1-}" != "merge_only" ]; then
    ARCH=x86_64 ./bundle.sh
    ARCH=arm64 ./bundle.sh
fi

SPEK_X86_64="$(realpath .)/Spek-Xtra.x86_64.app"
SPEK_ARM64="$(realpath .)/Spek-Xtra.arm64.app"
SPEK_UNIVERSAL="$(realpath .)/Spek-Xtra.app"

rm -rf "Spek-Xtra.app"
tar xf Spek-Xtra.x86_64.tgz
mv "Spek-Xtra.app" "$SPEK_X86_64"
tar xf Spek-Xtra.arm64.tgz
mv "Spek-Xtra.app" "$SPEK_ARM64"

cp -a "$SPEK_X86_64" "$SPEK_UNIVERSAL"

# Merge x86_64, arm64 excutables
cd "$SPEK_UNIVERSAL/Contents/MacOS"
for bin in *; do
    test -L "$bin" && continue
    rm $bin
    lipo -create \
        "$SPEK_X86_64/Contents/MacOS/$bin" \
        "$SPEK_ARM64/Contents/MacOS/$bin" \
        -o "$SPEK_UNIVERSAL/Contents/MacOS/$bin"
done

# Merge x86_64, arm64 libraries
cd "$SPEK_UNIVERSAL/Contents/Frameworks"
for bin in *; do
    test -L "$bin" && continue
    rm $bin
    lipo -create \
        "$SPEK_X86_64/Contents/Frameworks/$bin" \
        "$SPEK_ARM64/Contents/Frameworks/$bin" \
        -o "$SPEK_UNIVERSAL/Contents/Frameworks/$bin"
done

# Sign the app
codesign -fs - "$SPEK_UNIVERSAL" --deep

# Create a gzip tar archive
cd "$SPEK_UNIVERSAL/.."
rm -f Spek-Xtra.tgz
tar cvzf Spek-Xtra.tgz "Spek-Xtra.app"
