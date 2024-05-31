dofile(Application.GetLuaConstantsPath())

-- Sort the stop lists
Application.MergeWordLists(
    Debug.GetScriptFolderPath().."Words/StopWords/English.txt",
    Debug.GetScriptFolderPath().."Words/StopWords/English.txt")
Application.MergeWordLists(
    Debug.GetScriptFolderPath().."Words/StopWords/Spanish.txt",
    Debug.GetScriptFolderPath().."Words/StopWords/Spanish.txt")
Application.MergeWordLists(
    Debug.GetScriptFolderPath().."Words/StopWords/German.txt",
    Debug.GetScriptFolderPath().."Words/StopWords/German.txt")

-- Sort the word lists
Application.MergeWordLists(
    Debug.GetScriptFolderPath().."Words/PastParticiples/Exceptions.txt",
    Debug.GetScriptFolderPath().."Words/PastParticiples/Exceptions.txt")

Application.MergeWordLists(
    Debug.GetScriptFolderPath().."Words/base-english-dictionary.txt",
    Debug.GetScriptFolderPath().."Words/base-english-dictionary.txt")

Application.MergeWordLists(
    Debug.GetScriptFolderPath().."Words/base-personal.txt",
    Debug.GetScriptFolderPath().."Words/base-personal.txt")

Application.MergeWordLists(
    Debug.GetScriptFolderPath().."Words/base-non-personal.txt",
    Debug.GetScriptFolderPath().."Words/base-non-personal.txt")

Application.MergeWordLists(
    Debug.GetScriptFolderPath().."Words/ProperNouns/Personal.txt",
    Debug.GetScriptFolderPath().."Words/ProperNouns/Personal.txt")

Application.MergeWordLists(
    Debug.GetScriptFolderPath().."Words/StopWords/ProperNounsStopList.txt",
    Debug.GetScriptFolderPath().."Words/StopWords/ProperNounsStopList.txt")

Application.MergeWordLists(
    Debug.GetScriptFolderPath().."Words/Abbreviations/List.txt",
    Debug.GetScriptFolderPath().."Words/Abbreviations/List.txt")

Application.MergeWordLists(
    Debug.GetScriptFolderPath().."Words/Abbreviations/non-abbreviations.txt",
    Debug.GetScriptFolderPath().."Words/Abbreviations/non-abbreviations.txt")

Application.MergeWordLists(
    Debug.GetScriptFolderPath().."Words/Articles/AExceptions.txt",
    Debug.GetScriptFolderPath().."Words/Articles/AExceptions.txt")

Application.MergeWordLists(
    Debug.GetScriptFolderPath().."Words/Articles/AnExceptions.txt",
    Debug.GetScriptFolderPath().."Words/Articles/AnExceptions.txt")

Application.MergeWordLists(
    Debug.GetScriptFolderPath().."Words/WordLists/New Dale-Chall.txt",
    Debug.GetScriptFolderPath().."Words/WordLists/New Dale-Chall.txt")

Application.MergeWordLists(
    Debug.GetScriptFolderPath().."Words/WordLists/Harris-Jacobson.txt",
    Debug.GetScriptFolderPath().."Words/WordLists/Harris-Jacobson.txt")

Application.MergeWordLists(
    Debug.GetScriptFolderPath().."Words/WordLists/Revised Spache.txt",
    Debug.GetScriptFolderPath().."Words/WordLists/Revised Spache.txt")

-- Expand the proper nouns to possessive forms
-- (personal base is used by an upcoming test as well,
--  so it gets added to the resources as a new file also.)
Application.ExpandWordList(
    Debug.GetScriptFolderPath().."Words/base-personal.txt",
    Debug.GetScriptFolderPath().."Words/ProperNouns/Personal.txt", "s", "'s")

Application.ExpandWordList(
    Debug.GetScriptFolderPath().."Words/base-non-personal.txt",
    Debug.GetScriptFolderPath().."NonPersonalTEMP.txt", "s", "'s")

Application.MergeWordLists(
    Debug.GetScriptFolderPath().."Words/ProperNouns/All.txt",
    Debug.GetScriptFolderPath().."Words/ProperNouns/Personal.txt",
    Debug.GetScriptFolderPath().."NonPersonalTEMP.txt")

os.remove(Debug.GetScriptFolderPath().."NonPersonalTEMP.txt")

-- Sort (and reformat) phrase lists
Application.MergePhraseLists(
    Debug.GetScriptFolderPath().."Words/WordyPhrases/common-errors.txt",
    Debug.GetScriptFolderPath().."Words/WordyPhrases/common-errors.txt")
Application.MergePhraseLists(
    Debug.GetScriptFolderPath().."Words/WordyPhrases/german.txt",
    Debug.GetScriptFolderPath().."Words/WordyPhrases/common-errors.txt",
    Debug.GetScriptFolderPath().."Words/WordyPhrases/base-german.txt")
