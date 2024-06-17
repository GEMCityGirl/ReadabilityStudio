dofile(Application.GetLuaConstantsPath())

-- Sort the stop lists
Application.MergeWordLists(
    Debug.GetScriptFolderPath().."words/stop-words/english.txt",
    Debug.GetScriptFolderPath().."words/stop-words/english.txt")
Application.MergeWordLists(
    Debug.GetScriptFolderPath().."words/stop-words/spanish.txt",
    Debug.GetScriptFolderPath().."words/stop-words/spanish.txt")
Application.MergeWordLists(
    Debug.GetScriptFolderPath().."words/stop-words/german.txt",
    Debug.GetScriptFolderPath().."words/stop-words/german.txt")

-- Sort the word lists
Application.MergeWordLists(
    Debug.GetScriptFolderPath().."words/past-participles/exceptions.txt",
    Debug.GetScriptFolderPath().."words/past-participles/exceptions.txt")

Application.MergeWordLists(
    Debug.GetScriptFolderPath().."words/base-english-dictionary.txt",
    Debug.GetScriptFolderPath().."words/base-english-dictionary.txt")

Application.MergeWordLists(
    Debug.GetScriptFolderPath().."words/base-personal.txt",
    Debug.GetScriptFolderPath().."words/base-personal.txt")

Application.MergeWordLists(
    Debug.GetScriptFolderPath().."words/base-non-personal.txt",
    Debug.GetScriptFolderPath().."words/base-non-personal.txt")

Application.MergeWordLists(
    Debug.GetScriptFolderPath().."words/proper-nouns/personal.txt",
    Debug.GetScriptFolderPath().."words/proper-nouns/personal.txt")

Application.MergeWordLists(
    Debug.GetScriptFolderPath().."words/stop-words/proper-nouns-stoplist.txt",
    Debug.GetScriptFolderPath().."words/stop-words/proper-nouns-stoplist.txt")

Application.MergeWordLists(
    Debug.GetScriptFolderPath().."words/abbreviations/abbreviations.txt",
    Debug.GetScriptFolderPath().."words/abbreviations/abbreviations.txt")

Application.MergeWordLists(
    Debug.GetScriptFolderPath().."words/abbreviations/non-abbreviations.txt",
    Debug.GetScriptFolderPath().."words/abbreviations/non-abbreviations.txt")

Application.MergeWordLists(
    Debug.GetScriptFolderPath().."words/articles/a-exceptions.txt",
    Debug.GetScriptFolderPath().."words/articles/a-exceptions.txt")

Application.MergeWordLists(
    Debug.GetScriptFolderPath().."words/articles/an-exceptions.txt",
    Debug.GetScriptFolderPath().."words/articles/an-exceptions.txt")

Application.MergeWordLists(
    Debug.GetScriptFolderPath().."words/word-lists/new-dale-chall.txt",
    Debug.GetScriptFolderPath().."words/word-lists/new-dale-chall.txt")

Application.MergeWordLists(
    Debug.GetScriptFolderPath().."words/word-lists/harris-jacobson.txt",
    Debug.GetScriptFolderPath().."words/word-lists/harris-jacobson.txt")

Application.MergeWordLists(
    Debug.GetScriptFolderPath().."words/word-lists/revised-spache.txt",
    Debug.GetScriptFolderPath().."words/word-lists/revised-spache.txt")

Application.MergeWordLists(
    Debug.GetScriptFolderPath().."words/word-lists/stocker-catholic-supplement.txt",
    Debug.GetScriptFolderPath().."words/word-lists/stocker-catholic-supplement.txt")

-- Expand the proper nouns to possessive forms
-- (personal base is used by an upcoming test as well,
--  so it gets added to the resources as a new file also.)
Application.ExpandWordList(
    Debug.GetScriptFolderPath().."words/base-personal.txt",
    Debug.GetScriptFolderPath().."words/proper-nouns/personal.txt", "s", "'s")

Application.ExpandWordList(
    Debug.GetScriptFolderPath().."words/base-non-personal.txt",
    Debug.GetScriptFolderPath().."NonPersonalTEMP.txt", "s", "'s")

Application.MergeWordLists(
    Debug.GetScriptFolderPath().."words/proper-nouns/all.txt",
    Debug.GetScriptFolderPath().."words/proper-nouns/personal.txt",
    Debug.GetScriptFolderPath().."NonPersonalTEMP.txt")

os.remove(Debug.GetScriptFolderPath().."NonPersonalTEMP.txt")

-- Sort (and reformat) phrase lists
Application.MergePhraseLists(
    Debug.GetScriptFolderPath().."words/wordy-phrases/common-errors.txt",
    Debug.GetScriptFolderPath().."words/wordy-phrases/common-errors.txt")
Application.MergePhraseLists(
    Debug.GetScriptFolderPath().."words/wordy-phrases/base-english.txt",
    Debug.GetScriptFolderPath().."words/wordy-phrases/base-english.txt")
Application.MergePhraseLists(
    Debug.GetScriptFolderPath().."words/wordy-phrases/base-german.txt",
    Debug.GetScriptFolderPath().."words/wordy-phrases/base-german.txt")
Application.MergePhraseLists(
    Debug.GetScriptFolderPath().."words/wordy-phrases/base-spanish.txt",
    Debug.GetScriptFolderPath().."words/wordy-phrases/base-spanish.txt")

