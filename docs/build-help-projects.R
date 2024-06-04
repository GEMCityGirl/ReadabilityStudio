if (!require("pacman")) install.packages("pacman")
library(pacman)
pacman::p_load(bookdown, glue, readr, stringr, fs, lubridate, tidyverse, kableExtra,
               Hmisc, cowplot, beeswarm, tinytex, stringi, this.path, janitor, quarto)

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
source(glue("{docFolder}/readability-studio-manual/R/appdown.r"))

# delete previous builds
unlink(glue("{docFolder}/coding-bible/docs"), recursive=T)
unlink(glue("{docFolder}/shortcuts-cheatsheet/docs"), recursive=T)
unlink(glue("{docFolder}/readability-studio-manual/docs"), recursive=T)
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

  unlink(glue("{docFolder}/readability-studio-manual/_bookdown_files"), recursive=T)
  unlink(glue("{docFolder}/readability-studio-manual/readability-studio-manual.aux"))
  unlink(glue("{docFolder}/readability-studio-manual/readability-studio-manual.log"))
  unlink(glue("{docFolder}/readability-studio-manual/readability-studio-manual.toc"))
  unlink(glue("{docFolder}/readability-studio-manual/readability-studio-manual.tex"))
  unlink(glue("{docFolder}/readability-studio-manual/readability-studio-manual.bcf"))
  unlink(glue("{docFolder}/readability-studio-manual/readability-studio-manual.log"))
  unlink(glue("{docFolder}/readability-studio-manual/readability-studio-manual.idx"))
  unlink(glue("{docFolder}/readability-studio-manual/readability-studio-manual.lof"))
  unlink(glue("{docFolder}/readability-studio-manual/readability-studio-manual.lot"))
  unlink(glue("{docFolder}/readability-studio-manual/readability-studio-manual.bbl"))
  unlink(glue("{docFolder}/readability-studio-manual/readability-studio-manual.blg"))
  unlink(glue("{docFolder}/readability-studio-manual/readability-studio-manual.ilg"))
  unlink(glue("{docFolder}/readability-studio-manual/readability-studio-manual.ind"))
  unlink(glue("{docFolder}/readability-studio-manual/readability-studio-manual.run.xml"))
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

# When building a single *.qmd file as a PDF, embed this to the top of it
# so that inline R code will evaluate.
knitrQuartoSingleDocHeader <- "
```{r}
#| eval: true
#| include: false

library(glue)
source(glue('{getwd()}/../readability-studio-manual/R/appdown.r'))
UserManualMode <- FALSE
```\n\n"

# Coding Bible
##############

setwd(glue("{docFolder}/coding-bible/"))
bookdown::render_book(input="index.Rmd",
                      output_dir="docs")

# Release Notes
###############

# Note that this book has its own LaTeX files (i.e., does not copy them from readability-studio-manual).
# This "book" doesn't have any front or back matter, so its preamble TeX file doesn't include that.
setwd(glue("{docFolder}/release-notes/"))
file_copy(glue("{docFolder}/readability-studio-manual/overviews/03-new-features.rmd"),
          glue("{docFolder}/release-notes/index.qmd"),
          TRUE)
fileContents <- read_lines(glue("{docFolder}/release-notes/index.qmd"))
cat(knitrQuartoSingleDocHeader, file = glue("{docFolder}/release-notes/index.qmd"))
write_lines(fileContents, file = glue("{docFolder}/release-notes/index.qmd"), append = TRUE)
quarto::quarto_render(output_format="pdf", as_job=F)
unlink(glue("{docFolder}/release-notes/index.qmd"))

# Shortcuts Cheatsheet
######################

setwd(glue("{docFolder}/shortcuts-cheatsheet/"))
file_copy(glue("{docFolder}/readability-studio-manual/glossary/02-shortcuts.rmd"),
          glue("{docFolder}/shortcuts-cheatsheet/index.qmd"),
          TRUE)
fileContents <- read_lines(glue("{docFolder}/shortcuts-cheatsheet/index.qmd"))
cat(knitrQuartoSingleDocHeader, file = glue("{docFolder}/shortcuts-cheatsheet/index.qmd"))
write_lines(fileContents, file = glue("{docFolder}/shortcuts-cheatsheet/index.qmd"), append = TRUE)
quarto::quarto_render(output_format="pdf", as_job=F)
unlink(glue("{docFolder}/shortcuts-cheatsheet/index.qmd"))

# User Manual
#############

UserManualMode = TRUE
IncludeLinux = FALSE
setwd(glue("{docFolder}/readability-studio-manual/"))
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
unlink(glue("{docFolder}/readability-studio-manual/01-Overviews.Rmd"))
unlink(glue("{docFolder}/readability-studio-manual/20-ReadabilityTestsEnglish.rmd"))
unlink(glue("{docFolder}/readability-studio-manual/21-ReadabilityTestsSpanish.rmd"))
unlink(glue("{docFolder}/readability-studio-manual/22-ReadabilityTestsGerman.rmd"))
unlink(glue("{docFolder}/readability-studio-manual/41-ScoringNotes.rmd"))
unlink(glue("{docFolder}/readability-studio-manual/90-Acknowledgements.Rmd"))
unlink(glue("{docFolder}/readability-studio-manual/91-Glossaries.rmd"))

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
file_copy(glue("{docFolder}/readability-studio-manual/images/non-generated/CC_BY-NC-ND.png"),
          glue("{docFolder}/readability-studio-api/images/CC_BY-NC-ND.png"),
          TRUE)
file_copy(glue("{docFolder}/readability-studio-manual/images/non-generated/cover-programming.pdf"),
          glue("{docFolder}/readability-studio-api/images/cover.pdf"),
          TRUE)
