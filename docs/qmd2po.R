#' Quarto → PO Extraction Toolkit
#'
#' @description
#' Tools for converting human-readable **Quarto (.qmd)** Markdown documents
#' into **gettext (.po)** translation templates, preserving structure, context,
#' and existing translations.
#'
#' This suite provides a complete workflow for documentation localization:
#'
#' * **`qmd2po()`** — The main extractor. Walks through a Quarto or Markdown
#'   document, skips code, YAML, math, LaTeX, tables, and HTML, and emits
#'   only human-readable prose as gettext entries. Optionally adds source
#'   line numbers (`msgctxt`) for translators.
#'
#' * **`qmd2po_folder()`** — Batch mode for entire documentation trees.
#'   Scans recursively, runs integrity checks, concatenates all `.qmd` files,
#'   and produces a unified `.po` catalog.
#'
#' * **`qmd_check_integrity()`** — Sanity-checks `.qmd` files for malformed
#'   Markdown, unbalanced fences, TODO markers, and other authoring issues that
#'   can break rendering or translation extraction.
#'
#' @details
#' The extractor is designed for static documentation pipelines that rebuild
#' frequently while preserving human translations. It plays well with both
#' **Poedit** and automated translation tools.
#'
#' Code fences, callouts, YAML headers, math blocks, HTML comments, and
#' Quarto shortcodes are automatically skipped. Inline LaTeX commands,
#' Markdown links, citations, and inline R code are cleaned from the
#' extracted text to yield consistent message IDs.
#'
#' @note
#' Suppression blocks can be used to exclude sections from translation or
#' integrity checking. Both **md2po** and **Quneiform-specific** comment
#' styles are recognized:
#'
#' ```
#' <!-- translate:off -->
#'   (content excluded from translation)
#' <!-- translate:on -->
#'
#' <!-- quneiform-suppress-begin -->
#'   (content excluded from translation)
#' <!-- quneiform-suppress-end -->
#' ```
#'
#' These can appear anywhere in the document and may span multiple lines.
#'
#' @seealso [gettext], [Poedit], [qmd2po()], [parse_po()]
#'
#' @author
#' Blake Madden
#'
#' @keywords quarto translation localization po gettext documentation

if (nchar(system.file(package="pacman")) == 0)
{
  install.packages("pacman")
}
library(pacman)
pacman::p_load(tidyverse, this.path, glue)

#' @title Extract translatable lines from a Quarto or Markdown file
#'
#' @description
#' Returns a tibble of lines that represent natural-language text suitable for
#' translation, with non-translatable structures (code, YAML, math, tables, etc.)
#' and suppression blocks filtered out.
#'
#' Used internally by both [qmd2po()] and [po2qmd()] to guarantee identical
#' line selection and normalization.
#'
#' @param lines Character vector of file lines (from [readr::read_lines()]).
#' @return Tibble with columns:
#'   * `line_num` – line number
#'   * `text` – original text
#'   * `text_trim` – trimmed version
#'   * `text_clean` – normalized text used for msgid/msgstr matching
#' @keywords internal
extract_translatable_lines <- function(lines) {
  scan_fences(lines) %>%
    dplyr::filter(
      !in_code, !in_yaml, !in_math, !in_callout,
      !in_html_comment, !in_suppress
    ) %>%
    dplyr::mutate(
      text_trim = stringr::str_replace_all(text, "\\p{Z}", " ") |> stringr::str_trim(),
      skip =
        text_trim == "" |
        stringr::str_detect(text_trim, "^#+\\s*`[^`]+`\\s*(\\{#[-A-Za-z0-9_]+\\})?\\s*$") |  # headings like "# `foo`" or "## `foo` {#id}"
        stringr::str_detect(stringr::str_replace_all(text_trim, "\\s+", ""), "^`.*`$") |  # any pure backtick line
        stringr::str_detect(text_trim, "^\\s*(\\[`[^`]+`\\]\\([^)]*\\)\\s*[,:]?\\s*)+$") |  # links like [`Foo()`](#foo)
        stringr::str_detect(text_trim, "^```") |
        stringr::str_detect(text_trim, "^:::") |
        stringr::str_detect(text_trim, "^>\\s*$") |
        stringr::str_detect(text_trim, "^\\$\\$\\s*$") |
        stringr::str_detect(text_trim, "^!\\[.*\\]\\(.*\\)") |
        stringr::str_detect(text_trim, "^<!--.*-->$") |
        stringr::str_detect(text_trim, "^\\{%.*%\\}$") |
        stringr::str_detect(text_trim, "^\\{\\{<.*>\\}\\}$") |
        stringr::str_detect(text_trim, "^\\s{0,3}(\\*{3,}|-{3,}|_{3,})\\s*$") |
        stringr::str_detect(text_trim, "^\\|[\\s:\\-|]+\\|\\s*$") |
        stringr::str_detect(text_trim, "^\\+[\\s:\\-=+]+\\+\\s*$") |
        stringr::str_detect(text_trim, "^\\\\[A-Za-z]+(\\{.*\\})?$") |
        stringr::str_detect(text_trim, "^`r .*`$") |
        stringr::str_detect(text_trim, "^\\[toc\\]$") |
        stringr::str_detect(text_trim, "^\\{\\s*(tbl|fig)[:].*\\}$") |
        stringr::str_detect(text_trim, "^<\\/?[A-Za-z0-9]+.*>$")
    ) %>%
    dplyr::filter(!skip) %>%
    dplyr::mutate(
      text_clean = text_trim %>%
        stringr::str_remove("\\s*\\{#[-A-Za-z0-9_]+\\}\\s*$") %>%
        stringr::str_remove("^#+\\s*") %>%
        stringr::str_remove("^\\*+\\s*") %>%
        stringr::str_remove("^>\\s*")
    ) %>%
    dplyr::filter(nzchar(text_clean))
}

