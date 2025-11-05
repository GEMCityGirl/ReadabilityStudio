# -----------------------------------------------------------------------------
# DashboardCommon.cmake
#
# Shared helpers for all dashboard generators (Globalization, Documentation, etc.)
#
# NOTES ON GLOBAL STATE:
#
# - MD:
#   The Markdown accumulator variable. Dashboards are responsible for
#   creating it (e.g., `set(MD "")`) before including this file.
#   Helpers in this file may assume MD exists and append to it, but they
#   should *never* reset or overwrite it.
#
# - SEEN_LANGS:
#   Used by _read_json() to collect all language codes seen across multiple
#   JSON files. Each dashboard must initialize it to an empty list before
#   reading (e.g., `set(SEEN_LANGS "")`), then optionally de-duplicate and
#   sort afterward to form the LANGS list.
#
# In short:
#   - Dashboards own MD and SEEN_LANGS lifetimes.
#   - Common helpers only modify or append, never reinitialize.
# -----------------------------------------------------------------------------

cmake_minimum_required(VERSION 3.19)

# ----- Language full names -----
set(LANG_NAME_en "English")
set(LANG_NAME_en_GB "English (UK)")
set(LANG_NAME_es "Spanish")
set(LANG_NAME_de "German")
set(LANG_NAME_fr "French")
set(LANG_NAME_it "Italian")
set(LANG_NAME_pt_BR "Portuguese (Brazil)")
set(LANG_NAME_pt_PT "Portuguese (Portugal)")
set(LANG_NAME_pt "Portuguese")
set(LANG_NAME_ja "Japanese")
set(LANG_NAME_zh_CN "Chinese (Simplified)")
set(LANG_NAME_zh_TW "Chinese (Traditional)")
set(LANG_NAME_ko "Korean")
set(LANG_NAME_ru "Russian")
set(LANG_NAME_ar "Arabic")
set(LANG_NAME_pl "Polish")
set(LANG_NAME_nl "Dutch")
set(LANG_NAME_sv "Swedish")
set(LANG_NAME_cs "Czech")
set(LANG_NAME_tr "Turkish")
set(LANG_NAME_hi "Hindi")

# Resolve human-readable language name from code (fallback to code)
function(_resolve_lang_name OUT CODE)
    if(DEFINED LANG_NAME_${CODE})
        set(_name "${LANG_NAME_${CODE}}")
    else()
        set(_name "${CODE}")
    endif()
    set(${OUT} "${_name}" PARENT_SCOPE)
endfunction()

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

# Read i18n JSON: { "value": <0..100>, "title": "...", "must": "...", "recommended": "...", "comment": "..." }
# Exposes: <OUT>_VALUE, <OUT>_TITLE, <OUT>_MUST, <OUT>_RECOMMENDED, <OUT>_COMMENT (fallback)
function(_read_i18n JSON_FILE OUT_PREFIX)
    if(NOT EXISTS "${JSON_FILE}")
        message(STATUS "[G11N] (i18n) missing file: ${JSON_FILE} → value=0")
        set(${OUT_PREFIX}_VALUE 0 PARENT_SCOPE)
        set(${OUT_PREFIX}_TITLE "" PARENT_SCOPE)
        set(${OUT_PREFIX}_MUST "" PARENT_SCOPE)
        set(${OUT_PREFIX}_RECOMMENDED "" PARENT_SCOPE)
        set(${OUT_PREFIX}_COMMENT "" PARENT_SCOPE)
        return()
    endif()

    file(READ "${JSON_FILE}" _json)

    # value
    set(_val 0)
    if(NOT ("${_json}" STREQUAL ""))
        string(REGEX MATCH "\"value\"" _has_val "${_json}")
        if(NOT _has_val STREQUAL "")
            string(JSON _val GET "${_json}" "value")
        endif()
    endif()

    # optional strings
    set(_title "")
    set(_must  "")
    set(_rec   "")
    set(_cmt   "")

    string(REGEX MATCH "\"title\"" _has_title "${_json}")
    if(NOT _has_title STREQUAL "")
        string(JSON _title GET "${_json}" "title")
    endif()

    string(REGEX MATCH "\"must\"" _has_must "${_json}")
    if(NOT _has_must STREQUAL "")
        string(JSON _must GET "${_json}" "must")
    endif()

    string(REGEX MATCH "\"recommended\"" _has_rec "${_json}")
    if(NOT _has_rec STREQUAL "")
        string(JSON _rec GET "${_json}" "recommended")
    endif()

    string(REGEX MATCH "\"comment\"" _has_cmt "${_json}")
    if(NOT _has_cmt STREQUAL "")
        string(JSON _cmt GET "${_json}" "comment")
    endif()

    set(${OUT_PREFIX}_VALUE "${_val}" PARENT_SCOPE)
    set(${OUT_PREFIX}_TITLE "${_title}" PARENT_SCOPE)
    set(${OUT_PREFIX}_MUST "${_must}" PARENT_SCOPE)
    set(${OUT_PREFIX}_RECOMMENDED "${_rec}" PARENT_SCOPE)
    set(${OUT_PREFIX}_COMMENT "${_cmt}" PARENT_SCOPE)
