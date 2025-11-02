# Globalization dashboard generator (l10n + i18n)
cmake_minimum_required(VERSION 3.19)

# Turn "Header;0: item A;1: item B" (or newline-separated) into a GitHub checklist
function(_make_checklist OUT COMMENT)
  if(COMMENT STREQUAL "")
    set(${OUT} "" PARENT_SCOPE)
    return()
  endif()

  # Normalize newlines -> semicolons, then let CMake treat it as a list
  set(_s "${COMMENT}")
  string(REPLACE "\n" ";" _s "${_s}")

  # Now _parts is a proper CMake list split ONLY on ';'
  set(_parts ${_s})
  list(LENGTH _parts _len)
  if(_len EQUAL 0)
    set(${OUT} "" PARENT_SCOPE)
    return()
  endif()

  # First element = header
  list(GET _parts 0 _header)
  string(STRIP "${_header}" _header)
  set(_out "#### ${_header}\n")

  # Remaining elements = checklist items
  math(EXPR _last "${_len}-1")
  if(_last GREATER 0)
    foreach(_i RANGE 1 ${_last})
      list(GET _parts ${_i} _tok)
      string(STRIP "${_tok}" _tok)
      if(_tok STREQUAL "")
        continue()
      endif()

      # Parse "0:" / "1:" prefix (default unchecked)
      string(FIND "${_tok}" ":" _pos)
      if(_pos EQUAL -1)
        set(_state " ")
        set(_text "${_tok}")
      else()
        string(SUBSTRING "${_tok}" 0 1 _flag)
        if(_flag STREQUAL "1")
          set(_state "x")
        else()
          set(_state " ")
        endif()
        math(EXPR _start "${_pos}+1")
        string(SUBSTRING "${_tok}" ${_start} -1 _text)
      endif()

      string(STRIP "${_text}" _text)
      string(APPEND _out "- [${_state}] ${_text}\n")
    endforeach()
  endif()

  set(${OUT} "${_out}" PARENT_SCOPE)
endfunction()

# Decide checklist vs paragraph based on presence of 0:/1:
function(_emit_comment_md OUT COMMENT)
  if(COMMENT STREQUAL "")
    set(${OUT} "" PARENT_SCOPE)
    return()
  endif()

  string(FIND "${COMMENT}" "0:" _f0)
  string(FIND "${COMMENT}" "1:" _f1)
  if(NOT _f0 EQUAL -1 OR NOT _f1 EQUAL -1)
    _make_checklist(_blk "${COMMENT}")
    set(${OUT} "${_blk}" PARENT_SCOPE)
  else()
    # Plain paragraph (preserve original text)
    set(${OUT} "${COMMENT}\n" PARENT_SCOPE)
  endif()
endfunction()

# ----- Inputs -----
if(NOT DEFINED ROOT)
  set(ROOT "${CMAKE_SOURCE_DIR}")
endif()

if(NOT DEFINED STATUS_DIR)
  set(STATUS_DIR "${ROOT}/project-management/g11n/status")
endif()

if(NOT DEFINED STATUS_DIR_I18N)
  set(STATUS_DIR_I18N "${ROOT}/project-management/g11n/status/i18n")
endif()

if(NOT DEFINED OUT_MD)
  set(OUT_MD "${ROOT}/project-management/g11n/DASHBOARD.md")
endif()

if(NOT DEFINED OUT_BADGES)
  set(OUT_BADGES "${ROOT}/project-management/g11n/badges")
endif()

if(NOT DEFINED PROJECT_NAME)
  set(PROJECT_NAME "${CMAKE_PROJECT_NAME}")
endif()

# Normalize paths (Windows safe)
string(REPLACE "\\" "/" STATUS_DIR "${STATUS_DIR}")
string(REPLACE "\\" "/" STATUS_DIR_I18N "${STATUS_DIR_I18N}")
string(REPLACE "\\" "/" OUT_MD "${OUT_MD}")
string(REPLACE "\\" "/" OUT_BADGES "${OUT_BADGES}")

