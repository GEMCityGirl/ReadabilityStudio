#############################################################################
# Name:        CMakeLists.txt
# Purpose:     Build script for Readability Studio
# Author:      Blake Madden
# Created:     2023-07-31
# Copyright:   (c) 2006-2023 Blake Madden
# License:     3-Clause BSD license
#############################################################################

# Declare the minimum required CMake version
# Note that CMake 3.24 is needed if compiling wxWidgets 3.2
CMAKE_MINIMUM_REQUIRED(VERSION 3.24)
SET(CMAKE_CXX_STANDARD 17)
SET(CMAKE_CXX_STANDARD_REQUIRED True)

PROJECT(ReadStudio)

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

# Setup wxWidgets
########################

# Find wxWidgets (wxWidgetsConfig.cmake, specifically) and select its components
SET(wxWidgets_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../wxWidgets/out/install/x64-${CMAKE_BUILD_TYPE}/lib/cmake/wxWidgets")
MESSAGE(STATUS "Adding wxWidgets CMake cofig folder: ${wxWidgets_DIR}")
IF(NOT EXISTS "${wxWidgets_DIR}/wxWidgetsConfig.cmake" AND NOT EXISTS "${wxWidgets_DIR}/wxwidgets-config.cmake")
    MESSAGE(STATUS "Warning: cannot find wxWidgets CMake configuration. (Looked in ${wxWidgets_DIR}.)")
ENDIF()
SET(wxWidgets_USE_STATIC 1)
FIND_PACKAGE(wxWidgets 3.3 COMPONENTS core base xrc propgrid xml aui ribbon qa stc net richtext REQUIRED CONFIG)

# Include the wxWidgets use file to initialize various settings
IF(wxWidgets_USE_FILE)
    INCLUDE(${wxWidgets_USE_FILE})
ENDIF()
MESSAGE(STATUS "Adding wxWidgets libraries: ${wxWidgets_LIBRARIES}")
MESSAGE(STATUS "Adding wxWidgets: ${wxPLATFORM_LIB_DIR}")

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

# Build binary
########################
# (run "tools/Build CMake Files List.R" to update this)
INCLUDE(${CMAKE_CURRENT_SOURCE_DIR}/tools/files.cmake)
IF(WIN32)
    # Include an RC file and HiDPI manifest for Windows (provided by wxWidgets)
    SET(wxUSE_DPI_AWARE_MANIFEST 1)
    LIST(APPEND APP_SRC_FILES ${CMAKE_CURRENT_SOURCE_DIR}/ReadStudio.rc)
    LIST(APPEND APP_SRC_FILES "${CMAKE_CURRENT_SOURCE_DIR}/../wxWidgets/include/wx/msw/wx_dpi_aware.manifest")
ELSEIF(APPLE)
    # Add an icon for the apple .app file
    LIST(APPEND APP_SRC_FILES ${CMAKE_CURRENT_SOURCE_DIR}/app/wxmac.icns)
ENDIF()
ADD_EXECUTABLE(${PROJECT_NAME} WIN32 MACOSX_BUNDLE ${APP_SRC_FILES} ../Licensing/CyoDecode.cpp ../Licensing/CyoEncode.cpp ../Licensing/LicenseAdmin.cpp ../Licensing/LicenseDlg.cpp ../Licensing/Rijndael.cpp)

# Set definitions, warnings, and optimizations (will propagate to the demo project also)
IF(MSVC)
    TARGET_COMPILE_DEFINITIONS(${PROJECT_NAME} PUBLIC __WXMSW__ _CRT_SECURE_NO_WARNINGS CRCPP_USE_CPP11 LICENSE_FILE_GENERATOR_ENABLED _UNICODE
        $<$<CONFIG:Debug>:__WXDEBUG__ ENABLE_PROFILING> $<$<CONFIG:Release>:NDEBUG>)
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
    TARGET_COMPILE_OPTIONS(${PROJECT_NAME} PUBLIC /Zc:__cplusplus /MP /W3 /WX /wd6211 /openmp
                           $<$<CONFIG:Debug>:/Od> $<$<CONFIG:Release>:/O2>)
ELSEIF(MINGW OR MSYS)
    TARGET_COMPILE_DEFINITIONS(${PROJECT_NAME} PUBLIC __WXMSW__ CRCPP_USE_CPP11 LICENSE_FILE_GENERATOR_ENABLED _UNICODE
            $<$<CONFIG:Debug>:__WXDEBUG__ ENABLE_PROFILING> $<$<CONFIG:Release>:NDEBUG>)
    TARGET_COMPILE_OPTIONS(${PROJECT_NAME} PUBLIC -Wall -Wextra -Wpedantic -Wshadow -Werror
                           $<$<CONFIG:Debug>:-Og> $<$<CONFIG:Release>:-O2>)
# TODO: Experimental code for macOS
ELSEIF(APPLE)
    TARGET_COMPILE_DEFINITIONS(${PROJECT_NAME} PUBLIC __WXOSX__ CRCPP_USE_CPP11 LICENSE_FILE_GENERATOR_ENABLED
        $<$<CONFIG:Debug>:__WXDEBUG__ ENABLE_PROFILING> $<$<CONFIG:Release>:NDEBUG>)
    TARGET_COMPILE_OPTIONS(${PROJECT_NAME} PUBLIC -Wall -Wextra -Wpedantic -Wshadow -Werror
                           $<$<CONFIG:Debug>:-Og> $<$<CONFIG:Release>:-O2>)
ELSEIF(UNIX)
    TARGET_COMPILE_DEFINITIONS(${PROJECT_NAME} PUBLIC __WXGTK__ CRCPP_USE_CPP11 LICENSE_FILE_GENERATOR_ENABLED
        $<$<CONFIG:Debug>:__WXDEBUG__ ENABLE_PROFILING> $<$<CONFIG:Release>:NDEBUG>)
    TARGET_COMPILE_OPTIONS(${PROJECT_NAME} PUBLIC -Wall -Wextra -Wpedantic -Wshadow -Werror
                           $<$<CONFIG:Debug>:-Og> $<$<CONFIG:Release>:-O2>)
ENDIF()

# Define the build target for the executable and copy it into the "app" subfolder
MESSAGE(STATUS "Building the program...")

SET_TARGET_PROPERTIES(${PROJECT_NAME} PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/build")
SET_TARGET_PROPERTIES(${PROJECT_NAME} PROPERTIES LINKER_LANGUAGE CXX)

# Link required libraries to the executable
IF(WIN32)
    TARGET_LINK_LIBRARIES(${PROJECT_NAME} ${wxWidgets_LIBRARIES})
ELSEIF(UNIX)
    # Add OpenMP for mutlithreading
    FIND_PACKAGE(OpenMP)
    FIND_LIBRARY(TBB_LIB tbb)
    TARGET_LINK_LIBRARIES(${PROJECT_NAME} ${wxWidgets_LIBRARIES} ${TBB_LIB} OpenMP::OpenMP_CXX)
ENDIF()

# Copy help, scripts, and resources into build folder
MESSAGE(STATUS "Copying application resources...")
ADD_CUSTOM_COMMAND(TARGET ${PROJECT_NAME}
                   POST_BUILD
                   COMMAND ${CMAKE_COMMAND} -E copy_directory
                   ${CMAKE_CURRENT_SOURCE_DIR}/docs/ReadabilityStudioDocs/docs $<TARGET_FILE_DIR:${PROJECT_NAME}>/ReadabilityStudioDocs)
ADD_CUSTOM_COMMAND(TARGET ${PROJECT_NAME}
                   POST_BUILD
                   COMMAND ${CMAKE_COMMAND} -E copy_directory
                   ${CMAKE_CURRENT_SOURCE_DIR}/docs/ShortcutsCheatsheet/docs $<TARGET_FILE_DIR:${PROJECT_NAME}>/ReadabilityStudioDocs)
ADD_CUSTOM_COMMAND(TARGET ${PROJECT_NAME}
                   POST_BUILD
                   COMMAND ${CMAKE_COMMAND} -E copy_directory
                   ${CMAKE_CURRENT_SOURCE_DIR}/docs/ReadabilityTestsReference/docs $<TARGET_FILE_DIR:${PROJECT_NAME}>/ReadabilityStudioDocs)
ADD_CUSTOM_COMMAND(TARGET ${PROJECT_NAME}
                   POST_BUILD
                   COMMAND ${CMAKE_COMMAND} -E copy_directory
                   ${CMAKE_CURRENT_SOURCE_DIR}/docs/lua-5.4/doc $<TARGET_FILE_DIR:${PROJECT_NAME}>/ReadabilityStudioDocs/lua-5.4/doc)

ADD_CUSTOM_COMMAND(TARGET ${PROJECT_NAME}
                   POST_BUILD
                   COMMAND ${CMAKE_COMMAND} -E copy_directory
                   ${CMAKE_CURRENT_SOURCE_DIR}/Resources/Scripting $<TARGET_FILE_DIR:${PROJECT_NAME}>)

ADD_CUSTOM_COMMAND(TARGET ${PROJECT_NAME}
                   POST_BUILD
                   COMMAND ${CMAKE_COMMAND} -E copy_directory
                   ${CMAKE_CURRENT_SOURCE_DIR}/Resources/Themes $<TARGET_FILE_DIR:${PROJECT_NAME}>/Themes)
ADD_CUSTOM_COMMAND(TARGET ${PROJECT_NAME}
                   POST_BUILD
                   COMMAND ${CMAKE_COMMAND} -E copy_directory
                   ${CMAKE_CURRENT_SOURCE_DIR}/Resources/ReportThemes $<TARGET_FILE_DIR:${PROJECT_NAME}>/ReportThemes)
# Zip up images and menu resources
ADD_CUSTOM_COMMAND(TARGET ${PROJECT_NAME}
                   POST_BUILD
                   COMMAND ${CMAKE_COMMAND} -E tar "cvf" "$<TARGET_FILE_DIR:${PROJECT_NAME}>/res.wad" --format=zip --files-from="${CMAKE_CURRENT_SOURCE_DIR}/tools/images.cmake"
                   COMMAND ${CMAKE_COMMAND} -E copy "$<TARGET_FILE_DIR:${PROJECT_NAME}>/res.wad" "${CMAKE_CURRENT_SOURCE_DIR}/installers/resources/"
                   COMMENT "Building resourse bundle and copying to build and installer folders."
                   WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/Resources/Images")
# Word lists
ADD_CUSTOM_COMMAND(TARGET ${PROJECT_NAME}
                   POST_BUILD
                   COMMAND ${CMAKE_COMMAND} -E tar "cvf" "$<TARGET_FILE_DIR:${PROJECT_NAME}>/Words.wad" --format=zip --files-from="${CMAKE_CURRENT_SOURCE_DIR}/tools/words.cmake"
                   COMMAND ${CMAKE_COMMAND} -E copy "$<TARGET_FILE_DIR:${PROJECT_NAME}>/Words.wad" "${CMAKE_CURRENT_SOURCE_DIR}/installers/resources/"
                   COMMENT "Building word lists and copying to build and installer folders."
                   WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/Resources/Words")

# Copy application to installer folder (if a production build)
IF(${CMAKE_BUILD_TYPE} STREQUAL "Release")
    ADD_CUSTOM_COMMAND(TARGET ${PROJECT_NAME}
                       POST_BUILD
                       COMMAND ${CMAKE_COMMAND} -E copy "$<TARGET_FILE:${PROJECT_NAME}>" "${CMAKE_CURRENT_SOURCE_DIR}/installers/Windows/Release"
                       COMMENT "Copying binary to installer folder.")
ENDIF()

IF(APPLE)
    SET_TARGET_PROPERTIES(${PROJECT_NAME} PROPERTIES
        RESOURCE "app/wxmac.icns"
        MACOSX_BUNDLE_ICON_FILE wxmac.icns
        MACOSX_BUNDLE_COPYRIGHT "Copyright Blake Madden"
        MACOSX_BUNDLE_GUI_IDENTIFIER "org.readabilitystudio.app")
ENDIF()