#' Track YAML/code/math/callout/HTML comment fences in a Quarto file
#'
#' @param lines Character vector of lines from a .qmd or .md file
#' @return A tibble with columns: line_num, text, in_yaml, in_code, in_math, in_callout, in_html_comment, in_suppress
#' Track YAML/code/math/callout/HTML comment/suppression fences in a Quarto file
#'
#' @param lines Character vector of lines from a .qmd or .md file
#' @return A tibble with columns: line_num, text, in_yaml, in_code, in_math, in_callout, in_html_comment, in_suppress
scan_fences <- function(lines) {
  n <- length(lines)
  in_code <- in_yaml <- in_math <- in_callout <- in_html_comment <- in_suppress <- logical(n)
  code_state <- yaml_state <- math_state <- callout_state <- html_comment_state <- suppress_state <- FALSE

  for (i in seq_len(n)) {
    line <- lines[i]

    # YAML fences
    if (stringr::str_detect(line, "^---$")) {
      yaml_state <- !yaml_state
      in_yaml[i] <- yaml_state
      next
    }

    # ::: or ``` fences
    if (stringr::str_detect(line, "^(:::|```).*")) {
      if (stringr::str_starts(line, "```")) {
        code_state <- !code_state
        in_code[i] <- code_state
      } else {
        callout_state <- !callout_state
        in_callout[i] <- callout_state
      }
      next
    }

    # $$ math
    if (stringr::str_detect(line, "^\\$\\$$")) {
      math_state <- !math_state
      in_math[i] <- math_state
      next
    }

    # Suppression blocks (quneiform or md2po syntax)
    if (stringr::str_detect(
      line,
      "<!--\\s*(quneiform-suppress-begin|translate:off)\\s*-->"
    )) {
      suppress_state <- TRUE
      in_suppress[i] <- TRUE
      next
    }

    if (suppress_state) in_suppress[i] <- TRUE

    if (stringr::str_detect(
      line,
      "<!--\\s*(quneiform-suppress-end|translate:on)\\s*-->"
    )) {
      suppress_state <- FALSE
      in_suppress[i] <- TRUE
      next
    }

    # HTML comments (multi-line)
    if (stringr::str_detect(line, "<!--")) {
      html_comment_state <- TRUE
    }
    if (html_comment_state) in_html_comment[i] <- TRUE
    if (stringr::str_detect(line, "-->")) {
      html_comment_state <- FALSE
      in_html_comment[i] <- TRUE
      next
    }

    in_yaml[i] <- yaml_state
    in_code[i] <- code_state
    in_math[i] <- math_state
    in_callout[i] <- callout_state
    in_html_comment[i] <- html_comment_state
    in_suppress[i] <- suppress_state
  }

  tibble::tibble(
    line_num = seq_len(n),
    text = lines,
    in_yaml,
    in_code,
    in_math,
    in_callout,
    in_html_comment,
    in_suppress
  )
}

