dofile(Application.GetLuaConstantsPath())

-- Sort the stop lists
Application.MergeWordLists(
    Debug.GetScriptFolderPath().."Words/stop-words/english.txt",
    Debug.GetScriptFolderPath().."Words/stop-words/english.txt")
Application.MergeWordLists(
    Debug.GetScriptFolderPath().."Words/stop-words/spanish.txt",
    Debug.GetScriptFolderPath().."Words/stop-words/spanish.txt")
Application.MergeWordLists(
    Debug.GetScriptFolderPath().."Words/stop-words/german.txt",
    Debug.GetScriptFolderPath().."Words/stop-words/german.txt")

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
    Debug.GetScriptFolderPath().."Words/stop-words/proper-nouns-stoplist.txt",
    Debug.GetScriptFolderPath().."Words/stop-words/proper-nouns-stoplist.txt")

Application.MergeWordLists(
    Debug.GetScriptFolderPath().."Words/abbreviations/abbreviations.txt",
    Debug.GetScriptFolderPath().."Words/abbreviations/abbreviations.txt")

Application.MergeWordLists(
    Debug.GetScriptFolderPath().."Words/abbreviations/non-abbreviations.txt",
    Debug.GetScriptFolderPath().."Words/abbreviations/non-abbreviations.txt")

Application.MergeWordLists(
    Debug.GetScriptFolderPath().."Words/articles/a-exceptions.txt",
    Debug.GetScriptFolderPath().."Words/articles/a-exceptions.txt")

Application.MergeWordLists(
    Debug.GetScriptFolderPath().."Words/articles/an-exceptions.txt",
    Debug.GetScriptFolderPath().."Words/articles/an-exceptions.txt")

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
    Debug.GetScriptFolderPath().."Words/wordy-phrases/common-errors.txt",
    Debug.GetScriptFolderPath().."Words/wordy-phrases/common-errors.txt")
Application.MergePhraseLists(
    Debug.GetScriptFolderPath().."Words/wordy-phrases/base-english.txt",
    Debug.GetScriptFolderPath().."Words/wordy-phrases/base-english.txt")
Application.MergePhraseLists(
    Debug.GetScriptFolderPath().."Words/wordy-phrases/base-german.txt",
    Debug.GetScriptFolderPath().."Words/wordy-phrases/base-german.txt")
Application.MergePhraseLists(
    Debug.GetScriptFolderPath().."Words/wordy-phrases/base-spanish.txt",
    Debug.GetScriptFolderPath().."Words/wordy-phrases/base-spanish.txt")

Application.MergePhraseLists(
    Debug.GetScriptFolderPath().."Words/wordy-phrases/german.txt",
    Debug.GetScriptFolderPath().."Words/wordy-phrases/common-errors.txt",
    Debug.GetScriptFolderPath().."Words/wordy-phrases/base-german.txt")
Application.MergePhraseLists(
    Debug.GetScriptFolderPath().."Words/wordy-phrases/spanish.txt",
    Debug.GetScriptFolderPath().."Words/wordy-phrases/common-errors.txt",
    Debug.GetScriptFolderPath().."Words/wordy-phrases/base-spanish.txt")
Application.MergePhraseLists(
    Debug.GetScriptFolderPath().."Words/wordy-phrases/english.txt",
    Debug.GetScriptFolderPath().."Words/wordy-phrases/common-errors.txt",
    Debug.GetScriptFolderPath().."Words/wordy-phrases/base-english.txt")

-- Create single word list from wordy phrase list
Application.PhraseListToWordList(
    Debug.GetScriptFolderPath().."Words/wordy-phrases/english.txt",
    Debug.GetScriptFolderPath().."Words/wordy-phrases/single-word-replacements-english.txt")

Application.CrossReferenceWordLists(
    Debug.GetScriptFolderPath().."Words/wordy-phrases/single-word-replacements-english.txt",
    Debug.GetScriptFolderPath().."Words/WordLists/New Dale-Chall.txt",
    Debug.GetScriptFolderPath().."Words/wordy-phrases/dale-chall-replacements.txt")

Application.CrossReferenceWordLists(
    Debug.GetScriptFolderPath().."Words/wordy-phrases/single-word-replacements-english.txt",
    Debug.GetScriptFolderPath().."Words/WordLists/Harris-Jacobson.txt",
    Debug.GetScriptFolderPath().."Words/wordy-phrases/harris-jacobson-replacements.txt")

Application.CrossReferenceWordLists(
    Debug.GetScriptFolderPath().."Words/wordy-phrases/single-word-replacements-english.txt",
    Debug.GetScriptFolderPath().."Words/WordLists/Revised Spache.txt",
    Debug.GetScriptFolderPath().."Words/wordy-phrases/spache-replacements.txt")

-- Merge everything into the English dictionary
Application.MergeWordLists(
    Debug.GetScriptFolderPath().."Words/dictionaries/english.txt",
    Debug.GetScriptFolderPath().."Words/base-english-dictionary.txt",
    Debug.GetScriptFolderPath().."Words/abbreviations/abbreviations.txt",
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
    Debug.GetScriptFolderPath().."Words/Programming/all-languages.txt",
    Debug.GetScriptFolderPath().."Words/Programming/Assembly.txt",
    Debug.GetScriptFolderPath().."Words/Programming/CPP.txt",
    Debug.GetScriptFolderPath().."Words/Programming/CSharp.txt",
    Debug.GetScriptFolderPath().."Words/Programming/HTML.txt",
    Debug.GetScriptFolderPath().."Words/Programming/Java.txt",
    Debug.GetScriptFolderPath().."Words/Programming/Python.txt",
    Debug.GetScriptFolderPath().."Words/Programming/VisualBasic.txt",
    Debug.GetScriptFolderPath().."Words/Programming/SQL.txt",
    Debug.GetScriptFolderPath().."Words/Programming/R.txt")