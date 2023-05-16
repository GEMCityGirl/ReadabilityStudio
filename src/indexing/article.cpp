///////////////////////////////////////////////////////////////////////////////
// Name:        article.cpp
// Author:      Blake Madden
// Copyright:   (c) 2005-2023 Blake Madden
// Licence:     3-Clause BSD licence
// SPDX-License-Identifier: BSD-3-Clause
///////////////////////////////////////////////////////////////////////////////

#include "article.h"

using namespace grammar;

word_list is_incorrect_english_article::m_a_exceptions;
word_list is_incorrect_english_article::m_an_exceptions;

//-------------------------------------------------------------
bool is_incorrect_english_article::operator()(const wchar_t* article, const size_t article_length,
                                              const wchar_t* word, size_t word_length) const noexcept
    {
    // we only look at "a" and "an"
    if (article == nullptr || word == nullptr ||
        article_length == 0 || article_length > 2 ||
        !traits::case_insensitive_ex::eq(article[0], L'a') ||
        (article_length == 2 && !traits::case_insensitive_ex::eq(article[1], L'n')) ||
        word_length == 0 ||
        characters::is_character::is_punctuation(word[0]))
        { return false; }

    // words that are debatable, just return that they match
    if (word_length == 3 && traits::case_insensitive_ex::compare(word, L"URL", 3) == 0)
        { return false; }

    const auto hyphenPos = string_util::strncspn(word, word_length, L"-\uFF0D", 2);
    const wchar_t* const hyphenPosition = (hyphenPos == word_length) ? nullptr : word+hyphenPos;
    if (hyphenPosition)
        { word_length = hyphenPosition-word; }
    // word starting with five consonants is without a doubt sounded out
    const bool startsWith5Consonants = (word_length >= 5 &&
                                        characters::is_character::is_consonant(word[0]) &&
                                        characters::is_character::is_consonant(word[1]) &&
                                        characters::is_character::is_consonant(word[2]) &&
                                        characters::is_character::is_consonant(word[3]) &&
                                        characters::is_character::is_consonant(word[4]));
    // or weird consonant combination at the start of an acronym (e.g., "LGA", "NCAA")
    const bool oddAcronymConsonantCombo =
        (word_length >= 2 &&
         characters::is_character::is_upper(word[0]) &&
         characters::is_character::is_upper(word[1]) &&
         // only certain consonant combinations make sense to look like a word you would pronounce
         ((traits::case_insensitive_ex::eq_case_sensitive(word[0], 'F') &&
           characters::is_character::is_consonant(word[1]) &&
            (!traits::case_insensitive_ex::eq_case_sensitive(word[1], 'L') &&
             !traits::case_insensitive_ex::eq_case_sensitive(word[1], 'R'))) ||
         (traits::case_insensitive_ex::eq_case_sensitive(word[0], 'L') &&
          characters::is_character::is_consonant(word[1])) ||
         (traits::case_insensitive_ex::eq_case_sensitive(word[0], 'M') &&
          characters::is_character::is_consonant(word[1])) ||
         (traits::case_insensitive_ex::eq_case_sensitive(word[0], 'N') &&
          characters::is_character::is_consonant(word[1])) ||
         (traits::case_insensitive_ex::eq_case_sensitive(word[0], 'R') &&
          characters::is_character::is_consonant(word[1])) ||
         (traits::case_insensitive_ex::eq_case_sensitive(word[0], 'S') &&
          characters::is_character::is_consonant(word[1]) &&
            (!traits::case_insensitive_ex::eq_case_sensitive(word[1], 'C') &&
             !traits::case_insensitive_ex::eq_case_sensitive(word[1], 'L') &&
             !traits::case_insensitive_ex::eq_case_sensitive(word[1], 'M') &&
             !traits::case_insensitive_ex::eq_case_sensitive(word[1], 'T') &&
             !traits::case_insensitive_ex::eq_case_sensitive(word[1], 'W'))))
        );
    const grammar::is_acronym isAcronym;
    const bool useLetterSoundedOut = (startsWith5Consonants ||
        (word_length == 1) ||
        // 2-letter acronym
        (word_length == 2 && characters::is_character::is_upper(word[0]) &&
            characters::is_character::is_upper(word[1])) ||
        oddAcronymConsonantCombo ||
        characters::is_character::is_punctuation(word[1]) ||
        characters::is_character::is_numeric(word[0]) ||
        characters::is_character::is_numeric(word[1]) ||
        // if an acronym and no vowels (including wide versions), then each letter should be sounded out
        (isAcronym(word, word_length) &&
            string_util::strncspn(word, word_length,
                L"aeiouy\uFF41\uFF45\uFF49\uFF4F\uFF55\uFF59AEIOUY\uFF21\uFF25\uFF29\uFF2F\uFF35\uFF39", 24) ==
                    word_length) );
    const bool isYear = (word_length == 4 && characters::is_character::is_numeric(word[0]) &&
        characters::is_character::is_numeric(word[1]) && characters::is_character::is_numeric(word[2]) &&
        characters::is_character::is_numeric(word[3]));
    const bool isTime = (word_length == 5 && characters::is_character::is_numeric(word[0]) &&
        characters::is_character::is_numeric(word[1]) &&
        traits::case_insensitive_ex::eq(word[2], L':') &&
        characters::is_character::is_numeric(word[3]) &&
        characters::is_character::is_numeric(word[4]));
    const bool is2Digit = ((word_length == 2 &&
                            characters::is_character::is_numeric(word[0]) &&
                            characters::is_character::is_numeric(word[1])) ||
                            (word_length > 2 &&
                            characters::is_character::is_numeric(word[0]) &&
                            characters::is_character::is_numeric(word[1]) &&
                            characters::is_character::is_punctuation(word[2])));
    const bool isSoundingOutStartingNumbers = (isYear || isTime || is2Digit);
    // if something like "1,800,00" then don't bother analyzing, will be too complicated
    if (!isSoundingOutStartingNumbers &&
        characters::is_character::is_numeric(word[0]) &&
        string_util::strcspn_pointer(word, L",.", 2))
        { return false; }
    // a 2nd, a 1st, a 3rd
    else if (!isSoundingOutStartingNumbers &&
        word_length >= 3 &&
        characters::is_character::is_numeric(word[word_length-3]) &&
        (string_util::strnicmp(word+(word_length-2), L"st", 2) == 0||
            string_util::strnicmp(word+(word_length-2), L"rd", 2) == 0 ||
            string_util::strnicmp(word+(word_length-2), L"nd", 2) == 0) )
        {
        return !(article_length == 1 && traits::case_insensitive_ex::eq(article[0], L'a'));
        }
    // a 5th, etc.
    else if (!isSoundingOutStartingNumbers &&
        word_length >= 3 &&
        characters::is_character::is_numeric(word[word_length-3]) &&
        string_util::strnicmp(word+(word_length-2), L"th", 2) == 0)
        {
        // a 5th
        if (word[word_length-3] == L'2' ||
            word[word_length-3] == L'3' ||
            word[word_length-3] == L'4' ||
            word[word_length-3] == L'5' ||
            word[word_length-3] == L'6' ||
            word[word_length-3] == L'7' ||
            word[word_length-3] == L'9' ||
            word[word_length-3] == L'0')
            {
            return !(article_length == 1 &&
                traits::case_insensitive_ex::eq(article[0], L'a'));
            }
        // an 8th, an 11th
        else if (word[word_length-3] == L'1' ||
                    word[word_length-3] == L'8')
            {
            return !(article_length == 2 &&
                    traits::case_insensitive_ex::eq(article[0], L'a') &&
                    traits::case_insensitive_ex::eq(article[1], L'n'));
            }
        }
    if (article_length == 1 && traits::case_insensitive_ex::eq(article[0], L'a'))
        {
        // with years and times, you sound out the first two digits together
        if (isSoundingOutStartingNumbers)
            {
            return ((traits::case_insensitive_ex::eq(word[0], L'1') &&
                        traits::case_insensitive_ex::eq(word[1], L'1')) ||
                    (traits::case_insensitive_ex::eq(word[0], L'1') &&
                        traits::case_insensitive_ex::eq(word[1], L'8')) ||
                        traits::case_insensitive_ex::eq(word[0], L'8'));
            }
        // check for consonants that would cause this to be an error
        else if (useLetterSoundedOut)
            {
            if (traits::case_insensitive_ex::eq(word[0], L'8') ||
                traits::case_insensitive_ex::eq(word[0], L'a') ||
                traits::case_insensitive_ex::eq(word[0], L'e') ||
                traits::case_insensitive_ex::eq(word[0], L'i') ||
                traits::case_insensitive_ex::eq(word[0], L'o') ||
                // u is actually correct (e.g., "a u-turn")
                traits::case_insensitive_ex::eq(word[0], L'f') ||
                traits::case_insensitive_ex::eq(word[0], L'h') ||
                traits::case_insensitive_ex::eq(word[0], L'l') ||
                traits::case_insensitive_ex::eq(word[0], L'm') ||
                traits::case_insensitive_ex::eq(word[0], L'n') ||
                traits::case_insensitive_ex::eq(word[0], L'r') ||
                traits::case_insensitive_ex::eq(word[0], L's') ||
                traits::case_insensitive_ex::eq(word[0], L'x') )
                { return true; }
            else
                { return false; }
            }
        // if starts with a vowel (excluding 'y'),
        // then it should be wrong (unless a known exception)
        else if (!traits::case_insensitive_ex::eq(word[0], L'y') &&
            characters::is_character::is_vowel(word[0]))
            { return !is_a_exception(word, word_length); }
        // if starts with a consonant, then it should be correct
        // (unless a known exception that should actually go with an "an")
        else if (characters::is_character::is_consonant(word[0]))
            { return is_an_exception(word, word_length); }
        else
            { return false; }
        }
    else if (article_length == 2 &&
                traits::case_insensitive_ex::eq(article[0], L'a') &&
                traits::case_insensitive_ex::eq(article[1], L'n') )
        {
        if (isSoundingOutStartingNumbers)
            {
            return !((traits::case_insensitive_ex::eq(word[0], L'1') &&
                        traits::case_insensitive_ex::eq(word[1], L'1')) ||
                        (traits::case_insensitive_ex::eq(word[0], L'1') &&
                        traits::case_insensitive_ex::eq(word[1], L'8')) ||
                        traits::case_insensitive_ex::eq(word[0], L'8'));
            }
        else if (useLetterSoundedOut)
            {
            if ((traits::case_insensitive_ex::ge(word[0], L'0') &&
                    traits::case_insensitive_ex::le(word[0], L'7')) ||
                traits::case_insensitive_ex::eq(word[0], L'9') ||
                traits::case_insensitive_ex::eq(word[0], L'u') ||
                traits::case_insensitive_ex::eq(word[0], L'b') ||
                traits::case_insensitive_ex::eq(word[0], L'c') ||
                traits::case_insensitive_ex::eq(word[0], L'd') ||
                traits::case_insensitive_ex::eq(word[0], L'g') ||
                traits::case_insensitive_ex::eq(word[0], L'j') ||
                traits::case_insensitive_ex::eq(word[0], L'k') ||
                traits::case_insensitive_ex::eq(word[0], L'p') ||
                traits::case_insensitive_ex::eq(word[0], L'q') ||
                traits::case_insensitive_ex::eq(word[0], L't') ||
                traits::case_insensitive_ex::eq(word[0], L'v') ||
                traits::case_insensitive_ex::eq(word[0], L'w') ||
                traits::case_insensitive_ex::eq(word[0], L'y') ||
                traits::case_insensitive_ex::eq(word[0], L'z') )
                { return true; }
            else
                { return false; }
            }
        // check for any consonants that would be OK after an "an"
        else if (characters::is_character::is_consonant(word[0]))
            { return !is_an_exception(word, word_length); }
        // if starts with a vowel (excluding 'y'),
        // then it should be correct (unless a known exception that should actually go with an 'a')
        else if (!traits::case_insensitive_ex::eq(word[0], L'y') &&
            characters::is_character::is_vowel(word[0]))
            { return is_a_exception(word, word_length); }
        else
            { return false; }
        }
    else
        { return false; }
    }

