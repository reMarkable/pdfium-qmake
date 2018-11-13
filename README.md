# iOS

## Fetching binary

The binaries are stored using git-lfs. To replace the SHA1 links with real files, do

    git lfs fetch
    git lfs checkout

## Building binary

The binary is hosted via `git-lfs`, and can be fetched after `brew install git-lfs`;
   - `git lfs fetch`
   - `git lfs checkout`

`libpdfium.a` for iOS is no longer built via qmake, but rather directly from pdfium with a few patches applied and one revert;

   - `git submodule update --init --recursive`
   - `export PATH=$PATH:$PWD/depot_tools`
   - `gclient config --unmanaged https://pdfium.googlesource.com/pdfium.git`
   - `gclient sync`
   - `cd pdfium`
   - `patch -p1 < ../iosprecompiled/ios.patch`
   - `git revert --strategy=ours 0e5d892fe86d7c2527d8f7b7ac2c5aa8fc77a7be`
   - reset/checkout all conflicts
     - `git reset HEAD third_party/yasm/ .gitignore DEPS`
     - `git checkout third_party/yasm/ .gitignore DEPS`
   - `cd ..`
   - `python3 make.py run build-ios`
   - `lipo -create pdfium/out/release-x64/obj/libpdfium.a pdfium/out/release-x86/obj/libpdfium.a pdfium/out/release-arm/obj/libpdfium.a pdfium/out/release-arm64/obj/libpdfium.a -o iosprecompiled/libpdfium.a`

`libpdfium.a` for macOS can be built by generating the build using args in `macosprecompiled/`

   - `cd pdfium`
   - `../depot_tools/gn args ../macosprecompiled`
   - `../depot_tools/ninja -C ../macosxprecompiled/ -j6 pdfium_all`
   - `mv ../macosxprecompiled/obj/libpdfium.a ../macosxprecompiled/`
