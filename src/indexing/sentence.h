/** @addtogroup Indexing
    @brief Classes for parsing and indexing text.
    @date 2005-2023
    @copyright Oleander Software, Ltd.
    @author Blake Madden
    @details This program is free software; you can redistribute it and/or modify
     it under the terms of the 3-Clause BSD License.

     SPDX-License-Identifier: BSD-3-Clause
* @{*/

#ifndef __SENTENCE_H__
#define __SENTENCE_H__

#include <vector>
#include "word.h"
#include "abbreviation.h"
#include "punctuation.h"
#include "characters.h"
#include "../OleanderStemmingLibrary/src/common_lang_constants.h"

namespace grammar
    {
    /** @brief Determinant for end-of-line*/
    class is_end_of_line
        {
    public:
        /// @brief Constructor.
        is_end_of_line() noexcept
            {}
        /** @returns @c true if a character is a newline or linefeed.
            @param ch The character to examine.*/
        [[nodiscard]]
        constexpr bool operator()(const wchar_t ch) const noexcept
            { return (ch == 13 || ch == 10 || ch == 12); }
        /** @brief Counts how many unique end of line characters (or sets) are in a string.
            @param first The start of the string.
            @param last The end of the string.*/
        void operator()(const wchar_t* first, const wchar_t* last) noexcept
            {
            // reset data from last call
            m_characters_skipped_count = 0;
            m_eol_count = 0;

            if (!first || !last || (first > last))
                { return; }

            const wchar_t* const originalStart = first;

            size_t lineFeedCount(0), carriageReturnCount(0), formFeedCount(0);
            while (first != last)
                {
                if (first[0] == 10)
                    { ++lineFeedCount; }
                else if (first[0] == 13)
                    { ++carriageReturnCount; }
                else if (first[0] == 12)
                    { ++formFeedCount; }
                // scan over any spaces or tabs in case this is a blank line between two newlines
                else if (characters::is_character::is_space_horizontal(first[0]))
                    {
                    const wchar_t* scanAheadForNonSpace = first+1;
                    while (scanAheadForNonSpace != last &&
                        characters::is_character::is_space_horizontal(scanAheadForNonSpace[0]) )
                        { ++scanAheadForNonSpace; }
                    // Stopped on a non-space, evaluate if we are on a newline now.
                    // If so, then move to the newline and keep going. Otherwise, ignore the
                    // spaces and stop.
                    if (scanAheadForNonSpace != last &&
                        characters::is_character::is_space_vertical(scanAheadForNonSpace[0]) )
                        {
                        first = scanAheadForNonSpace;
                        continue;
                        }
                    else
                        { break; }
                    }
                else
                    { break; }
                ++first;
                }
            m_characters_skipped_count = (first-originalStart);
            // if no line feeds, this only carriage returns where used
            if (lineFeedCount == 0)
                { m_eol_count = carriageReturnCount; }
            // if no carriage returns, this only line feeds where used
            else if (carriageReturnCount == 0)
                { m_eol_count = lineFeedCount; }
            /* sometimes 10 and 13 are used as a single EOL separator,
               so pair them together if necessary*/
            else
                {
                // if they were perfectly paired up, then count them as sets
                if (carriageReturnCount == lineFeedCount)
                    { m_eol_count = lineFeedCount; }
                /* Otherwise, the text is malformed. Just pair them off and then
                   add the extra characters to it.*/
                else if (carriageReturnCount > lineFeedCount)
                    { m_eol_count = lineFeedCount + (carriageReturnCount - lineFeedCount); }
                else if (lineFeedCount > carriageReturnCount)
                    { m_eol_count = carriageReturnCount + (lineFeedCount - carriageReturnCount); }
                else
                    { m_eol_count = 0; }
                }
            m_eol_count += formFeedCount;
            }
        /** @returns The number of characters scanned from the last call to operator().*/
        [[nodiscard]]
        size_t get_characters_skipped_count() const noexcept
            { return m_characters_skipped_count; }
        /** @returns The number of lines read from the last call to operator().*/
        [[nodiscard]]
        size_t get_eol_count() const noexcept
            { return m_eol_count; }
    private:
        size_t m_characters_skipped_count{ 0 };
        size_t m_eol_count{ 0 };
        };

    /** @brief Text indentation functor.*/
    class is_intented_text
        {
    public:
        /** @brief Indicates whether a line of text is indented.
            @param text The line of text, which should be the start of a text
                stream right after a newline.
            @returns Whether text stream is indented, and the length of the indentation if true.*/
        [[nodiscard]]
        const std::pair<bool,size_t> operator()(const wchar_t* text) const noexcept
            {
            if (text == nullptr || text[0] == 0)
                {
                return std::make_pair(false, 0);
                }
            // first see if it's a tab
            else if (text[0] == 0x09)
                {
                return std::make_pair(true, 1);
                }
            // else, see if it more than two spaces (more than likely an indentation)
            const wchar_t* current_char = text;
            while (current_char[0] != 0 &&
                   characters::is_character::is_space_horizontal(current_char[0]))
                { ++current_char; }
            if ((current_char - text) > 2)
                {
                return std::make_pair(true, (current_char - text));
                }
            return std::make_pair(false, 0);
            }
        };

    /** @brief Bulleted text identifying functor.*/
    class is_bulleted_text
        {
    public:
        /** @brief Indicates whether a line of text is a bullet point.
            @param text The line of text, which should be the start of a
                text stream right after a newline.
            @returns Whether the text stream begins with a bullet,
                and the length of the bullet if true.*/
        [[nodiscard]]
        const std::pair<bool, size_t> operator()(std::wstring_view text) const noexcept
            {
            if (text.empty())
                {
                return std::make_pair(false, 0);
                }
            // first, step over any spaces at the start of the line,
            // sometimes bullets can be indented
            while (text.length() &&
                   characters::is_character::is_space_horizontal(text[0]))
                {
                text.remove_prefix(1);
                }
            if (text.empty())
                {
                return std::make_pair(false, 0);
                }
            // first see if it's a bullet
            else if (string_util::is_either<wchar_t>(text[0], 0x95, 0x2022)/* bullet*/ ||
                text[0] == 0x2023 /* triangle bullet*/ ||
                text[0] == 0x25A0 /* filled in square*/ ||
                text[0] == 0x25CF /* filled in circle*/ ||
                text[0] == 0xB7   /* middle dot*/)
                {
                return std::make_pair(true, 1);
                }
            // or a dash followed by a space
            else if (characters::is_character::is_hyphen(text[0]) &&
                characters::is_character::is_space_horizontal(text[1]))
                {
                return std::make_pair(true, 1);
                }
            // else, see if it more than two spaces (more than likely an indentation)
            const wchar_t* current_char = text.data();
            while (current_char[0] != 0 &&
                characters::is_character::is_space_horizontal(current_char[0]))
                { ++current_char; }
            if (current_char[0] == 0)
                {
                return std::make_pair(false, 0);
                }
            // or a numeric bullet
            if (characters::is_character::is_numeric(current_char[0]))
                {
                ++current_char;
                // scan until we hit something non-numeric
                do
                    {
                    if (characters::is_character::is_numeric(current_char[0]))
                        { ++current_char; }
                    else
                        { break; }
                    } while (current_char[0]);
                // if at the end of the text then this is not a bullet
                if (current_char[0] == 0)
                    {
                    return std::make_pair(false, 0);
                    }
                // if number if followed by a dot, tab, closing parenthesis, or colon then it is a numeric bullet
                else if (characters::is_character::is_period(current_char[0]) ||
                    current_char[0] == 0x09 ||
                    current_char[0] == 0x29 ||
                    current_char[0] == 0xFF09 ||
                    // colon only makes sense if it is followed by a space. If it part of the numeric work
                    // then it must be a time or something.
                    (current_char[0] == L':' && characters::is_character::is_space_horizontal(current_char[1])))
                    {
                    // sometimes you might see "1.) Some info", so skip all expected punctuation
                    do
                        {
                        ++current_char;
                        } while (current_char[0] &&
                           (characters::is_character::is_period(current_char[0]) ||
                            // right parentheses
                            current_char[0] == 0x29 ||
                            current_char[0] == 0xFF09));
                    return std::make_pair(true, (current_char - text.data()));
                    }
                else
                    {
                    // scan to the next line and see if it starts with a number too. If so,
                    // then this more than likely is a list of some sort (e.g., a recipe).
                    is_end_of_line isEol;
                    const wchar_t* nextLine = current_char;
                    while (nextLine[0] != 0 && string_util::is_neither<wchar_t>(nextLine[0],10,13))
                        { ++nextLine; }
                    if (nextLine[0] == 0)
                        {
                        return std::make_pair(false, 0);
                        }
                    // hit the next newline, so skip to the start of the line
                    const wchar_t* const newLineStart = nextLine;
                    while (nextLine[0] != 0 && characters::is_character::is_space(nextLine[0]))
                        { ++nextLine; }
                    // see how many newlines are after this line. If more than 1, then this is probably
                    // a bullet.
                    isEol(newLineStart, nextLine);
                    if (isEol.get_eol_count() > 1)
                        {
                        return std::make_pair(true, (current_char - text.data()));
                        }
                    return characters::is_character::is_numeric(nextLine[0]) ?
                        std::pair<bool, size_t>(true, (current_char - text.data())) :
                        // anything else means that this probably is not a numeric bullet
                        std::pair<bool,size_t>(false, 0);
                    }
                }
            /* ..or a letter bullet point. Note that we only count a single-digit,
               lowercased letter as a bullet point.
               Counting anything else could be a real word or (in the case of being uppercased) an initial.*/
            else if (characters::is_character::is_lower(current_char[0]) && current_char[1] != 0)
                {
                ++current_char;
                if (characters::is_character::is_period(current_char[0]) ||
                    // right parentheses
                    current_char[0] == 0x29 ||
                    current_char[0] == 0xFF09 ||
                    current_char[0] == L':')
                    {
                    // sometimes you might see "a.) Some info", so skip all expected punctuation
                    do
                        {
                        ++current_char;
                        } while (current_char[0] &&
                           (characters::is_character::is_period(current_char[0]) ||
                            current_char[0] == 0x29 ||
                            current_char[0] == 0xFF09));
                    return std::make_pair(true, (current_char - text.data()));
                    }
                // anything else means that this probably is not a letter bullet
                else
                    {
                    return std::make_pair(false, 0);
                    }
                }
            return std::make_pair(false, 0);
            }
        };

    /** @brief Determines if a position in a block of text is the end of a sentence.*/
    class is_end_of_sentence
        {
    public:
        explicit is_end_of_sentence(const bool sentence_start_must_be_uppercased) noexcept :
            m_sentence_start_must_be_uppercased(sentence_start_must_be_uppercased)
            {}
        is_end_of_sentence() = delete;
        /** @returns @c true if a position in a block of text is the end of a sentence.
            @param text The block of text to analyze.
            @param length The length of the text being analyzed.
            @param current_position The position in the text (usually a period) to analyze.
            @param previous_word_position The position in the text of the start of the word in
                front of the period we are analyzing.
            @param sentence_position_of_previous_word The position in the sentence
                of the proceeding word.*/
        [[nodiscard]]
        bool operator()(const wchar_t* text,
                        const size_t length,
                        size_t current_position,
                        const size_t previous_word_position,
                        const size_t sentence_position_of_previous_word)
            {
            assert(current_position < length);
            assert(previous_word_position < current_position);
            const size_t original_position = current_position;

            /* if it's a number and first word in sentence then it is probably a bullet point,
               so ignore it and treat it like part of the sentence*/
            if (sentence_position_of_previous_word == 0)
                {
                // check each digit to verify that they are a number or period
                if (std::wstring_view{ text + previous_word_position,
                                       current_position - previous_word_position }
                        .find_first_not_of(common_lang_constants::NUMBERS_AND_DOT)
                                                        == std::wstring_view::npos)
                    { return false; }
                }

            if (can_character_end_sentence_passive(text[current_position]) )
                {
                // end of text, so definitely ends sentence
                if (current_position == (length-1) )
                    { return true; }
                // watch out for ill-formatted quotes in the middle of a sentence
                // (e.g., "'hi there',! and 'goodbye',! he said.").
                if (current_position > 0 &&
                    traits::case_insensitive_ex::eq(text[current_position-1], L','))
                    { return false; }
                /* special case with colon; treat it like the end of a sentence if followed
                   by a newline. This is the assumption that it is proceeding a list of some sort.*/
                if (traits::case_insensitive_ex::eq(text[current_position], L':'))
                    {
                    // eat up "regular" whitespace (space or tab) after colon first
                    size_t i;
                    for (i = current_position+1; i < length; ++i)
                        {
                        if (!characters::is_character::is_space_horizontal(text[i]) )
                            { break; }
                        }
                    // if the rest of the document is whitespace then assume end of sentence
                    if (i >= length)
                        { return true; }
                    return characters::is_character::is_space_vertical(text[i]);
                    }
                /* ...or special case with dash (not hyphens, but actual long dashes)
                   followed by a double quote and then a hard return.
                   This happens with dialog that "trails" off at the end of a paragraph.
                   For example:
                   
                   "Why don't we—"
                   Suddenly, others entered the room.
                   
                   This will be two sentences.*/
                else if (characters::is_character::is_dash(text[current_position]) )
                    {
                    return (current_position+2 < length &&
                            characters::is_character::is_double_quote(text[current_position+1]) &&
                            characters::is_character::is_space_vertical(text[current_position+2]));
                    }
                /* Watch out for lines in a Table of Contents, where there is a space,
                   just one dot, and the page number.
                   For example:

                   "Welcome to WIC . 13"

                   Note that if there is no space before the period,
                   then we will not do this check,
                   that would lead to a false positive more often than not.*/
                if ((characters::is_character::is_period(text[current_position]) ||
                    text[current_position] == common_lang_constants::ELLIPSE) &&
                    current_position > 0 &&
                    characters::is_character::is_space_horizontal(text[current_position-1]))
                    {
                    size_t i = current_position+1;
                    // skip any spaces (real spaces, not newlines)
                    while (i < length &&
                        characters::is_character::is_space_horizontal(text[i]))
                        { ++i; }
                    // if the next word starts with a number, then it might be a page number
                    if (i < length && is_character.is_numeric(text[i]))
                        {
                        // skip any more text that is part of the number (possible page number)
                        while (i < length && !characters::is_character::is_space(text[i]))
                            { ++i; }
                        // this looks like a single-dot TOC line
                        if (i < length &&
                            (text[i] == L'\n' || text[i] == L'\r'))
                            { return false; }
                        }
                    }
                // special case for ellipse, interrobangs, or multiple exclamation points.
                // skip the extra punctuation.
                if (can_character_end_sentence_strict(text[current_position + 1]) ||
                    (current_position + 2 < length &&
                     characters::is_character::is_space(text[current_position + 1]) &&
                     can_character_end_sentence_strict(text[current_position + 2])) )
                    {
                    // eat up any more sentence-ending punctuation
                    size_t i(0);
                    size_t periodCount = characters::is_character::is_period(text[current_position]) ? 1 : 0;
                    size_t ellipsesCount = (text[current_position] == common_lang_constants::ELLIPSE) ? 1 : 0;
                    size_t lastSentenceEndingPunctuation = current_position;
                    for (i = current_position+1; i < length; ++i)
                        {
                        if (characters::is_character::is_space(text[i]))
                            { continue; }
                        else if (!can_character_end_sentence_strict(text[i]))
                            { break; }
                        else if (characters::is_character::is_period(text[i]))
                            { ++periodCount; }
                        else if (text[i] == common_lang_constants::ELLIPSE)
                            { ++ellipsesCount; }
                        lastSentenceEndingPunctuation = i;
                        }
                    // if the rest of the document is whitespace then assume end of sentence
                    if (i >= length)
                        { return true; }
                    // "words. . . . This is a new sentence." is fine, this would appear in a
                    // quotation block that is paraphrasing.
                    // But more than 4 dots would be something like a line in a Table of Contents,
                    // where there is a space between the last dot and the page number.
                    // For example:
                    // "Welcome to WIC........ 13"
                    if (periodCount > 4 || ellipsesCount > 1)
                        { return false; }
                    // move the current (sentence-ending) punctuation to the
                    // last one in the sequence
                    current_position = lastSentenceEndingPunctuation;
                    }
                // "word1. (word2", will be an end of sentence
                // (regardless of case), unless word1 is an abbreviation/acronym/initial.
                if (characters::is_character::is_space_horizontal(text[current_position+1]))
                    {
                    size_t nextNonSpace = current_position + 1;
                    while (nextNonSpace < length &&
                           characters::is_character::is_space_horizontal(text[nextNonSpace]))
                        { ++nextNonSpace; }
                    if (nextNonSpace+1 < length &&
                        traits::case_insensitive_ex::eq(text[nextNonSpace], L'('))
                        {
                        return
                            !(isAbbreviation({ text + previous_word_position,
                                               (original_position - previous_word_position) + 1 }) ||
                                isAcronym({ text + previous_word_position,
                                            (current_position - previous_word_position) + 1 }) ||
                             ((current_position-previous_word_position) == 1 &&
                               characters::is_character::is_alpha(text[previous_word_position])) );
                        }
                    }
                // citation (superscripted number) after a sentence? Skip over that.
                if (current_position + 1 < length &&
                    string_util::is_superscript_number(text[current_position + 1]))
                    {
                    auto nextNonCitationMarkChar = current_position + 1;
                    while (nextNonCitationMarkChar < length &&
                           string_util::is_superscript_number(text[nextNonCitationMarkChar]))
                        {
                        ++nextNonCitationMarkChar;
                        }
                    // move forward to the last character in the citation
                    current_position = nextNonCitationMarkChar - 1;
                    }
                else if (current_position + 1 < length &&
                         string_util::is_trademark_or_registration(text[current_position + 1]))
                    {
                    ++current_position;
                    }

                // At the end of the text? Assume this is a sentence end.
                if (current_position+1 >= length)
                    { return true; }
                // curved left arrow, used for marking the end of footnote.
                else if (text[current_position+1] == 8617)
                    { return true; }
                /* character matches, so now verify that the following text can start a sentence
                   (at least one space should follow)*/
                else if (characters::is_character::is_space(text[current_position+1]) ||
                    can_character_begin_or_end_parenthetical_or_quoted_section(text[current_position+1]))
                    {
                    // special case with "etc.".
                    if (previous_word_position+3 < length &&
                        characters::is_character::is_period(text[previous_word_position+3]) &&
                        // Latin "etc."
                        ((traits::case_insensitive_ex::eq(text[previous_word_position], L'e') &&
                         traits::case_insensitive_ex::eq(text[previous_word_position+1], L't') &&
                         traits::case_insensitive_ex::eq(text[previous_word_position+2], L'c')) ||
                         // or German "und so weiter"
                         (traits::case_insensitive_ex::eq(text[previous_word_position], L'u') &&
                         traits::case_insensitive_ex::eq(text[previous_word_position+1], L's') &&
                         traits::case_insensitive_ex::eq(text[previous_word_position+2], L'w')) ) )
                        {
                        // if inside of parentheses, then it is part of current sentence.
                        if (can_character_begin_or_end_parenthetical_or_quoted_section(text[current_position+1]))
                            { return false; }
                        // it's followed by a space, then if followed by lowercased word then
                        // definitely part of the current sentence.
                        else
                            {
                            if ((current_position+2 < length &&
                                 is_character.is_lower(text[current_position+2])) ||
                                (current_position+3 < length &&
                                 characters::is_character::is_space(text[current_position+2]) &&
                                 is_character.is_lower(text[current_position+3])) )
                                { return false; }
                            }
                        }
                    // Now verify that the next character can begin a sentence
                    // first, if the .?!: is followed by a CRLF or form feed and not an abbreviation then
                    // it's most likely end of sentence
                    if (characters::is_character::is_space_vertical(text[current_position+1]))
                        { return true; }
                    // eat up extra (secondary) parenthesis or quote if present. Rare, but it happens
                    if (current_position+2 < length &&
                        can_character_begin_or_end_parenthetical_or_quoted_section(text[current_position+2]))
                        { ++current_position; }
                    // eat up whitespace up to the next word
                    size_t i(0);
                    bool nonWordPunctEncountered = false;
                    for (i = current_position+2; i < length; ++i)
                        {
                        if (!characters::is_character::is_space(text[i]) )
                            {
                            // skip any "garbage" punctuation in between words
                            if (!is_character.can_character_begin_word(text[i]))
                                {
                                nonWordPunctEncountered = true;
                                continue;
                                }
                            // found a legit character to start the next word...
                            else
                                { break; }
                            }
                        // if the .?!: is followed by a CRLF and not an abbreviation
                        // then it's most likely end of sentence
                        if (characters::is_character::is_space_vertical(text[i]))
                            { return true; }
                        }
                    // if the rest of the document is whitespace then assume end of sentence
                    // (even if this is last word is an abbreviation or acronym).
                    if (i >= length)
                        { return true; }

                    // if the word is just one letter then it is probably an initial.
                    // A single number or non-letter at the end of a sentence is OK.
                    if ((original_position-previous_word_position) == 1 &&
                        characters::is_character::is_alpha(text[previous_word_position]) &&
                        characters::is_character::is_period(text[original_position]))
                        { return false; }
                    // Special case with "no. of." (number of).
                    // We can't make "no." an abbreviation because it is such a
                    // common word that can end a sentence.
                    // Note that "of" is a case sensitive comparison, so "No. Of course not."
                    // would be seen as two sentences.
                    // Note that we also treat "no. #" and "no. [0-9]" as an abbreviation.
                    else if ((previous_word_position+2 < length &&
                             traits::case_insensitive_ex::eq(text[previous_word_position], L'n') &&
                             traits::case_insensitive_ex::eq(text[previous_word_position+1], L'o') &&
                             characters::is_character::is_period(text[previous_word_position+2])) &&
                                ((i+2 < length &&
                                 string_util::full_width_to_narrow(text[i]) == L'o' &&
                                 string_util::full_width_to_narrow(text[i+1]) == L'f' &&
                                 string_util::full_width_to_narrow(text[i+2]) == L' ') ||
                                 (string_util::full_width_to_narrow(text[i]) == L'#') ||
                                 is_character.is_numeric(text[i])) )
                        { return false; }
                    // "Vs." can never start a sentence, so if a word is in front of it that
                    // ends with a period, then it must be an unknown abbreviation
                    else if (i+2 < length &&
                             traits::case_insensitive_ex::eq(text[i], L'v') &&
                             traits::case_insensitive_ex::eq(text[i+1], L's') &&
                             characters::is_character::is_period(text[i+2]))
                        { return false; }
                    // Special logic for month name ambiguous abbreviations.
                    // Only see them as a month if followed by a number.
                    else if ((current_position-previous_word_position) == 3 &&
                        // "Jan." can be a person's name or an abbreviation for January
                        ((string_util::full_width_to_narrow(text[previous_word_position]) == L'J' &&
                            traits::case_insensitive_ex::eq(text[previous_word_position+1], L'a') &&
                            traits::case_insensitive_ex::eq(text[previous_word_position+2], L'n')) ||
                        // "Mar." can be March abbreviation or "Sea" in Spanish
                        (string_util::full_width_to_narrow(text[previous_word_position]) == L'M' &&
                            traits::case_insensitive_ex::eq(text[previous_word_position+1], L'a') &&
                            traits::case_insensitive_ex::eq(text[previous_word_position+2], L'r'))) )
                        { return !is_character.is_numeric(text[i]); }
                    // see if it's just an abbreviation or acronym
                    else if (isAbbreviation({ text+previous_word_position,
                                (original_position-previous_word_position) + 1 }) )
                        { return false; }
                    else if (isAcronym.is_dotted_acronym(
                            { text + previous_word_position,
                              (current_position - previous_word_position) + 1 }))
                        { return false; }
                    /* "Hello." he said (this is one sentence)
                       "Hello." Blake said (this is two sentences)
                       "Hello." -unknown quote (this is two sentences)
                       Note, this quote scenario overrides the
                       'sentence_start_must_be_uppercased' flag.
                       Sentence termination inside of a quote is tricky,
                       so we need to have special logic for that.*/
                    else if (can_character_begin_or_end_parenthetical_or_quoted_section(text[current_position+1]) &&
                        // if following word is lowercase
                        characters::is_character::is_lower(text[i]) &&
                        // and there is no punctuation between quote and word (like a dash)
                        nonWordPunctEncountered == false)
                        { return false; }
                    else if (const auto ellipsisRes = is_character.is_ellipsis(
                                 { text + original_position,
                                   (current_position - original_position) + 1 });
                        ellipsisRes.first)
                        {
                        // three dots followed immediately by ? or ! is an end of sentence
                        if (ellipsisRes.second == 3 &&
                            is_non_period_terminator(text[current_position]) )
                            { return true; }
                        // if three dots and the following text is a letter or number (regardless of case)
                        // or ? or !, then consider this an ellipsis mid sentence.
                        else if (ellipsisRes.second == 3 &&
                            (characters::is_character::is_alpha(text[i]) ||
                             characters::is_character::is_numeric_simple(text[i])) )
                            { return false; }
                        return (can_character_begin_sentence(text[i]) &&
                                // OK for first word to be lowered if this was a block quotation where
                                // the sentence ended and was followed by an ellipsis and then a new
                                // sentence that was paraphrased (the front of it was cut off). In that case,
                                // the following (paraphrased) text will be a new sentence.
                                (ellipsisRes.second == 4 || !characters::is_character::is_lower(text[i])) &&
                                nonWordPunctEncountered == false);
                        }
                    // see if the next character can start a sentence
                    else if (can_character_begin_sentence(text[i]) )
                        { return true; }
                    else
                        { return false; }
                    }
                // must assume inside of a quotation or unknown abbreviation
                else
                    { return false; }
                }
            else
                { return false; }
            }
        /** @returns @c true if a character can start a sentence.
                This can be a letter, left parenthesis, dash, or quote.
            @param ch The character to review.*/
        [[nodiscard]]
        inline bool can_character_begin_sentence(const wchar_t ch) const noexcept
            {
            return ((m_sentence_start_must_be_uppercased && is_character.can_character_begin_word_uppercase(ch)) ||
                    (!m_sentence_start_must_be_uppercased && is_character.can_character_begin_word(ch)) ||
                    // or is a left parenthesis (sometimes sentences start with these)
                    traits::case_insensitive_ex::eq(ch, 40) ||
                    // or various hyphens/dashes, sometimes follows a quote from a person
                    is_character.is_dash_or_hyphen(ch) ||
                     // or if simply a quote symbol
                    is_character.is_quote(ch) );
            }
        /** @returns @c true if a character can be valid sentence-ending punctuation,
                other than period and ellipsis.
            @param character The character to review.
            @note This variation only returns true if character is a standard
                stop character (e.g., '?' or '.') or something like ':' or
                ellipse followed by a hard return.*/
        [[nodiscard]]
        inline static bool is_non_period_terminator(const wchar_t character) noexcept
            {
            return (traits::case_insensitive_ex::eq(character, L'!') ||
                    traits::case_insensitive_ex::eq(character, L'?') ||
                    character == 0x85 || /*horizontal ellipse on Windows*/
                    character == common_lang_constants::INTERROBANG || // interrobang
                    // special case when a colon is at the end of a line
                    traits::case_insensitive_ex::eq(character, L':'));
            }
        /** @returns @c true if a character can be valid sentence-ending punctuation.
            @param character The character to review.
            @note This variation only returns @c true if character is a
                standard stop character (e.g., '?' or '.')
                or something like ':' or ellipse followed by a hard return.*/
        [[nodiscard]]
        inline static bool can_character_end_sentence_strict(const wchar_t character) noexcept
            {
            return (characters::is_character::is_period(character) ||
                    traits::case_insensitive_ex::eq(character, L'!') ||
                    traits::case_insensitive_ex::eq(character, L'?') ||
                    character == 0x85 || /*horizontal ellipse on Windows*/
                    character == common_lang_constants::ELLIPSE/*Unicode horizontal ellipse*/ ||
                    character == common_lang_constants::INTERROBANG || // interrobang
                    // special case when a colon is at the end of a line
                    traits::case_insensitive_ex::eq(character, L':'));
            }
        /** @returns @c true if a character can be valid sentence-ending punctuation.
            @param character The character to review.
            @note This variation returns true if character is a standard stop character (e.g., '?' or '.')
                or something like ':' or ellipse followed by a hard return, or special characters that must be followed
                by something else (e.g., a dash which later must be determined to be followed by a quote).
                This function should only be used if you plan to further review the character
                (e.g., by calling operator()).*/
        [[nodiscard]]
        inline static bool can_character_end_sentence_passive(const wchar_t character) noexcept
            {
            return (can_character_end_sentence_strict(character) || characters::is_character::is_dash(character));
            }
        /** @returns @c true if a character can be a valid beginning or ending to a
                parenthetical section following a sentence.
            @param character The character to review.*/
        [[nodiscard]]
        inline static bool can_character_begin_or_end_parenthetical_or_quoted_section(
            const wchar_t character) noexcept
            {
            return (traits::case_insensitive_ex::eq(character, L'(') ||
                    traits::case_insensitive_ex::eq(character, L')') ||
                    traits::case_insensitive_ex::eq(character, L'[') ||
                    traits::case_insensitive_ex::eq(character, L']') ||
                    traits::case_insensitive_ex::eq(character, L'{') ||
                    traits::case_insensitive_ex::eq(character, L'}') ||
                    traits::case_insensitive_ex::eq(character, L'<') ||
                    traits::case_insensitive_ex::eq(character, L'>') ||
                    traits::case_insensitive_ex::eq(character, L'^') ||
                    characters::is_character::is_dash_or_hyphen(character) ||
                    characters::is_character::is_quote(character));
            }
    private:
        grammar::is_abbreviation isAbbreviation;
        grammar::is_acronym isAcronym;
        characters::is_character is_character;
        bool m_sentence_start_must_be_uppercased{ true };
        };

    /** @brief Types of sentences/paragraphs.*/
    enum class sentence_paragraph_type
        {
        header,     /*!< A header. */
        list_item,  /*!< A bullet or list item in a list. */
        incomplete, /*!< A sentence which does not end with a period, question mark,
                         or exclamation point. Also, is not a header or list item. */
        complete    /*!< A sentence that has proper terminating punctuation. */
        };

    /** @brief Structure describing a sentence.*/
    class sentence_info
        {
    public:
        sentence_info(const size_t begin_index,
                      const size_t end_index,
                      const wchar_t ending_punctuation) noexcept
            : m_begin_index(begin_index), m_end_index(end_index),
              m_size((end_index - begin_index)+1),
              m_valid_size((end_index - begin_index) + 1),
              m_ending_punctuation(ending_punctuation),
              // sentence will have at least one unit
              m_unit_count(1)
            {
            m_is_valid = ends_with_valid_punctuation();
            m_sentence_type = m_is_valid ?
                sentence_paragraph_type::complete : sentence_paragraph_type::incomplete;
            }
        sentence_info() = delete;
        [[nodiscard]]
        bool operator==(const size_t word_position) const noexcept
            { return word_position >= m_begin_index && word_position <= m_end_index; }
        [[nodiscard]]
        bool operator!=(const size_t word_position) const noexcept
            { return word_position <= m_begin_index || word_position >= m_end_index; }
        [[nodiscard]]
        bool operator<(const size_t word_position) const noexcept
            { return m_begin_index < word_position; }
        [[nodiscard]]
        bool operator>(const size_t word_position) const noexcept
            { return m_begin_index > word_position; }
        /// @brief Used for sentences to compare each other based on length,
        ///     so that statistics function (e.g., max_element) will work.
        /// @param that The other sentence to compare against.
        [[nodiscard]]
        bool operator<(const sentence_info& that) const noexcept
            { return m_size < that.m_size; }
        /** @returns The word position that this sentence starts at in the document.*/
        [[nodiscard]]
        size_t get_first_word_index() const noexcept
            { return m_begin_index; }
        /** @returns The word position that this sentence ends at in the document.*/
        [[nodiscard]]
        size_t get_last_word_index() const noexcept
            { return m_end_index; }
        /** @returns The number of words in the sentence, which also includes invalid words.*/
        [[nodiscard]]
        size_t get_word_count() const noexcept
            { return m_size; }
        /** @brief Sets the valid word count of the sentence.
            @param validCount The valid word count value.*/
        void set_valid_word_count(const size_t validCount) noexcept
            { m_valid_size = validCount; }
        /** @returns The valid number of words in the sentence,
                which is the words being included in the analysis.*/
        [[nodiscard]]
        size_t get_valid_word_count() const noexcept
            { return m_valid_size; }
        /** @returns The trailing punctuation mark of the sentence.*/
        [[nodiscard]]
        wchar_t get_ending_punctuation() const noexcept
            { return m_ending_punctuation; }
        /// @brief Sets the ending punctuation.
        /// @param punct The character to use as the terminating punctuation.
        void set_ending_punctuation(wchar_t punct) noexcept
            { m_ending_punctuation = punct; }
        /** @returns Whether the ending punctuation of the sentence is
                what can really end a sentence.\n
                This will include punctuation used for passive EOS deduction (e.g., dashes).*/
        [[nodiscard]]
        inline bool ends_with_valid_punctuation() const noexcept
            {
            return (is_end_of_sentence::can_character_end_sentence_passive(m_ending_punctuation));
            }
        /** @returns Whether the sentence is considered valid by the analysis.*/
        [[nodiscard]]
        bool is_valid() const noexcept
            { return m_is_valid; }
        /** @brief Call this is you need to override the default logic of a sentence being
                considered complete.
            @details By default, this is based on it ending with valid punctuation.
            @note If type was incomplete and is now being set to valid,
                then the type will be set to complete, and vice versa.\n
                If the type is something other than complete or incomplete (e.g., header)
                then the type will remain the same.
            @param valid Whether to make the sentence valid or not.*/
        void set_valid(const bool valid) noexcept
            {
            m_is_valid = valid;
            // If is valid now, but previous type was incomplete, then set it to complete.
            // If it was a header or list item, then just leave the type alone.
            if (is_valid() && get_type() == sentence_paragraph_type::incomplete)
                { set_type(sentence_paragraph_type::complete); }
            // ...and vice versa.
            else if (!is_valid() && get_type() == sentence_paragraph_type::complete)
                { set_type(sentence_paragraph_type::incomplete); }
            }
        /** @brief Sets what type of sentence this is in terms of being complete or header, etc...
            @param type The type of sentence this is.*/
        void set_type(sentence_paragraph_type type) noexcept
            { m_sentence_type = type; }
        /** @returns What type of sentence this is in terms of being complete or header, etc...*/
        [[nodiscard]]
        sentence_paragraph_type get_type() const noexcept
            { return m_sentence_type; }
        /** @brief Sets the unit count of the sentence.
            @param count The unit count value.*/
        void set_unit_count(const size_t count) noexcept
            { m_unit_count = count; }
        /** @returns The unit count of the sentence.
                Each section of a sentence divided by dashes is a unit.*/
        [[nodiscard]]
        size_t get_unit_count() const noexcept
            { return m_unit_count; }
    private:
        size_t m_begin_index{ 0 };
        size_t m_end_index{ 0 };
        size_t m_size{ 0 };
        size_t m_valid_size{ 0 };
        wchar_t m_ending_punctuation{ L'.' };
        bool m_is_valid{ true };
        sentence_paragraph_type m_sentence_type{ sentence_paragraph_type::complete };
        // sentence will have at least one unit
        size_t m_unit_count{ 1 };
        };

    class sentence_word_position_less_than
        {
    public:
        [[nodiscard]]
        inline bool operator()(const sentence_info& sent1,
                               const sentence_info& sent2) const noexcept
            { return (sent1.get_first_word_index() < sent2.get_first_word_index()); }
        };

    /** @brief Sentence length comparison functor for `std::count_if`.*/
    class sentence_length_greater_than
        {
    public:
        /** @brief Constructor.
            @param count The number of words that a sentence has to exceed to compare as bigger.*/
        explicit sentence_length_greater_than(const size_t count) noexcept :
            m_count(count) {}
        sentence_length_greater_than() = delete;
        /** @returns @c true if a sentence has more words than the predefined value.
            @param the_sentence The sentence to review.*/
        [[nodiscard]]
        inline bool operator()(const sentence_info& the_sentence) const noexcept
            { return the_sentence.get_word_count() > m_count; }
    private:
        size_t m_count{ 0 };
        };

    /** @brief Complete sentence length comparison functor for std::count_if.*/
    class complete_sentence_length_greater_than
        {
    public:
        /** @brief Constructor.
            @param count The valid number of words that a sentence has to
                exceed to compare as bigger.*/
        explicit complete_sentence_length_greater_than(const size_t count) noexcept :
            m_count(count)
            {}
        complete_sentence_length_greater_than() = delete;
        /** @returns @c true if a valid sentence has more valid words than the predefined value.
            @param the_sentence The sentence to review.*/
        [[nodiscard]]
        inline bool operator()(const sentence_info& the_sentence) const noexcept
            {
            if (!the_sentence.is_valid() )
                { return false; }
            return the_sentence.get_valid_word_count() > m_count;
            }
    private:
        size_t m_count{ 0 };
        };

    /** @brief Complete sentence length comparison functor for std::max_element.*/
    class complete_sentence_length_less
        {
    public:
        /** @returns @c true if first sentence has less valid words in it then the second sentence.
            @note If either sentence is invalid, then they are considered
                less than a valid sentence.
            @param sent1 The first sentence to compare.
            @param sent2 The second sentence to compare.*/
        [[nodiscard]]
        bool operator()(const sentence_info& sent1, const sentence_info& sent2) const noexcept
            {
            // if both sentences are incomplete then they are "equal"
            if (!sent1.is_valid() && !sent2.is_valid())
                { return false; }
            // ...if left sentence is incomplete then it is less than the other
            else if (!sent1.is_valid())
                { return true; }
            // ...otherwise, if left is complete and right is not then it must be bigger
            else if (!sent2.is_valid())
                { return false; }
            else
                { return sent1.get_valid_word_count() < sent2.get_valid_word_count(); }
            }
        };
    }

/** @}*/

#endif //__SENTENCE_H__