Application.MergePhraseLists(
    Debug.GetScriptFolderPath().."Words/WordyPhrases/spanish.txt",
    Debug.GetScriptFolderPath().."Words/WordyPhrases/common-errors.txt",
    Debug.GetScriptFolderPath().."Words/WordyPhrases/base-spanish.txt")
Application.MergePhraseLists(
    Debug.GetScriptFolderPath().."Words/WordyPhrases/english.txt",
    Debug.GetScriptFolderPath().."Words/WordyPhrases/common-errors.txt",
    Debug.GetScriptFolderPath().."Words/WordyPhrases/base-english.txt")

-- Create single word list from wordy phrase list
Application.PhraseListToWordList(
    Debug.GetScriptFolderPath().."Words/WordyPhrases/english.txt",
    Debug.GetScriptFolderPath().."Words/WordyPhrases/single-word-replacements-english.txt")

Application.CrossReferenceWordLists(
    Debug.GetScriptFolderPath().."Words/WordyPhrases/single-word-replacements-english.txt",
    Debug.GetScriptFolderPath().."Words/WordLists/New Dale-Chall.txt",
    Debug.GetScriptFolderPath().."Words/WordyPhrases/dale-chall-replacements.txt")

Application.CrossReferenceWordLists(
    Debug.GetScriptFolderPath().."Words/WordyPhrases/single-word-replacements-english.txt",
    Debug.GetScriptFolderPath().."Words/WordLists/Harris-Jacobson.txt",
    Debug.GetScriptFolderPath().."Words/WordyPhrases/harris-jacobson-replacements.txt")

Application.CrossReferenceWordLists(
    Debug.GetScriptFolderPath().."Words/WordyPhrases/single-word-replacements-english.txt",
    Debug.GetScriptFolderPath().."Words/WordLists/Revised Spache.txt",
    Debug.GetScriptFolderPath().."Words/WordyPhrases/spache-replacements.txt")

-- Merge everything into the English dictionary
Application.MergeWordLists(
    Debug.GetScriptFolderPath().."Words/Dictionaries/English.txt",
    Debug.GetScriptFolderPath().."Words/base-english-dictionary.txt",
    Debug.GetScriptFolderPath().."Words/Abbreviations/List.txt",
    Debug.GetScriptFolderPath().."Words/ProperNouns/All.txt")

-- Programming dictionaries (this is separate and optionally included)
Application.MergeWordLists(
    Debug.GetScriptFolderPath().."Words/Programming/Assembly.txt",
    Debug.GetScriptFolderPath().."Words/Programming/Assembly.txt")
Application.MergeWordLists(
    Debug.GetScriptFolderPath().."Words/Programming/CPP.txt",
    Debug.GetScriptFolderPath().."Words/Programming/CPP.txt")
Application.MergeWordLists(
    Debug.GetScriptFolderPath().."Words/Programming/CSharp.txt",
    Debug.GetScriptFolderPath().."Words/Programming/CSharp.txt")
Application.MergeWordLists(
    Debug.GetScriptFolderPath().."Words/Programming/HTML.txt",
    Debug.GetScriptFolderPath().."Words/Programming/HTML.txt")
Application.MergeWordLists(
    Debug.GetScriptFolderPath().."Words/Programming/Java.txt",
    Debug.GetScriptFolderPath().."Words/Programming/Java.txt")
Application.MergeWordLists(
    Debug.GetScriptFolderPath().."Words/Programming/Python.txt",
    Debug.GetScriptFolderPath().."Words/Programming/Python.txt")
Application.MergeWordLists(
    Debug.GetScriptFolderPath().."Words/Programming/VisualBasic.txt",
    Debug.GetScriptFolderPath().."Words/Programming/VisualBasic.txt")
Application.MergeWordLists(
    Debug.GetScriptFolderPath().."Words/Programming/R.txt",
    Debug.GetScriptFolderPath().."Words/Programming/R.txt")
Application.MergeWordLists(
    Debug.GetScriptFolderPath().."Words/Programming/SQL.txt",
    Debug.GetScriptFolderPath().."Words/Programming/SQL.txt")
Application.MergeWordLists(
    Debug.GetScriptFolderPath().."Words/Programming/All Languages.txt",
    Debug.GetScriptFolderPath().."Words/Programming/Assembly.txt",
    Debug.GetScriptFolderPath().."Words/Programming/CPP.txt",
    Debug.GetScriptFolderPath().."Words/Programming/CSharp.txt",
    Debug.GetScriptFolderPath().."Words/Programming/HTML.txt",
    Debug.GetScriptFolderPath().."Words/Programming/Java.txt",
    Debug.GetScriptFolderPath().."Words/Programming/Python.txt",
    Debug.GetScriptFolderPath().."Words/Programming/VisualBasic.txt",
    Debug.GetScriptFolderPath().."Words/Programming/SQL.txt",
    Debug.GetScriptFolderPath().."Words/Programming/R.txt")