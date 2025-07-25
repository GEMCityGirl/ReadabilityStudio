#############################################################################
# Name:        CMakeLists.txt
# Purpose:     Build script for Readability Studio
# Author:      Blake Madden
# Created:     2023-07-31
# Copyright:   (c) 2025 Blake Madden
# License:     Eclipse Public License 2.0
#############################################################################

# Build targets:
# readstudio: builds the main application (included with ALL)
# manuals: builds the manuals and HTML help
# doxygen-docs: builds the API documentation (included with ALL)

# Options:
# USE_ADDRESS_SANITIZE to use ASAN (ignored in release build)

# Requirements
cmake_minimum_required(VERSION 3.25)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(MIN_GCC_REQUIRED "12.2.1")
set(MIN_APPLECLANG_REQUIRED "15.3")
set(MIN_MSVC_REQUIRED "19.30") # VS 2022 (1930 is the build number, which is what _MSC_VER will be set to)

# https://blog.kitware.com/cmake-and-the-default-build-type/
if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
    set(default_build_type "Debug")
    message(STATUS "Setting build type to '${default_build_type}' as none was specified.")
    set(CMAKE_BUILD_TYPE "${default_build_type}" CACHE STRING "Choose the type of build." FORCE)
    # Set the possible values of build type for cmake-gui
    set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "Debug" "Release")
endif()

message(STATUS "Build: ${CMAKE_BUILD_TYPE}")
message(STATUS "Generator: ${CMAKE_GENERATOR}")

if(APPLE)
    set(CMAKE_OSX_ARCHITECTURES "arm64;x86_64" CACHE STRING "" FORCE)
    # Enable hardened runtime:
	# https://developer.apple.com/documentation/security/hardened_runtime
    # Note that this will need to be set via codesign later because we are not
    # codesigning via CMake. This here is more for bookkeeping.
    set(CMAKE_XCODE_ATTRIBUTE_ENABLE_HARDENED_RUNTIME YES)
endif()

# Always run these, even if not APPLE, as we use ${CMAKE_OSX_DEPLOYMENT_TARGET} for the docs.
#
# If no deployment target has been set default to the minimum supported
# OS version (this has to be set before the first project() call)
if(APPLE AND CMAKE_SYSTEM_NAME STREQUAL "iOS")
    set(CMAKE_OSX_DEPLOYMENT_TARGET 12.0 CACHE STRING "iOS Deployment Target")
else()
    set(CMAKE_OSX_DEPLOYMENT_TARGET 10.15 CACHE STRING "macOS Deployment Target")
endif()
message(STATUS "macOS deployment target: ${CMAKE_OSX_DEPLOYMENT_TARGET}")

set(UNIX_ARCHITECTURE "x86_64" CACHE STRING "" FORCE)

# Only used for the docs
set(WIN32_DEPLOYMENT_TARGET 10)
set(MIN_RAM "2 GB")
set(RAM_RECOMMENDED "4 GB")

