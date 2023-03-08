/** @addtogroup Formatting
    @brief Classes for parsing and formatting results output.
    @date 2005-2023
    @copyright Oleander Software, Ltd.
    @author Blake Madden
    @details This program is free software; you can redistribute it and/or modify
     it under the terms of the 3-Clause BSD License.

     SPDX-License-Identifier: BSD-3-Clause
* @{*/

#ifndef __WORD_COLLECTION_TEXT_FORMATTING_H__
#define __WORD_COLLECTION_TEXT_FORMATTING_H__

#include <wx/wx.h>
#include <wx/string.h>
#include "../indexing/word_collection.h"
#include "../../../SRC/Wisteria-Dataviz/src/import/rtf_encode.h"
#include "../../../SRC//Wisteria-Dataviz/src/math/mathematics.h"

template<typename documentT, typename highlightDeterminantT>
static size_t FormatWordCollectionHighlightedWords(const documentT* theDocument,
                        const highlightDeterminantT& shouldHighlight,
                        wxChar* text,
                        const size_t bufferSize,
                        const wxString& headerSection,
                        const wxString& endSection,
                        const wxString& legend,
                        const wxString& IGNORE_HIGHLIGHT_BEGIN,
                        const wxString& IGNORE_HIGHLIGHT_END,
                        const wxString& TAB_SYMBOL,
                        const wxString& CRLF,
                        const bool highlightIncompleteSentences,
                        const bool considerOnlyListItemsAsCompleteSentences,
                        const bool highlightInvalidWords,
                        const bool useRtfEncoding)
    {
    std::wmemset(text, wxT(' '), bufferSize);
    std::wcsncpy(text, headerSection, headerSection.length() );
    size_t documentTextLength = headerSection.length();

    std::wcsncpy(text+documentTextLength, legend, legend.length() );
    documentTextLength += legend.length();

    //punctuation markers
    std::vector<punctuation::punctuation_mark>::const_iterator punctPos = theDocument->get_punctuation().begin();
    std::vector<punctuation::punctuation_mark>::const_iterator punctEnd = theDocument->get_punctuation().end();
    //temp word
    lily_of_the_valley::rtf_encode_text rtfEncode;
    std::wstring currentWord;
    for (std::vector<grammar::paragraph_info>::const_iterator para_iter = theDocument->get_paragraphs().begin();
            para_iter != theDocument->get_paragraphs().end();
            ++para_iter)
        {
        // add a tab at the beginning of the paragraph
        std::wcsncpy(text+documentTextLength, TAB_SYMBOL, TAB_SYMBOL.length());
        documentTextLength += TAB_SYMBOL.length();
        //go through the current paragraph's sentences
        for (size_t j = para_iter->get_first_sentence_index();
             j <= para_iter->get_last_sentence_index();
             ++j)
            {
            if (j >= theDocument->get_sentences().size())
                { continue; } //this should not happen, this is just a sanity trap
            const grammar::sentence_info currentSentence = theDocument->get_sentences().at(j);
            const bool currentSentenceShouldBeHighlightedAsInvalid = (highlightIncompleteSentences && considerOnlyListItemsAsCompleteSentences && !currentSentence.is_valid() && (currentSentence.get_type() != grammar::sentence_paragraph_type::header)) ||
                (highlightIncompleteSentences && !considerOnlyListItemsAsCompleteSentences && !currentSentence.is_valid());
            if (currentSentenceShouldBeHighlightedAsInvalid)
                {
                std::wcsncpy(text+documentTextLength, IGNORE_HIGHLIGHT_BEGIN, IGNORE_HIGHLIGHT_BEGIN.length() ); documentTextLength += IGNORE_HIGHLIGHT_BEGIN.length();
                }
            //go through the current sentence's words
            bool atFirstWordInSentence = true;
            bool sentenceTerminatorAppendedAlready = false;
            currentWord.clear();
            for (size_t i = currentSentence.get_first_word_index();
                 i <= currentSentence.get_last_word_index();
                 ++i)
                {
                if (i >= theDocument->get_word_count())
                    { continue; } //shouldn't happen, this is a sanity trap
                currentWord = theDocument->get_word(i).c_str();
                if (!atFirstWordInSentence)
                    {
                    ++documentTextLength;//space between this and previous word
                    }
                atFirstWordInSentence = false;
                if (useRtfEncoding && rtfEncode.needs_to_be_encoded(currentWord))
                    { currentWord = rtfEncode(currentWord); }
                else if (!useRtfEncoding)
                    {
                    currentWord =
                        lily_of_the_valley::html_encode_text::simple_encode(currentWord);
                    }
                //append any punctuation that should be in front of this word
                while (punctPos != punctEnd &&
                    punctPos->get_word_position() == i)
                    {
                    std::wstring punct(1, punctPos->get_punctuation_mark());
                    if (useRtfEncoding && rtfEncode.needs_to_be_encoded(punct))
                        { punct = rtfEncode(punct); }
                    else if (!useRtfEncoding)
                        { punct = lily_of_the_valley::html_encode_text::simple_encode(punct); }
                    std::wcsncpy(text+documentTextLength, punct.c_str(), punct.length() );
                    documentTextLength += punct.length();
                    ++punctPos;
                    }
                //highlight if this word is invalid and not part of an incomplete sentence
                if (currentSentence.is_valid() && highlightInvalidWords &&
                    !theDocument->get_word(i).is_valid())
                    {
                    std::wcsncpy(text+documentTextLength, IGNORE_HIGHLIGHT_BEGIN.wc_str(), IGNORE_HIGHLIGHT_BEGIN.length() ); documentTextLength += IGNORE_HIGHLIGHT_BEGIN.length();
                    std::wcsncpy(text+documentTextLength, currentWord.c_str(), currentWord.length() );
                    documentTextLength += currentWord.length();
                    std::wcsncpy(text+documentTextLength, IGNORE_HIGHLIGHT_END.wc_str(), IGNORE_HIGHLIGHT_END.length() ); documentTextLength += IGNORE_HIGHLIGHT_END.length();
                    }
                //or highlight if this word meets our criteria for highlighting
                else if (!currentSentenceShouldBeHighlightedAsInvalid &&
                    shouldHighlight(theDocument->get_word(i) ) )
                    {
                    std::wcsncpy(text+documentTextLength, shouldHighlight.GetHightlightBegin().wc_str(), shouldHighlight.GetHightlightBegin().length() ); documentTextLength += shouldHighlight.GetHightlightBegin().length();
                    std::wcsncpy(text+documentTextLength, currentWord.c_str(), currentWord.length() );
                    documentTextLength += currentWord.length();
                    std::wcsncpy(text+documentTextLength, shouldHighlight.GetHightlightEnd().wc_str(), shouldHighlight.GetHightlightEnd().length() ); documentTextLength += shouldHighlight.GetHightlightEnd().length();
                    }
                else
                    {
                    std::wcsncpy(text+documentTextLength, currentWord.c_str(), currentWord.length() );
                    documentTextLength += currentWord.length();
                    }

                //append any punctuation that should be after this word
                while (punctPos != punctEnd &&
                    punctPos->get_word_position() == i+1 &&
                    punctPos->is_connected_to_previous_word() )
                    {
                    std::vector<punctuation::punctuation_mark>::const_iterator nextPunctPos = (punctPos+1);
                    //if last word in the sentence AND the last punctuation mark in the document OR
                    //the next punctuation mark is not connected to this word then handle the sentence termination here.
                    if (i == currentSentence.get_last_word_index() &&
                        (nextPunctPos == punctEnd ||
                         nextPunctPos->get_word_position() != i+1 ||
                         !nextPunctPos->is_connected_to_previous_word()) )
                        {
                        std::wstring punct(1, punctPos->get_punctuation_mark());
                        std::wstring endingPunctuation (1, currentSentence.get_ending_punctuation());
                        if (useRtfEncoding)
                            {
                            if (rtfEncode.needs_to_be_encoded(endingPunctuation))
                                { endingPunctuation = rtfEncode(endingPunctuation); }
                            if (rtfEncode.needs_to_be_encoded(punct))
                                { punct = rtfEncode(punct); }
                            }
                        else
                            {
                            endingPunctuation = lily_of_the_valley::html_encode_text::simple_encode(endingPunctuation);
                            punct = lily_of_the_valley::html_encode_text::simple_encode(punct);
                            }
                        //flip the last punctuation and period around if the punctuation is a quote (i.e., ". becomes .")
                        if (characters::is_character::is_quote(punctPos->get_punctuation_mark()))
                            {
                            std::wcsncpy(text+documentTextLength, endingPunctuation.c_str(), endingPunctuation.length() ); documentTextLength += endingPunctuation.length();
                            std::wcsncpy(text+documentTextLength, punct.c_str(), punct.length() ); documentTextLength += punct.length();
                            }
                        else
                            {
                            std::wcsncpy(text+documentTextLength, punct.c_str(), punct.length() );
                            documentTextLength += punct.length();
                            std::wcsncpy(text+documentTextLength, endingPunctuation.c_str(), endingPunctuation.length() );
                            documentTextLength += endingPunctuation.length();
                            }
                        sentenceTerminatorAppendedAlready = true;
                        ++punctPos;
                        break;
                        }
                    std::wstring punct(1, punctPos->get_punctuation_mark());
                    if (useRtfEncoding && rtfEncode.needs_to_be_encoded(punct))
                        { punct = rtfEncode(punct); }
                    else if (!useRtfEncoding)
                        { punct = lily_of_the_valley::html_encode_text::simple_encode(punct); }
                    std::wcsncpy(text+documentTextLength, punct.c_str(), punct.length() );
                        documentTextLength += punct.length();
                    ++punctPos;
                    }
                }

            //append sentence terminator if not done already
            if (!sentenceTerminatorAppendedAlready)
                {
                //watch out for abbreviations at end of sentence
                if (currentWord.length() > 0 && currentWord.back() == L'.')
                    { /*noop*/ }
                else
                    {
                    std::wstring endingPunctuation(1, currentSentence.get_ending_punctuation());
                    if (useRtfEncoding && rtfEncode.needs_to_be_encoded(endingPunctuation))
                        { endingPunctuation = rtfEncode(endingPunctuation); }
                    else if (!useRtfEncoding)
                        { endingPunctuation = lily_of_the_valley::html_encode_text::simple_encode(endingPunctuation); }
                    std::wcsncpy(text+documentTextLength, endingPunctuation.c_str(), endingPunctuation.length() );
                    documentTextLength += endingPunctuation.length();
                    }
                }

            if (currentSentenceShouldBeHighlightedAsInvalid)
                {
                std::wcsncpy(text+documentTextLength, IGNORE_HIGHLIGHT_END, IGNORE_HIGHLIGHT_END.length() ); documentTextLength += IGNORE_HIGHLIGHT_END.length();
                }

            //add a space at the end of the current sentence
            documentTextLength += 2;
            }
        if (para_iter->get_sentence_count() > 0)
            {
            documentTextLength -= 2;
            }
        //add the paragraph line feed
        for (size_t i = 0; i < para_iter->get_leading_end_of_line_count(); ++i)
            {
            std::wcsncpy(text+documentTextLength, CRLF, CRLF.length()); documentTextLength += CRLF.length();
            }
        }

    std::wcsncpy(text+documentTextLength, endSection, endSection.length());
    documentTextLength += endSection.length();
    // terminate the string
    text[documentTextLength] = 0;

    wxASSERT(documentTextLength < bufferSize);

    return documentTextLength;
    }