# L10n files (per-language)
set(UI_JSON       "${STATUS_DIR}/ui.json")
set(DOCS_JSON     "${STATUS_DIR}/docs.json")
set(WIN_JSON      "${STATUS_DIR}/windows-installer.json")
set(ART_JSON      "${STATUS_DIR}/art.json")
set(LICENSE_JSON  "${STATUS_DIR}/license.json")

# I18n files (single number + comment)
set(I18N_UI_JSON      "${STATUS_DIR_I18N}/ui.json")
set(I18N_DOCS_JSON    "${STATUS_DIR_I18N}/docs.json")
set(I18N_WIN_JSON     "${STATUS_DIR_I18N}/windows-installer.json")
set(I18N_ART_JSON     "${STATUS_DIR_I18N}/art.json")
set(I18N_LICENSE_JSON "${STATUS_DIR_I18N}/license.json")

# ----- Helpers -----
# Read top-level keys + values from a flat JSON object using LENGTH/MEMBER/GET
# Special key "comment" (string) is supported and does not count as a language.
# Exposes:
#   <OUT_PREFIX>_<lang>  = percent (number)
#   <OUT_PREFIX>_COMMENT = comment (string), if present
# Accumulates language codes into SEEN_LANGS.
function(_read_json JSON_FILE OUT_PREFIX)
  if(NOT EXISTS "${JSON_FILE}")
    message(FATAL_ERROR "Missing JSON: ${JSON_FILE}")
  endif()

  file(READ "${JSON_FILE}" _json)

  string(JSON _count LENGTH "${_json}")
  if(NOT _count GREATER 0)
    message(STATUS "[G11N] ${JSON_FILE} has 0 keys.")
    return()
  endif()

  set(_acc "${SEEN_LANGS}")
  math(EXPR _last "${_count}-1")
  foreach(_i RANGE ${_last})
    string(JSON _k MEMBER "${_json}" ${_i})
    string(JSON _v GET "${_json}" "${_k}")

    if(_k STREQUAL "comment")
      set(${OUT_PREFIX}_COMMENT "${_v}" PARENT_SCOPE)
    else()
      set(${OUT_PREFIX}_${_k} "${_v}" PARENT_SCOPE)
      list(APPEND _acc "${_k}")
    endif()
  endforeach()

  set(SEEN_LANGS "${_acc}" PARENT_SCOPE)
endfunction()

# Read a single i18n JSON: { "value": <0..100>, "comment": "..." }
# Exposes: <OUT_PREFIX>_VALUE, <OUT_PREFIX>_COMMENT
function(_read_i18n JSON_FILE OUT_PREFIX)
  if(NOT EXISTS "${JSON_FILE}")
    message(STATUS "[G11N] (i18n) missing file: ${JSON_FILE} → value=0")
    set(${OUT_PREFIX}_VALUE 0 PARENT_SCOPE)
    set(${OUT_PREFIX}_COMMENT "" PARENT_SCOPE)
    return()
  endif()

  file(READ "${JSON_FILE}" _json)

  # value
  set(_val 0)
  string(JSON _count LENGTH "${_json}")
  if(_count GREATER 0)
    # try to read "value" if present
    string(REGEX MATCH "\"value\"" _has_val "${_json}")
    if(NOT _has_val STREQUAL "")
      string(JSON _val GET "${_json}" "value")
    endif()
    # comment (optional)
    string(REGEX MATCH "\"comment\"" _has_c "${_json}")
    if(NOT _has_c STREQUAL "")
      string(JSON _cmt GET "${_json}" "comment")
    else()
      set(_cmt "")
    endif()
  else()
    set(_cmt "")
  endif()

  set(${OUT_PREFIX}_VALUE "${_val}" PARENT_SCOPE)
  set(${OUT_PREFIX}_COMMENT "${_cmt}" PARENT_SCOPE)
endfunction()

