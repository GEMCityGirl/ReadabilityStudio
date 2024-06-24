#############################################################################
# Name:        CMakeLists.txt
# Purpose:     Build script for Readability Studio
# Author:      Blake Madden
# Created:     2023-07-31
# Copyright:   (c) 2006-2024 Blake Madden
# License:     3-Clause BSD license
#############################################################################

# Build targets:
# readstudio: builds the main application
# manuals: builds the manuals and HTML help
# doxygen-docs: builds the API documentation (this is not included with ALL)

# Options:
# USE_ADDRESS_SANITIZE to use ASAN

# Declare the minimum required CMake version
cmake_minimum_required(VERSION 3.25)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Edit these values when bumping the release
project(readstudio VERSION 2021.4.0.0 DESCRIPTION "Readability Studio" HOMEPAGE_URL https://www.oleandersoftware.com/)
set(PROGRAM_NUMBERED_VERSION 8.0)
set(PUBLISHER "Oleander Software, Ltd.")
set(PROGRAM_AUTHOR "Blake Madden")
set(PROGRAM_COMMENTS "Readability analysis software")

# Build a displayable version string that only includes the necessary parts
set(FULL_DISPLAY_VERSION ${readstudio_VERSION_MAJOR})
if(NOT ${readstudio_VERSION_MINOR} EQUAL 0)
    string(APPEND FULL_DISPLAY_VERSION ".${readstudio_VERSION_MINOR}")
endif()
if(NOT ${readstudio_VERSION_PATCH} EQUAL 0)
    string(APPEND FULL_DISPLAY_VERSION ".${readstudio_VERSION_PATCH}")
endif()
if(NOT ${readstudio_VERSION_TWEAK} EQUAL 0)
    string(APPEND FULL_DISPLAY_VERSION ".${readstudio_VERSION_TWEAK}")
endif()
string(TIMESTAMP BUILD_YEAR "%Y")

message(STATUS "${readstudio_DESCRIPTION}")
message(STATUS "${PROGRAM_COMMENTS}")
message(STATUS "Full version: ${readstudio_VERSION_MAJOR}.${readstudio_VERSION_MINOR}.${readstudio_VERSION_PATCH}.${readstudio_VERSION_TWEAK}")
message(STATUS "Display version: ${FULL_DISPLAY_VERSION}")
message(STATUS "Numbered Version: ${PROGRAM_NUMBERED_VERSION}")
message(STATUS "Built in: ${BUILD_YEAR}")
message(STATUS "Authored by: ${PROGRAM_AUTHOR}")
message(STATUS "Published by: ${PUBLISHER}")
message(STATUS "Homepage: ${readstudio_HOMEPAGE_URL}")
message(STATUS "Output program file: ${CMAKE_PROJECT_NAME}")
message(STATUS "Writing program info to build peripheral files.")
configure_file("${CMAKE_CURRENT_SOURCE_DIR}/src/version.h.in"
               "${CMAKE_CURRENT_SOURCE_DIR}/src/app/version.h")
configure_file("${CMAKE_CURRENT_SOURCE_DIR}/src/citations.bib.in"
               "${CMAKE_CURRENT_SOURCE_DIR}/src/app/citations.bib")
configure_file("${CMAKE_CURRENT_SOURCE_DIR}/resource.h.in"
               "${CMAKE_CURRENT_SOURCE_DIR}/resource.h")
configure_file("${CMAKE_CURRENT_SOURCE_DIR}/docs/version_info.txt.in"
               "${CMAKE_CURRENT_SOURCE_DIR}/docs/version_info.txt")
configure_file("${CMAKE_CURRENT_SOURCE_DIR}/docs/_variables.yml.in"
               "${CMAKE_CURRENT_SOURCE_DIR}/docs/_variables.yml")
configure_file("${CMAKE_CURRENT_SOURCE_DIR}/installers/Windows/setup.isl.in"
               "${CMAKE_CURRENT_SOURCE_DIR}/installers/Windows/setup.isl")
configure_file("${CMAKE_CURRENT_SOURCE_DIR}/installers/Windows/icons.isl.in"
               "${CMAKE_CURRENT_SOURCE_DIR}/installers/Windows/icons.isl")

if(NOT CMAKE_CONFIGURATION_TYPES)
    get_property(HAVE_MULTI_CONFIG_GENERATOR GLOBAL PROPERTY GENERATOR_IS_MULTI_CONFIG)
    # Set default configuration types for multi-config generators
    if(HAVE_MULTI_CONFIG_GENERATOR)
        SET(CMAKE_CONFIGURATION_TYPES "Debug;Release")
    endif()
endif()

# https://blog.kitware.com/cmake-and-the-default-build-type/
if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
    set(default_build_type "Debug")
    message(STATUS "Setting build type to '${default_build_type}' as none was specified.")
    set(CMAKE_BUILD_TYPE "${default_build_type}" CACHE STRING "Choose the type of build." FORCE)
    # Set the possible values of build type for cmake-gui
    set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "Debug" "Release")
