/** @addtogroup Indexing
    @brief Classes for parsing and indexing text.
    @date 2005-2023
    @copyright Oleander Software, Ltd.
    @author Blake Madden
    @details This program is free software; you can redistribute it and/or modify
     it under the terms of the 3-Clause BSD License.

     SPDX-License-Identifier: BSD-3-Clause
* @{*/

#ifndef __SPANISH_SYLLABLE_H__
#define __SPANISH_SYLLABLE_H__

#include "syllable.h"
#include <utility>

namespace grammar
    {
    /// @brief Counts the number of (Spanish) syllables in a (single-digit) number.
    class syllabize_spanish_number
        {
    public:
        [[nodiscard]]
        constexpr size_t operator()(const wchar_t number) const noexcept
            {
            return ((number == common_lang_constants::NUMBER_2 || number == common_lang_constants::NUMBER_3 ||
                     number == common_lang_constants::NUMBER_6) ||
                     (number == common_lang_constants::NUMBER_2_FULL_WIDTH ||
                      number == common_lang_constants::NUMBER_3_FULL_WIDTH ||
                      number == common_lang_constants::NUMBER_6_FULL_WIDTH)) ?
                    1 : ((number == common_lang_constants::NUMBER_0 || number == common_lang_constants::NUMBER_1 ||
                          number == common_lang_constants::NUMBER_4 || number == common_lang_constants::NUMBER_5 ||
                          number == common_lang_constants::NUMBER_8) ||
                         (number == common_lang_constants::NUMBER_0_FULL_WIDTH ||
                          number == common_lang_constants::NUMBER_1_FULL_WIDTH ||
                          number == common_lang_constants::NUMBER_4_FULL_WIDTH ||
                          number == common_lang_constants::NUMBER_5_FULL_WIDTH ||
                             number == common_lang_constants::NUMBER_8_FULL_WIDTH)) ?
                    2 : ((number == common_lang_constants::NUMBER_7 || number == common_lang_constants::NUMBER_9) ||
                         (number == common_lang_constants::NUMBER_7_FULL_WIDTH ||
                          number == common_lang_constants::NUMBER_9_FULL_WIDTH)) ?
                    3 : 0;
            }
        };

    /// @brief Spanish syllable counting functor utility.
    class spanish_syllabize final : public base_syllabize
        {
    public:
        /// @private
        spanish_syllabize() noexcept = default;
        /** @brief Main interface for syllabizing a block of text.
            @returns The number of syllables from a block of text.
            @param start The text block to analyze.
            @param length The length of the text block to analyze.*/
        [[nodiscard]]
        size_t operator()(const wchar_t* start, const size_t length) final
            {
            // reset our data
            reset();
            assert(start);
            if (start == nullptr || length == 0)
                { return 0; }

            m_length = length;
            adjust_length_if_possesive(start);
            const wchar_t* end = start + m_length;

            const std::pair<bool,size_t> mathResult = is_special_math_word(start, m_length);
            if (mathResult.first)
                { return m_syllable_count = mathResult.second; }

            if (syllabize_if_contains_periods<spanish_syllabize>(start, end))
                { return m_syllable_count; }

            if (syllabize_if_contains_dashes<spanish_syllabize>(start))
                { return m_syllable_count; }

            const std::pair<size_t,size_t> prefixResult = get_prefix_length(start, m_length);
            if (prefixResult.second > 0)
                {
                start += prefixResult.second;
                m_length -= prefixResult.second;
                m_syllable_count += prefixResult.first;
                }

            m_previous_block_vowel = m_previous_vowel = m_length;
            const wchar_t* current_char = start;

            while (current_char != end)
                {
                bool current_char_is_vowel = isChar.is_vowel(current_char[0]);
                current_char_is_vowel =
                    (traits::case_insensitive_ex::eq(current_char[0], common_lang_constants::LOWER_Y) &&
                        is_consonant_y(start, current_char - start) ) ?
                    false : current_char_is_vowel;

                bool next_char_is_vowel = false;
                // if last letter, then there is no next letter
                if ((m_length-1) == static_cast<size_t>(current_char-start) )
                    { next_char_is_vowel = false; }
                else
                    {
                    next_char_is_vowel = isChar.is_vowel(current_char[1]);
                    next_char_is_vowel =
                        (traits::case_insensitive_ex::eq(current_char[1], common_lang_constants::LOWER_Y) &&
                            is_consonant_y(start, (current_char+1) - start) ) ?
                        false : next_char_is_vowel;
                    }

                bool is_in_vowel_block = current_char_is_vowel && next_char_is_vowel;
                // if it's a vowel and it's the only one in this block
                if (current_char_is_vowel && !is_in_vowel_block)
                    {
                    ++m_syllable_count;
                    m_previous_vowel = current_char - start;
                    }
                else if (current_char_is_vowel && is_in_vowel_block)
                    {
                    const wchar_t* start_of_block = current_char;
                    while (current_char != end &&
                        isChar.is_vowel(current_char[1]) )
                        {
                        ++current_char;
                        }
                    // if it is two consecutive vowels then make sure they
                    // aren't separate syllables
                    if (is_vowels_separate_syllables(start,
                            start_of_block - start,
                            (current_char+1)-start_of_block) )
                        {
                        m_syllable_count += 2;
                        }
                    else
                        {
                        ++m_syllable_count;
                        }
                    m_previous_vowel = current_char - start;
                    }
                // syllabize numbers
                else if (characters::is_character::is_numeric_simple(current_char[0]) )
                    {
                    size_t characters_counted = 0;
                    m_syllable_count +=
                        syllabify_numeral<syllabize_spanish_number>(current_char, end, characters_counted,
                            common_lang_constants::COMMA, common_lang_constants::PERIOD);
                    current_char += characters_counted;
                    if (current_char >= end)
                        { break; }
                    // else, we already moved to the next character to analyze, so just restart loop
                    else
                        { continue; }
                    }
                // syllabize any pertinent symbols
                m_syllable_count += get_symbol_syllable_count(start, end, current_char);
                if (!current_char_is_vowel)
                    {
                    m_previous_block_vowel = m_previous_vowel;
                    }
                ++current_char;
                }

            finalize_special_cases(start);
            // all words are at least one syllable (even all consonant acronyms)
            m_syllable_count = (m_syllable_count > 0) ? m_syllable_count : 1;
            return m_syllable_count;
            }
    protected:
        inline void finalize_special_cases(const wchar_t* start) noexcept
            {
            assert(start);
            // Irish names with proceeding "Mc" is a separate syllable
            if (m_length >= 2 &&
                traits::case_insensitive_ex::eq(start[0], common_lang_constants::LOWER_M) &&
                traits::case_insensitive_ex::eq(start[1], common_lang_constants::LOWER_C))
                {
                ++m_syllable_count;
                return;
                }
            }
        /** @brief Sees if a word begins with a special prefixes which should always end as a syllable division.
            @note Some of the prefixes from the article are omitted because they actually produce incorrect results.
            @param start The text block to analyze.
            @param length The length of the text block to analyze.
            @returns A pair with the syllable count and length of the prefix.*/
        [[nodiscard]]
        inline static std::pair<size_t,size_t> get_prefix_length(const wchar_t* start,
                                                                 const size_t length) noexcept
            {
            assert(start);
            if (length >= 5 &&
                traits::case_insensitive_ex::eq(start[0], common_lang_constants::LOWER_C) &&
                traits::case_insensitive_ex::eq(start[1], common_lang_constants::LOWER_U) &&
                traits::case_insensitive_ex::eq(start[2], common_lang_constants::LOWER_A) &&
                traits::case_insensitive_ex::eq(start[3], common_lang_constants::LOWER_S) &&
                traits::case_insensitive_ex::eq(start[4], common_lang_constants::LOWER_I))
                { return std::pair<size_t,size_t>(2,5); }
            else if (length >= 5 &&
                traits::case_insensitive_ex::eq(start[0], common_lang_constants::LOWER_I) &&
                traits::case_insensitive_ex::eq(start[1], common_lang_constants::LOWER_N) &&
                traits::case_insensitive_ex::eq(start[2], common_lang_constants::LOWER_F) &&
                traits::case_insensitive_ex::eq(start[3], common_lang_constants::LOWER_R) &&
                traits::case_insensitive_ex::eq(start[4], common_lang_constants::LOWER_A))
                { return std::pair<size_t,size_t>(2,5); }
            else if (length >= 5 &&
                traits::case_insensitive_ex::eq(start[0], common_lang_constants::LOWER_I) &&
                traits::case_insensitive_ex::eq(start[1], common_lang_constants::LOWER_N) &&
                traits::case_insensitive_ex::eq(start[2], common_lang_constants::LOWER_T) &&
                traits::case_insensitive_ex::eq(start[3], common_lang_constants::LOWER_R) &&
                traits::case_insensitive_ex::eq(start[4], common_lang_constants::LOWER_A))
                { return std::pair<size_t,size_t>(2,5); }
            else if (length >= 5 &&
                traits::case_insensitive_ex::eq(start[0], common_lang_constants::LOWER_M) &&
                traits::case_insensitive_ex::eq(start[1], common_lang_constants::LOWER_U) &&
                traits::case_insensitive_ex::eq(start[2], common_lang_constants::LOWER_L) &&
                traits::case_insensitive_ex::eq(start[3], common_lang_constants::LOWER_T) &&
                traits::case_insensitive_ex::eq(start[4], common_lang_constants::LOWER_I))
                { return std::pair<size_t,size_t>(2,5); }
            else if (length >= 5 &&
                traits::case_insensitive_ex::eq(start[0], common_lang_constants::LOWER_Y) &&
                traits::case_insensitive_ex::eq(start[1], common_lang_constants::LOWER_U) &&
                traits::case_insensitive_ex::eq(start[2], common_lang_constants::LOWER_X) &&
                traits::case_insensitive_ex::eq(start[3], common_lang_constants::LOWER_T) &&
                traits::case_insensitive_ex::eq(start[4], common_lang_constants::LOWER_A))
                { return std::pair<size_t,size_t>(2,5); }
            else if (length >= 5 &&
                traits::case_insensitive_ex::eq(start[0], common_lang_constants::LOWER_R) &&
                traits::case_insensitive_ex::eq(start[1], common_lang_constants::LOWER_A) &&
                traits::case_insensitive_ex::eq(start[2], common_lang_constants::LOWER_D) &&
                traits::case_insensitive_ex::eq(start[3], common_lang_constants::LOWER_I) &&
                traits::case_insensitive_ex::eq(start[4], common_lang_constants::LOWER_O))
                { return std::pair<size_t,size_t>(2,5); }
            else if (length >= 5 &&
                traits::case_insensitive_ex::eq(start[0], common_lang_constants::LOWER_R) &&
                traits::case_insensitive_ex::eq(start[1], common_lang_constants::LOWER_E) &&
                traits::case_insensitive_ex::eq(start[2], common_lang_constants::LOWER_T) &&
                traits::case_insensitive_ex::eq(start[3], common_lang_constants::LOWER_R) &&
                traits::case_insensitive_ex::eq(start[4], common_lang_constants::LOWER_O))
                { return std::pair<size_t,size_t>(2,5); }
            else if (length >= 5 &&
                traits::case_insensitive_ex::eq(start[0], common_lang_constants::LOWER_S) &&
                traits::case_insensitive_ex::eq(start[1], common_lang_constants::LOWER_O) &&
                traits::case_insensitive_ex::eq(start[2], common_lang_constants::LOWER_B) &&
                traits::case_insensitive_ex::eq(start[3], common_lang_constants::LOWER_R) &&
                traits::case_insensitive_ex::eq(start[4], common_lang_constants::LOWER_E))
                { return std::pair<size_t,size_t>(2,5); }
            else if (length >= 5 &&
                traits::case_insensitive_ex::eq(start[0], common_lang_constants::LOWER_S) &&
                traits::case_insensitive_ex::eq(start[1], common_lang_constants::LOWER_U) &&
                traits::case_insensitive_ex::eq(start[2], common_lang_constants::LOWER_P) &&
                traits::case_insensitive_ex::eq(start[3], common_lang_constants::LOWER_R) &&
                traits::case_insensitive_ex::eq(start[4], common_lang_constants::LOWER_A))
                { return std::pair<size_t,size_t>(2,5); }
            else if (length >= 5 &&
                traits::case_insensitive_ex::eq(start[0], common_lang_constants::LOWER_U) &&
                traits::case_insensitive_ex::eq(start[1], common_lang_constants::LOWER_L) &&
                traits::case_insensitive_ex::eq(start[2], common_lang_constants::LOWER_T) &&
                traits::case_insensitive_ex::eq(start[3], common_lang_constants::LOWER_R) &&
                traits::case_insensitive_ex::eq(start[4], common_lang_constants::LOWER_A))
                { return std::pair<size_t,size_t>(2,5); }
            else if (length >= 5 &&
                traits::case_insensitive_ex::eq(start[0], common_lang_constants::LOWER_V) &&
                traits::case_insensitive_ex::eq(start[1], common_lang_constants::LOWER_I) &&
                traits::case_insensitive_ex::eq(start[2], common_lang_constants::LOWER_D) &&
                traits::case_insensitive_ex::eq(start[3], common_lang_constants::LOWER_E) &&
                traits::case_insensitive_ex::eq(start[4], common_lang_constants::LOWER_O))
                { return std::pair<size_t,size_t>(2,5); }
            // pronounced as "a-e-ro"
            else if (length >= 4 &&
                traits::case_insensitive_ex::eq(start[0], common_lang_constants::LOWER_A) &&
                traits::case_insensitive_ex::eq(start[1], common_lang_constants::LOWER_E) &&
                traits::case_insensitive_ex::eq(start[2], common_lang_constants::LOWER_R) &&
                traits::case_insensitive_ex::eq(start[3], common_lang_constants::LOWER_O))
                { return std::pair<size_t,size_t>(3,4); }
            // pronounced as "aer-o"
            else if (length >= 4 && traits::case_insensitive_ex::eq(start[0], common_lang_constants::LOWER_A) &&
                traits::case_insensitive_ex::eq(start[1], common_lang_constants::LOWER_E) &&
                traits::case_insensitive_ex::eq(start[2], common_lang_constants::LOWER_R) &&
                traits::case_insensitive_ex::eq(start[3], common_lang_constants::LOWER_O_ACUTE))
                { return std::pair<size_t,size_t>(2,4); }
            else if (length >= 4 &&
                    traits::case_insensitive_ex::eq(start[0], common_lang_constants::LOWER_A) &&
                    traits::case_insensitive_ex::eq(start[1], common_lang_constants::LOWER_N) &&
                    traits::case_insensitive_ex::eq(start[2], common_lang_constants::LOWER_T) &&
                    traits::case_insensitive_ex::eq(start[3], common_lang_constants::LOWER_E))
                { return std::pair<size_t,size_t>(2,4); }
            else if (length >= 4 &&
                traits::case_insensitive_ex::eq(start[0], common_lang_constants::LOWER_A) &&
                traits::case_insensitive_ex::eq(start[1], common_lang_constants::LOWER_N) &&
                traits::case_insensitive_ex::eq(start[2], common_lang_constants::LOWER_T) &&
                traits::case_insensitive_ex::eq(start[3], common_lang_constants::LOWER_I))
                { return std::pair<size_t,size_t>(2,4); }
            else if (length >= 4 &&
                traits::case_insensitive_ex::eq(start[0], common_lang_constants::LOWER_C) &&
                traits::case_insensitive_ex::eq(start[1], common_lang_constants::LOWER_A) &&
                traits::case_insensitive_ex::eq(start[2], common_lang_constants::LOWER_T) &&
                traits::case_insensitive_ex::eq(start[3], common_lang_constants::LOWER_A))
                { return std::pair<size_t,size_t>(2,4); }
            else if (length >= 4 &&
                traits::case_insensitive_ex::eq(start[0], common_lang_constants::LOWER_D) &&
                traits::case_insensitive_ex::eq(start[1], common_lang_constants::LOWER_E) &&
                traits::case_insensitive_ex::eq(start[2], common_lang_constants::LOWER_C) &&
                traits::case_insensitive_ex::eq(start[3], common_lang_constants::LOWER_I))
                { return std::pair<size_t,size_t>(2,4); }
            else if (length >= 4 &&
                traits::case_insensitive_ex::eq(start[0], common_lang_constants::LOWER_E) &&
                traits::case_insensitive_ex::eq(start[1], common_lang_constants::LOWER_C) &&
                traits::case_insensitive_ex::eq(start[2], common_lang_constants::LOWER_T) &&
                traits::case_insensitive_ex::eq(start[3], common_lang_constants::LOWER_O))
                { return std::pair<size_t,size_t>(2,4); }
            else if (length >= 4 &&
                traits::case_insensitive_ex::eq(start[0], common_lang_constants::LOWER_E) &&
                traits::case_insensitive_ex::eq(start[1], common_lang_constants::LOWER_N) &&
                traits::case_insensitive_ex::eq(start[2], common_lang_constants::LOWER_D) &&
                traits::case_insensitive_ex::eq(start[3], common_lang_constants::LOWER_O))
                { return std::pair<size_t,size_t>(2,4); }
            else if (length >= 4 &&
                traits::case_insensitive_ex::eq(start[0], common_lang_constants::LOWER_H) &&
                traits::case_insensitive_ex::eq(start[1], common_lang_constants::LOWER_E) &&
                traits::case_insensitive_ex::eq(start[2], common_lang_constants::LOWER_M) &&
                traits::case_insensitive_ex::eq(start[3], common_lang_constants::LOWER_I))
                { return std::pair<size_t,size_t>(2,4); }
            else if (length >= 4 &&
                traits::case_insensitive_ex::eq(start[0], common_lang_constants::LOWER_H) &&
                traits::case_insensitive_ex::eq(start[1], common_lang_constants::LOWER_I) &&
                traits::case_insensitive_ex::eq(start[2], common_lang_constants::LOWER_P) &&
                traits::case_insensitive_ex::eq(start[3], common_lang_constants::LOWER_O))
                { return std::pair<size_t,size_t>(2,4); }
            else if (length >= 4 &&
                traits::case_insensitive_ex::eq(start[0], common_lang_constants::LOWER_M) &&
                traits::case_insensitive_ex::eq(start[1], common_lang_constants::LOWER_E) &&
                traits::case_insensitive_ex::eq(start[2], common_lang_constants::LOWER_T) &&
                traits::case_insensitive_ex::eq(start[3], common_lang_constants::LOWER_A))
                { return std::pair<size_t,size_t>(2,4); }
            else if (length >= 4 &&
                traits::case_insensitive_ex::eq(start[0], common_lang_constants::LOWER_O) &&
                traits::case_insensitive_ex::eq(start[1], common_lang_constants::LOWER_M) &&
                traits::case_insensitive_ex::eq(start[2], common_lang_constants::LOWER_N) &&
                traits::case_insensitive_ex::eq(start[3], common_lang_constants::LOWER_I))
                { return std::pair<size_t,size_t>(2,4); }
            else if (length >= 4 &&
                traits::case_insensitive_ex::eq(start[0], common_lang_constants::LOWER_P) &&
                traits::case_insensitive_ex::eq(start[1], common_lang_constants::LOWER_A) &&
                traits::case_insensitive_ex::eq(start[2], common_lang_constants::LOWER_L) &&
                traits::case_insensitive_ex::eq(start[3], common_lang_constants::LOWER_I))
                { return std::pair<size_t,size_t>(2,4); }
            else if (length >= 4 &&
                traits::case_insensitive_ex::eq(start[0], common_lang_constants::LOWER_P) &&
                traits::case_insensitive_ex::eq(start[1], common_lang_constants::LOWER_A) &&
                traits::case_insensitive_ex::eq(start[2], common_lang_constants::LOWER_R) &&
                traits::case_insensitive_ex::eq(start[3], common_lang_constants::LOWER_A))
                { return std::pair<size_t,size_t>(2,4); }
            else if (length >= 4 &&
                traits::case_insensitive_ex::eq(start[0], common_lang_constants::LOWER_P) &&
                traits::case_insensitive_ex::eq(start[1], common_lang_constants::LOWER_I) &&
                traits::case_insensitive_ex::eq(start[2], common_lang_constants::LOWER_A) &&
                traits::case_insensitive_ex::eq(start[3], common_lang_constants::LOWER_R))
                { return std::pair<size_t,size_t>(2,4); }
            else if (length >= 4 &&
                traits::case_insensitive_ex::eq(start[0], common_lang_constants::LOWER_S) &&
                traits::case_insensitive_ex::eq(start[1], common_lang_constants::LOWER_E) &&
                traits::case_insensitive_ex::eq(start[2], common_lang_constants::LOWER_M) &&
                traits::case_insensitive_ex::eq(start[3], common_lang_constants::LOWER_I))
                { return std::pair<size_t,size_t>(2,4); }
            else if (length >= 4 &&
                traits::case_insensitive_ex::eq(start[0], common_lang_constants::LOWER_V) &&
                traits::case_insensitive_ex::eq(start[1], common_lang_constants::LOWER_I) &&
                traits::case_insensitive_ex::eq(start[2], common_lang_constants::LOWER_C) &&
                traits::case_insensitive_ex::eq(start[3], common_lang_constants::LOWER_E))
                { return std::pair<size_t,size_t>(2,4); }
            else if (length >= 3 &&
                traits::case_insensitive_ex::eq(start[0], common_lang_constants::LOWER_A) &&
                traits::case_insensitive_ex::eq(start[1], common_lang_constants::LOWER_N) &&
                traits::case_insensitive_ex::eq(start[2], common_lang_constants::LOWER_A))
                { return std::pair<size_t,size_t>(2,3); }
            else if (length >= 3 &&
                traits::case_insensitive_ex::eq(start[0], common_lang_constants::LOWER_A) &&
                traits::case_insensitive_ex::eq(start[1], common_lang_constants::LOWER_P) &&
                traits::case_insensitive_ex::eq(start[2], common_lang_constants::LOWER_O))
                { return std::pair<size_t,size_t>(2,3); }
            else if (length >= 3 &&
                traits::case_insensitive_ex::eq(start[0], common_lang_constants::LOWER_E) &&
                traits::case_insensitive_ex::eq(start[1], common_lang_constants::LOWER_P) &&
                traits::case_insensitive_ex::eq(start[2], common_lang_constants::LOWER_I))
                { return std::pair<size_t,size_t>(2,3); }
            else if (length >= 3 &&
                traits::case_insensitive_ex::eq(start[0], common_lang_constants::LOWER_E) &&
                traits::case_insensitive_ex::eq(start[1], common_lang_constants::LOWER_X) &&
                traits::case_insensitive_ex::eq(start[2], common_lang_constants::LOWER_O))
                { return std::pair<size_t,size_t>(2,3); }
            else
                { return std::pair<size_t,size_t>(0,0); }
            }
        /** @brief Determines if a vowel is a strong (or accented strong) vowel,
                or accented weak vowel (the accent makes it strong).
            @param vowel The vowel to review.
            @returns @c true if vowel is strong.*/
        [[nodiscard]]
        static bool is_strong_vowel(const wchar_t vowel) noexcept
            {
            return (traits::case_insensitive_ex::eq(vowel, common_lang_constants::LOWER_A) ||
                traits::case_insensitive_ex::eq(vowel, common_lang_constants::LOWER_A_ACUTE) ||
                traits::case_insensitive_ex::eq(vowel, common_lang_constants::LOWER_O) ||
                traits::case_insensitive_ex::eq(vowel, common_lang_constants::LOWER_O_ACUTE) ||
                traits::case_insensitive_ex::eq(vowel, common_lang_constants::LOWER_E) ||
                traits::case_insensitive_ex::eq(vowel, common_lang_constants::LOWER_E_ACUTE) ||
                traits::case_insensitive_ex::eq(vowel, common_lang_constants::LOWER_I_ACUTE) ||
                traits::case_insensitive_ex::eq(vowel, common_lang_constants::LOWER_U_ACUTE));
            }
        /** @brief Determines if a vowel is weak. Note that accented weak vowels are not counted here.
            @param vowel The vowel to review.
            @returns @c true if vowel is weak.*/
        [[nodiscard]]
        static bool is_weak_vowel(const wchar_t vowel) noexcept
            {
            return (traits::case_insensitive_ex::eq(vowel, common_lang_constants::LOWER_I) ||
                traits::case_insensitive_ex::eq(vowel, common_lang_constants::LOWER_U)||
                // although the umlaut is an accent, it doesn't make U strong
                traits::case_insensitive_ex::eq(vowel, common_lang_constants::LOWER_U_UMLAUTS));
            }
        /** @brief Determines if a block of vowels are separate syllables.
            @param word The original word under review.
            @param position The position in @c word where the current vowel block begins.
            @param vowel_block_size The number of vowels in the current block.
            @returns @c true if vowels break into separate syllables.*/
        [[nodiscard]]
        bool is_vowels_separate_syllables(const wchar_t* word,
                                          size_t position, size_t vowel_block_size) const noexcept
            {
            assert(word);
            // four consecutive vowels would be odd, so just return true
            if (vowel_block_size > 3)
                { return true; }
            else if (vowel_block_size == 3)
                {
                // special case for "uiad[oa]" to be handled later
                if (position <= (m_length-5) &&
                     traits::case_insensitive_ex::eq(word[position], common_lang_constants::LOWER_U) &&
                    traits::case_insensitive_ex::eq(word[position+1], common_lang_constants::LOWER_I) &&
                    traits::case_insensitive_ex::eq(word[position+2], common_lang_constants::LOWER_A) &&
                    traits::case_insensitive_ex::eq(word[position+3], common_lang_constants::LOWER_D) &&
                    (traits::case_insensitive_ex::eq(word[position+4], common_lang_constants::LOWER_O) ||
                        traits::case_insensitive_ex::eq(word[position+4], common_lang_constants::LOWER_A)) )
                    { ++position; }
                else
                    {
                    // two strongs next to each other will split
                    return ((is_strong_vowel(word[position]) && is_strong_vowel(word[position+1])) ||
                            (is_strong_vowel(word[position+1]) && is_strong_vowel(word[position+2])));
                    }
                }

            // TUOS[OA] and STRUS[OA]--"uo" splits, even though 'u' is weak
            if (position > 0 &&
                position <= (m_length-4) &&
                traits::case_insensitive_ex::eq(word[position-1], common_lang_constants::LOWER_T) &&
                traits::case_insensitive_ex::eq(word[position], common_lang_constants::LOWER_U) &&
                traits::case_insensitive_ex::eq(word[position+1], common_lang_constants::LOWER_O) &&
                traits::case_insensitive_ex::eq(word[position+2], common_lang_constants::LOWER_S) &&
                (traits::case_insensitive_ex::eq(word[position+3], common_lang_constants::LOWER_O) ||
                    traits::case_insensitive_ex::eq(word[position+3], common_lang_constants::LOWER_A)))
                { return true; }
            else if (position > 2 &&
                position <= (m_length-4) &&
                traits::case_insensitive_ex::eq(word[position-3], common_lang_constants::LOWER_S) &&
                traits::case_insensitive_ex::eq(word[position-2], common_lang_constants::LOWER_T) &&
                traits::case_insensitive_ex::eq(word[position-1], common_lang_constants::LOWER_R) &&
                traits::case_insensitive_ex::eq(word[position], common_lang_constants::LOWER_U) &&
                traits::case_insensitive_ex::eq(word[position+1], common_lang_constants::LOWER_O) &&
                traits::case_insensitive_ex::eq(word[position+2], common_lang_constants::LOWER_S) &&
                (traits::case_insensitive_ex::eq(word[position+3], common_lang_constants::LOWER_O) ||
                    traits::case_insensitive_ex::eq(word[position+3], common_lang_constants::LOWER_A)))
                { return true; }
            // FIANZ[UA]
            else if (position > 0 &&
                position <= (m_length-5) &&
                traits::case_insensitive_ex::eq(word[position-1], common_lang_constants::LOWER_F) &&
                traits::case_insensitive_ex::eq(word[position], common_lang_constants::LOWER_I) &&
                traits::case_insensitive_ex::eq(word[position+1], common_lang_constants::LOWER_A) &&
                traits::case_insensitive_ex::eq(word[position+2], common_lang_constants::LOWER_N) &&
                traits::case_insensitive_ex::eq(word[position+3], common_lang_constants::LOWER_Z) &&
                (traits::case_insensitive_ex::eq(word[position+4], common_lang_constants::LOWER_U) ||
                 traits::case_insensitive_ex::eq(word[position+4], common_lang_constants::LOWER_A)) )
                { return true; }
            // IAD[OA]--"ia" splits, even though 'i' is weak
            else if (position > 0 &&
                position <= (m_length-4) &&
                traits::case_insensitive_ex::eq(word[position], common_lang_constants::LOWER_I) &&
                traits::case_insensitive_ex::eq(word[position+1], common_lang_constants::LOWER_A) &&
                traits::case_insensitive_ex::eq(word[position+2], common_lang_constants::LOWER_D) &&
                (traits::case_insensitive_ex::eq(word[position+3], common_lang_constants::LOWER_O) ||
                 traits::case_insensitive_ex::eq(word[position+3], common_lang_constants::LOWER_A)) )
                {
                // exception for "criado"
                if (position == 2 &&
                    traits::case_insensitive_ex::eq(word[position-2], common_lang_constants::LOWER_C) &&
                    traits::case_insensitive_ex::eq(word[position-1], common_lang_constants::LOWER_R))
                    { return false; }
                else
                    { return true; }
                }
            // two strong together will split--this is the standard rule, but exceptions need to be checked first
            else if (is_strong_vowel(word[position]) && is_strong_vowel(word[position+1]))
                { return true; }
            // two weak vowels or one strong and regular (i.e., not accented) weak vowel are one sound
            else
                { return false; }
            }
        };
    }

/** @}*/

#endif //__SPANISH_SYLLABLE_H__