function(_val_or VAR DEFAULT)
  if(DEFINED ${VAR})
    set(RET "${${VAR}}")
  else()
    set(RET "${DEFAULT}")
  endif()
  set(RET "${RET}" PARENT_SCOPE)
endfunction()

# Progress bar string (10 chars)
function(_bar10 OUT P)
  math(EXPR filled "(${P}+5)/10")
  if(filled GREATER 10)
    set(filled 10)
  endif()
  if(filled LESS 0)
    set(filled 0)
  endif()
  math(EXPR empty "10-${filled}")

  set(_f "")
  set(_e "")

  while(filled GREATER 0)
    string(APPEND _f "█")
    math(EXPR filled "${filled}-1")
  endwhile()

  while(empty GREATER 0)
    string(APPEND _e "░")
    math(EXPR empty "${empty}-1")
  endwhile()

  set(${OUT} "${_f}${_e}" PARENT_SCOPE)
endfunction()

# Color ramp
function(_color_for OUT P)
  if(P GREATER_EQUAL 95)
    set(c "brightgreen")
  elseif(P GREATER_EQUAL 80)
    set(c "green")
  elseif(P GREATER_EQUAL 60)
    set(c "yellowgreen")
  elseif(P GREATER_EQUAL 40)
    set(c "yellow")
  elseif(P GREATER_EQUAL 20)
    set(c "orange")
  else()
    set(c "red")
  endif()
  set(${OUT} "${c}" PARENT_SCOPE)
endfunction()

function(_hex_for OUT NAME)
  if(NAME STREQUAL "brightgreen")
    set(h "4c1")
  elseif(NAME STREQUAL "green")
    set(h "97CA00")
  elseif(NAME STREQUAL "yellowgreen")
    set(h "a4a61d")
  elseif(NAME STREQUAL "yellow")
    set(h "dfb317")
  elseif(NAME STREQUAL "orange")
    set(h "fe7d37")
  else()
    set(h "e05d44")
  endif()
  set(${OUT} "${h}" PARENT_SCOPE)
endfunction()

# Clean, flat badge (no gradients/masks). If LABEL is empty, it's a single bar.
function(_make_flat_badge OUT LABEL MSG COLOR)
  string(LENGTH "${LABEL}" lab_len)
  string(LENGTH "${MSG}"   msg_len)

  if(lab_len GREATER 0)
    math(EXPR lw "7*${lab_len}+14")
  else()
    set(lw 0)
  endif()
  math(EXPR mw "7*${msg_len}+14")
  math(EXPR w  "${lw}+${mw}")
  set(h 20)

  _hex_for(hex "${COLOR}")

  set(svg "<svg xmlns='http://www.w3.org/2000/svg' width='${w}' height='${h}' role='img' aria-label='${LABEL}: ${MSG}'>")
  string(APPEND svg "\n  <rect width='${w}' height='${h}' rx='3' fill='#0000'/>\n")

  if(lw GREATER 0)
    string(APPEND svg "  <rect width='${lw}' height='${h}' rx='3' fill='#555'/>\n")
    string(APPEND svg "  <rect x='${lw}' width='${mw}' height='${h}' fill='#${hex}'/>\n")
  else()
    string(APPEND svg "  <rect width='${mw}' height='${h}' rx='3' fill='#${hex}'/>\n")
  endif()

  string(APPEND svg "  <g fill='#fff' text-anchor='middle' font-family='DejaVu Sans,Verdana,Geneva,sans-serif' font-size='11'>\n")

  if(lw GREATER 0)
    string(APPEND svg "    <text x='${lw}/2' y='14'>${LABEL}</text>\n")
    string(APPEND svg "    <text x='${lw}+${mw}/2' y='14'>${MSG}</text>\n")
  else()
    math(EXPR cx "${mw}/2")
    string(APPEND svg "    <text x='${cx}' y='14'>${MSG}</text>\n")
  endif()

  string(APPEND svg "  </g>\n</svg>")
  set(${OUT} "${svg}" PARENT_SCOPE)