endfunction()

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

# i18n comments beneath (Baseline Requirements + Recommended Enhancements, per area)
# Fallback: if both are empty, show legacy 'comment' as paragraph/checklist.

# UI
# Build a checklist with an explicit header. COMMENT is a ;/newline list of "0: …" / "1: …" items.
function(_emit_checklist_with_header OUT HEADER COMMENT)
    if(COMMENT STREQUAL "")
        set(${OUT} "" PARENT_SCOPE)
        return()
    endif()

    # Normalize separators
    string(REPLACE "\n" ";" _s "${COMMENT}")
    set(_parts ${_s})

    # If the first element matches the header, remove it
    list(GET _parts 0 _first)
    string(STRIP "${_first}" _first)
    if(_first STREQUAL "${HEADER}")
        list(REMOVE_AT _parts 0)
    endif()

    set(_out "#### ${HEADER}\n")
    foreach(_tok IN LISTS _parts)
        string(STRIP "${_tok}" _tok)
        if(_tok STREQUAL "")
            continue()
        endif()

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

    set(${OUT} "${_out}\n" PARENT_SCOPE)
endfunction()

# Make an SVG badge from a numeric value (0–100)
# OUT_SVG = SVG string
function(_badge_from_value OUT_SVG VAL)
    set(_val "${VAL}")
    _bar10(_b "${_val}")
    _color_for(_c "${_val}")
    _make_flat_badge(_s "" "${_val}% ${_b}" "${_c}")
    set(${OUT_SVG} "${_s}" PARENT_SCOPE)
endfunction()

macro(_emit_section TITLE_VAR MUST_VAR REC_VAR CMT_VAR)
    # Optional per-area title (from JSON "title")
    if(NOT "${${TITLE_VAR}}" STREQUAL "")
        string(APPEND MD "#### ${${TITLE_VAR}}\n\n")
    endif()

    # Baseline Requirements
    _emit_checklist_with_header(_blk "Baseline Requirements" "${${MUST_VAR}}")
    if(NOT "${_blk}" STREQUAL "")
        string(APPEND MD "${_blk}")
    endif()

    # Recommended Enhancements (or fallback to legacy 'comment')
    _emit_checklist_with_header(_blk "Recommended Enhancements" "${${REC_VAR}}")
    if(NOT "${_blk}" STREQUAL "")
        string(APPEND MD "${_blk}")
    elseif(NOT "${${CMT_VAR}}" STREQUAL "")
        _emit_comment_md(_fallback "${${CMT_VAR}}")
        string(APPEND MD "${_fallback}\n")
    endif()
endmacro()
