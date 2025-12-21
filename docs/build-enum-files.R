library(tidyverse)
library(stringi)
library(magrittr)
library(janitor)

classifyFunction <- function(name)
  {
  if (str_detect(name, "Chart|Graph|Plot|Image|Bar|Box|Cloud|Histogram|Watermark|Stipple|Connect|Axis|Showcase|UseEnglishLabelsForGermanLix|RaygorStyle|Showcasing|FleschRulerDocGroups")) return("Graphics")
  if (str_detect(name, "TextWindow|Highlighting|TextHighlight|SetDolch|ReportFont|GrammarIssuesHighlightColor|IncludeScoreSummaryReport|HighlightDolch")) return("Reports")
  if (str_detect(name, "Export|Save|Write")) return("Export")
  if (str_detect(name, "SortList")) return("Lists")
  if (str_detect(name, "UI|Dialog|Window|Close|Sidebar|SelectHighlightedWordReport|GetActive|Warning|SplashScreen")) return("User Interface")
  if (str_detect(name, "Reviewer|Title|Language|Status|Settings")) return("General Settings")
  if (str_detect(name, "Harvest|Cookie|JavaScript|Download|Website|ReplaceExistingFiles|SSL|Links|Agent|GetUserFolder|Domain|DepthLevel|ExistingFiles")) return("Web Harvesting")
  if (str_detect(name, "Print|Paper")) return("Printing")
  if (str_detect(name, "MergeWord|MergePhrase|CrossRef")) return("Tools")
  if (str_detect(name, "Log|Message|GetProgramPath|GetLuaConstantsPath|GetExamplesFolder|GetAbsoluteFilePath|FindFiles")) return("Logging & System Info")
  "Analysis Options"
  }

# Loads Lua API function signatures from a header file.
# The format should be:
#
# int /*actual return type*/ FuncName(/*parameteres*/)
#
# Parameter and return types are optional.
loadClassInfo <- function(filePath, includeDescription = FALSE)
  {
  classInfoSignatureRE <- R'([[:space:]]*int[[:space:]]*(\/\*([[:space:]_[:alnum:]]*)\*\/)?[[:space:]]*([_[:alnum:]]+)[(]lua_State[*]( L)?[[:space:]]*(\/\*([[:space:]_\,\.[:alnum:]]*)\*\/)?[)][;]([ ]?\/\/[ ]?([ _[:alnum:][:punct:]]+))?)'
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
  xformedSignatureRE <- R"(([_[:alnum:]]+)[(]([[:alnum:],\._ ]+)?[)]->([_[:alnum:]]+)?[|]([ _[:alnum:][:punct:]]+)?)"
  topicContent <- stringr::str_replace(stringr::str_extract(functionInfo, xformedSignatureRE)[[1]], xformedSignatureRE, '### `\\1` {-}\n\n\\4\n\n#### Syntax {-}\n\n``` {.lua}\n\\3 \\1(\\2)\n```')
  topicContent <- str_replace(topicContent, "\n ", "\n")
  syntax <- str_match(topicContent, R"(```[[:space:]]*\{\.lua\}[[:space:]]*([[:alnum:], \(\)]+))")[[2]]
  funcNameLength <- str_length(str_match(syntax, "^[[:alnum:], ]+[(]"))
  syntaxTab <- str_pad(",\n", funcNameLength + 1, "right") # add one because the newline takes away from funcNameLength
  multiLineSyntax <- str_replace_all(syntax, ", ", syntaxTab)
  topicContent <- str_replace(topicContent, syntax %>% str_replace_all("([(]|[)])", "[\\1]"), multiLineSyntax)

  # format the parameters table (if there are any)
  params <- stringr::str_replace(stringr::str_extract_all(functionInfo, xformedSignatureRE)[[1]], xformedSignatureRE, '\\2')
  if (str_length(params))
    {
    params <- str_replace_all(params, "([[:alnum:]_]+) ([[:alnum:]_]+)", "| `\\1` \\2 |  |")
    # link param type if an uppercased type (which would usually be a custom type)
    params <- str_replace(params, "[|]`([A-Z][[:alnum:]]+)`", "|[`\\1`](#\\1)")
    params <- str_replace(params, "#[A-Z][[:alnum:]]+", str_to_lower) # lowercase the reference
    params <- str_replace_all(params, ", ", " \n")
    params <- str_glue("\n\n#### Parameters {-}\n\n**Parameter** | **Description**\n| :-- | :-- |\n<params>\n\n", .open="<", .close=">");
    }

  returnType <- stringr::str_replace(stringr::str_extract_all(functionInfo, xformedSignatureRE)[[1]], xformedSignatureRE, '\\3')
  if (str_length(returnType))
    {
    # if uppercased type, then it is most likely an object or enum; link to that
    if (str_detect(returnType, '^[A-Z]'))
      {
      returnType <- str_glue("\n\n#### Return value {-}\n\nType: [`<returnType>`](#<str_to_lower(returnType)>)\n\n", .open="<", .close=">");
      }
    else
      {
      returnType <- str_glue("\n\n#### Return value {-}\n\nType: `<returnType>`\n\n", .open="<", .close=">");
      }
    }

  fileFooter <- "\n\n:::"
  return(str_glue('::: {.minipage data-latex="{\\textwidth}"}\n<topicContent>\n\n<returnType><params><fileFooter>',
                  .open = "<",
                  .close = ">"))
  }

