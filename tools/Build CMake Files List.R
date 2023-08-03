library(tidyverse)
library(magrittr)
library(stringr)

srcFolder <- str_glue('{dirname(rstudioapi::getSourceEditorContext()$path)}/../src')
sourcesFile <- str_glue('{dirname(rstudioapi::getSourceEditorContext()$path)}/files.cmake')

imagesFolder <- str_glue('{dirname(rstudioapi::getSourceEditorContext()$path)}/../Resources/Images')
imagesFile <- str_glue('{dirname(rstudioapi::getSourceEditorContext()$path)}/images.cmake')

wordsFolder <- str_glue('{dirname(rstudioapi::getSourceEditorContext()$path)}/../Resources/Words')
wordsFile <- str_glue('{dirname(rstudioapi::getSourceEditorContext()$path)}/words.cmake')

# Source files for application
################################################
files <- str_glue(
  "src/{list.files(path=srcFolder, pattern='(*[.]cpp|cJSON[.]c|onelua_no_warnings[.]c)', recursive=TRUE)}")
# remove test and sample files from submodules
files <- files[!grepl("(demo.cpp|main.cpp|i18n-check/samples|Wisteria-Dataviz/tests|Catch2|OleanderStemmingLibrary/tests|tinyexpr-plusplus/tests|i18n-check/tests/|cpp_i18n_review.cpp|utfcpp/tests|utfcpp/samples|utfcpp/extern|cxxopts|xmltest.cpp|html5-printer.cpp)", files)]
write_file(str_glue("# Automatically generated from 'Build CMake Files List.R'
# DO NOT MODIFY MANUALLY!

SET(APP_SRC_FILES\n    {paste(files, collapse='\n    ')})"),
           file=sourcesFile)

# Image (and menu) files for resources
################################################
files <- str_glue(
  "{list.files(path=imagesFolder, pattern='*[.](png|jpg|svg|xrc)', recursive=TRUE)}")
write_file(paste(files, collapse='\n'), file=imagesFile)

# Word lists
################################################
files <- str_glue(
  "{list.files(path=wordsFolder, pattern='*[.]txt', recursive=TRUE)}")
files <- files[!grepl("(BaseEnglishDictionary.txt|NonPersonalBase.txt|PersonalBase.txt)", files)]
write_file(paste(files, collapse='\n'), file=wordsFile)
