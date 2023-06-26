library(bookdown)
library(glue)
library(readr)
library(stringr)
library(fs)

docFolder <- dirname(rstudioapi::getSourceEditorContext()$path)
source(glue("{docFolder}/ReadabilityStudioDocs/R/appdown.r"))

# delete previous builds
unlink(glue("{docFolder}/ReadabilityStudioAPI/docs"), recursive=T)
unlink(glue("{docFolder}/ReadabilityStudioAPI/images"), recursive=T)
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

setwd(glue("{docFolder}/ReadabilityStudioDocs/"))
combine_files("01-Overviews.Rmd", "overviews",
              ("(intro|program)"))
combine_files("92-Acknowledgements.Rmd", "acknowledgements",
              ("(intro|assistance)"))
bookdown::render_book(input="index.Rmd",
                      output_format="bookdown::pdf_book",
                      output_dir="docs")
combine_files("01-Overviews.Rmd", "overviews")
combine_files("92-Acknowledgements.Rmd", "acknowledgements")
bookdown::render_book(input="index.Rmd",
                      output_format="bookdown::gitbook",
                      output_dir="docs")
# used on github to serve the book
write(c(" "), file=glue("{docFolder}/ReadabilityStudioDocs/docs/.nojekyll"))

# Programming Manual
####################
dir_create(glue("{docFolder}/ReadabilityStudioAPI/images"))
file_copy(glue("{docFolder}/ReadabilityStudioDocs/images/NonGenerated/CC_BY-NC-ND.png"),
          glue("{docFolder}/ReadabilityStudioAPI/images/CC_BY-NC-ND.png"),
          TRUE)
file_copy(glue("{docFolder}/ReadabilityStudioDocs/images/NonGenerated/cover-programming.pdf"),
          glue("{docFolder}/ReadabilityStudioAPI/images/cover-programming.pdf"),
          TRUE)
file_copy(glue("{docFolder}/ReadabilityStudioDocs/images/NonGenerated/cover-programming.png"),
          glue("{docFolder}/ReadabilityStudioAPI/images/cover-programming.png"),
          TRUE)
setwd(glue("{docFolder}/ReadabilityStudioAPI/"))
combine_files("30-Enums.Rmd", "enums")
combine_files("20-Libraries.Rmd", "libraries")
bookdown::render_book(input="index.Rmd",
                      output_format="bookdown::gitbook",
                      output_dir="docs")
write(c(" "), file=glue("{docFolder}/ReadabilityStudioAPI/docs/.nojekyll"))
unlink(glue("{docFolder}/ReadabilityStudioAPI/30-Enums.Rmd"))
unlink(glue("{docFolder}/ReadabilityStudioAPI/20-Libraries.Rmd"))