endfunction()

# ----- Load L10n JSONs -----
set(SEEN_LANGS "")
_read_json("${UI_JSON}"      UI)
_read_json("${DOCS_JSON}"    DOCS)
_read_json("${WIN_JSON}"     WIN)
_read_json("${ART_JSON}"     ART)
_read_json("${LICENSE_JSON}" LICENSE)

list(REMOVE_DUPLICATES SEEN_LANGS)
set(LANGS ${SEEN_LANGS})
list(SORT LANGS)
list(JOIN LANGS "," _langs_joined)
message(STATUS "[G11N] LANGS detected: ${_langs_joined}")

# ----- Weights -----
set(W_UI 3)
set(W_DOCS 2)
set(W_INST 1)
set(W_ART 0)
set(W_LICENSE 1)

string(REPLACE "," ";" _wlist "${WEIGHTS}")
foreach(tok IN LISTS _wlist)
  string(REPLACE "=" ";" kv "${tok}")
  list(LENGTH kv _kvlen)
  if(_kvlen EQUAL 2)
    list(GET kv 0 k)
    list(GET kv 1 v)
    string(TOLOWER "${k}" k)
    if(k STREQUAL "ui")
      set(W_UI "${v}")
    elseif(k STREQUAL "docs")
      set(W_DOCS "${v}")
    elseif(k STREQUAL "installer")
      set(W_INST "${v}")
    elseif(k STREQUAL "art")
      set(W_ART "${v}")
    elseif(k STREQUAL "license")
      set(W_LICENSE "${v}")
    endif()
  endif()
endforeach()

math(EXPR W_SUM "${W_UI}+${W_DOCS}+${W_INST}+${W_ART}+${W_LICENSE}")
if(W_SUM EQUAL 0)
  message(FATAL_ERROR "All weights are zero; nothing to average.")
endif()

# ----- Compute L10n per-language, cache values, create badges -----
set(OVERALL_NUM 0)
set(OVERALL_DEN 0)

set(SUM_UI 0)
set(SUM_DOCS 0)
set(SUM_INST 0)
set(SUM_ART 0)
set(SUM_LICENSE 0)

file(MAKE_DIRECTORY "${OUT_BADGES}")

foreach(L IN LISTS LANGS)
  _val_or(UI_${L} 0)
  set(p_ui "${RET}")

  _val_or(DOCS_${L} 0)
  set(p_docs "${RET}")

  _val_or(WIN_${L} 0)
  set(p_win "${RET}")
  set(p_inst "${p_win}")  # installer = Windows for now

  _val_or(ART_${L} 0)
  set(p_art "${RET}")

  _val_or(LICENSE_${L} 0)
  set(p_license "${RET}")

  math(EXPR num "${p_ui}*${W_UI} + ${p_docs}*${W_DOCS} + ${p_inst}*${W_INST} + ${p_art}*${W_ART} + ${p_license}*${W_LICENSE}")
  math(EXPR p_all "${num} / ${W_SUM}")

  _bar10(bar "${p_all}")
  _color_for(color "${p_all}")
  _make_flat_badge(svg "l10n (${L})" "${p_all}% ${bar}" "${color}")
  file(WRITE "${OUT_BADGES}/lang-${L}.svg" "${svg}")

  foreach(_tag IN ITEMS ui docs installer license art)
    if(_tag STREQUAL "ui")
      set(_val "${p_ui}")
    elseif(_tag STREQUAL "docs")
      set(_val "${p_docs}")
    elseif(_tag STREQUAL "installer")
      set(_val "${p_inst}")
    elseif(_tag STREQUAL "license")
      set(_val "${p_license}")
    else()
      set(_val "${p_art}")
    endif()

    _bar10(_b "${_val}")
    _color_for(_c "${_val}")
    _make_flat_badge(_s "" "${_val}% ${_b}" "${_c}")
    file(WRITE "${OUT_BADGES}/${_tag}-${L}.svg" "${_s}")
  endforeach()

  math(EXPR SUM_UI      "${SUM_UI}+${p_ui}")
  math(EXPR SUM_DOCS    "${SUM_DOCS}+${p_docs}")
  math(EXPR SUM_INST    "${SUM_INST}+${p_inst}")
  math(EXPR SUM_ART     "${SUM_ART}+${p_art}")
  math(EXPR SUM_LICENSE "${SUM_LICENSE}+${p_license}")

  set(UI_PCT_${L}      "${p_ui}")
  set(DOCS_PCT_${L}    "${p_docs}")
  set(INST_PCT_${L}    "${p_inst}")
  set(LICENSE_PCT_${L} "${p_license}")
  set(ART_PCT_${L}     "${p_art}")
  set(PALL_${L} "${p_all}")
  set(BAR_${L}  "${bar}")

  math(EXPR OVERALL_NUM "${OVERALL_NUM}+${p_all}")
  math(EXPR OVERALL_DEN "${OVERALL_DEN}+1")