//-------------------------------------------------------------
bool is_incorrect_english_article::is_an_exception(const wchar_t* word, const size_t word_length)
    {
    assert(word);
    if (word == nullptr || word_length == 0)
        { return false; }

    // if a known (full-word, case-insensitive) exception
    if (get_an_exceptions().find(word, word_length))
        { return true; }
    else if (traits::case_insensitive_ex::compare(word, L"hono", 4) == 0 ||
        traits::case_insensitive_ex::compare(word, L"hour", 4) == 0 ||
        traits::case_insensitive_ex::compare(word, L"heir", 4) == 0 ||
        traits::case_insensitive_ex::compare(word, L"honest", 6) == 0 ||
        // treat SAT and sat differently
        (word_length == 3 &&
         traits::case_insensitive_ex::compare_case_sensitive(word, L"SAT", 3) == 0))
        { return true; }
    else
        { return false; }
    }

//-------------------------------------------------------------
bool is_incorrect_english_article::is_a_exception(const wchar_t* word, const size_t word_length)
    {
    assert(word);
    if (word == nullptr || word_length == 0)
        { return false; }

    static const std::set<traits::case_insensitive_wstring_ex> case_i_u_3_prefixes =
        { L"ubi", L"ukr", L"ure", L"uri", L"uro",
            L"usa", L"use", L"usi", L"usu", L"utf",
            L"uti", L"uto" };

    // if a known (full-word, case-insensitive) exception
    if (get_a_exceptions().find(word, word_length))
        { return true; }
    // a
    else if (traits::case_insensitive_ex::eq(word[0], L'a'))
        { return false; }
    // e
    else if (traits::case_insensitive_ex::eq(word[0], L'e'))
        {
        if (traits::case_insensitive_ex::compare(word, L"eu", 2) == 0)
            { return true; }
        else
            { return false; }
        }
    // i
    else if (traits::case_insensitive_ex::eq(word[0], L'i'))
        { return false; }
    // o
    else if (traits::case_insensitive_ex::eq(word[0], L'o'))
        {
        if (traits::case_insensitive_ex::compare(word, L"one-", 4) == 0 ||
            traits::case_insensitive_ex::compare(word, L"once-", 5) == 0)
            { return true; }
        // "A or B" is OK, but "a OR in the hospital" is wrong, so do this case sensitively
        else if (word_length == 2 &&
            traits::case_insensitive_ex::compare_case_sensitive(word, L"or", 2) == 0)
            { return true; }
        else
            { return false; }
        }
    // u
    else if (traits::case_insensitive_ex::eq(word[0], L'u'))
        {
        if ((word_length >= 3 &&
            case_i_u_3_prefixes.find(traits::case_insensitive_wstring_ex(word, 3)) != case_i_u_3_prefixes.cend()) ||
            // UNC, but not UNCLE
            (word_length == 3 && traits::case_insensitive_ex::compare_case_sensitive(word, L"UNC", 3) == 0) ||
            traits::case_insensitive_ex::compare(word, L"uter", 4) == 0)
            { return true; }
        else if (traits::case_insensitive_ex::compare(word, L"uni", 3) == 0)
            {
            // unimpressed, uninteresting, unignored, uninitialized
            // should have "an" in front
            if (word_length > 4 &&
                traits::case_insensitive_ex::eq(word[3], L'n'))
                { return false; }
            // ...but "unimolecular" should be an 'a'
            else if (word_length > 4 &&
                (traits::case_insensitive_ex::eq(word[3], L'm') ||
                 traits::case_insensitive_ex::eq(word[3], L'g')))
                {
                return characters::is_character::is_vowel(word[4]);
                }
            // "a unidimensional" is correct, "a undertermined" is wrong
            else if (word_length > 4 &&
                (traits::case_insensitive_ex::eq(word[3], L'd')))
                { return traits::case_insensitive_ex::eq(word[4], L'i'); }
            // unillegal (for the sake of argument, let's say that's a word)
            else if (word_length > 4 &&
                (traits::case_insensitive_ex::eq(word[3], L'l') &&
                // but should have a consonant following, because "a unilateral" is correct
                characters::is_character::is_consonant(word[4])))
                { return false; }
            // universal, united, unicycle should have "a" in front
            else
                { return true; }
            }
        // a uranium
        else if (traits::case_insensitive_ex::compare(word, L"ura", 3) == 0)
            { return true; }
        // "a unanimous decision" is correct
        else if (traits::case_insensitive_ex::compare(word, L"unani", 5) == 0)
            { return true; }
        else
            { return false; }
        }
    else
        { return false; }
    }
