# Documentation & Knowledge dashboard generator

set(MD "")
include("${CMAKE_CURRENT_LIST_DIR}/DashboardCommon.cmake")

# ----- Inputs -----
if(NOT DEFINED ROOT)
  set(ROOT "${CMAKE_SOURCE_DIR}")
endif()

# Per-doc-set status JSONs (l10n coverage)
if(NOT DEFINED DOC_STATUS_DIR)
  set(DOC_STATUS_DIR "${ROOT}/project-management/docs/status")
endif()

# Markdown output
if(NOT DEFINED DOC_OUT_MD)
  set(DOC_OUT_MD "${ROOT}/project-management/docs/DASHBOARD.md")
endif()

# Project name (for header)
if(NOT DEFINED PROJECT_NAME)
  set(PROJECT_NAME "${CMAKE_PROJECT_NAME}")
endif()

# Normalize paths (Windows safe)
string(REPLACE "\\" "/" DOC_STATUS_DIR "${DOC_STATUS_DIR}")
string(REPLACE "\\" "/" DOC_OUT_MD "${DOC_OUT_MD}")

# L10n-style per-language JSON files
set(DOC_USER_JSON   "${DOC_STATUS_DIR}/user-manual.json")
set(DOC_ADMIN_JSON  "${DOC_STATUS_DIR}/admin-guide.json")
set(DOC_API_JSON    "${DOC_STATUS_DIR}/programming-reference.json")

# Doxygen coverage / readiness (single-value i18n-style JSON)
set(DOC_DOXYGEN_JSON "${DOC_STATUS_DIR}/doxygen.json")

# ----- Read per-doc JSONs and collect languages -----
set(SEEN_LANGS "")

_read_json("${DOC_USER_JSON}"  DOC_USER)
_read_json("${DOC_ADMIN_JSON}" DOC_ADMIN)
_read_json("${DOC_API_JSON}"   DOC_API)

list(REMOVE_DUPLICATES SEEN_LANGS)
set(DOC_LANGS ${SEEN_LANGS})
list(SORT DOC_LANGS)

string(JOIN "," _langs_joined ${DOC_LANGS})
message(STATUS "[DOCS] Languages detected: ${_langs_joined}")

# ----- Compute per-language roll-up for docs -----
# Simple equal-weight average of User + Admin + API (0..100); Doxygen is English-only.
foreach(L IN LISTS DOC_LANGS)
  _val_or(DOC_USER_${L} 0)
  set(p_user "${RET}")

  _val_or(DOC_ADMIN_${L} 0)
  set(p_admin "${RET}")

  _val_or(DOC_API_${L} 0)
  set(p_api "${RET}")

  # 3 docs, equal weight; integer division is fine for dashboard purposes
  math(EXPR p_all "(${p_user} + ${p_admin} + ${p_api}) / 3")

  _bar10(bar "${p_all}")
  set(DOC_PALL_${L} "${p_all}")
  set(DOC_BAR_${L}  "${bar}")
endforeach()

# ----- Read Doxygen i18n-style JSON -----
_read_i18n("${DOC_DOXYGEN_JSON}" DOC_DOXYGEN)

# ----- Markdown emit -----
string(TIMESTAMP TODAY_YYYY_MM_DD "%Y-%m-%d")

# Header
set(MD "# 📚 Documentation & Knowledge Dashboard\n\n")
string(APPEND MD "> **Project:** ${PROJECT_NAME} · **Last updated:** ${TODAY_YYYY_MM_DD}\n\n---\n\n")

# Legend
string(APPEND MD
  "## Legend\n\n"
  "- 🟢 **Complete** (≥ 95%)\n"
  "- 🟡 **In Progress** (10–94%)\n"
  "- 🔴 **Not Started** (< 10%)\n"
  "- ⏳ **Needs Review** (pending proofread/QA)\n\n"
  "`██████████` = 100%, `█████░░░░░` ≈ 50%\n\n"
  "---\n\n"
)

# ----- Roll-up table by language -----
string(APPEND MD "## Summary by Language\n\n")
string(APPEND MD "Overall documentation coverage for translatable docs (User Manual, Admin Guide, Programming Reference).\n\n")
string(APPEND MD "| Language | Code | Progress | Overall |\n|---|:--:|:--:|:--:|\n")

foreach(L IN LISTS DOC_LANGS)
  set(_pall_var "DOC_PALL_${L}")
  set(_bar_var  "DOC_BAR_${L}")
  set(P_ALL "${${_pall_var}}")
  set(BAR   "${${_bar_var}}")

  _emoji_for(EMJ "${P_ALL}")
  _resolve_lang_name(_LNAME "${L}")

  string(APPEND MD "| ${_LNAME} | `${L}` | `${BAR}` | ${EMJ} **${P_ALL}%** |\n")
endforeach()

string(APPEND MD "\n---\n\n")

# ===== Sections =====

