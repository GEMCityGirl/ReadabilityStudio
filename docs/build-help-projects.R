if (!require("pacman")) install.packages("pacman")
library(pacman)
pacman::p_load(glue, readr, stringr, fs, lubridate, tidyverse, kableExtra,
               Hmisc, cowplot, beeswarm, tinytex, stringi, this.path, janitor, quarto,
               magick, pdftools)

if (nchar(tinytex::tinytex_root()) == 0)
  {
  tinytex::install_tinytex()
  }
# call tinytex::reinstall_tinytex() if you get a "tlmgr: Remote repository is newer than local..." error message

thumbnailWidth <- "256"

docFolder <- this.path::this.dir()
source(glue("{docFolder}/build-enum-files.R"))
source(glue("{docFolder}/readability-studio-manual/R/appdown.r"))

# delete previous builds
unlink(glue("{docFolder}/coding-bible/docs"), recursive=T)
unlink(glue("{docFolder}/shortcuts-cheatsheet/docs"), recursive=T)
unlink(glue("{docFolder}/readability-studio-manual/docs-manual"), recursive=T)
unlink(glue("{docFolder}/readability-test-reference/docs-online"), recursive=T)
unlink(glue("{docFolder}/readability-studio-api/docs"), recursive=T)
unlink(glue("{docFolder}/release-notes/docs"), recursive=T)

# delete helper files copied from the main project into others
clearFolders <- function()
  {
  unlink(glue("{docFolder}/R"), recursive=T)

  unlink(glue("{docFolder}/shortcuts-cheatsheet/R"), recursive=T)

  unlink(glue("{docFolder}/release-notes/images"), recursive=T)
  unlink(glue("{docFolder}/release-notes/_extensions"), recursive=T)

  unlink(glue("{docFolder}/sysadmin/images"), recursive=T)
  unlink(glue("{docFolder}/sysadmin/latex"), recursive=T)
  unlink(glue("{docFolder}/sysadmin/R"), recursive=T)
  unlink(glue("{docFolder}/sysadmin/_extensions"), recursive=T)

  unlink(glue("{docFolder}/readability-studio-api/images"), recursive=T)
  unlink(glue("{docFolder}/readability-studio-api/latex"), recursive=T)
  unlink(glue("{docFolder}/readability-studio-api/css"), recursive=T)
  unlink(glue("{docFolder}/readability-studio-api/R"), recursive=T)
  unlink(glue("{docFolder}/readability-studio-api/_extensions"), recursive=T)
  unlink(glue("{docFolder}/readability-studio-api/index.ptc"))

  unlink(glue("{docFolder}/readability-test-reference/images"), recursive=T)
  unlink(glue("{docFolder}/readability-test-reference/latex"), recursive=T)
  unlink(glue("{docFolder}/readability-test-reference/css"), recursive=T)
  unlink(glue("{docFolder}/readability-test-reference/R"), recursive=T)
  unlink(glue("{docFolder}/readability-test-reference/_extensions"), recursive=T)
  unlink(glue("{docFolder}/readability-test-reference/data"), recursive=T)
  unlink(glue("{docFolder}/readability-test-reference/overviews"), recursive=T)
  unlink(glue("{docFolder}/readability-test-reference/scoring-notes"), recursive=T)
  unlink(glue("{docFolder}/readability-test-reference/acknowledgements"), recursive=T)
  unlink(glue("{docFolder}/readability-test-reference/english"), recursive=T)
  unlink(glue("{docFolder}/readability-test-reference/spanish"), recursive=T)
  unlink(glue("{docFolder}/readability-test-reference/german"), recursive=T)
  unlink(glue("{docFolder}/readability-test-reference/index.ptc"))

  unlink(glue("{docFolder}/readability-studio-manual/_bookdown_files"), recursive=T)
  unlink(glue("{docFolder}/readability-studio-manual/docs-manual/glossary"), recursive=T)
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
  unlink(glue("{docFolder}/readability-studio-manual/index.ptc"))
  }

clearFolders()

