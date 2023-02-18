/** @addtogroup Indexing
    @brief Classes for parsing and indexing text.
    @date 2003-2020
    @copyright Oleander Software, Ltd.
    @author Blake Madden
    @details This program is free software; you can redistribute it and/or modify
     it under the terms of the 3-Clause BSD License.

     SPDX-License-Identifier: BSD-3-Clause
* @{*/

#ifndef __PUNCTUATION_H__
#define __PUNCTUATION_H__

#include "characters.h"
#include "character_traits.h"

/// @brief Namespace for punctuation classes.
namespace punctuation
    {
    /** @brief Extended version of ispunct, which takes into account special characters.*/
    class is_punctuation
        {
    public:
        [[nodiscard]] constexpr bool operator()(const wchar_t ch) const noexcept
            { return characters::is_character::is_punctuation(ch); }
        };

    /** @brief Punctuation counting functor utility.*/
    class punctuation_count
        {
    public:
        /** Counts and returns the number of punctuation marks from character stream.
            This is an advanced version of ispunct that takes other Unicode punctuation into account,
            as well as special logic for puncutation only words.
            @param text The character stream to analyze.
            @param length The length of the text being reviewed.
            @returns The number of punctuation marks from character stream.
            @note If the stream is just a single ampersand/pound/at, then it is treated
                  as NOT being punctuation, but rather a word.*/
        [[nodiscard]] inline size_t operator()(const wchar_t* text, const size_t length) const
            {
            if (!text)
                { return 0; }
            /* special case where the word is just '&', '#', or '@';
               treat it like a regular character (e.g., not punctuation)*/
            if (length == 1 &&
                string_util::is_one_of(string_util::full_width_to_narrow(text[0]), L"&#@"))
                { return 0; }
            return std::count_if(text, text+length, characters::is_character::is_punctuation);
            }
        };

    /** @brief Punctuation mark class. Used in tokenizer to keep track of punctuation marks in between words.*/
    class punctuation_mark
        {
    public:
        /** @brief Constructor.
            @param mark The punctuation mark.
            @param wordPosition The position into the parent document of this mark's following word.
            @param isConnectedToPreviousWord True if this mark is connected to the word proceeding it (e.g., an apostrophe on a plural possessive word).*/
        punctuation_mark(const wchar_t mark,
                         const size_t wordPosition,
                         const bool isConnectedToPreviousWord) noexcept :
            m_mark(mark), m_word_position(wordPosition),
            m_is_connected_to_previous_word(isConnectedToPreviousWord)
            {}
        punctuation_mark() noexcept = default;
        /// @returns The word position (in the document) that this punctuation is in front of.
        [[nodiscard]] size_t get_word_position() const noexcept
            { return m_word_position; }
        /// @returns The actual mark (character).
        [[nodiscard]] wchar_t get_punctuation_mark() const noexcept
            { return m_mark; }
        /// @returns Whether this punctuation is connected to the previous word (e.g., a quote symbol), rather than being in front of the current word.
        [[nodiscard]] bool is_connected_to_previous_word() const noexcept
            { return m_is_connected_to_previous_word; }
        /** Useful for doing a binary search/sorting on a collection of marks, based on word position.
            @note We compare against word position (instead of the actual mark) so that we can sort a collection
                  of punctuation marks by the order that they appear in the document.
            @param that The punctuation mark to compare against.
            @returns @c true if this mark's word position is less than that's word position.*/
        [[nodiscard]] bool operator<(const punctuation_mark& that) const noexcept
            { return m_word_position < that.m_word_position; }
        /// @brief Compare against a character as a punctuation mark.
        /// @param mark The character to compare against.
        /// @returns @c true if marks are equal.
        [[nodiscard]] constexpr bool operator==(const wchar_t mark) const noexcept
            { return m_mark == mark; }
    private:
        // mark is in front of this word
        wchar_t m_mark{ 32 }; // space, just generic default type
        size_t m_word_position{ 0 };
        bool m_is_connected_to_previous_word{ false };
        };
    }

/** @}*/

#endif //__PUNCTUATION_H__
