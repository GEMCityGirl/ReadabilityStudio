library(tidyverse)
library(stringi)
library(magrittr)

# Loads Lua API function signatures from a header file.
# The format should be:
#
# int /*actual return type*/ FuncName(/*parameteres*/)
#
# Parameter and return types are optional.
loadClassInfo <- function(filePath, includeDescription = F)
  {
  classInfoSignatureRE <- R'([[:space:]]*int[[:space:]]*(\/\*([[:space:]_[:alnum:]]*)\*\/)?[[:space:]]*([_[:alnum:]]+)[(]lua_State[*]( L)?[[:space:]]*(\/\*([[:space:]_\,[:alnum:]]*)\*\/)?[)][;]([ ]?\/\/[ ]?([ _[:alnum:][:punct:]]+))?)'
  classText <- read_file(filePath)
  if (includeDescription)
    return(stringr::str_replace(stringr::str_extract_all(classText, classInfoSignatureRE)[[1]], classInfoSignatureRE, '\\3(\\6)->\\2|\\8'))
  else
    return(stringr::str_replace(stringr::str_extract_all(classText, classInfoSignatureRE)[[1]], classInfoSignatureRE, '\\3(\\6)->\\2'))
  }

# Converts an transformed function signature to a topic.
# This is compatible with the signatures returned from loadClassInfo().
functionToTopic <- function(functionInfo)
  {
  xformedSignatureRE <- R"(([_[:alnum:]]+)[(]([[:alnum:],_ ]+)?[)]->([_[:alnum:]]+)?[|]([ _[:alnum:][:punct:]]+)?)"
  topicContent <- stringr::str_replace(stringr::str_extract(functionInfo, xformedSignatureRE)[[1]], xformedSignatureRE, '## `\\1`\n\n\\4\n\n### Syntax {-}\n\n``` {.lua}\n\\3 \\1(\\2)\n```')
  topicContent %<>% stringr::str_replace("\n\n ", "\n\n")

  params <- stringr::str_replace(stringr::str_extract_all(functionInfo, xformedSignatureRE)[[1]], xformedSignatureRE, '\\2')
  if (str_length(params))
    {
    params <- str_replace_all(str_replace_all(params, "([[:alnum:]_]+) ([[:alnum:]_]+)", "|`\\1` \\2 |      |"), ", ", " \n")
    params <- str_glue("\n\n### Parameters {-}\n\n**Parameter** | **Description**\n| :-- | :-- |\n<params>\n\n", .open="<", .close=">");
    }

  returnType <- stringr::str_replace(stringr::str_extract_all(functionInfo, xformedSignatureRE)[[1]], xformedSignatureRE, '\\3')
  if (str_length(returnType))
    {
    returnType <- str_glue("\n\n### Return value {-}\n\nType: `<returnType>`\n\n", .open="<", .close=">");
    }

  fileFooter <- "{{< pagebreak >}}"
  return(str_glue("{topicContent}{returnType}{params}{fileFooter}"))
  }

# Builds function topics for a class (or library), but only if the file doesn't already exist.
# It is assumed that these files can be edited later, usually by adding examples or expanding on what the return type does.
# The purpose of this is to create starter topics for any APIs that aren't documented yet.
writeClassTopics <- function(functions, folderPath)
  {
  for (i in 1 : length(functions))
    {
    topicName <- stringr::str_extract(functions[i], R"(([_[:alnum:]]+))")
    topicContent <- functionToTopic(functions[i])

    if (!file.exists(str_glue("{folderPath}/{topicName}.qmd")))
      {
      message(str_glue("New API topic available: {folderPath}/{topicName}.qmd"))
      readr::write_file(topicContent, str_glue("{folderPath}/{topicName}.qmd"))
      }
    }
  }

# Loads a Lua file with enumeration definitions and generates the intellisense file (for the editor)
# and builds help topics for each enumeration.
#
# Note that enumerations which do not have a comment above them will be excluded. This is by design,
# as it is assumed that these are hidden interfaces which should not be documented.
# Loads the values of an enumeration, with their name and description (from comment after it on the same line).
loadEnumValue <- function(enumValue)
  {
  name = stri_remove_empty_na(stringr::str_extract(enumValue, "[[:space:]]*([[:alnum:]]+)", group = 1))
  description = stri_remove_empty_na(stringr::str_extract(enumValue, "[-]{2,}[[:space:]]*([[:alnum:][:punct:] ]+)", group = 1))

  return(list(name=name, description=description))
  }