template<typename documentT>
static size_t FormatWordCollectionHighlightedGrammarIssues(const documentT* theDocument,
                                        const size_t longSentenceValue,
                                        wxChar* text,
                                        const size_t bufferSize,
                                        const wxString& headerSection,
                                        const wxString& endSection,
                                        const wxString& legend,
                                        const wxString& HIGHLIGHT_BEGIN,
                                        const wxString& HIGHLIGHT_END,
                                        const wxString& ERROR_HIGHLIGHT_BEGIN,
                                        const wxString& PHRASE_HIGHLIGHT_BEGIN,
                                        const wxString& IGNORE_HIGHLIGHT_BEGIN,
                                        const wxString& BOLD_BEGIN,
                                        const wxString& BOLD_END,
                                        const wxString& TAB_SYMBOL,
                                        const wxString& CRLF,
                                        const bool highlightIncompleteSentences,
                                        const bool highlightInvalidWords,
                                        const bool useRtfEncoding)
    {
    std::wmemset(text, wxT(' '), bufferSize);
    std::wcsncpy(text, headerSection, headerSection.length() );
    size_t documentTextLength = headerSection.length();

    std::wcsncpy(text+documentTextLength, legend, legend.length() );
    documentTextLength += legend.length();

    const auto& dupWordIndices = theDocument->get_duplicate_word_indices();
    const auto& mismatchedArticleIndices = theDocument->get_incorrect_article_indices();
    std::vector<std::pair<size_t,size_t>>::const_iterator passiveVoicesIter = theDocument->get_passive_voice_indices().begin();
    const auto& misspelledWordIndices = theDocument->get_misspelled_words();
    auto wordyIndicesIter = theDocument->get_known_phrase_indices().cbegin();
    const auto& wordyPhrases =
        theDocument->get_known_phrases().get_phrases();
    std::pair<bool,int> currentPhraseMode(false,0);//whether we are in phrase highlight mode and the "countdown" value (0 means to close the highlighting)
    //punctuation markers
    auto punctPos = theDocument->get_punctuation().cbegin();
    auto punctEnd = theDocument->get_punctuation().cend();
    //temp word
    lily_of_the_valley::rtf_encode_text rtfEncode;
    std::wstring currentWord;
    for (auto para_iter = theDocument->get_paragraphs().cbegin();
         para_iter != theDocument->get_paragraphs().cend();
         ++para_iter)
        {
        //add a tab at the beginning of the paragraph
        std::wcsncpy(text+documentTextLength, TAB_SYMBOL.wc_str(), TAB_SYMBOL.length()); documentTextLength += TAB_SYMBOL.length();
        //go through the current paragraph's sentences
        for (size_t j = para_iter->get_first_sentence_index();
             j <= para_iter->get_last_sentence_index();
             ++j)
            {
            if (j >= theDocument->get_sentences().size())
                { continue; } //this should not happen, this is just a sanity trap
            const grammar::sentence_info currentSentence = theDocument->get_sentences().at(j);
            const size_t currentSentenceLength = highlightInvalidWords ? currentSentence.get_valid_word_count() : currentSentence.get_word_count();
            const bool currentSentenceShouldBeHighlightedAsInvalid = (highlightIncompleteSentences && !currentSentence.is_valid());
            bool currentSentenceIsOverlyLong = false;
            if (currentSentenceShouldBeHighlightedAsInvalid)
                {
                std::wcsncpy(text+documentTextLength, IGNORE_HIGHLIGHT_BEGIN.wc_str(), IGNORE_HIGHLIGHT_BEGIN.length() ); documentTextLength += IGNORE_HIGHLIGHT_BEGIN.length();
                }
            //highlight the sentence if too long
            else if (currentSentenceLength > longSentenceValue)
                {
                currentSentenceIsOverlyLong = true;
                std::wcsncpy(text+documentTextLength, HIGHLIGHT_BEGIN.wc_str(), HIGHLIGHT_BEGIN.length() );
                documentTextLength += HIGHLIGHT_BEGIN.length();
                }
            //go through the current sentence's words
            bool atFirstWordInSentence = true;
            bool sentenceTerminatorAppendedAlready = false;
            currentWord.clear();
            for (size_t i = currentSentence.get_first_word_index();
                 i <= currentSentence.get_last_word_index();
                 ++i)
                {
                if (i >= theDocument->get_word_count())
                    { continue; } //shouldn't happen, this is a sanity trap
                currentWord = theDocument->get_word(i).c_str();
                const bool wordIsInvalid = highlightInvalidWords && !theDocument->get_word(i).is_valid();
                if (!atFirstWordInSentence)
                    {
                    ++documentTextLength;//space between this and previous word
                    }
                atFirstWordInSentence = false;
                //append any punctuation that should be in front of this word
                while (punctPos != punctEnd &&
                    punctPos->get_word_position() == i)
                    {
                    std::wstring punct(1, punctPos->get_punctuation_mark());
                    if (useRtfEncoding && rtfEncode.needs_to_be_encoded(punct))
                        { punct = rtfEncode(punct); }
                    else if (!useRtfEncoding)
                        { punct = lily_of_the_valley::html_encode_text::simple_encode(punct); }
                    std::wcsncpy(text+documentTextLength, punct.c_str(), punct.length() );
                    documentTextLength += punct.length();
                    ++punctPos;
                    }

                // highlight phrases
                if (!currentPhraseMode.first)//if not already in phrase highlight mode (avoids overlapping phrases)
                    {
                    // wordy items
                    while (wordyIndicesIter != theDocument->get_known_phrase_indices().end() && wordyIndicesIter->first < i)
                        { ++wordyIndicesIter; }
                    if (wordyIndicesIter != theDocument->get_known_phrase_indices().end() && wordyIndicesIter->first == i)
                        {
                        if (wordyPhrases[wordyIndicesIter->second].first.get_type() == grammar::phrase_type::phrase_error)
                            {
                            std::wcsncpy(text+documentTextLength, ERROR_HIGHLIGHT_BEGIN.wc_str(),
                                ERROR_HIGHLIGHT_BEGIN.length()); documentTextLength += ERROR_HIGHLIGHT_BEGIN.length();
                            }
                        else
                            {
                            std::wcsncpy(text+documentTextLength, PHRASE_HIGHLIGHT_BEGIN.wc_str(),
                                PHRASE_HIGHLIGHT_BEGIN.length()); documentTextLength += PHRASE_HIGHLIGHT_BEGIN.length();
                            }
                        currentPhraseMode.first = true;
                        currentPhraseMode.second = static_cast<int>(wordyPhrases[wordyIndicesIter->second].first.get_word_count());
                        }
                    }
                // passive voice
                if (!currentPhraseMode.first)
                    {
                    while (passiveVoicesIter != theDocument->get_passive_voice_indices().end() && passiveVoicesIter->first < i)
                        { ++passiveVoicesIter; }
                    if (passiveVoicesIter != theDocument->get_passive_voice_indices().end() && passiveVoicesIter->first == i)
                        {
                        std::wcsncpy(text+documentTextLength, PHRASE_HIGHLIGHT_BEGIN.wc_str(),
                            PHRASE_HIGHLIGHT_BEGIN.length()); documentTextLength += PHRASE_HIGHLIGHT_BEGIN.length();
                        currentPhraseMode.first = true;
                        currentPhraseMode.second = static_cast<int>(passiveVoicesIter->second);
                        }
                    }
                // highlight grammar issues
                const bool isErrorWord = (std::binary_search(dupWordIndices.begin(), dupWordIndices.end(), i) ) ||
                    (std::binary_search(mismatchedArticleIndices.begin(), mismatchedArticleIndices.end(), i) ) ||
                    (std::binary_search(misspelledWordIndices.begin(), misspelledWordIndices.end(), i) );
                if (currentSentence.is_valid() && wordIsInvalid)//valid sentence, but word is invalid
                    {
                    std::wcsncpy(text+documentTextLength, IGNORE_HIGHLIGHT_BEGIN.wc_str(),
                        IGNORE_HIGHLIGHT_BEGIN.length()); documentTextLength += IGNORE_HIGHLIGHT_BEGIN.length();
                    }
                else if (isErrorWord)
                    {
                    std::wcsncpy(text+documentTextLength, ERROR_HIGHLIGHT_BEGIN.wc_str(),
                        ERROR_HIGHLIGHT_BEGIN.length()); documentTextLength += ERROR_HIGHLIGHT_BEGIN.length();
                    }
                // copy over the word
                if (useRtfEncoding && rtfEncode.needs_to_be_encoded(currentWord))
                    { currentWord = rtfEncode(currentWord); }
                else if (!useRtfEncoding)
                    {
                    currentWord = lily_of_the_valley::html_encode_text::simple_encode(currentWord);
                    }
                std::wcsncpy(text+documentTextLength, currentWord.c_str(), currentWord.length() );
                documentTextLength += currentWord.length();
                // unhighlight grammar error/excluded word
                if (isErrorWord || (currentSentence.is_valid() && wordIsInvalid))
                    {
                    std::wcsncpy(text+documentTextLength, HIGHLIGHT_END.wc_str(),
                        HIGHLIGHT_END.length()); documentTextLength += HIGHLIGHT_END.length();
                    }
                if (currentPhraseMode.first)
                    {
                    --currentPhraseMode.second;
                    if (currentPhraseMode.second <= 0)
                        {
                        // out of phrase highlight mode now
                        currentPhraseMode.first = false;
                        std::wcsncpy(text+documentTextLength, HIGHLIGHT_END.wc_str(),
                            HIGHLIGHT_END.length()); documentTextLength += HIGHLIGHT_END.length();
                        }
                    }

                // append any punctuation that should be after this word
                while (punctPos != punctEnd &&
                    punctPos->get_word_position() == i+1 &&
                    punctPos->is_connected_to_previous_word() )
                    {
                    std::vector<punctuation::punctuation_mark>::const_iterator nextPunctPos = (punctPos+1);
                    //if last word in the sentence AND the last punctuation mark in the document OR
                    //the next punctuation mark is not connected to this word then handle the sentence termination here.
                    if (i == currentSentence.get_last_word_index() &&
                        (nextPunctPos == punctEnd ||
                         nextPunctPos->get_word_position() != i + 1 ||
                         !nextPunctPos->is_connected_to_previous_word()) )
                        {
                        std::wstring punct(1, punctPos->get_punctuation_mark());
                        std::wstring endingPunctuation(1, currentSentence.get_ending_punctuation());
                        if (useRtfEncoding)
                            {
                            if (rtfEncode.needs_to_be_encoded(endingPunctuation))
                                { endingPunctuation = rtfEncode(endingPunctuation); }
                            if (rtfEncode.needs_to_be_encoded(punct))
                                { punct = rtfEncode(punct); }
                            }
                        else
                            {
                            endingPunctuation =
                                lily_of_the_valley::html_encode_text::simple_encode(endingPunctuation);
                            punct =
                                lily_of_the_valley::html_encode_text::simple_encode(punct);
                            }
                        // flip the last punctuation and period around if the punctuation is a quote (i.e., ". becomes .")
                        if (characters::is_character::is_quote(punctPos->get_punctuation_mark()))
                            {
                            std::wcsncpy(text+documentTextLength, endingPunctuation.c_str(), endingPunctuation.length() );
                            documentTextLength += endingPunctuation.length();
                            std::wcsncpy(text+documentTextLength, punct.c_str(), punct.length() );
                            documentTextLength += punct.length();
                            }
                        else
                            {
                            std::wcsncpy(text+documentTextLength, punct.c_str(), punct.length() ); documentTextLength += punct.length();
                            std::wcsncpy(text+documentTextLength, endingPunctuation.c_str(), endingPunctuation.length() );
                            documentTextLength += endingPunctuation.length();
                            }
                        sentenceTerminatorAppendedAlready = true;
                        ++punctPos;
                        break;
                        }
                    std::wstring punct(1, punctPos->get_punctuation_mark());
                    if (useRtfEncoding && rtfEncode.needs_to_be_encoded(punct))
                        { punct = rtfEncode(punct); }
                    else if (!useRtfEncoding)
                        { punct = lily_of_the_valley::html_encode_text::simple_encode(punct); }
                    std::wcsncpy(text+documentTextLength, punct.c_str(), punct.length() );
                    documentTextLength += punct.length();
                    ++punctPos;
                    }
                }

            // append sentence terminator if not done already
            if (!sentenceTerminatorAppendedAlready)
                {
                if (currentWord.length() > 0 && currentWord.back() == L'.')
                    { /*noop*/ }
                else
                    {
                    // watch out for abbreviations at end of sentence
                    std::wstring endingPunctuation(1, currentSentence.get_ending_punctuation());
                    if (useRtfEncoding && rtfEncode.needs_to_be_encoded(endingPunctuation))
                        { endingPunctuation = rtfEncode(endingPunctuation); }
                    else if (!useRtfEncoding)
                        {
                        endingPunctuation =
                            lily_of_the_valley::html_encode_text::simple_encode(endingPunctuation);
                        }
                    std::wcsncpy(text+documentTextLength, endingPunctuation.c_str(), endingPunctuation.length() );
                    documentTextLength += endingPunctuation.length();
                    }
                }

            wxString wordCountStr = wxString::Format(wxT(" %s (%zu)%s"), BOLD_BEGIN, currentSentenceLength, BOLD_END);
            if (currentSentenceShouldBeHighlightedAsInvalid ||
                currentSentenceIsOverlyLong)
                {
                if (currentSentenceIsOverlyLong)
                    {
                    std::wcsncpy(text+documentTextLength, HIGHLIGHT_END.wc_str(), HIGHLIGHT_END.length() );
                    documentTextLength += HIGHLIGHT_END.length();
                    std::wcsncpy(text+documentTextLength, wordCountStr.wc_str(), wordCountStr.length() );
                    documentTextLength += wordCountStr.length();
                    }
                else
                    {
                    std::wcsncpy(text+documentTextLength, HIGHLIGHT_END.wc_str(), HIGHLIGHT_END.length() );
                    documentTextLength += HIGHLIGHT_END.length();
                    }
                }
            else
                {
                std::wcsncpy(text+documentTextLength, wordCountStr.wc_str(), wordCountStr.length() );
                documentTextLength += wordCountStr.length();
                }

            // add a space at the end of the current sentence
            documentTextLength += 2;
            }
        if (para_iter->get_sentence_count() > 0)
            {
            documentTextLength -= 2;
            }
        // add the paragraph line feed
        for (size_t i = 0; i < para_iter->get_leading_end_of_line_count(); ++i)
            {
            std::wcsncpy(text+documentTextLength, CRLF.wc_str(), CRLF.length());
            documentTextLength += CRLF.length();
            }
        }

    std::wcsncpy(text+documentTextLength, endSection, endSection.length());
    documentTextLength += endSection.length();
    // terminate the string
    text[documentTextLength] = 0;

    wxASSERT(documentTextLength < bufferSize);

    return documentTextLength;
    }

