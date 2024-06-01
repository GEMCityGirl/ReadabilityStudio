if (!require("pacman")) install.packages("pacman")
library(pacman)
pacman::p_load(bookdown, glue, readr, stringr, fs, lubridate, tidyverse, kableExtra,
               Hmisc, cowplot, beeswarm, tinytex, stringi, this.path, janitor)

# If called from RScript, pass in the path to pandoc
if (nchar(Sys.getenv("RSTUDIO_PANDOC")) == 0)
  {
  args = commandArgs(trailingOnly = TRUE)
  Sys.setenv(RSTUDIO_PANDOC=args[1L])
  }

if (nchar(tinytex::tinytex_root()) == 0)
  {
  tinytex::install_tinytex()
  }

docFolder <- this.path::this.dir()
source(glue("{docFolder}/build-enum-files.R"))
source(glue("{docFolder}/ReadabilityStudioDocs/R/appdown.r"))

# delete previous builds
unlink(glue("{docFolder}/coding-bible/docs"), recursive=T)
unlink(glue("{docFolder}/shortcuts-cheatsheet/docs"), recursive=T)
unlink(glue("{docFolder}/ReadabilityStudioDocs/docs"), recursive=T)
unlink(glue("{docFolder}/readability-test-reference/docs"), recursive=T)
unlink(glue("{docFolder}/readability-studio-api/docs"), recursive=T)

# delete helper files copied from the main project into others
clearFolders <- function()
  {
  unlink(glue("{docFolder}/readability-studio-api/images"), recursive=T)
  unlink(glue("{docFolder}/readability-studio-api/latex"), recursive=T)
  unlink(glue("{docFolder}/readability-studio-api/css"), recursive=T)
  unlink(glue("{docFolder}/readability-studio-api/R"), recursive=T)

  unlink(glue("{docFolder}/readability-test-reference/images"), recursive=T)
  unlink(glue("{docFolder}/readability-test-reference/latex"), recursive=T)
  unlink(glue("{docFolder}/readability-test-reference/css"), recursive=T)
  unlink(glue("{docFolder}/readability-test-reference/R"), recursive=T)
  unlink(glue("{docFolder}/readability-test-reference/data"), recursive=T)
  unlink(glue("{docFolder}/readability-test-reference/scoring-notes"), recursive=T)
  unlink(glue("{docFolder}/readability-test-reference/acknowledgements"), recursive=T)
  unlink(glue("{docFolder}/readability-test-reference/english"), recursive=T)
  unlink(glue("{docFolder}/readability-test-reference/spanish"), recursive=T)
  unlink(glue("{docFolder}/readability-test-reference/german"), recursive=T)

  unlink(glue("{docFolder}/ReadabilityStudioDocs/_bookdown_files"), recursive=T)
  unlink(glue("{docFolder}/ReadabilityStudioDocs/ReadabilityStudioManual.aux"))
  unlink(glue("{docFolder}/ReadabilityStudioDocs/ReadabilityStudioManual.log"))
  unlink(glue("{docFolder}/ReadabilityStudioDocs/ReadabilityStudioManual.toc"))
  unlink(glue("{docFolder}/ReadabilityStudioDocs/ReadabilityStudioManual.tex"))
  unlink(glue("{docFolder}/ReadabilityStudioDocs/ReadabilityStudioManual.bcf"))
  unlink(glue("{docFolder}/ReadabilityStudioDocs/ReadabilityStudioManual.log"))
  unlink(glue("{docFolder}/ReadabilityStudioDocs/ReadabilityStudioManual.idx"))
  unlink(glue("{docFolder}/ReadabilityStudioDocs/ReadabilityStudioManual.lof"))
  unlink(glue("{docFolder}/ReadabilityStudioDocs/ReadabilityStudioManual.lot"))
  }

clearFolders()

# Read dynamic values for the projects
setwd(glue("{docFolder}/"))
version_info <- read_delim("version_info.txt", 
                           delim = "\t", escape_double = FALSE, 
                           trim_ws = TRUE)
