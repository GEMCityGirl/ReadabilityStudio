///////////////////////////////////////////////////////////////////////////////
// Name:        article.cpp
// Author:      Blake Madden
// Copyright:   (c) 2005-2023 Blake Madden
// Licence:     3-Clause BSD licence
// SPDX-License-Identifier: BSD-3-Clause
///////////////////////////////////////////////////////////////////////////////

#include "article.h"

word_list grammar::is_incorrect_english_article::m_a_exceptions;
word_list grammar::is_incorrect_english_article::m_an_exceptions;

//-------------------------------------------------------------
bool grammar::is_incorrect_english_article::operator()(std::wstring_view article,
                                                       std::wstring_view word) const noexcept
    {
    // we only look at "a" and "an"
    if (article.empty() || word.empty() || article.length() > 2 ||
        !traits::case_insensitive_ex::eq(article[0], L'a') ||
        (article.length() == 2 && !traits::case_insensitive_ex::eq(article[1], L'n')) ||
        (word.empty() || characters::is_character::is_punctuation(word[0])))
        {
        return false;
        }

    // words that are debatable, just return that they match
    if ((word.length() == 3 && traits::case_insensitive_ex::compare(word.data(), L"URL", 3) == 0) ||
        // the pronunciation of Xavier is debated, so ignore that word entirely
        (word.length() >= 3 && traits::case_insensitive_ex::compare(word.data(), L"Xav", 3) == 0))
        {
        return false;
        }

    if (const size_t hyphenPos = word.find_first_of(L"-\uFF0D");
        hyphenPos != std::wstring_view::npos)
        {
        word = word.substr(0, hyphenPos);
        }
    // word starting with five consonants is without a doubt sounded out
    const bool startsWith5Consonants =
        (word.length() >= 5 && characters::is_character::is_consonant(word[0]) &&
         characters::is_character::is_consonant(word[1]) &&
         characters::is_character::is_consonant(word[2]) &&
         characters::is_character::is_consonant(word[3]) &&
         characters::is_character::is_consonant(word[4]));
    // or weird consonant combination at the start of an acronym (e.g., "LGA", "NCAA")
    const bool oddAcronymConsonantCombo =
        (word.length() >= 2 && characters::is_character::is_upper(word[0]) &&
         characters::is_character::is_upper(word[1]) &&
         // only certain consonant combinations make sense to look like a word you would pronounce
         ((word[0] == 'F' && characters::is_character::is_consonant(word[1]) &&
           (word[1] != 'L' && word[1] != 'R')) ||
          (word[0] == 'L' && characters::is_character::is_consonant(word[1])) ||
          (word[0] == 'M' && characters::is_character::is_consonant(word[1])) ||
          (word[0] == 'N' && characters::is_character::is_consonant(word[1])) ||
          (word[0] == 'R' && characters::is_character::is_consonant(word[1])) ||
          (word[0] == 'S' && characters::is_character::is_consonant(word[1]) &&
           (word[1] != 'C' && word[1] != 'H' && word[1] != 'L' && word[1] != 'M' &&
            word[1] != 'T' && word[1] != 'W'))));
    const grammar::is_acronym isAcronym;
    const bool useLetterSoundedOut =
        (startsWith5Consonants || (word.length() == 1) ||
         // 2-letter acronym
         (word.length() == 2 && characters::is_character::is_upper(word[0]) &&
          characters::is_character::is_upper(word[1])) ||
         oddAcronymConsonantCombo || characters::is_character::is_punctuation(word[1]) ||
         characters::is_character::is_numeric(word[0]) ||
         characters::is_character::is_numeric(word[1]) ||
         // if an acronym and no vowels (including wide versions), then each letter should be
         // sounded out
         (isAcronym(word) &&
          word.find_first_of(L"aeiouy\uFF41\uFF45\uFF49\uFF4F\uFF55\uFF59AEIOUY\uFF21\uFF25"
                             "\uFF29\uFF2F\uFF35\uFF39") == std::wstring_view::npos));
    const bool isYear = (word.length() == 4 && characters::is_character::is_numeric(word[0]) &&
                         characters::is_character::is_numeric(word[1]) &&
                         characters::is_character::is_numeric(word[2]) &&
                         characters::is_character::is_numeric(word[3]));
    const bool isTime = (word.length() == 5 && characters::is_character::is_numeric(word[0]) &&
                         characters::is_character::is_numeric(word[1]) &&
                         traits::case_insensitive_ex::eq(word[2], L':') &&
                         characters::is_character::is_numeric(word[3]) &&
                         characters::is_character::is_numeric(word[4]));
    const bool is2Digit = ((word.length() == 2 && characters::is_character::is_numeric(word[0]) &&
                            characters::is_character::is_numeric(word[1])) ||
                           (word.length() > 2 && characters::is_character::is_numeric(word[0]) &&
                            characters::is_character::is_numeric(word[1]) &&
                            characters::is_character::is_punctuation(word[2])));
    const bool isSoundingOutStartingNumbers = (isYear || isTime || is2Digit);
    // if something like "1,800,00" then don't bother analyzing, will be too complicated
    if (!isSoundingOutStartingNumbers && characters::is_character::is_numeric(word[0]) &&
        std::wcscspn(word.data(), L",.") < word.length())
        {
        return false;
        }
    // a 2nd, a 1st, a 3rd
    else if (!isSoundingOutStartingNumbers && word.length() >= 3 &&
             characters::is_character::is_numeric(word[word.length() - 3]) &&
             (string_util::strnicmp(word.data() + (word.length() - 2), L"st", 2) == 0 ||
              string_util::strnicmp(word.data() + (word.length() - 2), L"rd", 2) == 0 ||
              string_util::strnicmp(word.data() + (word.length() - 2), L"nd", 2) == 0))
        {
        return !(article.length() == 1 && traits::case_insensitive_ex::eq(article[0], L'a'));
        }
    // a 5th, etc.
    else if (!isSoundingOutStartingNumbers && word.length() >= 3 &&
             characters::is_character::is_numeric(word[word.length() - 3]) &&
             string_util::strnicmp(word.data() + (word.length() - 2), L"th", 2) == 0)
        {
        // a 5th
        if (word[word.length() - 3] == L'2' || word[word.length() - 3] == L'3' ||
            word[word.length() - 3] == L'4' || word[word.length() - 3] == L'5' ||
            word[word.length() - 3] == L'6' || word[word.length() - 3] == L'7' ||
            word[word.length() - 3] == L'9' || word[word.length() - 3] == L'0')
            {
            return !(article.length() == 1 && traits::case_insensitive_ex::eq(article[0], L'a'));
            }
        // an 8th, an 11th
        else if (word[word.length() - 3] == L'1' || word[word.length() - 3] == L'8')
            {
            return !(article.length() == 2 && traits::case_insensitive_ex::eq(article[0], L'a') &&
                     traits::case_insensitive_ex::eq(article[1], L'n'));
            }
        }
    if (article.length() == 1 && traits::case_insensitive_ex::eq(article[0], L'a'))
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
            if (!is_a_exception(word) && (traits::case_insensitive_ex::eq(word[0], L'8') ||
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
                                          traits::case_insensitive_ex::eq(word[0], L'x')))
                {
                return true;
                }
            else
                {
                return false;
                }
            }
        // if starts with a vowel (excluding 'y'),
        // then it should be wrong (unless a known exception)
        else if (!traits::case_insensitive_ex::eq(word[0], L'y') &&
                 characters::is_character::is_vowel(word[0]))
            {
            return !is_a_exception(word);
            }
        // if starts with a consonant, then it should be correct
        // (unless a known exception that should actually go with an "an")
        else if (characters::is_character::is_consonant(word[0]))
            {
            return is_an_exception(word);
            }
        else
            {
            return false;
            }
        }
    else if (article.length() == 2 && traits::case_insensitive_ex::eq(article[0], L'a') &&
             traits::case_insensitive_ex::eq(article[1], L'n'))
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
            if (!is_an_exception(word) && ((traits::case_insensitive_ex::ge(word[0], L'0') &&
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
                                           traits::case_insensitive_ex::eq(word[0], L'z')))
                {
                return true;
                }
            else
                {
                return false;
                }
            }
        // check for any consonants that would be OK after an "an"
        else if (characters::is_character::is_consonant(word[0]))
            {
            return !is_an_exception(word);
            }
        // handle any 'an' exceptions that may start with a vowel
        else if (is_an_exception(word))
            {
            return false;
            }
        // if starts with a vowel (excluding 'y'),
        // then it should be correct (unless a known exception that should actually go with an 'a')
        else if (!traits::case_insensitive_ex::eq(word[0], L'y') &&
                 characters::is_character::is_vowel(word[0]))
            {
            return is_a_exception(word);
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

//-------------------------------------------------------------
bool grammar::is_incorrect_english_article::is_an_exception(std::wstring_view word)
    {
    assert(!word.empty());
    if (word.empty())
        {
        return false;
        }

    // if a known (full-word, case-insensitive) exception
    if (get_an_exceptions().contains(word))
        {
        return true;
        }
    else if (traits::case_insensitive_ex::compare(word.data(), L"hono", 4) == 0 ||
             traits::case_insensitive_ex::compare(word.data(), L"hour", 4) == 0 ||
             traits::case_insensitive_ex::compare(word.data(), L"heir", 4) == 0 ||
             traits::case_insensitive_ex::compare(word.data(), L"html", 4) == 0 ||
             traits::case_insensitive_ex::compare(word.data(), L"honest", 6) == 0 ||
             // an HRESULT
             (word.length() >= 2 && traits::case_insensitive_ex::eq(word[0], L'H') &&
              characters::is_character::is_consonant(word[1])) ||
             // treat SAT and sat differently
             (word.length() == 3 &&
              traits::case_insensitive_ex::compare_case_sensitive(word.data(), L"SAT", 3) == 0))
        {
        return true;
        }
    // an XML, an XBox
    else if (word.length() >= 2 && traits::case_insensitive_ex::eq(word[0], L'x') &&
             characters::is_character::is_consonant(word[1]))
        {
        return true;
        }
    else
        {
        return false;
        }
    }

//-------------------------------------------------------------
bool grammar::is_incorrect_english_article::is_a_exception(std::wstring_view word)
    {
    assert(!word.empty());
    if (word.empty())
        {
        return false;
        }

    static const std::set<traits::case_insensitive_wstring_ex> case_i_u_3_prefixes = {
        L"uac", L"ubi", L"udf", L"uef", L"uie", L"uin" /* a uint_32 is correct */,
        L"ukr", L"ulo", /* a ULONG_PTR */
        L"ure", L"uri", L"uro", L"usa", L"usb", L"use",
        L"usi", L"usn", L"usu", L"utc", L"utf", L"uti",
        L"uto", L"uue", /* a uuencoded */
        L"uui",         /* a UUID */
        L"uwo"
    };

    // if a known (full-word, case-insensitive) exception
    if (get_a_exceptions().contains(word))
        {
        return true;
        }
    // a
    else if (traits::case_insensitive_ex::eq(word[0], L'a'))
        {
        return false;
        }
    // e
    else if (traits::case_insensitive_ex::eq(word[0], L'e'))
        {
        if (traits::case_insensitive_ex::compare(word.data(), L"eu", 2) == 0)
            {
            return true;
            }
        else
            {
            return false;
            }
        }
    // i
    else if (traits::case_insensitive_ex::eq(word[0], L'i'))
        {
        return false;
        }
    // o
    else if (traits::case_insensitive_ex::eq(word[0], L'o'))
        {
        if (traits::case_insensitive_ex::compare(word.data(), L"one-", 4) == 0 ||
            traits::case_insensitive_ex::compare(word.data(), L"once-", 5) == 0)
            {
            return true;
            }
        // "A or B" is OK, but "a OR in the hospital" is wrong, so do this case sensitively
        else if (word.length() == 2 &&
                 traits::case_insensitive_ex::compare_case_sensitive(word.data(), L"or", 2) == 0)
            {
            return true;
            }
        else
            {
            return false;
            }
        }
    // u
    else if (traits::case_insensitive_ex::eq(word[0], L'u'))
        {
        if ((word.length() >= 3 && case_i_u_3_prefixes.find(traits::case_insensitive_wstring_ex(
                                       word.data(), 3)) != case_i_u_3_prefixes.cend()) ||
            // UNC, but not UNCLE
            (word.length() == 3 &&
             traits::case_insensitive_ex::compare_case_sensitive(word.data(), L"UNC", 3) == 0) ||
            traits::case_insensitive_ex::compare(word.data(), L"uter", 4) == 0)
            {
            return true;
            }
        // "a UX" (User eXperience) is correct
        else if (word.length() >= 2 && traits::case_insensitive_ex::eq(word[1], L'x'))
            {
            return true;
            }
        // "a UAA-compliant" is correct
        else if (word.length() >= 2 && traits::case_insensitive_ex::eq(word[1], L'a'))
            {
            return true;
            }
        else if (traits::case_insensitive_ex::compare(word.data(), L"uni", 3) == 0)
            {
            // unimpressed, uninteresting, unignored, uninitialized
            // should have "an" in front
            if (word.length() > 4 && traits::case_insensitive_ex::eq(word[3], L'n'))
                {
                return false;
                }
            // ...but "unimolecular" should be an 'a'
            else if (word.length() > 4 && (traits::case_insensitive_ex::eq(word[3], L'm') ||
                                           traits::case_insensitive_ex::eq(word[3], L'g')))
                {
                return characters::is_character::is_vowel(word[4]);
                }
            // "a unidimensional" is correct, "a undetermined" is wrong
            else if (word.length() > 4 && (traits::case_insensitive_ex::eq(word[3], L'd')))
                {
                return traits::case_insensitive_ex::eq(word[4], L'i');
                }
            // unillegal (for the sake of argument, let's say that's a word)
            else if (word.length() > 4 &&
                     (traits::case_insensitive_ex::eq(word[3], L'l') &&
                      // but should have a consonant following, because "a unilateral" is correct
                      characters::is_character::is_consonant(word[4])))
                {
                return false;
                }
            // universal, united, unicycle should have "a" in front
            else
                {
                return true;
                }
            }
        // a uranium
        else if (traits::case_insensitive_ex::compare(word.data(), L"ura", 3) == 0)
            {
            return true;
            }
        // "a unanimous decision" is correct
        else if (traits::case_insensitive_ex::compare(word.data(), L"unani", 5) == 0)
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