Application.MergePhraseLists(
    Debug.GetScriptFolderPath().."words/wordy-phrases/german.txt",
    Debug.GetScriptFolderPath().."words/wordy-phrases/common-errors.txt",
    Debug.GetScriptFolderPath().."words/wordy-phrases/base-german.txt")
Application.MergePhraseLists(
    Debug.GetScriptFolderPath().."words/wordy-phrases/spanish.txt",
    Debug.GetScriptFolderPath().."words/wordy-phrases/common-errors.txt",
    Debug.GetScriptFolderPath().."words/wordy-phrases/base-spanish.txt")
Application.MergePhraseLists(
    Debug.GetScriptFolderPath().."words/wordy-phrases/english.txt",
    Debug.GetScriptFolderPath().."words/wordy-phrases/common-errors.txt",
    Debug.GetScriptFolderPath().."words/wordy-phrases/base-english.txt")

-- Create single word list from wordy phrase list
Application.PhraseListToWordList(
    Debug.GetScriptFolderPath().."words/wordy-phrases/english.txt",
    Debug.GetScriptFolderPath().."words/wordy-phrases/single-word-replacements-english.txt")

Application.CrossReferenceWordLists(
    Debug.GetScriptFolderPath().."words/wordy-phrases/single-word-replacements-english.txt",
    Debug.GetScriptFolderPath().."words/word-lists/new-dale-chall.txt",
    Debug.GetScriptFolderPath().."words/wordy-phrases/dale-chall-replacements.txt")

Application.CrossReferenceWordLists(
    Debug.GetScriptFolderPath().."words/wordy-phrases/single-word-replacements-english.txt",
    Debug.GetScriptFolderPath().."words/word-lists/harris-jacobson.txt",
    Debug.GetScriptFolderPath().."words/wordy-phrases/harris-jacobson-replacements.txt")

Application.CrossReferenceWordLists(
    Debug.GetScriptFolderPath().."words/wordy-phrases/single-word-replacements-english.txt",
    Debug.GetScriptFolderPath().."words/word-lists/revised-spache.txt",
    Debug.GetScriptFolderPath().."words/wordy-phrases/spache-replacements.txt")

-- Merge everything into the English dictionary
Application.MergePhraseLists(
    Debug.GetScriptFolderPath().."words/common-dictionary.txt",
    Debug.GetScriptFolderPath().."words/common-dictionary.txt")
Application.ExpandWordList(
    Debug.GetScriptFolderPath().."words/common-dictionary.txt",
    Debug.GetScriptFolderPath().."CommonDictionaryTEMP.txt", "'s")
Application.MergeWordLists(
    Debug.GetScriptFolderPath().."words/dictionaries/english.txt",
    Debug.GetScriptFolderPath().."words/base-english-dictionary.txt",
    Debug.GetScriptFolderPath().."words/abbreviations/abbreviations.txt",
    Debug.GetScriptFolderPath().."words/proper-nouns/all.txt",
    Debug.GetScriptFolderPath().."CommonDictionaryTEMP.txt")

os.remove(Debug.GetScriptFolderPath().."CommonDictionaryTEMP.txt")

-- Programming dictionaries (this is separate and optionally included)
Application.MergeWordLists(
    Debug.GetScriptFolderPath().."words/programming/assembly.txt",
    Debug.GetScriptFolderPath().."words/programming/assembly.txt")
Application.MergeWordLists(
    Debug.GetScriptFolderPath().."words/programming/cpp.txt",
    Debug.GetScriptFolderPath().."words/programming/cpp.txt")
Application.MergeWordLists(
    Debug.GetScriptFolderPath().."words/programming/csharp.txt",
    Debug.GetScriptFolderPath().."words/programming/csharp.txt")
Application.MergeWordLists(
    Debug.GetScriptFolderPath().."words/programming/html.txt",
    Debug.GetScriptFolderPath().."words/programming/html.txt")
Application.MergeWordLists(
    Debug.GetScriptFolderPath().."words/programming/java.txt",
    Debug.GetScriptFolderPath().."words/programming/java.txt")
Application.MergeWordLists(
    Debug.GetScriptFolderPath().."words/programming/python.txt",
    Debug.GetScriptFolderPath().."words/programming/python.txt")
Application.MergeWordLists(
    Debug.GetScriptFolderPath().."words/programming/visual-basic.txt",
    Debug.GetScriptFolderPath().."words/programming/visual-basic.txt")
Application.MergeWordLists(
    Debug.GetScriptFolderPath().."words/programming/r.txt",
    Debug.GetScriptFolderPath().."words/programming/r.txt")
Application.MergeWordLists(
    Debug.GetScriptFolderPath().."words/programming/sql.txt",
    Debug.GetScriptFolderPath().."words/programming/sql.txt")
Application.MergeWordLists(
    Debug.GetScriptFolderPath().."words/programming/all-languages.txt",
    Debug.GetScriptFolderPath().."words/programming/assembly.txt",
    Debug.GetScriptFolderPath().."words/programming/cpp.txt",
    Debug.GetScriptFolderPath().."words/programming/csharp.txt",
    Debug.GetScriptFolderPath().."words/programming/html.txt",
    Debug.GetScriptFolderPath().."words/programming/java.txt",
    Debug.GetScriptFolderPath().."words/programming/python.txt",
    Debug.GetScriptFolderPath().."words/programming/visual-basic.txt",
    Debug.GetScriptFolderPath().."words/programming/sql.txt",
    Debug.GetScriptFolderPath().."words/programming/r.txt")