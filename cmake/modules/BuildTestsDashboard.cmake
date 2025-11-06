#############################################################################
# Name:        BuildTestsDashboard.txt
# Purpose:     Build script for Readability Studio
# Author:      Blake Madden
# Created:     2025-11-06
# Copyright:   (c) 2025 Blake Madden
# License:     Eclipse Public License 2.0
#############################################################################

# cmake/modules/BuildTestsDashboard.cmake
# Basic Testing & Quality dashboard generator.
#
#   - emits a header (title, project, last updated)
#   - then inlines the contents of project-management/tests/badges.md

set(MD "")

# Optional: pull in shared helpers (timestamp/json/etc.).
# This assumes DashboardCommon.cmake is in the same directory.
include("${CMAKE_CURRENT_LIST_DIR}/DashboardCommon.cmake")

# ----- Inputs -----
if(NOT DEFINED ROOT)
  set(ROOT "${CMAKE_SOURCE_DIR}")
endif()

# Where the raw badges live (GH Actions badges etc.)
if(NOT DEFINED TEST_BADGES_MD)
  set(TEST_BADGES_MD "${ROOT}/project-management/tests/badges.md")
endif()

# Where to write the Testing & Quality dashboard
if(NOT DEFINED TESTS_OUT_MD)
  set(TESTS_OUT_MD "${ROOT}/project-management/tests/DASHBOARD.md")
endif()

# Project name for header
if(NOT DEFINED PROJECT_NAME)
  set(PROJECT_NAME "${CMAKE_PROJECT_NAME}")
endif()

# Normalize paths (Windows-safe)
string(REPLACE "\\" "/" TEST_BADGES_MD "${TEST_BADGES_MD}")
string(REPLACE "\\" "/" TESTS_OUT_MD   "${TESTS_OUT_MD}")

# ----- Header -----
string(TIMESTAMP TODAY_YYYY_MM_DD "%Y-%m-%d")

set(MD "# 🧪 Testing & Quality Dashboard\n\n")
string(APPEND MD "> **Project:** ${PROJECT_NAME} · **Last updated:** ${TODAY_YYYY_MM_DD}\n\n")
string(APPEND MD "---\n\n")

# ----- Inline badges.md -----
if(EXISTS "${TEST_BADGES_MD}")
  file(READ "${TEST_BADGES_MD}" _badges_md)
  # Just dump it verbatim under the header
  string(APPEND MD "${_badges_md}\n")
else()
  string(APPEND MD "_(No testing badges found at \`${TEST_BADGES_MD}\`)_\n")
endif()

# ----- Write file -----
get_filename_component(_md_dir "${TESTS_OUT_MD}" DIRECTORY)
file(MAKE_DIRECTORY "${_md_dir}")
file(WRITE "${TESTS_OUT_MD}" "${MD}")

message(STATUS "Testing & Quality dashboard written to ${TESTS_OUT_MD}")