endforeach()

# L10n overall badges (across languages)
if(OVERALL_DEN EQUAL 0)
  set(OVERALL 0)
else()
  math(EXPR OVERALL "${OVERALL_NUM}/${OVERALL_DEN}")
endif()
_bar10(OVERALL_BAR "${OVERALL}")
_color_for(OVERALL_COLOR "${OVERALL}")
_make_flat_badge(OVERALL_SVG "" "${OVERALL}% ${OVERALL_BAR}" "${OVERALL_COLOR}")
file(WRITE "${OUT_BADGES}/overall.svg" "${OVERALL_SVG}")

if(OVERALL_DEN EQUAL 0)
  set(AVG_UI 0)
  set(AVG_DOCS 0)
  set(AVG_INST 0)
  set(AVG_ART 0)
  set(AVG_LICENSE 0)
else()
  math(EXPR AVG_UI      "${SUM_UI}/${OVERALL_DEN}")
  math(EXPR AVG_DOCS    "${SUM_DOCS}/${OVERALL_DEN}")
  math(EXPR AVG_INST    "${SUM_INST}/${OVERALL_DEN}")
  math(EXPR AVG_ART     "${SUM_ART}/${OVERALL_DEN}")
  math(EXPR AVG_LICENSE "${SUM_LICENSE}/${OVERALL_DEN}")
endif()

foreach(_tag IN ITEMS ui docs installer art license)
  if(_tag STREQUAL "ui")
    set(_avg "${AVG_UI}")
  elseif(_tag STREQUAL "docs")
    set(_avg "${AVG_DOCS}")
  elseif(_tag STREQUAL "installer")
    set(_avg "${AVG_INST}")
  elseif(_tag STREQUAL "art")
    set(_avg "${AVG_ART}")
  else()
    set(_avg "${AVG_LICENSE}")
  endif()

  _bar10(_b "${_avg}")
  _color_for(_c "${_avg}")
  _make_flat_badge(_s "" "${_avg}% ${_b}" "${_c}")
  file(WRITE "${OUT_BADGES}/${_tag}-overall.svg" "${_s}")
endforeach()

# ----- Read I18n JSONs + badges -----
_read_i18n("${I18N_UI_JSON}"      I18N_UI)
_read_i18n("${I18N_DOCS_JSON}"    I18N_DOCS)
_read_i18n("${I18N_WIN_JSON}"     I18N_WIN)
_read_i18n("${I18N_ART_JSON}"     I18N_ART)
_read_i18n("${I18N_LICENSE_JSON}" I18N_LICENSE)

# helper: write a single i18n badge
macro(_write_i18n_badge VAL OUTNAME)
  set(_val "${VAL}")
  _bar10(_b "${_val}")
  _color_for(_c "${_val}")
  _make_flat_badge(_s "" "${_val}% ${_b}" "${_c}")
  file(WRITE "${OUT_BADGES}/${OUTNAME}.svg" "${_s}")
endmacro()