endif()

message(STATUS "Build: ${CMAKE_BUILD_TYPE}")
if(TARGET manuals)
    message(STATUS "Help build has been requested.")
endif()
if(BUILD_TESTING)
    message(STATUS "Unit test run has been requested.")
endif()

# Setup wxWidgets
##############################################################

# Find wxWidgets (wxWidgetsConfig.cmake, specifically) and select its components
if(WIN32)
    set(wxWidgets_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../wxWidgets/out/install/x64-${CMAKE_BUILD_TYPE}/lib/cmake/wxWidgets")
elseif(UNIX)
    set(wxWidgets_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../wxWidgets/wxlib/lib/cmake/wxWidgets")
endif()

message(STATUS "Adding wxWidgets CMake config folder: ${wxWidgets_DIR}")
if(NOT EXISTS "${wxWidgets_DIR}/wxWidgetsConfig.cmake" AND NOT EXISTS "${wxWidgets_DIR}/wxwidgets-config.cmake")
    message(WARNING "Cannot find wxWidgets CMake configuration. (Looked in ${wxWidgets_DIR}.)")
endif()
set(wxWidgets_USE_STATIC 1)
find_package(wxWidgets 3.3 COMPONENTS core base xrc propgrid xml aui ribbon qa stc net richtext REQUIRED CONFIG)

message(STATUS "Adding wxWidgets libraries: ${wxWidgets_LIBRARIES}")
message(STATUS "Adding wxWidgets: ${wxPLATFORM_LIB_DIR}")

if(UNIX AND NOT APPLE)
    # GTK 3 dependency
    find_package(PkgConfig REQUIRED)
    pkg_check_modules(GTK3 REQUIRED gtk+-3.0)

    # Setup CMake to use GTK+, tell the compiler where to look for headers
    # and to the linker where to look for libraries
    include_directories(${GTK3_INCLUDE_DIRS})
    link_directories(${GTK3_LIBRARY_DIRS})
endif()

# Run ALL unit-test suites for this program and its submodules
##############################################################
if(BUILD_TESTING)
    set(BUILD_TEMP ${CMAKE_CURRENT_SOURCE_DIR}/BuildTemp)
    file(REMOVE_RECURSE ${BUILD_TEMP})
    file(REMOVE_RECURSE ${CMAKE_CURRENT_SOURCE_DIR}/TestResults)
    file(MAKE_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/TestResults)

    # submodule test suites
    function(RUN_TEST_SUITE TEST_PROJECT TEST_FOLDER TEST_RUNNER)
        execute_process(COMMAND ${CMAKE_COMMAND} -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}
                                -S ${CMAKE_CURRENT_SOURCE_DIR}/src/${TEST_PROJECT}/${TEST_FOLDER} -B ${BUILD_TEMP})
        execute_process(COMMAND ${CMAKE_COMMAND} --build ${BUILD_TEMP} --config ${CMAKE_BUILD_TYPE})
        execute_process(COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/src/${TEST_PROJECT}/${TEST_FOLDER}/bin/${CMAKE_BUILD_TYPE}/${TEST_RUNNER}
                                --reporter=junit --out=${CMAKE_CURRENT_SOURCE_DIR}/TestResults/${TEST_RUNNER}-results.xml
                        WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/src/${TEST_PROJECT}/${TEST_FOLDER}/bin/${CMAKE_BUILD_TYPE}")
        file(REMOVE_RECURSE ${BUILD_TEMP})
    endfunction()

    # test suites for RS specifically
    function(RUN_RS_TEST_SUITE TEST_FOLDER TEST_RUNNER)
        execute_process(COMMAND ${CMAKE_COMMAND} -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}
                                -S ${CMAKE_CURRENT_SOURCE_DIR}/${TEST_FOLDER} -B ${BUILD_TEMP})
        execute_process(COMMAND ${CMAKE_COMMAND} --build ${BUILD_TEMP} --config ${CMAKE_BUILD_TYPE})
        execute_process(COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/${TEST_FOLDER}/bin/${CMAKE_BUILD_TYPE}/${TEST_RUNNER}
                                --reporter=junit --out=${CMAKE_CURRENT_SOURCE_DIR}/TestResults/${TEST_RUNNER}-results.xml
                        WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/${TEST_FOLDER}/bin/${CMAKE_BUILD_TYPE}")
        file(REMOVE_RECURSE ${BUILD_TEMP})
    endfunction()

    run_test_suite(OleanderStemmingLibrary tests OleanderStemTestRunner)
    run_test_suite(Wisteria-Dataviz/src/i18n-check tests i18nTestRunner)
    run_test_suite(tinyexpr-plusplus tests TETestRunner)
    run_test_suite(Wisteria-Dataviz tests WisteriaTestRunner)
    run_test_suite(Wisteria-Dataviz tests/gui-tests WisteriaGuiTestRunner)
    run_rs_test_suite(tests RSTestRunner)
    run_rs_test_suite(tests/gui-tests RSGuiTestRunner)
    message(STATUS "Unit test results availabe in ${CMAKE_CURRENT_SOURCE_DIR}/TestResults.")
endif()

# Help Files
##############################################################

include(cmake/modules/FindR.cmake)
# TODO: Windows path to Pandoc currently only looks for Quarto installations.
include(cmake/modules/FindPandoc.cmake)
if(TARGET manuals)
    if(NOT R_FOUND)
        message(FATAL_ERROR "R not found. Help cannot be built.")
    elseif(NOT PANDOC_FOUND)
        message(FATAL_ERROR "Pandoc not found. Help cannot be built.")
    endif()
endif()

add_custom_target(manuals
                ALL
                COMMAND "${R_SCRIPT}"
                    "${CMAKE_CURRENT_SOURCE_DIR}/docs/build-help-projects.R"
                    "${PANDOC_ROOT_DIR}"
                WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/docs/")

# Build the API documentation
########################
find_package(Doxygen)
if(TARGET doxygen-docs)
    if(NOT DOXYGEN_FOUND)
        message("Doxygen not found. API documentation cannot be generated.")
    endif()
endif()

add_custom_target(doxygen-docs
            COMMAND ${DOXYGEN_EXECUTABLE} "${CMAKE_SOURCE_DIR}/docs/doxygen/Doxyfile"
            WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}/docs/doxygen"
            COMMENT "Generating API documentation with Doxygen"
            VERBATIM)

if(APPLE AND NOT CMAKE_OSX_DEPLOYMENT_TARGET)
    # If no deployment target has been set default to the minimum supported
    # OS version (this has to be set before the first project() call)
    if(CMAKE_SYSTEM_NAME STREQUAL "iOS")
        set(CMAKE_OSX_DEPLOYMENT_TARGET 12.0 CACHE STRING "iOS Deployment Target")
    else()
        set(CMAKE_OSX_DEPLOYMENT_TARGET 10.10 CACHE STRING "macOS Deployment Target")
    endif()
endif()

add_compile_options("$<$<C_COMPILER_ID:MSVC>:/utf-8>")
add_compile_options("$<$<CXX_COMPILER_ID:MSVC>:/utf-8>")

# Build binary
##############################################################

# (run "tools/Build CMake Files List.R" to update this)
include(${CMAKE_CURRENT_SOURCE_DIR}/tools/files.cmake)
if(WIN32)
    # Include an RC file and HiDPI manifest for Windows (provided by wxWidgets)
    set(wxUSE_DPI_AWARE_MANIFEST 1)
    list(APPEND APP_SRC_FILES ${CMAKE_CURRENT_SOURCE_DIR}/readstudio.rc)
    list(APPEND APP_SRC_FILES "${CMAKE_CURRENT_SOURCE_DIR}/../wxWidgets/include/wx/msw/wx_dpi_aware.manifest")
elseif(APPLE)
    # Add an icon for the apple .app file
    list(APPEND APP_SRC_FILES ${CMAKE_CURRENT_SOURCE_DIR}/app/wxmac.icns)
endif()
add_executable(${PROJECT_NAME} WIN32 MACOSX_BUNDLE ${APP_SRC_FILES} ../Licensing/CyoDecode.cpp ../Licensing/CyoEncode.cpp ../Licensing/LicenseAdmin.cpp ../Licensing/Rijndael.cpp)

# Set definitions, warnings, and optimizations
if(MSVC)
    # _DISABLE_VECTOR_ANNOTATION _DISABLE_STRING_ANNOTATION are needed for ASAN not being enabled in linked wxWidgets libs
    # https://learn.microsoft.com/en-us/cpp/sanitizers/error-container-overflow?view=msvc-170
    target_compile_definitions(${PROJECT_NAME} PUBLIC __WXMSW__ CRCPP_USE_CPP11 LICENSE_FILE_GENERATOR_ENABLED _UNICODE
        $<$<CONFIG:Debug>:__WXDEBUG__ ENABLE_PROFILING _DISABLE_VECTOR_ANNOTATION _DISABLE_STRING_ANNOTATION> $<$<CONFIG:Release>:NDEBUG>)
    # /Zc:__cplusplus tells MSVC to set the C++ version what we are
    # actually compiling as. The default behavior in MSVC is to say that the
    # C++ version is 98 always (for compatibility reasons).

    # /MP enables multi-core build support, speeding up compilation time.

    # /W3 cranks up the warning level, /WX treats all warnings as error.
    # Note that simply using /W4 causes a warning that it is overriding /W3,
    # and then that turns into an error because of /W4. So /W3 is the
    # highest you can go with /WX.

    # /wd6211 turns off C6211 warning: leaking memory due to an exception.
    # wxWidgets uses heap-based objects for most everything, and MVSC complains
    # about not wrapping all of this logic in try blocks.
    if(USE_ADDRESS_SANITIZE)
        target_compile_options(${PROJECT_NAME} PUBLIC /Zc:__cplusplus /MP /W3 /WX /wd6211 /openmp
                               $<$<CONFIG:Debug>:/Od /fsanitize=address>
                               $<$<CONFIG:Release>:/O2>)
    else()
        target_compile_options(${PROJECT_NAME} PUBLIC /Zc:__cplusplus /MP /W3 /WX /wd6211 /openmp
                               $<$<CONFIG:Debug>:/Od>
                               $<$<CONFIG:Release>:/O2>)
    endif()
elseif(MINGW OR MSYS)
    target_compile_definitions(${PROJECT_NAME} PUBLIC __WXMSW__ CRCPP_USE_CPP11 LICENSE_FILE_GENERATOR_ENABLED _UNICODE
            $<$<CONFIG:Debug>:__WXDEBUG__ ENABLE_PROFILING> $<$<CONFIG:Release>:NDEBUG>)
    target_compile_options(${PROJECT_NAME} PUBLIC -Wall -Wextra -Wpedantic -Wshadow -Werror
                           $<$<CONFIG:Debug>:-Og>
                           $<$<CONFIG:Release>:-O2>)
# TODO: Experimental code for macOS
elseif(APPLE)
    target_compile_definitions(${PROJECT_NAME} PUBLIC __WXOSX__ CRCPP_USE_CPP11 LICENSE_FILE_GENERATOR_ENABLED
        $<$<CONFIG:Debug>:__WXDEBUG__ ENABLE_PROFILING> $<$<CONFIG:Release>:NDEBUG>)
    # TODO: don't include -Werror for now, but eventually add it
    target_compile_options(${PROJECT_NAME} PUBLIC -Wall -Wextra -Wpedantic -Wshadow
                           $<$<CONFIG:Debug>:-Og>
                           $<$<CONFIG:Release>:-O2>)
elseif(UNIX)
    target_compile_definitions(${PROJECT_NAME} PUBLIC __WXGTK__ CRCPP_USE_CPP11 LICENSE_FILE_GENERATOR_ENABLED
        $<$<CONFIG:Debug>:__WXDEBUG__ ENABLE_PROFILING> $<$<CONFIG:Release>:NDEBUG>)
    # TODO: don't include -Werror for now, but eventually add it
    target_compile_options(${PROJECT_NAME} PUBLIC -Wall -Wextra -Wpedantic -Wshadow
                           $<$<CONFIG:Debug>:-Og -g3>
                           $<$<CONFIG:Release>:-O2>)
endif()

# Define the build target for the executable and copy it into the "app" subfolder
set_target_properties(${PROJECT_NAME} PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/build")
set_target_properties(${PROJECT_NAME} PROPERTIES LINKER_LANGUAGE CXX)

# Link required libraries to the executable
if(WIN32)
    target_link_libraries(${PROJECT_NAME} ${wxWidgets_LIBRARIES})
elseif(UNIX)
    # Add OpenMP for mutlithreading
    find_package(OpenMP)
    find_library(TBB_LIB tbb)
    target_link_libraries(${PROJECT_NAME} ${wxWidgets_LIBRARIES} ${TBB_LIB} OpenMP::OpenMP_CXX)
endif()

# Copy help, scripts, and resources into build folder
##############################################################
message(STATUS "Copying application resources...")

# make help directories if missing to avoid a build error
if(NOT EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/docs/readability-studio-manual/docs/")
    add_custom_command(TARGET ${PROJECT_NAME}
                   POST_BUILD
                   COMMAND ${CMAKE_COMMAND} -E make_directory
                   "${CMAKE_CURRENT_SOURCE_DIR}/docs/readability-studio-manual/docs/")
endif()
if(NOT EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/docs/readability-test-reference/docs/")
    add_custom_command(TARGET ${PROJECT_NAME}
                   POST_BUILD
                   COMMAND ${CMAKE_COMMAND} -E make_directory
                   "${CMAKE_CURRENT_SOURCE_DIR}/docs/readability-test-reference/docs/")
endif()
if(NOT EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/docs/shortcuts-cheatsheet/docs/")
    add_custom_command(TARGET ${PROJECT_NAME}
                   POST_BUILD
                   COMMAND ${CMAKE_COMMAND} -E make_directory
                   "${CMAKE_CURRENT_SOURCE_DIR}/docs/shortcuts-cheatsheet/docs/")
endif()
if(NOT EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/docs/release-notes/docs/")
    add_custom_command(TARGET ${PROJECT_NAME}
                   POST_BUILD
                   COMMAND ${CMAKE_COMMAND} -E make_directory
                   "${CMAKE_CURRENT_SOURCE_DIR}/docs/release-notes/docs/")
endif()

add_custom_command(TARGET ${PROJECT_NAME}
                   POST_BUILD
                   COMMAND ${CMAKE_COMMAND} -E copy_directory
                   ${CMAKE_CURRENT_SOURCE_DIR}/docs/readability-studio-manual/docs $<TARGET_FILE_DIR:${PROJECT_NAME}>/readability-studio-manual)
add_custom_command(TARGET ${PROJECT_NAME}
                   POST_BUILD
                   COMMAND ${CMAKE_COMMAND} -E copy_directory
                   ${CMAKE_CURRENT_SOURCE_DIR}/docs/shortcuts-cheatsheet/docs $<TARGET_FILE_DIR:${PROJECT_NAME}>/readability-studio-manual)
add_custom_command(TARGET ${PROJECT_NAME}
                   POST_BUILD
                   COMMAND ${CMAKE_COMMAND} -E copy_directory
                   ${CMAKE_CURRENT_SOURCE_DIR}/docs/readability-test-reference/docs $<TARGET_FILE_DIR:${PROJECT_NAME}>/readability-studio-manual)
add_custom_command(TARGET ${PROJECT_NAME}
                   POST_BUILD
                   COMMAND ${CMAKE_COMMAND} -E copy_directory
                   ${CMAKE_CURRENT_SOURCE_DIR}/docs/release-notes/docs $<TARGET_FILE_DIR:${PROJECT_NAME}>/readability-studio-manual)
add_custom_command(TARGET ${PROJECT_NAME}
                   POST_BUILD
                   COMMAND ${CMAKE_COMMAND} -E copy_directory
                   ${CMAKE_CURRENT_SOURCE_DIR}/docs/lua-5.4/doc $<TARGET_FILE_DIR:${PROJECT_NAME}>/readability-studio-manual/lua-5.4/doc)

add_custom_command(TARGET ${PROJECT_NAME}
                   POST_BUILD
                   COMMAND ${CMAKE_COMMAND} -E copy_directory
                   ${CMAKE_CURRENT_SOURCE_DIR}/resources/scripting $<TARGET_FILE_DIR:${PROJECT_NAME}>)

add_custom_command(TARGET ${PROJECT_NAME}
                   POST_BUILD
                   COMMAND ${CMAKE_COMMAND} -E copy_directory
                   ${CMAKE_CURRENT_SOURCE_DIR}/resources/themes $<TARGET_FILE_DIR:${PROJECT_NAME}>/themes)
add_custom_command(TARGET ${PROJECT_NAME}
                   POST_BUILD
                   COMMAND ${CMAKE_COMMAND} -E copy_directory
                   ${CMAKE_CURRENT_SOURCE_DIR}/resources/report-themes $<TARGET_FILE_DIR:${PROJECT_NAME}>/report-themes)

add_custom_command(TARGET ${PROJECT_NAME}
                   POST_BUILD
                   COMMAND ${CMAKE_COMMAND} -E copy_directory
                   ${CMAKE_CURRENT_SOURCE_DIR}/examples $<TARGET_FILE_DIR:${PROJECT_NAME}>/examples)
# License file
add_custom_command(TARGET ${PROJECT_NAME}
                   POST_BUILD
                   COMMAND ${CMAKE_COMMAND} -E copy
                   ${CMAKE_CURRENT_SOURCE_DIR}/installers/redist/license.rtf $<TARGET_FILE_DIR:${PROJECT_NAME}>/)
# Zip up images and menu resources
file(MAKE_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/installers/resources/")
add_custom_command(TARGET ${PROJECT_NAME}
                   POST_BUILD
                   COMMAND ${CMAKE_COMMAND} -E tar "cvf" "$<TARGET_FILE_DIR:${PROJECT_NAME}>/res.wad" --format=zip --files-from="${CMAKE_CURRENT_SOURCE_DIR}/tools/images.cmake"
                   COMMAND ${CMAKE_COMMAND} -E copy "$<TARGET_FILE_DIR:${PROJECT_NAME}>/res.wad" "${CMAKE_CURRENT_SOURCE_DIR}/installers/resources/"
                   COMMENT "Building resourse bundle and copying to build and installer folders."
                   WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/resources/images")
# Word lists
add_custom_command(TARGET ${PROJECT_NAME}
                   POST_BUILD
                   COMMAND ${CMAKE_COMMAND} -E tar "cvf" "$<TARGET_FILE_DIR:${PROJECT_NAME}>/words.wad" --format=zip --files-from="${CMAKE_CURRENT_SOURCE_DIR}/tools/words.cmake"
                   COMMAND ${CMAKE_COMMAND} -E copy "$<TARGET_FILE_DIR:${PROJECT_NAME}>/words.wad" "${CMAKE_CURRENT_SOURCE_DIR}/installers/resources/"
                   COMMENT "Building word lists and copying to build and installer folders."
                   WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/resources/words")

# Copy application to installer folder (if a production build)
if(${CMAKE_BUILD_TYPE} STREQUAL "Release")
    add_custom_command(TARGET ${PROJECT_NAME}
                       POST_BUILD
                       COMMAND ${CMAKE_COMMAND} -E copy "$<TARGET_FILE:${PROJECT_NAME}>" "${CMAKE_CURRENT_SOURCE_DIR}/installers/Windows/Release"
                       COMMENT "Copying binary to installer folder.")
endif()

if(APPLE)
    set_target_properties(${PROJECT_NAME} PROPERTIES
        RESOURCE "app/wxmac.icns"
        MACOSX_BUNDLE_ICON_FILE wxmac.icns
        MACOSX_BUNDLE_COPYRIGHT "Copyright Blake Madden"
        MACOSX_BUNDLE_GUI_IDENTIFIER "org.readabilitystudio.app")
endif()