# copy R script to parent so that single-chapter files can reference it
dir_copy(glue("{docFolder}/readability-studio-manual/R"),
         glue("{docFolder}/R"),
         TRUE)

# System Admin Manual
#####################

setwd(glue("{docFolder}/sysadmin/"))
file_copy(glue("{docFolder}/readability-studio-manual/_quarto.yml"),
          glue("{docFolder}/sysadmin/_quarto.yml"),
          TRUE)
file_copy(glue("{docFolder}/readability-studio-manual/_quarto-sysadmin.yml"),
          glue("{docFolder}/sysadmin/_quarto-sysadmin.yml"),
          TRUE)
file_copy(glue("{docFolder}/readability-studio-manual/modern-language-association.csl"),
          glue("{docFolder}/sysadmin/modern-language-association.csl"),
          TRUE)
file_copy(glue("{docFolder}/readability-studio-manual/LegrandOrangeBook.cls"),
          glue("{docFolder}/sysadmin/LegrandOrangeBook.cls"),
          TRUE)
file_copy(glue("{docFolder}/_variables.yml"),
          glue("{docFolder}/sysadmin/_variables.yml"),
          TRUE)
dir_copy(glue("{docFolder}/readability-studio-manual/latex"),
         glue("{docFolder}/sysadmin/latex"),
         TRUE)
dir_copy(glue("{docFolder}/readability-studio-manual/images"),
         glue("{docFolder}/sysadmin/images"),
         TRUE)
dir_copy(glue("{docFolder}/readability-studio-manual/R"),
         glue("{docFolder}/sysadmin/R"),
         TRUE)
dir_copy(glue("{docFolder}/readability-studio-manual/_extensions"),
         glue("{docFolder}/sysadmin/_extensions"),
         TRUE)

quarto::quarto_render(output_format="pdf", as_job=F, profile="sysadmin")
# create a thumbnail for the website
img <- image_convert(image_read_pdf(glue("{docFolder}/sysadmin/docs/sysadmin-manual.pdf"), 1),
                     format="jpeg")
img <- image_scale(img, thumbnailWidth)
image_write(img, glue("{docFolder}/sysadmin/docs/sysadmin-manual-thumb.jpg"))

unlink(glue("{docFolder}/sysadmin/modern-language-association.csl"))
unlink(glue("{docFolder}/sysadmin/_variables.yml"))
unlink(glue("{docFolder}/sysadmin/_quarto.yml"))
unlink(glue("{docFolder}/sysadmin/_quarto-sysadmin.yml"))
unlink(glue("{docFolder}/sysadmin/LegrandOrangeBook.cls"))

# Release Notes
###############

# Note that this book has its own LaTeX files (i.e., does not copy them from readability-studio-manual).
# This "book" doesn't have any front or back matter, so its preamble TeX file doesn't include that.
setwd(glue("{docFolder}/release-notes/"))
file_copy(glue("{docFolder}/_variables.yml"),
          glue("{docFolder}/release-notes/_variables.yml"),
          TRUE)
file_copy(glue("{docFolder}/readability-studio-manual/_quarto-release-notes.yml"),
          glue("{docFolder}/release-notes/_quarto.yml"),
          TRUE)
file_copy(glue("{docFolder}/readability-studio-manual/overviews/new-features.qmd"),
          glue("{docFolder}/release-notes/index.qmd"),
          TRUE)
dir_copy(glue("{docFolder}/readability-studio-manual/images"),
         glue("{docFolder}/release-notes/images"),
         TRUE)
dir_copy(glue("{docFolder}/readability-studio-manual/_extensions"),
         glue("{docFolder}/release-notes/_extensions"),
         TRUE)

quarto::quarto_render(output_format="pdf", as_job=F)
# create a thumbnail for the website
img <- image_convert(image_read_pdf(glue("{docFolder}/release-notes/docs/release-notes.pdf"), 1),
                     format="jpeg")
img <- image_scale(img, thumbnailWidth)
image_write(img, glue("{docFolder}/release-notes/docs/release-notes-thumb.jpg"))

