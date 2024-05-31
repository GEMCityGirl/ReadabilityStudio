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
    Debug.GetScriptFolderPath().."Words/past-participles/exceptions.txt",
    Debug.GetScriptFolderPath().."Words/past-participles/exceptions.txt")

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
    Debug.GetScriptFolderPath().."Words/proper-nouns/personal.txt",
    Debug.GetScriptFolderPath().."Words/proper-nouns/personal.txt")

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
    Debug.GetScriptFolderPath().."Words/word-lists/new-dale-chall.txt",
    Debug.GetScriptFolderPath().."Words/word-lists/new-dale-chall.txt")

Application.MergeWordLists(
    Debug.GetScriptFolderPath().."Words/word-lists/harris-jacobson.txt",
    Debug.GetScriptFolderPath().."Words/word-lists/harris-jacobson.txt")

Application.MergeWordLists(
    Debug.GetScriptFolderPath().."Words/word-lists/revised-spache.txt",
    Debug.GetScriptFolderPath().."Words/word-lists/revised-spache.txt")

Application.MergeWordLists(
    Debug.GetScriptFolderPath().."Words/word-lists/stocker-catholic-supplement.txt",
    Debug.GetScriptFolderPath().."Words/word-lists/stocker-catholic-supplement.txt")

-- Expand the proper nouns to possessive forms
-- (personal base is used by an upcoming test as well,
--  so it gets added to the resources as a new file also.)
Application.ExpandWordList(
    Debug.GetScriptFolderPath().."Words/base-personal.txt",
    Debug.GetScriptFolderPath().."Words/proper-nouns/personal.txt", "s", "'s")

Application.ExpandWordList(
    Debug.GetScriptFolderPath().."Words/base-non-personal.txt",
    Debug.GetScriptFolderPath().."NonPersonalTEMP.txt", "s", "'s")

Application.MergeWordLists(
    Debug.GetScriptFolderPath().."Words/proper-nouns/all.txt",
    Debug.GetScriptFolderPath().."Words/proper-nouns/personal.txt",
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
    Debug.GetScriptFolderPath().."Words/word-lists/new-dale-chall.txt",
    Debug.GetScriptFolderPath().."Words/wordy-phrases/dale-chall-replacements.txt")

Application.CrossReferenceWordLists(
    Debug.GetScriptFolderPath().."Words/wordy-phrases/single-word-replacements-english.txt",
    Debug.GetScriptFolderPath().."Words/word-lists/harris-jacobson.txt",
    Debug.GetScriptFolderPath().."Words/wordy-phrases/harris-jacobson-replacements.txt")

Application.CrossReferenceWordLists(
    Debug.GetScriptFolderPath().."Words/wordy-phrases/single-word-replacements-english.txt",
    Debug.GetScriptFolderPath().."Words/word-lists/revised-spache.txt",
    Debug.GetScriptFolderPath().."Words/wordy-phrases/spache-replacements.txt")

-- Merge everything into the English dictionary
Application.MergeWordLists(
    Debug.GetScriptFolderPath().."Words/dictionaries/english.txt",
    Debug.GetScriptFolderPath().."Words/base-english-dictionary.txt",
    Debug.GetScriptFolderPath().."Words/abbreviations/abbreviations.txt",
    Debug.GetScriptFolderPath().."Words/proper-nouns/all.txt")

-- Programming dictionaries (this is separate and optionally included)
Application.MergeWordLists(
    Debug.GetScriptFolderPath().."Words/programming/assembly.txt",
    debug.getscriptfolderpath().."words/programming/assembly.txt")
Application.MergeWordLists(
    Debug.GetScriptFolderPath().."Words/programming/cpp.txt",
    Debug.GetScriptFolderPath().."Words/programming/cpp.txt")
Application.MergeWordLists(
    Debug.GetScriptFolderPath().."Words/programming/csharp.txt",
    Debug.GetScriptFolderPath().."Words/programming/csharp.txt")
Application.MergeWordLists(
    Debug.GetScriptFolderPath().."Words/programming/html.txt",
    Debug.GetScriptFolderPath().."Words/programming/html.txt")
Application.MergeWordLists(
    Debug.GetScriptFolderPath().."Words/programming/java.txt",
    Debug.GetScriptFolderPath().."Words/programming/java.txt")
Application.MergeWordLists(
    Debug.GetScriptFolderPath().."Words/programming/python.txt",
    Debug.GetScriptFolderPath().."Words/programming/python.txt")
Application.MergeWordLists(
    Debug.GetScriptFolderPath().."Words/programming/visual-basic.txt",
    Debug.GetScriptFolderPath().."Words/programming/visual-basic.txt")
Application.MergeWordLists(
    Debug.GetScriptFolderPath().."Words/programming/r.txt",
    Debug.GetScriptFolderPath().."Words/programming/r.txt")
Application.MergeWordLists(
    Debug.GetScriptFolderPath().."Words/programming/sql.txt",
    Debug.GetScriptFolderPath().."Words/programming/sql.txt")
Application.MergeWordLists(
    Debug.GetScriptFolderPath().."Words/programming/all-languages.txt",
    Debug.GetScriptFolderPath().."Words/programming/assembly.txt",
    Debug.GetScriptFolderPath().."Words/programming/cpp.txt",
    Debug.GetScriptFolderPath().."Words/programming/csharp.txt",
    Debug.GetScriptFolderPath().."Words/programming/html.txt",
    Debug.GetScriptFolderPath().."Words/programming/java.txt",
    Debug.GetScriptFolderPath().."Words/programming/python.txt",
    Debug.GetScriptFolderPath().."Words/programming/visual-basic.txt",
    Debug.GetScriptFolderPath().."Words/programming/sql.txt",
    Debug.GetScriptFolderPath().."Words/programming/r.txt")