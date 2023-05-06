library(bookdown)
library(glue)
library(readr)
library(stringr)

docFolder <- dirname(rstudioapi::getSourceEditorContext()$path)

# delete previous builds
unlink(glue("{docFolder}/ReadabilityStudioAPI/docs"), recursive=T)
unlink(glue("{docFolder}/ReadabilityStudio/docs"), recursive=T)
unlink(glue("{docFolder}/ReadabilityStudio/_bookdown_files"), recursive=T)
unlink(glue("{docFolder}/Coding-Bible/docs"), recursive=T)

setwd(glue("{docFolder}/Coding-Bible/"))
bookdown::render_book(input="index.Rmd",
                      output_dir="docs")

setwd(glue("{docFolder}/ReadabilityStudio/"))
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
write(c(" "), file=glue("{docFolder}/ReadabilityStudio/docs/.nojekyll"))

setwd(glue("{docFolder}/ReadabilityStudioAPI/"))
combine_files("30-Enums.Rmd", "enums")
combine_files("20-Libraries.Rmd", "libraries")
bookdown::render_book(input="index.Rmd",
                      output_format="bookdown::gitbook",
                      output_dir="docs")
write(c(" "), file=glue("{docFolder}/ReadabilityStudioAPI/docs/.nojekyll"))
unlink(glue("{docFolder}/ReadabilityStudioAPI/30-Enums.Rmd"))
unlink(glue("{docFolder}/ReadabilityStudioAPI/20-Libraries.Rmd"))
