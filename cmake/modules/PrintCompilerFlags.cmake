# =============================================================================
#  PrintCompilerFlags.cmake
#
#  Purpose:
#    Extracts and prints a human-readable summary of the *actual* compiler
#    flags applied to a target. Generator expressions are ignored entirely to
#    avoid configuration-time misinterpretation and platform-specific noise.
#
#  Features:
#    • Skips all generator expressions (tokens starting with "$<" or ending ">")
#    • Identifies and classifies real compiler flags (warnings, optimizations,
#      security flags, MSVC extensions, OpenMP, etc.)
#    • Prints a clean, annotated list of the meaningful flags used to build
#      the target.
#    • Produces an exportable multi-line string (COMPILE_FLAGS_SUMMARY) for
#      embedding in the application (e.g., About dialogs or diagnostic output).
#
#  Notes:
#    • Only *concrete* flags are summarized. Generator expressions vary by
#      configuration and compiler and cannot be evaluated reliably at configure
#      time, so they are intentionally excluded.
#    • Safe to call after all target_compile_options() and related settings.
#
# Exports to parent scope:
#   COMPILE_FLAGS_SUMMARY     (pretty, multi-line text to print in build script)
#   COMPILE_FLAGS_SUMMARY_STR (pretty, multi-line text to export to code)
#
# =============================================================================

set(COMPILE_FLAGS_SUMMARY "")
set(COMPILE_FLAGS_SUMMARY_STR "")

# ======================================================
# Extracts the main flag from a compile option
# ======================================================
function(extract_flag_from_expression flag out_var)

    # Remove surrounding single quotes added by 'message'
    string(REGEX REPLACE "^'(.*)'$" "\\1" expr "${flag}")

    # If not a generator expression, return as-is
    if(NOT "${expr}" MATCHES "^\\$<.*>$")
        set(${out_var} "${expr}" PARENT_SCOPE)
        return()
    endif()

    # Extract substring after last colon and before final >
    string(REGEX REPLACE "^.*:([^>]+)>$" "\\1" payload_raw "${expr}")

    # Split payload into tokens
    separate_arguments(payload_tokens NATIVE_COMMAND "${payload_raw}")

    # First flag token
    list(GET payload_tokens 0 primary)

    set(${out_var} "${primary}" PARENT_SCOPE)
endfunction()

# ======================================================
# Detect generator expressions reliably
# ======================================================
function(is_generator_expression token result)
    # Any token that *starts* with "$<" is part of a generator expression
    if("${token}" MATCHES "^\\$<")
        set(${result} TRUE PARENT_SCOPE)
        return()
    endif()

    # Also skip if it is a dangling remainder of a split GE: "<", ">", "-g3>", etc.
    if("${token}" MATCHES ".*>$" OR "${token}" STREQUAL ">" OR "${token}" STREQUAL "<")
        set(${result} TRUE PARENT_SCOPE)
        return()
    endif()

    set(${result} FALSE PARENT_SCOPE)
endfunction()

# ======================================================
# Pretty-prints meaningful flag descriptions
# ======================================================
function(print_compile_flags_summary TARGET_NAME)
    get_target_property(_flags ${TARGET_NAME} COMPILE_OPTIONS)

    if(NOT _flags)
        message(WARNING "No compile options found for target ${TARGET_NAME}")
        set(COMPILE_FLAGS_SUMMARY "" PARENT_SCOPE)
        return()
    endif()

    message(STATUS "=== Compile Flag Summary ===")

    set(_summary_list "")

    foreach(flag IN LISTS _flags)
        is_generator_expression("${flag}" is_genex)
        if(is_genex)
            continue()
        endif()

        extract_flag_from_expression("${flag}" clean_flag)

        set(line "")

        # -------- Flag classification --------
        if(clean_flag STREQUAL "-Wall" OR clean_flag STREQUAL "/W3")
            set(line "• ${clean_flag}: Enable common warnings")

        elseif(clean_flag STREQUAL "-Wextra" OR clean_flag STREQUAL "/W4")
            set(line "• ${clean_flag}: Enable extra warnings")

        elseif(clean_flag STREQUAL "-Wpedantic")
            set(line "• ${clean_flag}: Enforce strict ISO compliance")

        elseif(clean_flag STREQUAL "-Wshadow")
            set(line "• ${clean_flag}: Warn about variable shadowing")

        elseif(clean_flag STREQUAL "-Werror" OR clean_flag STREQUAL "/WX")
            set(line "• ${clean_flag}: Treat warnings as errors")

        elseif(clean_flag STREQUAL "-fstack-protector-strong")
            set(line "• ${clean_flag}: Enable strong stack protection")

        elseif(clean_flag MATCHES "-D_FORTIFY_SOURCE=2")
            set(line "• ${clean_flag}: Enable fortified libc functions")

        elseif(clean_flag STREQUAL "/sdl")
            set(line "• ${clean_flag}: Enable SDL security checks") # MSVC

        elseif(clean_flag STREQUAL "/permissive-")
            set(line "• ${clean_flag}: Enforce strict ISO C++") # MSVC

        elseif(clean_flag STREQUAL "/MP" OR clean_flag STREQUAL "/Zc:__cplusplus")
            # intentionally ignored
            continue()

        elseif(clean_flag STREQUAL "/wd6211")
            message(STATUS "• ${clean_flag}: Suppress MSVC code analysis warning C6211 (false-positive memory leak detection)")

        elseif(clean_flag STREQUAL "-Og" OR clean_flag STREQUAL "/Od")
            set(line "• ${clean_flag}: Optimize for debugging")

        elseif(clean_flag STREQUAL "-O2" OR clean_flag STREQUAL "/O2")
            set(line "• ${clean_flag}: Optimize for speed")

        elseif(clean_flag STREQUAL "-g3")
            set(line "• ${clean_flag}: Full debug info")

        elseif(clean_flag STREQUAL "-fopenmp" OR clean_flag STREQUAL "/openmp")
            set(line "• ${clean_flag}: Enable OpenMP parallelism")

        else()
            set(line "• ${clean_flag}: [Unrecognized or custom flag]")
        endif()

        if(line)
            message(STATUS "${line}")
            list(APPEND _summary_list "${line}")
        endif()

    endforeach()

    # Convert list to pretty multi-line string
    string(REPLACE ";" "\n" COMPILE_FLAGS_SUMMARY "${_summary_list}")
    # Make a human-readable list for code (e.g., About box) also
    string(REPLACE "\n" "\\n" COMPILE_FLAGS_SUMMARY_STR "${COMPILE_FLAGS_SUMMARY}")
    string(REPLACE "\"" "\\\"" COMPILE_FLAGS_SUMMARY_STR "${COMPILE_FLAGS_SUMMARY_STR}")

    # Export to parent scope
    set(COMPILE_FLAGS_SUMMARY "${COMPILE_FLAGS_SUMMARY}" PARENT_SCOPE)
    set(COMPILE_FLAGS_SUMMARY_STR "${COMPILE_FLAGS_SUMMARY_STR}" PARENT_SCOPE)
endfunction()
