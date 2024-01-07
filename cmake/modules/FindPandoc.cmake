# Copyright (C) 2024, Blake Madden.
# All rights reserved.
# SPDX-License-Identifier: BSD-3-Clause

# CMake module to find Pandoc
# - Try to find Pandoc
# Once done, this will define
#
#  PANDOC_FOUND - system has Pandoc
#  PANDOC_ROOT_DIR - Pandoc folder
#  PANDOC_EXECUTABLE - the Pandoc executable (full path)

find_program(PANDOC_EXECUTABLE NAMES pandoc pandoc.exe)

# If not found and we are on Windows, try to look for it in the default installation path
if(WIN32 AND PANDOC_EXECUTABLE MATCHES "PANDOC_EXECUTABLE-NOTFOUND")
  GET_FILENAME_COMPONENT(PANDOC_QUARTO_PATH "C:\\Program Files\\RStudio\\resources\\app\\bin\\quarto\\bin\\tools" REALPATH)
  if(EXISTS "${PANDOC_QUARTO_PATH}")
    set(PANDOC_EXECUTABLE "${PANDOC_QUARTO_PATH}\\pandoc.exe")
    set(PANDOC_ROOT_DIR "${PANDOC_QUARTO_PATH}")
  endif()
elseif(UNIX AND PANDOC_EXECUTABLE MATCHES "PANDOC_EXECUTABLE-NOTFOUND")
  GET_FILENAME_COMPONENT(PANDOC_BIN_PATH "/usr/bin/pandoc" REALPATH)
  if(EXISTS "${PANDOC_BIN_PATH}")
    set(PANDOC_EXECUTABLE "${PANDOC_BIN_PATH}")
    set(PANDOC_ROOT_DIR "/usr/bin/")
  endif()
endif()

# Set PANDOC_FOUND to TRUE if all listed variables are TRUE
include(FindPackageHandleStandardArgs)
mark_as_advanced(PANDOC_FOUND PANDOC_EXECUTABLE PANDOC_ROOT_DIR)
