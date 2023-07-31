library(tidyverse)
library(magrittr)
library(stringr)

srcFolder <- str_glue('{dirname(rstudioapi::getSourceEditorContext()$path)}/../src')
buildFile <- str_glue('{dirname(rstudioapi::getSourceEditorContext()$path)}/files.cmake')

# Files for full application
################################################
files <- str_glue(
  "src/{list.files(path=srcFolder, pattern='(*[.]cpp|cJSON[.]c|onelua_no_warnings[.]c)', recursive=TRUE)}")
# remove test and sample files from submodules
files <- files[!grepl("(demo.cpp|main.cpp|i18n-check/samples|Wisteria-Dataviz/tests|Catch2|OleanderStemmingLibrary/tests|tinyexpr-plusplus/tests|i18n-check/tests/|cpp_i18n_review.cpp|i18n_review.cpp|utfcpp/tests|utfcpp/samples|utfcpp/extern|cxxopts|xmltest.cpp|html5-printer.cpp)", files)]
write_file(str_glue("# Automatically generated from 'Build CMake Files List.R'
# DO NOT MODIFY MANUALLY!

SET(WISTERIA_SRC\n    {paste(files, collapse='\n    ')})"),
           file=buildFile)