# Loads enumerations from a file.
loadEnums <- function(filePath)
  {
  enumText <- read_file(filePath)
  return(stringr::str_extract_all(enumText,
                                  '[-]{2,}[ [:alnum:][:punct:]]+(\r\n|\r|\n)+[ [:alnum:][:punct:]]+[=][ ]*(\r\n|\r|\n)+[[:space:][:alnum:]["][\'][+][=][#][*][(][)][.][,][-][{][=]]+[}]')[[1]])
  }

# Loads an enumeration, its description (single line comment above it), and values (and their respective descriptions).
loadEnum <- function(enum)
  {
  description <- stringr::str_extract(enum, "[-]{1,}[ ]*([[:alnum:] [:punct:]]+)", group = 1)
  name <- stringr::str_extract(enum, "[-]{2,}[ ]*([[:alnum:] [:punct:]]+)[[:space:]]*([:alnum:]+)", group = 2)
  
  values <- stringr::str_extract(enum, '[{]([[:space:][:alnum:]["][\'][+][=][#][*][(][)][.][,][-][{][=]]+)[}]', group = 1)
  values <- stringr::str_extract_all(values, '[:space:]*([ [:alnum:]["][\'][+][=][#][*][(][)][.][,][-][{][=]]+)')
  values <- lapply(values, loadEnumValue)
  
  return(list(description=description, name=name, values=values))
  }

# Converts enumerations to a file in the format of:
# EnumName Value1,Value2,etc.
#
# This is loaded by the script editor (in the actual program) for intellisense.
enumToEditorString <- function(enum)
  {
  str_glue("{enum$name}\t{paste0({enum$values[[1]]$name}, collapse = ';')}")
  }

writeEnumEditorFile <- function(enums, filePath)
  {
  enumEditorString <- "DO NOT EDIT THIS FILE, IT IS GENERATED FROM A BUILD SCRIPT!"
  for (i in 1 : length(enums))
    {
    enum <- loadEnum(enums[i])
    enumEditorString <- str_glue("{enumEditorString}\n{enumToEditorString(enum)}")
    }
  enumEditorString <- stringr::str_trim(enumEditorString)
  readr::write_file(enumEditorString, filePath)
  }

# Converts an enum to a topic
enumToTopic <- function(enum)
  {
  for (i in 1 : length(enum$values[[1]]$name))
    {
    if (is.na(enum$values[[1]]$description[i]))
     {
     stop(str_glue("'{enum$name}' enumeration: '{enum$values[[1]]$name[i]}' value is not documented!"))
     }
    }

  mat <- t(data.frame(matrix(unlist(enum$values[[1]]), nrow=length(enum$values[[1]]), byrow= T)))
  colnames(mat) <- c("VALUES", "DETAILS")
  df <- as_tibble(mat, .name_repair = "unique")
  # sort most of the enums by values' names, except for a few where their order shows a rank of some sort
  if (!(enum$name %in% c("FontWeight")))
    { df %<>% dplyr::arrange(VALUES) }

  dupes <- df %>% janitor::get_dupes(VALUES)
  if (nrow(dupes))
    {
    View(dupes)
    stop(str_glue("'{enum$name}' enumeration: duplicate values!"))
    }

  topicContent <- str_glue(
    "## `<enum$name>` {-}\n\n<enum$description>\n\n| **Value** | **Description** |\n| :-- | :-- |\n",
    .open="<", .close=">")
  for (i in 1 : nrow(df))
    {
    topicContent <- str_glue(
      "<topicContent>\n| `<df$VALUES[i]>` | <df$DETAILS[i]> |",
      .open="<", .close=">")
    }

  fileHeader <- "<!--DO NOT EDIT THIS FILE, IT IS GENERATED FROM A BUILD SCRIPT!-->"
  fileFooter <- "{{< pagebreak >}}"
  return(str_glue("{fileHeader}\n\n{topicContent}\n\n{fileFooter}"))
  }

# Writes topic files for all enums into the provided folder.
# Files will have the same name as the enumeration.
writeEnumTopics <- function(enums, folderPath)
  {
  overviewTopic <- "<!--DO NOT EDIT THIS FILE, IT IS GENERATED FROM A BUILD SCRIPT!-->\n\n# Enumerations\n\nThis chapter discusses the enumeration types available throughout the program.\n\n{{< pagebreak >}}"

  for (i in 1 : length(enums))
    {
    enum <- loadEnum(enums[i])
    topicContent <- enumToTopic(enum)
    readr::write_file(topicContent, str_glue("{folderPath}/{enum$name}.qmd"))
    }

  # parent overview
  readr::write_file(overviewTopic, str_glue("{folderPath}/_overview.qmd"))
  }
