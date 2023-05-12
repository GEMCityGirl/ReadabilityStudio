/** @addtogroup Indexing
    @brief Classes for parsing and indexing text.
    @date 2005-2023
    @copyright Oleander Software, Ltd.
    @author Blake Madden
    @details This program is free software; you can redistribute it and/or modify
     it under the terms of the 3-Clause BSD License.

     SPDX-License-Identifier: BSD-3-Clause
* @{*/

#ifndef __ENGLISH_SYLLABLE_H__
#define __ENGLISH_SYLLABLE_H__

#include <set>
#include "character_traits.h"
#include "characters.h"
#include "../OleanderStemmingLibrary/src/common_lang_constants.h"
#include "../Wisteria-Dataviz/src/i18n-check/src/string_util.h"

namespace grammar
    {
    /// @brief Base syllable counting functor class.
    class base_syllabize
        {
    public:
        base_syllabize() noexcept : m_syllable_count(0),
            m_length(0),
            m_previous_vowel(0),
            m_previous_block_vowel(0)
            {}
        base_syllabize(const base_syllabize& that) = delete;
        base_syllabize(base_syllabize&& that) = delete;
        base_syllabize& operator=(const base_syllabize& that) = delete;
        base_syllabize& operator=(base_syllabize&& that) = delete;
        virtual ~base_syllabize()
            {}
        virtual size_t operator()(const wchar_t* start, const size_t length) = 0;
    protected:
        void reset() noexcept
            { m_syllable_count = m_length = m_previous_vowel = m_previous_block_vowel = 0; }
        /** Special case mathematical terms that need to be counted differently.
            @param start The word to analyze.
            @param length The length of the word.
            @returns A pair indicating whether or not this is a special case, and if so the syllable count.*/
        [[nodiscard]]
        static std::pair<bool,size_t> is_special_math_word(const wchar_t* start, const size_t length) noexcept;
        /** Determines the number of syllables for a numeric string.
            @param numeral_string The string to analyze.
            @param end_of_string The sentinel of the main string (can't read beyond that point).
            @param[out] characters_counted The number of characters in the string that were a numeric value
            @param thousands_separator The character used for the thousands separator.
            @param decimal_separator The character used for the decimal separator.
            @returns The number of syllables for a numeric string.*/
        template <typename Tsyllabize_number>
        [[nodiscard]]
        static size_t syllabify_numeral(const wchar_t* numeral_string,
                                               const wchar_t* end_of_string,
                                               size_t& characters_counted,
                                               const wchar_t thousands_separator,
                                               const wchar_t decimal_separator) noexcept
            {
            assert(numeral_string);
            assert(end_of_string);
            characters_counted = 0;
            if (numeral_string == nullptr || end_of_string == nullptr)
                { return 0; }
            Tsyllabize_number syllabify_number;
            const wchar_t* const start = numeral_string;
            size_t syllableCount = 0;
            while (numeral_string[0] && numeral_string < end_of_string)
                {
                // skip thousands separator
                if (traits::case_insensitive_ex::eq(numeral_string[0], thousands_separator))
                    {
                    ++numeral_string;
                    continue;
                    }
                // decimal separator counts as one syllable ("point" or "dot" in most languages)
                if (traits::case_insensitive_ex::eq(numeral_string[0], decimal_separator))
                    {
                    ++syllableCount;
                    ++numeral_string;
                    continue;
                    }
                else if (characters::is_character::is_numeric_simple(numeral_string[0]) )
                    { syllableCount += syllabify_number(numeral_string[0]); }
                else
                    { break; }
                ++numeral_string;
                }
            characters_counted = (numeral_string-start);
            return syllableCount;
            }
        /** @brief Syllabizes a symbol, dependent on its position in the word.
            @param start The start of the text under review.
            @param end The end of the text under review.
            @param current_char The current position (of the symbol) in the text under review to analyze.
            @returns The syllable count of the symbol.*/
        [[nodiscard]]
        static size_t get_symbol_syllable_count(const wchar_t* start,
                                                const wchar_t* end,
                                                const wchar_t* current_char) noexcept;
        /** @brief For possessive words (ape's), this will adjust the length to ignore the possessive part of the word.
            @note This adjustment is done by shortening the internal length variable,
                it does not change the text buffer.
            @param start The text being reviewed.*/
        void adjust_length_if_possesive(const wchar_t* start);
        /** If there are any periods in this word, then break it up into smaller words.
            @returns @c true if word was split and syllabized.
            @param start The start of the word.
            @param end The end of the word.*/
        template<typename Tsyllabizer>
        bool syllabize_if_contains_periods(const wchar_t* start, const wchar_t* end)
            {
            size_t periodCount = 0;
            auto periodPos = string_util::strncspn(start, m_length, L".\uFF0E\uFF61", 3);
            const wchar_t* period = (periodPos == m_length) ? nullptr : start+periodPos;
            if (period)
                {
                Tsyllabizer dotSyllabize;
                size_t separateSectionsSyllableCount = 0;
                size_t currentSectionLength = 0;
                const wchar_t* currentSection = start;
                while (period)
                    {
                    /* don't consider this a valid dot in the word if it is at the end and is the only
                       one in the word. In this case it would be an initial.*/
                    if (periodCount == 0 &&
                        (end-1) == period)
                        { /*NOOP*/ }
                    else
                        { ++periodCount; }
                    currentSectionLength = period-currentSection;
                    separateSectionsSyllableCount += dotSyllabize(currentSection, currentSectionLength);
                    currentSection = period+1;
                    periodPos = string_util::strncspn(++period, m_length-(currentSection-start), L".\uFF0E\uFF61", 3);
                    period = (periodPos == m_length-(currentSection-start) || m_length-(currentSection-start) == 0) ?
                        nullptr : period+periodPos;
                    }
                separateSectionsSyllableCount += dotSyllabize(currentSection, m_length-(currentSection-start));
                m_syllable_count = (separateSectionsSyllableCount > 0) ? separateSectionsSyllableCount : 1;
                /* if there was a single period in the word (that was not at the end) then
                   count it as "dot" or "point" like in a number. If more than one period then
                   this is more than likely an acronym and we don't count these.*/
                if (periodCount == 1)
                    { ++m_syllable_count; }
                return true;
                }
            else
                { return false; }
            }
        /** @brief If there are any dashes in this word, then break it up into smaller words.
            @returns @c true if word was split and syllabized.
            @param start The start of the word.*/
        template<typename Tsyllabizer>
        [[nodiscard]]
        bool syllabize_if_contains_dashes(const wchar_t* start)
            {
            auto dashPos = string_util::strncspn(start, m_length, L"-\uFF0D", 2);
            const wchar_t* dash = (dashPos == m_length) ? nullptr : start+dashPos;
            if (dash)
                {
                Tsyllabizer dashSyllabize;
                size_t separateSectionsSyllableCount = 0;
                size_t currentSectionLength = 0;
                const wchar_t* currentSection = start;
                while (dash)
                    {
                    currentSectionLength = dash-currentSection;
                    separateSectionsSyllableCount += dashSyllabize(currentSection, currentSectionLength);
                    currentSection = dash+1;
                    dashPos = string_util::strncspn(++dash, m_length-(currentSection-start), L"-\uFF0D", 2);
                    dash = (dashPos == m_length-(currentSection-start) || m_length-(currentSection-start) == 0) ?
                        nullptr : dash+dashPos;
                    }
                separateSectionsSyllableCount += dashSyllabize(currentSection, m_length-(currentSection-start));
                m_syllable_count = (separateSectionsSyllableCount > 0) ? separateSectionsSyllableCount : 1;
                return true;
                }
            else
                { return false; }
            }
        /** @brief Determines if a 'y' is a consonant.
            @param word The text begin examined.
            @param position Where the 'y' being examined is in the block of text.
            @returns @c true if the letter at "position" is a consonant 'y'.
            @bug benzoyl is broken*/
        [[nodiscard]]
        bool is_consonant_y(const wchar_t* word, size_t position) const;
        characters::is_character isChar;
        size_t m_syllable_count{ 0 };
        size_t m_length{ 0 };
        size_t m_previous_vowel{ 0 };
        size_t m_previous_block_vowel{ 0 };
        };

    /// @brief Counts the number of (English) syllables in a (single-digit) number.
    class syllabize_english_number
        {
    public:
        [[nodiscard]]
        constexpr size_t operator()(const wchar_t number) const noexcept
            {
            return ((number >= common_lang_constants::NUMBER_1 && number <= common_lang_constants::NUMBER_6) ||
                     (number >= common_lang_constants::NUMBER_1_FULL_WIDTH &&
                      number <= common_lang_constants::NUMBER_6_FULL_WIDTH)) ?
                    1 : ((number == common_lang_constants::NUMBER_8 || number == common_lang_constants::NUMBER_9) ||
                         (number == common_lang_constants::NUMBER_8_FULL_WIDTH ||
                          number == common_lang_constants::NUMBER_9_FULL_WIDTH)) ?
                    1 : ((number == common_lang_constants::NUMBER_0 || number == common_lang_constants::NUMBER_7) ||
                         (number == common_lang_constants::NUMBER_0_FULL_WIDTH ||
                          number == common_lang_constants::NUMBER_7_FULL_WIDTH)) ?
                    2 : 0;
            }
        };

    /// @brief English syllable counting functor utility.
    class english_syllabize final : public base_syllabize
        {
    public:
        /// Constructor.
        english_syllabize() noexcept :
                    m_was_last_vowel_block_separable_vowels(false),
                    m_ends_with_nt_contraction(false) {}
        /** @brief Counts the syllables in a block of text.
            @param start The start of the block of text.
            @param length The length of the text.
            @returns The number of syllables in the text.*/
        [[nodiscard]]
        size_t operator()(const wchar_t* start, const size_t length) final;
    private:
        /// @brief Analyzes the overall string for special situations that
        ///     they standard syllabizer would have missed.
        /// @param start The section of text to finalize.
        void finalize_special_cases(const wchar_t* start);
        /** @brief Determines if a 'u' at given position is silent.
            @param word The current word being analyzed (must be the start of the word).
            @param position The position of the vowel.
            @returns @c true if 'u' at @position is silent.*/
        [[nodiscard]]
        bool is_silent_u(const wchar_t* word, size_t position) const;
        //--------------------------------------------------
        [[nodiscard]]
        bool does_prefix_silence_e(const wchar_t* word, size_t prefix_length) const;
        //--------------------------------------------------
        [[nodiscard]]
        static bool does_prefix_disconnect_e(const wchar_t* word, size_t prefix_length);
        //--------------------------------------------------
        [[nodiscard]]
        static bool is_none_affecting_suffix(const wchar_t* word, size_t suffix_length);
        /// @returns @c true if suffix makes the preceding 'e' NOT silent
        [[nodiscard]]
        bool does_suffix_negate_silent_e(const wchar_t* suffix, const size_t suffix_length,
            const size_t next_vowel_index) const;
        /** @returns @c true if a single (non-'e') vowel (i.e., vowel surrounded by consonants) should not
             be seen as a syllable break. There are fairly uncommon.
            @param word The current word being analyzed (must be the start of the word).
            @param position The position of the vowel.
            @note Do not use this for 'e's; is_silent_e() should be called for those.*/
        [[nodiscard]]
        bool is_single_non_e_vowel_ignored(const wchar_t* word, const size_t position) const;
        /** @brief Determines if an 'e' at given position is silent.
            @param word The current word being analyzed (must be the start of the word).
            @param position The position of the vowel.
            @returns @c true if 'e' at @position is silent.*/
        [[nodiscard]]
        bool is_silent_e(const wchar_t* word, const size_t position) const;
        //--------------------------------------------------
        [[nodiscard]]
        bool is_vowels_separate_syllables(const wchar_t* word,
                                          const size_t position, const size_t vowel_block_size,
                                          const bool is_first_vowel_block_in_word) const;
        /** @returns @c true if a consonant block can be joined with the proceeding vowels to form a single syllable.*/
        [[nodiscard]]
        static bool can_consonants_end_sound(const wchar_t* consonants, size_t block_length);
        //--------------------------------------------------
        [[nodiscard]]
        static bool can_consonants_be_modified_by_following_e(const wchar_t* consonants, size_t block_length);
        //--------------------------------------------------
        [[nodiscard]]
        static bool can_consonants_begin_sound(const wchar_t* consonants, size_t block_length);
        /// Sees if a word begins with a special prefixes which should always end as a syllable division.
        /// @returns A pair with the syllable count and length of the prefix
        [[nodiscard]]
        static std::pair<size_t,size_t> get_prefix_length(const wchar_t* start, const size_t length);

        static std::set<traits::case_insensitive_wstring_ex> m_e_disconnecting_prefixes;
        static std::set<traits::case_insensitive_wstring_ex> m_non_affecting_suffixes_4;

        bool m_was_last_vowel_block_separable_vowels{ false };
        bool m_ends_with_nt_contraction{ false };
        };
    }

/** @}*/

#endif // __ENGLISH_SYLLABLE_H__