BUILD_YEAR <- dplyr::filter(version_info, TAG == 'BUILD_YEAR')$VALUE
PROGRAM_VERSION <- dplyr::filter(version_info, TAG == 'VERSION')$VALUE
PROGRAM_AUTHOR <- dplyr::filter(version_info, TAG == 'PROGRAM_AUTHOR')$VALUE
PROGRAM_NAME <- dplyr::filter(version_info, TAG == 'PROGRAM_NAME')$VALUE

# Coding Bible
##############

setwd(glue("{docFolder}/coding-bible/"))
bookdown::render_book(input="index.Rmd",
                      output_dir="docs")

# Shortcuts Cheatsheet
######################

# Note that this book has its own LaTeX files (i.e., does not copy them from ReadabilityStudioDocs).
# This "book" doesn't have any front or back matter, so its preamble TeX file doesn't include that.
setwd(glue("{docFolder}/shortcuts-cheatsheet/"))
file_copy(glue("{docFolder}/ReadabilityStudioDocs/glossary/02-shortcuts.rmd"),
          glue("{docFolder}/shortcuts-cheatsheet/02-shortcuts.rmd"),
          TRUE)
bookdown::render_book(input="index.Rmd",
                      output_format="bookdown::pdf_book",
                      output_dir="docs")
unlink(glue("{docFolder}/shortcuts-cheatsheet/02-shortcuts.rmd"))

# User Manual
#############

UserManualMode = TRUE
IncludeLinux = FALSE
setwd(glue("{docFolder}/ReadabilityStudioDocs/"))
combine_files("01-Overviews.Rmd", "overviews",
              ("(intro|program)"))
combine_files("20-ReadabilityTestsEnglish.rmd", "english")
combine_files("21-ReadabilityTestsSpanish.rmd", "spanish")
combine_files("22-ReadabilityTestsGerman.rmd", "german")
combine_files("90-Acknowledgements.Rmd", "acknowledgements",
              ("(intro)"))
combine_files("91-Glossaries.rmd", "glossary")
combine_files("41-ScoringNotes.rmd", "scoring-notes")
bookdown::render_book(input="index.Rmd",
                      output_format="bookdown::pdf_book",
                      output_dir="docs")
combine_files("01-Overviews.Rmd", "overviews")
combine_files("90-Acknowledgements.Rmd", "acknowledgements")
bookdown::render_book(input="index.Rmd",
                      output_format="bookdown::gitbook",
                      output_dir="docs")
unlink(glue("{docFolder}/ReadabilityStudioDocs/01-Overviews.Rmd"))
unlink(glue("{docFolder}/ReadabilityStudioDocs/20-ReadabilityTestsEnglish.rmd"))
unlink(glue("{docFolder}/ReadabilityStudioDocs/21-ReadabilityTestsSpanish.rmd"))
unlink(glue("{docFolder}/ReadabilityStudioDocs/22-ReadabilityTestsGerman.rmd"))
unlink(glue("{docFolder}/ReadabilityStudioDocs/41-ScoringNotes.rmd"))
unlink(glue("{docFolder}/ReadabilityStudioDocs/90-Acknowledgements.Rmd"))
unlink(glue("{docFolder}/ReadabilityStudioDocs/91-Glossaries.rmd"))
# used on github to serve the book
write(c(" "), file=glue("{docFolder}/ReadabilityStudioDocs/docs/.nojekyll"))

# Programming Manual
####################

# build the API help topics and intellisense file
unlink(glue("{docFolder}/readability-studio-api/enums"), recursive=T) # rebuild this folder and keep the latest content
dir_create(glue("{docFolder}/readability-studio-api/enums"))
write(c("# (PART) Enumerations{-}\n\n# Enumerations\n"), file=glue("{docFolder}/readability-studio-api/enums/01.md"))

enums <- loadEnums(normalizePath(glue("{docFolder}/../resources/scripting/rs-constants.lua")))
writeEnumEditorFile(enums, glue("{docFolder}/../resources/scripting/rs-enums.api"))
writeEnumTopics(enums, glue("{docFolder}/readability-studio-api/enums/"))