unlink(glue("{docFolder}/release-notes/_quarto.yml"))
unlink(glue("{docFolder}/release-notes/_variables.yml"))
unlink(glue("{docFolder}/release-notes/index.qmd"))

# Shortcuts Cheatsheet
######################

setwd(glue("{docFolder}/shortcuts-cheatsheet/"))
file_copy(glue("{docFolder}/readability-studio-manual/glossary/shortcuts.qmd"),
          glue("{docFolder}/shortcuts-cheatsheet/index.qmd"),
          TRUE)
file_copy(glue("{docFolder}/readability-studio-manual/_quarto-shortcuts-cheatsheet.yml"),
          glue("{docFolder}/shortcuts-cheatsheet/_quarto.yml"),
          TRUE)

readLines(glue("{docFolder}/shortcuts-cheatsheet/index.qmd")) |>
  str_replace("# Keyboard Shortcuts", "# *Readability Studio* Keyboard Shortcuts") |>
  writeLines(glue("{docFolder}/shortcuts-cheatsheet/index.qmd"))

quarto::quarto_render(output_format="pdf", as_job=F)
# create a thumbnail for the website
img <- image_convert(image_read_pdf(glue("{docFolder}/shortcuts-cheatsheet/docs/shortcuts-cheatsheet.pdf"), 1),
                     format="jpeg")
img <- image_scale(img, thumbnailWidth)
image_write(img, glue("{docFolder}/shortcuts-cheatsheet/docs/shortcuts-cheatsheet-thumb.jpg"))

unlink(glue("{docFolder}/shortcuts-cheatsheet/_quarto.yml"))
unlink(glue("{docFolder}/shortcuts-cheatsheet/index.qmd"))

# User Manual
#############

setwd(glue("{docFolder}/readability-studio-manual/"))
file_copy(glue("{docFolder}/_variables.yml"),
          glue("{docFolder}/readability-studio-manual/_variables.yml"),
          TRUE)
combine_files("readability-tests-english.qmd", "english", addendum = "{{< elevator 'Back to top' >}}")
combine_files("readability-tests-spanish.qmd", "spanish", addendum = "{{< elevator 'Back to top' >}}")
combine_files("readability-tests-german.qmd", "german", addendum = "{{< elevator 'Back to top' >}}")
combine_files("scoring-notes.qmd", "scoring-notes", addendum = "{{< elevator 'Back to top' >}}")

quarto::quarto_render(output_format="pdf", as_job=F, profile="manual")
quarto::quarto_render(output_format="html", as_job=F, profile="online")
img <- image_convert(image_read_pdf(glue("{docFolder}/readability-studio-manual/docs-manual/readability-studio-manual.pdf"), 1),
                     format="jpeg")
img <- image_scale(img, thumbnailWidth)
image_write(img, glue("{docFolder}/readability-studio-manual/docs-manual/readability-studio-manual-thumb.jpg"))

unlink(glue("{docFolder}/readability-studio-manual/readability-tests-english.qmd"))
unlink(glue("{docFolder}/readability-studio-manual/readability-tests-spanish.qmd"))
unlink(glue("{docFolder}/readability-studio-manual/readability-tests-german.qmd"))
unlink(glue("{docFolder}/readability-studio-manual/scoring-notes.qmd"))
unlink(glue("{docFolder}/readability-studio-manual/ding.mp3"))
unlink(glue("{docFolder}/readability-studio-manual/_variables.yml"))

# Programming Manual
####################

# build the API help topics and code completion file
unlink(glue("{docFolder}/readability-studio-api/enums"), recursive=T) # rebuild this folder and keep the latest content
dir_create(glue("{docFolder}/readability-studio-api/enums"))

# build resource files for editor
appLibStr <- paste0('Application', '\t', str_flatten(loadClassInfo(normalizePath(glue("{docFolder}/../src/lua-scripting/lua_application.h"))), ';'))
debugLibStr <- paste0('Debug', '\t', str_flatten(loadClassInfo(normalizePath(glue("{docFolder}/../src/lua-scripting/lua_debug.h"))), ';'))
readr::write_file(paste('DO NOT EDIT THIS FILE, IT IS GENERATED FROM A BUILD SCRIPT!', appLibStr, debugLibStr, sep='\n'), normalizePath(glue("{docFolder}/../resources/scripting/rs-libraries.api")))