# Generate i18n badges (one per area)
_write_i18n_badge("${I18N_UI_VALUE}"        "i18n-ui")
_write_i18n_badge("${I18N_DOCS_VALUE}"      "i18n-docs")
_write_i18n_badge("${I18N_WIN_VALUE}"       "i18n-installer")
_write_i18n_badge("${I18N_ART_VALUE}"       "i18n-art")
_write_i18n_badge("${I18N_LICENSE_VALUE}"   "i18n-license")

# ----- Markdown emit -----
string(TIMESTAMP TODAY_YYYY_MM_DD "%Y-%m-%d")

# helper: progress emoji for the roll-up table
function(_emoji_for OUT P)
  if(P GREATER_EQUAL 95)
    set(e "🟢")
  elseif(P GREATER_EQUAL 10)
    set(e "🟡")
  else()
    set(e "🔴")
  endif()
  set(${OUT} "${e}" PARENT_SCOPE)
endfunction()

# Header
set(MD "# 🌐 Globalization Dashboard\n\n> **Project:** ${PROJECT_NAME} · **Last updated:** ${TODAY_YYYY_MM_DD}\n\n")

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

# --- Localization band ---
string(APPEND MD "## 🌍 Localization\n\n_Shipping the product in multiple languages. This section tracks per-language translation status for each area._\n\n")

# Roll-up Status (All Components)
string(APPEND MD "### 📈 Roll-up Status (All Components)\n\n> Overall = weighted average of **UI (${W_UI})**, **Docs (${W_DOCS})**, **Installer (${W_INST})**, **Art (${W_ART})**, **License (${W_LICENSE})**.\n\n")
string(APPEND MD "| Language | Code | Overall | Progress |\n|---|:--:|:--:|:--:|\n")
foreach(L IN LISTS LANGS)
  set(_pall_var "PALL_${L}")
  set(_bar_var  "BAR_${L}")
  set(P_ALL "${${_pall_var}}")
  set(BAR   "${${_bar_var}}")
  _emoji_for(EMJ "${P_ALL}")
  # crude name heuristic: show code as name if we don't have a nicer name map
  string(APPEND MD "| ${L} | \`${L}\` | \`${BAR}\` **${P_ALL}%** | ${EMJ} |\n")
endforeach()
string(APPEND MD "\n---\n\n")

# ===== L10N COMPONENTS =====

# UI
string(APPEND MD "### 1) 🧩 Application UI\n\n")
string(APPEND MD "**Policy**: All visible strings must be externalized into gettext \`.po\` files under \`locale/<lang>/LC_MESSAGES/\`. Avoid hard-coded text in source files. Add translator comments when context isn’t obvious.\n\n")
string(APPEND MD "**Scope**: Menus, dialogs, errors, tooltips, status messages.  \n**Source**: \`src/\`  \n**Locale dir**: \`locale/<lang>/LC_MESSAGES/\`  \n**Recommended tool**: **POEdit** — Update from POT, translate, compile to \`.mo\`.\n\n")
string(APPEND MD "> Use POEdit’s *Catalog → Update from POT/Source code* to merge new strings; keep placeholders intact and run QA before commit.\n\n")
string(APPEND MD "**Status**\n\n| Lang | % | Badge | Notes |\n|:---:|:--:|:--:|---|\n")
foreach(L IN LISTS LANGS)
  set(_v "UI_PCT_${L}")
  set(_pct "${${_v}}")
  if(_pct STREQUAL "")
    set(_pct 0)
  endif()
  string(APPEND MD "| \`${L}\` | **${_pct}** | ![ui ${L}](badges/ui-${L}.svg) |  |\n")
endforeach()
# optional l10n comment
if(NOT "${UI_COMMENT}" STREQUAL "")
  string(APPEND MD "\n${UI_COMMENT}\n")
endif()
string(APPEND MD "\n**Assets**: localized UI screenshots under \`docs/screenshots/ui/<lang>/\`.\n\n---\n\n")

