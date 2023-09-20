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
- *R* (and optionally *RStudio*)
- *InnoSetup*

- Place "wxWidgets" at the same folder level as this project, downloading it with this command:
```
git clone https://github.com/wxWidgets/wxWidgets.git --recurse-submodules
```
- Open wxWidgets's CMake file in *Visual Studio*
  - Open **Project** > **CMake Project Settings**
    - Uncheck **wxBUILD_SHARED** and change **MAKE_INSTALL_PREFIX** to *[path to wxWidgets]/wxlib* (*[path to wxWidgets]* will be a hard-coded path on your computer)
  - Select **Build** > **Build All**
  - Select **Build** > **Install wxWidgets** (really just copies the header, lib, and cmake files to the prefix folder)
- Open this project's CMake file in *Visual Studio* and save it (this is equivalent to configuring CMake and will generate some necessary configuration files)
- Open **ReadabilityStudio/docs/BuildHelpProjects.R** in *RStudio* and source the entire script
- Go back to *Visual Studio* and build the project

## Linux

Install the following tools to build *Readability Studio*:

- *GCC* (C++ and fortran compilers)
- *CMake*
- *git*
- *R* (and optionally *RStudio*)
- *POEdit* (if editing the translations)

Install the following libraries (*and* their development files if mentioned):

- *GTK+ 3*, *gtk3-devel*
- *libCURL*, *libcurl-devel*
- *GStreamer*, *gstreamer-devel*
- *libsecret*, *libsecret-devel*
- *webkit*, *webkit2gtk3-devel*
- *SDL2-devel*
- *libnotify*, *libnotify-devel*
- *TBB*, *tbb-devel*
- *OpenMP*
- *libopenssl*, *libopenssl-3-devel*
- *libxml2*, *libxml2-devel*

- Place "wxWidgets" at the same folder level as this project, downloading it with this command:
```
git clone https://github.com/wxWidgets/wxWidgets.git --recurse-submodules
cd wxWidgets
cmake . -DCMAKE_INSTALL_PREFIX=./wxlib -DwxBUILD_SHARED=OFF
cmake --build . --target install
cd ..
cd ReadabilityStudio
cmake .
cmake --build . --target all
```

**...INCOMPLETE**

## All Platforms

If any of the word lists (**ReadabilityStudio/Resources/Words**) have changed, then:

- Build the program as usual
- Open up *Readability Studio* and open the file **ReadabilityStudio/Resources/Finalize Word Lists.lua** in the Lua editor and run it
- Rebuild the program (this will re-package the word files)

This is necessary to re-sort the files and also re-build other words lists (e.g., the spelling dictionaries) that are based on the ones you may have edited.

If the documentation screenshots need updating, then:

- Build the program as usual
- Open up *Readability Studio* and open the file **ReadabilityStudio/docs/Generate Screenshots.lua** in the Lua editor and run it
- Open **ReadabilityStudio/docs/BuildHelpProjects.R** and source the entire script
- Rebuild the program (this will re-package the documentation)