#' @title Check structural and syntax integrity of a Quarto (.qmd) document
#'
#' @description
#' Scans a Quarto or Markdown document for common authoring mistakes
#' that can break rendering.  
#' Detects malformed Markdown, duplicate anchors, TODO markers,
#' suspicious paths, and unbalanced fences or comments—while ignoring
#' code, math, YAML, and LaTeX lines.
#'
#' @param lines Character vector of file lines (e.g., from [readr::read_lines()]).
#' @param file Optional character. File name for display in messages.
#'
#' @return Invisibly returns a tibble of detected issues (`type`, `line`, `snippet`).
#' Prints a summary report only if issues are found.
#'
#' @examples
#' \dontrun{
#' buf <- readr::read_lines("docs/chapter1.qmd")
#' qmd_check_integrity(buf, "docs/chapter1.qmd")
#' }
#' @export
qmd_check_integrity <- function(lines, file = NULL) {
  add_issue <- function(type, idx, msg)
    tibble::tibble(type = type, line = idx, snippet = msg)

  issues <- tibble::tibble(type = character(), line = integer(), snippet = character())
  df_fence <- scan_fences(lines)

  # --- Strip out fenced content
  df <- scan_fences(lines) %>%
    dplyr::filter(!in_code, !in_yaml, !in_math, !in_callout, !in_html_comment, !in_suppress) %>%
    dplyr::mutate(text_trim = stringr::str_trim(text))

  # Common patterns
  latex_cmd_rx <- "^\\\\[A-Za-z]+(\\{.*\\})?"
  id_rx        <- "\\{#([-A-Za-z0-9_]+)\\}"

  # --- High-level fence/comment sanity on the raw file
  count <- function(rx) sum(stringr::str_detect(lines, rx))

  if (count("^---$") %% 2 != 0)
    issues <- dplyr::bind_rows(issues, add_issue("Unbalanced YAML fence", NA, "---"))
  if (count("^```") %% 2 != 0)
    issues <- dplyr::bind_rows(issues, add_issue("Unbalanced code fence", NA, "```"))
  if (count("^:::") %% 2 != 0)
    issues <- dplyr::bind_rows(issues, add_issue("Unbalanced callout fence", NA, ":::"))
  if (count("^\\$\\$$") %% 2 != 0)
    issues <- dplyr::bind_rows(issues, add_issue("Unbalanced math fence", NA, "$$"))
  if (count("<!--") != count("-->"))
    issues <- dplyr::bind_rows(issues, add_issue("Unbalanced HTML comment", NA, "Mismatched <!-- / -->"))

  # helper: is this line a LaTeX command?
  is_latex_cmd <- stringr::str_detect(df$text_trim, "^\\\\")

  # --- Malformed images
  bad_img <- which(
    !is_latex_cmd &
      (
        stringr::str_detect(df$text_trim, "^!\\[.*\\]\\(\\s*\\\".*\\\"\\s*\\)")  |  # quoted path
          stringr::str_detect(df$text_trim, "^!\\[[^\\]]*$")                     |  # no closing ]
          stringr::str_detect(df$text_trim, "^!\\[.*\\]\\([^\\)]*$")                # no closing )
      )
  )
  if (length(bad_img))
    issues <- dplyr::bind_rows(
      issues,
      add_issue("Malformed image", df$line_num[bad_img], df$text[bad_img]))

  # --- Malformed links (skip LaTeX, citations, footnotes, Quarto spans, and harmless bracketed refs)
  is_citation_or_footnote <- 
    stringr::str_detect(df$text_trim, "\\[[-@][^\\]]*\\]") |   # [@...], [-@...]
    stringr::str_detect(df$text_trim, "\\[\\^[^\\]]*\\]")      # [^footnote]

  is_quarto_attr_span <- 
    stringr::str_detect(df$text_trim, "\\]\\s*\\{\\.[^}]+\\}")  # [text]{.class attr="..."}

  is_bold_bracket <- 
    stringr::str_detect(df$text_trim, "\\*\\*\\s*\\[[^\\]]+\\]\\s*\\*\\*") # **[ ... ]**

  # any [digits] pattern not immediately followed by '('
  is_numeric_bracket <- 
    stringr::str_detect(df$text_trim, "\\[[0-9]+\\](?!\\()")    # e.g. [22], [123]

  bad_link <- which(
    !is_latex_cmd &
      !is_citation_or_footnote &
      !is_quarto_attr_span &
      !is_numeric_bracket &
      !is_bold_bracket &
      stringr::str_detect(df$text_trim, "\\[") &
      (
        # Has [ ... ] but a clearly malformed ( ... )
        (stringr::str_detect(df$text_trim, "\\[[^\\]]*\\]") &
           stringr::str_detect(df$text_trim, "\\[[^\\]]*\\]\\([^\\)]*$")) |
          # Or has [ ... ] but no (...) at all
          (stringr::str_detect(df$text_trim, "\\[.*\\]") &
             !stringr::str_detect(df$text_trim, "\\(.*\\)"))))

  if (length(bad_link)) {
    issues <- dplyr::bind_rows(
      issues,
      add_issue("Malformed link", df$line_num[bad_link], df$text[bad_link]))
  }

  # --- Duplicate {#id} anchors
  id_matches <- stringr::str_match_all(df$text, id_rx)
  ids <- unlist(lapply(id_matches, function(m) {
    if (!is.null(m) && nrow(m) > 0) m[, 2, drop = TRUE] else character(0)
  }))
  if (length(ids)) {
    dup_ids <- ids[duplicated(ids)]
    if (length(dup_ids))
      issues <- dplyr::bind_rows(
        issues,
        add_issue("Duplicate ID", NA, paste(unique(dup_ids), collapse = ", ")))
  }

  # --- Empty headings (# {#id} with no title)
  empty_head <- which(stringr::str_detect(df$text_trim, "^#+\\s*\\{#"))
  if (length(empty_head))
    issues <- dplyr::bind_rows(
      issues,
      add_issue("Empty heading", df$line_num[empty_head], df$text[empty_head]))

  # --- Suspicious absolute paths (starting with /)
  bad_path <- which(stringr::str_detect(df$text_trim, "\\]\\(/"))
  if (length(bad_path))
    issues <- dplyr::bind_rows(
      issues,
      add_issue("Suspicious absolute path", df$line_num[bad_path], df$text[bad_path]))

  # --- TODO markers
  todo <- which(stringr::str_detect(df$text_trim, "(?i)\\bTODO\\b"))
  if (length(todo))
    issues <- dplyr::bind_rows(
      issues,
      add_issue("TODO marker", df$line_num[todo], df$text[todo]))

  # --- Long lines (style / consistency)
  long_line <- purrr::keep(seq_len(nrow(df)), function(i) {
    txt <- df$text_trim[i]

    # Explicit break markers mean user-controlled wrapping — skip those lines
    if (stringr::str_detect(txt, "<br>|\\\\linebreak")) {
      return(FALSE)
    }

    # Work on a temporary copy
    tmp <- txt

    # Strip out complete markdown links (including quoted URLs)
    tmp <- stringr::str_remove_all(tmp, "\\[\\\\?\"?https?://[^\\]]*\\]\\([^\\)]*\\)")
    tmp <- stringr::str_remove_all(tmp, "\\[[^\\]]*\\]\\([^\\)]*\\)")

    # Strip LaTeX-style commands, e.g. \index{...}, \textit{...}, etc.
    tmp <- stringr::str_remove_all(tmp, "\\\\[A-Za-z]+\\{[^}]*\\}")

    # Strip inline R code chunks like `r something(...)`
    tmp <- stringr::str_remove_all(tmp, "`r [^`]+`")

    # Strip Pandoc citations and cross-refs:
    tmp <- stringr::str_remove_all(tmp, "\\[[-@][^\\]]*\\]")   # [@ref], [-@ref]
    tmp <- stringr::str_remove_all(tmp, "(?<=\\s)@[-A-Za-z0-9_:.]+")  # bare @ref
    tmp <- stringr::str_remove_all(tmp, "\\[\\^[^\\]]*\\]")     # [^footnote]

    # Only flag by character count (no punctuation heuristic)
    nchar(tmp) > 250
  })

  if (length(long_line)) {
    line_lengths <- nchar(df$text_trim[long_line])
    snippet_with_len <- paste0("[LINE LENGTH: ", line_lengths, "] ", df$text[long_line])

    issues <- dplyr::bind_rows(
      issues,
      add_issue(
        "Possible long line",
        df$line_num[long_line],
        snippet_with_len))
  }

  # --- Output only if there are issues
  if (nrow(issues) > 0) {
    cat("\n⚠️  Detected", nrow(issues), "potential issue(s) in", file %||% "<buffer>", "\n\n")
    print(dplyr::arrange(issues, line))
  }

  invisible(issues)
}

