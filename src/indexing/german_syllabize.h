/** @addtogroup Indexing
    @brief Classes for parsing and indexing text.
    @date 2003-2020
    @copyright Oleander Software, Ltd.
    @author Blake Madden
    @details This program is free software; you can redistribute it and/or modify
     it under the terms of the 3-Clause BSD License.

     SPDX-License-Identifier: BSD-3-Clause
* @{*/

#ifndef __GERMAN_SYLLABLE_H__
#define __GERMAN_SYLLABLE_H__

#include "syllable.h"
#include <utility>

namespace grammar
    {
    /// @brief Counts the number of (German) syllables in a (single-digit) number.
    class syllabize_german_number
        {
    public:
        /** @returns The number of syllables for a given number.
            @param number The number to review.*/
        [[nodiscard]] size_t operator()(const wchar_t number) const noexcept
            {
            if (!characters::is_character::is_numeric_simple(number))
                { return 0; }
            return traits::case_insensitive_ex::eq(number, common_lang_constants::NUMBER_7) ? 2 : 1;
            }
        };

    /// @brief German syllable counting functor utility.
    class german_syllabize final : public base_syllabize
        {
    public:
        german_syllabize() noexcept {}
        /** @brief Main interface for syllabizing a block of text.
            @param start The start of the text to parse.
            @param length The length of the text.
            @returns The number of syllables counted.*/
        [[nodiscard]] size_t operator()(const wchar_t* start, const size_t length) final
            {
            assert(start);
            assert(std::wcslen(start) >= length);
            //reset our data
            reset();
            if (start == nullptr || length == 0)
                { return 0; }

            m_length = length;
            adjust_length_if_possesive(start);
            const wchar_t* end = start + m_length;

            const std::pair<bool,size_t> mathResult = is_special_math_word(start, m_length);
            if (mathResult.first)
                { return m_syllable_count = mathResult.second; }

            if (syllabize_if_contains_periods<german_syllabize>(start, end))
                { return m_syllable_count; }

            if (syllabize_if_contains_dashes<german_syllabize>(start))
                { return m_syllable_count; }

            const std::pair<size_t,size_t> prefixResult = get_prefix_length(start, m_length);
            if (prefixResult.second > 0)
                {
                start += prefixResult.second;
                m_length -= prefixResult.second;
                m_syllable_count += prefixResult.first;
                }
            //if past tense form of verb, skip "ge" prefix
            if (has_past_tense_prefix(start, m_length))
                {
                start += 2;
                ++m_syllable_count;
                }

            m_previous_block_vowel = m_previous_vowel = m_length;
            bool is_in_vowel_block = false;
            bool current_char_is_vowel = false;
            const wchar_t* current_char = start;

            while (current_char != end)
                {
                current_char_is_vowel = isChar.is_vowel(current_char[0]);

                bool next_char_is_vowel = false;
                /*if last letter, then there is no next letter*/
                if ((m_length-1) == static_cast<size_t>(current_char-start) )
                    { next_char_is_vowel = false; }
                else
                    {
                    next_char_is_vowel = isChar.is_vowel(current_char[1]);
                    }

                is_in_vowel_block = current_char_is_vowel && next_char_is_vowel;
                //if it's a vowel and it's the first one in this block
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
                    //some vowels blocks split into separate syllables, so count that.
                    m_syllable_count += get_vowel_block_syllable_count(start,
                        start_of_block - start,
                        (current_char+1)-start_of_block,
                        m_previous_block_vowel > static_cast<size_t>(start_of_block-start));
                    m_previous_vowel = current_char - start;
                    }
                //syllabize numbers
                else if (characters::is_character::is_numeric_simple(current_char[0]) )
                    {
                    size_t characters_counted = 0;
                    m_syllable_count += syllabify_numeral<syllabize_german_number>(current_char, end, characters_counted, common_lang_constants::COMMA, common_lang_constants::PERIOD);
                    current_char += characters_counted;
                    if (current_char >= end)
                        { break; }
                    //else, we already moved to the next character to analyze, so just restart loop
                    else
                        { continue; }
                    }
                //syllabize any pertinent symbols
                m_syllable_count += get_symbol_syllable_count(start, end, current_char);
                if (!current_char_is_vowel)
                    {
                    m_previous_block_vowel = m_previous_vowel;
                    }
                ++current_char;
                }

            //all words are at least one syllable (even all consonant acronyms)
            m_syllable_count = (m_syllable_count > 0) ? m_syllable_count : 1;
            return m_syllable_count;
            }
    protected:
        /** @brief Sees if a word begins with a "ge" prefix which should always end as a syllable division.
            @param start The start of the text to parse.
            @param length The length of the text.
            @returns @c true if the word begins with a valid past tense ("ge") prefix.*/
        [[nodiscard]] inline static bool has_past_tense_prefix(const wchar_t* start, const size_t length) noexcept
            {
            assert(start);
            assert(std::wcslen(start) >= length);
            if (length >= 2 &&
                traits::case_insensitive_ex::eq(start[0], common_lang_constants::LOWER_G) &&
                traits::case_insensitive_ex::eq(start[1], common_lang_constants::LOWER_E))
                {
                //watch out for "geis" or "geiß"
                if (length >= 4 &&
                    traits::case_insensitive_ex::eq(start[2], common_lang_constants::LOWER_I) &&
                    (traits::case_insensitive_ex::eq(start[3], common_lang_constants::LOWER_S) ||
                     start[3] == common_lang_constants::ESZETT))
                    { return false; }
                return true;
                }
            else
                { return false; }
            }
        /** @brief Sees if a word begins with a special prefix which should always end as a syllable division.
            @param start The start of the text to parse.
            @param length The length of the text.
            @returns A pair with the syllable count and length of the prefix.*/
        [[nodiscard]] inline static std::pair<size_t,size_t> get_prefix_length(const wchar_t* start, const size_t length) noexcept
            {
            assert(start);
            assert(std::wcslen(start) >= length);
            if (length >= 9 &&
                traits::case_insensitive_ex::eq(start[0], common_lang_constants::LOWER_G) &&
                traits::case_insensitive_ex::eq(start[1], common_lang_constants::LOWER_E) &&
                traits::case_insensitive_ex::eq(start[2], common_lang_constants::LOWER_B) &&
                traits::case_insensitive_ex::eq(start[3], common_lang_constants::LOWER_E) &&
                traits::case_insensitive_ex::eq(start[4], common_lang_constants::LOWER_N) &&
                traits::case_insensitive_ex::eq(start[5], common_lang_constants::LOWER_U_UMLAUTS) &&
                traits::case_insensitive_ex::eq(start[6], common_lang_constants::LOWER_B) &&
                traits::case_insensitive_ex::eq(start[7], common_lang_constants::LOWER_E) &&
                traits::case_insensitive_ex::eq(start[8], common_lang_constants::LOWER_R))
                { return std::pair<size_t,size_t>(4,9); }
            if (length >= 8)
                {
                if (traits::case_insensitive_ex::eq(start[0], common_lang_constants::LOWER_E) &&
                    traits::case_insensitive_ex::eq(start[1], common_lang_constants::LOWER_N) &&
                    traits::case_insensitive_ex::eq(start[2], common_lang_constants::LOWER_T) &&
                    traits::case_insensitive_ex::eq(start[3], common_lang_constants::LOWER_G) &&
                    traits::case_insensitive_ex::eq(start[4], common_lang_constants::LOWER_E) &&
                    traits::case_insensitive_ex::eq(start[5], common_lang_constants::LOWER_B) &&
                    traits::case_insensitive_ex::eq(start[6], common_lang_constants::LOWER_E) &&
                    traits::case_insensitive_ex::eq(start[7], common_lang_constants::LOWER_N))
                    { return std::pair<size_t,size_t>(3,8); }
                else if (traits::case_insensitive_ex::eq(start[0], common_lang_constants::LOWER_Z) &&
                    traits::case_insensitive_ex::eq(start[1], common_lang_constants::LOWER_W) &&
                    traits::case_insensitive_ex::eq(start[2], common_lang_constants::LOWER_I) &&
                    traits::case_insensitive_ex::eq(start[3], common_lang_constants::LOWER_S) &&
                    traits::case_insensitive_ex::eq(start[4], common_lang_constants::LOWER_C) &&
                    traits::case_insensitive_ex::eq(start[5], common_lang_constants::LOWER_H) &&
                    traits::case_insensitive_ex::eq(start[6], common_lang_constants::LOWER_E) &&
                    traits::case_insensitive_ex::eq(start[7], common_lang_constants::LOWER_N))
                    { return std::pair<size_t,size_t>(2,8); }
                else if (traits::case_insensitive_ex::eq(start[0], common_lang_constants::LOWER_Z) &&
                    traits::case_insensitive_ex::eq(start[1], common_lang_constants::LOWER_U) &&
                    traits::case_insensitive_ex::eq(start[2], common_lang_constants::LOWER_S) &&
                    traits::case_insensitive_ex::eq(start[3], common_lang_constants::LOWER_A) &&
                    traits::case_insensitive_ex::eq(start[4], common_lang_constants::LOWER_M) &&
                    traits::case_insensitive_ex::eq(start[5], common_lang_constants::LOWER_M) &&
                    traits::case_insensitive_ex::eq(start[6], common_lang_constants::LOWER_E) &&
                    traits::case_insensitive_ex::eq(start[7], common_lang_constants::LOWER_N))
                    { return std::pair<size_t,size_t>(3,8); }
                }
            if (length >= 7 &&
                traits::case_insensitive_ex::eq(start[0], common_lang_constants::LOWER_E) &&
                traits::case_insensitive_ex::eq(start[1], common_lang_constants::LOWER_N) &&
                traits::case_insensitive_ex::eq(start[2], common_lang_constants::LOWER_T) &&
                traits::case_insensitive_ex::eq(start[3], common_lang_constants::LOWER_L) &&
                traits::case_insensitive_ex::eq(start[4], common_lang_constants::LOWER_A) &&
                traits::case_insensitive_ex::eq(start[4], common_lang_constants::LOWER_N) &&
                traits::case_insensitive_ex::eq(start[6], common_lang_constants::LOWER_G))
                { return std::pair<size_t,size_t>(2,7); }
            if (length >= 6)
                {
                if (traits::case_insensitive_ex::eq(start[0], common_lang_constants::LOWER_Z) &&
                    traits::case_insensitive_ex::eq(start[1], common_lang_constants::LOWER_U) &&
                    traits::case_insensitive_ex::eq(start[2], common_lang_constants::LOWER_R) &&
                    traits::case_insensitive_ex::eq(start[3], common_lang_constants::LOWER_U_UMLAUTS) &&
                    traits::case_insensitive_ex::eq(start[4], common_lang_constants::LOWER_C) &&
                    traits::case_insensitive_ex::eq(start[5], common_lang_constants::LOWER_K))
                    { return std::pair<size_t,size_t>(2,6); }
                else if (traits::case_insensitive_ex::eq(start[0], common_lang_constants::LOWER_G) &&
                    traits::case_insensitive_ex::eq(start[1], common_lang_constants::LOWER_L) &&
                    traits::case_insensitive_ex::eq(start[2], common_lang_constants::LOWER_E) &&
                    traits::case_insensitive_ex::eq(start[3], common_lang_constants::LOWER_I) &&
                    traits::case_insensitive_ex::eq(start[4], common_lang_constants::LOWER_C) &&
                    traits::case_insensitive_ex::eq(start[5], common_lang_constants::LOWER_H))
                    { return std::pair<size_t,size_t>(1,6); }
                else if (traits::case_insensitive_ex::eq(start[0], common_lang_constants::LOWER_H) &&
                    traits::case_insensitive_ex::eq(start[1], common_lang_constants::LOWER_E) &&
                    traits::case_insensitive_ex::eq(start[2], common_lang_constants::LOWER_R) &&
                    traits::case_insensitive_ex::eq(start[3], common_lang_constants::LOWER_A) &&
                    traits::case_insensitive_ex::eq(start[4], common_lang_constants::LOWER_U) &&
                    (traits::case_insensitive_ex::eq(start[5], common_lang_constants::LOWER_F) ||
                     traits::case_insensitive_ex::eq(start[5], common_lang_constants::LOWER_S)))
                    { return std::pair<size_t,size_t>(2,6); }
                else if (traits::case_insensitive_ex::eq(start[0], common_lang_constants::LOWER_H) &&
                    traits::case_insensitive_ex::eq(start[1], common_lang_constants::LOWER_I) &&
                    traits::case_insensitive_ex::eq(start[2], common_lang_constants::LOWER_N) &&
                    traits::case_insensitive_ex::eq(start[3], common_lang_constants::LOWER_W) &&
                    traits::case_insensitive_ex::eq(start[4], common_lang_constants::LOWER_E) &&
                    traits::case_insensitive_ex::eq(start[5], common_lang_constants::LOWER_G))
                    { return std::pair<size_t,size_t>(2,6); }
                }
            if (length >= 5)
                {
                if (traits::case_insensitive_ex::eq(start[0], common_lang_constants::LOWER_B) &&
                    traits::case_insensitive_ex::eq(start[1], common_lang_constants::LOWER_E) &&
                    traits::case_insensitive_ex::eq(start[2], common_lang_constants::LOWER_E) &&
                    traits::case_insensitive_ex::eq(start[3], common_lang_constants::LOWER_R) &&
                    (traits::case_insensitive_ex::eq(start[4], common_lang_constants::LOWER_D) ||
                     traits::case_insensitive_ex::eq(start[4], common_lang_constants::LOWER_B)))
                    { return std::pair<size_t,size_t>(2,5); }
                else if (traits::case_insensitive_ex::eq(start[0], common_lang_constants::LOWER_D) &&
                    traits::case_insensitive_ex::eq(start[1], common_lang_constants::LOWER_A) &&
                    traits::case_insensitive_ex::eq(start[2], common_lang_constants::LOWER_B) &&
                    traits::case_insensitive_ex::eq(start[3], common_lang_constants::LOWER_E) &&
                    traits::case_insensitive_ex::eq(start[4], common_lang_constants::LOWER_I))
                    { return std::pair<size_t,size_t>(2,5); }
                else if (traits::case_insensitive_ex::eq(start[0], common_lang_constants::LOWER_D) &&
                    traits::case_insensitive_ex::eq(start[1], common_lang_constants::LOWER_A) &&
                    traits::case_insensitive_ex::eq(start[2], common_lang_constants::LOWER_R) &&
                    traits::case_insensitive_ex::eq(start[3], common_lang_constants::LOWER_A) &&
                    traits::case_insensitive_ex::eq(start[4], common_lang_constants::LOWER_N))
                    { return std::pair<size_t,size_t>(2,5); }
                else if (traits::case_insensitive_ex::eq(start[0], common_lang_constants::LOWER_D) &&
                    traits::case_insensitive_ex::eq(start[1], common_lang_constants::LOWER_U) &&
                    traits::case_insensitive_ex::eq(start[2], common_lang_constants::LOWER_R) &&
                    traits::case_insensitive_ex::eq(start[3], common_lang_constants::LOWER_C) &&
                    traits::case_insensitive_ex::eq(start[4], common_lang_constants::LOWER_H))
                    { return std::pair<size_t,size_t>(1,5); }
                else if (traits::case_insensitive_ex::eq(start[0], common_lang_constants::LOWER_E) &&
                    traits::case_insensitive_ex::eq(start[1], common_lang_constants::LOWER_M) &&
                    traits::case_insensitive_ex::eq(start[2], common_lang_constants::LOWER_P) &&
                    traits::case_insensitive_ex::eq(start[3], common_lang_constants::LOWER_O) &&
                    traits::case_insensitive_ex::eq(start[4], common_lang_constants::LOWER_R))
                    { return std::pair<size_t,size_t>(2,5); }
                else if (traits::case_insensitive_ex::eq(start[0], common_lang_constants::LOWER_H) &&
                    traits::case_insensitive_ex::eq(start[1], common_lang_constants::LOWER_I) &&
                    traits::case_insensitive_ex::eq(start[2], common_lang_constants::LOWER_N) &&
                    traits::case_insensitive_ex::eq(start[3], common_lang_constants::LOWER_Z) &&
                    traits::case_insensitive_ex::eq(start[4], common_lang_constants::LOWER_U))
                    { return std::pair<size_t,size_t>(2,5); }
                else if (traits::case_insensitive_ex::eq(start[0], common_lang_constants::LOWER_S) &&
                    traits::case_insensitive_ex::eq(start[1], common_lang_constants::LOWER_T) &&
                    traits::case_insensitive_ex::eq(start[2], common_lang_constants::LOWER_A) &&
                    traits::case_insensitive_ex::eq(start[3], common_lang_constants::LOWER_T) &&
                    traits::case_insensitive_ex::eq(start[4], common_lang_constants::LOWER_T))
                    { return std::pair<size_t,size_t>(1,5); }
                }
            if (length >= 4)
                {
                if (traits::case_insensitive_ex::eq(start[0], common_lang_constants::LOWER_B) &&
                    traits::case_insensitive_ex::eq(start[1], common_lang_constants::LOWER_E) &&
                    traits::case_insensitive_ex::eq(start[2], common_lang_constants::LOWER_E) &&
                    (traits::case_insensitive_ex::eq(start[3], common_lang_constants::LOWER_N) ||
                     traits::case_insensitive_ex::eq(start[3], common_lang_constants::LOWER_H)))
                    { return std::pair<size_t,size_t>(2,4); }
                else if (traits::case_insensitive_ex::eq(start[0], common_lang_constants::LOWER_F) &&
                    traits::case_insensitive_ex::eq(start[1], common_lang_constants::LOWER_E) &&
                    traits::case_insensitive_ex::eq(start[2], common_lang_constants::LOWER_H) &&
                    traits::case_insensitive_ex::eq(start[3], common_lang_constants::LOWER_L))
                    { return std::pair<size_t,size_t>(1,4); }
                else if (traits::case_insensitive_ex::eq(start[0], common_lang_constants::LOWER_F) &&
                    traits::case_insensitive_ex::eq(start[1], common_lang_constants::LOWER_E) &&
                    traits::case_insensitive_ex::eq(start[2], common_lang_constants::LOWER_S) &&
                    traits::case_insensitive_ex::eq(start[3], common_lang_constants::LOWER_T))
                    { return std::pair<size_t,size_t>(1,4); }
                else if (traits::case_insensitive_ex::eq(start[0], common_lang_constants::LOWER_F) &&
                    traits::case_insensitive_ex::eq(start[1], common_lang_constants::LOWER_O) &&
                    traits::case_insensitive_ex::eq(start[2], common_lang_constants::LOWER_R) &&
                    traits::case_insensitive_ex::eq(start[3], common_lang_constants::LOWER_T))
                    { return std::pair<size_t,size_t>(1,4); }
                else if (traits::case_insensitive_ex::eq(start[0], common_lang_constants::LOWER_N) &&
                    traits::case_insensitive_ex::eq(start[1], common_lang_constants::LOWER_A) &&
                    traits::case_insensitive_ex::eq(start[2], common_lang_constants::LOWER_C) &&
                    traits::case_insensitive_ex::eq(start[3], common_lang_constants::LOWER_H))
                    { return std::pair<size_t,size_t>(1,4); }
                }
            if (length >= 3)
                {
                if (traits::case_insensitive_ex::eq(start[0], common_lang_constants::LOWER_A) &&
                    traits::case_insensitive_ex::eq(start[1], common_lang_constants::LOWER_U) &&
                    (traits::case_insensitive_ex::eq(start[2], common_lang_constants::LOWER_F) ||
                     traits::case_insensitive_ex::eq(start[2], common_lang_constants::LOWER_S)))
                    { return std::pair<size_t,size_t>(1,3); }
                else if (length >= 3 &&
                    traits::case_insensitive_ex::eq(start[0], common_lang_constants::LOWER_B) &&
                    traits::case_insensitive_ex::eq(start[1], common_lang_constants::LOWER_E) &&
                    traits::case_insensitive_ex::eq(start[2], common_lang_constants::LOWER_I))
                    { return std::pair<size_t,size_t>(1,3); }
                else if (traits::case_insensitive_ex::eq(start[0], common_lang_constants::LOWER_E) &&
                    traits::case_insensitive_ex::eq(start[1], common_lang_constants::LOWER_I) &&
                    traits::case_insensitive_ex::eq(start[2], common_lang_constants::LOWER_N))
                    { return std::pair<size_t,size_t>(1,3); }
                else if (length >= 3 &&
                    traits::case_insensitive_ex::eq(start[0], common_lang_constants::LOWER_H) &&
                    traits::case_insensitive_ex::eq(start[1], common_lang_constants::LOWER_I) &&
                    traits::case_insensitive_ex::eq(start[2], common_lang_constants::LOWER_N))
                    { return std::pair<size_t,size_t>(1,3); }
                else if (traits::case_insensitive_ex::eq(start[0], common_lang_constants::LOWER_H) &&
                    traits::case_insensitive_ex::eq(start[1], common_lang_constants::LOWER_E) &&
                    traits::case_insensitive_ex::eq(start[2], common_lang_constants::LOWER_R))
                    { return std::pair<size_t,size_t>(1,3); }
                else if (traits::case_insensitive_ex::eq(start[0], common_lang_constants::LOWER_L) &&
                    traits::case_insensitive_ex::eq(start[1], common_lang_constants::LOWER_O) &&
                    traits::case_insensitive_ex::eq(start[2], common_lang_constants::LOWER_S))
                    { return std::pair<size_t,size_t>(1,3); }
                else if (traits::case_insensitive_ex::eq(start[0], common_lang_constants::LOWER_M) &&
                    traits::case_insensitive_ex::eq(start[1], common_lang_constants::LOWER_I) &&
                    traits::case_insensitive_ex::eq(start[2], common_lang_constants::LOWER_T))
                    { return std::pair<size_t,size_t>(1,3); }
                else if (traits::case_insensitive_ex::eq(start[0], common_lang_constants::LOWER_V) &&
                    traits::case_insensitive_ex::eq(start[1], common_lang_constants::LOWER_O) &&
                    traits::case_insensitive_ex::eq(start[2], common_lang_constants::LOWER_R))
                    { return std::pair<size_t,size_t>(1,3); }
                else if (traits::case_insensitive_ex::eq(start[0], common_lang_constants::LOWER_W) &&
                    traits::case_insensitive_ex::eq(start[1], common_lang_constants::LOWER_E) &&
                    traits::case_insensitive_ex::eq(start[2], common_lang_constants::LOWER_G))
                    { return std::pair<size_t,size_t>(1,3); }
                }
            if (length >= 2)
                {
                //Irish names with proceeding "Mc" is a separate syllable
                if (traits::case_insensitive_ex::eq(start[0], common_lang_constants::LOWER_M) &&
                    traits::case_insensitive_ex::eq(start[1], common_lang_constants::LOWER_C))
                    { return std::pair<size_t,size_t>(1,2); }
                //separable prefixes, which need to be stripped in case the word is past tense and has a GE in it
                else if (traits::case_insensitive_ex::eq(start[0], common_lang_constants::LOWER_A) &&
                    traits::case_insensitive_ex::eq(start[1], common_lang_constants::LOWER_N))
                    { return std::pair<size_t,size_t>(1,2); }
                else if (traits::case_insensitive_ex::eq(start[0], common_lang_constants::LOWER_A) &&
                    traits::case_insensitive_ex::eq(start[1], common_lang_constants::LOWER_B))
                    { return std::pair<size_t,size_t>(1,2); }
                else if (traits::case_insensitive_ex::eq(start[0], common_lang_constants::LOWER_D) &&
                    traits::case_insensitive_ex::eq(start[1], common_lang_constants::LOWER_A))
                    {
                    if (length >= 3 && traits::case_insensitive_ex::eq(start[2], common_lang_constants::LOWER_U))
                        { return std::pair<size_t,size_t>(0,0); }
                    return std::pair<size_t,size_t>(1,2);
                    }
                else if (traits::case_insensitive_ex::eq(start[0], common_lang_constants::LOWER_E) &&
                    traits::case_insensitive_ex::eq(start[1], common_lang_constants::LOWER_R))
                    { return std::pair<size_t,size_t>(1,2); }
                else if (traits::case_insensitive_ex::eq(start[0], common_lang_constants::LOWER_Z) &&
                    traits::case_insensitive_ex::eq(start[1], common_lang_constants::LOWER_U))
                    { return std::pair<size_t,size_t>(1,2); }
                }
            return std::pair<size_t,size_t>(0,0);
            }
        /** @brief Sees how many syllables are in a block of vowels.
            @param word The original text block under review.
            @param position The position in @c word where the vowel block under review is at.
            @param vowel_block_size How many vowels are in this block to review.
            @param is_first_vowel_block_in_word Whether this is the first block of vowels encountered in @c word.
            @returns The number of syllables in this vowel block.*/
        [[nodiscard]] size_t get_vowel_block_syllable_count(const wchar_t* word,
                                          const size_t position, const size_t vowel_block_size,
                                          const bool is_first_vowel_block_in_word) const noexcept
            {
            assert(word);
            //EOAU (a rare combination)
            if (vowel_block_size == 4 &&
                traits::case_insensitive_ex::eq(word[position], common_lang_constants::LOWER_E) &&
                traits::case_insensitive_ex::eq(word[position+1], common_lang_constants::LOWER_O) &&
                traits::case_insensitive_ex::eq(word[position+2], common_lang_constants::LOWER_A) &&
                traits::case_insensitive_ex::eq(word[position+3], common_lang_constants::LOWER_U))
                { return 3; }
            //other four consecutive vowels would be odd, so just return a syllable count of two
            else if (vowel_block_size > 3)
                { return 2; }
            else if (vowel_block_size == 3)
                {
                //EAU (imported from French) needs to be overridden here first as not splitting.
                if (traits::case_insensitive_ex::eq(word[position], common_lang_constants::LOWER_E) &&
                    traits::case_insensitive_ex::eq(word[position+1], common_lang_constants::LOWER_A) &&
                    traits::case_insensitive_ex::eq(word[position+2], common_lang_constants::LOWER_U))
                    { return 1; }
                //IEE
                else if (traits::case_insensitive_ex::eq(word[position], common_lang_constants::LOWER_I) &&
                    traits::case_insensitive_ex::eq(word[position+1], common_lang_constants::LOWER_E) &&
                    traits::case_insensitive_ex::eq(word[position+2], common_lang_constants::LOWER_E))
                    { return 2; }
                //EEI
                else if (traits::case_insensitive_ex::eq(word[position], common_lang_constants::LOWER_E) &&
                    traits::case_insensitive_ex::eq(word[position+1], common_lang_constants::LOWER_E) &&
                    traits::case_insensitive_ex::eq(word[position+2], common_lang_constants::LOWER_I))
                    { return 2; }
                //EEA (very rare, on in compound words)
                else if (traits::case_insensitive_ex::eq(word[position], common_lang_constants::LOWER_E) &&
                    traits::case_insensitive_ex::eq(word[position+1], common_lang_constants::LOWER_E) &&
                    traits::case_insensitive_ex::eq(word[position+2], common_lang_constants::LOWER_A))
                    { return 2; }
                //AUE
                else if (traits::case_insensitive_ex::eq(word[position], common_lang_constants::LOWER_A) &&
                    traits::case_insensitive_ex::eq(word[position+1], common_lang_constants::LOWER_U) &&
                    traits::case_insensitive_ex::eq(word[position+2], common_lang_constants::LOWER_E))
                    { return 2; }
                //AUI
                else if (traits::case_insensitive_ex::eq(word[position], common_lang_constants::LOWER_A) &&
                    traits::case_insensitive_ex::eq(word[position+1], common_lang_constants::LOWER_U) &&
                    traits::case_insensitive_ex::eq(word[position+2], common_lang_constants::LOWER_I))
                    { return 2; }
                //EUE
                else if (traits::case_insensitive_ex::eq(word[position], common_lang_constants::LOWER_E) &&
                    traits::case_insensitive_ex::eq(word[position+1], common_lang_constants::LOWER_U) &&
                    traits::case_insensitive_ex::eq(word[position+2], common_lang_constants::LOWER_E))
                    { return 2; }
                //EIE
                else if (traits::case_insensitive_ex::eq(word[position], common_lang_constants::LOWER_E) &&
                    traits::case_insensitive_ex::eq(word[position+1], common_lang_constants::LOWER_I) &&
                    traits::case_insensitive_ex::eq(word[position+2], common_lang_constants::LOWER_E))
                    { return 2; }
                //EOÜ (a rare combination)
                else if (traits::case_insensitive_ex::eq(word[position], common_lang_constants::LOWER_E) &&
                    traits::case_insensitive_ex::eq(word[position+1], common_lang_constants::LOWER_O) &&
                    traits::case_insensitive_ex::eq(word[position+2], common_lang_constants::LOWER_U_UMLAUTS))
                    { return 3; }
                else
                    { return 1; }
                }
             else if (vowel_block_size == 2)
                {
                //UELL
                if (m_length >= 4 &&
                    position <= m_length-4 &&
                    traits::case_insensitive_ex::eq(word[position], common_lang_constants::LOWER_U) &&
                    traits::case_insensitive_ex::eq(word[position+1], common_lang_constants::LOWER_E) &&
                    traits::case_insensitive_ex::eq(word[position+2], common_lang_constants::LOWER_L) &&
                    traits::case_insensitive_ex::eq(word[position+3], common_lang_constants::LOWER_L))
                    {
                    //except for "quell"
                    if (position >= 1 && traits::case_insensitive_ex::eq(word[position-1], common_lang_constants::LOWER_Q))
                        { return 1; }
                    else
                        { return 2; }
                    }
                //UAR
                if (m_length >= 3 &&
                    position <= m_length-3 &&
                    traits::case_insensitive_ex::eq(word[position], common_lang_constants::LOWER_U) &&
                    traits::case_insensitive_ex::eq(word[position+1], common_lang_constants::LOWER_A) &&
                    traits::case_insensitive_ex::eq(word[position+2], common_lang_constants::LOWER_R))
                    {
                    //except for "quar"
                    if (position >= 1 && traits::case_insensitive_ex::eq(word[position-1], common_lang_constants::LOWER_Q))
                        { return 1; }
                    else
                        { return 2; }
                    }
                //IO always splits, even in a "TION" ending of a word (unlike English)
                else if (traits::case_insensitive_ex::eq(word[position], common_lang_constants::LOWER_I) &&
                    traits::case_insensitive_ex::eq(word[position+1], common_lang_constants::LOWER_O))
                    { return 2; }
                //IA always splits
                else if (traits::case_insensitive_ex::eq(word[position], common_lang_constants::LOWER_I) &&
                    traits::case_insensitive_ex::eq(word[position+1], common_lang_constants::LOWER_A))
                    { return 2; }
                //EO
                else if (traits::case_insensitive_ex::eq(word[position], common_lang_constants::LOWER_E) &&
                    traits::case_insensitive_ex::eq(word[position+1], common_lang_constants::LOWER_O))
                    { return 2; }
                else if (traits::case_insensitive_ex::eq(word[position], common_lang_constants::LOWER_E) &&
                    traits::case_insensitive_ex::eq(word[position+1], common_lang_constants::LOWER_O_UMLAUTS))
                    { return 2; }
                //kOEfficient (special case)
                else if (position > 0 &&
                    m_length >= 5 &&
                    position <= m_length-4 &&
                    traits::case_insensitive_ex::eq(word[position-1], common_lang_constants::LOWER_K) &&
                    traits::case_insensitive_ex::eq(word[position], common_lang_constants::LOWER_O) &&
                    traits::case_insensitive_ex::eq(word[position+1], common_lang_constants::LOWER_E) &&
                    traits::case_insensitive_ex::eq(word[position+2], common_lang_constants::LOWER_F) &&
                    traits::case_insensitive_ex::eq(word[position+3], common_lang_constants::LOWER_F))
                    { return 2; }
                //EA
                else if (traits::case_insensitive_ex::eq(word[position], common_lang_constants::LOWER_E) &&
                    traits::case_insensitive_ex::eq(word[position+1], common_lang_constants::LOWER_A))
                    { return 2; }
                else if (traits::case_insensitive_ex::eq(word[position], common_lang_constants::LOWER_E) &&
                    traits::case_insensitive_ex::eq(word[position+1], common_lang_constants::LOWER_A_UMLAUTS))
                    { return 2; }
                //OA
                else if (traits::case_insensitive_ex::eq(word[position], common_lang_constants::LOWER_O) &&
                    traits::case_insensitive_ex::eq(word[position+1], common_lang_constants::LOWER_A))
                    {
                    //TOAST would be one syllable
                    if (position == 1 &&
                        traits::case_insensitive_ex::eq(word[position-1], common_lang_constants::LOWER_T))
                        { return 1; }
                    else
                        { return 2; }
                    }
                //IEN
                else if (!is_first_vowel_block_in_word &&
                    m_length >= 4 &&
                    position == m_length-3 &&
                    traits::case_insensitive_ex::eq(word[position], common_lang_constants::LOWER_I) &&
                    traits::case_insensitive_ex::eq(word[position+1], common_lang_constants::LOWER_E) &&
                    traits::case_insensitive_ex::eq(word[position+2], common_lang_constants::LOWER_N) )
                    { return 2; }
                //IENS
                else if (!is_first_vowel_block_in_word &&
                    m_length >= 5 &&
                    position == m_length-4 &&
                    traits::case_insensitive_ex::eq(word[position], common_lang_constants::LOWER_I) &&
                    traits::case_insensitive_ex::eq(word[position+1], common_lang_constants::LOWER_E) &&
                    traits::case_insensitive_ex::eq(word[position+2], common_lang_constants::LOWER_N) &&
                    traits::case_insensitive_ex::eq(word[position+3], common_lang_constants::LOWER_S) )
                    { return 2; }
                //IELL
                else if (!is_first_vowel_block_in_word &&
                    m_length >= 6 &&
                    position <= m_length-4 &&
                    traits::case_insensitive_ex::eq(word[position], common_lang_constants::LOWER_I) &&
                    traits::case_insensitive_ex::eq(word[position+1], common_lang_constants::LOWER_E) &&
                    traits::case_insensitive_ex::eq(word[position+2], common_lang_constants::LOWER_L) &&
                    traits::case_insensitive_ex::eq(word[position+3], common_lang_constants::LOWER_L) )
                    { return 2; }
                else
                    { return 1; }
                }
            else
                { return 1; }
            }
        };
    }

/** @}*/

#endif //__GERMAN_SYLLABLE_H__
