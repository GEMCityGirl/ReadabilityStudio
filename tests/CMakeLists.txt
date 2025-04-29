#############################################################################
# Name:        CMakeListst.txt
# Purpose:     Unit test runner Readability Studio
# Author:      Blake Madden
# Created:     2023-03-12
# Copyright:   (c) 2024 Blake Madden
# Licence:     Eclipse Public License 2.0
#############################################################################

project(RSTestRunner)

cmake_minimum_required(VERSION 3.12)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Options:
# - USE_ADDRESS_SANITIZE to use ASAN
# - USE_CLANG_TIDY to run clang-tidy

find_program(CLANG_TIDY_COMMAND
      NAMES
         clang-tidy
      NO_CACHE)
if(CLANG_TIDY_COMMAND AND USE_CLANG_TIDY)
    if(MSVC)
        set(CMAKE_CXX_CLANG_TIDY ${CLANG_TIDY_COMMAND} --extra-arg=/EHsc)
    else()
        set(CMAKE_CXX_CLANG_TIDY ${CLANG_TIDY_COMMAND})
    endif()
endif()

# place Catch2 in the src folder if not installed
# (you will need to do this on Windows or macOS or if version 3 of Catch2 isn't installed)
get_filename_component(_fullpath "${_dir}" REALPATH)
if(EXISTS "${_fullpath}" AND EXISTS "${_fullpath}/../src/Catch2")
    add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/../src/Catch2 ${CMAKE_CURRENT_BINARY_DIR}/Catch2)
else()
    # ...otherwise, see if it is installed
    find_package(Catch2 3 REQUIRED)
endif()

add_compile_options("$<$<C_COMPILER_ID:MSVC>:/utf-8>")
add_compile_options("$<$<CXX_COMPILER_ID:MSVC>:/utf-8>")

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/bin)

add_executable(${CMAKE_PROJECT_NAME} ../src/indexing/article.cpp ../src/indexing/abbreviation.cpp
    ../src/indexing/conjunction.cpp ../src/indexing/contraction.cpp ../src/indexing/double_words.cpp
    ../src/indexing/negating_word.cpp ../src/indexing/passive_voice.cpp ../src/indexing/pronoun.cpp
    ../src/indexing/romanize.cpp ../src/indexing/stop_lists.cpp ../src/indexing/syllable.cpp
    ../src/Wisteria-Dataviz/src/util/i18n_string_util.cpp
    ../src/indexing/word_functional.cpp
    ../src/indexing/diacritics.cpp
    abbreviationtests.cpp
    acronymtests.cpp articletests.cpp documenttests.cpp englishsyllabletests.cpp
    germansyllabletests.cpp sentencetests.cpp wordfunctortests.cpp
    conjunctiontests.cpp ischartests.cpp contractiontests.cpp
    doublewordtests.cpp grammartests.cpp stringtests.cpp doctokenizetests.cpp
    romanizetests.cpp readingtests.cpp wordlisttests.cpp chartraitstext.cpp
    testingmain.cpp wordtests.cpp spanishsyllabletests.cpp projectrefresh.cpp
    diacriticstests.cpp)

if(MSVC)
    target_compile_definitions(${CMAKE_PROJECT_NAME} PUBLIC _CRT_SECURE_NO_WARNINGS __UNITTEST _DISABLE_VECTOR_ANNOTATION _DISABLE_STRING_ANNOTATION)
    if(USE_ADDRESS_SANITIZE)
        target_compile_options(${CMAKE_PROJECT_NAME} PRIVATE /Zc:__cplusplus /fsanitize=address /MP /W3 /WX)
    else()
        target_compile_options(${CMAKE_PROJECT_NAME} PRIVATE /Zc:__cplusplus /MP /W3 /WX)
    endif()
else()
    target_compile_definitions(${CMAKE_PROJECT_NAME} PUBLIC _CRT_SECURE_NO_WARNINGS __UNITTEST)
endif()
target_link_libraries(${CMAKE_PROJECT_NAME} PRIVATE Catch2::Catch2)

MESSAGE(STATUS "Copying test files...")
ADD_CUSTOM_COMMAND(TARGET ${CMAKE_PROJECT_NAME}
                   POST_BUILD
                   COMMAND ${CMAKE_COMMAND} -E copy_directory
                   ${CMAKE_CURRENT_SOURCE_DIR}/Dictionaries $<TARGET_FILE_DIR:${CMAKE_PROJECT_NAME}>)

# load the test cases into the runner
include(CTest)
include(Catch)
catch_discover_tests(${CMAKE_PROJECT_NAME})