#' @title Parse a simple PO file into a data frame
#'
#' @description
#' Reads an existing GNU gettext-style PO file and extracts all `msgid` /
#' `msgstr` pairs into a tidy [tibble][tibble::tibble].  
#' This helper is designed for merging translations back into regenerated PO files
#' created by [qmd2po()], preserving existing translations instead of overwriting them.
#'
#' @param po_path Character. Path to the `.po` file to read.
#'
#' @details
#' The parser handles single- and multi-line `msgid` / `msgstr` entries, skips
#' comment lines and headers, and concatenates any continuation lines wrapped in
#' quotes.  
#' It does **not** currently process plural forms, fuzzy markers, or obsolete
#' entries (`#~`). It is intended for clean, machine-generated PO files typical of
#' static documentation pipelines.
#'
#' @return
#' A tibble with two columns:
#' \describe{
#'   \item{msgid}{Original source string (character).}
#'   \item{msgstr}{Existing translation, possibly empty (character).}
#' }
#' Returns `NULL` if the specified file does not exist or contains no valid entries.
#'
#' @examples
#' \dontrun{
#' # Parse an existing PO file
#' existing <- parse_po("translations/all.po")
#'
#' # Inspect the first few translations
#' head(existing)
#'
#' # Use in a merge
#' if (!is.null(existing)) {
#'   translated <- dplyr::filter(existing, nzchar(msgstr))
#' }
#' }
#'
#' @seealso [qmd2po()], [qmd2po_folder()]
parse_po <- function(po_path) {
  if (!file.exists(po_path)) return(NULL)

  lines <- readr::read_lines(po_path)
  entries <- list()
  msgid <- msgstr <- NULL
  state <- NULL

  for (ln in lines) {
    # skip header or comments
    if (stringr::str_starts(ln, "#")) next
    if (stringr::str_trim(ln) == "") next

    if (stringr::str_starts(ln, "msgid ")) {
      msgid <- stringr::str_match(ln, '^msgid "(.*)"$')[, 2]
      msgstr <- ""
      state <- "id"
      next
    }

    if (stringr::str_starts(ln, "msgstr ")) {
      msgstr <- stringr::str_match(ln, '^msgstr "(.*)"$')[, 2]
      state <- "str"
      next
    }

    if (!is.null(state) && state == "id" && stringr::str_starts(ln, '"')) {
      msgid <- paste0(msgid, stringr::str_match(ln, '^"(.*)"$')[, 2])
      next
    }

    if (!is.null(state) && state == "str" && stringr::str_starts(ln, '"')) {
      msgstr <- paste0(msgstr, stringr::str_match(ln, '^"(.*)"$')[, 2])
      next
    }

    # commit an entry once we leave a block
    if (!is.null(msgid) && !is.null(msgstr) && nzchar(msgid) && state == "str") {
      entries[[msgid]] <- msgstr
      msgid <- msgstr <- NULL
      state <- NULL
    }
  }

  # catch last entry if loop ended mid-string
  if (!is.null(msgid) && !is.null(msgstr) && nzchar(msgid)) {
    entries[[msgid]] <- msgstr
  }

  if (length(entries) == 0) return(NULL)

  # after building the tibble
  tibble::tibble(
    msgid = names(entries),
    msgstr = unlist(entries, use.names = FALSE)
  ) %>%
    dplyr::mutate(
      msgid  = stringr::str_replace_all(msgid,  "\\\\", "\\"),
      msgstr = stringr::str_replace_all(msgstr, "\\\\", "\\")
    )
}

