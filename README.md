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

  - Place "wxWidgets" at the same folder level as this project
  - Open wxWidgets's CMake file in Visual Studio
  - Open **Project** > **CMake Project Settings** and enter `-DwxBUILD_SHARED=OFF` as a command line argument to CMake
  - Build wxWidgets
- Open **ReadabilityStudio/docs/BuildHelpProjects.R** and source the entire script
- Open this project's CMake file in Visual Studio and build it

## Linux

- Place "wxWidgets" at the same folder level as this project
- Run the following:
```
cd wxWidgets
cmake . -DwxBUILD_SHARED=OFF
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