template<typename documentT>
static size_t FormatFilteredWordCollection(const documentT* theDocument,
                                           wxChar* text,
                                           const size_t bufferSize,
                                           const InvalidTextFilterFormat validTextFormatting,
                                           const bool removeFilePaths,
                                           const bool stripAbbreviations)
    {
    wxASSERT(text);
    if (!text)
        { return 0; }
    std::wmemset(text, wxT(' '), bufferSize);
    size_t documentTextLength = 0;

    // punctuation markers
    std::vector<punctuation::punctuation_mark>::const_iterator punctPos = theDocument->get_punctuation().begin();
    std::vector<punctuation::punctuation_mark>::const_iterator punctEnd = theDocument->get_punctuation().end();
    // temp word
    std::wstring currentWord;
    for (auto para_iter = theDocument->get_paragraphs().cbegin();
         para_iter != theDocument->get_paragraphs().cend();
         ++para_iter)
        {
        // add a tab at the beginning of the paragraph
        text[documentTextLength++] = wxT('\t');
        // go through the current paragraph's sentences
        size_t formattedSentencesInCurrentParagraph = 0;
        for (size_t j = para_iter->get_first_sentence_index();
             j <= para_iter->get_last_sentence_index();
             ++j)
            {
            // this should not happen, this is just a sanity trap
            if (j >= theDocument->get_sentences().size())
                { continue; }
            const grammar::sentence_info currentSentence = theDocument->get_sentences().at(j);
            if (validTextFormatting == InvalidTextFilterFormat::IncludeOnlyValidText)
                {
                if (!currentSentence.is_valid())
                    { continue; }
                }
            else if (validTextFormatting == InvalidTextFilterFormat::IncludeOnlyInvalidText)
                {
                if (currentSentence.is_valid())
                    { continue; }
                }
            ++formattedSentencesInCurrentParagraph;
            // go through the current sentence's words
            bool atFirstWordInSentence = true;
            bool sentenceTerminatorAppendedAlready = false;
            currentWord.clear();
            for (size_t i = currentSentence.get_first_word_index();
                 i <= currentSentence.get_last_word_index();
                 ++i)
                {
                // shouldn't happen, this is a sanity trap
                if (i >= theDocument->get_word_count())
                    { continue; }
                if (removeFilePaths && theDocument->get_word(i).is_file_address())
                    { continue; }
                if (validTextFormatting == InvalidTextFilterFormat::IncludeOnlyValidText && !theDocument->get_word(i).is_valid())
                    { continue; }
                currentWord = theDocument->get_word(i).c_str();
                if (stripAbbreviations && !theDocument->get_word(i).is_numeric() &&
                    // last word in the sentence will need to keep its period (if an abbreviation)
                    i < currentSentence.get_last_word_index() &&
                    currentWord.length() && currentWord.back()== wxT('.'))
                    { currentWord.pop_back(); }
                if (!atFirstWordInSentence)
                    {
                    // space between this and previous word
                    ++documentTextLength;
                    }
                atFirstWordInSentence = false;
                // append any punctuation that should be in front of this word
                while (punctPos != punctEnd &&
                    punctPos->get_word_position() < i)
                    // this is needed because a skipped sentence could prevent us from
                    // comparing against every word position
                    { ++punctPos; }
                while (punctPos != punctEnd &&
                    punctPos->get_word_position() == i)
                    {
                    wxString punct = punctPos->get_punctuation_mark();
                    std::wcsncpy(text+documentTextLength, punct.wc_str(), punct.length() ); documentTextLength += punct.length();
                    ++punctPos;
                    }

                // copy over the word
                std::wcsncpy(text+documentTextLength, currentWord.c_str(), currentWord.length() );
                documentTextLength += currentWord.length();

                // append any punctuation that should be after this word
                while (punctPos != punctEnd &&
                    punctPos->get_word_position() == i+1 &&
                    punctPos->is_connected_to_previous_word() )
                    {
                    std::vector<punctuation::punctuation_mark>::const_iterator nextPunctPos = (punctPos+1);
                    // if last word in the sentence AND the last punctuation mark in the document OR
                    // the next punctuation mark is not connected to this word then handle the sentence termination here.
                    if (i == currentSentence.get_last_word_index() &&
                        (nextPunctPos == punctEnd ||
                         nextPunctPos->get_word_position() != i+1 ||
                         !nextPunctPos->is_connected_to_previous_word()) )
                        {
                        std::wstring punct(1, punctPos->get_punctuation_mark());
                        std::wstring endingPunctuation(1, currentSentence.get_ending_punctuation());
                        // flip the last punctuation and period around if the punctuation is a quote (i.e., ". becomes .")
                        if (characters::is_character::is_quote(punctPos->get_punctuation_mark()))
                            {
                            std::wcsncpy(text+documentTextLength, endingPunctuation.c_str(), endingPunctuation.length() ); documentTextLength += endingPunctuation.length();
                            std::wcsncpy(text+documentTextLength, punct.c_str(), punct.length() ); documentTextLength += punct.length();
                            }
                        else
                            {
                            std::wcsncpy(text+documentTextLength, punct.c_str(), punct.length() ); documentTextLength += punct.length();
                            std::wcsncpy(text+documentTextLength, endingPunctuation.c_str(), endingPunctuation.length() ); documentTextLength += endingPunctuation.length();
                            }
                        sentenceTerminatorAppendedAlready = true;
                        ++punctPos;
                        break;
                        }
                    std::wstring punct(1, punctPos->get_punctuation_mark());
                    std::wcsncpy(text+documentTextLength, punct.c_str(), punct.length() );
                    documentTextLength += punct.length();
                    ++punctPos;
                    }
                }

            // append sentence terminator if not done already
            if (!sentenceTerminatorAppendedAlready)
                {
                if (currentWord.length() > 0 && currentWord.back() == L'.')
                    { /*noop*/ }
                else
                    {
                    // watch out for abbreviations at end of sentence
                    std::wstring endingPunctuation(1, currentSentence.get_ending_punctuation());
                    std::wcsncpy(text + documentTextLength, endingPunctuation.c_str(), endingPunctuation.length() ); documentTextLength += endingPunctuation.length();
                    }
                }

            // add a space at the end of the current sentence
            documentTextLength += 2;
            }
        if (formattedSentencesInCurrentParagraph > 0)
            { documentTextLength -= 2; }
        // add the paragraph line feed(s)
        for (size_t i = 0; i < para_iter->get_leading_end_of_line_count(); ++i)
            { text[documentTextLength++] = L'\n'; }
        }

    // terminate the string
    text[documentTextLength] = 0;

    wxASSERT(documentTextLength < bufferSize);

    return documentTextLength;
    }

/** @}*/

#endif //__WORD_COLLECTION_TEXT_FORMATTING_H__
