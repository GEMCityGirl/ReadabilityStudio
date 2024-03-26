///////////////////////////////////////////////////////////////////////////////
// Name:        contraction.cpp
// Author:      Blake Madden
// Copyright:   (c) 2005-2023 Blake Madden
// Licence:     3-Clause BSD licence
// SPDX-License-Identifier: BSD-3-Clause
///////////////////////////////////////////////////////////////////////////////

#include "contraction.h"

using namespace grammar;

// Words that end with "'s" that are NOT possessive, they are contractions of "is" or "has".
std::set<is_contraction::string_type> is_contraction::m_s_contractions = {
    L"anything's", L"everything's", L"it's",        L"he's",    L"here's",  L"how's",
    L"let's",      L"she's",        L"something's", L"that's",  L"there's", L"this's",
    L"what's",     L"when's",       L"where's",     L"which's", L"who's",   L"why's"
};

// Words following an "'s" that indicate that it may be a contraction
// of "is" or "has". These are used when the word connected to the "'s"
// is unknown and can be ambiguous as a possessive  word. For example:
// "Frank's got a new car. Frank's a happy man. Frank's car is nice."
// The first two "Frank's" are contractions, the last one is a possessive  noun.
std::set<is_contraction::string_type> is_contraction::m_s_contractions_following_word = {
    L"a", L"an", L"got", L"the"
};

// Contractions that would start with an apostrophe (that parser may not be including).
std::set<is_contraction::string_type> is_contraction::m_contraction_without_apostrophe = {
    L"tis", L"twas", L"twere", L"twould", L"twill",
    // Contractions of two words without an apostrophe (e.g., going to -> gonna).
    L"gonna", L"wanna", L"kinda", L"shoulda", L"woulda", L"coulda", L"dunno", L"gimme", L"gotta"
};

//--------------------------------------------------------
bool is_contraction::operator()(const std::wstring_view text,
                                const std::wstring_view nextWord /*= std::wstring_view{}*/) const
    {
    if (text.length() < 3)
        {
        return false;
        }
    for (size_t i = 0; i < text.length(); ++i)
        {
        if (characters::is_character::is_apostrophe(text[i]))
            {
            if (text.length() > 2 && i == text.length() - 2 &&
                traits::case_insensitive_ex::eq(text.back(), L's'))
                {
                // If something like "that's", then we know it is "that is"
                // and indeed a contraction.
                if (m_s_contractions.find(string_type{ text.data(), text.length() }) !=
                    m_s_contractions.cend())
                    {
                    return true;
                    }
                // ...otherwise, it might be a possessive word; review it.
                else
                    {
                    if (nextWord.length() > 0)
                        {
                        return m_s_contractions_following_word.find(
                                   string_type{ nextWord.data(), nextWord.length() }) !=
                               m_s_contractions_following_word.cend();
                        }
                    return false;
                    }
                }
            // something like "that'll" or "I'd" or "would've" will immediately return true
            return true;
            }
        }
    // "it [word]" being contracted to "t[word]"
    if (m_contraction_without_apostrophe.find(string_type{ text.data(), text.length() }) !=
        m_contraction_without_apostrophe.cend())
        {
        return true;
        }
    return false;
    }