standardProjectClassStr <- paste0('StandardProject', '\t', str_flatten(loadClassInfo(normalizePath(glue("{docFolder}/../src/lua-scripting/lua_standard_project.h"))), ';'))
batchProjectClassStr <- paste0('BatchProject', '\t', str_flatten(loadClassInfo(normalizePath(glue("{docFolder}/../src/lua-scripting/lua_batch_project.h"))), ';'))
readr::write_file(paste('DO NOT EDIT THIS FILE, IT IS GENERATED FROM A BUILD SCRIPT!', standardProjectClassStr, batchProjectClassStr, sep='\n'), normalizePath(glue("{docFolder}/../resources/scripting/rs-classes.api")))

# Build API topics (only ones that don't already exist).
# A message will appear in the console window listing any topics that are created;
# these can be edited afterwards (e.g., adding examples).
writeClassTopics(loadClassInfo(normalizePath(glue("{docFolder}/../src/lua-scripting/lua_debug.h")), TRUE),
                 glue("{docFolder}/readability-studio-api/libraries/Debug"))
writeClassTopics(loadClassInfo(normalizePath(glue("{docFolder}/../src/lua-scripting/lua_application.h")), TRUE),
                 glue("{docFolder}/readability-studio-api/libraries/Application"))
writeClassTopics(loadClassInfo(normalizePath(glue("{docFolder}/../src/lua-scripting/lua_standard_project.h")), TRUE),
                 glue("{docFolder}/readability-studio-api/classes/StandardProject"))
writeClassTopics(loadClassInfo(normalizePath(glue("{docFolder}/../src/lua-scripting/lua_batch_project.h")), TRUE),
                 glue("{docFolder}/readability-studio-api/classes/BatchProject"))

# build enum topics and resource file for editor
enums <- loadEnums(normalizePath(glue("{docFolder}/../resources/scripting/rs-constants.lua")))
writeEnumEditorFile(enums, glue("{docFolder}/../resources/scripting/rs-enums.api"))
writeEnumTopics(enums, glue("{docFolder}/readability-studio-api/enums/"))

setwd(glue("{docFolder}/readability-studio-api/"))
dir_copy(glue("{docFolder}/readability-studio-manual/images"),
         glue("{docFolder}/readability-studio-api/images"),
         TRUE)
file_copy(glue("{docFolder}/readability-studio-manual/modern-language-association.csl"),
          glue("{docFolder}/readability-studio-api/modern-language-association.csl"),
          TRUE)
