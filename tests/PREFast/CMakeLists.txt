#############################################################################
# Name:        CMakeListst.txt
# Purpose:     PREFast analysis project for Readability Studio
# Author:      Blake Madden
# Created:     2023-06-12
# Copyright:   (c) 2023 Blake Madden
# Licence:     3-Clause BSD licence
#############################################################################

PROJECT(RSPREFast)

CMAKE_MINIMUM_REQUIRED(VERSION 3.12)
SET(CMAKE_CXX_STANDARD 17)
SET(CMAKE_CXX_STANDARD_REQUIRED True)

SET(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)

ADD_EXECUTABLE(${CMAKE_PROJECT_NAME} rsprefast.cpp
    src/indexing/syllable.cpp
    src/indexing/stop_lists.cpp
    src/indexing/romanize.cpp
    src/indexing/pronoun.cpp
    src/indexing/passive_voice.cpp
    src/indexing/negating_word.cpp
    src/indexing/double_words.cpp
    src/indexing/contraction.cpp
    src/indexing/conjunction.cpp
    src/indexing/article.cpp
    src/indexing/abbreviation.cpp)

IF(MSVC)
    TARGET_COMPILE_DEFINITIONS(${CMAKE_PROJECT_NAME} PUBLIC _CRT_SECURE_NO_WARNINGS)
    TARGET_COMPILE_OPTIONS(${CMAKE_PROJECT_NAME} PUBLIC /Zc:__cplusplus /MP /W3 /WX)
ENDIF()
