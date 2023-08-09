#############################################################################
# Name:        CMakeLists.txt
# Purpose:     GUI unit test runner for Readability Studio
# Author:      Blake Madden
# Created:     20232-04-29
# Copyright:   (c) 2023 Blake Madden
# License:     3-Clause BSD license
#############################################################################

CMAKE_MINIMUM_REQUIRED(VERSION 3.14)
SET(CMAKE_CXX_STANDARD 17)
SET(CMAKE_CXX_STANDARD_REQUIRED True)

IF(NOT CMAKE_CONFIGURATION_TYPES)
    GET_PROPERTY(HAVE_MULTI_CONFIG_GENERATOR GLOBAL PROPERTY GENERATOR_IS_MULTI_CONFIG)
    # Set default configuration types for multi-config generators
    IF(HAVE_MULTI_CONFIG_GENERATOR)
        SET(CMAKE_CONFIGURATION_TYPES "Debug;Release")
    ENDIF()
ENDIF()

# https://blog.kitware.com/cmake-and-the-default-build-type/
IF(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
    SET(default_build_type "Debug")
    MESSAGE(STATUS "Setting build type to '${default_build_type}' as none was specified.")
    SET(CMAKE_BUILD_TYPE "${default_build_type}" CACHE STRING "Choose the type of build." FORCE)
    # Set the possible values of build type for cmake-gui
    SET_PROPERTY(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "Debug" "Release")
ENDIF()

PROJECT(WisteriaGuiTestRunner)

# Setup wxWidgets
########################

# Find wxWidgets (wxWidgetsConfig.cmake, specifically) and select its components
SET(wxWidgets_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../../../wxWidgets/out/install/x64-${CMAKE_BUILD_TYPE}/lib/cmake/wxWidgets")
SET(wxWidgets_USE_STATIC 1)
FIND_PACKAGE(wxWidgets 3.3 COMPONENTS core base xrc propgrid xml aui ribbon qa stc net REQUIRED CONFIG)

IF(APPLE AND NOT CMAKE_OSX_DEPLOYMENT_TARGET)
    # If no deployment target has been set default to the minimum supported
    # OS version (this has to be set before the first project() call)
    IF(CMAKE_SYSTEM_NAME STREQUAL "iOS")
        SET(CMAKE_OSX_DEPLOYMENT_TARGET 12.0 CACHE STRING "iOS Deployment Target")
    ELSE()
        SET(CMAKE_OSX_DEPLOYMENT_TARGET 10.10 CACHE STRING "macOS Deployment Target")
    ENDIF()
ENDIF()

ADD_COMPILE_OPTIONS("$<$<C_COMPILER_ID:MSVC>:/utf-8>")
ADD_COMPILE_OPTIONS("$<$<CXX_COMPILER_ID:MSVC>:/utf-8>")

# Set definitions, warnings, and optimizations (will propagate to the demo project also)
IF(MSVC)
    TARGET_COMPILE_DEFINITIONS(Wisteria PUBLIC __WXMSW__ _CRT_SECURE_NO_WARNINGS __UNITTEST
        $<$<CONFIG:Debug>:__WXDEBUG__ wxDEBUG_LEVEL=2> $<$<CONFIG:Release>:wxDEBUG_LEVEL=0>)
    # /Zc:__cplusplus tells MSVC to set the C++ version what we are
    # actually compiling as. The default behavior in MSVC is to say that the
    # C++ version is 98 always (for compatibility reasons).

    # /MP enables multi-core build support, speeding up compilation time.

    # /W3 cranks up the warning level, /WX treats all warnings as error.
    # Note that simply using /W4 causes a warning that it is overriding
    # /W3, and then that turns into an error because of /W4. So /W3 is the
    # highest you can go with /WX.

    # /wd6211 turns off C6211 warning: leaking memory due to an exception.
    # wxWidgets uses heap-based objects for most everything, and MVSC complains
    # about not wrapping all of this logic in try blocks.
    TARGET_COMPILE_OPTIONS(Wisteria PUBLIC /Zc:__cplusplus /MP /W3 /WX /wd6211
                           $<$<CONFIG:Debug>:/Od> $<$<CONFIG:Release>:/O2>)
# Placeholder code for macOS (needs testing)
#ELSEIF(APPLE)
#    TARGET_COMPILE_DEFINITIONS(Wisteria PUBLIC __WXOSX__ __UNITTEST
#        $<$<CONFIG:Debug>:__WXDEBUG__ wxDEBUG_LEVEL=2> $<$<CONFIG:Release>:wxDEBUG_LEVEL=0>)
#    TARGET_COMPILE_OPTIONS(Wisteria PUBLIC -Wall -Wextra -Wpedantic
#                           $<$<CONFIG:Debug>:-Og> $<$<CONFIG:Release>:-O2>)
ELSEIF(UNIX)
    TARGET_COMPILE_DEFINITIONS(Wisteria PUBLIC __WXGTK__ __UNITTEST
        $<$<CONFIG:Debug>:__WXDEBUG__ wxDEBUG_LEVEL=2> $<$<CONFIG:Release>:wxDEBUG_LEVEL=0>)
    TARGET_COMPILE_OPTIONS(Wisteria PUBLIC -Wall -Wextra -Wpedantic
                           $<$<CONFIG:Debug>:-Og> $<$<CONFIG:Release>:-O2>)
ENDIF()

MESSAGE(STATUS "Finding Catch2...")
# place Catch2 at the same folder level as this repo if it isn't installed
# (you will need to do this on Windows or macOS or if version 3 of Catch2 isn't installed)
GET_FILENAME_COMPONENT(_fullpath "${_dir}" REALPATH)
IF(EXISTS "${_fullpath}" AND EXISTS "${_fullpath}/../../../Catch2")
    ADD_SUBDIRECTORY(${CMAKE_CURRENT_SOURCE_DIR}/../../../Catch2 ${CMAKE_CURRENT_BINARY_DIR}/Catch2)
ELSE()
    # ...otherwise, see if it is installed
    FIND_PACKAGE(Catch2 3 REQUIRED)
ENDIF()

# Define the build target for the executable
MESSAGE(STATUS "Preparing the test runner...")
SET(TEST_APP RSGuiTestRunner)
INCLUDE(${CMAKE_SOURCE_DIR}/messagestests.cpp)
SET(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)
ADD_EXECUTABLE(${TEST_APP} WIN32 MACOSX_BUNDLE ${TEST_SRC_FILES})

# Link required libraries to the executable
IF(WIN32)
    TARGET_LINK_LIBRARIES(${TEST_APP} PRIVATE Catch2::Catch2 ${wxWidgets_LIBRARIES})
ELSEIF(UNIX)
    FIND_LIBRARY(TBB_LIB tbb)
    TARGET_LINK_LIBRARIES(${TEST_APP} ${wxWidgets_LIBRARIES} ${TBB_LIB} Catch2::Catch2)
ENDIF()

# load the test cases into the runner
INCLUDE(CTest)
INCLUDE(Catch)
CATCH_DISCOVER_TESTS(${TEST_APP})