#' qmd2po: Extract translatable text from Quarto (.qmd) documents into a .po file
#'
#' Reads a Quarto/Markdown file line-by-line and extracts natural-language text
#' suitable for translation, ignoring YAML headers, code fences, images, LaTeX,
#' tables column specs, HTML, and other non-translatable syntax.
#'
#' Produces a gettext-compatible `.po` file with proper headers, ready for use in
#' Poedit or gettext workflows. Optionally prints the extracted lines instead of
#' writing a file (`dry_run = TRUE`).
#'
#' @param input Path to the input `.qmd` file.
#' @param output Path to the output `.po` file. Ignored if `dry_run = TRUE`.
#' @param dry_run If TRUE, shows extracted text without writing a file. (`output` argument will be ignored.)
#' @param lang Two-letter language code for the PO header (e.g. `"de"`, `"es"`).
#' @param use_context If TRUE, adds line information to output `.po` file.
#'
#' @return Invisibly returns a tibble of extracted lines if `dry_run = TRUE`.
#'
#' @note
#' Both *md2po* and *Quneiform*-style suppression comments are
#' supported. Any content wrapped inside either of these pairs will be
#' completely ignored during extraction and not included in the output PO file:
#'
#' ```
#' <!-- translate:off -->
#'   (content excluded from translation)
#' <!-- translate:on -->
#'
#' <!-- quneiform-suppress-begin -->
#'   (content excluded from translation)
#' <!-- quneiform-suppress-end -->
#' ```
#'
#' This allows authors to protect boilerplate text,
#' code snippets, or other non-translatable content from being extracted.
#'
#' @examples
#' # Preview only
#' qmd2po("chapter1.qmd", dry_run = TRUE)
#'
#' # Generate PO with line-context for Poedit
#' qmd2po("chapter1.qmd", "chapter1.po", lang = "es", use_context = TRUE)
#'
#' @seealso [qmd2po_folder()]
#' @export
qmd2po <- function(input, output = NULL, dry_run = FALSE, lang = "en", use_context = TRUE) {
  lines <- readr::read_lines(input)
  df_fence <- scan_fences(lines)

  # regex helpers
  latex_cmd_rx   <- "^\\\\[A-Za-z]+(\\{.*\\})?$"
  inline_r_rx    <- "^`r .*`$"
  list_marker_rx <- "^[*\\-\\+]\\s*$"
  toc_rx         <- "^\\[toc\\]$"
  quarto_obj_rx  <- "^\\{\\s*(tbl|fig)[:].*\\}$"
  html_tag_rx    <- "^<\\/?[A-Za-z0-9]+.*>$"
  hrule_rx       <- "^\\s{0,3}(\\*{3,}|-{3,}|_{3,})\\s*$"   # *** --- ___
  grid_border_rx <- "^\\+[\\s:\\-=+]+\\+\\s*$"               # +:----+:----+
  pipe_align_rx  <- "^\\|[\\s:\\-|]+\\|\\s*$"                # | :-- | :-- |

  # Trim lines and filter out fenced content
  df <- df_fence %>%
    dplyr::filter(!in_code, !in_yaml, !in_math, !in_callout, !in_html_comment, !in_suppress) %>%
    dplyr::mutate(text_trim = stringr::str_trim(text))

  # Warn for malformed images
  invalid_candidates <- df %>%
    dplyr::filter(
      (stringr::str_detect(text_trim, "^!\\[") &
         !stringr::str_detect(text_trim, "^!\\[.*\\]\\(.*\\)")) |
        stringr::str_detect(text_trim, "^!\\[.*\\]\\(\\\".*\\\"\\)"))
  if (nrow(invalid_candidates) > 0) {
    cat("\n⚠️  Warning: possible malformed Markdown image lines detected:\n")
    purrr::walk(invalid_candidates$text_trim, ~cat("  •", .x, "\n"))
    cat("  (Common causes: missing '(', misplaced quotes, or mismatched brackets.)\n\n")
  }

  # Apply skip rules
  df <- extract_translatable_lines(lines)
  
  # Stop if nothing left
  if (nrow(df) == 0) {
    message("No translatable lines found.")
    return(invisible(NULL))
  }

  # --- Deduplicate text but keep all source line contexts, sorted
  if (use_context) {
    df <- df %>%
      dplyr::group_by(text_clean) %>%
      dplyr::summarise(
        lines = paste(sort(unique(line_num)), collapse = ", "),
        .groups = "drop") %>%
      dplyr::arrange(as.numeric(stringr::str_split_i(lines, ",", 1)))
  } else {
    df <- df %>% dplyr::distinct(text_clean, .keep_all = TRUE)
  }

  # Dry run output
  if (dry_run) {
    cat("\n🧩 Dry run — lines that would be extracted:\n\n")
    cat(paste0(df$text_clean, collapse = "\n"))
    cat("\n\n")
    return(invisible(df))
  }

  # PO header
  header_block <- paste0(
    'msgid ""\n',
    'msgstr ""\n',
    '"Project-Id-Version: PACKAGE VERSION\\n"\n',
    '"POT-Creation-Date: ', format(Sys.time(), "%Y-%m-%d %H:%M%z"), '\\n"\n',
    '"PO-Revision-Date: YEAR-MO-DA HO:MI+ZONE\\n"\n',
    '"Last-Translator: FULL NAME <EMAIL@ADDRESS>\\n"\n',
    '"Language-Team: LANGUAGE <LL@li.org>\\n"\n',
    '"Language: ', lang, '\\n"\n',
    '"MIME-Version: 1.0\\n"\n',
    '"Content-Type: text/plain; charset=UTF-8\\n"\n',
    '"Content-Transfer-Encoding: 8bit\\n"\n\n')
  
  # --- Load existing translations if present
  existing <- parse_po(output)
  has_existing <- !is.null(existing) && nrow(existing) > 0
  
  # Build entries
  po_lines <- df %>%
    dplyr::mutate(
      header = sprintf("#: %s:%s", basename(input), lines),
      msgid  = sprintf(
        'msgid "%s"',
        text_clean |>
          stringr::str_replace_all('\\\\', '\\\\\\\\') |>  # escape backslashes
          stringr::str_replace_all('"', '\\\\\"')          # escape quotes
      ),
      msgstr = purrr::map_chr(
        text_clean,
        ~ {
          if (has_existing && .x %in% existing$msgid) {
            old <- dplyr::filter(existing, msgid == .x)$msgstr
            if (!is.na(old) && nzchar(old)) sprintf('msgstr "%s"', old)
            else 'msgstr ""'
          } else 'msgstr ""'
        }
      )) %>%
    dplyr::mutate(
      po_block = paste(header, msgid, msgstr, "", sep = "\n")) %>%
    dplyr::pull(po_block)
  
  readr::write_lines(c(header_block, po_lines), output)
  message("Wrote ", nrow(df), " entries to ", output, " [Language: ", lang, "]")
}

