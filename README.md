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

- Open *R* and run `install.packages("pacman")`.
- Place "wxWidgets" at the same folder level as this project, downloading it with this command:
```
git clone https://github.com/wxWidgets/wxWidgets.git --recurse-submodules
```
- Open wxWidgets's CMake file in *Visual Studio*
  - Open **Project** > **CMake Settings for wxWidgets**
    - Uncheck **wxBUILD_SHARED** and save
  - Select **Build** > **Install wxWidgets** (builds and then copies the header, lib, and cmake files to the prefix folder)
- Open this project's CMake file in *Visual Studio* and save it (this is equivalent to configuring CMake and will generate some necessary configuration files)
- Open "ReadabilityStudio/docs/BuildHelpProjects.R" in *RStudio* and source the entire script
- Go back to *Visual Studio* and build the project

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

Place "wxWidgets" at the same folder level as this project, downloading and building it as follows:

```
cd ..
git clone https://github.com/wxWidgets/wxWidgets.git --recurse-submodules
cd wxWidgets
cmake . -DCMAKE_INSTALL_PREFIX=./wxlib -DwxBUILD_SHARED=OFF
cmake --build . --target install -j 4
cd ..
cd ReadabilityStudio
```

Build the program as follows:

```
cmake .
cmake --build . --target all  -j 4
```

**...INCOMPLETE**

## All Platforms

## Building

The first time that you build, you may receive LaTeX errors about missing \*.sty files during the help build phase.
Re-try the `cmake --build . --target all  -j 4` command a few times until these go away.

This happens because numerous \*.sty files need to be installed and the installation process times out occasionally.
Once you have these files installed, this won't be an issue when rebuilding later.

### Word Lists

If any of the word lists ("ReadabilityStudio/Resources/Words") have changed, then:

- Build the program as usual
- Open up *Readability Studio* and open the file "ReadabilityStudio/Resources/Finalize Word Lists.lua" in the Lua editor and run it
- Rebuild the program (this will re-package the word files)

This is necessary to re-sort the files and also re-build other words lists (e.g., the spelling dictionaries) that are based on the ones you may have edited.

### Screenshots

If the documentation screenshots need updating, then:

- Build the program as usual
- Open up *Readability Studio* and open the file "ReadabilityStudio/docs/Generate Screenshots.lua" in the Lua editor and run it
- Open "ReadabilityStudio/docs/BuildHelpProjects.R" and source the entire script
- Rebuild the program (this will re-package the documentation)
