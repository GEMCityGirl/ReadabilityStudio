#############################################################################
# Name:        CMakeListst.txt
# Purpose:     Unit test runner Readability Studio
# Author:      Blake Madden
# Created:     2023-03-12
# Copyright:   (c) 2023 Blake Madden
# Licence:     3-Clause BSD licence
#############################################################################

PROJECT(RSTestRunner)

CMAKE_MINIMUM_REQUIRED(VERSION 3.12)
SET(CMAKE_CXX_STANDARD 17)
SET(CMAKE_CXX_STANDARD_REQUIRED True)

# place Catch2 in the src folder if not installed
# (you will need to do this on Windows or macOS or if version 3 of Catch2 isn't installed)
GET_FILENAME_COMPONENT(_fullpath "${_dir}" REALPATH)
IF(EXISTS "${_fullpath}" AND EXISTS "${_fullpath}/../src/Catch2")
    ADD_SUBDIRECTORY(${CMAKE_CURRENT_SOURCE_DIR}/../src/Catch2 ${CMAKE_CURRENT_BINARY_DIR}/Catch2)
ELSE()
    # ...otherwise, see if it is installed
    FIND_PACKAGE(Catch2 3 REQUIRED)
ENDIF()

ADD_COMPILE_OPTIONS("$<$<C_COMPILER_ID:MSVC>:/utf-8>")
ADD_COMPILE_OPTIONS("$<$<CXX_COMPILER_ID:MSVC>:/utf-8>")

SET(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)

ADD_EXECUTABLE(${CMAKE_PROJECT_NAME} ../src/indexing/article.cpp ../src/indexing/abbreviation.cpp
../src/indexing/conjunction.cpp ../src/indexing/contraction.cpp ../src/indexing/double_words.cpp
../src/indexing/negating_word.cpp ../src/indexing/passive_voice.cpp ../src/indexing/pronoun.cpp
../src/indexing/romanize.cpp ../src/indexing/stop_lists.cpp ../src/indexing/syllable.cpp
../src/Wisteria-Dataviz/src/i18n-check/src/i18n_string_util.cpp
acronymtests.cpp articletests.cpp documenttests.cpp englishsyllabletests.cpp
germansyllabletests.cpp sentencetests.cpp wordfunctortests.cpp
conjunctiontests.cpp ischartests.cpp contractiontests.cpp
doublewordtests.cpp grammartests.cpp stringtests.cpp doctokenizetests.cpp
romanizetests.cpp readingtests.cpp wordlisttests.cpp chartraitstext.cpp
testingmain.cpp  wordtests.cpp spanishsyllabletests.cpp projectrefresh.cpp)
IF(MSVC)
    TARGET_COMPILE_DEFINITIONS(${CMAKE_PROJECT_NAME} PUBLIC _CRT_SECURE_NO_WARNINGS __UNITTEST)
ELSE()
    TARGET_COMPILE_DEFINITIONS(${CMAKE_PROJECT_NAME} PUBLIC _CRT_SECURE_NO_WARNINGS __UNITTEST)
ENDIF()
TARGET_LINK_LIBRARIES(${CMAKE_PROJECT_NAME} PRIVATE Catch2::Catch2)

MESSAGE(STATUS "Copying test files...")
ADD_CUSTOM_COMMAND(TARGET ${CMAKE_PROJECT_NAME}
                   POST_BUILD
                   COMMAND ${CMAKE_COMMAND} -E copy_directory
                   ${CMAKE_SOURCE_DIR}/Dictionaries $<TARGET_FILE_DIR:${CMAKE_PROJECT_NAME}>)

# load the test cases into the runner
INCLUDE(CTest)
INCLUDE(Catch)
CATCH_DISCOVER_TESTS(${CMAKE_PROJECT_NAME})
