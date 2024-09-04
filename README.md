# ReadabilityStudio
Readability Studio

[![Linux Unit Tests](https://github.com/Blake-Madden/ReadabilityStudio/actions/workflows/unit-tests.yml/badge.svg)](https://github.com/Blake-Madden/ReadabilityStudio/actions/workflows/unit-tests.yml)
[![macOS Unit Tests](https://github.com/Blake-Madden/ReadabilityStudio/actions/workflows/mac-unit-tests.yml/badge.svg)](https://github.com/Blake-Madden/ReadabilityStudio/actions/workflows/mac-unit-tests.yml)
[![Windows Unit Tests](https://github.com/Blake-Madden/ReadabilityStudio/actions/workflows/windows-unit-tests.yml/badge.svg)](https://github.com/Blake-Madden/ReadabilityStudio/actions/workflows/windows-unit-tests.yml)

[![cppcheck](https://github.com/Blake-Madden/ReadabilityStudio/actions/workflows/cppcheck.yml/badge.svg)](https://github.com/Blake-Madden/ReadabilityStudio/actions/workflows/cppcheck.yml)
[![Microsoft C++ Code Analysis](https://github.com/Blake-Madden/ReadabilityStudio/actions/workflows/msvc.yml/badge.svg)](https://github.com/Blake-Madden/ReadabilityStudio/actions/workflows/msvc.yml)
[![i18n-check](https://github.com/Blake-Madden/ReadabilityStudio/actions/workflows/i18n-check.yml/badge.svg)](https://github.com/Blake-Madden/ReadabilityStudio/actions/workflows/i18n-check.yml)

[![doxygen](https://github.com/Blake-Madden/ReadabilityStudio/actions/workflows/doxygen.yml/badge.svg)](https://github.com/Blake-Madden/ReadabilityStudio/actions/workflows/doxygen.yml)
[![Spell Check](https://github.com/Blake-Madden/ReadabilityStudio/actions/workflows/spell-check.yml/badge.svg)](https://github.com/Blake-Madden/ReadabilityStudio/actions/workflows/spell-check.yml)

# Building

## Windows

Install the following tools to build *Readability Studio*:

- *Visual Studio*
- *R*
- *Quarto*
- *InnoSetup*
- *POEdit* (if editing the translations)

- Open *Visual Studio* and select *Clone a Repository*
  - Enter "https://github.com/wxWidgets/wxWidgets.git" and clone it to same level as this project
- Once the wxWidgets folder is cloned and opened in *Visual Studio*:
  - Open **Project** > **CMake Settings for wxWidgets**
    - Uncheck **wxBUILD_SHARED**
    - Set **wxBUILD_OPTIMISE** to "ON"
    - Save your changes
  - Select **Build** > **Install wxWidgets** (builds and then copies the header, lib, and cmake files to the prefix folder)
- Open this project's CMake file in *Visual Studio* and save it (this is equivalent to configuring CMake and will generate some necessary configuration files)
- Select **View** > **CMake Targets**
- Build the *manuals* and *readstudio* targets

## Linux

Install the following tools to build *Readability Studio*:

- *GCC* (C++ and fortran compilers)
- *CMake*
- *git*
- *R*
- *Quarto*
- *pandoc*
- *pandoc-citeproc*
- *POEdit* (if editing the translations)
- *AppImage*, *linuxdeploy*, and *AppImageLauncher* (if building an AppImage)

Install the following libraries (*and* their development files if mentioned):

- *GTK+ 3*, *gtk3-devel*/*libgtk3-dev*
- *libCURL*, *libcurl-devel*/*libcurl-dev*
- *GStreamer*, *gstreamer-devel*
- *libsecret*, *libsecret-devel*/*libsecret-1-dev*
- *webkit*, *webkit2*
- *SDL2-devel*
- *libnotify*, *libnotify-devel*/*libnotify-dev*
- *TBB*, *tbb-devel*/*tbb-dev*
- *OpenMP*
- *libopenssl*, *libopenssl-3-devel*/*libssl-dev*
- *libxml2*, *libxml2-devel*/*libxml2-dev*

Prepare *R* to install packages (needed for the help):

```
R
install.packages("pacman")
```

Place *wxWidgets* at the same folder level as this project, downloading and building it as follows:

```
cd ..
git clone https://github.com/wxWidgets/wxWidgets.git --recurse-submodules
cd wxWidgets
cmake . -DCMAKE_INSTALL_PREFIX=./wxlib -DwxBUILD_SHARED=OFF -DwxBUILD_OPTIMISE=ON -DwxBUILD_STRIPPED_RELEASE_DEFAULT=ON -DCMAKE_BUILD_TYPE=Release
cmake --build . --target install -j $(nproc)
cd ..
cd ReadabilityStudio
```

Build the program as follows:

```
cmake .
cmake --build . --target all -j $(nproc)
```

Build an AppImage:

```
linuxdeploy-x86_64.AppImage --appdir installers/unix/AppDir --executable installers/unix/AppDir/readstudio -d installers/unix/AppDir/readstudio.desktop -i installers/unix/AppDir/app-logo.svg --output appimage
mv Readability_Studio*.AppImage ./installers/unix
```

**...INCOMPLETE**

## macOS

To install home-brew:

```
/bin/bash -c "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/HEAD/install.sh)"
```

To install CMake:

```
brew install cmake
```

If you get errors about not finding a CXX compiler, run this:

```
sudo xcode-select --reset
```

Place *wxWidgets* at the same folder level as this project, downloading and building it as follows:

```
cd ..
git clone https://github.com/wxWidgets/wxWidgets.git --recurse-submodules
cd wxWidgets
cmake . -DCMAKE_INSTALL_PREFIX=./wxlib -DwxBUILD_SHARED=OFF -D"CMAKE_OSX_ARCHITECTURES:STRING=arm64;x86_64" -DwxBUILD_OPTIMISE=ON -DwxBUILD_STRIPPED_RELEASE_DEFAULT=ON -DCMAKE_BUILD_TYPE=Release
cmake --build . --target install -j $(nproc)
cd ..
cd ReadabilityStudio
```

## All Platforms

## Building

The first time that you build, you may receive LaTeX errors about missing \*.sty files during the help build phase.
Re-try the `cmake --build . --target all  -j $(nproc)` command a few times until these go away.

This happens because numerous \*.sty files need to be installed and the installation process times out occasionally.
Once you have these files installed, this won't be an issue when rebuilding later.

### Application Icon

If the application icon ("resources/images/ribbon/app-logo.svg") is changed, then in the project directory:

- Save a copy as "app-logo.png" (1024x1024 resolution)
- Save a copy as "app-logo.ico" (512x512 resolution)
- On macOS, run this in a terminal:

```
mkdir app-logo.iconset
sips -z 16 16     app-logo.png --out app-logo.iconset/icon_16x16.png
sips -z 32 32     app-logo.png --out app-logo.iconset/icon_16x16@2x.png
sips -z 32 32     app-logo.png --out app-logo.iconset/icon_32x32.png
sips -z 64 64     app-logo.png --out app-logo.iconset/icon_32x32@2x.png
sips -z 128 128   app-logo.png --out app-logo.iconset/icon_128x128.png
sips -z 256 256   app-logo.png --out app-logo.iconset/icon_128x128@2x.png
sips -z 256 256   app-logo.png --out app-logo.iconset/icon_256x256.png
sips -z 512 512   app-logo.png --out app-logo.iconset/icon_256x256@2x.png
sips -z 512 512   app-logo.png --out app-logo.iconset/icon_512x512.png
cp app-logo.png app-logo.iconset/icon_512x512@2x.png
iconutil -c icns app-logo.iconset
rm -R app-logo.iconset
```

### Word Lists

If any of the word lists ("ReadabilityStudio/resources/words") have changed, then:

- Build the program as usual
- Open up *Readability Studio* and open the file "ReadabilityStudio/resources/finalize-word-lists.lua" in the Lua editor and run it
- Rebuild the program (this will re-package the word files)

This is necessary to re-sort the files and also re-build other words lists (e.g., the spelling dictionaries) that are based on the ones you may have edited.

### Screenshots

If the documentation screenshots need updating, then:

- Build the program as usual
- Open up *Readability Studio* and open the file "ReadabilityStudio/docs/generate-screenshots.lua" in the Lua editor and run it
- Open "ReadabilityStudio/docs/build-help-projects.R" and source the entire script
- Rebuild the program (this will re-package the documentation)