# Installer (Windows)
string(APPEND MD "### 2) 📦 Installer\n\n")
string(APPEND MD "**Policy**: All installer message catalogs must be Unicode-safe and localized by platform.\n\n")
string(APPEND MD "#### 2.1 Windows Installer\n\n")
string(APPEND MD "**Strings**: \`installers/windows/messages/*.isl\`  \n**Encoding**: UTF-8 (Unicode Inno Setup).  \n**Notes**: Preserve placeholders like \`{app}\`, \`{cf}\`, \`%1\`. Add comments above each key to clarify intent.\n\n")
string(APPEND MD "**Status**\n\n| Lang | % | Badge | Notes |\n|:---:|:--:|:--:|---|\n")
foreach(L IN LISTS LANGS)
  set(_v "INST_PCT_${L}")
  set(_pct "${${_v}}")
  if(_pct STREQUAL "")
    set(_pct 0)
  endif()
  string(APPEND MD "| \`${L}\` | **${_pct}** | ![installer ${L}](badges/installer-${L}.svg) |  |\n")
endforeach()
if(NOT "${WIN_COMMENT}" STREQUAL "")
  string(APPEND MD "\n${WIN_COMMENT}\n")
endif()
string(APPEND MD "\n> Future subsections: Linux installer, macOS DMG assets. Each will fold into roll-up weight **Installer = ${W_INST}**.\n\n")
string(APPEND MD "**Assets**: installer icons under \`installers/assets/icons/\`.\n\n---\n\n")

# Docs
string(APPEND MD "### 3) 📚 Documentation (User Manuals)\n\n")
string(APPEND MD "**Policy**: Documentation follows the same translation review cycle as UI. Source text is English Quarto/Markdown; translations tracked via \`.po\` in \`locale/docs/\`.\n\n")
string(APPEND MD "**Docs root**: \`docs/\`  \n**Doc locale**: \`locale/docs/\`  \n**File types**: \`.qmd\` / \`.md\` → \`.po\` → \`.mo\`\n\n")
string(APPEND MD "**Default md2po → po2mo flow**\n```bash\nmd2po docs/ -o locale/docs/<lang>.po --wrapwidth=0\nmsgmerge --update --backup=none locale/docs/<lang>.po locale/docs/template.pot\nmsgfmt locale/docs/<lang>.po -o locale/docs/<lang>.mo\n```\n> Or generate translated Markdown with \`po2md\` for static localized doc sets.\n\n")
string(APPEND MD "**Status**\n\n| Lang | % | Badge | Path |\n|:---:|:--:|:--:|---|\n")
foreach(L IN LISTS LANGS)
  set(_v "DOCS_PCT_${L}")
  set(_pct "${${_v}}")
  if(_pct STREQUAL "")
    set(_pct 0)
  endif()
  string(APPEND MD "| `${L}` | **${_pct}** | ![docs ${L}](badges/docs-${L}.svg) | `docs/<lang>/` or `locale/docs/<lang>.po` |\n")
endforeach()
if(NOT "${DOCS_COMMENT}" STREQUAL "")
  string(APPEND MD "\n${DOCS_COMMENT}\n")
endif()
string(APPEND MD "\n**Assets**: diagrams/screenshots in \`docs/assets/\`; translated captions in \`.po\` entries.\n\n---\n\n")

# Art
string(APPEND MD "### 4) 🎨 Art & Screenshots\n\n")
string(APPEND MD "**Policy**: Core art assets remain language-neutral. Textual variants are generated at runtime from scriptable sources.\n\n")
string(APPEND MD "**Assets**: \`resources/images/\`  \n**Help screenshots**: generated by \`docs/generate-screenshots.lua\` per language.\n\n")
string(APPEND MD "**Status**\n\n| Lang | % | Badge | Notes |\n|:---:|:--:|:--:|---|\n")
foreach(L IN LISTS LANGS)
  set(_v "ART_PCT_${L}")
  set(_pct "${${_v}}")
  if(_pct STREQUAL "")
    set(_pct 0)
  endif()
  string(APPEND MD "| `${L}` | **${_pct}** | ![art ${L}](badges/art-${L}.svg) | Programmatic generation |\n")