# Edit these values when bumping the release version
project(readstudio VERSION 2025.0.0.0 DESCRIPTION "Readability Studio" HOMEPAGE_URL https://www.oleandersoftware.com/)
set(PROGRAM_LONGNAME "Eclipse ${readstudio_DESCRIPTION}")
set(PROGRAM_NUMBERED_VERSION 9.0)
set(PUBLISHER "Eclipse Foundation AISBL")
set(PROGRAM_AUTHOR "Blake Madden")
set(PROGRAM_AUTHOR_MLA "Madden, Blake")
set(PROGRAM_AUTHOR_APA "Madden, B.")
set(PROGRAM_COMMENTS "Readability analysis software")
set(PROGRAM_RDNS_IDENTIFIER "org.eclipse.readabilitystudio")
set(GIT_REPO "https://github.com/eclipse-ers/ReadabilityStudio.git")
set(INNOSETUP_GUID "{FBFBEAB6-622A-4E16-893D-216C25FF6A69}")
set(PROGRAM_LICENSE "Eclipse Public License 2.0")

set(FULL_VERSION ${readstudio_VERSION_MAJOR}.${readstudio_VERSION_MINOR}.${readstudio_VERSION_PATCH}.${readstudio_VERSION_TWEAK})
# Build a displayable version string that only includes the necessary parts
set(FULL_DISPLAY_VERSION ${readstudio_VERSION_MAJOR})
if(NOT ${readstudio_VERSION_MINOR} EQUAL 0 OR NOT ${readstudio_VERSION_PATCH} EQUAL 0 OR NOT ${readstudio_VERSION_TWEAK} EQUAL 0)
    string(APPEND FULL_DISPLAY_VERSION ".${readstudio_VERSION_MINOR}")
endif()
if(NOT ${readstudio_VERSION_PATCH} EQUAL 0 OR NOT ${readstudio_VERSION_TWEAK} EQUAL 0)
    string(APPEND FULL_DISPLAY_VERSION ".${readstudio_VERSION_PATCH}")
endif()
if(NOT ${readstudio_VERSION_TWEAK} EQUAL 0)
    string(APPEND FULL_DISPLAY_VERSION ".${readstudio_VERSION_TWEAK}")
endif()
string(TIMESTAMP BUILD_YEAR "%Y")

if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU" AND CMAKE_CXX_COMPILER_VERSION VERSION_LESS ${MIN_GCC_REQUIRED})
  message(FATAL_ERROR "Insufficient GCC version")
elseif(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC" AND CMAKE_CXX_COMPILER_VERSION VERSION_LESS ${MIN_MSVC_REQUIRED})
  message(FATAL_ERROR "Insufficient Visual Studio version")
elseif(CMAKE_CXX_COMPILER_ID STREQUAL "APPLECLANG" AND CMAKE_CXX_COMPILER_VERSION VERSION_LESS ${MIN_APPLECLANG_REQUIRED})
  message(FATAL_ERROR "Insufficient AppleClang version")
endif()
message(STATUS "Compiler: ${CMAKE_CXX_COMPILER_ID} ${CMAKE_CXX_COMPILER_VERSION}")

# interproceduarl optimization
if(${CMAKE_BUILD_TYPE} STREQUAL "Release")
    include(CheckIPOSupported)
    check_ipo_supported(RESULT ipo_supported)
    if(${ipo_supported})
      message(STATUS "Interproceduarl compiler optimization: enabled")
      set(CMAKE_INTERPROCEDURAL_OPTIMIZATION ${ipo_supported})
    elseif()
      message(STATUS "Interproceduarl compiler optimization: not enabled")
    endif()
endif()

# Read implemtation of Snowball used by stemming library (this appears in the help)
file(READ "${CMAKE_CURRENT_SOURCE_DIR}/src/OleanderStemmingLibrary/src/stemming.h" STEM_CONTENT)
string(REGEX MATCH "SNOWBALL_MAJOR_VERSION = ([0-9]+)" _ ${STEM_CONTENT})
set(SNOWBALL_MAJOR_VERSION ${CMAKE_MATCH_1})
string(REGEX MATCH "SNOWBALL_MINOR_VERSION = ([0-9]+)" _ ${STEM_CONTENT})
set(SNOWBALL_MINOR_VERSION ${CMAKE_MATCH_1})
message(STATUS "Snowball standard in use: ${SNOWBALL_MAJOR_VERSION}.${SNOWBALL_MINOR_VERSION}")

message(STATUS "${readstudio_DESCRIPTION}")
message(STATUS "${PROGRAM_COMMENTS}")
message(STATUS "Full version: ${readstudio_VERSION_MAJOR}.${readstudio_VERSION_MINOR}.${readstudio_VERSION_PATCH}.${readstudio_VERSION_TWEAK}")
message(STATUS "Display version: ${FULL_DISPLAY_VERSION}")
message(STATUS "Numbered Version: ${PROGRAM_NUMBERED_VERSION}")
message(STATUS "Built in: ${BUILD_YEAR}")
message(STATUS "Authored by: ${PROGRAM_AUTHOR}")
message(STATUS "Published by: ${PUBLISHER}")
message(STATUS "Homepage: ${readstudio_HOMEPAGE_URL}")
message(STATUS "Output binary: ${CMAKE_PROJECT_NAME}")
message(STATUS "Writing program info to build peripheral files")
if(WIN32)
    set(CJK_FONT "MS Gothic")
elseif(UNIX)
    set(CJK_FONT "Noto Serif CJK SC")
endif()
configure_file("${CMAKE_CURRENT_SOURCE_DIR}/src/version.h.in"
               "${CMAKE_CURRENT_SOURCE_DIR}/src/app/version.h")
configure_file("${CMAKE_CURRENT_SOURCE_DIR}/citations.bib.in"
               "${CMAKE_CURRENT_SOURCE_DIR}/citations.bib")
configure_file("${CMAKE_CURRENT_SOURCE_DIR}/citations.mla.in"
               "${CMAKE_CURRENT_SOURCE_DIR}/citations.mla")
configure_file("${CMAKE_CURRENT_SOURCE_DIR}/citations.apa.in"
               "${CMAKE_CURRENT_SOURCE_DIR}/citations.apa")
configure_file("${CMAKE_CURRENT_SOURCE_DIR}/resource.h.in"
               "${CMAKE_CURRENT_SOURCE_DIR}/resource.h")
if(UNIX AND NOT APPLE)
    configure_file("${CMAKE_CURRENT_SOURCE_DIR}/installers/unix/AppDir/readstudio.desktop.in"
                   "${CMAKE_CURRENT_SOURCE_DIR}/installers/unix/AppDir/readstudio.desktop")
    # AppImage tools have issues with building correct appdata.xml files, so turn this off for now
    # configure_file("${CMAKE_CURRENT_SOURCE_DIR}/installers/unix/AppDir/usr/share/metainfo/readstudio.appdata.xml.in"
    #               "${CMAKE_CURRENT_SOURCE_DIR}/installers/unix/AppDir/usr/share/metainfo/readstudio.appdata.xml")
elseif(WIN32)
    configure_file("${CMAKE_CURRENT_SOURCE_DIR}/installers/windows/setup.isl.in"
                   "${CMAKE_CURRENT_SOURCE_DIR}/installers/windows/setup.isl")
    configure_file("${CMAKE_CURRENT_SOURCE_DIR}/installers/windows/icons.isl.in"
                   "${CMAKE_CURRENT_SOURCE_DIR}/installers/windows/icons.isl")
    configure_file("${CMAKE_CURRENT_SOURCE_DIR}/installers/windows/readstudio.iss.in"
                   "${CMAKE_CURRENT_SOURCE_DIR}/installers/windows/readstudio.iss")
endif()
configure_file("${CMAKE_CURRENT_SOURCE_DIR}/docs/_variables.yml.in"
               "${CMAKE_CURRENT_SOURCE_DIR}/docs/_variables.yml")
configure_file("${CMAKE_CURRENT_SOURCE_DIR}/docs/sysadmin/build-commands.txt.in"
               "${CMAKE_CURRENT_SOURCE_DIR}/docs/sysadmin/build-commands.txt")
configure_file("${CMAKE_CURRENT_SOURCE_DIR}/docs/sysadmin/windows-silent.txt.in"
               "${CMAKE_CURRENT_SOURCE_DIR}/docs/sysadmin/windows-silent.txt")
configure_file("${CMAKE_CURRENT_SOURCE_DIR}/docs/readability-studio-manual/_quarto.yml.in"
               "${CMAKE_CURRENT_SOURCE_DIR}/docs/readability-studio-manual/_quarto.yml")

if(NOT CMAKE_CONFIGURATION_TYPES)
    get_property(HAVE_MULTI_CONFIG_GENERATOR GLOBAL PROPERTY GENERATOR_IS_MULTI_CONFIG)
    # Set default configuration types for multi-config generators
    if(HAVE_MULTI_CONFIG_GENERATOR)
        SET(CMAKE_CONFIGURATION_TYPES "Debug;Release")
    endif()
endif()

if(TARGET manuals)
    message(STATUS "Help build has been requested.")
endif()
if(BUILD_TESTING)
    message(STATUS "Unit test run has been requested.")
endif()

if(APPLE)
    message(STATUS "Supported architectures: ${CMAKE_OSX_ARCHITECTURES}")
else()
    message(STATUS "Architecture: ${CMAKE_SYSTEM_PROCESSOR}" )
endif()

# Setup wxWidgets (can be 1-5 folders up from here)
##############################################################
include(src/Wisteria-Dataviz/cmake/folders.cmake)

# Add wxWidgets to the build
set(wxUSE_WEBVIEW_EDGE ON)
set(wxBUILD_SHARED OFF)
if(CMAKE_BUILD_TYPE STREQUAL "Release")
    set(__WXDEBUG__ 0)
    set(wxBUILD_OPTIMISE ON)
    set(wxBUILD_STRIPPED_RELEASE ON)
endif()
find_directory("wxWidgets" wxWidgets_BASEDIR)
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/../wxWidgets" ${CMAKE_CURRENT_SOURCE_DIR}/wxWidgets_lib)
set(wxWidgets_LIBRARIES wx::core wx::base wx::xrc wx::propgrid wx::xml wx::aui wx::ribbon wx::qa wx::stc wx::net wx::richtext)
message(STATUS "Adding wxWidgets libraries: ${wxWidgets_LIBRARIES}")

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
    set(BUILD_TEMP ${CMAKE_CURRENT_SOURCE_DIR}/build-temp)
    if(EXISTS ${BUILD_TEMP})
        file(REMOVE_RECURSE ${BUILD_TEMP})
    endif()
    if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/test-results)
        file(REMOVE_RECURSE ${CMAKE_CURRENT_SOURCE_DIR}/test-results)
    endif()
    file(MAKE_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/test-results)

    # submodule test suites
    function(run_test_suite TEST_PROJECT TEST_FOLDER TEST_RUNNER)
        execute_process(COMMAND ${CMAKE_COMMAND} -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}
                                -S ${CMAKE_CURRENT_SOURCE_DIR}/src/${TEST_PROJECT}/${TEST_FOLDER} -B ${BUILD_TEMP})
        execute_process(COMMAND ${CMAKE_COMMAND} --build ${BUILD_TEMP} --config ${CMAKE_BUILD_TYPE})
        set(TEST_OUT_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src/${TEST_PROJECT}/${TEST_FOLDER}/bin)
        # MSVC tends to make a "Debug" or "Release" in your "bin" folder and put the binaries
        # in there instead of just the "bin" folder like you tell it to.
        if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/src/${TEST_PROJECT}/${TEST_FOLDER}/bin/${CMAKE_BUILD_TYPE})
            set(TEST_OUT_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src/${TEST_PROJECT}/${TEST_FOLDER}/bin)
        endif()
        message(STATUS "Running ${TEST_OUT_DIR}/${TEST_RUNNER}...")
        execute_process(COMMAND ${TEST_OUT_DIR}/${TEST_RUNNER}
                                --reporter=junit --out=${CMAKE_CURRENT_SOURCE_DIR}/test-results/${TEST_RUNNER}-results.xml
                        WORKING_DIRECTORY "${TEST_OUT_DIR}")
        file(REMOVE_RECURSE ${BUILD_TEMP})
    endfunction()

    # test suites for RS specifically
    function(run_rs_test_suite TEST_FOLDER TEST_RUNNER)
        execute_process(COMMAND ${CMAKE_COMMAND} -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}
                                -S ${CMAKE_CURRENT_SOURCE_DIR}/${TEST_FOLDER} -B ${BUILD_TEMP})
        execute_process(COMMAND ${CMAKE_COMMAND} --build ${BUILD_TEMP} --config ${CMAKE_BUILD_TYPE})
        set(TEST_OUT_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src/${TEST_PROJECT}/${TEST_FOLDER}/bin)
        if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/src/${TEST_PROJECT}/${TEST_FOLDER}/bin/${CMAKE_BUILD_TYPE})
            set(TEST_OUT_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src/${TEST_PROJECT}/${TEST_FOLDER}/bin)
        endif()
        execute_process(COMMAND ${TEST_OUT_DIR}/${TEST_RUNNER}
                                --reporter=junit --out=${CMAKE_CURRENT_SOURCE_DIR}/test-results/${TEST_RUNNER}-results.xml
                        WORKING_DIRECTORY "${TEST_OUT_DIR}")
        file(REMOVE_RECURSE ${BUILD_TEMP})
    endfunction()

    run_test_suite(OleanderStemmingLibrary tests OleanderStemTestRunner)
    run_test_suite(tinyexpr-plusplus tests TETestRunner)
    run_test_suite(Wisteria-Dataviz tests WisteriaTestRunner)
    run_test_suite(Wisteria-Dataviz tests/gui-tests WisteriaGuiTestRunner)
    run_rs_test_suite(tests RSTestRunner)
    run_rs_test_suite(tests/gui-tests RSGuiTestRunner)
    message(STATUS "Unit test results available in ${CMAKE_CURRENT_SOURCE_DIR}/test-results")
