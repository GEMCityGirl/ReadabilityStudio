if (!require("pacman")) install.packages("pacman")
library(pacman)
pacman::p_load(bookdown, glue, readr, stringr, fs, lubridate, tidyverse, kableExtra,
               Hmisc, cowplot, beeswarm, tinytex, this.path)

if (nchar(tinytex::tinytex_root()) == 0)
  {
  tinytex::install_tinytex()
  }

docFolder <- this.path::this.dir()
source(glue("{docFolder}/ReadabilityStudioDocs/R/appdown.r"))

# delete previous builds
unlink(glue("{docFolder}/Coding-Bible/docs"), recursive=T)
unlink(glue("{docFolder}/ShortcutsCheatsheet/docs"), recursive=T)
unlink(glue("{docFolder}/ReadabilityStudioDocs/docs"), recursive=T)
unlink(glue("{docFolder}/ReadabilityTestsReference/docs"), recursive=T)
unlink(glue("{docFolder}/ReadabilityStudioAPI/docs"), recursive=T)

# delete helper files copied from the main project into others
clearFolders <- function()
  {
  unlink(glue("{docFolder}/ReadabilityStudioAPI/images"), recursive=T)
  unlink(glue("{docFolder}/ReadabilityStudioAPI/latex"), recursive=T)
  unlink(glue("{docFolder}/ReadabilityStudioAPI/css"), recursive=T)
  unlink(glue("{docFolder}/ReadabilityStudioAPI/R"), recursive=T)

  unlink(glue("{docFolder}/ReadabilityTestsReference/images"), recursive=T)
  unlink(glue("{docFolder}/ReadabilityTestsReference/latex"), recursive=T)
  unlink(glue("{docFolder}/ReadabilityTestsReference/css"), recursive=T)
  unlink(glue("{docFolder}/ReadabilityTestsReference/R"), recursive=T)
  unlink(glue("{docFolder}/ReadabilityTestsReference/data"), recursive=T)
  unlink(glue("{docFolder}/ReadabilityTestsReference/scoring-notes"), recursive=T)
  unlink(glue("{docFolder}/ReadabilityTestsReference/acknowledgements"), recursive=T)
  unlink(glue("{docFolder}/ReadabilityTestsReference/english"), recursive=T)
  unlink(glue("{docFolder}/ReadabilityTestsReference/spanish"), recursive=T)
  unlink(glue("{docFolder}/ReadabilityTestsReference/german"), recursive=T)

  unlink(glue("{docFolder}/ReadabilityStudioDocs/_bookdown_files"), recursive=T)
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

setwd(glue("{docFolder}/Coding-Bible/"))
bookdown::render_book(input="index.Rmd",
                      output_dir="docs")

# Shortcuts Cheatsheet
######################

# Note that this book has its own LaTeX files (i.e., does not copy them from ReadabilityStudioDocs).
# This "book" doesn't have any front or back matter, so its preamble TeX file doesn't include that.
setwd(glue("{docFolder}/ShortcutsCheatsheet/"))
file_copy(glue("{docFolder}/ReadabilityStudioDocs/glossary/02-shortcuts.rmd"),
          glue("{docFolder}/ShortcutsCheatsheet/02-shortcuts.rmd"),
          TRUE)
bookdown::render_book(input="index.Rmd",
                      output_format="bookdown::pdf_book",
                      output_dir="docs")
unlink(glue("{docFolder}/ShortcutsCheatsheet/02-shortcuts.rmd"))

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

setwd(glue("{docFolder}/ReadabilityStudioAPI/"))
dir_create(glue("{docFolder}/ReadabilityStudioAPI/images"))
file_copy(glue("{docFolder}/ReadabilityStudioDocs/images/NonGenerated/CC_BY-NC-ND.png"),
          glue("{docFolder}/ReadabilityStudioAPI/images/CC_BY-NC-ND.png"),
          TRUE)
file_copy(glue("{docFolder}/ReadabilityStudioDocs/images/NonGenerated/cover-programming.pdf"),
          glue("{docFolder}/ReadabilityStudioAPI/images/cover.pdf"),
          TRUE)
file_copy(glue("{docFolder}/ReadabilityStudioDocs/images/NonGenerated/cover-programming.png"),
          glue("{docFolder}/ReadabilityStudioAPI/images/cover.png"),
          TRUE)
file_copy(glue("{docFolder}/ReadabilityStudioDocs/modern-language-association.csl"),
          glue("{docFolder}/ReadabilityStudioAPI/modern-language-association.csl"),
          TRUE)
dir_copy(glue("{docFolder}/ReadabilityStudioDocs/latex"),
         glue("{docFolder}/ReadabilityStudioAPI/latex"),
         TRUE)
dir_copy(glue("{docFolder}/ReadabilityStudioDocs/css"),
         glue("{docFolder}/ReadabilityStudioAPI/css"),
         TRUE)
dir_copy(glue("{docFolder}/ReadabilityStudioDocs/R"),
         glue("{docFolder}/ReadabilityStudioAPI/R"),
         TRUE)

combine_files("10-Classes.Rmd", "classes")
combine_files("20-Libraries.Rmd", "libraries")
combine_files("30-Enums.Rmd", "enums")
bookdown::render_book(input="index.Rmd",
                      output_format="bookdown::gitbook",
                      output_dir="docs")
bookdown::render_book(input="index.Rmd",
                      output_format="bookdown::pdf_book",
                      output_dir="docs")
write(c(" "), file=glue("{docFolder}/ReadabilityStudioAPI/docs/.nojekyll"))
unlink(glue("{docFolder}/ReadabilityStudioAPI/10-Classes.Rmd"))
unlink(glue("{docFolder}/ReadabilityStudioAPI/20-Libraries.Rmd"))
unlink(glue("{docFolder}/ReadabilityStudioAPI/30-Enums.Rmd"))

# Tests Reference Manual
####################

UserManualMode = F
setwd(glue("{docFolder}/ReadabilityTestsReference/"))
dir_create(glue("{docFolder}/ReadabilityTestsReference/images"))
file_copy(glue("{docFolder}/ReadabilityStudioDocs/images/NonGenerated/CC_BY-NC-ND.png"),
          glue("{docFolder}/ReadabilityTestsReference/images/CC_BY-NC-ND.png"),
          TRUE)
file_copy(glue("{docFolder}/ReadabilityStudioDocs/images/NonGenerated/cover-tests.pdf"),
          glue("{docFolder}/ReadabilityTestsReference/images/cover.pdf"),
          TRUE)
file_copy(glue("{docFolder}/ReadabilityStudioDocs/images/NonGenerated/cover-tests.png"),
          glue("{docFolder}/ReadabilityTestsReference/images/cover.png"),
          TRUE)
file_copy(glue("{docFolder}/ReadabilityStudioDocs/images/FleschChart.png"),
          glue("{docFolder}/ReadabilityTestsReference/images/FleschChart.png"),
          TRUE)
file_copy(glue("{docFolder}/ReadabilityStudioDocs/images/frygraph.png"),
          glue("{docFolder}/ReadabilityTestsReference/images/frygraph.png"),
          TRUE)
file_copy(glue("{docFolder}/ReadabilityStudioDocs/images/LixGauge.png"),
          glue("{docFolder}/ReadabilityTestsReference/images/LixGauge.png"),
          TRUE)
file_copy(glue("{docFolder}/ReadabilityStudioDocs/images/raygorgraph.png"),
          glue("{docFolder}/ReadabilityTestsReference/images/raygorgraph.png"),
          TRUE)
file_copy(glue("{docFolder}/ReadabilityStudioDocs/images/CrawfordGraph.png"),
          glue("{docFolder}/ReadabilityTestsReference/images/CrawfordGraph.png"),
          TRUE)
file_copy(glue("{docFolder}/ReadabilityStudioDocs/images/FraseGraph.png"),
          glue("{docFolder}/ReadabilityTestsReference/images/FraseGraph.png"),
          TRUE)
file_copy(glue("{docFolder}/ReadabilityStudioDocs/images/GpmFry.png"),
          glue("{docFolder}/ReadabilityTestsReference/images/GpmFry.png"),
          TRUE)
file_copy(glue("{docFolder}/ReadabilityStudioDocs/images/GermanLixGauge.png"),
          glue("{docFolder}/ReadabilityTestsReference/images/GermanLixGauge.png"),
          TRUE)
file_copy(glue("{docFolder}/ReadabilityStudioDocs/images/Schwartz.png"),
          glue("{docFolder}/ReadabilityTestsReference/images/Schwartz.png"),
          TRUE)
dir_copy(glue("{docFolder}/ReadabilityStudioDocs/english"),
         glue("{docFolder}/ReadabilityTestsReference/english"),
         TRUE)
dir_copy(glue("{docFolder}/ReadabilityStudioDocs/spanish"),
         glue("{docFolder}/ReadabilityTestsReference/spanish"),
         TRUE)
dir_copy(glue("{docFolder}/ReadabilityStudioDocs/german"),
         glue("{docFolder}/ReadabilityTestsReference/german"),
         TRUE)
dir_copy(glue("{docFolder}/ReadabilityStudioDocs/scoring-notes"),
         glue("{docFolder}/ReadabilityTestsReference/scoring-notes"),
         TRUE)
dir_copy(glue("{docFolder}/ReadabilityStudioDocs/acknowledgements"),
         glue("{docFolder}/ReadabilityTestsReference/acknowledgements"),
         TRUE)
dir_copy(glue("{docFolder}/ReadabilityStudioDocs/latex"),
         glue("{docFolder}/ReadabilityTestsReference/latex"),
         TRUE)
dir_copy(glue("{docFolder}/ReadabilityStudioDocs/css"),
         glue("{docFolder}/ReadabilityTestsReference/css"),
         TRUE)
dir_copy(glue("{docFolder}/ReadabilityStudioDocs/R"),
         glue("{docFolder}/ReadabilityTestsReference/R"),
         TRUE)
dir_copy(glue("{docFolder}/ReadabilityStudioDocs/data"),
         glue("{docFolder}/ReadabilityTestsReference/data"),
         TRUE)
file_copy(glue("{docFolder}/ReadabilityStudioDocs/overviews/01-intro.rmd"),
          glue("{docFolder}/ReadabilityTestsReference/01-intro.rmd"),
          TRUE)
file_copy(glue("{docFolder}/ReadabilityStudioDocs/93-Author.rmd"),
          glue("{docFolder}/ReadabilityTestsReference/93-Author.rmd"),
          TRUE)
file_copy(glue("{docFolder}/ReadabilityStudioDocs/modern-language-association.csl"),
          glue("{docFolder}/ReadabilityTestsReference/modern-language-association.csl"),
          TRUE)
file_copy(glue("{docFolder}/ReadabilityStudioDocs/cites.bib"),
          glue("{docFolder}/ReadabilityTestsReference/cites.bib"),
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
unlink(glue("{docFolder}/ReadabilityTestsReference/01-intro.Rmd"))
unlink(glue("{docFolder}/ReadabilityTestsReference/20-ReadabilityTestsEnglish.rmd"))
unlink(glue("{docFolder}/ReadabilityTestsReference/21-ReadabilityTestsSpanish.rmd"))
unlink(glue("{docFolder}/ReadabilityTestsReference/22-ReadabilityTestsGerman.rmd"))
unlink(glue("{docFolder}/ReadabilityTestsReference/41-ScoringNotes.rmd"))
unlink(glue("{docFolder}/ReadabilityTestsReference/90-Acknowledgements.Rmd"))
unlink(glue("{docFolder}/ReadabilityTestsReference/93-Author.Rmd"))
unlink(glue("{docFolder}/ReadabilityTestsReference/cites.bib"))
unlink(glue("{docFolder}/ReadabilityTestsReference/modern-language-association.csl"))

# final cleanup
clearFolders()
