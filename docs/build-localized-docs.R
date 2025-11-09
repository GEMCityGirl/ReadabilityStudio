if (nchar(system.file(package="pacman")) == 0)
{
  install.packages("pacman")
}
library(pacman)
pacman::p_load(tidyverse, this.path, glue)

docFolder <- this.path::this.dir()

source(glue("{docFolder}/qmd2po.R"))

#' @title Build localized documentation for a given language
#'
#' @description
#' Runs both [qmd2po_folder()] and [po2qmd_folder()] to generate a `.po`
#' translation catalog and build translated `.qmd` documents for the specified
#' language.  
#' Produces output under `locale/docs/<lang>/` and `<docFolder>/<lang>/...`
#'
#' @param docFolder Path to the base documentation folder (e.g. `"docs"`).
#' @param lang Two-letter language code (e.g. `"es"`, `"de"`, `"fr"`).
#' @param exclude_pattern Optional regex pattern of QMD files to skip.
#'
#' @examples
#' \dontrun{
#' build_lang_docs("docs", "es", exclude_pattern = "libraries\\.qmd")
#' }
#'
#' @export
build_lang_docs <- function(docFolder, lang, exclude_pattern = NULL) {
  # output folder under locale/docs/<lang>
  out_dir <- file.path(dirname(docFolder), "locale", "docs", lang)
  dir.create(out_dir, recursive = TRUE, showWarnings = FALSE)

  message(glue::glue("🌐 Building documentation for language: {lang}"))
  message(glue::glue("PO output: {out_dir}/{lang}.po\n"))

  # --- Step 1: Generate updated PO catalog
  qmd2po_folder(
    input_dir = glue::glue("{docFolder}"),
    output_po = glue::glue("{out_dir}/{lang}.po"),
    lang = lang,
    use_context = TRUE,
    dry_run = FALSE,
    exclude_pattern = exclude_pattern
  )

  # --- Step 2: Apply translations to produce localized docs
  po2qmd_folder(
    input_dir = glue::glue("{docFolder}/readability-studio-manual"),
    output_dir = glue::glue("{docFolder}/{lang}/readability-studio-manual"),
    po = glue::glue("{out_dir}/{lang}.po"),
    exclude_pattern = exclude_pattern
  )

  message(glue::glue("\n✅ Completed {lang} documentation build."))
  invisible(out_dir)
}

# Build the localized help
build_lang_docs(docFolder, "es", "libraries\\.qmd")