setwd(glue("{docFolder}/readability-studio-api/"))
dir_create(glue("{docFolder}/readability-studio-api/images"))
file_copy(glue("{docFolder}/ReadabilityStudioDocs/images/NonGenerated/CC_BY-NC-ND.png"),
          glue("{docFolder}/readability-studio-api/images/CC_BY-NC-ND.png"),
          TRUE)
file_copy(glue("{docFolder}/ReadabilityStudioDocs/images/NonGenerated/cover-programming.pdf"),
          glue("{docFolder}/readability-studio-api/images/cover.pdf"),
          TRUE)
file_copy(glue("{docFolder}/ReadabilityStudioDocs/images/NonGenerated/cover-programming.png"),
          glue("{docFolder}/readability-studio-api/images/cover.png"),
          TRUE)
file_copy(glue("{docFolder}/ReadabilityStudioDocs/modern-language-association.csl"),
          glue("{docFolder}/readability-studio-api/modern-language-association.csl"),
          TRUE)
dir_copy(glue("{docFolder}/ReadabilityStudioDocs/latex"),
         glue("{docFolder}/readability-studio-api/latex"),
         TRUE)
dir_copy(glue("{docFolder}/ReadabilityStudioDocs/css"),
         glue("{docFolder}/readability-studio-api/css"),
         TRUE)
dir_copy(glue("{docFolder}/ReadabilityStudioDocs/R"),
         glue("{docFolder}/readability-studio-api/R"),
         TRUE)

combine_files("10-Classes.Rmd", "classes")
combine_files("20-Libraries.Rmd", "libraries")
combine_files("30-Enums.Rmd", "enums")
bookdown::render_book(input="index.Rmd",
                      output_format="bookdown::pdf_book",
                      output_dir="docs")
unlink(glue("{docFolder}/readability-studio-api/10-Classes.Rmd"))
unlink(glue("{docFolder}/readability-studio-api/20-Libraries.Rmd"))
unlink(glue("{docFolder}/readability-studio-api/30-Enums.Rmd"))

# Tests Reference Manual
####################

UserManualMode = F
setwd(glue("{docFolder}/readability-test-reference/"))
dir_create(glue("{docFolder}/readability-test-reference/images"))
file_copy(glue("{docFolder}/ReadabilityStudioDocs/images/NonGenerated/CC_BY-NC-ND.png"),
          glue("{docFolder}/readability-test-reference/images/CC_BY-NC-ND.png"),
          TRUE)
file_copy(glue("{docFolder}/ReadabilityStudioDocs/images/NonGenerated/cover-tests.pdf"),
          glue("{docFolder}/readability-test-reference/images/cover.pdf"),
          TRUE)
file_copy(glue("{docFolder}/ReadabilityStudioDocs/images/NonGenerated/cover-tests.png"),
          glue("{docFolder}/readability-test-reference/images/cover.png"),
          TRUE)
file_copy(glue("{docFolder}/ReadabilityStudioDocs/images/FleschChart.png"),
          glue("{docFolder}/readability-test-reference/images/FleschChart.png"),
          TRUE)
file_copy(glue("{docFolder}/ReadabilityStudioDocs/images/frygraph.png"),
          glue("{docFolder}/readability-test-reference/images/frygraph.png"),
          TRUE)
file_copy(glue("{docFolder}/ReadabilityStudioDocs/images/LixGauge.png"),
          glue("{docFolder}/readability-test-reference/images/LixGauge.png"),
          TRUE)
file_copy(glue("{docFolder}/ReadabilityStudioDocs/images/raygorgraph.png"),
          glue("{docFolder}/readability-test-reference/images/raygorgraph.png"),
          TRUE)
file_copy(glue("{docFolder}/ReadabilityStudioDocs/images/CrawfordGraph.png"),
          glue("{docFolder}/readability-test-reference/images/CrawfordGraph.png"),
          TRUE)
file_copy(glue("{docFolder}/ReadabilityStudioDocs/images/FraseGraph.png"),
          glue("{docFolder}/readability-test-reference/images/FraseGraph.png"),
          TRUE)
file_copy(glue("{docFolder}/ReadabilityStudioDocs/images/GpmFry.png"),
          glue("{docFolder}/readability-test-reference/images/GpmFry.png"),
          TRUE)
