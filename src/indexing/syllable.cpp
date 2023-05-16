///////////////////////////////////////////////////////////////////////////////
// Name:        syllable.cpp
// Author:      Blake Madden
// Copyright:   (c) 2005-2023 Blake Madden
// Licence:     3-Clause BSD licence
// SPDX-License-Identifier: BSD-3-Clause
///////////////////////////////////////////////////////////////////////////////

#include "syllable.h"

namespace grammar
    {
    std::set<traits::case_insensitive_wstring_ex> english_syllabize::m_e_disconnecting_prefixes =
        { L"corr", L"minn", L"diss", L"disc", L"comm",
          L"dysp", L"ind", L"cer", L"alg", L"coh",
          L"art", L"ben", L"tel", L"dis", L"irr", L"all",
          L"alp", L"amn", L"arr", L"agg", L"ass", L"eff",
          L"ell", L"ill", L"imm", L"un", L"dand" };
    std::set<traits::case_insensitive_wstring_ex> english_syllabize::m_non_affecting_suffixes_4 =
        { L"ness", L"ment", L"room", L"shoe", L"pick",
          L"maid", L"yard", L"book", L"hill" };;

    std::pair<bool,size_t> base_syllabize::is_special_math_word(const wchar_t* start, const size_t length) noexcept
        {
        assert(start);
        if (length == 2 &&
            characters::is_character::is_numeric_simple(start[0]) &&
            traits::case_insensitive_ex::eq(start[1], common_lang_constants::LOWER_D) )
            { return std::pair<bool,size_t>(true,2); }
        else
            { return std::pair<bool,size_t>(false,0); }
        }

    size_t base_syllabize::get_symbol_syllable_count(const wchar_t* start,
                                                     const wchar_t* end,
                                                     const wchar_t* current_char) noexcept
        {
        assert(start);
        assert(end);
        assert(current_char);
        if (current_char[0] == common_lang_constants::POUND)
            { return 1; }
        //"plus/minus" if symbol is at front of word
        else if (start == current_char &&
            traits::case_insensitive_ex::eq(current_char[0], 177))
            { return 3; }
        //"dollars" or "pesos" (Cuban) if dollar symbol is at front of word
        else if (start == current_char &&
            (traits::case_insensitive_ex::eq(current_char[0], common_lang_constants::DOLLAR_SIGN) ||
             traits::case_insensitive_ex::eq(current_char[0], 0x20B1)))
            { return 2; }
        // Euro
        else if (start == current_char &&
            traits::case_insensitive_ex::eq(current_char[0], 0x20AC))
            { return 2; }
        // Korean "won" symbol
        else if (start == current_char &&
            traits::case_insensitive_ex::eq(current_char[0], 0x20A9))
            { return 1; }
        // Pound Sterling/"quid" if symbol is at front of word
        else if (start == current_char &&
            traits::case_insensitive_ex::eq(current_char[0], 163))
            { return 1; }
        //"cents" if  symbol is at end of word
        else if ((end-1) == current_char &&
            traits::case_insensitive_ex::eq(current_char[0], 162))
            { return 1; }
        //"percent"/"degrees"/"one-half"/"one-fourth"/"three-fourth" if at end of word
        else if ((end-1) == current_char &&
            (traits::case_insensitive_ex::eq(current_char[0], common_lang_constants::PERCENTAGE_SIGN) ||
                traits::case_insensitive_ex::eq(current_char[0], 176) ||
                traits::case_insensitive_ex::eq(current_char[0], 188) ||
                traits::case_insensitive_ex::eq(current_char[0], 189) ||
                traits::case_insensitive_ex::eq(current_char[0], 190)) )
            { return 2; }
        // fractions, super and subscripts
        else if (string_util::is_fraction(current_char[0]) ||
                 string_util::is_superscript(current_char[0]) ||
                 string_util::is_subscript(current_char[0]))
            { return 1; }
        else
            { return 0; }
        }

    void base_syllabize::adjust_length_if_possesive(const wchar_t* start)
        {
        assert(start);
        if (start == nullptr)
             { return; }
        if (m_length >= 3 &&
            isChar.is_apostrophe(start[m_length-2]) &&
            traits::case_insensitive_ex::eq(start[m_length-1], common_lang_constants::LOWER_S))
            { m_length -= 2; }
        else if (m_length >= 2 && isChar.is_apostrophe(start[m_length-1]))
            { --m_length; }
        }

    bool base_syllabize::is_consonant_y(const wchar_t* word, size_t position) const
        {
        assert(word);
        assert(traits::case_insensitive_ex::eq(word[position], common_lang_constants::LOWER_Y) );
        if (word == nullptr)
             { return false; }
        if (!traits::case_insensitive_ex::eq(word[position], common_lang_constants::LOWER_Y) )
            { return false; }
        // if 'y' begins word then it cannot be a vowel sound
        else if (position == 0)
            { return true; }
        // or if after another vowel (alloyed)
        else if (isChar.is_vowel(word[position-1]) )
            {
            // boy
            return true;
            }
        // tanya, anyu
        else if (position+2 == m_length &&
            isChar.is_vowel(word[position+1]) )
            { return true; }
        // lAwyer, but watch out for compounds like "hIghflYer"
        else if ((m_previous_vowel != m_length) &&
                (position-m_previous_vowel) < 3 &&
                (position+2 < m_length) &&
                traits::case_insensitive_ex::eq(word[position+1], common_lang_constants::LOWER_E) &&
                traits::case_insensitive_ex::eq(word[position+2], common_lang_constants::LOWER_R))
            { return true; }
        // churchyard
        else if ((position+3 < m_length) &&
                traits::case_insensitive_ex::eq(word[position+1], common_lang_constants::LOWER_A) &&
                traits::case_insensitive_ex::eq(word[position+2], common_lang_constants::LOWER_R) &&
                traits::case_insensitive_ex::eq(word[position+3], common_lang_constants::LOWER_D))
            { return true; }
        else
            { return false; }
        }

    size_t english_syllabize::operator()(const wchar_t* start, const size_t length)
        {
        // reset our data
        reset();
        m_previous_vowel = 0;
        m_previous_block_vowel = 0;
        m_was_last_vowel_block_separable_vowels = false;
        m_ends_with_nt_contraction = false;
        // sanity checks
        assert(start && "null string passed to syllable parser");
        if (start == nullptr || length == 0)
            { return 0; }

        m_length = length;
        adjust_length_if_possesive(start);
        const wchar_t* end = start + m_length;

        m_ends_with_nt_contraction = (m_length >= 4 &&
            traits::case_insensitive_ex::eq(start[m_length-3], common_lang_constants::LOWER_N) &&
            isChar.is_apostrophe(start[m_length-2]) &&
            traits::case_insensitive_ex::eq(start[m_length-1], common_lang_constants::LOWER_T));

        const std::pair<bool,size_t> mathResult = is_special_math_word(start, m_length);
        if (mathResult.first)
            { return m_syllable_count = mathResult.second; }

        if (syllabize_if_contains_periods<english_syllabize>(start, end))
            { return m_syllable_count; }

        if (syllabize_if_contains_dashes<english_syllabize>(start))
            { return m_syllable_count; }

        const std::pair<size_t,size_t> prefixResult = get_prefix_length(start, m_length);
        if (prefixResult.second > 0)
            {
            start += prefixResult.second;
            m_length -= prefixResult.second;
            m_syllable_count += prefixResult.first;
            }

        m_previous_block_vowel = m_previous_vowel = m_length;
        bool is_in_vowel_block = false;
        bool current_char_is_vowel = false;
        const wchar_t* current_char = start;
        bool wasLastVowelSilentE = false;

        while (current_char != end)
            {
            current_char_is_vowel = isChar.is_vowel(current_char[0]);
            current_char_is_vowel =
                (traits::case_insensitive_ex::eq(current_char[0],
                    common_lang_constants::LOWER_Y) && is_consonant_y(start, current_char - start) ) ?
                false : current_char_is_vowel;
            current_char_is_vowel =
                (traits::case_insensitive_ex::eq(current_char[0],
                    common_lang_constants::LOWER_U) && is_silent_u(start, current_char - start) ) ?
                false : current_char_is_vowel;

            bool next_char_is_vowel = false;
            /* if last letter, then there is no next letter*/
            if ((m_length-1) == static_cast<size_t>(current_char-start) )
                { next_char_is_vowel = false; }
            else
                {
                next_char_is_vowel = isChar.is_vowel(current_char[1]);
                next_char_is_vowel =
                    (traits::case_insensitive_ex::eq(current_char[1],
                        common_lang_constants::LOWER_Y) && is_consonant_y(start, (current_char+1) - start) ) ?
                    false : next_char_is_vowel;
                next_char_is_vowel =
                    (traits::case_insensitive_ex::eq(current_char[1],
                        common_lang_constants::LOWER_U) && is_silent_u(start, (current_char+1) - start) ) ?
                    false : next_char_is_vowel;
                }

            is_in_vowel_block = current_char_is_vowel && next_char_is_vowel;
            // if it's a vowel and it's the only one in this block
            if (current_char_is_vowel && !is_in_vowel_block)
                {
                // only check an 'e' for silence if there was already a previos vowel
                if (m_previous_vowel != m_length &&
                    !wasLastVowelSilentE &&
                    (traits::case_insensitive_ex::eq(current_char[0], common_lang_constants::LOWER_E) &&
                    is_silent_e(start, (current_char-start)) ) )
                    {
                    // if 'e' is silent then treat it as part of the previous sound
                    // and not part of a multiple vowel sound
                    wasLastVowelSilentE = true;
                    }
                else if (!traits::case_insensitive_ex::eq(current_char[0], common_lang_constants::LOWER_E) &&
                    is_single_non_e_vowel_ignored(start, (current_char-start)) )
                    { /* NOOP, just skip this vowel*/ }
                else
                    {
                    ++m_syllable_count;
                    wasLastVowelSilentE = false;
                    }
                m_was_last_vowel_block_separable_vowels = false;
                m_previous_vowel = current_char - start;
                }
            else if (current_char_is_vowel && is_in_vowel_block)
                {
                wasLastVowelSilentE = false;
                const wchar_t* start_of_block = current_char;
                while (current_char != end &&
                    isChar.is_vowel(current_char[1]) )
                    {
                    ++current_char;
                    }
                // if it is two consecutive vowels then make sure they
                // aren't separate syllables
                // io[ai] is 3 syllables
                if ( ((current_char+1)-start_of_block) == 3 &&
                    traits::case_insensitive_ex::eq(start[start_of_block - start],
                        common_lang_constants::LOWER_I) &&
                    traits::case_insensitive_ex::eq(start[(start_of_block - start) + 1],
                        common_lang_constants::LOWER_O) &&
                    (traits::case_insensitive_ex::eq(start[(start_of_block - start) + 2],
                        common_lang_constants::LOWER_I) ||
                     traits::case_insensitive_ex::eq(start[(start_of_block - start) + 2],
                         common_lang_constants::LOWER_A)) )
                    {
                    m_was_last_vowel_block_separable_vowels = true;
                    m_syllable_count += 3;
                    }
                else if (is_vowels_separate_syllables(start,
                        start_of_block - start,
                        (current_char+1)-start_of_block,
                        m_previous_block_vowel > static_cast<size_t>(start_of_block-start)) )
                    {
                    m_was_last_vowel_block_separable_vowels = true;
                    m_syllable_count += 2;
                    }
                else
                    {
                    ++m_syllable_count;
                    m_was_last_vowel_block_separable_vowels = false;
                    }
                m_previous_vowel = current_char - start;
                }
            // syllabize numbers
            else if (characters::is_character::is_numeric_simple(current_char[0]))
                {
                size_t characters_counted = 0;
                m_syllable_count +=
                    syllabify_numeral<syllabize_english_number>(current_char, end, characters_counted,
                                                                common_lang_constants::COMMA,
                                                                common_lang_constants::PERIOD);
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

    void english_syllabize::finalize_special_cases(const wchar_t* start)
        {
        if (start == nullptr)
            { return; }
        // Irish names with proceeding "Mc" is a separate syllable
        if (m_length >= 2 &&
            traits::case_insensitive_ex::eq(start[0], common_lang_constants::LOWER_M) &&
            traits::case_insensitive_ex::eq(start[1], common_lang_constants::LOWER_C))
            {
            ++m_syllable_count;
            return;
            }
        // that'll, it'll, what'll, they'll are 2 syllables
        else if (m_length >= 5 &&
            isChar.is_apostrophe(start[m_length-3]) &&
            traits::case_insensitive_ex::eq(start[m_length-2], common_lang_constants::LOWER_L) &&
            traits::case_insensitive_ex::eq(start[m_length-1], common_lang_constants::LOWER_L))
            {
            // that, it, what, they
            if ((traits::case_insensitive_ex::eq(start[0], common_lang_constants::LOWER_T) &&
                    traits::case_insensitive_ex::eq(start[1], common_lang_constants::LOWER_H) &&
                    traits::case_insensitive_ex::eq(start[2], common_lang_constants::LOWER_A) &&
                    traits::case_insensitive_ex::eq(start[3], common_lang_constants::LOWER_T)) ||
                (traits::case_insensitive_ex::eq(start[0], common_lang_constants::LOWER_I) &&
                    traits::case_insensitive_ex::eq(start[1], common_lang_constants::LOWER_T)) ||
                (traits::case_insensitive_ex::eq(start[0], common_lang_constants::LOWER_W) &&
                    traits::case_insensitive_ex::eq(start[1], common_lang_constants::LOWER_H) &&
                    traits::case_insensitive_ex::eq(start[2], common_lang_constants::LOWER_A) &&
                    traits::case_insensitive_ex::eq(start[3], common_lang_constants::LOWER_T)) ||
                (traits::case_insensitive_ex::eq(start[0], common_lang_constants::LOWER_T) &&
                    traits::case_insensitive_ex::eq(start[1], common_lang_constants::LOWER_H) &&
                    traits::case_insensitive_ex::eq(start[2], common_lang_constants::LOWER_E) &&
                    traits::case_insensitive_ex::eq(start[3], common_lang_constants::LOWER_Y)) )
                {
                ++m_syllable_count;
                return;
                }
            }
        // Special case contractions
        else if (m_ends_with_nt_contraction)
            {
            // should(n't)
            if (m_length >= 6 &&
                traits::case_insensitive_ex::eq(start[0], common_lang_constants::LOWER_S) &&
                traits::case_insensitive_ex::eq(start[1], common_lang_constants::LOWER_H) &&
                traits::case_insensitive_ex::eq(start[2], common_lang_constants::LOWER_O) &&
                traits::case_insensitive_ex::eq(start[3], common_lang_constants::LOWER_U) &&
                traits::case_insensitive_ex::eq(start[4], common_lang_constants::LOWER_L) &&
                traits::case_insensitive_ex::eq(start[5], common_lang_constants::LOWER_D) )
                {
                ++m_syllable_count;
                return;
                }
            // might, would, could, ought
            else if (m_length >= 5 &&
                // might
                ((traits::case_insensitive_ex::eq(start[0], common_lang_constants::LOWER_M) &&
                    traits::case_insensitive_ex::eq(start[1], common_lang_constants::LOWER_I) &&
                    traits::case_insensitive_ex::eq(start[2], common_lang_constants::LOWER_G) &&
                    traits::case_insensitive_ex::eq(start[3], common_lang_constants::LOWER_H) &&
                    traits::case_insensitive_ex::eq(start[4], common_lang_constants::LOWER_T)) ||
                // would/could
                ((traits::case_insensitive_ex::eq(start[0], common_lang_constants::LOWER_W) ||
                    traits::case_insensitive_ex::eq(start[0], common_lang_constants::LOWER_C)) &&
                    traits::case_insensitive_ex::eq(start[1], common_lang_constants::LOWER_O) &&
                    traits::case_insensitive_ex::eq(start[2], common_lang_constants::LOWER_U) &&
                    traits::case_insensitive_ex::eq(start[3], common_lang_constants::LOWER_L) &&
                    traits::case_insensitive_ex::eq(start[4], common_lang_constants::LOWER_D)) ||
                // ought
                (traits::case_insensitive_ex::eq(start[0], common_lang_constants::LOWER_O) &&
                    traits::case_insensitive_ex::eq(start[1], common_lang_constants::LOWER_U) &&
                    traits::case_insensitive_ex::eq(start[2], common_lang_constants::LOWER_G) &&
                    traits::case_insensitive_ex::eq(start[3], common_lang_constants::LOWER_H) &&
                    traits::case_insensitive_ex::eq(start[4], common_lang_constants::LOWER_T)) ))
                {
                ++m_syllable_count;
                return;
                }
            else if (m_length >= 4 &&
                // does
                ((traits::case_insensitive_ex::eq(start[0], common_lang_constants::LOWER_D) &&
                    traits::case_insensitive_ex::eq(start[1], common_lang_constants::LOWER_O) &&
                    traits::case_insensitive_ex::eq(start[2], common_lang_constants::LOWER_E) &&
                    traits::case_insensitive_ex::eq(start[3], common_lang_constants::LOWER_S)) ||
                // must
                (traits::case_insensitive_ex::eq(start[0], common_lang_constants::LOWER_M) &&
                    traits::case_insensitive_ex::eq(start[1], common_lang_constants::LOWER_U) &&
                    traits::case_insensitive_ex::eq(start[2], common_lang_constants::LOWER_S) &&
                    traits::case_insensitive_ex::eq(start[3], common_lang_constants::LOWER_T)) ||
                // need
                (traits::case_insensitive_ex::eq(start[0], common_lang_constants::LOWER_N) &&
                    traits::case_insensitive_ex::eq(start[1], common_lang_constants::LOWER_E) &&
                    traits::case_insensitive_ex::eq(start[2], common_lang_constants::LOWER_E) &&
                    traits::case_insensitive_ex::eq(start[3], common_lang_constants::LOWER_D)) ||
                // have
                (traits::case_insensitive_ex::eq(start[0], common_lang_constants::LOWER_H) &&
                    traits::case_insensitive_ex::eq(start[1], common_lang_constants::LOWER_A) &&
                    traits::case_insensitive_ex::eq(start[2], common_lang_constants::LOWER_V) &&
                    traits::case_insensitive_ex::eq(start[3], common_lang_constants::LOWER_E)) ))
                {
                ++m_syllable_count;
                return;
                }
            else if (m_length >= 3 &&
                // has
                ((traits::case_insensitive_ex::eq(start[0], common_lang_constants::LOWER_H) &&
                    traits::case_insensitive_ex::eq(start[1], common_lang_constants::LOWER_A) &&
                    traits::case_insensitive_ex::eq(start[2], common_lang_constants::LOWER_S)) ||
                // had
                (traits::case_insensitive_ex::eq(start[0], common_lang_constants::LOWER_H) &&
                    traits::case_insensitive_ex::eq(start[1], common_lang_constants::LOWER_A) &&
                    traits::case_insensitive_ex::eq(start[2], common_lang_constants::LOWER_D)) ||
                // did
                (traits::case_insensitive_ex::eq(start[0], common_lang_constants::LOWER_D) &&
                    traits::case_insensitive_ex::eq(start[1], common_lang_constants::LOWER_I) &&
                    traits::case_insensitive_ex::eq(start[2], common_lang_constants::LOWER_D)) ||
                    // was
                    (traits::case_insensitive_ex::eq(start[0], common_lang_constants::LOWER_W) &&
                    traits::case_insensitive_ex::eq(start[1], common_lang_constants::LOWER_A) &&
                    traits::case_insensitive_ex::eq(start[2], common_lang_constants::LOWER_S)) ))
                {
                ++m_syllable_count;
                return;
                }
            else if (m_length >= 2 &&
                // is
                traits::case_insensitive_ex::eq(start[0], common_lang_constants::LOWER_I) &&
                traits::case_insensitive_ex::eq(start[1], common_lang_constants::LOWER_S))
                {
                ++m_syllable_count;
                return;
                }
            }
        }

    bool english_syllabize::is_silent_u(const wchar_t* word, size_t position) const
        {
        assert(word);
        // verify that is a 'u' first
        assert(traits::case_insensitive_ex::eq(word[position], common_lang_constants::LOWER_U) );
        if (word == nullptr)
            { return false; }
        if (!traits::case_insensitive_ex::eq(word[position], common_lang_constants::LOWER_U) )
            { return false; }
        //"qu" makes the 'u' silent
        else if ( (position > 0) &&
                traits::case_insensitive_ex::eq(word[position-1], common_lang_constants::LOWER_Q) )
            { return true; }
        //"gu[vowel]" makes the 'u' silent (forms a "gw" sound)
        // exceptions exists though, such as "ambiguity"
        else if ( (position > 0) &&
                (position+2 <= m_length) &&
                traits::case_insensitive_ex::eq(word[position-1], common_lang_constants::LOWER_G) &&
                (isChar.is_vowel(word[position+1]) ) )
            {
            // watch out for out vowel combinations such as "guous"
            if (position+3 <= m_length &&
                isChar.is_vowel(word[position+1]) &&
                isChar.is_vowel(word[position+2]) )
                { return false; }
            // ambiguity
            else if (position+3 <= m_length &&
                traits::case_insensitive_ex::eq(word[position+1], common_lang_constants::LOWER_I) &&
                // position+1 already determined to be a consonant
                traits::case_insensitive_ex::eq(word[position+3], common_lang_constants::LOWER_Y) )
                { return false; }
            else
                { return true; }
            }
        else
            { return false; }
        }

    bool english_syllabize::does_prefix_silence_e(const wchar_t* word, size_t prefix_length) const
        {
        assert(word);
        if (word == nullptr)
            { return false; }
        if (prefix_length == 3)
            {
            // fore
            if (traits::case_insensitive_ex::eq(word[0], common_lang_constants::LOWER_F) &&
                traits::case_insensitive_ex::eq(word[1], common_lang_constants::LOWER_O) &&
                traits::case_insensitive_ex::eq(word[2], common_lang_constants::LOWER_R))
                {
                // exceptions: forest...
                if (m_length >= 6 &&
                    traits::case_insensitive_ex::eq(word[4], common_lang_constants::LOWER_S) &&
                    traits::case_insensitive_ex::eq(word[5], common_lang_constants::LOWER_T))
                    {
                    // fore-stall
                    if (m_length >= 7 && traits::case_insensitive_ex::eq(word[6], common_lang_constants::LOWER_A))
                        { return true; }
                    else
                        { return false; }
                    }
                // forex
                else if (m_length >= 5 &&
                    traits::case_insensitive_ex::eq(word[4], common_lang_constants::LOWER_X) )
                    { return false; }
                // for-en-sic
                else if (m_length >= 8 &&
                    traits::case_insensitive_ex::eq(word[4], common_lang_constants::LOWER_N) &&
                    traits::case_insensitive_ex::eq(word[5], common_lang_constants::LOWER_S) &&
                    traits::case_insensitive_ex::eq(word[6], common_lang_constants::LOWER_I) &&
                    traits::case_insensitive_ex::eq(word[7], common_lang_constants::LOWER_C))
                    { return false; }
                // for-e-ver
                else if (m_length >= 5 &&
                    traits::case_insensitive_ex::eq(word[4], common_lang_constants::LOWER_V))
                    { return false; }
                // fore-sight
                else
                    { return true; }
                }
            else
                { return false; }
            }
        else
            { return false; }
        }

    bool english_syllabize::does_prefix_disconnect_e(const wchar_t* word, size_t prefix_length)
        {
        return m_e_disconnecting_prefixes.find(traits::case_insensitive_wstring_ex(word, prefix_length)) !=
                m_e_disconnecting_prefixes.end();
        }

    bool english_syllabize::is_none_affecting_suffix(const wchar_t* word, size_t suffix_length)
        {
        assert(word);
        if (suffix_length >= 4 &&
            (m_non_affecting_suffixes_4.find(
                traits::case_insensitive_wstring_ex(word, 4)) != m_non_affecting_suffixes_4.end()) )
            { return true; }
        else if (suffix_length >= 3 &&
            ((traits::case_insensitive_ex::eq(word[0], common_lang_constants::LOWER_M) &&
                traits::case_insensitive_ex::eq(word[1], common_lang_constants::LOWER_A) &&
                traits::case_insensitive_ex::eq(word[2], common_lang_constants::LOWER_N)) ||
            (traits::case_insensitive_ex::eq(word[0], common_lang_constants::LOWER_B) &&
                traits::case_insensitive_ex::eq(word[1], common_lang_constants::LOWER_O) &&
                traits::case_insensitive_ex::eq(word[2], common_lang_constants::LOWER_X)) ))
            { return true; }
        else if (suffix_length >= 2 &&
            traits::case_insensitive_ex::eq(word[0], common_lang_constants::LOWER_L) &&
            traits::case_insensitive_ex::eq(word[1], common_lang_constants::LOWER_Y))
            { return true; }
        return false;
        }

    bool english_syllabize::does_suffix_negate_silent_e(const wchar_t* suffix, const size_t suffix_length,
            const size_t next_vowel_index) const
        {
        assert(suffix);
        if (suffix == nullptr)
            { return false; }
        // Check for explicitly known suffixes that can follow a silent 'e'.
        // Just return true from here for these and the parent logic will further attempt to figure out
        // whether this is really a silent 'e.'
        if (suffix_length >= 5 &&
            traits::case_insensitive_ex::eq(suffix[0], common_lang_constants::LOWER_T) &&
            traits::case_insensitive_ex::eq(suffix[1], common_lang_constants::LOWER_H) &&
            traits::case_insensitive_ex::eq(suffix[2], common_lang_constants::LOWER_I) &&
            traits::case_insensitive_ex::eq(suffix[3], common_lang_constants::LOWER_N) &&
            traits::case_insensitive_ex::eq(suffix[4], common_lang_constants::LOWER_G))
            { return false; }
        else if (suffix_length >= 5 &&
            traits::case_insensitive_ex::eq(suffix[0], common_lang_constants::LOWER_L) &&
            traits::case_insensitive_ex::eq(suffix[1], common_lang_constants::LOWER_I) &&
            traits::case_insensitive_ex::eq(suffix[2], common_lang_constants::LOWER_E) &&
            traits::case_insensitive_ex::eq(suffix[3], common_lang_constants::LOWER_S) &&
            traits::case_insensitive_ex::eq(suffix[4], common_lang_constants::LOWER_T))
            { return false; }
        else if (suffix_length >= 4 &&
            traits::case_insensitive_ex::eq(suffix[0], common_lang_constants::LOWER_W) &&
            traits::case_insensitive_ex::eq(suffix[1], common_lang_constants::LOWER_R) &&
            traits::case_insensitive_ex::eq(suffix[2], common_lang_constants::LOWER_I) &&
            traits::case_insensitive_ex::eq(suffix[3], common_lang_constants::LOWER_T))
            { return false; }
        // The rest will negate
        // harv-e-ster
        else if (suffix_length >= 4 &&
            traits::case_insensitive_ex::eq(suffix[0], common_lang_constants::LOWER_S) &&
            traits::case_insensitive_ex::eq(suffix[1], common_lang_constants::LOWER_T) &&
            traits::case_insensitive_ex::eq(suffix[2], common_lang_constants::LOWER_E) &&
            traits::case_insensitive_ex::eq(suffix[3], common_lang_constants::LOWER_R))
            { return true; }
        // Pam-e-la
        else if (suffix_length == 2 &&
            traits::case_insensitive_ex::eq(suffix[0], common_lang_constants::LOWER_L) &&
            traits::case_insensitive_ex::eq(suffix[1], common_lang_constants::LOWER_A))
            { return true; }
        // Pam-e-las
        else if (suffix_length == 3 &&
            traits::case_insensitive_ex::eq(suffix[0], common_lang_constants::LOWER_L) &&
            traits::case_insensitive_ex::eq(suffix[1], common_lang_constants::LOWER_A) &&
            traits::case_insensitive_ex::eq(suffix[1], common_lang_constants::LOWER_S))
            { return true; }
        // Cam-e-lot, oc-e-lot
        else if (suffix_length >= 3 &&
            traits::case_insensitive_ex::eq(suffix[0], common_lang_constants::LOWER_L) &&
            traits::case_insensitive_ex::eq(suffix[1], common_lang_constants::LOWER_O) &&
            traits::case_insensitive_ex::eq(suffix[2], common_lang_constants::LOWER_T))
            { return true; }
        // inebriate
        else if ((suffix_length-next_vowel_index) >= 4 &&
            ((traits::case_insensitive_ex::eq(suffix[next_vowel_index], common_lang_constants::LOWER_I) &&
                traits::case_insensitive_ex::eq(suffix[next_vowel_index+1], common_lang_constants::LOWER_A) &&
                traits::case_insensitive_ex::eq(suffix[next_vowel_index+2], common_lang_constants::LOWER_T) &&
                traits::case_insensitive_ex::eq(suffix[next_vowel_index+3], common_lang_constants::LOWER_E)) ||
            (traits::case_insensitive_ex::eq(suffix[next_vowel_index], common_lang_constants::LOWER_I) &&
                traits::case_insensitive_ex::eq(suffix[next_vowel_index+1], common_lang_constants::LOWER_E) &&
                traits::case_insensitive_ex::eq(suffix[next_vowel_index+2], common_lang_constants::LOWER_T) &&
                traits::case_insensitive_ex::eq(suffix[next_vowel_index+3], common_lang_constants::LOWER_Y)) ))
            { return true; }
        // celebrate, procedure
        else if ((suffix_length-next_vowel_index) >= 3 &&
            ((traits::case_insensitive_ex::eq(suffix[next_vowel_index], common_lang_constants::LOWER_A) &&
                traits::case_insensitive_ex::eq(suffix[next_vowel_index+1], common_lang_constants::LOWER_T) &&
                traits::case_insensitive_ex::eq(suffix[next_vowel_index+2], common_lang_constants::LOWER_E)) ||
            (traits::case_insensitive_ex::eq(suffix[next_vowel_index], common_lang_constants::LOWER_A) &&
                traits::case_insensitive_ex::eq(suffix[next_vowel_index+1], common_lang_constants::LOWER_T) &&
                traits::case_insensitive_ex::eq(suffix[next_vowel_index+2], common_lang_constants::LOWER_I)) ||
            (traits::case_insensitive_ex::eq(suffix[next_vowel_index], common_lang_constants::LOWER_E) &&
                traits::case_insensitive_ex::eq(suffix[next_vowel_index+1], common_lang_constants::LOWER_C) &&
                traits::case_insensitive_ex::eq(suffix[next_vowel_index+2], common_lang_constants::LOWER_T)) ||
            (traits::case_insensitive_ex::eq(suffix[next_vowel_index], common_lang_constants::LOWER_I) &&
                traits::case_insensitive_ex::eq(suffix[next_vowel_index+1], common_lang_constants::LOWER_E) &&
                traits::case_insensitive_ex::eq(suffix[next_vowel_index+2], common_lang_constants::LOWER_D)) ||
            (traits::case_insensitive_ex::eq(suffix[next_vowel_index], common_lang_constants::LOWER_I) &&
                traits::case_insensitive_ex::eq(suffix[next_vowel_index+1], common_lang_constants::LOWER_E) &&
                traits::case_insensitive_ex::eq(suffix[next_vowel_index+2], common_lang_constants::LOWER_S)) ||
            (traits::case_insensitive_ex::eq(suffix[next_vowel_index], common_lang_constants::LOWER_I) &&
                traits::case_insensitive_ex::eq(suffix[next_vowel_index+1], common_lang_constants::LOWER_Z) &&
                traits::case_insensitive_ex::eq(suffix[next_vowel_index+2], common_lang_constants::LOWER_E)) ||
            (traits::case_insensitive_ex::eq(suffix[next_vowel_index], common_lang_constants::LOWER_U) &&
                traits::case_insensitive_ex::eq(suffix[next_vowel_index+1], common_lang_constants::LOWER_R) &&
                traits::case_insensitive_ex::eq(suffix[next_vowel_index+2], common_lang_constants::LOWER_E)) ||
            (traits::case_insensitive_ex::eq(suffix[next_vowel_index], common_lang_constants::LOWER_A) &&
                traits::case_insensitive_ex::eq(suffix[next_vowel_index+1], common_lang_constants::LOWER_N) &&
                traits::case_insensitive_ex::eq(suffix[next_vowel_index+2], common_lang_constants::LOWER_T)) ||
            (traits::case_insensitive_ex::eq(suffix[next_vowel_index], common_lang_constants::LOWER_A) &&
                traits::case_insensitive_ex::eq(suffix[next_vowel_index+1], common_lang_constants::LOWER_N) &&
                traits::case_insensitive_ex::eq(suffix[next_vowel_index+2], common_lang_constants::LOWER_C)) ||
            (traits::case_insensitive_ex::eq(suffix[next_vowel_index], common_lang_constants::LOWER_U) &&
                traits::case_insensitive_ex::eq(suffix[next_vowel_index+1], common_lang_constants::LOWER_T)) ))
            { return true; }
        // angelic, generic, benefice, bedevil
        else if ((suffix_length-next_vowel_index) >= 2 &&
            traits::case_insensitive_ex::eq(suffix[next_vowel_index], common_lang_constants::LOWER_I) &&
            (traits::case_insensitive_ex::eq(suffix[next_vowel_index+1], common_lang_constants::LOWER_C) ||
                traits::case_insensitive_ex::eq(suffix[next_vowel_index+1], common_lang_constants::LOWER_T) ||
                traits::case_insensitive_ex::eq(suffix[next_vowel_index+1], common_lang_constants::LOWER_L)) )
            { return true; }
        /*if the word ends like "e[consonant block]e" then the front 'e' must be part of the last
        syllable and the last 'e' will be silent*/
        else if ((suffix_length-next_vowel_index) == 1 &&
            traits::case_insensitive_ex::eq(suffix[next_vowel_index], common_lang_constants::LOWER_E) )
            { return true; }
        /*if the word ends like "e[consonant block][es][er][ed][or][ing]" then the front 'e' must be part of the last
        syllable*/
        else if ((suffix_length-next_vowel_index) == 2 &&
            ((traits::case_insensitive_ex::eq(suffix[next_vowel_index], common_lang_constants::LOWER_E) &&
                (traits::case_insensitive_ex::eq(suffix[next_vowel_index+1], common_lang_constants::LOWER_S) ||
                    traits::case_insensitive_ex::eq(suffix[next_vowel_index+1], common_lang_constants::LOWER_D) ||
                    traits::case_insensitive_ex::eq(suffix[next_vowel_index+1], common_lang_constants::LOWER_R))) ||
            (traits::case_insensitive_ex::eq(suffix[next_vowel_index], common_lang_constants::LOWER_O) &&
                traits::case_insensitive_ex::eq(suffix[next_vowel_index+1], common_lang_constants::LOWER_R)) ))
            { return true; }
        // forEsting--the middle 'e' is part of a separate vowel
        else if ((suffix_length-next_vowel_index) == 3 &&
            traits::case_insensitive_ex::eq(suffix[next_vowel_index], common_lang_constants::LOWER_I) &&
            traits::case_insensitive_ex::eq(suffix[next_vowel_index+1], common_lang_constants::LOWER_N) &&
            traits::case_insensitive_ex::eq(suffix[next_vowel_index+2], common_lang_constants::LOWER_G))
            { return true; }
        else
            {
            if (suffix_length >= 6 &&
                traits::case_insensitive_ex::eq(suffix[0], common_lang_constants::LOWER_S) &&
                traits::case_insensitive_ex::eq(suffix[1], common_lang_constants::LOWER_C) &&
                traits::case_insensitive_ex::eq(suffix[2], common_lang_constants::LOWER_E) &&
                traits::case_insensitive_ex::eq(suffix[3], common_lang_constants::LOWER_N) &&
                traits::case_insensitive_ex::eq(suffix[4], common_lang_constants::LOWER_C) &&
                traits::case_insensitive_ex::eq(suffix[5], common_lang_constants::LOWER_E))
                { return true; }
            else if (suffix_length >= 5 &&
                ((traits::case_insensitive_ex::eq(suffix[0], common_lang_constants::LOWER_M) &&
                    traits::case_insensitive_ex::eq(suffix[1], common_lang_constants::LOWER_A) &&
                    traits::case_insensitive_ex::eq(suffix[2], common_lang_constants::LOWER_T) &&
                    traits::case_insensitive_ex::eq(suffix[3], common_lang_constants::LOWER_I) &&
                    traits::case_insensitive_ex::eq(suffix[4], common_lang_constants::LOWER_C)) ||
                // scend, scent
                (traits::case_insensitive_ex::eq(suffix[0], common_lang_constants::LOWER_S) &&
                    traits::case_insensitive_ex::eq(suffix[1], common_lang_constants::LOWER_C) &&
                    traits::case_insensitive_ex::eq(suffix[2], common_lang_constants::LOWER_E) &&
                    traits::case_insensitive_ex::eq(suffix[3], common_lang_constants::LOWER_N) &&
                    (traits::case_insensitive_ex::eq(suffix[4], common_lang_constants::LOWER_D) ||
                        traits::case_insensitive_ex::eq(suffix[4], common_lang_constants::LOWER_T)) ) ||
                (traits::case_insensitive_ex::eq(suffix[0], common_lang_constants::LOWER_L) &&
                    traits::case_insensitive_ex::eq(suffix[1], common_lang_constants::LOWER_E) &&
                    traits::case_insensitive_ex::eq(suffix[2], common_lang_constants::LOWER_T) &&
                    traits::case_insensitive_ex::eq(suffix[3], common_lang_constants::LOWER_T) &&
                    traits::case_insensitive_ex::eq(suffix[4], common_lang_constants::LOWER_E)) ||
                (traits::case_insensitive_ex::eq(suffix[0], common_lang_constants::LOWER_S) &&
                    traits::case_insensitive_ex::eq(suffix[1], common_lang_constants::LOWER_T) &&
                    traits::case_insensitive_ex::eq(suffix[2], common_lang_constants::LOWER_R) &&
                    traits::case_insensitive_ex::eq(suffix[3], common_lang_constants::LOWER_A) &&
                    traits::case_insensitive_ex::eq(suffix[4], common_lang_constants::LOWER_L)) ||
                (traits::case_insensitive_ex::eq(suffix[0], common_lang_constants::LOWER_L) &&
                    traits::case_insensitive_ex::eq(suffix[1], common_lang_constants::LOWER_I) &&
                    traits::case_insensitive_ex::eq(suffix[2], common_lang_constants::LOWER_C) &&
                    traits::case_insensitive_ex::eq(suffix[3], common_lang_constants::LOWER_A) &&
                    traits::case_insensitive_ex::eq(suffix[4], common_lang_constants::LOWER_L)) ))
                { return true; }
            else if (suffix_length >= 4 &&
                ((traits::case_insensitive_ex::eq(suffix[0], common_lang_constants::LOWER_L) &&
                    traits::case_insensitive_ex::eq(suffix[1], common_lang_constants::LOWER_I) &&
                    traits::case_insensitive_ex::eq(suffix[2], common_lang_constants::LOWER_S) &&
                    traits::case_insensitive_ex::eq(suffix[3], common_lang_constants::LOWER_M)) ||
                (traits::case_insensitive_ex::eq(suffix[0], common_lang_constants::LOWER_L) &&
                    traits::case_insensitive_ex::eq(suffix[1], common_lang_constants::LOWER_I) &&
                    traits::case_insensitive_ex::eq(suffix[2], common_lang_constants::LOWER_Z) &&
                    traits::case_insensitive_ex::eq(suffix[3], common_lang_constants::LOWER_E)) ||
                (traits::case_insensitive_ex::eq(suffix[0], common_lang_constants::LOWER_T) &&
                    traits::case_insensitive_ex::eq(suffix[1], common_lang_constants::LOWER_I) &&
                    traits::case_insensitive_ex::eq(suffix[2], common_lang_constants::LOWER_S) &&
                    traits::case_insensitive_ex::eq(suffix[3], common_lang_constants::LOWER_M)) ||
                (traits::case_insensitive_ex::eq(suffix[0], common_lang_constants::LOWER_D) &&
                    traits::case_insensitive_ex::eq(suffix[1], common_lang_constants::LOWER_R) &&
                    traits::case_insensitive_ex::eq(suffix[2], common_lang_constants::LOWER_A) &&
                    traits::case_insensitive_ex::eq(suffix[3], common_lang_constants::LOWER_L)) ||
                (traits::case_insensitive_ex::eq(suffix[0], common_lang_constants::LOWER_Q) &&
                    traits::case_insensitive_ex::eq(suffix[1], common_lang_constants::LOWER_U) &&
                    traits::case_insensitive_ex::eq(suffix[2], common_lang_constants::LOWER_E) &&
                    traits::case_insensitive_ex::eq(suffix[3], common_lang_constants::LOWER_N)) ||
                (traits::case_insensitive_ex::eq(suffix[0], common_lang_constants::LOWER_N) &&
                    traits::case_insensitive_ex::eq(suffix[1], common_lang_constants::LOWER_I) &&
                    traits::case_insensitive_ex::eq(suffix[2], common_lang_constants::LOWER_A) &&
                    traits::case_insensitive_ex::eq(suffix[3], common_lang_constants::LOWER_L)) ||
                (traits::case_insensitive_ex::eq(suffix[0], common_lang_constants::LOWER_P) &&
                    traits::case_insensitive_ex::eq(suffix[1], common_lang_constants::LOWER_T) &&
                    traits::case_insensitive_ex::eq(suffix[2], common_lang_constants::LOWER_O) &&
                    traits::case_insensitive_ex::eq(suffix[3], common_lang_constants::LOWER_R)) ||
                (traits::case_insensitive_ex::eq(suffix[0], common_lang_constants::LOWER_S) &&
                    traits::case_insensitive_ex::eq(suffix[1], common_lang_constants::LOWER_T) &&
                    traits::case_insensitive_ex::eq(suffix[2], common_lang_constants::LOWER_O) &&
                    traits::case_insensitive_ex::eq(suffix[3], common_lang_constants::LOWER_R)) ||
                (traits::case_insensitive_ex::eq(suffix[0], common_lang_constants::LOWER_S) &&
                    traits::case_insensitive_ex::eq(suffix[1], common_lang_constants::LOWER_T) &&
                    traits::case_insensitive_ex::eq(suffix[2], common_lang_constants::LOWER_R) &&
                    traits::case_insensitive_ex::eq(suffix[3], common_lang_constants::LOWER_Y)) ||
                (traits::case_insensitive_ex::eq(suffix[0], common_lang_constants::LOWER_T) &&
                    traits::case_insensitive_ex::eq(suffix[1], common_lang_constants::LOWER_H) &&
                    traits::case_insensitive_ex::eq(suffix[2], common_lang_constants::LOWER_E) &&
                    traits::case_insensitive_ex::eq(suffix[3], common_lang_constants::LOWER_R)) ||
                (traits::case_insensitive_ex::eq(suffix[0], common_lang_constants::LOWER_T) &&
                    traits::case_insensitive_ex::eq(suffix[1], common_lang_constants::LOWER_R) &&
                    traits::case_insensitive_ex::eq(suffix[2], common_lang_constants::LOWER_I) &&
                    traits::case_insensitive_ex::eq(suffix[3], common_lang_constants::LOWER_C)) ||
                (traits::case_insensitive_ex::eq(suffix[0], common_lang_constants::LOWER_Q) &&
                    traits::case_insensitive_ex::eq(suffix[1], common_lang_constants::LOWER_U) &&
                    traits::case_insensitive_ex::eq(suffix[2], common_lang_constants::LOWER_I) &&
                    traits::case_insensitive_ex::eq(suffix[3], common_lang_constants::LOWER_N)) ||
                (traits::case_insensitive_ex::eq(suffix[0], common_lang_constants::LOWER_V) &&
                traits::case_insensitive_ex::eq(suffix[1], common_lang_constants::LOWER_I) &&
                traits::case_insensitive_ex::eq(suffix[2], common_lang_constants::LOWER_S) &&
                traits::case_insensitive_ex::eq(suffix[3], common_lang_constants::LOWER_M)) ))
                { return true; }
            else if (suffix_length >= 3 &&
                ((traits::case_insensitive_ex::eq(suffix[0], common_lang_constants::LOWER_M) &&
                    traits::case_insensitive_ex::eq(suffix[1], common_lang_constants::LOWER_I) &&
                    traits::case_insensitive_ex::eq(suffix[2], common_lang_constants::LOWER_C)) ||
                // cat-E-gor-y ('e' is not silent)
                (traits::case_insensitive_ex::eq(suffix[0], common_lang_constants::LOWER_G) &&
                    traits::case_insensitive_ex::eq(suffix[1], common_lang_constants::LOWER_O) &&
                    traits::case_insensitive_ex::eq(suffix[2], common_lang_constants::LOWER_R)) ||
                (traits::case_insensitive_ex::eq(suffix[0], common_lang_constants::LOWER_S) &&
                    traits::case_insensitive_ex::eq(suffix[1], common_lang_constants::LOWER_I) &&
                    traits::case_insensitive_ex::eq(suffix[2], common_lang_constants::LOWER_S)) ||
                (traits::case_insensitive_ex::eq(suffix[0], common_lang_constants::LOWER_T) &&
                    traits::case_insensitive_ex::eq(suffix[1], common_lang_constants::LOWER_I) &&
                    traits::case_insensitive_ex::eq(suffix[2], common_lang_constants::LOWER_C)) ||
                (traits::case_insensitive_ex::eq(suffix[0], common_lang_constants::LOWER_T) &&
                    traits::case_insensitive_ex::eq(suffix[1], common_lang_constants::LOWER_E) &&
                    traits::case_insensitive_ex::eq(suffix[2], common_lang_constants::LOWER_R)) ||
                (traits::case_insensitive_ex::eq(suffix[0], common_lang_constants::LOWER_T) &&
                    traits::case_insensitive_ex::eq(suffix[1], common_lang_constants::LOWER_E) &&
                    traits::case_insensitive_ex::eq(suffix[2], common_lang_constants::LOWER_S)) ||
                (traits::case_insensitive_ex::eq(suffix[0], common_lang_constants::LOWER_T) &&
                    traits::case_insensitive_ex::eq(suffix[1], common_lang_constants::LOWER_I) &&
                    traits::case_insensitive_ex::eq(suffix[2], common_lang_constants::LOWER_N)) ||
                (traits::case_insensitive_ex::eq(suffix[0], common_lang_constants::LOWER_T) &&
                    traits::case_insensitive_ex::eq(suffix[1], common_lang_constants::LOWER_R) &&
                    traits::case_insensitive_ex::eq(suffix[2], common_lang_constants::LOWER_Y)) ||
                (traits::case_insensitive_ex::eq(suffix[0], common_lang_constants::LOWER_T) &&
                    traits::case_insensitive_ex::eq(suffix[1], common_lang_constants::LOWER_U) &&
                    traits::case_insensitive_ex::eq(suffix[2], common_lang_constants::LOWER_M)) ||
                (traits::case_insensitive_ex::eq(suffix[0], common_lang_constants::LOWER_V) &&
                    traits::case_insensitive_ex::eq(suffix[1], common_lang_constants::LOWER_E) &&
                    traits::case_insensitive_ex::eq(suffix[2], common_lang_constants::LOWER_R)) ||
                (traits::case_insensitive_ex::eq(suffix[0], common_lang_constants::LOWER_V) &&
                    traits::case_insensitive_ex::eq(suffix[1], common_lang_constants::LOWER_I) &&
                    traits::case_insensitive_ex::eq(suffix[2], common_lang_constants::LOWER_K)) ||
                (traits::case_insensitive_ex::eq(suffix[0], common_lang_constants::LOWER_M) &&
                    traits::case_insensitive_ex::eq(suffix[1], common_lang_constants::LOWER_I) &&
                    traits::case_insensitive_ex::eq(suffix[2], common_lang_constants::LOWER_A)) ||
                (traits::case_insensitive_ex::eq(suffix[0], common_lang_constants::LOWER_G) &&
                    traits::case_insensitive_ex::eq(suffix[1], common_lang_constants::LOWER_A) &&
                    traits::case_insensitive_ex::eq(suffix[2], common_lang_constants::LOWER_S)) ))
                { return true; }
            else if (suffix_length >= 2 &&
                ((traits::case_insensitive_ex::eq(suffix[0], common_lang_constants::LOWER_G) &&
                    traits::case_insensitive_ex::eq(suffix[1], common_lang_constants::LOWER_A)) ||
                (traits::case_insensitive_ex::eq(suffix[0], common_lang_constants::LOWER_B) &&
                    traits::case_insensitive_ex::eq(suffix[1], common_lang_constants::LOWER_O)) ||
                (traits::case_insensitive_ex::eq(suffix[0], common_lang_constants::LOWER_F) &&
                    traits::case_insensitive_ex::eq(suffix[1], common_lang_constants::LOWER_Y)) ||
                (traits::case_insensitive_ex::eq(suffix[0], common_lang_constants::LOWER_M) &&
                    traits::case_insensitive_ex::eq(suffix[1], common_lang_constants::LOWER_A)) ||
                (traits::case_insensitive_ex::eq(suffix[0], common_lang_constants::LOWER_M) &&
                    traits::case_insensitive_ex::eq(suffix[1], common_lang_constants::LOWER_Y)) ||
                (traits::case_insensitive_ex::eq(suffix[0], common_lang_constants::LOWER_R) &&
                    traits::case_insensitive_ex::eq(suffix[1], common_lang_constants::LOWER_R)) ))
                { return true; }
            return false;
            }
        }

    bool english_syllabize::is_single_non_e_vowel_ignored(const wchar_t* word, const size_t position) const
        {
        assert(word);
        if (word == nullptr)
            { return false; }
        assert(!traits::case_insensitive_ex::eq(word[position], L'e') &&
               "is_single_non_e_vowel_ignored() should not be reviewing an 'e', use is_silent_e()");
        // busi-ness (pronounced "biz-nis")
        if (position >= 3 &&
            m_length >= 8 &&
            position+4 < m_length &&
            traits::case_insensitive_ex::eq(word[position], common_lang_constants::LOWER_I) &&
            // prefix and suffix
            traits::case_insensitive_ex::eq(word[position-3], common_lang_constants::LOWER_B) &&
            traits::case_insensitive_ex::eq(word[position-2], common_lang_constants::LOWER_U) &&
            traits::case_insensitive_ex::eq(word[position-1], common_lang_constants::LOWER_S) &&
            traits::case_insensitive_ex::eq(word[position+1], common_lang_constants::LOWER_N) &&
            traits::case_insensitive_ex::eq(word[position+2], common_lang_constants::LOWER_E) &&
            traits::case_insensitive_ex::eq(word[position+3], common_lang_constants::LOWER_S) &&
            traits::case_insensitive_ex::eq(word[position+4], common_lang_constants::LOWER_S))
            { return true; }
        return false;
        }

    bool english_syllabize::is_silent_e(const wchar_t* word, const size_t position) const
        {
        assert(word);
        // verify that it's an 'e' first and
        // not the first letter (first letter can't be silent)
        if (position == 0 || (!traits::case_insensitive_ex::eq(word[position], common_lang_constants::LOWER_E) ) )
            { return false; }
        // [letters]e[number] would be silent (e.g., "base64")
        else if (position+1 < m_length && characters::is_character::is_numeric_simple(word[position+1]))
            { return true; }
        // if 'e' is second to last letter and NOT followed by 's' or 'd' then it's not silent
        else if (position+2 == m_length &&
            !traits::case_insensitive_ex::eq(word[position+1], common_lang_constants::LOWER_S) &&
            !traits::case_insensitive_ex::eq(word[position+1], common_lang_constants::LOWER_D))
            { return false; }
        // special case for "aren't" and "weren't"
        else if (position == 2 && m_ends_with_nt_contraction && m_length == 6)
            { return true; }
        else if (position == 3 && m_ends_with_nt_contraction && m_length == 7)
            { return true; }
        // special case for "'re" and "'ve" contractions
        else if (position == m_length-1 &&
            m_length > 3 &&
            (traits::case_insensitive_ex::eq(word[m_length-2], common_lang_constants::LOWER_R) ||
                traits::case_insensitive_ex::eq(word[m_length-2], common_lang_constants::LOWER_V))&&
            isChar.is_apostrophe(word[m_length-3]))
            { return true; }
        // e-sty, e-sties
        else if (position <= m_length-3 &&
            traits::case_insensitive_ex::eq(word[m_length-3], common_lang_constants::LOWER_S) &&
            traits::case_insensitive_ex::eq(word[m_length-2], common_lang_constants::LOWER_T) &&
            (traits::case_insensitive_ex::eq(word[m_length-1], common_lang_constants::LOWER_Y) ||
                traits::case_insensitive_ex::eq(word[m_length-1], common_lang_constants::LOWER_I)))
            { return false; }
        // see if there are any proceeding vowels
        size_t next_vowel = m_length;
        if (position+2 < m_length)
            {
            // start from last position and backtrack
            for (next_vowel = (position+1); next_vowel < m_length; ++next_vowel)
                    {
                    // found another vowel before end of word,
                    // so this 'e' is still a candidate
                    if (isChar.is_vowel(word[next_vowel]) )
                        {
                        // watch out for "ey", this could still be a silent y
                        if (traits::case_insensitive_ex::eq(word[next_vowel], common_lang_constants::LOWER_Y) &&
                            is_consonant_y(word, next_vowel))
                        { continue; }
                        break;
                        }
                }
                if (next_vowel == m_length ||
                next_vowel == position+1)
                    {
                    // no proceeding vowels or 'e' is directly proceeded by another vowel,
                    // so this fails
                    return false;
                    }
            }
        // make sure that there are previous vowels;
        // otherwise, an 'e' could not be a silent modifier       
        if (m_previous_vowel == m_length)
            {
            // no previous vowels, so this fails
            return false;
            }
        // special case for "surveyor"
        else if (position <= m_length-4 &&
            traits::case_insensitive_ex::eq(word[position+1], common_lang_constants::LOWER_Y) &&
            traits::case_insensitive_ex::eq(word[position+2], common_lang_constants::LOWER_O) &&
                traits::case_insensitive_ex::eq(word[position+3], common_lang_constants::LOWER_R) )
            { return false; }
        // wesleyan
        else if (position <= m_length-4 &&
            traits::case_insensitive_ex::eq(word[position+1], common_lang_constants::LOWER_Y) &&
            traits::case_insensitive_ex::eq(word[position+2], common_lang_constants::LOWER_A) &&
                traits::case_insensitive_ex::eq(word[position+3], common_lang_constants::LOWER_N) )
            { return false; }
        // special case for "honeyest"
        else if (position <= m_length-5 &&
            traits::case_insensitive_ex::eq(word[position+1], common_lang_constants::LOWER_Y) &&
            traits::case_insensitive_ex::eq(word[position+2], common_lang_constants::LOWER_E) &&
            traits::case_insensitive_ex::eq(word[position+3], common_lang_constants::LOWER_S) &&
            traits::case_insensitive_ex::eq(word[position+4], common_lang_constants::LOWER_T) )
            { return false; }
        // special case for "eve-ry"
        else if (position == 2 && m_length >= 5 &&
            traits::case_insensitive_ex::eq(word[0], common_lang_constants::LOWER_E) &&
            traits::case_insensitive_ex::eq(word[1], common_lang_constants::LOWER_V) &&
            traits::case_insensitive_ex::eq(word[2], common_lang_constants::LOWER_E) &&
            traits::case_insensitive_ex::eq(word[3], common_lang_constants::LOWER_R) &&
            traits::case_insensitive_ex::eq(word[4], common_lang_constants::LOWER_Y))
            { return true; }
        // special case for "equEstrienne" (this is not silent)
        else if (position == 3 &&
            traits::case_insensitive_ex::eq(word[0], common_lang_constants::LOWER_E) &&
            traits::case_insensitive_ex::eq(word[1], common_lang_constants::LOWER_Q) &&
            traits::case_insensitive_ex::eq(word[2], common_lang_constants::LOWER_U) )
            { return false; }
        // special case when 'y' is the vowel that may be getting modified
        else if (traits::case_insensitive_ex::eq(word[m_previous_vowel], common_lang_constants::LOWER_Y) &&
            m_previous_vowel > 0/*'y' as beginning of the word would be a consonant*/)
            {
            // oxyhemoglobin, system
            if (traits::case_insensitive_ex::eq(word[m_previous_vowel-1], common_lang_constants::LOWER_X) ||
                traits::case_insensitive_ex::eq(word[m_previous_vowel-1], common_lang_constants::LOWER_S) )
                {
                return false;
                }
            }

        // special case where "tente" and "tante" see last 'e' as silent
        if (position == (m_length-1) &&
            position >= 4 &&
            traits::case_insensitive_ex::eq(word[position-4], common_lang_constants::LOWER_T) &&
            (traits::case_insensitive_ex::eq(word[position-3], common_lang_constants::LOWER_A) ||
                traits::case_insensitive_ex::eq(word[position-3], common_lang_constants::LOWER_E)) &&
            traits::case_insensitive_ex::eq(word[position-2], common_lang_constants::LOWER_N) &&
            traits::case_insensitive_ex::eq(word[position-1], common_lang_constants::LOWER_T) )
            {
            return true;
            }
        else if (position == 3 &&
            does_prefix_silence_e(word, position) )
            {
            return true;
            }
        else if (position >= 2 &&
            does_prefix_disconnect_e(word, position) )
            {
            return false;
            }
        // gobbledEgook
        else if (position >= 5 &&
            traits::case_insensitive_ex::eq(word[position-5], common_lang_constants::LOWER_B) &&
            traits::case_insensitive_ex::eq(word[position-4], common_lang_constants::LOWER_B) &&
            traits::case_insensitive_ex::eq(word[position-3], common_lang_constants::LOWER_L) &&
            traits::case_insensitive_ex::eq(word[position-2], common_lang_constants::LOWER_E) &&
            traits::case_insensitive_ex::eq(word[position-1], common_lang_constants::LOWER_D) )
            { return false; }
        // medical terms with "ane" prefix (anemia)
        else if (m_length >= 4 &&
            position == 2 &&
            traits::case_insensitive_ex::eq(word[0], common_lang_constants::LOWER_A) &&
            traits::case_insensitive_ex::eq(word[1], common_lang_constants::LOWER_N) )
            {
            return false;
            }
        // somE...
        else if (position == 3 &&
            traits::case_insensitive_ex::eq(word[0], common_lang_constants::LOWER_S) &&
            traits::case_insensitive_ex::eq(word[1], common_lang_constants::LOWER_O) &&
            traits::case_insensitive_ex::eq(word[2], common_lang_constants::LOWER_M))
            {
            // somErsault, 'er' is separate sound
            if (m_length >= 5 && traits::case_insensitive_ex::eq(word[4], common_lang_constants::LOWER_R))
                { return false; }
            // otherwise, it is somEbody or somEtimes
            return true;
            }
        // special case for "graphed"
        else if ((position+1 < m_length) &&
            position > 3 && traits::case_insensitive_ex::eq(word[position-3], common_lang_constants::LOWER_A) &&
            traits::case_insensitive_ex::eq(word[position-2], common_lang_constants::LOWER_P) &&
            traits::case_insensitive_ex::eq(word[position-1], common_lang_constants::LOWER_H) &&
            traits::case_insensitive_ex::eq(word[position+1], common_lang_constants::LOWER_D))
            {
            return true;
            }
        // make sure not following separable consonants
        else if (!can_consonants_be_modified_by_following_e(word+m_previous_vowel+1, (position-1)-m_previous_vowel) )
            {
            return false;
            }
        // verify that the following consonants can start a new syllable. if not,
        // then this 'e' would have to be connected to the first following consonant
        else if ( (position+2 < m_length) )
            {
            if (next_vowel != m_length &&
                !can_consonants_begin_sound(word+position+1, (next_vowel-1)-position) )
                {
                return false;
                }
            }

        // uneven, unearthed, decelerate
        if (position == 2 &&
            traits::case_insensitive_ex::eq(word[0], common_lang_constants::LOWER_U) &&
            traits::case_insensitive_ex::eq(word[1], common_lang_constants::LOWER_N))
            { return false; }
        // unde always splits
        else if (position == 3 &&
            traits::case_insensitive_ex::eq(word[0], common_lang_constants::LOWER_U) &&
            traits::case_insensitive_ex::eq(word[1], common_lang_constants::LOWER_N) &&
            traits::case_insensitive_ex::eq(word[2], common_lang_constants::LOWER_D))
            { return false; }
        // alchemist
        else if (position == 4 &&
            traits::case_insensitive_ex::eq(word[0], common_lang_constants::LOWER_A) &&
            traits::case_insensitive_ex::eq(word[1], common_lang_constants::LOWER_L) &&
            traits::case_insensitive_ex::eq(word[2], common_lang_constants::LOWER_C) &&
            traits::case_insensitive_ex::eq(word[3], common_lang_constants::LOWER_H))
            { return false; }
        // un-der-e always splits
        else if (position == 5 &&
            traits::case_insensitive_ex::eq(word[0], common_lang_constants::LOWER_U) &&
            traits::case_insensitive_ex::eq(word[1], common_lang_constants::LOWER_N) &&
            traits::case_insensitive_ex::eq(word[2], common_lang_constants::LOWER_D) &&
            traits::case_insensitive_ex::eq(word[3], common_lang_constants::LOWER_E) &&
            traits::case_insensitive_ex::eq(word[4], common_lang_constants::LOWER_R))
            { return false; }
        // horse... and house... also silent
        else if (position == 4 &&
            position < (m_length-2) &&
            // past
            ((traits::case_insensitive_ex::eq(word[0], common_lang_constants::LOWER_P) &&
                traits::case_insensitive_ex::eq(word[1], common_lang_constants::LOWER_A) &&
                traits::case_insensitive_ex::eq(word[2], common_lang_constants::LOWER_S) &&
                traits::case_insensitive_ex::eq(word[3], common_lang_constants::LOWER_T)) ||
            // peac
            (traits::case_insensitive_ex::eq(word[0], common_lang_constants::LOWER_P) &&
                traits::case_insensitive_ex::eq(word[1], common_lang_constants::LOWER_E) &&
                traits::case_insensitive_ex::eq(word[2], common_lang_constants::LOWER_A) &&
                traits::case_insensitive_ex::eq(word[3], common_lang_constants::LOWER_C)) ||
            // hors
            (traits::case_insensitive_ex::eq(word[0], common_lang_constants::LOWER_H) &&
                traits::case_insensitive_ex::eq(word[1], common_lang_constants::LOWER_O) &&
                traits::case_insensitive_ex::eq(word[2], common_lang_constants::LOWER_R) &&
                traits::case_insensitive_ex::eq(word[3], common_lang_constants::LOWER_S)) ||
            // hous
            (traits::case_insensitive_ex::eq(word[0], common_lang_constants::LOWER_H) &&
                traits::case_insensitive_ex::eq(word[1], common_lang_constants::LOWER_O) &&
                traits::case_insensitive_ex::eq(word[2], common_lang_constants::LOWER_U) &&
                traits::case_insensitive_ex::eq(word[3], common_lang_constants::LOWER_S)) ||
            // ston
            (traits::case_insensitive_ex::eq(word[0], common_lang_constants::LOWER_S) &&
                traits::case_insensitive_ex::eq(word[1], common_lang_constants::LOWER_T) &&
                traits::case_insensitive_ex::eq(word[2], common_lang_constants::LOWER_O) &&
                traits::case_insensitive_ex::eq(word[3], common_lang_constants::LOWER_N)) ||
            // stal
            (traits::case_insensitive_ex::eq(word[0], common_lang_constants::LOWER_S) &&
                traits::case_insensitive_ex::eq(word[1], common_lang_constants::LOWER_T) &&
                traits::case_insensitive_ex::eq(word[2], common_lang_constants::LOWER_A) &&
                traits::case_insensitive_ex::eq(word[3], common_lang_constants::LOWER_L)) ||
            // spac
            (traits::case_insensitive_ex::eq(word[0], common_lang_constants::LOWER_S) &&
                traits::case_insensitive_ex::eq(word[1], common_lang_constants::LOWER_P) &&
                traits::case_insensitive_ex::eq(word[2], common_lang_constants::LOWER_A) &&
                traits::case_insensitive_ex::eq(word[3], common_lang_constants::LOWER_C)) ||
            // skat
            (traits::case_insensitive_ex::eq(word[0], common_lang_constants::LOWER_S) &&
                traits::case_insensitive_ex::eq(word[1], common_lang_constants::LOWER_K) &&
                traits::case_insensitive_ex::eq(word[2], common_lang_constants::LOWER_A) &&
                traits::case_insensitive_ex::eq(word[3], common_lang_constants::LOWER_T)) ||
            // shak
            (traits::case_insensitive_ex::eq(word[0], common_lang_constants::LOWER_S) &&
                traits::case_insensitive_ex::eq(word[1], common_lang_constants::LOWER_H) &&
                traits::case_insensitive_ex::eq(word[2], common_lang_constants::LOWER_A) &&
                traits::case_insensitive_ex::eq(word[3], common_lang_constants::LOWER_K)) ||
            // shap
            (traits::case_insensitive_ex::eq(word[0], common_lang_constants::LOWER_S) &&
                traits::case_insensitive_ex::eq(word[1], common_lang_constants::LOWER_H) &&
                traits::case_insensitive_ex::eq(word[2], common_lang_constants::LOWER_A) &&
                traits::case_insensitive_ex::eq(word[3], common_lang_constants::LOWER_P)) ||
            // wher
            (traits::case_insensitive_ex::eq(word[0], common_lang_constants::LOWER_W) &&
                traits::case_insensitive_ex::eq(word[1], common_lang_constants::LOWER_H) &&
                traits::case_insensitive_ex::eq(word[2], common_lang_constants::LOWER_E) &&
                traits::case_insensitive_ex::eq(word[3], common_lang_constants::LOWER_R)) ||
            // valu
            (traits::case_insensitive_ex::eq(word[0], common_lang_constants::LOWER_V) &&
                traits::case_insensitive_ex::eq(word[1], common_lang_constants::LOWER_A) &&
                traits::case_insensitive_ex::eq(word[2], common_lang_constants::LOWER_L) &&
                traits::case_insensitive_ex::eq(word[3], common_lang_constants::LOWER_U)) ||
            // shak
            (traits::case_insensitive_ex::eq(word[0], common_lang_constants::LOWER_S) &&
                traits::case_insensitive_ex::eq(word[1], common_lang_constants::LOWER_H) &&
                traits::case_insensitive_ex::eq(word[2], common_lang_constants::LOWER_A) &&
                traits::case_insensitive_ex::eq(word[3], common_lang_constants::LOWER_K)) ||
            // ther
            (traits::case_insensitive_ex::eq(word[0], common_lang_constants::LOWER_T) &&
                traits::case_insensitive_ex::eq(word[1], common_lang_constants::LOWER_H) &&
                traits::case_insensitive_ex::eq(word[2], common_lang_constants::LOWER_E) &&
                traits::case_insensitive_ex::eq(word[3], common_lang_constants::LOWER_R)) ||
            // stov
            (traits::case_insensitive_ex::eq(word[0], common_lang_constants::LOWER_S) &&
                traits::case_insensitive_ex::eq(word[1], common_lang_constants::LOWER_T) &&
                traits::case_insensitive_ex::eq(word[2], common_lang_constants::LOWER_O) &&
                traits::case_insensitive_ex::eq(word[3], common_lang_constants::LOWER_V)) ||
            // trad
            (traits::case_insensitive_ex::eq(word[0], common_lang_constants::LOWER_T) &&
                traits::case_insensitive_ex::eq(word[1], common_lang_constants::LOWER_R) &&
                traits::case_insensitive_ex::eq(word[2], common_lang_constants::LOWER_A) &&
                traits::case_insensitive_ex::eq(word[3], common_lang_constants::LOWER_D)) ))
            { return true; }
        else if (position == 4 &&
            position < (m_length-2) &&
            traits::case_insensitive_ex::eq(word[0], common_lang_constants::LOWER_P) &&
            traits::case_insensitive_ex::eq(word[1], common_lang_constants::LOWER_L) &&
            traits::case_insensitive_ex::eq(word[2], common_lang_constants::LOWER_A) &&
            traits::case_insensitive_ex::eq(word[3], common_lang_constants::LOWER_C))
            {
            // except for "placebo"
            if (position < (m_length-2) &&
                traits::case_insensitive_ex::eq(word[position+1], common_lang_constants::LOWER_B) &&
                traits::case_insensitive_ex::eq(word[position+2], common_lang_constants::LOWER_O))
                { return false; }
            return true;
            }

        /// life... is always silent
        if (position == 3 &&
            position < (m_length-2) &&
            ((traits::case_insensitive_ex::eq(word[0], common_lang_constants::LOWER_L) &&
                traits::case_insensitive_ex::eq(word[1], common_lang_constants::LOWER_I) &&
                traits::case_insensitive_ex::eq(word[2], common_lang_constants::LOWER_F)) ||
            (traits::case_insensitive_ex::eq(word[0], common_lang_constants::LOWER_L) &&
                traits::case_insensitive_ex::eq(word[1], common_lang_constants::LOWER_O) &&
                traits::case_insensitive_ex::eq(word[2], common_lang_constants::LOWER_V)) ||
            (traits::case_insensitive_ex::eq(word[0], common_lang_constants::LOWER_W) &&
                traits::case_insensitive_ex::eq(word[1], common_lang_constants::LOWER_I) &&
                traits::case_insensitive_ex::eq(word[2], common_lang_constants::LOWER_R)) ||
            (traits::case_insensitive_ex::eq(word[0], common_lang_constants::LOWER_T) &&
                traits::case_insensitive_ex::eq(word[1], common_lang_constants::LOWER_I) &&
                traits::case_insensitive_ex::eq(word[2], common_lang_constants::LOWER_R)) ||
            (traits::case_insensitive_ex::eq(word[0], common_lang_constants::LOWER_T) &&
                traits::case_insensitive_ex::eq(word[1], common_lang_constants::LOWER_I) &&
                traits::case_insensitive_ex::eq(word[2], common_lang_constants::LOWER_M)) ||
            (traits::case_insensitive_ex::eq(word[0], common_lang_constants::LOWER_S) &&
                traits::case_insensitive_ex::eq(word[1], common_lang_constants::LOWER_I) &&
                traits::case_insensitive_ex::eq(word[2], common_lang_constants::LOWER_D)) ||
            (traits::case_insensitive_ex::eq(word[0], common_lang_constants::LOWER_F) &&
                traits::case_insensitive_ex::eq(word[1], common_lang_constants::LOWER_I) &&
                traits::case_insensitive_ex::eq(word[2], common_lang_constants::LOWER_R)) ||
            (traits::case_insensitive_ex::eq(word[0], common_lang_constants::LOWER_D) &&
                traits::case_insensitive_ex::eq(word[1], common_lang_constants::LOWER_A) &&
                traits::case_insensitive_ex::eq(word[2], common_lang_constants::LOWER_R)) ||
            (traits::case_insensitive_ex::eq(word[0], common_lang_constants::LOWER_E) &&
                traits::case_insensitive_ex::eq(word[1], common_lang_constants::LOWER_A) &&
                traits::case_insensitive_ex::eq(word[2], common_lang_constants::LOWER_V)) ||
            (traits::case_insensitive_ex::eq(word[0], common_lang_constants::LOWER_F) &&
                traits::case_insensitive_ex::eq(word[1], common_lang_constants::LOWER_I) &&
                traits::case_insensitive_ex::eq(word[2], common_lang_constants::LOWER_L)) ||
            (traits::case_insensitive_ex::eq(word[0], common_lang_constants::LOWER_H) &&
                traits::case_insensitive_ex::eq(word[1], common_lang_constants::LOWER_I) &&
                traits::case_insensitive_ex::eq(word[2], common_lang_constants::LOWER_D)) ||
            (traits::case_insensitive_ex::eq(word[0], common_lang_constants::LOWER_H) &&
                traits::case_insensitive_ex::eq(word[1], common_lang_constants::LOWER_O) &&
                traits::case_insensitive_ex::eq(word[2], common_lang_constants::LOWER_M)) ||
            (traits::case_insensitive_ex::eq(word[0], common_lang_constants::LOWER_N) &&
                traits::case_insensitive_ex::eq(word[1], common_lang_constants::LOWER_A) &&
                traits::case_insensitive_ex::eq(word[2], common_lang_constants::LOWER_M)) ||
            (traits::case_insensitive_ex::eq(word[0], common_lang_constants::LOWER_N) &&
                traits::case_insensitive_ex::eq(word[1], common_lang_constants::LOWER_I) &&
                traits::case_insensitive_ex::eq(word[2], common_lang_constants::LOWER_N)) ||
            (traits::case_insensitive_ex::eq(word[0], common_lang_constants::LOWER_N) &&
                traits::case_insensitive_ex::eq(word[1], common_lang_constants::LOWER_O) &&
                traits::case_insensitive_ex::eq(word[2], common_lang_constants::LOWER_S)) ||
            (traits::case_insensitive_ex::eq(word[0], common_lang_constants::LOWER_L) &&
                traits::case_insensitive_ex::eq(word[1], common_lang_constants::LOWER_O) &&
                traits::case_insensitive_ex::eq(word[2], common_lang_constants::LOWER_N)) ||
            (traits::case_insensitive_ex::eq(word[0], common_lang_constants::LOWER_Y) &&
                traits::case_insensitive_ex::eq(word[1], common_lang_constants::LOWER_U) &&
                traits::case_insensitive_ex::eq(word[2], common_lang_constants::LOWER_L)) ||
            (traits::case_insensitive_ex::eq(word[0], common_lang_constants::LOWER_W) &&
                traits::case_insensitive_ex::eq(word[1], common_lang_constants::LOWER_I) &&
                traits::case_insensitive_ex::eq(word[2], common_lang_constants::LOWER_N)) ||
            (traits::case_insensitive_ex::eq(word[0], common_lang_constants::LOWER_V) &&
                traits::case_insensitive_ex::eq(word[1], common_lang_constants::LOWER_I) &&
                traits::case_insensitive_ex::eq(word[2], common_lang_constants::LOWER_C)) ||
            (traits::case_insensitive_ex::eq(word[0], common_lang_constants::LOWER_P) &&
                traits::case_insensitive_ex::eq(word[1], common_lang_constants::LOWER_A) &&
                traits::case_insensitive_ex::eq(word[2], common_lang_constants::LOWER_C)) ||
            (traits::case_insensitive_ex::eq(word[0], common_lang_constants::LOWER_D) &&
                traits::case_insensitive_ex::eq(word[1], common_lang_constants::LOWER_O) &&
                traits::case_insensitive_ex::eq(word[2], common_lang_constants::LOWER_V)) ||
            (traits::case_insensitive_ex::eq(word[0], common_lang_constants::LOWER_T) &&
                traits::case_insensitive_ex::eq(word[1], common_lang_constants::LOWER_A) &&
                traits::case_insensitive_ex::eq(word[2], common_lang_constants::LOWER_K)) ))
            { return true; }
        // esp-e, int-er, wat-er, re-de-fine, re-fer, re-new
        else if (position == 3 &&
            ((traits::case_insensitive_ex::eq(word[0], common_lang_constants::LOWER_E) &&
                traits::case_insensitive_ex::eq(word[1], common_lang_constants::LOWER_S) &&
                traits::case_insensitive_ex::eq(word[2], common_lang_constants::LOWER_P)) ||
            (traits::case_insensitive_ex::eq(word[0], common_lang_constants::LOWER_I) &&
                traits::case_insensitive_ex::eq(word[1], common_lang_constants::LOWER_N) &&
                traits::case_insensitive_ex::eq(word[2], common_lang_constants::LOWER_T)) ||
            (traits::case_insensitive_ex::eq(word[0], common_lang_constants::LOWER_W) &&
                traits::case_insensitive_ex::eq(word[1], common_lang_constants::LOWER_A) &&
                traits::case_insensitive_ex::eq(word[2], common_lang_constants::LOWER_T)) ||
            (traits::case_insensitive_ex::eq(word[0], common_lang_constants::LOWER_S) &&
                traits::case_insensitive_ex::eq(word[1], common_lang_constants::LOWER_U) &&
                traits::case_insensitive_ex::eq(word[2], common_lang_constants::LOWER_P)) ||
            (traits::case_insensitive_ex::eq(word[0], common_lang_constants::LOWER_D) &&
                traits::case_insensitive_ex::eq(word[1], common_lang_constants::LOWER_E) &&
                traits::case_insensitive_ex::eq(word[2], common_lang_constants::LOWER_M)) ||
            (traits::case_insensitive_ex::eq(word[0], common_lang_constants::LOWER_D) &&
                traits::case_insensitive_ex::eq(word[1], common_lang_constants::LOWER_E) &&
                traits::case_insensitive_ex::eq(word[2], common_lang_constants::LOWER_R)) ||
            (traits::case_insensitive_ex::eq(word[0], common_lang_constants::LOWER_D) &&
                traits::case_insensitive_ex::eq(word[1], common_lang_constants::LOWER_E) &&
                traits::case_insensitive_ex::eq(word[2], common_lang_constants::LOWER_S)) ||
            (traits::case_insensitive_ex::eq(word[0], common_lang_constants::LOWER_D) &&
                traits::case_insensitive_ex::eq(word[1], common_lang_constants::LOWER_E) &&
                traits::case_insensitive_ex::eq(word[2], common_lang_constants::LOWER_T)) ||
            (traits::case_insensitive_ex::eq(word[0], common_lang_constants::LOWER_D) &&
                traits::case_insensitive_ex::eq(word[1], common_lang_constants::LOWER_E) &&
                traits::case_insensitive_ex::eq(word[2], common_lang_constants::LOWER_V)) ||
            (traits::case_insensitive_ex::eq(word[0], common_lang_constants::LOWER_V) &&
                traits::case_insensitive_ex::eq(word[1], common_lang_constants::LOWER_E) &&
                traits::case_insensitive_ex::eq(word[2], common_lang_constants::LOWER_H)) ||
            (traits::case_insensitive_ex::eq(word[0], common_lang_constants::LOWER_V) &&
                traits::case_insensitive_ex::eq(word[1], common_lang_constants::LOWER_E) &&
                traits::case_insensitive_ex::eq(word[2], common_lang_constants::LOWER_N)) ||
            (traits::case_insensitive_ex::eq(word[0], common_lang_constants::LOWER_P) &&
                traits::case_insensitive_ex::eq(word[1], common_lang_constants::LOWER_E) &&
                traits::case_insensitive_ex::eq(word[2], common_lang_constants::LOWER_D)) ||
            (traits::case_insensitive_ex::eq(word[0], common_lang_constants::LOWER_P) &&
                traits::case_insensitive_ex::eq(word[1], common_lang_constants::LOWER_E) &&
                traits::case_insensitive_ex::eq(word[2], common_lang_constants::LOWER_R)) ||
            (traits::case_insensitive_ex::eq(word[0], common_lang_constants::LOWER_N) &&
                traits::case_insensitive_ex::eq(word[1], common_lang_constants::LOWER_O) &&
                traits::case_insensitive_ex::eq(word[2], common_lang_constants::LOWER_V)) ||
            (traits::case_insensitive_ex::eq(word[0], common_lang_constants::LOWER_R) &&
                traits::case_insensitive_ex::eq(word[1], common_lang_constants::LOWER_E) &&
                traits::case_insensitive_ex::eq(word[2], common_lang_constants::LOWER_D)) ||
            (traits::case_insensitive_ex::eq(word[0], common_lang_constants::LOWER_R) &&
                traits::case_insensitive_ex::eq(word[1], common_lang_constants::LOWER_E) &&
                traits::case_insensitive_ex::eq(word[2], common_lang_constants::LOWER_F)) ||
            (traits::case_insensitive_ex::eq(word[0], common_lang_constants::LOWER_R) &&
                traits::case_insensitive_ex::eq(word[1], common_lang_constants::LOWER_E) &&
                traits::case_insensitive_ex::eq(word[2], common_lang_constants::LOWER_N)) ||
            (traits::case_insensitive_ex::eq(word[0], common_lang_constants::LOWER_I) &&
                traits::case_insensitive_ex::eq(word[1], common_lang_constants::LOWER_N) &&
                traits::case_insensitive_ex::eq(word[2], common_lang_constants::LOWER_S)) ))
            { return false; }
        // pre-re-cord, pre-de-cease, pro-test, ver-te-brae
        else if (position == 4 &&
            m_length > 5 &&
            ((traits::case_insensitive_ex::eq(word[0], common_lang_constants::LOWER_P) &&
                traits::case_insensitive_ex::eq(word[1], common_lang_constants::LOWER_R) &&
                traits::case_insensitive_ex::eq(word[2], common_lang_constants::LOWER_E) &&
                (traits::case_insensitive_ex::eq(word[3], common_lang_constants::LOWER_R) || traits::case_insensitive_ex::eq(word[3], common_lang_constants::LOWER_D))) ||
            (traits::case_insensitive_ex::eq(word[0], common_lang_constants::LOWER_P) &&
                traits::case_insensitive_ex::eq(word[1], common_lang_constants::LOWER_R) &&
                traits::case_insensitive_ex::eq(word[2], common_lang_constants::LOWER_O) &&
                traits::case_insensitive_ex::eq(word[3], common_lang_constants::LOWER_T)) ||
            (traits::case_insensitive_ex::eq(word[0], common_lang_constants::LOWER_V) &&
                traits::case_insensitive_ex::eq(word[1], common_lang_constants::LOWER_E) &&
                traits::case_insensitive_ex::eq(word[2], common_lang_constants::LOWER_R) &&
                traits::case_insensitive_ex::eq(word[3], common_lang_constants::LOWER_T)) ||
            (traits::case_insensitive_ex::eq(word[0], common_lang_constants::LOWER_P) &&
                traits::case_insensitive_ex::eq(word[1], common_lang_constants::LOWER_E) &&
                traits::case_insensitive_ex::eq(word[2], common_lang_constants::LOWER_R) &&
                traits::case_insensitive_ex::eq(word[3], common_lang_constants::LOWER_P)) ||
            (traits::case_insensitive_ex::eq(word[0], common_lang_constants::LOWER_T) &&
                traits::case_insensitive_ex::eq(word[1], common_lang_constants::LOWER_R) &&
                traits::case_insensitive_ex::eq(word[2], common_lang_constants::LOWER_A) &&
                traits::case_insensitive_ex::eq(word[3], common_lang_constants::LOWER_V)) ||
            (traits::case_insensitive_ex::eq(word[0], common_lang_constants::LOWER_T) &&
                traits::case_insensitive_ex::eq(word[1], common_lang_constants::LOWER_R) &&
                traits::case_insensitive_ex::eq(word[2], common_lang_constants::LOWER_A) &&
                traits::case_insensitive_ex::eq(word[3], common_lang_constants::LOWER_P)) ||
            (traits::case_insensitive_ex::eq(word[0], common_lang_constants::LOWER_P) &&
                traits::case_insensitive_ex::eq(word[1], common_lang_constants::LOWER_A) &&
                traits::case_insensitive_ex::eq(word[2], common_lang_constants::LOWER_N) &&
                traits::case_insensitive_ex::eq(word[3], common_lang_constants::LOWER_D)) ))
            { return false; }
        // nickelodeon
        else if (position == 4 &&
            m_length > 5 &&
            traits::case_insensitive_ex::eq(word[0], common_lang_constants::LOWER_N) &&
            traits::case_insensitive_ex::eq(word[1], common_lang_constants::LOWER_I) &&
            traits::case_insensitive_ex::eq(word[2], common_lang_constants::LOWER_C) &&
            traits::case_insensitive_ex::eq(word[3], common_lang_constants::LOWER_K) &&
            traits::case_insensitive_ex::eq(word[position+1], common_lang_constants::LOWER_L))
            {
            return false;
            }
        else if (position == 4 &&
            m_length > 5 &&
            traits::case_insensitive_ex::eq(word[0], common_lang_constants::LOWER_T) &&
            traits::case_insensitive_ex::eq(word[1], common_lang_constants::LOWER_E) &&
            traits::case_insensitive_ex::eq(word[2], common_lang_constants::LOWER_M) &&
            traits::case_insensitive_ex::eq(word[3], common_lang_constants::LOWER_P))
            {
            // exception: temp
            if (m_length == 6 && traits::case_insensitive_ex::eq(word[position+1], common_lang_constants::LOWER_D))
                { return true; }
            return false;
            }

        if (position == 4 &&
            m_length > 5 &&
            traits::case_insensitive_ex::eq(word[0], common_lang_constants::LOWER_P) &&
            traits::case_insensitive_ex::eq(word[1], common_lang_constants::LOWER_E) &&
            traits::case_insensitive_ex::eq(word[2], common_lang_constants::LOWER_R) &&
            traits::case_insensitive_ex::eq(word[3], common_lang_constants::LOWER_M))
            {
            // exception: permed
            if (m_length == 6 && traits::case_insensitive_ex::eq(word[m_length-1], common_lang_constants::LOWER_D))
                { return true; }
            return false;
            }

        // int-er-est
        if (position == 5 &&
            ((traits::case_insensitive_ex::eq(word[0], common_lang_constants::LOWER_I) &&
                traits::case_insensitive_ex::eq(word[1], common_lang_constants::LOWER_N) &&
                traits::case_insensitive_ex::eq(word[2], common_lang_constants::LOWER_T) &&
                traits::case_insensitive_ex::eq(word[3], common_lang_constants::LOWER_E) &&
                traits::case_insensitive_ex::eq(word[4], common_lang_constants::LOWER_R)) ||
            (traits::case_insensitive_ex::eq(word[0], common_lang_constants::LOWER_S) &&
                traits::case_insensitive_ex::eq(word[1], common_lang_constants::LOWER_T) &&
                traits::case_insensitive_ex::eq(word[2], common_lang_constants::LOWER_R) &&
                traits::case_insensitive_ex::eq(word[3], common_lang_constants::LOWER_A) &&
                traits::case_insensitive_ex::eq(word[4], common_lang_constants::LOWER_T)) ))
            { return false; }
        // ice.. always silent
        else if (position == 2 &&
            position < (m_length-2) &&
            ((traits::case_insensitive_ex::eq(word[0], common_lang_constants::LOWER_I) &&
                traits::case_insensitive_ex::eq(word[1], common_lang_constants::LOWER_C)) ||
            (traits::case_insensitive_ex::eq(word[0], common_lang_constants::LOWER_E) &&
                traits::case_insensitive_ex::eq(word[1], common_lang_constants::LOWER_Y)) ))
            { return true; }
        // ine... and ite... 'e' is always new sound
        else if (position == 2 &&
            ((traits::case_insensitive_ex::eq(word[0], common_lang_constants::LOWER_I) &&
                (traits::case_insensitive_ex::eq(word[1], common_lang_constants::LOWER_N) || traits::case_insensitive_ex::eq(word[1], common_lang_constants::LOWER_T) )) ||
            (traits::case_insensitive_ex::eq(word[0], common_lang_constants::LOWER_E) &&
                (traits::case_insensitive_ex::eq(word[1], common_lang_constants::LOWER_L) || traits::case_insensitive_ex::eq(word[1], common_lang_constants::LOWER_X))) ||
            (traits::case_insensitive_ex::eq(word[0], common_lang_constants::LOWER_A) &&
                traits::case_insensitive_ex::eq(word[1], common_lang_constants::LOWER_D)) ||
            (traits::case_insensitive_ex::eq(word[0], common_lang_constants::LOWER_O) &&
                traits::case_insensitive_ex::eq(word[1], common_lang_constants::LOWER_B)) ) )
            { return false; }
        else if (position == 3 &&
            traits::case_insensitive_ex::eq(word[0], common_lang_constants::LOWER_H) &&
            traits::case_insensitive_ex::eq(word[1], common_lang_constants::LOWER_E) &&
            traits::case_insensitive_ex::eq(word[2], common_lang_constants::LOWER_R))
            {
            if (m_length > 5)
                {
                // here-after
                if (isChar.is_vowel(word[position+1]))
                    { return true; }
                else
                    {
                    // here-by
                    if (position+2 < m_length &&
                        ((traits::case_insensitive_ex::eq(word[position+1], common_lang_constants::LOWER_B) &&
                            traits::case_insensitive_ex::eq(word[position+2], common_lang_constants::LOWER_Y)) ||
                        (traits::case_insensitive_ex::eq(word[position+1], common_lang_constants::LOWER_T) &&
                            traits::case_insensitive_ex::eq(word[position+2], common_lang_constants::LOWER_O)) ))
                        { return true; }
                    else if (position+4 < m_length &&
                        traits::case_insensitive_ex::eq(word[position+1], common_lang_constants::LOWER_W) &&
                        traits::case_insensitive_ex::eq(word[position+2], common_lang_constants::LOWER_I) &&
                        traits::case_insensitive_ex::eq(word[position+3], common_lang_constants::LOWER_T) &&
                        traits::case_insensitive_ex::eq(word[position+4], common_lang_constants::LOWER_H))
                        { return true; }
                    else if (position+6 < m_length &&
                        traits::case_insensitive_ex::eq(word[position+1], common_lang_constants::LOWER_T) &&
                        traits::case_insensitive_ex::eq(word[position+2], common_lang_constants::LOWER_O) &&
                        traits::case_insensitive_ex::eq(word[position+3], common_lang_constants::LOWER_F) &&
                        traits::case_insensitive_ex::eq(word[position+4], common_lang_constants::LOWER_O) &&
                        traits::case_insensitive_ex::eq(word[position+5], common_lang_constants::LOWER_R) &&
                        traits::case_insensitive_ex::eq(word[position+6], common_lang_constants::LOWER_E))
                        { return true; }
                    // her-e-sy
                    return false;
                    }
                }
            }

        if ((m_length-(position+1)) >= 2)
            {
            if (is_none_affecting_suffix(word+(position+1), m_length-(position+1)) )
                { return true; }
            else if (does_suffix_negate_silent_e(word+(position+1), m_length-(position+1), next_vowel-(position+1)) )
                { return false; }
            }

        // special logic for "elist". The 'e' here is almost never silent, but have to watch out for special cases
        if (position+4 < m_length &&
            traits::case_insensitive_ex::eq(word[position+1], common_lang_constants::LOWER_L) &&
            traits::case_insensitive_ex::eq(word[position+2], common_lang_constants::LOWER_I) &&
            traits::case_insensitive_ex::eq(word[position+3], common_lang_constants::LOWER_S) &&
            traits::case_insensitive_ex::eq(word[position+4], common_lang_constants::LOWER_T))
            {
            // pricelist--'e' is silent
            if (position == 4 &&
                traits::case_insensitive_ex::eq(word[0], common_lang_constants::LOWER_P) &&
                traits::case_insensitive_ex::eq(word[1], common_lang_constants::LOWER_R) &&
                traits::case_insensitive_ex::eq(word[2], common_lang_constants::LOWER_I) &&
                traits::case_insensitive_ex::eq(word[3], common_lang_constants::LOWER_C))
                { return true; }
            else
                { return false; }
            }

        // start analyzing
        // [consonant][consonant]e
        if ( (position >= 3) &&
            (position-m_previous_vowel) > 2)
            {
            if (!isChar.is_vowel(word[position-2]) &&
                !traits::case_insensitive_ex::eq(word[position-1], word[position-2]) )
                {
                // [consonant]re causes a separate syllable
                // acre, area, hatred, sacred    
                if (traits::case_insensitive_ex::eq(word[position-1], common_lang_constants::LOWER_R) )
                    {
                    return false;
                    }
                // [consonant]le causes a separate syllable
                // able, ible, example
                else if (traits::case_insensitive_ex::eq(word[position-1], common_lang_constants::LOWER_L) &&
                    (traits::case_insensitive_ex::eq(word[position-1], common_lang_constants::LOWER_B) ||
                    traits::case_insensitive_ex::eq(word[position-1], common_lang_constants::LOWER_P)) )
                    {
                    return false;
                    }
                }
            }

        // EX
        if (m_length >= (position+2) &&
            traits::case_insensitive_ex::eq(word[position+1], common_lang_constants::LOWER_X) )
            {
            // cortex
            return false;
            }
        // EZ (only at end of word is this 100% nonsilent)
        else if (m_length == (position+2) &&
            traits::case_insensitive_ex::eq(word[position+1], common_lang_constants::LOWER_Z) )
            {
            // cortez
            return false;
            }
        // ES
        else if ((position+2) <= m_length &&
            traits::case_insensitive_ex::eq(word[position+1], common_lang_constants::LOWER_S) )
            {
            // if it is something like "processes", "ages", "hoses", "hazes": it is a new syllable
            if (traits::case_insensitive_ex::eq(word[position-1], common_lang_constants::LOWER_G) ||
                traits::case_insensitive_ex::eq(word[position-1], common_lang_constants::LOWER_C) ||
                traits::case_insensitive_ex::eq(word[position-1], common_lang_constants::LOWER_X) ||
                traits::case_insensitive_ex::eq(word[position-1], common_lang_constants::LOWER_Z) ||
                traits::case_insensitive_ex::eq(word[position-1], common_lang_constants::LOWER_S) )
                { return false; }
            //...esia
            else if ((position+3) <= m_length &&
                traits::case_insensitive_ex::eq(word[position+2], common_lang_constants::LOWER_I) &&
                traits::case_insensitive_ex::eq(word[position+3], common_lang_constants::LOWER_A) )
                { return false; }
            //[vowel]rest[vowel]
            else if ((position+4) <= m_length &&
                position >= 1 &&
                traits::case_insensitive_ex::eq(word[position-1], common_lang_constants::LOWER_R) &&
                traits::case_insensitive_ex::eq(word[position+2], common_lang_constants::LOWER_T) &&
                traits::case_insensitive_ex::eq(word[position+3], common_lang_constants::LOWER_E) )
                { return false; }
            // features
            else if (position >= 2 && isChar.is_vowel(word[position-2]))
                { return true; }
            // clothes, tastes, midinettes
            else if (position > 1 &&
                ((traits::case_insensitive_ex::eq(word[position-2], common_lang_constants::LOWER_T) &&
                    traits::case_insensitive_ex::eq(word[position-1], common_lang_constants::LOWER_H)) ||
                (traits::case_insensitive_ex::eq(word[position-2], common_lang_constants::LOWER_G) &&
                    traits::case_insensitive_ex::eq(word[position-1], common_lang_constants::LOWER_H)) ||
                (traits::case_insensitive_ex::eq(word[position-2], common_lang_constants::LOWER_S) &&
                    traits::case_insensitive_ex::eq(word[position-1], common_lang_constants::LOWER_T)) ||
                (traits::case_insensitive_ex::eq(word[position-2], common_lang_constants::LOWER_T) &&
                    traits::case_insensitive_ex::eq(word[position-1], common_lang_constants::LOWER_T)) ||
                (traits::case_insensitive_ex::eq(word[position-2], common_lang_constants::LOWER_M) &&
                    traits::case_insensitive_ex::eq(word[position-1], common_lang_constants::LOWER_M)) ))
                { return true; }
            // halves, elkes ,dwarves
            else if (position > 1 &&
                ((traits::case_insensitive_ex::eq(word[position-2], common_lang_constants::LOWER_L) &&
                    traits::case_insensitive_ex::eq(word[position-1], common_lang_constants::LOWER_V)) ||
                (traits::case_insensitive_ex::eq(word[position-2], common_lang_constants::LOWER_L) &&
                    traits::case_insensitive_ex::eq(word[position-1], common_lang_constants::LOWER_K)) ||
                (traits::case_insensitive_ex::eq(word[position-2], common_lang_constants::LOWER_Q) &&
                    traits::case_insensitive_ex::eq(word[position-1], common_lang_constants::LOWER_U)) ||
                (traits::case_insensitive_ex::eq(word[position-2], common_lang_constants::LOWER_R) &&
                    traits::case_insensitive_ex::eq(word[position-1], common_lang_constants::LOWER_V)) ))
                { return true; }
            // andes, candescent
            else if (position >= 3 &&
                traits::case_insensitive_ex::eq(word[position-1], common_lang_constants::LOWER_D) &&
                traits::case_insensitive_ex::eq(word[position-2], common_lang_constants::LOWER_N) &&
                traits::case_insensitive_ex::eq(word[position-3], common_lang_constants::LOWER_A) )
                { return false; }
            // CHES
            else if (position >= 3 &&
                traits::case_insensitive_ex::eq(word[position-2], common_lang_constants::LOWER_C) &&
                traits::case_insensitive_ex::eq(word[position-1], common_lang_constants::LOWER_H))
                {
                // aches is silent
                if (position == 3 &&
                    position+1 < m_length)
                    { return true; }
                // backaches is silent
                else if (position+2 == m_length &&
                    position == 7 &&
                    traits::case_insensitive_ex::eq(word[position-7], common_lang_constants::LOWER_B) &&
                    traits::case_insensitive_ex::eq(word[position-6], common_lang_constants::LOWER_A) &&
                    traits::case_insensitive_ex::eq(word[position-5], common_lang_constants::LOWER_C) &&
                    traits::case_insensitive_ex::eq(word[position-4], common_lang_constants::LOWER_K) &&
                    traits::case_insensitive_ex::eq(word[position-3], common_lang_constants::LOWER_A))
                    { return true; }
                // heartaches is silent
                else if (position+2 == m_length &&
                    position == 8 &&
                    traits::case_insensitive_ex::eq(word[position-8], common_lang_constants::LOWER_H) &&
                    traits::case_insensitive_ex::eq(word[position-7], common_lang_constants::LOWER_E) &&
                    traits::case_insensitive_ex::eq(word[position-6], common_lang_constants::LOWER_A) &&
                    traits::case_insensitive_ex::eq(word[position-5], common_lang_constants::LOWER_R) &&
                    traits::case_insensitive_ex::eq(word[position-4], common_lang_constants::LOWER_T) &&
                    traits::case_insensitive_ex::eq(word[position-3], common_lang_constants::LOWER_A))
                    { return true; }
                // earaches is silent
                else if (position+2 == m_length &&
                    position == 6 &&
                    traits::case_insensitive_ex::eq(word[position-6], common_lang_constants::LOWER_E) &&
                    traits::case_insensitive_ex::eq(word[position-5], common_lang_constants::LOWER_A) &&
                    traits::case_insensitive_ex::eq(word[position-4], common_lang_constants::LOWER_R) &&
                    traits::case_insensitive_ex::eq(word[position-3], common_lang_constants::LOWER_A))
                    { return true; }
                // peaches, inches
                else
                    { return false; }
                }
            // lashes, ashes
            else if (position >= 3 &&
                traits::case_insensitive_ex::eq(word[position-2], common_lang_constants::LOWER_S) &&
                traits::case_insensitive_ex::eq(word[position-1], common_lang_constants::LOWER_H))
                { return false; }
            // ben-nes
            else if (position >= 3 &&
                traits::case_insensitive_ex::eq(word[position-2], common_lang_constants::LOWER_N) &&
                traits::case_insensitive_ex::eq(word[position-1], common_lang_constants::LOWER_N))
                { return false; }
            // ter-rest-tri-al
            else if (position >= 3 &&
                traits::case_insensitive_ex::eq(word[position-2], common_lang_constants::LOWER_R) &&
                traits::case_insensitive_ex::eq(word[position-1], common_lang_constants::LOWER_R))
                { return false; }
            // con-de-scen-sion
            else if (position >= 3 &&
                traits::case_insensitive_ex::eq(word[position-2], common_lang_constants::LOWER_N) &&
                traits::case_insensitive_ex::eq(word[position-1], common_lang_constants::LOWER_D))
                { return false; }
            // court-es-y
            else if (position >= 3 &&
                traits::case_insensitive_ex::eq(word[position-2], common_lang_constants::LOWER_R) &&
                traits::case_insensitive_ex::eq(word[position-1], common_lang_constants::LOWER_T))
                { return false; }
            // mag-nes
            else if (position >= 3 &&
                traits::case_insensitive_ex::eq(word[position-2], common_lang_constants::LOWER_G) &&
                traits::case_insensitive_ex::eq(word[position-1], common_lang_constants::LOWER_N))
                { return false; }
            // ar-gues
            else if (position >= 4 &&
                traits::case_insensitive_ex::eq(word[position-3], common_lang_constants::LOWER_R) &&
                traits::case_insensitive_ex::eq(word[position-2], common_lang_constants::LOWER_G) &&
                traits::case_insensitive_ex::eq(word[position-1], common_lang_constants::LOWER_U))
                { return false; }
            // a-gues
            else if (position == 3 &&
                traits::case_insensitive_ex::eq(word[position-3], common_lang_constants::LOWER_A) &&
                traits::case_insensitive_ex::eq(word[position-2], common_lang_constants::LOWER_G) &&
                traits::case_insensitive_ex::eq(word[position-1], common_lang_constants::LOWER_U))
                { return false; }
            // a-chill-es
            else if (position >= 3 &&
                traits::case_insensitive_ex::eq(word[position-3], common_lang_constants::LOWER_I) &&
                traits::case_insensitive_ex::eq(word[position-2], common_lang_constants::LOWER_L) &&
                traits::case_insensitive_ex::eq(word[position-1], common_lang_constants::LOWER_L))
                { return false; }
            // bur-lesqu
            else if (position >= 3 &&
                traits::case_insensitive_ex::eq(word[position-2], common_lang_constants::LOWER_R) &&
                traits::case_insensitive_ex::eq(word[position-1], common_lang_constants::LOWER_L))
                { return false; }
            // mea-sles
            else if (position >= 3 &&
                traits::case_insensitive_ex::eq(word[position-2], common_lang_constants::LOWER_S) &&
                traits::case_insensitive_ex::eq(word[position-1], common_lang_constants::LOWER_L))
                { return false; }
            // par-mes-an
            else if (position >= 3 &&
                traits::case_insensitive_ex::eq(word[position-2], common_lang_constants::LOWER_R) &&
                traits::case_insensitive_ex::eq(word[position-1], common_lang_constants::LOWER_M))
                { return false; }
            else
                { return true; }
            }
        // ER
        else if (m_length >= (position+2) &&
            traits::case_insensitive_ex::eq(word[position+1], common_lang_constants::LOWER_R) )
            { return false; }
        // CHEMI
        else if (m_length >= (position+3) && position >= 2 &&
            traits::case_insensitive_ex::eq(word[position-2], common_lang_constants::LOWER_C) &&
            traits::case_insensitive_ex::eq(word[position-1], common_lang_constants::LOWER_H) &&
            traits::case_insensitive_ex::eq(word[position+1], common_lang_constants::LOWER_M) &&
            traits::case_insensitive_ex::eq(word[position+2], common_lang_constants::LOWER_I))
            { return false; }
        // EP (door-step)
        else if (m_length == (position+2) &&
            traits::case_insensitive_ex::eq(word[position+1], common_lang_constants::LOWER_P) )
            { return false; }
        // EPS (door-steps)
        else if (m_length == (position+3) &&
            traits::case_insensitive_ex::eq(word[position+1], common_lang_constants::LOWER_P) &&
            traits::case_insensitive_ex::eq(word[position+2], common_lang_constants::LOWER_S) )
            { return false; }
        // EN (heath-en, chos-en)
        else if (m_length >= (position+2) &&
            traits::case_insensitive_ex::eq(word[position+1], common_lang_constants::LOWER_N) )
            { return false; }
        // EL
        else if (m_length >= (position+2) &&
            traits::case_insensitive_ex::eq(word[position+1], common_lang_constants::LOWER_L) )
            {
            // lovely
            if (traits::case_insensitive_ex::eq(word[position+2], common_lang_constants::LOWER_Y) )
                { return true; }
            // travel
            else if (position+2 == m_length)
                { return false; }
            // ape-look (two consecutive vowels cannot start syllable)
            else if ( (position+3 < m_length) &&
                isChar.is_vowel(word[position+2]) &&
                isChar.is_vowel(word[position+3]) )
                { return true; }
            else
                { return true; }
            }
        // ET
        else if (m_length >= (position+2) &&
            traits::case_insensitive_ex::eq(word[position+1], common_lang_constants::LOWER_T) )
            {
            if (m_length >= (position+3) )
                {
                // first, check for explicitly known suffixes that can follow a silent 'e'
                if (position+5 < m_length &&
                    traits::case_insensitive_ex::eq(word[position+1], common_lang_constants::LOWER_T) &&
                    traits::case_insensitive_ex::eq(word[position+2], common_lang_constants::LOWER_H) &&
                    traits::case_insensitive_ex::eq(word[position+3], common_lang_constants::LOWER_I) &&
                    traits::case_insensitive_ex::eq(word[position+4], common_lang_constants::LOWER_N) &&
                    traits::case_insensitive_ex::eq(word[position+5], common_lang_constants::LOWER_G))
                    { return true; }
                else if (position+4 < m_length &&
                    ((traits::case_insensitive_ex::eq(word[position+1], common_lang_constants::LOWER_T) &&
                        traits::case_insensitive_ex::eq(word[position+2], common_lang_constants::LOWER_A) &&
                        traits::case_insensitive_ex::eq(word[position+3], common_lang_constants::LOWER_K) &&
                        traits::case_insensitive_ex::eq(word[position+4], common_lang_constants::LOWER_E)) ||
                    (traits::case_insensitive_ex::eq(word[position+1], common_lang_constants::LOWER_T) &&
                        traits::case_insensitive_ex::eq(word[position+2], common_lang_constants::LOWER_I) &&
                        traits::case_insensitive_ex::eq(word[position+3], common_lang_constants::LOWER_M) &&
                        traits::case_insensitive_ex::eq(word[position+4], common_lang_constants::LOWER_E)) ) )
                    { return true; }
                //"ninety" and "safety" are special cases that set the 'e' to be silent
                else if (position >= 3 &&
                    m_length >= 6 &&
                    ((traits::case_insensitive_ex::eq(word[0], common_lang_constants::LOWER_N) &&
                        traits::case_insensitive_ex::eq(word[1], common_lang_constants::LOWER_I) &&
                        traits::case_insensitive_ex::eq(word[2], common_lang_constants::LOWER_N) &&
                        traits::case_insensitive_ex::eq(word[3], common_lang_constants::LOWER_E) &&
                        traits::case_insensitive_ex::eq(word[4], common_lang_constants::LOWER_T) &&
                        traits::case_insensitive_ex::eq(word[5], common_lang_constants::LOWER_Y)) ||
                    (traits::case_insensitive_ex::eq(word[0], common_lang_constants::LOWER_S) &&
                        traits::case_insensitive_ex::eq(word[1], common_lang_constants::LOWER_A) &&
                        traits::case_insensitive_ex::eq(word[2], common_lang_constants::LOWER_F) &&
                        traits::case_insensitive_ex::eq(word[3], common_lang_constants::LOWER_E) &&
                        traits::case_insensitive_ex::eq(word[4], common_lang_constants::LOWER_T) &&
                        traits::case_insensitive_ex::eq(word[5], common_lang_constants::LOWER_Y)) ))
                    { return true; }
                // verify that it is not something like "someEthing" or "etr"
                else if (next_vowel != m_length && (next_vowel-2) > position)
                    { return true; }
                else return false;
                }
            // puppEt, comEth
            else
                { return false; }
            }
        // ED
        else if (m_length >= (position+2) &&
            traits::case_insensitive_ex::eq(word[position+1], common_lang_constants::LOWER_D) )
            {
            // if "ted" and "ded" then 'ed' is a new syllable
            //(e.g., "exited", "refuted", "acted", "embedded")
            if (traits::case_insensitive_ex::eq(word[position-1], common_lang_constants::LOWER_T) ||
                traits::case_insensitive_ex::eq(word[position-1], common_lang_constants::LOWER_D) )
                { return false; }
            // boredom
            else if (position+3 < m_length &&
                traits::case_insensitive_ex::eq(word[position+1], common_lang_constants::LOWER_D) &&
                traits::case_insensitive_ex::eq(word[position+2], common_lang_constants::LOWER_O) &&
                traits::case_insensitive_ex::eq(word[position+3], common_lang_constants::LOWER_M))
                { return true; }
            // tragedy
            else if (traits::case_insensitive_ex::eq(word[position+2], common_lang_constants::LOWER_Y) ||
                traits::case_insensitive_ex::eq(word[position+2], common_lang_constants::LOWER_I) )
                { return false; }
            else if (m_length >= (position+3) )
                {
                // verify that it is not something like "edr"
                if (next_vowel != m_length && (next_vowel-2) > position)
                    { return true; }
                else return false;
                }
            // something like "raked"
            else
                { return true; }
            }
        // E and various consonants at end of word
        else if (position+2 == m_length &&
            (traits::case_insensitive_ex::eq(word[position+1], common_lang_constants::LOWER_B) ||
            traits::case_insensitive_ex::eq(word[position+1], common_lang_constants::LOWER_C) ||
            traits::case_insensitive_ex::eq(word[position+1], common_lang_constants::LOWER_K) ||
            traits::case_insensitive_ex::eq(word[position+1], common_lang_constants::LOWER_M) ||
            traits::case_insensitive_ex::eq(word[position+1], common_lang_constants::LOWER_N) ||
            traits::case_insensitive_ex::eq(word[position+1], common_lang_constants::LOWER_W) ||
            traits::case_insensitive_ex::eq(word[position+1], common_lang_constants::LOWER_Y)) )
            { return false; }
        /// E consonant O at end of word
        /// toledo, imported words (e.g. Italian words)
        else if (position+3 == m_length &&
            !isChar.is_vowel(word[position+1]) &&
            traits::case_insensitive_ex::eq(word[position+2], common_lang_constants::LOWER_O) )
            { return false; }
        else if (position >= 4)
            {
            if (traits::case_insensitive_ex::eq(word[position-4], common_lang_constants::LOWER_A) &&
                traits::case_insensitive_ex::eq(word[position-3], common_lang_constants::LOWER_R) &&
                traits::case_insensitive_ex::eq(word[position-2], common_lang_constants::LOWER_G) &&
                traits::case_insensitive_ex::eq(word[position-1], common_lang_constants::LOWER_U) )
                { return false; }
            }
        return true;
        }

    bool english_syllabize::is_vowels_separate_syllables(const wchar_t* word,
                                          const size_t position, const size_t vowel_block_size,
                                          const bool is_first_vowel_block_in_word) const
        {
        assert(word);
        // queue
        if (vowel_block_size == 3 &&
            traits::case_insensitive_ex::eq(word[position], common_lang_constants::LOWER_E) &&
            traits::case_insensitive_ex::eq(word[position+1], common_lang_constants::LOWER_U) &&
            traits::case_insensitive_ex::eq(word[position+2], common_lang_constants::LOWER_E))
            { return false; }
        // re-unification, re-use, but not 'reu-ters'
        else if (vowel_block_size == 2 &&
            position == 1 &&
            m_length >= 4 &&
            traits::case_insensitive_ex::eq(word[position-1], common_lang_constants::LOWER_R) &&
            traits::case_insensitive_ex::eq(word[position], common_lang_constants::LOWER_E) &&
            traits::case_insensitive_ex::eq(word[position+1], common_lang_constants::LOWER_U) &&
            (traits::case_insensitive_ex::eq(word[position+2], common_lang_constants::LOWER_N) ||
             traits::case_insensitive_ex::eq(word[position+2], common_lang_constants::LOWER_S)) )
            { return true; }
        // nious will always split
        else if (vowel_block_size == 3 &&
            position > 0 &&
            m_length >= 5 &&
            traits::case_insensitive_ex::eq(word[position-1], common_lang_constants::LOWER_N) &&
            traits::case_insensitive_ex::eq(word[position], common_lang_constants::LOWER_I) &&
            traits::case_insensitive_ex::eq(word[position+1], common_lang_constants::LOWER_O) &&
            traits::case_insensitive_ex::eq(word[position+2], common_lang_constants::LOWER_U) &&
            traits::case_insensitive_ex::eq(word[position+3], common_lang_constants::LOWER_S))
            { return true; }
        // se-er
        else if (position > 0 && (position <= m_length -3) && (vowel_block_size == 2) &&
            traits::case_insensitive_ex::eq(word[position-1], common_lang_constants::LOWER_S) &&
            traits::case_insensitive_ex::eq(word[position], common_lang_constants::LOWER_E) &&
            traits::case_insensitive_ex::eq(word[position+1], common_lang_constants::LOWER_E) &&
            traits::case_insensitive_ex::eq(word[position+2], common_lang_constants::LOWER_R))
            { return true; }
        else if (vowel_block_size == 2 &&
            traits::case_insensitive_ex::eq(word[position], common_lang_constants::LOWER_A) &&
            traits::case_insensitive_ex::eq(word[position+1], common_lang_constants::LOWER_I_UMLAUTS))
            { return true; }
        // Check for vowel followed by "ism"--this always splits
        // truism, heroism
        else if (vowel_block_size == 2 &&
            position+3 < m_length &&
            traits::case_insensitive_ex::eq(word[position+1], common_lang_constants::LOWER_I) &&
            traits::case_insensitive_ex::eq(word[position+2], common_lang_constants::LOWER_S) &&
            traits::case_insensitive_ex::eq(word[position+3], common_lang_constants::LOWER_M) )
            { return true; }
        // Check for vowel followed by "ist"--this always splits
        // egoist
        else if (m_syllable_count >= 1 &&
            vowel_block_size == 2 &&
            position+3 < m_length &&
            traits::case_insensitive_ex::eq(word[position+1], common_lang_constants::LOWER_I) &&
            traits::case_insensitive_ex::eq(word[position+2], common_lang_constants::LOWER_S) &&
            traits::case_insensitive_ex::eq(word[position+3], common_lang_constants::LOWER_T) )
            {
            // exception: shirt-waist
            if (position > 0 &&
                traits::case_insensitive_ex::eq(word[position-1], common_lang_constants::LOWER_W) &&
                traits::case_insensitive_ex::eq(word[position], common_lang_constants::LOWER_A))
                { return false; }
            return true;
            }
        // theist
        else if (vowel_block_size == 2 &&
            position+3 < m_length &&
            position == 2 &&
            traits::case_insensitive_ex::eq(word[0], common_lang_constants::LOWER_T) &&
            traits::case_insensitive_ex::eq(word[1], common_lang_constants::LOWER_H) &&
            traits::case_insensitive_ex::eq(word[2], common_lang_constants::LOWER_E) &&
            traits::case_insensitive_ex::eq(word[3], common_lang_constants::LOWER_I) &&
            traits::case_insensitive_ex::eq(word[4], common_lang_constants::LOWER_S) &&
            traits::case_insensitive_ex::eq(word[5], common_lang_constants::LOWER_T) )
            {
            // exception: shirt-waist
            if (position > 0 &&
                traits::case_insensitive_ex::eq(word[position-1], common_lang_constants::LOWER_W) &&
                traits::case_insensitive_ex::eq(word[position], common_lang_constants::LOWER_A))
                { return false; }
            return true;
            }
        // Check for vowel followed by "ic"--this always splits
        // stoic, heroic
        else if (vowel_block_size == 2 &&
            position+2 < m_length &&
            traits::case_insensitive_ex::eq(word[position+1], common_lang_constants::LOWER_I) &&
            traits::case_insensitive_ex::eq(word[position+2], common_lang_constants::LOWER_C) &&
            (position+3 == m_length ||
             !traits::case_insensitive_ex::eq(word[position+3], common_lang_constants::LOWER_E)))
            {
            // juice and sluice are exceptions
            if (traits::case_insensitive_ex::eq(word[position], common_lang_constants::LOWER_U) &&
                position > 0 &&
                (traits::case_insensitive_ex::eq(word[position-1], common_lang_constants::LOWER_J) ||
                traits::case_insensitive_ex::eq(word[position-1], common_lang_constants::LOWER_L)))
                { return false; }
            return true;
            }
        // [vowel]ing always splits
        else if (vowel_block_size == 2 &&
            position+3 < m_length &&
            traits::case_insensitive_ex::eq(word[position+1], common_lang_constants::LOWER_I) &&
            traits::case_insensitive_ex::eq(word[position+2], common_lang_constants::LOWER_N) &&
            traits::case_insensitive_ex::eq(word[position+3], common_lang_constants::LOWER_G) )
            { return true; }
        // poly[vowel] always splits
        else if (vowel_block_size >= 2 && position == 3 &&
            traits::case_insensitive_ex::eq(word[0], common_lang_constants::LOWER_P) &&
            traits::case_insensitive_ex::eq(word[1], common_lang_constants::LOWER_O) &&
            traits::case_insensitive_ex::eq(word[2], common_lang_constants::LOWER_L) &&
            traits::case_insensitive_ex::eq(word[3], common_lang_constants::LOWER_Y))
            { return true; }
        // eex always splits
        else if (vowel_block_size == 2 &&
            position+2 < m_length &&
            traits::case_insensitive_ex::eq(word[position], common_lang_constants::LOWER_E) &&
            traits::case_insensitive_ex::eq(word[position+1], common_lang_constants::LOWER_E) &&
            traits::case_insensitive_ex::eq(word[position+2], common_lang_constants::LOWER_X))
            { return true; }
        // ooe always splits
        else if (vowel_block_size == 2 &&
            position+2 < m_length &&
            traits::case_insensitive_ex::eq(word[position], common_lang_constants::LOWER_O) &&
            traits::case_insensitive_ex::eq(word[position+1], common_lang_constants::LOWER_O) &&
            traits::case_insensitive_ex::eq(word[position+2], common_lang_constants::LOWER_E))
            { return true; }
        // fry-er, fly-er
        else if (vowel_block_size == 2 &&
            position >= 2 &&
            traits::case_insensitive_ex::eq(word[position], common_lang_constants::LOWER_Y) &&
            traits::case_insensitive_ex::eq(word[position+1], common_lang_constants::LOWER_E) &&
            (traits::case_insensitive_ex::eq(word[position-1], common_lang_constants::LOWER_L) ||
                traits::case_insensitive_ex::eq(word[position-1], common_lang_constants::LOWER_R)) )
            { return true; }

        size_t next_vowel{0};
        // start from last position and backtrack
        for (next_vowel = (position+vowel_block_size); next_vowel < m_length; ++next_vowel)
                {
                /* found another vowel before end of word,
                   so this 'e' is still a candidate*/
                if (isChar.is_vowel(word[next_vowel]) )
                    { break; }
            }
        if (vowel_block_size > 2)
            {
            // beau, bourgeois, adieu, ciao
            if (vowel_block_size == 3 &&
                ((traits::case_insensitive_ex::eq(word[position], common_lang_constants::LOWER_E) &&
                    traits::case_insensitive_ex::eq(word[position+1], common_lang_constants::LOWER_A) &&
                    traits::case_insensitive_ex::eq(word[position+2], common_lang_constants::LOWER_U)) ||
                (traits::case_insensitive_ex::eq(word[position], common_lang_constants::LOWER_E) &&
                    traits::case_insensitive_ex::eq(word[position+1], common_lang_constants::LOWER_O) &&
                    traits::case_insensitive_ex::eq(word[position+2], common_lang_constants::LOWER_I)) ||
                (traits::case_insensitive_ex::eq(word[position], common_lang_constants::LOWER_I) &&
                    traits::case_insensitive_ex::eq(word[position+1], common_lang_constants::LOWER_A) &&
                    traits::case_insensitive_ex::eq(word[position+2], common_lang_constants::LOWER_O)) ||
                (traits::case_insensitive_ex::eq(word[position], common_lang_constants::LOWER_I) &&
                    traits::case_insensitive_ex::eq(word[position+1], common_lang_constants::LOWER_E) &&
                    traits::case_insensitive_ex::eq(word[position+2], common_lang_constants::LOWER_U)) ||
                (traits::case_insensitive_ex::eq(word[position], common_lang_constants::LOWER_O) &&
                    traits::case_insensitive_ex::eq(word[position+1], common_lang_constants::LOWER_O) &&
                    traits::case_insensitive_ex::eq(word[position+2], common_lang_constants::LOWER_E)) ) )
                { return false; }
            else if (vowel_block_size == 3 &&
                traits::case_insensitive_ex::eq(word[position], common_lang_constants::LOWER_E) &&
                traits::case_insensitive_ex::eq(word[position+1], common_lang_constants::LOWER_O) &&
                traits::case_insensitive_ex::eq(word[position+2], common_lang_constants::LOWER_U))
                {
                // herbaceous, advantageous, righteous
                if (position >= 1 &&
                    (traits::case_insensitive_ex::eq(word[position-1], common_lang_constants::LOWER_C) ||
                    traits::case_insensitive_ex::eq(word[position-1], common_lang_constants::LOWER_G) ||
                    traits::case_insensitive_ex::eq(word[position-1], common_lang_constants::LOWER_R) ) )
                    { return false; }
                else
                    { return true; }
                }
            else if (vowel_block_size == 3 &&
                traits::case_insensitive_ex::eq(word[position], common_lang_constants::LOWER_I) &&
                traits::case_insensitive_ex::eq(word[position+1], common_lang_constants::LOWER_O) &&
                traits::case_insensitive_ex::eq(word[position+2], common_lang_constants::LOWER_U))
                {
                // superCili-ous, puncTili-ous
                if (position >= 3 &&
                    (traits::case_insensitive_ex::eq(word[position-3], common_lang_constants::LOWER_C) ||
                    traits::case_insensitive_ex::eq(word[position-3], common_lang_constants::LOWER_T) ) &&
                    traits::case_insensitive_ex::eq(word[position-2], common_lang_constants::LOWER_I) &&
                    traits::case_insensitive_ex::eq(word[position-1], common_lang_constants::LOWER_L) )
                    { return true; }
                // re-li-gious, capricious, obnoxious, ambitious, bilious
                else if (position > 0 &&
                    (traits::case_insensitive_ex::eq(word[position-1], common_lang_constants::LOWER_C) ||
                    traits::case_insensitive_ex::eq(word[position-1], common_lang_constants::LOWER_G) ||
                    traits::case_insensitive_ex::eq(word[position-1], common_lang_constants::LOWER_L) ||
                    traits::case_insensitive_ex::eq(word[position-1], common_lang_constants::LOWER_T) ||
                    traits::case_insensitive_ex::eq(word[position-1], common_lang_constants::LOWER_X) ) )
                    { return false; }
                // no-tor-i-ous
                else
                    { return true; }
                }
            // any other combination probably splits
            else
                { return true; }
            }
        // chaos
        else if ((position+1 < m_length) &&
            traits::case_insensitive_ex::eq(word[position], common_lang_constants::LOWER_A) &&
            traits::case_insensitive_ex::eq(word[position+1], common_lang_constants::LOWER_O))
            {
            // exception: gaol
            if (position == 1 && m_length >= 4 &&
                traits::case_insensitive_ex::eq(word[0], common_lang_constants::LOWER_G) &&
                traits::case_insensitive_ex::eq(word[3], common_lang_constants::LOWER_L))
                { return false; }
            return true;
            }
        // coliseum
        else if (position+3 <= m_length &&
            (traits::case_insensitive_ex::eq(word[position], common_lang_constants::LOWER_E) &&
                traits::case_insensitive_ex::eq(word[position+1], common_lang_constants::LOWER_U)) &&
            m_syllable_count >= 1 /* Zeus*/ &&
            (traits::case_insensitive_ex::eq(word[position+2], common_lang_constants::LOWER_M) ||
            (traits::case_insensitive_ex::eq(word[position+2], common_lang_constants::LOWER_S) &&
             // diseuse
             !traits::case_insensitive_ex::eq(word[position-1], common_lang_constants::LOWER_S))) )
            { return true; }
        // boa, boat
        else if ((position+1 < m_length) &&
            traits::case_insensitive_ex::eq(word[position], common_lang_constants::LOWER_O) &&
            traits::case_insensitive_ex::eq(word[position+1], common_lang_constants::LOWER_A))
            {
            // oar, oak
            if (position == 0)
                {
                // special case for oasis
                if (m_length > 3 &&
                    traits::case_insensitive_ex::eq(word[position+2], common_lang_constants::LOWER_S) &&
                    traits::case_insensitive_ex::eq(word[position+3], common_lang_constants::LOWER_I))
                    { return true; }
                else
                    { return false; }
                }
            // first chance logic for special case "coa[consonant]"
            else if (position >= 1 &&
                traits::case_insensitive_ex::eq(word[position-1], common_lang_constants::LOWER_C) &&
                position+3 <= m_length)
                {
                // coagulate, coadjutant
                if (traits::case_insensitive_ex::eq(word[position+2], common_lang_constants::LOWER_D) ||
                    traits::case_insensitive_ex::eq(word[position+2], common_lang_constants::LOWER_G) )
                    {
                    return true;
                    }
                // coacervate, coacH
                else if ((position+3 == m_length ||
                    !traits::case_insensitive_ex::eq(word[position+3], common_lang_constants::LOWER_H)) &&
                    traits::case_insensitive_ex::eq(word[position+2], common_lang_constants::LOWER_C) )
                    {
                    return true;
                    }
                // coalesce, coal (coaler, coaled)
                else if (position+4 <= m_length &&
                    traits::case_insensitive_ex::eq(word[position+2], common_lang_constants::LOWER_L) &&
                    isChar.is_vowel(word[position+3]) )
                    {
                    if (position+5 <= m_length &&
                        traits::case_insensitive_ex::eq(word[position+3], common_lang_constants::LOWER_E) &&
                        (traits::case_insensitive_ex::eq(word[position+4], common_lang_constants::LOWER_R) ||
                            traits::case_insensitive_ex::eq(word[position+4], common_lang_constants::LOWER_D)) )
                        { return false; }
                    else
                        { return true; }
                    }
                }

            // jOAnne
            if (position+3 <= m_length &&
                traits::case_insensitive_ex::eq(word[position+1], common_lang_constants::LOWER_A) &&
                traits::case_insensitive_ex::eq(word[position+2], common_lang_constants::LOWER_N) &&
                traits::case_insensitive_ex::eq(word[position+3], common_lang_constants::LOWER_N) )
                { return true; }

            // if at the end of the word then they split (boa)
            if (position+2 == m_length)
                {
                //"cocoa" is and exception
                if (m_length == 5 &&
                    traits::case_insensitive_ex::eq(word[0], common_lang_constants::LOWER_C) &&
                    traits::case_insensitive_ex::eq(word[1], common_lang_constants::LOWER_O) &&
                    traits::case_insensitive_ex::eq(word[2], common_lang_constants::LOWER_C))
                    { return false; }
                return true;
                }
            // psychoa splots
            else if (position == 5 &&
                traits::case_insensitive_ex::eq(word[0], common_lang_constants::LOWER_P) &&
                traits::case_insensitive_ex::eq(word[1], common_lang_constants::LOWER_S) &&
                traits::case_insensitive_ex::eq(word[2], common_lang_constants::LOWER_Y) &&
                traits::case_insensitive_ex::eq(word[3], common_lang_constants::LOWER_C) &&
                traits::case_insensitive_ex::eq(word[4], common_lang_constants::LOWER_H))
                { return true; }
            //...otherwise it's one sound
            else
                { return false; }
            }
        // duet, issuer, fuel
        else if ((position+1 < m_length) &&
            traits::case_insensitive_ex::eq(word[position], common_lang_constants::LOWER_U) &&
            traits::case_insensitive_ex::eq(word[position+1], common_lang_constants::LOWER_E))
            {
            ///@bug Puert-o Rico is broken.
            if ((position+2) < m_length &&
                (traits::case_insensitive_ex::eq(word[position+2], common_lang_constants::LOWER_T) ||
                traits::case_insensitive_ex::eq(word[position+2], common_lang_constants::LOWER_R) ||
                traits::case_insensitive_ex::eq(word[position+2], common_lang_constants::LOWER_L) ) )
                {
                return true;
                }
            else if ((position+3) < m_length)
                {
                // affluence, fluent
                if (traits::case_insensitive_ex::eq(word[position+2], common_lang_constants::LOWER_N) &&
                    (traits::case_insensitive_ex::eq(word[position+3], common_lang_constants::LOWER_C) ||
                    traits::case_insensitive_ex::eq(word[position+3], common_lang_constants::LOWER_T)) )
                    {
                    return true;
                    }
                // bluest
                else if (traits::case_insensitive_ex::eq(word[position+2], common_lang_constants::LOWER_S) &&
                        traits::case_insensitive_ex::eq(word[position+3], common_lang_constants::LOWER_T) )
                    {
                    return true;
                    }
                else
                    {
                    return false;
                    }
                }
            else
                {
                return false;
                }
            }
        // simplifying
        else if ((position+1 < m_length) &&
            traits::case_insensitive_ex::eq(word[position], common_lang_constants::LOWER_Y) &&
            traits::case_insensitive_ex::eq(word[position+1], common_lang_constants::LOWER_I))
            {
            return true;
            }
        // OI
        else if ((position+1 < m_length) &&
            traits::case_insensitive_ex::eq(word[position], common_lang_constants::LOWER_O) &&
            traits::case_insensitive_ex::eq(word[position+1], common_lang_constants::LOWER_I))
            {
            // O-IZE
            if ((position+3 < m_length) &&
                traits::case_insensitive_ex::eq(word[position+2], common_lang_constants::LOWER_Z) &&
                traits::case_insensitive_ex::eq(word[position+3], common_lang_constants::LOWER_E) )
                { return true; }
            // co-in-cide
            else if ((position+3 < m_length) && (position >= 1) &&
                traits::case_insensitive_ex::eq(word[position-1], common_lang_constants::LOWER_C) &&
                traits::case_insensitive_ex::eq(word[position+2], common_lang_constants::LOWER_N) &&
                traits::case_insensitive_ex::eq(word[position+3], common_lang_constants::LOWER_C) )
                { return true; }
            // anything else is like "an-droid"  or "join"
            else
                { return false; }
            }
        // rye, goodbye
        else if ((position+1 < m_length) &&
            traits::case_insensitive_ex::eq(word[position], common_lang_constants::LOWER_Y) &&
            traits::case_insensitive_ex::eq(word[position+1], common_lang_constants::LOWER_E))
            {
            // dryer
            if (position+3 == m_length &&
                traits::case_insensitive_ex::eq(word[position+2], common_lang_constants::LOWER_R) )
                {
                return true;
                }
            else
                {
                return false;
                }
            }
        // bryan
        else if (traits::case_insensitive_ex::eq(word[position], common_lang_constants::LOWER_Y) )
            {
            return true;
            }
        // IO
        else if ((position+1 < m_length) &&
            traits::case_insensitive_ex::eq(word[position], common_lang_constants::LOWER_I) &&
            traits::case_insensitive_ex::eq(word[position+1], common_lang_constants::LOWER_O))
            {
            // if at the end of the word then always split (e.g., bio, radio, io)
            if (position + 2 == m_length ||
                // lion, dion, pion
                (m_previous_vowel == m_length))
                {
                return true;
                }
            else if ((position > 0) &&
                ((position + 3) < m_length) &&
                (traits::case_insensitive_ex::eq(word[position-1], common_lang_constants::LOWER_C) &&
                traits::case_insensitive_ex::eq(word[position], common_lang_constants::LOWER_I) &&
                traits::case_insensitive_ex::eq(word[position+1], common_lang_constants::LOWER_O) &&
                traits::case_insensitive_ex::eq(word[position+2], common_lang_constants::LOWER_U) &&
                traits::case_insensitive_ex::eq(word[position+3], common_lang_constants::LOWER_S)) )
                {
                return false;
                }
            // legion, nation, union
            else if (position > 0 &&
                position+2 < m_length &&
                traits::case_insensitive_ex::eq(word[position+2], common_lang_constants::LOWER_N) &&
                (traits::case_insensitive_ex::eq(word[position-1], common_lang_constants::LOWER_L) ||
                traits::case_insensitive_ex::eq(word[position-1], common_lang_constants::LOWER_N) ||
                traits::case_insensitive_ex::eq(word[position-1], common_lang_constants::LOWER_S) ||
                traits::case_insensitive_ex::eq(word[position-1], common_lang_constants::LOWER_G) ||
                traits::case_insensitive_ex::eq(word[position-1], common_lang_constants::LOWER_T) ||
                traits::case_insensitive_ex::eq(word[position-1], common_lang_constants::LOWER_X) ) )
                {
                return false;
                }
            // fashion
            else if ((position > 1) &&
                ((position + 2) < m_length) &&
                ((traits::case_insensitive_ex::eq(word[position-2], common_lang_constants::LOWER_S) &&
                    traits::case_insensitive_ex::eq(word[position-1], common_lang_constants::LOWER_H) &&
                    traits::case_insensitive_ex::eq(word[position], common_lang_constants::LOWER_I) &&
                    traits::case_insensitive_ex::eq(word[position+1], common_lang_constants::LOWER_O) &&
                    traits::case_insensitive_ex::eq(word[position+2], common_lang_constants::LOWER_N)) ||
                (traits::case_insensitive_ex::eq(word[position-2], common_lang_constants::LOWER_C) &&
                    traits::case_insensitive_ex::eq(word[position-1], common_lang_constants::LOWER_H) &&
                    traits::case_insensitive_ex::eq(word[position], common_lang_constants::LOWER_I) &&
                    traits::case_insensitive_ex::eq(word[position+1], common_lang_constants::LOWER_O) &&
                    traits::case_insensitive_ex::eq(word[position+2], common_lang_constants::LOWER_N)) ) )
                {
                return false;
                }
            else
                {
                return true;
                }
            }
        // IA
        else if ((position+1 < m_length) &&
            traits::case_insensitive_ex::eq(word[position], common_lang_constants::LOWER_I) &&
            traits::case_insensitive_ex::eq(word[position+1], common_lang_constants::LOWER_A))
            {
            // [consonant]ia[consonant]
            if ( (position > 0) && (position+2 < m_length) )
                {
                // partial
                if (traits::case_insensitive_ex::eq(word[position-1], common_lang_constants::LOWER_T) &&
                    traits::case_insensitive_ex::eq(word[position+2], common_lang_constants::LOWER_L) )
                    { return false; }
                // christI-ANIty
                else if ((position+3 < m_length) &&
                    traits::case_insensitive_ex::eq(word[position-1], common_lang_constants::LOWER_T) &&
                    traits::case_insensitive_ex::eq(word[position+2], common_lang_constants::LOWER_N) &&
                        (traits::case_insensitive_ex::eq(word[position+3], common_lang_constants::LOWER_I) ||
                        traits::case_insensitive_ex::eq(word[position+3], common_lang_constants::LOWER_T)) )
                    { return true; }
                // christian
                else if (traits::case_insensitive_ex::eq(word[position-1], common_lang_constants::LOWER_T) &&
                    traits::case_insensitive_ex::eq(word[position+2], common_lang_constants::LOWER_N) )
                    { return false; }
                // technician, special
                else if (traits::case_insensitive_ex::eq(word[position-1], common_lang_constants::LOWER_C) &&
                    (traits::case_insensitive_ex::eq(word[position+2], common_lang_constants::LOWER_N) ||
                    traits::case_insensitive_ex::eq(word[position+2], common_lang_constants::LOWER_L) ) )
                    { return false; }
                // cartesian
                else if (traits::case_insensitive_ex::eq(word[position-1], common_lang_constants::LOWER_S) &&
                    traits::case_insensitive_ex::eq(word[position+2], common_lang_constants::LOWER_N) )
                    { return false; }
                // carnaSSI-AL
                else if ((position >= 2) &&
                    traits::case_insensitive_ex::eq(word[position-2], common_lang_constants::LOWER_S) &&
                    traits::case_insensitive_ex::eq(word[position-1], common_lang_constants::LOWER_S) &&
                    traits::case_insensitive_ex::eq(word[position+2], common_lang_constants::LOWER_L) )
                    { return true; }
                // controver-SIAL
                else if (traits::case_insensitive_ex::eq(word[position-1], common_lang_constants::LOWER_S) &&
                    traits::case_insensitive_ex::eq(word[position+2], common_lang_constants::LOWER_L) )
                    { return false; }
                // marriage
                else if (position >= 3 &&
                    traits::case_insensitive_ex::eq(word[position-3], common_lang_constants::LOWER_A) &&
                    traits::case_insensitive_ex::eq(word[position-2], common_lang_constants::LOWER_R) &&
                    traits::case_insensitive_ex::eq(word[position-1], common_lang_constants::LOWER_R) &&
                    traits::case_insensitive_ex::eq(word[position+2], common_lang_constants::LOWER_G) )
                    { return false; }
                // co-lle-giate, pla-giar-ize
                else if (traits::case_insensitive_ex::eq(word[position-1], common_lang_constants::LOWER_G) &&
                    (traits::case_insensitive_ex::eq(word[position+2], common_lang_constants::LOWER_T) ||
                        traits::case_insensitive_ex::eq(word[position+2], common_lang_constants::LOWER_R)) )
                    {
                    if (position == 1)
                        { return true; }
                    else if (position >= 2 &&
                        traits::case_insensitive_ex::eq(word[position-2], common_lang_constants::LOWER_G))
                        { return true; }
                    else
                        { return false; }
                    }
                // historian
                else
                    {
                    return true;
                    }
                }
            // diabetes
            else if (position+2 < m_length && position > 0 &&
                    (traits::case_insensitive_ex::eq(word[position+2], common_lang_constants::LOWER_B)) )
                {
                return false;
                }
            ///@bug ammonia is broken
            // fantasia
            else if (position+2 == m_length && position > 0 &&
                    traits::case_insensitive_ex::eq(word[position-1], common_lang_constants::LOWER_S) )
                {
                return false;
                }
            // dial, pia
            else
                {
                return true;
                }
            }
        // EE
        else if ((position+1 < m_length) &&
            traits::case_insensitive_ex::eq(word[position], common_lang_constants::LOWER_E) &&
            traits::case_insensitive_ex::eq(word[position+1], common_lang_constants::LOWER_E))
            {
            ///@bug seer is broken
            return false;
            }
        // EI
        else if ((position+1 < m_length) &&
            traits::case_insensitive_ex::eq(word[position], common_lang_constants::LOWER_E) &&
            traits::case_insensitive_ex::eq(word[position+1], common_lang_constants::LOWER_I))
            {
            // reign
            if (position+3 < m_length &&
                traits::case_insensitive_ex::eq(word[position+2], common_lang_constants::LOWER_G) &&
                traits::case_insensitive_ex::eq(word[position+3], common_lang_constants::LOWER_N) )
                {
                return false;
                }
            // cor-por-e-it-y
            else if ((position == m_length-4) &&
                traits::case_insensitive_ex::eq(word[position+2], common_lang_constants::LOWER_T) &&
                traits::case_insensitive_ex::eq(word[position+3], common_lang_constants::LOWER_Y))
                { return true; }
            else if (position == 1)
                {
                // reintroduce
                if (traits::case_insensitive_ex::eq(word[position-1], common_lang_constants::LOWER_R))
                    { return true; }
                // deity, deirdre
                else if (position+3 < m_length &&
                    traits::case_insensitive_ex::eq(word[position-1], common_lang_constants::LOWER_D) &&
                    !traits::case_insensitive_ex::eq(word[position+2], common_lang_constants::LOWER_R) )
                    {
                    // watch out for deify.  "deify" splits, but some other forms don't
                    if (traits::case_insensitive_ex::eq(word[position+2], common_lang_constants::LOWER_F))
                        {
                        if (traits::case_insensitive_ex::eq(word[position+3], common_lang_constants::LOWER_Y) ||
                            (position+4 < m_length &&
                            traits::case_insensitive_ex::eq(word[position+3], common_lang_constants::LOWER_I) &&
                            traits::case_insensitive_ex::eq(word[position+4], common_lang_constants::LOWER_C)) )
                            { return true; }
                        else
                                { return false; }
                        }
                    else
                        { return true; }
                    }
                else
                    { return false; }
                }
            else
                { return false; }
            }
        else if ((position+1 < m_length) &&
            traits::case_insensitive_ex::eq(word[position], common_lang_constants::LOWER_E) &&
            traits::case_insensitive_ex::eq(word[position+1], common_lang_constants::LOWER_A))
            {
            // EA[letter][letter]...
            if (position == 0)
                {
                // eaten (only a consonant in front can cause a split)
                return false;
                }
            // i-de-a, i-de-al
            else if (position == 2 &&
                traits::case_insensitive_ex::eq(word[0], common_lang_constants::LOWER_I) &&
                traits::case_insensitive_ex::eq(word[1], common_lang_constants::LOWER_D))
                { return true; }
            // pre-arr-ange, pre-am-ble
            else if (position == 2 &&
                m_length >= 6 &&
                traits::case_insensitive_ex::eq(word[0], common_lang_constants::LOWER_P) &&
                traits::case_insensitive_ex::eq(word[1], common_lang_constants::LOWER_R) &&
                (traits::case_insensitive_ex::eq(word[4], common_lang_constants::LOWER_R) ||
                    traits::case_insensitive_ex::eq(word[4], common_lang_constants::LOWER_M) ||
                    traits::case_insensitive_ex::eq(word[4], common_lang_constants::LOWER_G)) )
                { return true; }
            // EA at end of word
            else if (position+2 == m_length)
                {
                // plea
                if (m_syllable_count == 0)
                    {
                    return false;
                    }
                // achillea, nausea
                else
                    {
                    return true;
                    }
                }
            // EA[letter]...
            else if (position+3 == m_length ||
                    (position+4 == m_length &&
                        traits::case_insensitive_ex::eq(word[position+3], common_lang_constants::LOWER_S)) )
                {
                // EAN
                if (traits::case_insensitive_ex::eq(word[position+2], common_lang_constants::LOWER_N) )
                    {
                    // If "ea" is the first vowels then it is one sound
                    // bean
                    if (m_previous_block_vowel == m_length)
                        {
                        return false;
                        }
                    // boolean
                    else if (position >= 3 &&
                        traits::case_insensitive_ex::eq(word[position-3], common_lang_constants::LOWER_O) &&
                        traits::case_insensitive_ex::eq(word[position-2], common_lang_constants::LOWER_O) &&
                        traits::case_insensitive_ex::eq(word[position-1], common_lang_constants::LOWER_L))
                        {
                        return true;
                        }
                    // aegean
                    else if (position >= 1 &&
                        traits::case_insensitive_ex::eq(word[position-1], common_lang_constants::LOWER_G))
                        {
                        return true;
                        }
                    // ocean
                    else if (m_syllable_count == 1 &&
                        (m_previous_block_vowel == position - 2 ||
                        can_consonants_begin_sound(word+(m_previous_block_vowel + 1),
                            position-(m_previous_block_vowel + 1))) )
                        {
                        return false;
                        }
                    // demean, crustacean
                    else if (position >= 1 &&
                        (traits::case_insensitive_ex::eq(word[position-1], common_lang_constants::LOWER_M) ||
                            traits::case_insensitive_ex::eq(word[position-1], common_lang_constants::LOWER_C)) )
                        { return false; }
                    // soybean
                    else if (position >= 1 &&
                        traits::case_insensitive_ex::eq(word[position-1], common_lang_constants::LOWER_B) )
                        {
                        // exception: carribean
                        if (position >= 2 &&
                            traits::case_insensitive_ex::eq(word[position-2], common_lang_constants::LOWER_B) )
                            { return true; }
                        return false;
                        }
                    // cyclopean
                    else if (position >= 1 &&
                        traits::case_insensitive_ex::eq(word[position-1], common_lang_constants::LOWER_P))
                        {
                        return true;
                        }
                    // korean
                    else if (position >= 1 &&
                        traits::case_insensitive_ex::eq(word[position-1], common_lang_constants::LOWER_R))
                        {
                        return true;
                        }
                    // european
                    else
                        {
                        return true;
                        }
                    }
                // surreal, cereal, real
                else if ( (position > 1) &&
                    traits::case_insensitive_ex::eq(word[position-1], common_lang_constants::LOWER_R) &&
                    traits::case_insensitive_ex::eq(word[position+2], common_lang_constants::LOWER_L) )
                    {
                    // exception: unreal
                    if (position == 3 &&
                        traits::case_insensitive_ex::eq(word[position-3], common_lang_constants::LOWER_U) &&
                        traits::case_insensitive_ex::eq(word[position-2], common_lang_constants::LOWER_N) )
                        { return false; }
                    return true;
                    }
                }
            else if (position+3 < m_length)
                {
                // react
                if (traits::case_insensitive_ex::eq(word[position+2], common_lang_constants::LOWER_C) &&
                    traits::case_insensitive_ex::eq(word[position+3], common_lang_constants::LOWER_T) )
                    {
                    return true;
                    }
                // miscreant
                else if (position >= 2 &&
                    traits::case_insensitive_ex::eq(word[position-2], common_lang_constants::LOWER_C) &&
                    traits::case_insensitive_ex::eq(word[position-1], common_lang_constants::LOWER_R) &&
                    traits::case_insensitive_ex::eq(word[position+2], common_lang_constants::LOWER_N) &&
                    traits::case_insensitive_ex::eq(word[position+3], common_lang_constants::LOWER_T) )
                    {
                    return true;
                    }
                // MEAB
                else if (position > 0 &&
                    traits::case_insensitive_ex::eq(word[position-1], common_lang_constants::LOWER_M) &&
                    traits::case_insensitive_ex::eq(word[position+2], common_lang_constants::LOWER_B) )
                    { return true; }
                // re-al-i-ty
                else if ((position == 1) && traits::case_insensitive_ex::eq(word[0], common_lang_constants::LOWER_R))
                    {
                    if (traits::case_insensitive_ex::eq(word[position+2], common_lang_constants::LOWER_L))
                        {
                        // always splits (except for "realm")
                        return !(traits::case_insensitive_ex::eq(word[position+3], common_lang_constants::LOWER_M));
                        }
                    // reappointment
                    else if (traits::case_insensitive_ex::eq(word[position+2], common_lang_constants::LOWER_P) &&
                            traits::case_insensitive_ex::eq(word[position+3], common_lang_constants::LOWER_P))
                        { return true; }
                        // rearranged
                    else if (traits::case_insensitive_ex::eq(word[position+2], common_lang_constants::LOWER_R) &&
                            traits::case_insensitive_ex::eq(word[position+3], common_lang_constants::LOWER_R))
                        { return true; }
                    else
                        { return false; }
                    }
                // ne-ther-realms
                else if (position > 1 &&
                    traits::case_insensitive_ex::eq(word[position+3], common_lang_constants::LOWER_M) )
                    { return false; }
                // mile-age
                else if (traits::case_insensitive_ex::eq(word[position+2], common_lang_constants::LOWER_G) &&
                    traits::case_insensitive_ex::eq(word[position+3], common_lang_constants::LOWER_E) )
                    {
                    // except for "eag-er"
                    if (position+4 < m_length &&
                        traits::case_insensitive_ex::eq(word[position+4], common_lang_constants::LOWER_R))
                        { return false; }
                    return true;
                    }
                else if (traits::case_insensitive_ex::eq(word[position+2], common_lang_constants::LOWER_T) )
                    {
                    // creator
                    if (position > 1 &&
                        traits::case_insensitive_ex::eq(word[position-2], common_lang_constants::LOWER_C) &&
                        traits::case_insensitive_ex::eq(word[position-1], common_lang_constants::LOWER_R))
                        {
                        //"creature" special case
                        if (position+3 < m_length &&
                            traits::case_insensitive_ex::eq(word[position+3], common_lang_constants::LOWER_U))
                            { return false; }
                        else
                            { return true; }
                        }
                    // permeate
                    else if (position >= 2 &&
                        traits::case_insensitive_ex::eq(word[position-2], common_lang_constants::LOWER_R) &&
                        traits::case_insensitive_ex::eq(word[position-1], common_lang_constants::LOWER_M) &&
                        position+3 < m_length &&
                        traits::case_insensitive_ex::eq(word[position+2], common_lang_constants::LOWER_T) &&
                        traits::case_insensitive_ex::eq(word[position+3], common_lang_constants::LOWER_E))
                        { return true; }
                    // theatrical
                    else if (position+4 < m_length &&
                        traits::case_insensitive_ex::eq(word[position+3], common_lang_constants::LOWER_R) &&
                        traits::case_insensitive_ex::eq(word[position+4], common_lang_constants::LOWER_I))
                        { return true; }
                    // eaten
                    else
                        { return false; }
                    }
                else if (traits::case_insensitive_ex::eq(word[position+2], common_lang_constants::LOWER_N) )
                    {
                    // oleander, leann
                    if (traits::case_insensitive_ex::eq(word[position+3], common_lang_constants::LOWER_D) ||
                        traits::case_insensitive_ex::eq(word[position+3], common_lang_constants::LOWER_N))
                        {
                        return true;
                        }
                    else
                        {
                        return false;
                        }
                    }
                // surreal, cereal, realize
                else if ( (position > 1) &&
                    traits::case_insensitive_ex::eq(word[position-1], common_lang_constants::LOWER_R) &&
                    traits::case_insensitive_ex::eq(word[position+2], common_lang_constants::LOWER_L) )
                    {
                    return true;
                    }
                }
            else
                {
                return false;
                }
            }
        else if ((position+1 < m_length) &&
            traits::case_insensitive_ex::eq(word[position], common_lang_constants::LOWER_I) &&
            traits::case_insensitive_ex::eq(word[position+1], common_lang_constants::LOWER_U))
            {
            return true;
            }
        // EO
        else if ((position+1 < m_length) &&
            traits::case_insensitive_ex::eq(word[position], common_lang_constants::LOWER_E) &&
            traits::case_insensitive_ex::eq(word[position+1], common_lang_constants::LOWER_O))
            {
            // foreordain
            if (position >= 3 &&
                traits::case_insensitive_ex::eq(word[position-3], common_lang_constants::LOWER_F) &&
                traits::case_insensitive_ex::eq(word[position-2], common_lang_constants::LOWER_O) &&
                traits::case_insensitive_ex::eq(word[position-1], common_lang_constants::LOWER_R))
                { return false; }
            // george
            else if (position == 1 &&
                position+3 < m_length &&
                traits::case_insensitive_ex::eq(word[position-1], common_lang_constants::LOWER_G) &&
                traits::case_insensitive_ex::eq(word[position+2], common_lang_constants::LOWER_R) &&
                traits::case_insensitive_ex::eq(word[position+3], common_lang_constants::LOWER_G))
                { return false; }
            // geoff
            else if (position == 1 &&
                position+3 < m_length &&
                traits::case_insensitive_ex::eq(word[position-1], common_lang_constants::LOWER_G) &&
                traits::case_insensitive_ex::eq(word[position+2], common_lang_constants::LOWER_F) &&
                traits::case_insensitive_ex::eq(word[position+3], common_lang_constants::LOWER_F))
                { return false; }
            // where-on
            else if (position >= 3 &&
                traits::case_insensitive_ex::eq(word[position-3], common_lang_constants::LOWER_H) &&
                traits::case_insensitive_ex::eq(word[position-2], common_lang_constants::LOWER_E) &&
                traits::case_insensitive_ex::eq(word[position-1], common_lang_constants::LOWER_R))
                { return false; }
            // bludgeon
            else if (position >= 1 &&
                    position+2 < m_length &&
                    (traits::case_insensitive_ex::eq(word[position-1], common_lang_constants::LOWER_G) &&
                    traits::case_insensitive_ex::eq(word[position+2], common_lang_constants::LOWER_N) ))
                { return false; }
            // PEO and JEO
            else if (position >= 1 &&
                    (traits::case_insensitive_ex::eq(word[position-1], common_lang_constants::LOWER_P) ||
                    traits::case_insensitive_ex::eq(word[position-1], common_lang_constants::LOWER_J)))
                {
                // people and jeopardy will not split on the "eo"
                if (position+2 < m_length &&
                    traits::case_insensitive_ex::eq(word[position+2], common_lang_constants::LOWER_P))
                    { return false; }
                // everything else (e.g., "peon" and "Peoria" will split)
                else
                    { return true; }
                }
            // someone
            else if (position == 3 &&
                    traits::case_insensitive_ex::eq(word[0], common_lang_constants::LOWER_S) &&
                    traits::case_insensitive_ex::eq(word[1], common_lang_constants::LOWER_O) &&
                    traits::case_insensitive_ex::eq(word[2], common_lang_constants::LOWER_M))
                { return false; }
            // geometric, neon
            else
                { return true; }
            }
        // UI
        else if ((position+1 < m_length) &&
            traits::case_insensitive_ex::eq(word[position], common_lang_constants::LOWER_U) &&
            traits::case_insensitive_ex::eq(word[position+1], common_lang_constants::LOWER_I))
            {
            if ((position+3) <= m_length &&
                traits::case_insensitive_ex::eq(word[position+2], common_lang_constants::LOWER_T) )
                {
                // fruit(s)
                if (position + 3 == m_length ||
                    (position + 4 == m_length &&
                     traits::case_insensitive_ex::eq(word[position+3], common_lang_constants::LOWER_S)) )
                    { return false; }
                // acuity
                else if (traits::case_insensitive_ex::eq(word[position+3], common_lang_constants::LOWER_Y) )
                    {
                    // except "fruity"
                    if (position == 2 &&
                        traits::case_insensitive_ex::eq(word[0], common_lang_constants::LOWER_F) &&
                        traits::case_insensitive_ex::eq(word[1], common_lang_constants::LOWER_R))
                        { return false; }
                    return true;
                    }
                // uities
                else if (position+6 == m_length &&
                    traits::case_insensitive_ex::eq(word[position+2], common_lang_constants::LOWER_T) &&
                    traits::case_insensitive_ex::eq(word[position+3], common_lang_constants::LOWER_I) &&
                    traits::case_insensitive_ex::eq(word[position+4], common_lang_constants::LOWER_E) &&
                    traits::case_insensitive_ex::eq(word[position+5], common_lang_constants::LOWER_S) )
                    {
                    return true;
                    }
                // intuitive
                else if (position+5 < m_length &&
                    traits::case_insensitive_ex::eq(word[position+3], common_lang_constants::LOWER_I) &&
                    traits::case_insensitive_ex::eq(word[position+4], common_lang_constants::LOWER_V) &&
                    traits::case_insensitive_ex::eq(word[position+5], common_lang_constants::LOWER_E) )
                    {
                    return true;
                    }
                // intuition
                else if (position+5 < m_length &&
                    traits::case_insensitive_ex::eq(word[position+3], common_lang_constants::LOWER_I) &&
                    traits::case_insensitive_ex::eq(word[position+4], common_lang_constants::LOWER_O) &&
                    traits::case_insensitive_ex::eq(word[position+5], common_lang_constants::LOWER_N) )
                    {
                    return true;
                    }
                // suite, suited
                else
                    {
                    return false;
                    }
                }
            // guide, fluid, ruin
            else if ((traits::case_insensitive_ex::eq(word[position+2], common_lang_constants::LOWER_D) ||
                traits::case_insensitive_ex::eq(word[position+2], common_lang_constants::LOWER_N) )&&
                !is_silent_u(word, position) )
                {
                return true;
                }
            // bluish
            else if ((position+4) <= m_length &&
                traits::case_insensitive_ex::eq(word[position+2], common_lang_constants::LOWER_S) &&
                traits::case_insensitive_ex::eq(word[position+3], common_lang_constants::LOWER_H) )
                {
                return true;
                }
            else
                {
                return false;
                }
            }
        // UO
        else if ((position+1 < m_length) &&
            traits::case_insensitive_ex::eq(word[position], common_lang_constants::LOWER_U) &&
            traits::case_insensitive_ex::eq(word[position+1], common_lang_constants::LOWER_O))
            {
            // fluoridate
            if (position+2 < m_length &&
                traits::case_insensitive_ex::eq(word[position+2], common_lang_constants::LOWER_R) )
                {
                return false;
                }
            // duo
            else
                {
                return true;
                }
            }
        // OO
        else if ((position+1 < m_length) &&
            traits::case_insensitive_ex::eq(word[position], common_lang_constants::LOWER_O) &&
            traits::case_insensitive_ex::eq(word[position+1], common_lang_constants::LOWER_O))
            {
            // zoologist
            if (m_length >= 4 &&
                position == 1 &&
                traits::case_insensitive_ex::eq(word[0], common_lang_constants::LOWER_Z) &&
                (traits::case_insensitive_ex::eq(word[position+2], common_lang_constants::LOWER_G) ||
                traits::case_insensitive_ex::eq(word[position+2], common_lang_constants::LOWER_L) ||
                traits::case_insensitive_ex::eq(word[position+2], common_lang_constants::LOWER_P) ) &&
                next_vowel != m_length)
                {
                return true;
                }
            // coordinate, coop
            else if (m_length > 6 &&
                position == 1 &&
                traits::case_insensitive_ex::eq(word[0], common_lang_constants::LOWER_C) &&
                (traits::case_insensitive_ex::eq(word[position+2], common_lang_constants::LOWER_R) ||
                traits::case_insensitive_ex::eq(word[position+2], common_lang_constants::LOWER_P) ||
                traits::case_insensitive_ex::eq(word[position+2], common_lang_constants::LOWER_W) ) &&
                next_vowel != m_length)
                {
                return true;
                }
            // coowner
            else if (m_length >= 4 &&
                position == 1 &&
                traits::case_insensitive_ex::eq(word[0], common_lang_constants::LOWER_C) &&
                traits::case_insensitive_ex::eq(word[position+2], common_lang_constants::LOWER_W) &&
                next_vowel != m_length)
                {
                return true;
                }
            else
                {
                return false;
                }
            }
        /// OE
        else if ((position+1 < m_length) &&
            traits::case_insensitive_ex::eq(word[position], common_lang_constants::LOWER_O) &&
            traits::case_insensitive_ex::eq(word[position+1], common_lang_constants::LOWER_E))
            {
            // toe-nail, woe-ful
            if (position == 1 &&
                (traits::case_insensitive_ex::eq(word[position-1], common_lang_constants::LOWER_T) ||
                traits::case_insensitive_ex::eq(word[position-1], common_lang_constants::LOWER_W)) )
                {
                return false;
                }
            // tomatoe, doe
            else if (position+2 == m_length)
                {
                return false;
                }
            // does, hoed
            else if (position+3 == m_length &&
                (traits::case_insensitive_ex::eq(word[position+2], common_lang_constants::LOWER_S) ||
                    traits::case_insensitive_ex::eq(word[position+2], common_lang_constants::LOWER_D)) )
                {
                return false;
                }
            // doesn't
            else if (m_ends_with_nt_contraction &&
                m_length == 7 &&
                position == 1 &&
                traits::case_insensitive_ex::eq(word[position+2], common_lang_constants::LOWER_S))
                { return false; }
            // phoenix
            else if (position >= 2 &&
                (traits::case_insensitive_ex::eq(word[position-2], common_lang_constants::LOWER_P) ||
                    traits::case_insensitive_ex::eq(word[position-1], common_lang_constants::LOWER_H)) )
                {
                return false;
                }
            // coerce
            else
                {
                return true;
                }
            }
        /// IE
        else if ((position+1 < m_length) &&
            traits::case_insensitive_ex::eq(word[position], common_lang_constants::LOWER_I) &&
            traits::case_insensitive_ex::eq(word[position+1], common_lang_constants::LOWER_E))
            {
            // hottie, tie
            if (position+2 == m_length)
                {
                return false;
                }
            // hy-giene
            else if (position > 0 && position+2 < m_length &&
                traits::case_insensitive_ex::eq(word[position-1], common_lang_constants::LOWER_G) &&
                traits::case_insensitive_ex::eq(word[position+2], common_lang_constants::LOWER_N))
                { return false; }
            // acqui-esce, "quies" at the end won't split
            else if (position >= 2 && position+3 < m_length &&
                traits::case_insensitive_ex::eq(word[position-2], common_lang_constants::LOWER_Q) &&
                traits::case_insensitive_ex::eq(word[position+2], common_lang_constants::LOWER_S))
                { return true; }
            // fiend
            else if ((position+3 < m_length) &&
                traits::case_insensitive_ex::eq(word[position+2], common_lang_constants::LOWER_N) &&
                traits::case_insensitive_ex::eq(word[position+3], common_lang_constants::LOWER_D))
                {
                return false;
                }
            // happiest, driest
            else if ((position+3 < m_length) &&
                traits::case_insensitive_ex::eq(word[position+2], common_lang_constants::LOWER_S) &&
                traits::case_insensitive_ex::eq(word[position+3], common_lang_constants::LOWER_T))
                {
                // special case for "priest"
                if ((position == 2) &&
                    traits::case_insensitive_ex::eq(word[0], common_lang_constants::LOWER_P) &&
                    traits::case_insensitive_ex::eq(word[1], common_lang_constants::LOWER_R))
                    { return false; }
                return true;
                }
            // soviet
            else if ( (position+2 < m_length) &&
                traits::case_insensitive_ex::eq(word[position+2], common_lang_constants::LOWER_T) )
                {
                return true;
                }
            // IEG
            else if ( (position+2 < m_length) &&
                traits::case_insensitive_ex::eq(word[position+2], common_lang_constants::LOWER_G) )
                {
                // blitzkrieg
                if (position+3 == m_length)
                    {
                    return false;
                    }
                // Diego
                else if (traits::case_insensitive_ex::eq(word[position+3], common_lang_constants::LOWER_O))
                    {
                    return true;
                    }
                // siege
                else
                    {
                    return false;
                    }
                }
            // IEN
            else if ( (position+2 < m_length) &&
                traits::case_insensitive_ex::eq(word[position+2], common_lang_constants::LOWER_N) )
                {
                //"science", "client"
                if (is_first_vowel_block_in_word && position > 1)
                    { return true; }
                // watch out for "cien"
                if (position > 0 &&
                    traits::case_insensitive_ex::eq(word[position-1], common_lang_constants::LOWER_C) )
                    {
                    // sci-en[tc] words should split
                    if (position > 1 &&
                        traits::case_insensitive_ex::eq(word[position-2], common_lang_constants::LOWER_S) &&
                        (position+3 < m_length) &&
                        (traits::case_insensitive_ex::eq(word[position+3], common_lang_constants::LOWER_T) ||
                        traits::case_insensitive_ex::eq(word[position+3], common_lang_constants::LOWER_C)) )
                        {
                        // exception for "ne-scient"
                        if (position == 4 &&
                            traits::case_insensitive_ex::eq(word[0], common_lang_constants::LOWER_N) &&
                            traits::case_insensitive_ex::eq(word[1], common_lang_constants::LOWER_E) )
                            { return false; }
                        // exception for "pre-scient"
                        else if (position == 5 &&
                            traits::case_insensitive_ex::eq(word[0], common_lang_constants::LOWER_P) &&
                            traits::case_insensitive_ex::eq(word[1], common_lang_constants::LOWER_R) &&
                            traits::case_insensitive_ex::eq(word[2], common_lang_constants::LOWER_E) )
                            { return false; }
                        // exception for "om-ni-scient"
                        else if (position == 6 &&
                            traits::case_insensitive_ex::eq(word[0], common_lang_constants::LOWER_O) &&
                            traits::case_insensitive_ex::eq(word[1], common_lang_constants::LOWER_M) &&
                            traits::case_insensitive_ex::eq(word[2], common_lang_constants::LOWER_N) &&
                            traits::case_insensitive_ex::eq(word[3], common_lang_constants::LOWER_I) )
                            { return false; }
                        return true;
                        }
                    return false;
                    }
                // IENCE
                else if (position > 0 &&
                    position+5 <= m_length &&
                    traits::case_insensitive_ex::eq(word[position+3], common_lang_constants::LOWER_C) &&
                    (traits::case_insensitive_ex::eq(word[position+4], common_lang_constants::LOWER_E) ||
                    traits::case_insensitive_ex::eq(word[position+4], common_lang_constants::LOWER_Y) ||
                    traits::case_insensitive_ex::eq(word[position+4], common_lang_constants::LOWER_I)) )
                    {
                    // nu-tri-ence
                    if (position > 1 &&
                        traits::case_insensitive_ex::eq(word[position-2], common_lang_constants::LOWER_T) &&
                        traits::case_insensitive_ex::eq(word[position-1], common_lang_constants::LOWER_R) )
                        { return true; }
                    // "[ltn]ienc[ye]" will be one sound.  "pat-ience", "conv-en-ience", "sent-ience", "e-bull-ience"
                    else if (traits::case_insensitive_ex::eq(word[position-1], common_lang_constants::LOWER_L) ||
                        traits::case_insensitive_ex::eq(word[position-1], common_lang_constants::LOWER_T) ||
                        traits::case_insensitive_ex::eq(word[position-1], common_lang_constants::LOWER_N))
                        {
                        // special exception for "len-i-ence"
                        if (position == 3 &&
                            traits::case_insensitive_ex::eq(word[0], common_lang_constants::LOWER_L) &&
                            traits::case_insensitive_ex::eq(word[1], common_lang_constants::LOWER_E))
                            { return true; }
                        // special exception for "sal-i-ence"
                        else if (position == 3 &&
                            traits::case_insensitive_ex::eq(word[0], common_lang_constants::LOWER_S) &&
                            traits::case_insensitive_ex::eq(word[1], common_lang_constants::LOWER_A) &&
                            traits::case_insensitive_ex::eq(word[2], common_lang_constants::LOWER_L))
                            { return true; }
                        return false;
                        }
                    else
                        { return true; }
                    }
                // "[ltn]ient" will be one sound.  "pat-ient", "conv-en-ient", "sent-ient"
                else if (position > 0 &&
                    position + 4 <= m_length &&
                    traits::case_insensitive_ex::eq(word[position+3], common_lang_constants::LOWER_T) &&
                    (traits::case_insensitive_ex::eq(word[position-1], common_lang_constants::LOWER_L) ||
                    traits::case_insensitive_ex::eq(word[position-1], common_lang_constants::LOWER_T) ||
                    traits::case_insensitive_ex::eq(word[position-1], common_lang_constants::LOWER_N)) )
                    {
                    // special exception for "len-i-ent"
                    if (position == 3 &&
                        traits::case_insensitive_ex::eq(word[0], common_lang_constants::LOWER_L) &&
                        traits::case_insensitive_ex::eq(word[1], common_lang_constants::LOWER_E))
                        { return true; }
                    // special exception for "sal-i-ent"
                    else if (position == 3 &&
                            traits::case_insensitive_ex::eq(word[0], common_lang_constants::LOWER_S) &&
                            traits::case_insensitive_ex::eq(word[1], common_lang_constants::LOWER_A) &&
                            traits::case_insensitive_ex::eq(word[2], common_lang_constants::LOWER_L))
                            { return true; }
                    else return false;
                    }
                // a-per-i-ent, nu-tri-ent
                else if (position > 0 &&
                    traits::case_insensitive_ex::eq(word[position-1], common_lang_constants::LOWER_R) )
                    {
                    // ...except for "friend"
                    if (position + 3 <
                        m_length && traits::case_insensitive_ex::eq(word[position-1], common_lang_constants::LOWER_D))
                        { return false; }
                    else return true;
                    }
                // [?]lien
                else if (position > 1 &&
                    traits::case_insensitive_ex::eq(word[position-1], common_lang_constants::LOWER_L) )
                    {
                    // a-li-en, cli-ent
                    return true;
                    }
                // lien
                else if (position == 1 &&
                    traits::case_insensitive_ex::eq(word[position-1], common_lang_constants::LOWER_L) )
                    { return false; }
                return true;
                }
            // IER
            else if ( (position+2 < m_length) &&
                traits::case_insensitive_ex::eq(word[position+2], common_lang_constants::LOWER_R) )
                {
                // bombardier, frontier, and soldier are special cases
                if (position == 4 &&
                    traits::case_insensitive_ex::eq(word[0], common_lang_constants::LOWER_S) &&
                    traits::case_insensitive_ex::eq(word[1], common_lang_constants::LOWER_O) &&
                    traits::case_insensitive_ex::eq(word[2], common_lang_constants::LOWER_L) &&
                    traits::case_insensitive_ex::eq(word[3], common_lang_constants::LOWER_D))
                    { return false; }
                if (position == 4 &&
                    traits::case_insensitive_ex::eq(word[0], common_lang_constants::LOWER_G) &&
                    traits::case_insensitive_ex::eq(word[1], common_lang_constants::LOWER_L) &&
                    traits::case_insensitive_ex::eq(word[2], common_lang_constants::LOWER_A) &&
                    traits::case_insensitive_ex::eq(word[3], common_lang_constants::LOWER_C))
                    { return false; }
                else if (position == 5 &&
                    traits::case_insensitive_ex::eq(word[0], common_lang_constants::LOWER_F) &&
                    traits::case_insensitive_ex::eq(word[1], common_lang_constants::LOWER_R) &&
                    traits::case_insensitive_ex::eq(word[2], common_lang_constants::LOWER_O) &&
                    traits::case_insensitive_ex::eq(word[3], common_lang_constants::LOWER_N) &&
                    traits::case_insensitive_ex::eq(word[4], common_lang_constants::LOWER_T))
                    { return false; }
                else if (position == 7 &&
                        traits::case_insensitive_ex::eq(word[0], common_lang_constants::LOWER_B) &&
                        traits::case_insensitive_ex::eq(word[1], common_lang_constants::LOWER_O) &&
                        traits::case_insensitive_ex::eq(word[2], common_lang_constants::LOWER_M) &&
                        traits::case_insensitive_ex::eq(word[3], common_lang_constants::LOWER_B) &&
                        traits::case_insensitive_ex::eq(word[4], common_lang_constants::LOWER_A) &&
                        traits::case_insensitive_ex::eq(word[5], common_lang_constants::LOWER_R) &&
                        traits::case_insensitive_ex::eq(word[6], common_lang_constants::LOWER_D))
                    { return false; }

                if (position > 1 /*"pier" won't split*/ && (position+3 == m_length))
                    {
                    //"ier" may be replacing a lower 'y' at the end of a word                    
                    return true;
                    }
                // out-li-ers
                else if (position > 1 && //"piers" won't split
                    position+4 == m_length &&
                    traits::case_insensitive_ex::eq(word[position+3], common_lang_constants::LOWER_S))
                    { return true; }
                //"ier" is inside the word
                else if (isChar.is_vowel(word[position+3]) )
                    {
                    // antierosion
                    return true;
                    }
                else if (next_vowel != m_length)
                    {
                    // apierce
                    if (can_consonants_end_sound(word+position+2, (next_vowel-1)-(position+1)) )
                        { return false; }
                    else
                        { return true; }
                    }
                else
                    {
                    // rest of word is consonants, so this is one sound at the end
                    return false;
                    }
                }
            else return false;
            }
        // UA
        else if ((position+1) < m_length &&
            traits::case_insensitive_ex::eq(word[position], common_lang_constants::LOWER_U) &&
            traits::case_insensitive_ex::eq(word[position+1], common_lang_constants::LOWER_A))
            {
            // persuade
            if ( (position+2) < m_length &&
                traits::case_insensitive_ex::eq(word[position+2], common_lang_constants::LOWER_D) )
                { return false; }
            // persuasion, suave
            else if (position > 0 &&
                traits::case_insensitive_ex::eq(word[position-1], common_lang_constants::LOWER_S) &&
                (position+2) < m_length &&
                (traits::case_insensitive_ex::eq(word[position+2], common_lang_constants::LOWER_S) ||
                    traits::case_insensitive_ex::eq(word[position+2], common_lang_constants::LOWER_V) ) )
                { return false; }
            // dual, manual
            return true;
            }
        // UU
        else if ((position+1) < m_length &&
            traits::case_insensitive_ex::eq(word[position], common_lang_constants::LOWER_U) &&
            traits::case_insensitive_ex::eq(word[position+1], common_lang_constants::LOWER_U))
            {
            // muumuu seems to be the only special case where it does not split
            if (position > 0 &&
                traits::case_insensitive_ex::eq(word[position-1], common_lang_constants::LOWER_M))
                {
                return false;
                }
            // everything else, such as vacuum, duumvir
            return true;
            }
        // embryo
        else if (position > 0 &&
                traits::case_insensitive_ex::eq(word[position], common_lang_constants::LOWER_Y) &&
                !isChar.is_vowel(word[position-1]) )
            {
            return true;
            }
        else
            {
            return false;
            }
        return false;
        }

    bool english_syllabize::can_consonants_end_sound(const wchar_t* consonants, size_t block_length)
        {
        assert(consonants);
        if (consonants == nullptr)
            { return false; }
        if (block_length > 3)
            { return false; }
        else if (block_length == 3)
            {
            return (traits::case_insensitive_ex::eq(consonants[0], common_lang_constants::LOWER_C) &&
                    traits::case_insensitive_ex::eq(consonants[1], common_lang_constants::LOWER_H) &&
                    traits::case_insensitive_ex::eq(consonants[2], common_lang_constants::LOWER_R)) ?
                true : (traits::case_insensitive_ex::eq(consonants[0], common_lang_constants::LOWER_M) &&
                    traits::case_insensitive_ex::eq(consonants[1], common_lang_constants::LOWER_P) &&
                    traits::case_insensitive_ex::eq(consonants[2], common_lang_constants::LOWER_T)) ?
                true : (traits::case_insensitive_ex::eq(consonants[0], common_lang_constants::LOWER_N) &&
                    traits::case_insensitive_ex::eq(consonants[1], common_lang_constants::LOWER_T) &&
                    traits::case_insensitive_ex::eq(consonants[2], common_lang_constants::LOWER_H)) ?
                true : (traits::case_insensitive_ex::eq(consonants[0], common_lang_constants::LOWER_N) &&
                    traits::case_insensitive_ex::eq(consonants[1], common_lang_constants::LOWER_C) &&
                    traits::case_insensitive_ex::eq(consonants[2], common_lang_constants::LOWER_H)) ?
                true : (traits::case_insensitive_ex::eq(consonants[0], common_lang_constants::LOWER_N) &&
                    traits::case_insensitive_ex::eq(consonants[1], common_lang_constants::LOWER_G) &&
                    traits::case_insensitive_ex::eq(consonants[2], common_lang_constants::LOWER_U)) ?
                true : (traits::case_insensitive_ex::eq(consonants[0], common_lang_constants::LOWER_N) &&
                    traits::case_insensitive_ex::eq(consonants[1], common_lang_constants::LOWER_Q) &&
                    traits::case_insensitive_ex::eq(consonants[2], common_lang_constants::LOWER_U)) ?
                true : (traits::case_insensitive_ex::eq(consonants[0], common_lang_constants::LOWER_R) &&
                    traits::case_insensitive_ex::eq(consonants[1], common_lang_constants::LOWER_S) &&
                    traits::case_insensitive_ex::eq(consonants[2], common_lang_constants::LOWER_T)) ?
                true : (traits::case_insensitive_ex::eq(consonants[0], common_lang_constants::LOWER_R) &&
                    traits::case_insensitive_ex::eq(consonants[1], common_lang_constants::LOWER_C) &&
                    traits::case_insensitive_ex::eq(consonants[2], common_lang_constants::LOWER_H)) ?
                true : (traits::case_insensitive_ex::eq(consonants[0], common_lang_constants::LOWER_R) &&
                    traits::case_insensitive_ex::eq(consonants[1], common_lang_constants::LOWER_G) &&
                    traits::case_insensitive_ex::eq(consonants[2], common_lang_constants::LOWER_U)) ?
                true : (traits::case_insensitive_ex::eq(consonants[0], common_lang_constants::LOWER_R) &&
                    traits::case_insensitive_ex::eq(consonants[1], common_lang_constants::LOWER_Q) &&
                    traits::case_insensitive_ex::eq(consonants[2], common_lang_constants::LOWER_U)) ?
                true : (traits::case_insensitive_ex::eq(consonants[0], common_lang_constants::LOWER_R) &&
                    traits::case_insensitive_ex::eq(consonants[1], common_lang_constants::LOWER_T) &&
                    traits::case_insensitive_ex::eq(consonants[2], common_lang_constants::LOWER_H)) ?
                true : (traits::case_insensitive_ex::eq(consonants[0], common_lang_constants::LOWER_S) &&
                    traits::case_insensitive_ex::eq(consonants[1], common_lang_constants::LOWER_C) &&
                    traits::case_insensitive_ex::eq(consonants[2], common_lang_constants::LOWER_H)) ?
                true : (traits::case_insensitive_ex::eq(consonants[0], common_lang_constants::LOWER_S) &&
                    traits::case_insensitive_ex::eq(consonants[1], common_lang_constants::LOWER_C) &&
                    traits::case_insensitive_ex::eq(consonants[2], common_lang_constants::LOWER_R)) ?
                true : (traits::case_insensitive_ex::eq(consonants[0], common_lang_constants::LOWER_T) &&
                    traits::case_insensitive_ex::eq(consonants[1], common_lang_constants::LOWER_C) &&
                    traits::case_insensitive_ex::eq(consonants[2], common_lang_constants::LOWER_H)) ?
                true : false;
            }
        else if (block_length == 2)
            {
            return (traits::case_insensitive_ex::eq(consonants[0], common_lang_constants::LOWER_B) &&
                    traits::case_insensitive_ex::eq(consonants[1], common_lang_constants::LOWER_B)) ?
                true : (traits::case_insensitive_ex::eq(consonants[0], common_lang_constants::LOWER_C) &&
                        (traits::case_insensitive_ex::eq(consonants[1], common_lang_constants::LOWER_H) ||
                        traits::case_insensitive_ex::eq(consonants[1], common_lang_constants::LOWER_K) ||
                        traits::case_insensitive_ex::eq(consonants[1], common_lang_constants::LOWER_R) )) ?
                true : (traits::case_insensitive_ex::eq(consonants[0], common_lang_constants::LOWER_D) &&
                        (traits::case_insensitive_ex::eq(consonants[1], common_lang_constants::LOWER_D) ||
                        traits::case_insensitive_ex::eq(consonants[1], common_lang_constants::LOWER_G))) ?
                true : (traits::case_insensitive_ex::eq(consonants[0], common_lang_constants::LOWER_F) &&
                        (traits::case_insensitive_ex::eq(consonants[1], common_lang_constants::LOWER_F) ||
                        traits::case_insensitive_ex::eq(consonants[1], common_lang_constants::LOWER_R))) ?
                true : (traits::case_insensitive_ex::eq(consonants[0], common_lang_constants::LOWER_G) &&
                        (traits::case_insensitive_ex::eq(consonants[1], common_lang_constants::LOWER_G) ||
                        traits::case_insensitive_ex::eq(consonants[1], common_lang_constants::LOWER_H) ||
                        traits::case_insensitive_ex::eq(consonants[1], common_lang_constants::LOWER_R) ||
                        traits::case_insensitive_ex::eq(consonants[1], common_lang_constants::LOWER_U))) ?
                true : (traits::case_insensitive_ex::eq(consonants[0], common_lang_constants::LOWER_K) &&
                        traits::case_insensitive_ex::eq(consonants[1], common_lang_constants::LOWER_K)) ?
                true : (traits::case_insensitive_ex::eq(consonants[0], common_lang_constants::LOWER_L) &&
                        (traits::case_insensitive_ex::eq(consonants[1], common_lang_constants::LOWER_C) ||
                        traits::case_insensitive_ex::eq(consonants[1], common_lang_constants::LOWER_F) ||
                        traits::case_insensitive_ex::eq(consonants[1], common_lang_constants::LOWER_G) ||
                        traits::case_insensitive_ex::eq(consonants[1], common_lang_constants::LOWER_K) ||
                        traits::case_insensitive_ex::eq(consonants[1], common_lang_constants::LOWER_L) ||
                        traits::case_insensitive_ex::eq(consonants[1], common_lang_constants::LOWER_M) ||
                        traits::case_insensitive_ex::eq(consonants[1], common_lang_constants::LOWER_N) ||
                        traits::case_insensitive_ex::eq(consonants[1], common_lang_constants::LOWER_P) ||
                        traits::case_insensitive_ex::eq(consonants[1], common_lang_constants::LOWER_T) ||
                        traits::case_insensitive_ex::eq(consonants[1], common_lang_constants::LOWER_V) )) ?
                true : (traits::case_insensitive_ex::eq(consonants[0], common_lang_constants::LOWER_M) &&
                        (traits::case_insensitive_ex::eq(consonants[1], common_lang_constants::LOWER_B) ||
                        traits::case_insensitive_ex::eq(consonants[1], common_lang_constants::LOWER_M) ||
                        traits::case_insensitive_ex::eq(consonants[1], common_lang_constants::LOWER_P) )) ?
                true : (traits::case_insensitive_ex::eq(consonants[0], common_lang_constants::LOWER_N) &&
                        (traits::case_insensitive_ex::eq(consonants[1], common_lang_constants::LOWER_C) ||
                        traits::case_insensitive_ex::eq(consonants[1], common_lang_constants::LOWER_D) ||
                        traits::case_insensitive_ex::eq(consonants[1], common_lang_constants::LOWER_G) ||
                        traits::case_insensitive_ex::eq(consonants[1], common_lang_constants::LOWER_K) ||
                        traits::case_insensitive_ex::eq(consonants[1], common_lang_constants::LOWER_N) ||
                        traits::case_insensitive_ex::eq(consonants[1], common_lang_constants::LOWER_S) ||
                        traits::case_insensitive_ex::eq(consonants[1], common_lang_constants::LOWER_T) )) ?
                true : (traits::case_insensitive_ex::eq(consonants[0], common_lang_constants::LOWER_P) &&
                        (traits::case_insensitive_ex::eq(consonants[1], common_lang_constants::LOWER_P) ||
                        traits::case_insensitive_ex::eq(consonants[1], common_lang_constants::LOWER_R) ||
                        traits::case_insensitive_ex::eq(consonants[1], common_lang_constants::LOWER_S) )) ?
                true : (traits::case_insensitive_ex::eq(consonants[0], common_lang_constants::LOWER_Q) &&
                        traits::case_insensitive_ex::eq(consonants[1], common_lang_constants::LOWER_U)) ?
                true : (traits::case_insensitive_ex::eq(consonants[0], common_lang_constants::LOWER_R) &&
                        (traits::case_insensitive_ex::eq(consonants[1], common_lang_constants::LOWER_B) ||
                        traits::case_insensitive_ex::eq(consonants[1], common_lang_constants::LOWER_C) ||
                        traits::case_insensitive_ex::eq(consonants[1], common_lang_constants::LOWER_D) ||
                        traits::case_insensitive_ex::eq(consonants[1], common_lang_constants::LOWER_F) ||
                        traits::case_insensitive_ex::eq(consonants[1], common_lang_constants::LOWER_G) ||
                        traits::case_insensitive_ex::eq(consonants[1], common_lang_constants::LOWER_K) ||
                        traits::case_insensitive_ex::eq(consonants[1], common_lang_constants::LOWER_L) ||
                        traits::case_insensitive_ex::eq(consonants[1], common_lang_constants::LOWER_M) ||
                        traits::case_insensitive_ex::eq(consonants[1], common_lang_constants::LOWER_N) ||
                        traits::case_insensitive_ex::eq(consonants[1], common_lang_constants::LOWER_P) ||
                        traits::case_insensitive_ex::eq(consonants[1], common_lang_constants::LOWER_Q) ||
                        traits::case_insensitive_ex::eq(consonants[1], common_lang_constants::LOWER_R) ||
                        traits::case_insensitive_ex::eq(consonants[1], common_lang_constants::LOWER_S) ||
                        traits::case_insensitive_ex::eq(consonants[1], common_lang_constants::LOWER_T) ||
                        traits::case_insensitive_ex::eq(consonants[1], common_lang_constants::LOWER_V) )) ?
                true : (traits::case_insensitive_ex::eq(consonants[0], common_lang_constants::LOWER_S) &&
                        (traits::case_insensitive_ex::eq(consonants[1], common_lang_constants::LOWER_H) ||
                        traits::case_insensitive_ex::eq(consonants[1], common_lang_constants::LOWER_K) ||
                        traits::case_insensitive_ex::eq(consonants[1], common_lang_constants::LOWER_L) ||
                        traits::case_insensitive_ex::eq(consonants[1], common_lang_constants::LOWER_M) ||
                        traits::case_insensitive_ex::eq(consonants[1], common_lang_constants::LOWER_N) ||
                        traits::case_insensitive_ex::eq(consonants[1], common_lang_constants::LOWER_P) ||
                        traits::case_insensitive_ex::eq(consonants[1], common_lang_constants::LOWER_S) ||
                        traits::case_insensitive_ex::eq(consonants[1], common_lang_constants::LOWER_T) )) ?
                true : (traits::case_insensitive_ex::eq(consonants[0], common_lang_constants::LOWER_T) &&
                        (traits::case_insensitive_ex::eq(consonants[1], common_lang_constants::LOWER_H) ||
                            traits::case_insensitive_ex::eq(consonants[1], common_lang_constants::LOWER_T))) ?
                true : (traits::case_insensitive_ex::eq(consonants[0], common_lang_constants::LOWER_W) &&
                        (traits::case_insensitive_ex::eq(consonants[1], common_lang_constants::LOWER_N) ||
                        traits::case_insensitive_ex::eq(consonants[1], common_lang_constants::LOWER_S))) ?
                true : (traits::case_insensitive_ex::eq(consonants[0], common_lang_constants::LOWER_Y) &&
                        traits::case_insensitive_ex::eq(consonants[1], common_lang_constants::LOWER_C)) ?
                true : false;
            }
        else if (block_length == 1)
            {
            return true;
            }
        else
            {
            return false;
            }
        }

    bool english_syllabize::can_consonants_be_modified_by_following_e(const wchar_t* consonants, size_t block_length)
        {
        assert(consonants);
        if (consonants == nullptr)
            { return false; }
        if (block_length > 3)
            {
            return false;
            }
        else if (block_length == 3)
            {
            return (traits::case_insensitive_ex::eq(consonants[0], common_lang_constants::LOWER_C) &&
                    traits::case_insensitive_ex::eq(consonants[1], common_lang_constants::LOWER_H) &&
                    traits::case_insensitive_ex::eq(consonants[2], common_lang_constants::LOWER_R)) ?
                true : (traits::case_insensitive_ex::eq(consonants[0], common_lang_constants::LOWER_L) &&
                        traits::case_insensitive_ex::eq(consonants[1], common_lang_constants::LOWER_C) &&
                        traits::case_insensitive_ex::eq(consonants[2], common_lang_constants::LOWER_H)) ?
                true : (traits::case_insensitive_ex::eq(consonants[0], common_lang_constants::LOWER_L) &&
                        traits::case_insensitive_ex::eq(consonants[1], common_lang_constants::LOWER_S) &&
                        traits::case_insensitive_ex::eq(consonants[2], common_lang_constants::LOWER_H)) ?
                true : (traits::case_insensitive_ex::eq(consonants[0], common_lang_constants::LOWER_L) &&
                        traits::case_insensitive_ex::eq(consonants[1], common_lang_constants::LOWER_T) &&
                        traits::case_insensitive_ex::eq(consonants[2], common_lang_constants::LOWER_Z)) ?
                true : (traits::case_insensitive_ex::eq(consonants[0], common_lang_constants::LOWER_R) &&
                        traits::case_insensitive_ex::eq(consonants[1], common_lang_constants::LOWER_P) &&
                        traits::case_insensitive_ex::eq(consonants[2], common_lang_constants::LOWER_S)) ?
                true : (traits::case_insensitive_ex::eq(consonants[0], common_lang_constants::LOWER_M) &&
                        traits::case_insensitive_ex::eq(consonants[1], common_lang_constants::LOWER_P) &&
                        traits::case_insensitive_ex::eq(consonants[2], common_lang_constants::LOWER_H)) ?
                true : (traits::case_insensitive_ex::eq(consonants[0], common_lang_constants::LOWER_L) &&
                        traits::case_insensitive_ex::eq(consonants[1], common_lang_constants::LOWER_Q) &&
                        traits::case_insensitive_ex::eq(consonants[2], common_lang_constants::LOWER_U)) ?
                true : (traits::case_insensitive_ex::eq(consonants[0], common_lang_constants::LOWER_M) &&
                        traits::case_insensitive_ex::eq(consonants[1], common_lang_constants::LOWER_P) &&
                        traits::case_insensitive_ex::eq(consonants[2], common_lang_constants::LOWER_S)) ?
                true : (traits::case_insensitive_ex::eq(consonants[0], common_lang_constants::LOWER_M) &&
                        traits::case_insensitive_ex::eq(consonants[1], common_lang_constants::LOWER_P) &&
                        traits::case_insensitive_ex::eq(consonants[2], common_lang_constants::LOWER_T)) ?
                true : (traits::case_insensitive_ex::eq(consonants[0], common_lang_constants::LOWER_N) &&
                        traits::case_insensitive_ex::eq(consonants[1], common_lang_constants::LOWER_T) &&
                        traits::case_insensitive_ex::eq(consonants[2], common_lang_constants::LOWER_H)) ?
                true : (traits::case_insensitive_ex::eq(consonants[0], common_lang_constants::LOWER_N) &&
                        traits::case_insensitive_ex::eq(consonants[1], common_lang_constants::LOWER_C) &&
                        traits::case_insensitive_ex::eq(consonants[2], common_lang_constants::LOWER_H)) ?
                true : (traits::case_insensitive_ex::eq(consonants[0], common_lang_constants::LOWER_N) &&
                        traits::case_insensitive_ex::eq(consonants[1], common_lang_constants::LOWER_G) &&
                        traits::case_insensitive_ex::eq(consonants[2], common_lang_constants::LOWER_U)) ?
                true : (traits::case_insensitive_ex::eq(consonants[0], common_lang_constants::LOWER_N) &&
                        traits::case_insensitive_ex::eq(consonants[1], common_lang_constants::LOWER_Q) &&
                        traits::case_insensitive_ex::eq(consonants[2], common_lang_constants::LOWER_U)) ?
                true : (traits::case_insensitive_ex::eq(consonants[0], common_lang_constants::LOWER_P) &&
                        traits::case_insensitive_ex::eq(consonants[1], common_lang_constants::LOWER_H) &&
                        traits::case_insensitive_ex::eq(consonants[2], common_lang_constants::LOWER_R)) ?
                true : (traits::case_insensitive_ex::eq(consonants[0], common_lang_constants::LOWER_R) &&
                        traits::case_insensitive_ex::eq(consonants[1], common_lang_constants::LOWER_S) &&
                        traits::case_insensitive_ex::eq(consonants[2], common_lang_constants::LOWER_T)) ?
                true : (traits::case_insensitive_ex::eq(consonants[0], common_lang_constants::LOWER_R) &&
                        traits::case_insensitive_ex::eq(consonants[1], common_lang_constants::LOWER_C) &&
                        traits::case_insensitive_ex::eq(consonants[2], common_lang_constants::LOWER_H)) ?
                true : (traits::case_insensitive_ex::eq(consonants[0], common_lang_constants::LOWER_R) &&
                        traits::case_insensitive_ex::eq(consonants[1], common_lang_constants::LOWER_G) &&
                        traits::case_insensitive_ex::eq(consonants[2], common_lang_constants::LOWER_U)) ?
                true : (traits::case_insensitive_ex::eq(consonants[0], common_lang_constants::LOWER_R) &&
                        traits::case_insensitive_ex::eq(consonants[1], common_lang_constants::LOWER_Q) &&
                        traits::case_insensitive_ex::eq(consonants[2], common_lang_constants::LOWER_U)) ?
                true : (traits::case_insensitive_ex::eq(consonants[0], common_lang_constants::LOWER_R) &&
                        traits::case_insensitive_ex::eq(consonants[1], common_lang_constants::LOWER_T) &&
                        traits::case_insensitive_ex::eq(consonants[2], common_lang_constants::LOWER_H)) ?
                true : (traits::case_insensitive_ex::eq(consonants[0], common_lang_constants::LOWER_S) &&
                        traits::case_insensitive_ex::eq(consonants[1], common_lang_constants::LOWER_C) &&
                        traits::case_insensitive_ex::eq(consonants[2], common_lang_constants::LOWER_H)) ?
                true : (traits::case_insensitive_ex::eq(consonants[0], common_lang_constants::LOWER_S) &&
                        traits::case_insensitive_ex::eq(consonants[1], common_lang_constants::LOWER_H) &&
                        traits::case_insensitive_ex::eq(consonants[2], common_lang_constants::LOWER_R)) ?
                true : (traits::case_insensitive_ex::eq(consonants[0], common_lang_constants::LOWER_S) &&
                        traits::case_insensitive_ex::eq(consonants[1], common_lang_constants::LOWER_T) &&
                        traits::case_insensitive_ex::eq(consonants[2], common_lang_constants::LOWER_R)) ?
                true : (traits::case_insensitive_ex::eq(consonants[0], common_lang_constants::LOWER_S) &&
                        traits::case_insensitive_ex::eq(consonants[1], common_lang_constants::LOWER_C) &&
                        traits::case_insensitive_ex::eq(consonants[2], common_lang_constants::LOWER_R)) ?
                true : (traits::case_insensitive_ex::eq(consonants[0], common_lang_constants::LOWER_S) &&
                        traits::case_insensitive_ex::eq(consonants[1], common_lang_constants::LOWER_Q) &&
                        traits::case_insensitive_ex::eq(consonants[2], common_lang_constants::LOWER_U)) ?
                true : (traits::case_insensitive_ex::eq(consonants[0], common_lang_constants::LOWER_T) &&
                        traits::case_insensitive_ex::eq(consonants[1], common_lang_constants::LOWER_C) &&
                        traits::case_insensitive_ex::eq(consonants[2], common_lang_constants::LOWER_H)) ?
                true : false;
            }
        else if (block_length == 2)
            {
            return (traits::case_insensitive_ex::eq(consonants[0], common_lang_constants::LOWER_B) &&
                    traits::case_insensitive_ex::eq(consonants[1], common_lang_constants::LOWER_B)) ?
                true : (traits::case_insensitive_ex::eq(consonants[0], common_lang_constants::LOWER_K) &&
                        traits::case_insensitive_ex::eq(consonants[1], common_lang_constants::LOWER_K)) ?
                true : (traits::case_insensitive_ex::eq(consonants[0], common_lang_constants::LOWER_C) &&
                        (traits::case_insensitive_ex::eq(consonants[1], common_lang_constants::LOWER_H) ||
                        traits::case_insensitive_ex::eq(consonants[1], common_lang_constants::LOWER_K) ||
                        traits::case_insensitive_ex::eq(consonants[1], common_lang_constants::LOWER_R))) ?
                true : (traits::case_insensitive_ex::eq(consonants[0], common_lang_constants::LOWER_D) &&
                        (traits::case_insensitive_ex::eq(consonants[1], common_lang_constants::LOWER_D) ||
                        traits::case_insensitive_ex::eq(consonants[1], common_lang_constants::LOWER_G))) ?
                true : (traits::case_insensitive_ex::eq(consonants[0], common_lang_constants::LOWER_F) &&
                        (traits::case_insensitive_ex::eq(consonants[1], common_lang_constants::LOWER_F) ||
                        traits::case_insensitive_ex::eq(consonants[1], common_lang_constants::LOWER_R))) ?
                true : (traits::case_insensitive_ex::eq(consonants[0], common_lang_constants::LOWER_G) &&
                        (traits::case_insensitive_ex::eq(consonants[1], common_lang_constants::LOWER_G) ||
                        traits::case_insensitive_ex::eq(consonants[1], common_lang_constants::LOWER_H) ||
                        traits::case_insensitive_ex::eq(consonants[1], common_lang_constants::LOWER_N) ||
                        traits::case_insensitive_ex::eq(consonants[1], common_lang_constants::LOWER_R) ||
                        traits::case_insensitive_ex::eq(consonants[1], common_lang_constants::LOWER_U))) ?
                true : (traits::case_insensitive_ex::eq(consonants[0], common_lang_constants::LOWER_L) &&
                        (traits::case_insensitive_ex::eq(consonants[1], common_lang_constants::LOWER_C) ||
                            traits::case_insensitive_ex::eq(consonants[1], common_lang_constants::LOWER_F) ||
                            traits::case_insensitive_ex::eq(consonants[1], common_lang_constants::LOWER_G) ||
                            traits::case_insensitive_ex::eq(consonants[1], common_lang_constants::LOWER_K) ||
                            traits::case_insensitive_ex::eq(consonants[1], common_lang_constants::LOWER_L) ||
                            traits::case_insensitive_ex::eq(consonants[1], common_lang_constants::LOWER_M) ||
                            traits::case_insensitive_ex::eq(consonants[1], common_lang_constants::LOWER_N) ||
                            traits::case_insensitive_ex::eq(consonants[1], common_lang_constants::LOWER_P) ||
                            traits::case_insensitive_ex::eq(consonants[1], common_lang_constants::LOWER_S) ||
                            traits::case_insensitive_ex::eq(consonants[1], common_lang_constants::LOWER_T) ||
                            traits::case_insensitive_ex::eq(consonants[1], common_lang_constants::LOWER_V))) ?
                true : (traits::case_insensitive_ex::eq(consonants[0], common_lang_constants::LOWER_M) &&
                        (traits::case_insensitive_ex::eq(consonants[1], common_lang_constants::LOWER_B) ||
                        traits::case_insensitive_ex::eq(consonants[1], common_lang_constants::LOWER_M) ||
                        traits::case_insensitive_ex::eq(consonants[1], common_lang_constants::LOWER_N) ||
                        traits::case_insensitive_ex::eq(consonants[1], common_lang_constants::LOWER_P) )) ?
                true : (traits::case_insensitive_ex::eq(consonants[0], common_lang_constants::LOWER_N) &&
                        (traits::case_insensitive_ex::eq(consonants[1], common_lang_constants::LOWER_C) ||
                        traits::case_insensitive_ex::eq(consonants[1], common_lang_constants::LOWER_D) ||
                        traits::case_insensitive_ex::eq(consonants[1], common_lang_constants::LOWER_G) ||
                        traits::case_insensitive_ex::eq(consonants[1], common_lang_constants::LOWER_K) ||
                        traits::case_insensitive_ex::eq(consonants[1], common_lang_constants::LOWER_N) ||
                        traits::case_insensitive_ex::eq(consonants[1], common_lang_constants::LOWER_S) ||
                        traits::case_insensitive_ex::eq(consonants[1], common_lang_constants::LOWER_Z) )) ?
                true : (traits::case_insensitive_ex::eq(consonants[0], common_lang_constants::LOWER_P) &&
                        (traits::case_insensitive_ex::eq(consonants[1], common_lang_constants::LOWER_P) ||
                        traits::case_insensitive_ex::eq(consonants[1], common_lang_constants::LOWER_R) ||
                        traits::case_insensitive_ex::eq(consonants[1], common_lang_constants::LOWER_S) )) ?
                true : (traits::case_insensitive_ex::eq(consonants[0], common_lang_constants::LOWER_Q) &&
                        traits::case_insensitive_ex::eq(consonants[1], common_lang_constants::LOWER_U)) ?
                true : (traits::case_insensitive_ex::eq(consonants[0], common_lang_constants::LOWER_R) &&
                        (traits::case_insensitive_ex::eq(consonants[1], common_lang_constants::LOWER_B) ||
                        traits::case_insensitive_ex::eq(consonants[1], common_lang_constants::LOWER_C) ||
                        traits::case_insensitive_ex::eq(consonants[1], common_lang_constants::LOWER_D) ||
                        traits::case_insensitive_ex::eq(consonants[1], common_lang_constants::LOWER_F) ||
                        traits::case_insensitive_ex::eq(consonants[1], common_lang_constants::LOWER_G) ||
                        traits::case_insensitive_ex::eq(consonants[1], common_lang_constants::LOWER_K) ||
                        traits::case_insensitive_ex::eq(consonants[1], common_lang_constants::LOWER_L) ||
                        traits::case_insensitive_ex::eq(consonants[1], common_lang_constants::LOWER_M) ||
                        traits::case_insensitive_ex::eq(consonants[1], common_lang_constants::LOWER_N) ||
                        traits::case_insensitive_ex::eq(consonants[1], common_lang_constants::LOWER_P) ||
                        traits::case_insensitive_ex::eq(consonants[1], common_lang_constants::LOWER_Q) ||
                        traits::case_insensitive_ex::eq(consonants[1], common_lang_constants::LOWER_R) ||
                        traits::case_insensitive_ex::eq(consonants[1], common_lang_constants::LOWER_S) ||
                        traits::case_insensitive_ex::eq(consonants[1], common_lang_constants::LOWER_T) ||
                        traits::case_insensitive_ex::eq(consonants[1], common_lang_constants::LOWER_V) ||
                        traits::case_insensitive_ex::eq(consonants[1], common_lang_constants::LOWER_Z) )) ?
                true : (traits::case_insensitive_ex::eq(consonants[0], common_lang_constants::LOWER_S) &&
                        (traits::case_insensitive_ex::eq(consonants[1], common_lang_constants::LOWER_C) ||
                        traits::case_insensitive_ex::eq(consonants[1], common_lang_constants::LOWER_H) ||
                        traits::case_insensitive_ex::eq(consonants[1], common_lang_constants::LOWER_K) ||
                        traits::case_insensitive_ex::eq(consonants[1], common_lang_constants::LOWER_L) ||
                        traits::case_insensitive_ex::eq(consonants[1], common_lang_constants::LOWER_N) ||
                        traits::case_insensitive_ex::eq(consonants[1], common_lang_constants::LOWER_P) ||
                        traits::case_insensitive_ex::eq(consonants[1], common_lang_constants::LOWER_S) ||
                        traits::case_insensitive_ex::eq(consonants[1], common_lang_constants::LOWER_T) )) ?
                true : (traits::case_insensitive_ex::eq(consonants[0], common_lang_constants::LOWER_T) &&
                        (traits::case_insensitive_ex::eq(consonants[1], common_lang_constants::LOWER_H) ||
                        traits::case_insensitive_ex::eq(consonants[1], common_lang_constants::LOWER_T) ||
                        traits::case_insensitive_ex::eq(consonants[1], common_lang_constants::LOWER_Z) )) ?
                true : (traits::case_insensitive_ex::eq(consonants[0], common_lang_constants::LOWER_W) &&
                        (traits::case_insensitive_ex::eq(consonants[1], common_lang_constants::LOWER_L) ||
                        traits::case_insensitive_ex::eq(consonants[1], common_lang_constants::LOWER_N) ||
                        traits::case_insensitive_ex::eq(consonants[1], common_lang_constants::LOWER_S) ||
                        traits::case_insensitive_ex::eq(consonants[1], common_lang_constants::LOWER_K) )) ?
                true : (traits::case_insensitive_ex::eq(consonants[0], common_lang_constants::LOWER_Y) &&
                        traits::case_insensitive_ex::eq(consonants[1], common_lang_constants::LOWER_L)) ?// gargoyle
                true : (traits::case_insensitive_ex::eq(consonants[0], common_lang_constants::LOWER_Z) &&
                        traits::case_insensitive_ex::eq(consonants[1], common_lang_constants::LOWER_Z)) ?
                true : false;
            }
        else if (block_length == 1)
            {
            return true;
            }
        else
            {
            return false;
            }
        }

    bool english_syllabize::can_consonants_begin_sound(const wchar_t* consonants, size_t block_length)
        {
        assert(consonants);
        if (consonants == nullptr)
            { return false; }
        /*if not a valid letter or length, then it is definitely a new section of the word
        It is probably a hyphen or period.*/
        if (block_length == 0)
            { return false; }
        if (!characters::is_character::is_alpha(consonants[0]) )
            { return true; }
        else if (block_length > 3)
            { return false; }
        else if (block_length == 3)
            {
            // true if chr, phr, sch, shr, spr, str,scr, thr, sdr
            return (traits::case_insensitive_ex::eq(consonants[0], common_lang_constants::LOWER_C) &&
                    traits::case_insensitive_ex::eq(consonants[1], common_lang_constants::LOWER_H) &&
                    traits::case_insensitive_ex::eq(consonants[2], common_lang_constants::LOWER_R)) ?
                true : (traits::case_insensitive_ex::eq(consonants[0], common_lang_constants::LOWER_P) &&
                        traits::case_insensitive_ex::eq(consonants[1], common_lang_constants::LOWER_H) &&
                        traits::case_insensitive_ex::eq(consonants[2], common_lang_constants::LOWER_R)) ?
                true : (traits::case_insensitive_ex::eq(consonants[0], common_lang_constants::LOWER_S) &&
                        traits::case_insensitive_ex::eq(consonants[1], common_lang_constants::LOWER_C) &&
                        traits::case_insensitive_ex::eq(consonants[2], common_lang_constants::LOWER_H)) ?
                true : (traits::case_insensitive_ex::eq(consonants[0], common_lang_constants::LOWER_S) &&
                        traits::case_insensitive_ex::eq(consonants[1], common_lang_constants::LOWER_H) &&
                        traits::case_insensitive_ex::eq(consonants[2], common_lang_constants::LOWER_R) ) ?
                true : (traits::case_insensitive_ex::eq(consonants[0], common_lang_constants::LOWER_S) &&
                        traits::case_insensitive_ex::eq(consonants[1], common_lang_constants::LOWER_P) &&
                        traits::case_insensitive_ex::eq(consonants[2], common_lang_constants::LOWER_R)) ?
                true : (traits::case_insensitive_ex::eq(consonants[0], common_lang_constants::LOWER_S) &&
                        traits::case_insensitive_ex::eq(consonants[1], common_lang_constants::LOWER_T) &&
                        traits::case_insensitive_ex::eq(consonants[2], common_lang_constants::LOWER_R)) ?
                true : (traits::case_insensitive_ex::eq(consonants[0], common_lang_constants::LOWER_S) &&
                        traits::case_insensitive_ex::eq(consonants[1], common_lang_constants::LOWER_C) &&
                        traits::case_insensitive_ex::eq(consonants[2], common_lang_constants::LOWER_R)) ?
                true : (traits::case_insensitive_ex::eq(consonants[0], common_lang_constants::LOWER_T) &&
                        traits::case_insensitive_ex::eq(consonants[1], common_lang_constants::LOWER_H) &&
                        traits::case_insensitive_ex::eq(consonants[2], common_lang_constants::LOWER_R)) ?
                true : (traits::case_insensitive_ex::eq(consonants[0], common_lang_constants::LOWER_S) &&
                        traits::case_insensitive_ex::eq(consonants[1], common_lang_constants::LOWER_D) &&
                        traits::case_insensitive_ex::eq(consonants[2], common_lang_constants::LOWER_R)) ?
                true : false;
            }
        else if (block_length == 2)
            {
            // true if bl, br, ch, cl, cr, fl, fr, gn, gu, gr, pl, pr, th, tr, wr, wh, sc
            // sh, sk, sl, sm, sn, sp, sq, st, sw, sf, sb, sd
            return (traits::case_insensitive_ex::eq(consonants[0], common_lang_constants::LOWER_B) &&
                    (traits::case_insensitive_ex::eq(consonants[1], common_lang_constants::LOWER_L) || traits::case_insensitive_ex::eq(consonants[1], common_lang_constants::LOWER_R)) ) ?
                true : (traits::case_insensitive_ex::eq(consonants[0], common_lang_constants::LOWER_C) &&
                        (traits::case_insensitive_ex::eq(consonants[1], common_lang_constants::LOWER_H) ||
                            traits::case_insensitive_ex::eq(consonants[1], common_lang_constants::LOWER_L) ||
                            traits::case_insensitive_ex::eq(consonants[1], common_lang_constants::LOWER_R)) ) ?
                true : (traits::case_insensitive_ex::eq(consonants[0], common_lang_constants::LOWER_D) &&
                        traits::case_insensitive_ex::eq(consonants[1], common_lang_constants::LOWER_R)) ?
                true : (traits::case_insensitive_ex::eq(consonants[0], common_lang_constants::LOWER_F) &&
                        (traits::case_insensitive_ex::eq(consonants[1], common_lang_constants::LOWER_L) || traits::case_insensitive_ex::eq(consonants[1], common_lang_constants::LOWER_R)) ) ?
                true : (traits::case_insensitive_ex::eq(consonants[0], common_lang_constants::LOWER_G) &&
                        (traits::case_insensitive_ex::eq(consonants[1], common_lang_constants::LOWER_N) ||
                            traits::case_insensitive_ex::eq(consonants[1], common_lang_constants::LOWER_U) ||
                            traits::case_insensitive_ex::eq(consonants[1], common_lang_constants::LOWER_R)) ) ?
                true : (traits::case_insensitive_ex::eq(consonants[0], common_lang_constants::LOWER_K) &&
                        traits::case_insensitive_ex::eq(consonants[1], common_lang_constants::LOWER_N)) ?
                true : (traits::case_insensitive_ex::eq(consonants[0], common_lang_constants::LOWER_P) &&
                        (traits::case_insensitive_ex::eq(consonants[1], common_lang_constants::LOWER_L) || traits::case_insensitive_ex::eq(consonants[1], common_lang_constants::LOWER_R)) ) ?
                true : (traits::case_insensitive_ex::eq(consonants[0], common_lang_constants::LOWER_T) &&
                        (traits::case_insensitive_ex::eq(consonants[1], common_lang_constants::LOWER_H) || traits::case_insensitive_ex::eq(consonants[1], common_lang_constants::LOWER_R)) ) ?
                true : (traits::case_insensitive_ex::eq(consonants[0], common_lang_constants::LOWER_W) &&
                        (traits::case_insensitive_ex::eq(consonants[1], common_lang_constants::LOWER_R) || traits::case_insensitive_ex::eq(consonants[1], common_lang_constants::LOWER_H)) ) ?
                true : (traits::case_insensitive_ex::eq(consonants[0], common_lang_constants::LOWER_S) &&
                        (traits::case_insensitive_ex::eq(consonants[1], common_lang_constants::LOWER_C) ||
                            traits::case_insensitive_ex::eq(consonants[1], common_lang_constants::LOWER_H) ||
                            traits::case_insensitive_ex::eq(consonants[1], common_lang_constants::LOWER_K) ||
                            traits::case_insensitive_ex::eq(consonants[1], common_lang_constants::LOWER_L) ||
                            traits::case_insensitive_ex::eq(consonants[1], common_lang_constants::LOWER_M) ||
                            traits::case_insensitive_ex::eq(consonants[1], common_lang_constants::LOWER_N) ||
                            traits::case_insensitive_ex::eq(consonants[1], common_lang_constants::LOWER_P) ||
                            traits::case_insensitive_ex::eq(consonants[1], common_lang_constants::LOWER_Q) ||
                            traits::case_insensitive_ex::eq(consonants[1], common_lang_constants::LOWER_T) ||
                            traits::case_insensitive_ex::eq(consonants[1], common_lang_constants::LOWER_W) ||
                            traits::case_insensitive_ex::eq(consonants[1], common_lang_constants::LOWER_F) ||
                            traits::case_insensitive_ex::eq(consonants[1], common_lang_constants::LOWER_B) ||
                            traits::case_insensitive_ex::eq(consonants[1], common_lang_constants::LOWER_D) )) ?
                true : false;
            }
        else if (block_length == 1)
            {
            if (traits::case_insensitive_ex::eq(consonants[0], common_lang_constants::LOWER_X) )
                {
                return false;
                }
            else return true;
            }
        else
            {
            return false;
            }
        }

    std::pair<size_t,size_t> english_syllabize::get_prefix_length(const wchar_t* start, const size_t length)
        {
        assert(start);
        if (start == nullptr)
            { return std::make_pair(0,0); }
        // prayer
        if (length >= 6 &&
            traits::case_insensitive_ex::eq(start[0], common_lang_constants::LOWER_P) &&
            traits::case_insensitive_ex::eq(start[1], common_lang_constants::LOWER_R) &&
            traits::case_insensitive_ex::eq(start[2], common_lang_constants::LOWER_A) &&
            traits::case_insensitive_ex::eq(start[3], common_lang_constants::LOWER_Y) &&
            traits::case_insensitive_ex::eq(start[4], common_lang_constants::LOWER_E) &&
            traits::case_insensitive_ex::eq(start[5], common_lang_constants::LOWER_R))
            { return std::pair<size_t,size_t>(1,6); }
        if (length >= 5)
            {
            // where
            if (traits::case_insensitive_ex::eq(start[0], common_lang_constants::LOWER_W) &&
                traits::case_insensitive_ex::eq(start[1], common_lang_constants::LOWER_H) &&
                traits::case_insensitive_ex::eq(start[2], common_lang_constants::LOWER_E) &&
                traits::case_insensitive_ex::eq(start[3], common_lang_constants::LOWER_R) &&
                traits::case_insensitive_ex::eq(start[4], common_lang_constants::LOWER_E))
                {
                // exception: wherever
                if (length >= 6 && traits::case_insensitive_ex::eq(start[5], common_lang_constants::LOWER_V))
                    { return std::pair<size_t,size_t>(2,6); }
                return std::pair<size_t,size_t>(1,5);
                }
            // readj
            else if (traits::case_insensitive_ex::eq(start[0], common_lang_constants::LOWER_R) &&
                traits::case_insensitive_ex::eq(start[1], common_lang_constants::LOWER_E) &&
                traits::case_insensitive_ex::eq(start[2], common_lang_constants::LOWER_A) &&
                traits::case_insensitive_ex::eq(start[3], common_lang_constants::LOWER_D) &&
                traits::case_insensitive_ex::eq(start[4], common_lang_constants::LOWER_J))
                { return std::pair<size_t,size_t>(2,5); }
            // rearm, readm
            else if (traits::case_insensitive_ex::eq(start[0], common_lang_constants::LOWER_R) &&
                traits::case_insensitive_ex::eq(start[1], common_lang_constants::LOWER_E) &&
                traits::case_insensitive_ex::eq(start[2], common_lang_constants::LOWER_A) &&
                (traits::case_insensitive_ex::eq(start[3], common_lang_constants::LOWER_R) ||
                    traits::case_insensitive_ex::eq(start[3], common_lang_constants::LOWER_D)) &&
                traits::case_insensitive_ex::eq(start[4], common_lang_constants::LOWER_M))
                { return std::pair<size_t,size_t>(2,5); }
            // reall
            else if (traits::case_insensitive_ex::eq(start[0], common_lang_constants::LOWER_R) &&
                traits::case_insensitive_ex::eq(start[1], common_lang_constants::LOWER_E) &&
                traits::case_insensitive_ex::eq(start[2], common_lang_constants::LOWER_A) &&
                traits::case_insensitive_ex::eq(start[3], common_lang_constants::LOWER_L) &&
                traits::case_insensitive_ex::eq(start[4], common_lang_constants::LOWER_L))
                {
                if (length == 6 && traits::case_insensitive_ex::eq(start[5], common_lang_constants::LOWER_Y))
                    { return std::pair<size_t,size_t>(3,6); }
                return std::pair<size_t,size_t>(2,5);
                }
            // reapp
            else if (traits::case_insensitive_ex::eq(start[0], common_lang_constants::LOWER_R) &&
                traits::case_insensitive_ex::eq(start[1], common_lang_constants::LOWER_E) &&
                traits::case_insensitive_ex::eq(start[2], common_lang_constants::LOWER_A) &&
                traits::case_insensitive_ex::eq(start[3], common_lang_constants::LOWER_P) &&
                traits::case_insensitive_ex::eq(start[4], common_lang_constants::LOWER_P))
                { return std::pair<size_t,size_t>(2,5); }
            // sales
            else if (traits::case_insensitive_ex::eq(start[0], common_lang_constants::LOWER_S) &&
                traits::case_insensitive_ex::eq(start[1], common_lang_constants::LOWER_A) &&
                traits::case_insensitive_ex::eq(start[2], common_lang_constants::LOWER_L) &&
                traits::case_insensitive_ex::eq(start[3], common_lang_constants::LOWER_E) &&
                traits::case_insensitive_ex::eq(start[4], common_lang_constants::LOWER_S))
                { return std::pair<size_t,size_t>(1,5); }
            // intra
            else if (traits::case_insensitive_ex::eq(start[0], common_lang_constants::LOWER_I) &&
                traits::case_insensitive_ex::eq(start[1], common_lang_constants::LOWER_N) &&
                traits::case_insensitive_ex::eq(start[2], common_lang_constants::LOWER_T) &&
                traits::case_insensitive_ex::eq(start[3], common_lang_constants::LOWER_R) &&
                traits::case_insensitive_ex::eq(start[4], common_lang_constants::LOWER_A))
                { return std::pair<size_t,size_t>(2,5); }
            // reass
            else if (traits::case_insensitive_ex::eq(start[0], common_lang_constants::LOWER_R) &&
                traits::case_insensitive_ex::eq(start[1], common_lang_constants::LOWER_E) &&
                traits::case_insensitive_ex::eq(start[2], common_lang_constants::LOWER_A) &&
                traits::case_insensitive_ex::eq(start[3], common_lang_constants::LOWER_S) &&
                traits::case_insensitive_ex::eq(start[4], common_lang_constants::LOWER_S))
                { return std::pair<size_t,size_t>(2,5); }
            // reatt
            else if (traits::case_insensitive_ex::eq(start[0], common_lang_constants::LOWER_R) &&
                traits::case_insensitive_ex::eq(start[1], common_lang_constants::LOWER_E) &&
                traits::case_insensitive_ex::eq(start[2], common_lang_constants::LOWER_A) &&
                traits::case_insensitive_ex::eq(start[3], common_lang_constants::LOWER_T) &&
                traits::case_insensitive_ex::eq(start[4], common_lang_constants::LOWER_T))
                { return std::pair<size_t,size_t>(2,5); }
            // retro
            else if (traits::case_insensitive_ex::eq(start[0], common_lang_constants::LOWER_R) &&
                traits::case_insensitive_ex::eq(start[1], common_lang_constants::LOWER_E) &&
                traits::case_insensitive_ex::eq(start[2], common_lang_constants::LOWER_T) &&
                traits::case_insensitive_ex::eq(start[3], common_lang_constants::LOWER_R) &&
                traits::case_insensitive_ex::eq(start[4], common_lang_constants::LOWER_O))
                { return std::pair<size_t,size_t>(2,5); }
            // supra
            else if (traits::case_insensitive_ex::eq(start[0], common_lang_constants::LOWER_S) &&
                traits::case_insensitive_ex::eq(start[1], common_lang_constants::LOWER_U) &&
                traits::case_insensitive_ex::eq(start[2], common_lang_constants::LOWER_P) &&
                traits::case_insensitive_ex::eq(start[3], common_lang_constants::LOWER_R) &&
                traits::case_insensitive_ex::eq(start[4], common_lang_constants::LOWER_A))
                { return std::pair<size_t,size_t>(2,5); }
            // ultra
            else if (traits::case_insensitive_ex::eq(start[0], common_lang_constants::LOWER_U) &&
                traits::case_insensitive_ex::eq(start[1], common_lang_constants::LOWER_L) &&
                traits::case_insensitive_ex::eq(start[2], common_lang_constants::LOWER_T) &&
                traits::case_insensitive_ex::eq(start[3], common_lang_constants::LOWER_R) &&
                traits::case_insensitive_ex::eq(start[4], common_lang_constants::LOWER_A))
                { return std::pair<size_t,size_t>(2,5); }
            // video
            else if (traits::case_insensitive_ex::eq(start[0], common_lang_constants::LOWER_V) &&
                traits::case_insensitive_ex::eq(start[1], common_lang_constants::LOWER_I) &&
                traits::case_insensitive_ex::eq(start[2], common_lang_constants::LOWER_D) &&
                traits::case_insensitive_ex::eq(start[3], common_lang_constants::LOWER_E) &&
                traits::case_insensitive_ex::eq(start[4], common_lang_constants::LOWER_O))
                { return std::pair<size_t,size_t>(3,5); }
            // under
            else if (traits::case_insensitive_ex::eq(start[0], common_lang_constants::LOWER_U) &&
                traits::case_insensitive_ex::eq(start[1], common_lang_constants::LOWER_N) &&
                traits::case_insensitive_ex::eq(start[2], common_lang_constants::LOWER_D) &&
                traits::case_insensitive_ex::eq(start[3], common_lang_constants::LOWER_E) &&
                traits::case_insensitive_ex::eq(start[4], common_lang_constants::LOWER_R))
                { return std::pair<size_t,size_t>(2,5); }
            // inade
            else if (traits::case_insensitive_ex::eq(start[0], common_lang_constants::LOWER_I) &&
                traits::case_insensitive_ex::eq(start[1], common_lang_constants::LOWER_N) &&
                traits::case_insensitive_ex::eq(start[2], common_lang_constants::LOWER_A) &&
                traits::case_insensitive_ex::eq(start[3], common_lang_constants::LOWER_D) &&
                traits::case_insensitive_ex::eq(start[4], common_lang_constants::LOWER_E))
                { return std::pair<size_t,size_t>(3,5); }
            }
        if (length >= 4)
            {
            // over
            if (traits::case_insensitive_ex::eq(start[0], common_lang_constants::LOWER_O) &&
                traits::case_insensitive_ex::eq(start[1], common_lang_constants::LOWER_V) &&
                traits::case_insensitive_ex::eq(start[2], common_lang_constants::LOWER_E) &&
                traits::case_insensitive_ex::eq(start[3], common_lang_constants::LOWER_R))
                { return std::pair<size_t,size_t>(2,4); }
            // reen
            else if (traits::case_insensitive_ex::eq(start[0], common_lang_constants::LOWER_R) &&
                traits::case_insensitive_ex::eq(start[1], common_lang_constants::LOWER_E) &&
                traits::case_insensitive_ex::eq(start[2], common_lang_constants::LOWER_E) &&
                traits::case_insensitive_ex::eq(start[3], common_lang_constants::LOWER_N))
                { return std::pair<size_t,size_t>(2,4); }
            // reaf
            else if (traits::case_insensitive_ex::eq(start[0], common_lang_constants::LOWER_R) &&
                traits::case_insensitive_ex::eq(start[1], common_lang_constants::LOWER_E) &&
                traits::case_insensitive_ex::eq(start[2], common_lang_constants::LOWER_A) &&
                traits::case_insensitive_ex::eq(start[3], common_lang_constants::LOWER_F))
                { return std::pair<size_t,size_t>(2,4); }
            // gyne
            else if (traits::case_insensitive_ex::eq(start[0], common_lang_constants::LOWER_G) &&
                traits::case_insensitive_ex::eq(start[1], common_lang_constants::LOWER_Y) &&
                traits::case_insensitive_ex::eq(start[2], common_lang_constants::LOWER_N) &&
                traits::case_insensitive_ex::eq(start[3], common_lang_constants::LOWER_E))
                { return std::pair<size_t,size_t>(2,4); }
            // real
            else if (traits::case_insensitive_ex::eq(start[0], common_lang_constants::LOWER_R) &&
                traits::case_insensitive_ex::eq(start[1], common_lang_constants::LOWER_E) &&
                traits::case_insensitive_ex::eq(start[2], common_lang_constants::LOWER_A) &&
                traits::case_insensitive_ex::eq(start[3], common_lang_constants::LOWER_L))
                {
                // exceptions: realm, real
                if (length >= 4 && traits::case_insensitive_ex::eq(start[4], common_lang_constants::LOWER_M))
                    { return std::pair<size_t,size_t>(0,0); }
                else if (length == 4)
                    { return std::pair<size_t,size_t>(0,0); }
                return std::pair<size_t,size_t>(2,4);
                }
            // blue
            else if (traits::case_insensitive_ex::eq(start[0], common_lang_constants::LOWER_B) &&
                    traits::case_insensitive_ex::eq(start[1], common_lang_constants::LOWER_L) &&
                    traits::case_insensitive_ex::eq(start[2], common_lang_constants::LOWER_U) &&
                    traits::case_insensitive_ex::eq(start[3], common_lang_constants::LOWER_E))
                {
                if (length == 5 &&
                    (traits::case_insensitive_ex::eq(start[4], common_lang_constants::LOWER_R) ||
                    traits::case_insensitive_ex::eq(start[4], common_lang_constants::LOWER_T)) )
                    { return std::pair<size_t,size_t>(2,5); }
                else if (length == 6 && traits::case_insensitive_ex::eq(start[4], common_lang_constants::LOWER_S) &&
                    traits::case_insensitive_ex::eq(start[5], common_lang_constants::LOWER_T))
                    { return std::pair<size_t,size_t>(2,6); }
                return std::pair<size_t,size_t>(1,4);
                }
            // ecto
            else if (traits::case_insensitive_ex::eq(start[0], common_lang_constants::LOWER_E) &&
                    traits::case_insensitive_ex::eq(start[1], common_lang_constants::LOWER_C) &&
                    traits::case_insensitive_ex::eq(start[2], common_lang_constants::LOWER_T) &&
                    traits::case_insensitive_ex::eq(start[3], common_lang_constants::LOWER_O))
                { return std::pair<size_t,size_t>(2,4); }
            // seis
            else if (traits::case_insensitive_ex::eq(start[0], common_lang_constants::LOWER_S) &&
                    traits::case_insensitive_ex::eq(start[1], common_lang_constants::LOWER_E) &&
                    traits::case_insensitive_ex::eq(start[2], common_lang_constants::LOWER_I) &&
                    traits::case_insensitive_ex::eq(start[3], common_lang_constants::LOWER_S))
                { return std::pair<size_t,size_t>(1,4); }
            // hemi
            else if (traits::case_insensitive_ex::eq(start[0], common_lang_constants::LOWER_H) &&
                    traits::case_insensitive_ex::eq(start[1], common_lang_constants::LOWER_E) &&
                    traits::case_insensitive_ex::eq(start[2], common_lang_constants::LOWER_M) &&
                    traits::case_insensitive_ex::eq(start[3], common_lang_constants::LOWER_I))
                { return std::pair<size_t,size_t>(2,4); }
            }
        // exo
        if (length >= 3 &&
                traits::case_insensitive_ex::eq(start[0], common_lang_constants::LOWER_E) &&
                traits::case_insensitive_ex::eq(start[1], common_lang_constants::LOWER_X) &&
                traits::case_insensitive_ex::eq(start[2], common_lang_constants::LOWER_O))
            { return std::pair<size_t,size_t>(2,3); }
        else
            { return std::pair<size_t,size_t>(0,0); }
        }
    }