endif()

# Help Files
##############################################################

include(cmake/modules/FindR.cmake)
if(NOT R_FOUND)
    message(WARNING "R not found. Help manuals cannot be built.")
endif()

add_custom_target(manuals
                COMMAND "${R_SCRIPT}"
                    "${CMAKE_CURRENT_SOURCE_DIR}/docs/build-help-projects.R"
                WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/docs/")

# Build the API documentation
########################
find_package(Doxygen)
if(NOT DOXYGEN_FOUND)
    # Just warning about API docs not being built.
    # We will try to include it with ALL, but not necessary for end users.
    message(WARNING "Doxygen not found. API documentation cannot be built.")
else()
    set(DOXYGEN_ALL_OPTION_MAYBE_UNAVAILABLE ALL)
endif()

add_custom_target(doxygen-docs
            ${DOXYGEN_ALL_OPTION_MAYBE_UNAVAILABLE}
            COMMAND ${DOXYGEN_EXECUTABLE} "${CMAKE_SOURCE_DIR}/docs/doxygen/Doxyfile"
            WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}/docs/doxygen"
            COMMENT "Generating API documentation with Doxygen"
            VERBATIM)

add_compile_options("$<$<C_COMPILER_ID:MSVC>:/utf-8>")
add_compile_options("$<$<CXX_COMPILER_ID:MSVC>:/utf-8>")