file_copy(glue("{docFolder}/ReadabilityStudioDocs/images/GermanLixGauge.png"),
          glue("{docFolder}/readability-test-reference/images/GermanLixGauge.png"),
          TRUE)
file_copy(glue("{docFolder}/ReadabilityStudioDocs/images/Schwartz.png"),
          glue("{docFolder}/readability-test-reference/images/Schwartz.png"),
          TRUE)
dir_copy(glue("{docFolder}/ReadabilityStudioDocs/english"),
         glue("{docFolder}/readability-test-reference/english"),
         TRUE)
dir_copy(glue("{docFolder}/ReadabilityStudioDocs/spanish"),
         glue("{docFolder}/readability-test-reference/spanish"),
         TRUE)
dir_copy(glue("{docFolder}/ReadabilityStudioDocs/german"),
         glue("{docFolder}/readability-test-reference/german"),
         TRUE)
dir_copy(glue("{docFolder}/ReadabilityStudioDocs/scoring-notes"),
         glue("{docFolder}/readability-test-reference/scoring-notes"),
         TRUE)
dir_copy(glue("{docFolder}/ReadabilityStudioDocs/acknowledgements"),
         glue("{docFolder}/readability-test-reference/acknowledgements"),
         TRUE)
dir_copy(glue("{docFolder}/ReadabilityStudioDocs/latex"),
         glue("{docFolder}/readability-test-reference/latex"),
         TRUE)
dir_copy(glue("{docFolder}/ReadabilityStudioDocs/css"),
         glue("{docFolder}/readability-test-reference/css"),
         TRUE)
dir_copy(glue("{docFolder}/ReadabilityStudioDocs/R"),
         glue("{docFolder}/readability-test-reference/R"),
         TRUE)
dir_copy(glue("{docFolder}/ReadabilityStudioDocs/data"),
         glue("{docFolder}/readability-test-reference/data"),
         TRUE)
file_copy(glue("{docFolder}/ReadabilityStudioDocs/overviews/01-intro.rmd"),
          glue("{docFolder}/readability-test-reference/01-intro.rmd"),
          TRUE)
file_copy(glue("{docFolder}/ReadabilityStudioDocs/93-Author.rmd"),
          glue("{docFolder}/readability-test-reference/93-Author.rmd"),
          TRUE)
file_copy(glue("{docFolder}/ReadabilityStudioDocs/modern-language-association.csl"),
          glue("{docFolder}/readability-test-reference/modern-language-association.csl"),
          TRUE)
file_copy(glue("{docFolder}/ReadabilityStudioDocs/cites.bib"),
          glue("{docFolder}/readability-test-reference/cites.bib"),
          TRUE)
combine_files("20-ReadabilityTestsEnglish.rmd", "english")
combine_files("21-ReadabilityTestsSpanish.rmd", "spanish")
combine_files("22-ReadabilityTestsGerman.rmd", "german")
combine_files("41-ScoringNotes.rmd", "scoring-notes",
              "(intro|grade[-]level[-]results|cloze)")
combine_files("90-Acknowledgements.Rmd", "acknowledgements",
              ("(intro)"))

bookdown::render_book(input="index.Rmd",
                      output_format="bookdown::pdf_book",
                      output_dir="docs")
unlink(glue("{docFolder}/readability-test-reference/01-intro.Rmd"))
unlink(glue("{docFolder}/readability-test-reference/20-ReadabilityTestsEnglish.rmd"))
unlink(glue("{docFolder}/readability-test-reference/21-ReadabilityTestsSpanish.rmd"))
unlink(glue("{docFolder}/readability-test-reference/22-ReadabilityTestsGerman.rmd"))
unlink(glue("{docFolder}/readability-test-reference/41-ScoringNotes.rmd"))
unlink(glue("{docFolder}/readability-test-reference/90-Acknowledgements.Rmd"))
unlink(glue("{docFolder}/readability-test-reference/93-Author.Rmd"))
unlink(glue("{docFolder}/readability-test-reference/cites.bib"))
unlink(glue("{docFolder}/readability-test-reference/modern-language-association.csl"))

# final cleanup
clearFolders()
