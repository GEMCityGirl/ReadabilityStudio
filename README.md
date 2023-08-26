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

- Visual Studio
- RStudio
- InnoSetup

- Place "wxWidgets" at the same folder level as this project, downloading it with this command:
```
git clone https://github.com/wxWidgets/wxWidgets.git --recurse-submodules
```
- Open wxWidgets's CMake file in Visual Studio
  - Open **Project** > **CMake Project Settings** and enter `-DwxBUILD_SHARED=OFF` as a command line argument to CMake
  - Build wxWidgets
- Open **ReadabilityStudio/docs/BuildHelpProjects.R** and source the entire script
- Open this project's CMake file in Visual Studio and build it

## Linux

Install the following tools to build *Readability Studio*:

- GCC (C++ and fortran compilers)
- CMake
- git
- RStudio

Install the following libraries *and* their development files:

- GTK+ 3
- libCURL
- GStreamer
- libsecret
- webkit
- TBB
- OpenMP
- libopenssl
- libxml2

- Place "wxWidgets" at the same folder level as this project, downloading it with this command:
```
git clone https://github.com/wxWidgets/wxWidgets.git --recurse-submodules
cd wxWidgets
cmake . -DCMAKE_INSTALL_PREFIX=./wxlib -DwxBUILD_SHARED=OFF
cmake --build . --target install
cd ..
cd ReadabilityStudio
cmake .
cmake --build .
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
