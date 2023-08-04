library(bookdown)
library(glue)
library(readr)
library(stringr)
library(fs)

docFolder <- dirname(rstudioapi::getSourceEditorContext()$path)
source(glue("{docFolder}/ReadabilityStudioDocs/R/appdown.r"))

# delete previous builds and helper files copied from
# the main project into others
unlink(glue("{docFolder}/ReadabilityStudioAPI/docs"), recursive=T)
unlink(glue("{docFolder}/ReadabilityStudioAPI/images"), recursive=T)
unlink(glue("{docFolder}/ReadabilityStudioAPI/latex"), recursive=T)
unlink(glue("{docFolder}/ReadabilityStudioAPI/css"), recursive=T)
unlink(glue("{docFolder}/ReadabilityStudioAPI/R"), recursive=T)

unlink(glue("{docFolder}/ReadabilityTestsReference/docs"), recursive=T)
unlink(glue("{docFolder}/ReadabilityTestsReference/images"), recursive=T)
unlink(glue("{docFolder}/ReadabilityTestsReference/latex"), recursive=T)
unlink(glue("{docFolder}/ReadabilityTestsReference/css"), recursive=T)
unlink(glue("{docFolder}/ReadabilityTestsReference/R"), recursive=T)
unlink(glue("{docFolder}/ReadabilityTestsReference/data"), recursive=T)
unlink(glue("{docFolder}/ReadabilityTestsReference/scoring-notes"), recursive=T)
unlink(glue("{docFolder}/ReadabilityTestsReference/acknowledgements"), recursive=T)

unlink(glue("{docFolder}/ReadabilityStudioDocs/docs"), recursive=T)
unlink(glue("{docFolder}/ReadabilityStudioDocs/_bookdown_files"), recursive=T)
unlink(glue("{docFolder}/Coding-Bible/docs"), recursive=T)

# Coding Bible
##############

setwd(glue("{docFolder}/Coding-Bible/"))
bookdown::render_book(input="index.Rmd",
                      output_dir="docs")

# User Manual
#############

UserManualMode = TRUE
setwd(glue("{docFolder}/ReadabilityStudioDocs/"))
combine_files("01-Overviews.Rmd", "overviews",
              ("(intro|program)"))
combine_files("90-Glossaries.rmd", "glossary")
combine_files("92-Acknowledgements.Rmd", "acknowledgements",
              ("(intro|assistance)"))
combine_files("41-ScoringNotes.rmd", "scoring-notes")
bookdown::render_book(input="index.Rmd",
                      output_format="bookdown::pdf_book",
                      output_dir="docs")
combine_files("01-Overviews.Rmd", "overviews")
combine_files("92-Acknowledgements.Rmd", "acknowledgements")
bookdown::render_book(input="index.Rmd",
                      output_format="bookdown::gitbook",
                      output_dir="docs")
unlink(glue("{docFolder}/ReadabilityStudioDocs/01-Overviews.Rmd"))
unlink(glue("{docFolder}/ReadabilityStudioDocs/41-ScoringNotes.rmd"))
unlink(glue("{docFolder}/ReadabilityStudioDocs/92-Acknowledgements.Rmd"))
unlink(glue("{docFolder}/ReadabilityStudioDocs/90-Glossaries.rmd"))
# used on github to serve the book
write(c(" "), file=glue("{docFolder}/ReadabilityStudioDocs/docs/.nojekyll"))

# Programming Manual
####################

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
setwd(glue("{docFolder}/ReadabilityStudioAPI/"))
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
file_copy(glue("{docFolder}/ReadabilityStudioDocs/20-ReadabilityTestsEnglish.rmd"),
          glue("{docFolder}/ReadabilityTestsReference/20-ReadabilityTestsEnglish.rmd"),
          TRUE)
file_copy(glue("{docFolder}/ReadabilityStudioDocs/21-ReadabilityTestsSpanish.rmd"),
          glue("{docFolder}/ReadabilityTestsReference/21-ReadabilityTestsSpanish.rmd"),
          TRUE)
file_copy(glue("{docFolder}/ReadabilityStudioDocs/23-ReadabilityTestsGerman.rmd"),
          glue("{docFolder}/ReadabilityTestsReference/23-ReadabilityTestsGerman.rmd"),
          TRUE)
file_copy(glue("{docFolder}/ReadabilityStudioDocs/91-Citations.rmd"),
          glue("{docFolder}/ReadabilityTestsReference/91-Citations.rmd"),
          TRUE)
file_copy(glue("{docFolder}/ReadabilityStudioDocs/93-Author.rmd"),
          glue("{docFolder}/ReadabilityTestsReference/93-Author.rmd"),
          TRUE)
file_copy(glue("{docFolder}/ReadabilityStudioDocs/modern-language-association.csl"),
          glue("{docFolder}/ReadabilityTestsReference/modern-language-association.csl"),
          TRUE)
combine_files("41-ScoringNotes.rmd", "scoring-notes",
              "(intro|grade[-]level[-]results|cloze)")
combine_files("92-Acknowledgements.Rmd", "acknowledgements",
              ("(intro|assistance)"))

setwd(glue("{docFolder}/ReadabilityTestsReference/"))
bookdown::render_book(input="index.Rmd",
                      output_format="bookdown::gitbook",
                      output_dir="docs")
bookdown::render_book(input="index.Rmd",
                      output_format="bookdown::pdf_book",
                      output_dir="docs")
unlink(glue("{docFolder}/ReadabilityTestsReference/41-ScoringNotes.rmd"))
unlink(glue("{docFolder}/ReadabilityTestsReference/92-Acknowledgements.Rmd"))