file_copy(glue("{docFolder}/readability-studio-manual/LegrandOrangeBook.cls"),
          glue("{docFolder}/readability-studio-api/LegrandOrangeBook.cls"),
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
dir_copy(glue("{docFolder}/readability-studio-manual/_extensions"),
         glue("{docFolder}/readability-studio-api/_extensions"),
         TRUE)

combine_files("standard-project.qmd", "classes/StandardProject")
combine_files("batch-project.qmd", "classes/BatchProject")
combine_files("libapp.qmd", "libraries/Application")
combine_files("libdebug.qmd", "libraries/Debug")
combine_files("enums.qmd", "enums")

quarto::quarto_render(output_format="pdf", as_job=F)
img <- image_convert(image_read_pdf(glue("{docFolder}/readability-studio-api/docs/readability-studio-api.pdf"), 1),
                     format="jpeg")
img <- image_scale(img, thumbnailWidth)
image_write(img, glue("{docFolder}/readability-studio-api/docs/rreadability-studio-api-thumb.jpg"))

unlink(glue("{docFolder}/readability-studio-api/standard-project.qmd"))
unlink(glue("{docFolder}/readability-studio-api/batch-project.qmd"))
unlink(glue("{docFolder}/readability-studio-api/libapp.qmd"))
unlink(glue("{docFolder}/readability-studio-api/libdebug.qmd"))
unlink(glue("{docFolder}/readability-studio-api/enums.qmd"))
unlink(glue("{docFolder}/readability-studio-api/_variables.yml"))
unlink(glue("{docFolder}/readability-studio-api/modern-language-association.csl"))
unlink(glue("{docFolder}/readability-studio-api/LegrandOrangeBook.cls"))

# Tests Reference Manual
########################

setwd(glue("{docFolder}/readability-test-reference/"))
file_copy(glue("{docFolder}/_variables.yml"),
          glue("{docFolder}/readability-test-reference/_variables.yml"),
          TRUE)
dir_copy(glue("{docFolder}/readability-studio-manual/images"),
         glue("{docFolder}/readability-test-reference/images"),
         TRUE)
file_copy(glue("{docFolder}/readability-studio-manual/_quarto.yml"),
          glue("{docFolder}/readability-test-reference/_quarto.yml"),
          TRUE)
file_copy(glue("{docFolder}/readability-studio-manual/_quarto-reference.yml"),
          glue("{docFolder}/readability-test-reference/_quarto-reference.yml"),
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
dir_copy(glue("{docFolder}/readability-studio-manual/overviews"),
         glue("{docFolder}/readability-test-reference/overviews"),
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
dir_copy(glue("{docFolder}/readability-studio-manual/_extensions"),
         glue("{docFolder}/readability-test-reference/_extensions"),
         TRUE)
dir_copy(glue("{docFolder}/readability-studio-manual/data"),
         glue("{docFolder}/readability-test-reference/data"),
         TRUE)
file_copy(glue("{docFolder}/readability-studio-manual/author.qmd"),
          glue("{docFolder}/readability-test-reference/author.qmd"),
          TRUE)
file_copy(glue("{docFolder}/readability-studio-manual/modern-language-association.csl"),
          glue("{docFolder}/readability-test-reference/modern-language-association.csl"),
          TRUE)
file_copy(glue("{docFolder}/readability-studio-manual/LegrandOrangeBook.cls"),
          glue("{docFolder}/readability-test-reference/LegrandOrangeBook.cls"),
          TRUE)
file_copy(glue("{docFolder}/readability-studio-manual/cites.bib"),
          glue("{docFolder}/readability-test-reference/cites.bib"),
          TRUE)
combine_files("readability-tests-english.qmd", "english")
combine_files("readability-tests-spanish.qmd", "spanish")
combine_files("readability-tests-german.qmd", "german")
combine_files("scoring-notes.qmd", "scoring-notes",
              "(intro|grade[-]level[-]results|cloze)")

quarto::quarto_render(output_format="pdf", as_job=F, profile="reference")
# create a thumbnail for the website
img <- image_convert(image_read_pdf(glue("{docFolder}/readability-test-reference/docs/readability-test-reference.pdf"), 1),
                     format="jpeg")
img <- image_scale(img, thumbnailWidth)
image_write(img, glue("{docFolder}/readability-test-reference/docs/readability-test-reference-thumb.jpg"))

unlink(glue("{docFolder}/readability-test-reference/_variables.yml"))
unlink(glue("{docFolder}/readability-test-reference/readability-tests-english.qmd"))
unlink(glue("{docFolder}/readability-test-reference/_quarto.yml"))
unlink(glue("{docFolder}/readability-test-reference/_quarto-reference.yml"))
unlink(glue("{docFolder}/readability-test-reference/readability-tests-spanish.qmd"))
unlink(glue("{docFolder}/readability-test-reference/readability-tests-german.qmd"))
unlink(glue("{docFolder}/readability-test-reference/scoring-notes.qmd"))
unlink(glue("{docFolder}/readability-test-reference/author.qmd"))
unlink(glue("{docFolder}/readability-test-reference/cites.bib"))
unlink(glue("{docFolder}/readability-test-reference/modern-language-association.csl"))
unlink(glue("{docFolder}/readability-test-reference/LegrandOrangeBook.cls"))

# final cleanup
clearFolders()