fixLegacyHeadings <- function(file)
  {
  lines <- readLines(file, warn = FALSE)
  original <- lines
 
  # function header: ## `Foo` → ### `Foo`
  lines <- sub("^## (`.+`)", "### \\1", lines)
 
  # subsection headers: ### → ####
  lines <- sub("^### (Syntax|Parameters|Return value|Example|See also)", "#### \\1", lines)
 
  # only write back if something actually changed
  if (!identical(lines, original))
    {
    message(sprintf("🛠️  Fixing legacy heading levels in %s", basename(file)))
    writeLines(lines, file)
    }
  }

# Builds function topics for a class (or library), but only if the file doesn't already exist.
# It is assumed that these files can be edited later, usually by adding examples or expanding on what the return type does.
# The purpose of this is to create starter topics for any APIs that aren't documented yet.
writeClassTopics <- function(functions, folderPath, classify = TRUE)
  {
  for (i in seq_along(functions))
    {
    topicName <- stringr::str_extract(functions[i], R"(([_[:alnum:]]+))")
    topicContent <- functionToTopic(functions[i])

    if (classify)
      {
      section <- classifyFunction(topicName)
      sectionPath <- file.path(folderPath, section)
      dir.create(sectionPath, recursive = TRUE, showWarnings = FALSE)
      
      legacyFile <- file.path(folderPath, str_glue("{topicName}.qmd"))
      newFile    <- file.path(sectionPath, str_glue("{topicName}.qmd"))
  
      # already classified → do nothing
      if (file.exists(newFile))
        {
        if (file.exists(newFile))
          {
          fixLegacyHeadings(newFile)
          }
        next
        }
  
      # legacy root file exists → copy it
      if (file.exists(legacyFile))
        {
        message(str_glue("📦 Copying legacy topic: {topicName} → {section}/"))
        file.copy(legacyFile, newFile)
        if (file.exists(newFile))
          {
          fixLegacyHeadings(newFile)
          }
        next
        }
  
      # brand new topic → generate
      message(str_glue("✨ New API topic available: {newFile}"))
      readr::write_file(topicContent, newFile)
      }
    else
      {
      # flat (legacy) behavior
      outFile <- file.path(folderPath, str_glue("{topicName}.qmd"))
  
      if (!file.exists(outFile))
        {
        message(str_glue("✨ New API topic available: {outFile}"))
        readr::write_file(topicContent, outFile)
        }
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
                                  '[-]{2,}[ [:alnum:][:punct:]]+(\r\n|\r|\n)+[ [:alnum:][:punct:]]+[=][ ]*(\r\n|\r|\n)+[[:space:][:alnum:][;&"\'+=#*().,{–—][-]]+[}]')[[1]])
  }

# Loads an enumeration, its description (single line comment above it), and values (and their respective descriptions).
loadEnum <- function(enum)
  {
  description <- stringr::str_extract(enum, "[-]{1,}[ ]*([[:alnum:] [:punct:]]+)", group = 1)
  name <- stringr::str_extract(enum, "[-]{2,}[ ]*([[:alnum:] [:punct:]]+)[[:space:]]*([[:alnum:]]+)", group = 2)
  
  values <- stringr::str_extract(enum, '[{]([[:space:][:alnum:][;&"\'+=#*().,{–—][-]]+)[}]', group = 1)
  values <- stringr::str_extract_all(values, '[:space:]*([ [:alnum:][;&"\'+=#*().,{–—][-]]+)')
  values <- lapply(values, loadEnumValue)
  
  return(list(description=description, name=name, values=values))
  }

# Converts enumerations to a file in the format of:
# EnumName Value1,Value2,etc.
#
# This is loaded by the script editor (in the actual program) for intellisense.
enumToEditorString <- function(enum)
  {
  str_glue("{enum$name}\t{paste(enum$values[[1]]$name, collapse = ';')}")
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
  if (!(enum$name %in% c("FontWeight", "RaygorStyle")))
    { df %<>% dplyr::arrange(VALUES) }

  dupes <- df %>% janitor::get_dupes(VALUES)
  if (nrow(dupes))
    {
    View(dupes)
    stop(str_glue("'{enum$name}' enumeration: duplicate values!"))
    }

  topicContent <- str_glue(
    "## `<enum$name>`\n\n<enum$description>\n\n| **Value** | **Description** |\n| :-- | :-- |\n",
    .open="<", .close=">")
  for (i in 1 : nrow(df))
    {
    topicContent <- str_glue(
      "<topicContent>\n| `<df$VALUES[i]>` | <df$DETAILS[i]> |",
      .open="<", .close=">")
    }

  if (nrow(df) > 30) {
    fileHeader <- "<!--DO NOT EDIT THIS FILE, IT IS GENERATED FROM A BUILD SCRIPT!-->\n\n{{< pagebreak >}}\n\n"
    fileFooter <- "{{< pagebreak >}}"
  }
  else {
    fileHeader <- '<!--DO NOT EDIT THIS FILE, IT IS GENERATED FROM A BUILD SCRIPT!-->\n\n::: {.minipage data-latex="{\\textwidth}"}\n'
    fileFooter <- ":::"
  }
  
  return(str_glue("<fileHeader><topicContent>\n\n<fileFooter>",
                  .open = "<",
                  .close = ">"))
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