endforeach()
if(NOT "${ART_COMMENT}" STREQUAL "")
  string(APPEND MD "\n${ART_COMMENT}\n")
endif()
string(APPEND MD "\n---\n\n")

# Licenses
string(APPEND MD "### 5) 📜 Licenses\n\n")
string(APPEND MD "**Policy**: Each platform and language must include legally reviewed translations. Use UTF-8 plain text. Keep English master in sync with translated versions.\n\n")
string(APPEND MD "**Suggested paths**: \`licenses/\` (project-wide) or platform-specific under \`installers/*/LICENSE*\`.\n\n")
string(APPEND MD "**Status**\n\n| Lang | % | Badge | Notes |\n|:---:|:--:|:--:|---|\n")
foreach(L IN LISTS LANGS)
  set(_v "LICENSE_PCT_${L}")
  set(_pct "${${_v}}")
  if(_pct STREQUAL "")
    set(_pct 0)
  endif()
  string(APPEND MD "| `${L}` | **${_pct}** | ![license ${L}](badges/license-${L}.svg) |  |\n")
endforeach()
if(NOT "${LICENSE_COMMENT}" STREQUAL "")
  string(APPEND MD "\n${LICENSE_COMMENT}\n")
endif()
string(APPEND MD "\n---\n\n")

# --- Internationalization band ---
string(APPEND MD "## 💻 Internationalization\n\n_Making the product world-ready (Unicode, plural rules, RTL, locale-sensitive formats, input methods). Each area shows a single readiness score plus a short note._\n\n")

# i18n summary table
string(APPEND MD "| Area | % | Badge |\n|---|:--:|:--:|\n")
string(APPEND MD "| 🧩 UI | **${I18N_UI_VALUE}%** | ![i18n ui](badges/i18n-ui.svg) |\n")
string(APPEND MD "| 📦 Installer | **${I18N_WIN_VALUE}%** | ![i18n installer](badges/i18n-installer.svg) |\n")
string(APPEND MD "| 📚 Docs | **${I18N_DOCS_VALUE}%** | ![i18n docs](badges/i18n-docs.svg) |\n")
string(APPEND MD "| 🎨 Art | **${I18N_ART_VALUE}%** | ![i18n art](badges/i18n-art.svg) |\n")
string(APPEND MD "| 📜 License | **${I18N_LICENSE_VALUE}%** | ![i18n license](badges/i18n-license.svg) |\n\n")

# i18n comments beneath
_emit_comment_md(_cblk "${I18N_UI_COMMENT}")
if(NOT "${_cblk}" STREQUAL "")
  string(APPEND MD "\n${_cblk}\n")
endif()
_emit_comment_md(_cblk "${I18N_WIN_COMMENT}")
if(NOT "${_cblk}" STREQUAL "")
  string(APPEND MD "\n${_cblk}\n")
endif()
_emit_comment_md(_cblk "${I18N_DOCS_COMMENT}")
if(NOT "${_cblk}" STREQUAL "")
  string(APPEND MD "\n${_cblk}\n")
endif()
_emit_comment_md(_cblk "${I18N_ART_COMMENT}")
if(NOT "${_cblk}" STREQUAL "")
  string(APPEND MD "\n${_cblk}\n")
endif()
_emit_comment_md(_cblk "${I18N_LICENSE_COMMENT}")
if(NOT "${_cblk}" STREQUAL "")
  string(APPEND MD "\n${_cblk}\n")
endif()

# Write file
get_filename_component(_md_dir "${OUT_MD}" DIRECTORY)
file(MAKE_DIRECTORY "${_md_dir}")
file(WRITE "${OUT_MD}" "${MD}")

message(STATUS "Globalization dashboard written to ${OUT_MD}")
message(STATUS "Badges written to ${OUT_BADGES}")