# 1) User Manual
string(APPEND MD "### 1) 📖 User Manual\n\n")
string(APPEND MD
  "**Scope**: End-user tasks, workflows, troubleshooting, and feature overviews.  \n"
  "**Sources**: Quarto/Markdown in `docs/` (e.g., `docs/user-manual.qmd`).  \n"
  "**Localization**: Strings extracted into `locale/docs/*.po` and maintained with POEdit.\n\n"
)
string(APPEND MD "**Status**\n\n| Language | Code | Progress | Overall |\n|:---:|:--:|:--:|:--:|\n")
foreach(L IN LISTS DOC_LANGS)
  _val_or(DOC_USER_${L} 0)
  set(_pct "${RET}")
  _bar10(_bar "${_pct}")
  _emoji_for(_emj "${_pct}")
  _resolve_lang_name(_LNAME "${L}")
  string(APPEND MD "| ${_LNAME} | `${L}` | `${_bar}` | ${_emj} **${_pct}%** |\n")
endforeach()
if(NOT "${DOC_USER_COMMENT}" STREQUAL "")
  _emit_comment_md(_blk "${DOC_USER_COMMENT}")
  string(APPEND MD "\n${_blk}\n")
endif()
string(APPEND MD "\n---\n\n")

# 2) System Administrator Guide
string(APPEND MD "### 2) 🛠️ System Administrator Guide\n\n")
string(APPEND MD
  "**Scope**: Install/upgrade procedures, backup/restore, deployment topologies, and integration notes.  \n"
  "**Sources**: Quarto/Markdown in `docs/` (e.g., `docs/admin-guide.qmd`).  \n"
  "**Localization**: Tracked via `locale/docs/*.po` alongside the User Manual.\n\n"
)
string(APPEND MD "**Status**\n\n| Language | Code | Progress | Overall |\n|:---:|:--:|:--:|:--:|\n")
foreach(L IN LISTS DOC_LANGS)
  _val_or(DOC_ADMIN_${L} 0)
  set(_pct "${RET}")
  _bar10(_bar "${_pct}")
  _emoji_for(_emj "${_pct}")
  _resolve_lang_name(_LNAME "${L}")
  string(APPEND MD "| ${_LNAME} | `${L}` | `${_bar}` | ${_emj} **${_pct}%** |\n")
endforeach()
if(NOT "${DOC_ADMIN_COMMENT}" STREQUAL "")
  _emit_comment_md(_blk "${DOC_ADMIN_COMMENT}")
  string(APPEND MD "\n${_blk}\n")
endif()
string(APPEND MD "\n---\n\n")

# 3) Programming Reference (Lua / API)
string(APPEND MD "### 3) 🧪 Programming Reference (Lua / Automation API)\n\n")
string(APPEND MD
  "**Scope**: Public scripting entry points, configuration structures, return values, and code examples.  \n"
  "**Sources**: Quarto/Markdown in `docs/` (e.g., `docs/programming-reference.qmd`).  \n"
  "**Localization**: Translations optional but recommended for major locales; examples may remain in English.\n\n"
)
string(APPEND MD "**Status**\n\n| Language | Code | Progress | Overall |\n|:---:|:--:|:--:|:--:|\n")
foreach(L IN LISTS DOC_LANGS)
  _val_or(DOC_API_${L} 0)
  set(_pct "${RET}")
  _bar10(_bar "${_pct}")
  _emoji_for(_emj "${_pct}")
  _resolve_lang_name(_LNAME "${L}")
  string(APPEND MD "| ${_LNAME} | `${L}` | `${_bar}` | ${_emj} **${_pct}%** |\n")
endforeach()
if(NOT "${DOC_API_COMMENT}" STREQUAL "")
  _emit_comment_md(_blk "${DOC_API_COMMENT}")
  string(APPEND MD "\n${_blk}\n")
endif()
string(APPEND MD "\n---\n\n")

# 4) Doxygen Reference (English-only technical reference)
string(APPEND MD "### 4) 📦 Doxygen API Reference (English Only)\n\n")
string(APPEND MD
  "This is a **developer-facing** reference generated from source comments. It is not localized, "
  "but we still track coverage and quality of documentation.\n\n"
)

set(_doxy_val "${DOC_DOXYGEN_VALUE}")
if(_doxy_val STREQUAL "")
  set(_doxy_val 0)
endif()
_bar10(_doxy_bar "${_doxy_val}")
_emoji_for(_doxy_emj "${_doxy_val}")

string(APPEND MD
  "| Metric | Value |\n|---|:--:|\n"
  "| Overall documentation coverage | `${_doxy_bar}` ${_doxy_emj} **${_doxy_val}%** |\n\n"
)

# Optional title and checklists from the Doxygen JSON
_emit_section(DOC_DOXYGEN_TITLE DOC_DOXYGEN_MUST DOC_DOXYGEN_RECOMMENDED DOC_DOXYGEN_COMMENT)

# Write file
get_filename_component(_md_dir "${DOC_OUT_MD}" DIRECTORY)
file(MAKE_DIRECTORY "${_md_dir}")
file(WRITE "${DOC_OUT_MD}" "${MD}")

message(STATUS "Docs dashboard written to ${DOC_OUT_MD}")