#' @title Extract translatable text from all Quarto (.qmd) files in a folder
#'
#' @description
#' Scans a directory recursively for all Quarto (`.qmd`) documents, concatenates them
#' into a temporary combined file, and passes the merged content to [qmd2po()] to
#' generate a single translation template (`.po`).
#'
#' The function supports excluding files by regex, performing a dry run (to preview
#' extracted strings), and automatically cleans up the temporary file after processing.
#'
#' @param input_dir Character. Path to the root directory containing `.qmd` files.
#' @param output_po Character. Destination path for the generated `.po` file.
#' @param lang Character. ISO language code for the output PO header. Default `"en"`.
#' @param use_context Logical. If `TRUE`, each entry in the PO file includes a
#'   `msgctxt` field listing the originating line numbers (merged and sorted). Default `FALSE`.
#' @param dry_run Logical. If `TRUE`, prints the extracted text to the console without
#'   writing a `.po` file. Useful for quick validation. Default `FALSE`.
#' @param exclude_pattern Character or `NULL`. Optional regex pattern used to exclude
#'   files whose full paths match (e.g. `"appendix|draft|libraries\\.qmd"`).
#'
#' @details
#' For each `.qmd` file found under `input_dir`, the function:
#'
#' 1. Wraps the file’s content with comment markers showing its origin.
#' 2. Concatenates all files into one temporary `.qmd` file.
#' 3. Calls [qmd2po()] on the merged file to extract translatable sentences.
#' 4. Removes the temporary file once processing completes.
#'
#' Output entries are written in the order encountered. Each entry’s header includes
#' source file and line numbers (if `use_context = TRUE`), enabling precise mapping
#' between original and translated text.
#'
#' @return
#' Invisibly returns the path to the output `.po` file.  
#' Prints progress messages to the console.
#' 
#' @note
#' Both *md2po* and *Quneiform*-style suppression comments are
#' supported. Any content wrapped inside either of these pairs will be
#' completely ignored during extraction and not included in the output PO file:
#'
#' ```
#' <!-- translate:off -->
#'   (content excluded from translation)
#' <!-- translate:on -->
#'
#' <!-- quneiform-suppress-begin -->
#'   (content excluded from translation)
#' <!-- quneiform-suppress-end -->
#' ```
#'
#' This allows authors to protect boilerplate text,
#' code snippets, or other non-translatable content from being extracted.
#'
#' @examples
#' \dontrun{
#' # Build a single .po file from all QMDs under docs/, skipping libraries.qmd
#' qmd2po_folder(
#'   input_dir = "docs",
#'   output_po = "translations/all.po",
#'   lang = "en",
#'   use_context = TRUE,
#'   exclude_pattern = "libraries\\.qmd$"
#' )
#'
#' # Preview extracted strings only
#' qmd2po_folder("docs", "out.po", dry_run = TRUE)
#' }
#'
#' @seealso [qmd2po()]
#' @export
qmd2po_folder <- function(input_dir,
                          output_po,
                          lang = "en",
                          use_context = FALSE,
                          dry_run = FALSE,
                          exclude_pattern = NULL) {
  # Find all .qmd files recursively
  qmd_files <- list.files(
    path = input_dir,
    pattern = "\\.qmd$",
    full.names = TRUE,
    recursive = TRUE)

  if (!is.null(exclude_pattern)) {
    qmd_files <- qmd_files[!stringr::str_detect(qmd_files, exclude_pattern)]
  }

  if (length(qmd_files) == 0) {
    stop("No .qmd files found in ", input_dir)
  }

  message("Found ", length(qmd_files), " QMD files under ", input_dir)

  temp_qmd <- tempfile(fileext = ".qmd")

  all_text <- unlist(lapply(qmd_files, function(f) {
    buf <- readr::read_lines(f, skip_empty_rows = FALSE)

    # --- Run integrity checks
    issues <- qmd_check_integrity(buf, f)

    # --- Stop early on dry run if issues found
    if (dry_run && nrow(issues) > 0) {
      msg <- stringr::str_glue(
      "❌ Integrity issues detected in {f}.
      Dry run aborted — please fix these issues before extraction.")
      stop(msg, call. = FALSE)
    }

    # --- If clean, include file in concatenation
    c(stringr::str_glue('\n\n<!-- ===== Begin: {basename(f)} ===== -->'),
      buf,
      stringr::str_glue('<!-- ===== End: {basename(f)} ===== -->\n\n'))
  }))

  readr::write_lines(all_text, temp_qmd)

  qmd2po(
    input = temp_qmd,
    output = output_po,
    lang = lang,
    use_context = use_context,
    dry_run = dry_run)

  if (file.exists(temp_qmd)) {
    unlink(temp_qmd)
  }
}

###############################
# TM application to QMD files #
###############################

#' @title Apply translations from a PO file back into a Quarto (.qmd) document
#'
#' @description
#' Reconstructs a translated Quarto or Markdown document by applying translations
#' from an existing gettext `.po` file on a **line-by-line** basis.
#'
#' The function reuses the same extraction logic as [qmd2po()] — scanning the
#' document, ignoring YAML/code/math/callout/HTML/suppression blocks, and
#' normalizing text to compute message IDs.  
#' Each matching `msgid` in the PO file is replaced with its `msgstr`,
#' while preserving Markdown syntax such as headings, bullet markers,
#' or `{#id}` anchors.
#'
#' @param input Path to the source `.qmd` file to translate.
#' @param po Path to the `.po` file providing translations.
#' @param output Optional path for the translated `.qmd` file.
#'   If omitted, the input file is overwritten.
#' @param show_missing Logical. If `TRUE`, prints each untranslated line’s text
#'   (truncated to 120 characters) in the console. Default `FALSE`.
#'
#' @return Invisibly returns the tibble of applied translations.
#'
#' @examples
#' \dontrun{
#' po2qmd("docs/chapter1.qmd", "translations/chapter1.po",
#'        output = "docs/chapter1.es.qmd", show_missing = TRUE)
#' }
#'
#' @seealso [qmd2po()], [parse_po()], [scan_fences()]
#' @export
#' @title Apply translations from a PO file back into a Quarto (.qmd) document
#'
#' @description
#' Reconstructs a translated Quarto or Markdown document by applying translations
#' from an existing gettext `.po` file on a **line-by-line** basis.
#'
#' Uses the same extraction logic as [qmd2po()] to identify translatable lines:
#' YAML, code, math, callouts, HTML comments, and suppression blocks are skipped.
#' Each line’s text is normalized (e.g., stripping `#`, bullets, and `{#id}`),
#' matched against the PO’s `msgid`, and replaced with `msgstr` while preserving
#' the original Markdown syntax such as headings and anchors.
#'
#' @param input Path to the source `.qmd` file to translate.
#' @param po Path to the `.po` file providing translations.
#' @param output Optional path for the translated `.qmd` file.
#'   If omitted, the input file is overwritten.
#' @param show_missing Logical. If `TRUE`, prints untranslated line snippets
#'   (truncated to 120 characters) in the console. Default `FALSE`.
#'
#' @return Invisibly returns a tibble of applied translations.
#'
#' @examples
#' \dontrun{
#' po2qmd("docs/chapter1.qmd",
#'        po = "translations/chapter1.po",
#'        output = "docs/chapter1.es.qmd",
#'        show_missing = TRUE)
#' }
#'
#' @seealso [qmd2po()], [parse_po()], [scan_fences()]
#' @export
po2qmd <- function(input, po, output = NULL, show_missing = FALSE) {
  if (!file.exists(input)) stop("Input file not found: ", input)
  if (!file.exists(po)) stop("PO file not found: ", po)

  # Load translation memory
  tm <- parse_po(po)
  if (is.null(tm) || nrow(tm) == 0) {
    stop("No valid translations found in ", po)
  }
  tm <- dplyr::filter(tm, nzchar(msgstr))

  # Read and scan file
  lines <- readr::read_lines(input)

  df <- extract_translatable_lines(lines)

  # Merge with translation memory
  merged <- df %>%
    dplyr::left_join(tm, by = c("text_clean" = "msgid")) %>%
    dplyr::mutate(
      has_translation = !is.na(msgstr) & nzchar(msgstr),
      translated_line = purrr::map2_chr(
        text, 
        dplyr::if_else(has_translation & nzchar(text_clean), text_clean, NA_character_),
        ~ {
          if (is.na(.y) || !nzchar(.y)) .x
          else sub(stringr::fixed(.y), msgstr[which(df$text_clean == .y)[1]], .x, fixed = TRUE)
        }
      )
    )

  # Report translation stats
  translated <- dplyr::filter(merged, has_translation)

  if (nrow(translated) > 0) {
    cat("\nℹ️  ", nrow(translated),
        " line(s) successfully translated in ", input, ":\n", sep = "")
    purrr::walk2(translated$line_num, translated$text_clean, function(num, txt) {
      cat("  • [L", num, "] ", stringr::str_trunc(txt, 120), "\n", sep = "")
    })
  }

  missing <- dplyr::filter(merged, !has_translation)
  if (nrow(missing) > 0) {
    cat("\n⚠️  ", nrow(missing),
        " untranslated line(s) in ", input, "\n", sep = "")
    if (isTRUE(show_missing)) {
      purrr::walk2(missing$line_num, missing$text_trim, function(num, txt) {
        cat("  • [L", num, "] ", stringr::str_trunc(txt, 120), "\n", sep = "")
      })
    }
    cat("  (see PO file for details)\n")
  }

  # Apply translations
  lines[merged$line_num] <- merged$translated_line

  # Write output
  out_path <- output %||% input
  readr::write_lines(lines, out_path)
  cat("✅ Wrote translated file:", out_path, "\n")

  invisible(merged)
}

#' @title Apply a PO translation memory to all Quarto (.qmd) files in a folder
#'
#' @description
#' Copies an entire folder tree, then applies translations from a single `.po`
#' file to every `.qmd` document inside the copied tree.  
#' Non‐QMD files are copied verbatim, so the output folder becomes a localized
#' clone of the source tree.
#'
#' @param input_dir Path to the source folder containing `.qmd` files.
#' @param output_dir Path to the destination folder for the translated tree.
#' @param po Path to the `.po` translation file to apply.
#' @param show_missing Logical. Passed to [po2qmd()] to print untranslated lines.
#'   Default `FALSE`.
#' @param exclude_pattern Optional regex pattern to exclude QMD files whose
#'   full paths match (e.g. `"appendix|draft|sandbox\\.qmd$"`). Default `NULL`.
#'
#' @return Invisibly returns a tibble summary of processed files.
#' @export
po2qmd_folder <- function(input_dir,
                          output_dir,
                          po,
                          show_missing = FALSE,
                          exclude_pattern = NULL) {
  if (!dir.exists(input_dir))
    stop("Input directory not found: ", input_dir)
  if (!file.exists(po))
    stop("PO file not found: ", po)
  
  # Fresh output tree
  if (dir.exists(output_dir)) unlink(output_dir, recursive = TRUE)
  dir.create(output_dir, recursive = TRUE, showWarnings = FALSE)
  
  # Copy full directory tree, preserving structure
  roots <- list.files(input_dir, full.names = TRUE, all.files = FALSE)
  file.copy(from = roots, to = output_dir, recursive = TRUE)
  
  # Find all QMDs under the copied tree
  qmd_files <- list.files(
    output_dir,
    pattern = "\\.qmd$",
    recursive = TRUE,
    full.names = TRUE
  )
  
  # Exclude any matching the provided regex
  if (!is.null(exclude_pattern)) {
    qmd_files <- qmd_files[!stringr::str_detect(qmd_files, exclude_pattern)]
  }
  
  if (length(qmd_files) == 0) {
    message("No .qmd files found under ", output_dir)
    return(invisible(NULL))
  }
  
  message("📂 Found ", length(qmd_files), " QMD file(s) under ", output_dir)
  if (!is.null(exclude_pattern))
    message("Excluded files matching pattern: ", exclude_pattern)
  message("Applying translations from: ", po, "\n")
  
  # Apply TM to each QMD in place (inside output_dir)
  results <- purrr::map_dfr(qmd_files, function(f) {
    message("→ Translating ", f)
    merged <- tryCatch(
      po2qmd(f, po = po, output = f, show_missing = show_missing),
      error = function(e) {
        warning("Failed to process ", f, ": ", conditionMessage(e))
        return(NULL)
      }
    )
    
    if (is.null(merged)) {
      dplyr::tibble(
        file = f,
        translated_lines = NA_integer_,
        total_lines = NA_integer_
      )
    } else {
      dplyr::tibble(
        file = f,
        translated_lines = sum(merged$has_translation),
        total_lines = nrow(merged)
      )
    }
  })
  
  message("\n✅ Completed translation for ", nrow(results), " file(s).")
  invisible(results)
}