# Build binary
##############################################################

# (run "tools/build-cmake-files-list.R" to update this)
include(${CMAKE_CURRENT_SOURCE_DIR}/tools/files.cmake)
if(WIN32)
    # Include an RC file and HiDPI manifest for Windows (provided by wxWidgets)
    set(wxUSE_DPI_AWARE_MANIFEST 1)
    list(APPEND APP_SRC_FILES ${CMAKE_CURRENT_SOURCE_DIR}/readstudio.rc)
    list(APPEND APP_SRC_FILES "${wxWidgets_BASEDIR}/include/wx/msw/wx_dpi_aware.manifest")
elseif(APPLE)
    # Add an icon for the apple .app file
    list(APPEND APP_SRC_FILES ${CMAKE_CURRENT_SOURCE_DIR}/app-logo.icns)
endif()
add_executable(${PROJECT_NAME} WIN32 MACOSX_BUNDLE ${APP_SRC_FILES})

# Set definitions, warnings, and optimizations
if(MSVC)
    # _DISABLE_VECTOR_ANNOTATION _DISABLE_STRING_ANNOTATION are needed for ASAN not being enabled in linked wxWidgets libs
    # https://learn.microsoft.com/en-us/cpp/sanitizers/error-container-overflow?view=msvc-170
    target_compile_definitions(${PROJECT_NAME} PUBLIC __WXMSW__ CRCPP_USE_CPP11 _CRT_SECURE_NO_WARNINGS _UNICODE
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
    target_compile_definitions(${PROJECT_NAME} PUBLIC __WXMSW__ CRCPP_USE_CPP11 _CRT_SECURE_NO_WARNINGS _UNICODE
            $<$<CONFIG:Debug>:__WXDEBUG__ ENABLE_PROFILING> $<$<CONFIG:Release>:NDEBUG>)
    target_compile_options(${PROJECT_NAME} PUBLIC -Wall -Wextra -Wpedantic -Wshadow -Werror
                           $<$<CONFIG:Debug>:-Og>
                           $<$<CONFIG:Release>:-O2>)
elseif(APPLE)
    target_compile_definitions(${PROJECT_NAME} PUBLIC __WXOSX__ CRCPP_USE_CPP11
        $<$<CONFIG:Debug>:__WXDEBUG__ ENABLE_PROFILING> $<$<CONFIG:Release>:NDEBUG>)
    # TODO: don't include -Werror for now, but eventually add it
    target_compile_options(${PROJECT_NAME} PUBLIC -Wall -Wextra -Wpedantic -Wshadow
                           $<$<CONFIG:Debug>:-Og>
                           $<$<CONFIG:Release>:-O2>)
elseif(UNIX)
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -s")
    target_compile_definitions(${PROJECT_NAME} PUBLIC __WXGTK__ CRCPP_USE_CPP11
        $<$<CONFIG:Debug>:__WXDEBUG__ ENABLE_PROFILING> $<$<CONFIG:Release>:NDEBUG>)
    # TODO: don't include -Werror for now, but eventually add it
    target_compile_options(${PROJECT_NAME} PUBLIC -Wall -Wextra -Wpedantic -Wshadow -fopenmp
                           $<$<CONFIG:Debug>:-Og -g3>
                           $<$<CONFIG:Release>:-O2>)
endif()

# Define the build target for the executable and copy it into the "app" subfolder
set_target_properties(${PROJECT_NAME} PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/build")
set_target_properties(${PROJECT_NAME} PROPERTIES LINKER_LANGUAGE CXX)

# Link required libraries to the executable
if(WIN32 OR APPLE)
    # AppleClang dropped support for OpenMP
    # Including TBB is not necessary for macOS and actually breaks hardened runtime
    # because that library is not included by default with the OS (XCode installs it)
    # and it is not signed.
    target_link_libraries(${PROJECT_NAME} ${wxWidgets_LIBRARIES})
elseif(UNIX)
    # Add OpenMP for mutlithreading.
    # TBB is required for GCC linkage sometimes, although this app is not explicitly using it,
    # so try to link it in.
    find_package(OpenMP)
    find_package(TBB)
    if(OpenMP_FOUND AND TBB_FOUND)
        message(STATUS "OpenMP: ${OpenMP_VERSION}")
        message(STATUS "Intel Threading Building Blocks: ${TBB_VERSION}")
        target_link_libraries(${PROJECT_NAME} ${wxWidgets_LIBRARIES} TBB::tbb OpenMP::OpenMP_CXX)
    elseif(OpenMP_FOUND)
        message(STATUS "OpenMP: ${OpenMP_VERSION}")
        target_link_libraries(${PROJECT_NAME} ${wxWidgets_LIBRARIES} OpenMP::OpenMP_CXX)
    elseif(TBB_FOUND)
        message(STATUS "Intel Threading Building Blocks: ${TBB_VERSION}")
        target_link_libraries(${PROJECT_NAME} ${wxWidgets_LIBRARIES} TBB::tbb)
    else()
        target_link_libraries(${PROJECT_NAME} ${wxWidgets_LIBRARIES})
    endif()
endif()

# Copy help, scripts, and resources into build folder
##############################################################
message(STATUS "Copying application resources...")

# make help directories if missing to avoid a build error
if(NOT EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/docs/readability-studio-manual/docs-manual/")
    add_custom_command(TARGET ${PROJECT_NAME}
                   POST_BUILD
                   COMMAND ${CMAKE_COMMAND} -E make_directory
                   "${CMAKE_CURRENT_SOURCE_DIR}/docs/readability-studio-manual/docs-manual/")
endif()
if(NOT EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/docs/readability-studio-manual/docs-online/")
    add_custom_command(TARGET ${PROJECT_NAME}
                   POST_BUILD
                   COMMAND ${CMAKE_COMMAND} -E make_directory
                   "${CMAKE_CURRENT_SOURCE_DIR}/docs/readability-studio-manual/docs-online/")
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
if(NOT EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/docs/readability-studio-api/docs/")
    add_custom_command(TARGET ${PROJECT_NAME}
                   POST_BUILD
                   COMMAND ${CMAKE_COMMAND} -E make_directory
                   "${CMAKE_CURRENT_SOURCE_DIR}/docs/readability-studio-api/docs/")
endif()
if(NOT EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/docs/sysadmin/docs/")
    add_custom_command(TARGET ${PROJECT_NAME}
                   POST_BUILD
                   COMMAND ${CMAKE_COMMAND} -E make_directory
                   "${CMAKE_CURRENT_SOURCE_DIR}/docs/sysadmin/docs/")
endif()

if(WIN32 AND NOT EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/installers/windows/release/")
    add_custom_command(TARGET ${PROJECT_NAME}
                   POST_BUILD
                   COMMAND ${CMAKE_COMMAND} -E make_directory
                   "${CMAKE_CURRENT_SOURCE_DIR}/installers/windows/release/")
endif()
if(APPLE AND NOT EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/installers/macos/release/")
    add_custom_command(TARGET ${PROJECT_NAME}
                   POST_BUILD
                   COMMAND ${CMAKE_COMMAND} -E make_directory
                   "${CMAKE_CURRENT_SOURCE_DIR}/installers/macos/release/")
endif()

# On macOS, move the resource files up from the binary folder to the resources folder
# (note that iOS has a flat folder structure, so only do this for macOS).
# Some of these files are compiled post build, and others are dynamic folders,
# so that is why we copy them like this post build instead of including them
# via RESOURCE like we do the icon.
if(APPLE AND NOT CMAKE_SYSTEM_NAME STREQUAL "iOS")
    set(RESOURCE_FOLDER "/../resources")
else()
    set(RESOURCE_FOLDER "")
endif()

add_custom_command(TARGET ${PROJECT_NAME}
                   POST_BUILD
                   COMMAND ${CMAKE_COMMAND} -E copy_directory
                   ${CMAKE_CURRENT_SOURCE_DIR}/docs/readability-studio-manual/docs-manual $<TARGET_FILE_DIR:${PROJECT_NAME}>${RESOURCE_FOLDER}/readability-studio-manual)
add_custom_command(TARGET ${PROJECT_NAME}
                   POST_BUILD
                   COMMAND ${CMAKE_COMMAND} -E copy_directory
                   ${CMAKE_CURRENT_SOURCE_DIR}/docs/readability-studio-manual/docs-online $<TARGET_FILE_DIR:${PROJECT_NAME}>${RESOURCE_FOLDER}/readability-studio-manual/online)
add_custom_command(TARGET ${PROJECT_NAME}
                   POST_BUILD
                   COMMAND ${CMAKE_COMMAND} -E copy_directory
                   ${CMAKE_CURRENT_SOURCE_DIR}/docs/shortcuts-cheatsheet/docs $<TARGET_FILE_DIR:${PROJECT_NAME}>${RESOURCE_FOLDER}/readability-studio-manual)
add_custom_command(TARGET ${PROJECT_NAME}
                   POST_BUILD
                   COMMAND ${CMAKE_COMMAND} -E copy_directory
                   ${CMAKE_CURRENT_SOURCE_DIR}/docs/readability-test-reference/docs $<TARGET_FILE_DIR:${PROJECT_NAME}>${RESOURCE_FOLDER}/readability-studio-manual)
add_custom_command(TARGET ${PROJECT_NAME}
                   POST_BUILD
                   COMMAND ${CMAKE_COMMAND} -E copy_directory
                   ${CMAKE_CURRENT_SOURCE_DIR}/docs/release-notes/docs $<TARGET_FILE_DIR:${PROJECT_NAME}>${RESOURCE_FOLDER}/readability-studio-manual)
add_custom_command(TARGET ${PROJECT_NAME}
                   POST_BUILD
                   COMMAND ${CMAKE_COMMAND} -E copy_directory
                   ${CMAKE_CURRENT_SOURCE_DIR}/docs/sysadmin/docs $<TARGET_FILE_DIR:${PROJECT_NAME}>${RESOURCE_FOLDER}/readability-studio-manual)
add_custom_command(TARGET ${PROJECT_NAME}
                   POST_BUILD
                   COMMAND ${CMAKE_COMMAND} -E copy_directory
                   ${CMAKE_CURRENT_SOURCE_DIR}/docs/readability-studio-api/docs $<TARGET_FILE_DIR:${PROJECT_NAME}>${RESOURCE_FOLDER}/readability-studio-manual)
add_custom_command(TARGET ${PROJECT_NAME}
                   POST_BUILD
                   COMMAND ${CMAKE_COMMAND} -E copy_directory
                   ${CMAKE_CURRENT_SOURCE_DIR}/docs/lua-5.4/doc $<TARGET_FILE_DIR:${PROJECT_NAME}>${RESOURCE_FOLDER}/readability-studio-manual/lua-5.4/doc)

add_custom_command(TARGET ${PROJECT_NAME}
                   POST_BUILD
                   COMMAND ${CMAKE_COMMAND} -E copy_directory
                   ${CMAKE_CURRENT_SOURCE_DIR}/resources/scripting $<TARGET_FILE_DIR:${PROJECT_NAME}>${RESOURCE_FOLDER})

add_custom_command(TARGET ${PROJECT_NAME}
                   POST_BUILD
                   COMMAND ${CMAKE_COMMAND} -E copy_directory
                   ${CMAKE_CURRENT_SOURCE_DIR}/resources/report-themes $<TARGET_FILE_DIR:${PROJECT_NAME}>${RESOURCE_FOLDER}/report-themes)

add_custom_command(TARGET ${PROJECT_NAME}
                   POST_BUILD
                   COMMAND ${CMAKE_COMMAND} -E copy_directory
                   ${CMAKE_CURRENT_SOURCE_DIR}/examples $<TARGET_FILE_DIR:${PROJECT_NAME}>${RESOURCE_FOLDER}/examples)

# Translations
function(copy_translations LANG)
    # Project's translations
    if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/locale/${LANG}.mo")
        add_custom_command(TARGET ${PROJECT_NAME}
                        POST_BUILD
                        COMMAND ${CMAKE_COMMAND} -E copy "${CMAKE_CURRENT_SOURCE_DIR}/locale/${LANG}.mo" "$<TARGET_FILE_DIR:${PROJECT_NAME}>${RESOURCE_FOLDER}/${LANG}/readstudio.mo"
                        COMMAND ${CMAKE_COMMAND} -E copy "${CMAKE_CURRENT_SOURCE_DIR}/locale/${LANG}.mo" "${CMAKE_CURRENT_SOURCE_DIR}/installers/windows/resources/${LANG}/readstudio.mo"
                        COMMENT "Copying Spanish translations to build and installer folders.")
    else()
        message(WARNING "Compiled ${PROJECT_NAME} '${LANG}' translations not found.\n(Expected it in '${CMAKE_CURRENT_SOURCE_DIR}/locale/${LANG}.mo')\nTranslations will not be available.")
    endif()
    # wxWidgets stock translations
    if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/locale/${LANG}/wxstd.mo")
        add_custom_command(TARGET ${PROJECT_NAME}
                        POST_BUILD
                        COMMAND ${CMAKE_COMMAND} -E copy "${CMAKE_CURRENT_SOURCE_DIR}/locale/${LANG}/wxstd.mo" "$<TARGET_FILE_DIR:${PROJECT_NAME}>${RESOURCE_FOLDER}/${LANG}/wxstd.mo"
                        COMMAND ${CMAKE_COMMAND} -E copy "${CMAKE_CURRENT_SOURCE_DIR}/locale/${LANG}/wxstd.mo" "${CMAKE_CURRENT_SOURCE_DIR}/installers/windows/resources/${LANG}/wxstd.mo"
                        COMMENT "Copying Spanish translations to build and installer folders.")
    else()
        message(WARNING "Compiled wxWidgets '${LANG}' translations not found.\n(Expected it in '${CMAKE_CURRENT_SOURCE_DIR}/locale/${LANG}/wxstd.mo'.)\nTranslations will not be available.")
    endif()
endfunction()

copy_translations("es")

if(UNIX AND NOT APPLE)
    # Icon used for Linux
    add_custom_command(TARGET ${PROJECT_NAME}
                       POST_BUILD
                       COMMAND ${CMAKE_COMMAND} -E copy
                       ${CMAKE_CURRENT_SOURCE_DIR}/resources/images/ribbon/app-logo.svg ${CMAKE_CURRENT_SOURCE_DIR}/installers/unix/AppDir)
    # Application screenshot
    add_custom_command(TARGET ${PROJECT_NAME}
                       POST_BUILD
                       COMMAND ${CMAKE_COMMAND} -E copy
                       ${CMAKE_CURRENT_SOURCE_DIR}/docs/readability-studio-manual/images/frygraph.png ${CMAKE_CURRENT_SOURCE_DIR}/installers/unix/AppDir)
endif()
# License file
add_custom_command(TARGET ${PROJECT_NAME}
                   POST_BUILD
                   COMMAND ${CMAKE_COMMAND} -E copy
                   ${CMAKE_CURRENT_SOURCE_DIR}/LICENSE $<TARGET_FILE_DIR:${PROJECT_NAME}>${RESOURCE_FOLDER})
# Citation files
add_custom_command(TARGET ${PROJECT_NAME}
                   POST_BUILD
                   COMMAND ${CMAKE_COMMAND} -E copy
                   ${CMAKE_CURRENT_SOURCE_DIR}/citations.bib $<TARGET_FILE_DIR:${PROJECT_NAME}>${RESOURCE_FOLDER})
add_custom_command(TARGET ${PROJECT_NAME}
                   POST_BUILD
                   COMMAND ${CMAKE_COMMAND} -E copy
                   ${CMAKE_CURRENT_SOURCE_DIR}/citations.mla $<TARGET_FILE_DIR:${PROJECT_NAME}>${RESOURCE_FOLDER})
add_custom_command(TARGET ${PROJECT_NAME}
                   POST_BUILD
                   COMMAND ${CMAKE_COMMAND} -E copy
                   ${CMAKE_CURRENT_SOURCE_DIR}/citations.apa $<TARGET_FILE_DIR:${PROJECT_NAME}>${RESOURCE_FOLDER})
# Zip up images and menu resources
file(MAKE_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/installers/windows/resources/")
add_custom_command(TARGET ${PROJECT_NAME}
                   POST_BUILD
                   COMMAND ${CMAKE_COMMAND} -E tar "cvf" "$<TARGET_FILE_DIR:${PROJECT_NAME}>${RESOURCE_FOLDER}/res.wad" --format=zip --files-from="${CMAKE_CURRENT_SOURCE_DIR}/tools/images.cmake"
                   COMMAND ${CMAKE_COMMAND} -E copy "$<TARGET_FILE_DIR:${PROJECT_NAME}>${RESOURCE_FOLDER}/res.wad" "${CMAKE_CURRENT_SOURCE_DIR}/installers/windows/resources/"
                   COMMENT "Building resourse bundle and copying to build and installer folders."
                   WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/resources/images")
# Word lists
add_custom_command(TARGET ${PROJECT_NAME}
                   POST_BUILD
                   COMMAND ${CMAKE_COMMAND} -E tar "cvf" "$<TARGET_FILE_DIR:${PROJECT_NAME}>${RESOURCE_FOLDER}/words.wad" --format=zip --files-from="${CMAKE_CURRENT_SOURCE_DIR}/tools/words.cmake"
                   COMMAND ${CMAKE_COMMAND} -E copy "$<TARGET_FILE_DIR:${PROJECT_NAME}>${RESOURCE_FOLDER}/words.wad" "${CMAKE_CURRENT_SOURCE_DIR}/installers/windows/resources/"
                   COMMENT "Building word lists and copying to build and installer folders."
                   WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/resources/words")

if(UNIX)
    set_target_properties(${PROJECT_NAME} PROPERTIES
        CXX_VISIBILITY_PRESET hidden
        C_VISIBILITY_PRESET hidden
        VISIBILITY_INLINES_HIDDEN TRUE)
endif()

if(APPLE)
    set_target_properties(${PROJECT_NAME} PROPERTIES
        RESOURCE app-logo.icns
        OUTPUT_NAME ${readstudio_DESCRIPTION}
        XCODE_ATTRIBUTE_EXECUTABLE_NAME ${PROJECT_NAME} 
        MACOSX_BUNDLE_INFO_PLIST ${CMAKE_CURRENT_SOURCE_DIR}/Info.plist.in)

        message(STATUS "App Bundle: ${OUTPUT_NAME}")
endif()

# Copy application (and resources) to installer folders (if a production build)
if(${CMAKE_BUILD_TYPE} STREQUAL "Release")
    if(WIN32)
        add_custom_command(TARGET ${PROJECT_NAME}
                           POST_BUILD
                           COMMAND ${CMAKE_COMMAND} -E copy "$<TARGET_FILE:${PROJECT_NAME}>" "${CMAKE_CURRENT_SOURCE_DIR}/installers/windows/release/"
                           COMMENT "Copying binary to installer folder.")
    elseif(UNIX AND NOT APPLE)
        add_custom_command(TARGET ${PROJECT_NAME}
                       POST_BUILD
                       COMMAND ${CMAKE_COMMAND} -E make_directory
                       ${CMAKE_CURRENT_SOURCE_DIR}/installers/unix/AppDir/usr/bin)
        add_custom_command(TARGET ${PROJECT_NAME}
                        POST_BUILD
                        COMMAND ${CMAKE_COMMAND} -E copy_directory "$<TARGET_FILE_DIR:${PROJECT_NAME}>" "${CMAKE_CURRENT_SOURCE_DIR}/installers/unix/AppDir/usr/bin"
                        COMMENT "Copying files to AppImage staging folder.")
    elseif(APPLE)
        add_custom_command(TARGET ${PROJECT_NAME}
                           POST_BUILD
                           COMMAND ${CMAKE_COMMAND} -E copy_directory "$<TARGET_FILE_DIR:${PROJECT_NAME}>" "${CMAKE_CURRENT_SOURCE_DIR}/installers/macos/release/"
                           COMMENT "Copying files to DMG staging folder.")
    endif()
endif()