file_copy(glue("{docFolder}/readability-studio-manual/images/non-generated/cover-programming.png"),
          glue("{docFolder}/readability-studio-api/images/cover.png"),
          TRUE)
file_copy(glue("{docFolder}/readability-studio-manual/modern-language-association.csl"),
          glue("{docFolder}/readability-studio-api/modern-language-association.csl"),
          TRUE)
file_copy(glue("{docFolder}/_variables.yml"),
          glue("{docFolder}/readability-studio-api/_variables.yml"),
          TRUE)
dir_copy(glue("{docFolder}/readability-studio-manual/latex"),
         glue("{docFolder}/readability-studio-api/latex"),
         TRUE)
dir_copy(glue("{docFolder}/readability-studio-manual/css"),
         glue("{docFolder}/readability-studio-api/css"),
         TRUE)
dir_copy(glue("{docFolder}/readability-studio-manual/R"),
         glue("{docFolder}/readability-studio-api/R"),
         TRUE)

quarto::quarto_render(output_format="pdf", as_job=F)
unlink(glue("{docFolder}/readability-studio-api/_variables.yml"))

# Tests Reference Manual
########################

UserManualMode = F
setwd(glue("{docFolder}/readability-test-reference/"))
dir_create(glue("{docFolder}/readability-test-reference/images"))
file_copy(glue("{docFolder}/readability-studio-manual/images/non-generated/CC_BY-NC-ND.png"),
          glue("{docFolder}/readability-test-reference/images/CC_BY-NC-ND.png"),
          TRUE)
file_copy(glue("{docFolder}/readability-studio-manual/images/non-generated/cover-tests.pdf"),
          glue("{docFolder}/readability-test-reference/images/cover.pdf"),
          TRUE)
file_copy(glue("{docFolder}/readability-studio-manual/images/non-generated/cover-tests.png"),
          glue("{docFolder}/readability-test-reference/images/cover.png"),
          TRUE)
file_copy(glue("{docFolder}/readability-studio-manual/images/FleschChart.png"),
          glue("{docFolder}/readability-test-reference/images/FleschChart.png"),
          TRUE)
file_copy(glue("{docFolder}/readability-studio-manual/images/frygraph.png"),
          glue("{docFolder}/readability-test-reference/images/frygraph.png"),
          TRUE)
file_copy(glue("{docFolder}/readability-studio-manual/images/LixGauge.png"),
          glue("{docFolder}/readability-test-reference/images/LixGauge.png"),
          TRUE)
file_copy(glue("{docFolder}/readability-studio-manual/images/raygorgraph.png"),
          glue("{docFolder}/readability-test-reference/images/raygorgraph.png"),
          TRUE)
file_copy(glue("{docFolder}/readability-studio-manual/images/CrawfordGraph.png"),
          glue("{docFolder}/readability-test-reference/images/CrawfordGraph.png"),
          TRUE)
file_copy(glue("{docFolder}/readability-studio-manual/images/FraseGraph.png"),
          glue("{docFolder}/readability-test-reference/images/FraseGraph.png"),
          TRUE)
file_copy(glue("{docFolder}/readability-studio-manual/images/GpmFry.png"),
          glue("{docFolder}/readability-test-reference/images/GpmFry.png"),
          TRUE)
file_copy(glue("{docFolder}/readability-studio-manual/images/GermanLixGauge.png"),
          glue("{docFolder}/readability-test-reference/images/GermanLixGauge.png"),
          TRUE)
file_copy(glue("{docFolder}/readability-studio-manual/images/Schwartz.png"),
          glue("{docFolder}/readability-test-reference/images/Schwartz.png"),
          TRUE)
dir_copy(glue("{docFolder}/readability-studio-manual/english"),
         glue("{docFolder}/readability-test-reference/english"),
         TRUE)
dir_copy(glue("{docFolder}/readability-studio-manual/spanish"),
         glue("{docFolder}/readability-test-reference/spanish"),
         TRUE)
dir_copy(glue("{docFolder}/readability-studio-manual/german"),
         glue("{docFolder}/readability-test-reference/german"),
         TRUE)
dir_copy(glue("{docFolder}/readability-studio-manual/scoring-notes"),
         glue("{docFolder}/readability-test-reference/scoring-notes"),
         TRUE)
dir_copy(glue("{docFolder}/readability-studio-manual/acknowledgements"),
         glue("{docFolder}/readability-test-reference/acknowledgements"),
         TRUE)
dir_copy(glue("{docFolder}/readability-studio-manual/latex"),
         glue("{docFolder}/readability-test-reference/latex"),
         TRUE)
dir_copy(glue("{docFolder}/readability-studio-manual/css"),
         glue("{docFolder}/readability-test-reference/css"),
         TRUE)
dir_copy(glue("{docFolder}/readability-studio-manual/R"),
         glue("{docFolder}/readability-test-reference/R"),
         TRUE)
dir_copy(glue("{docFolder}/readability-studio-manual/data"),
         glue("{docFolder}/readability-test-reference/data"),
         TRUE)
file_copy(glue("{docFolder}/readability-studio-manual/overviews/01-intro.rmd"),
          glue("{docFolder}/readability-test-reference/01-intro.rmd"),
          TRUE)
file_copy(glue("{docFolder}/readability-studio-manual/93-Author.rmd"),
          glue("{docFolder}/readability-test-reference/93-Author.rmd"),
          TRUE)
file_copy(glue("{docFolder}/readability-studio-manual/modern-language-association.csl"),
          glue("{docFolder}/readability-test-reference/modern-language-association.csl"),
          TRUE)
file_copy(glue("{docFolder}/readability-studio-manual/cites.bib"),
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
