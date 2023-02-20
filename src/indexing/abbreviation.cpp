///////////////////////////////////////////////////////////////////////////////
// Name:        abbreviation.cpp
// Author:      Blake Madden
// Copyright:   (c) 2005-2023 Blake Madden
// Licence:     3-Clause BSD licence
// SPDX-License-Identifier: BSD-3-Clause
///////////////////////////////////////////////////////////////////////////////

#include "abbreviation.h"

using namespace grammar;

word_list is_abbreviation::m_abbreviations;

bool is_abbreviation::operator()(const wchar_t* text, const size_t length) const
    {
    if (text == nullptr || text[0] == 0 || length == 0)
        { return false; }
    if (characters::is_character::is_consonant(text[0]))
        {
        size_t i = 1;
        //go through everything (except the last character) and stop if we hit something other than a consonant
        for (i = 1; i < length-1; ++i)
            {
            //if [consonant][consonant].[more letters]. then this will be a "compound abbreviation" (e.g., "std.err.").
            if (length >= 5 &&
                traits::case_insensitive_ex::eq(text[i], common_lang_constants::PERIOD) &&
                i > 1)
                { return true; }
            //we would want the rest of the consonants to be lowercase (e.g., Dpt.); otherwise, it
            //might be an acronym.
            if (!characters::is_character::is_lower_consonant(text[i]))
                { break; }
            }
        //if everything up to the last character is a consonant, the last char is a period, and there are at
        //least 4 consonants in front of the period then this more than likely an abbreviation.
        if (length >= 5 && i == (length-1) &&
            (traits::case_insensitive_ex::eq(text[length-1], common_lang_constants::PERIOD)) )
            { return true; }
        }
    word_list::word_type cmpKey(text,length);
    bool result = std::binary_search(m_abbreviations.get_words().begin(),
                            m_abbreviations.get_words().end(), 
                            cmpKey);
    //if not found, then try to see if this is more than one word combined by '/',
    //followed by an abbreviation.
    if (!result)
        {
        const size_t lastSlash = string_util::find_last_of(text, L'/', length-1);
        if (lastSlash != static_cast<size_t>(-1) &&
            lastSlash != length-1)
            {
            cmpKey.assign(text+(lastSlash+1), length-(lastSlash+1));
            result = std::binary_search(m_abbreviations.get_words().begin(),
                            m_abbreviations.get_words().end(), 
                            cmpKey);
            }
        }
    return result;
    }
