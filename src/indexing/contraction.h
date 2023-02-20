/** @addtogroup Indexing
    @brief Classes for parsing and indexing text.
    @date 2005-2023
    @copyright Oleander Software, Ltd.
    @author Blake Madden
    @details This program is free software; you can redistribute it and/or modify
     it under the terms of the 3-Clause BSD License.

     SPDX-License-Identifier: BSD-3-Clause
* @{*/

#ifndef __CONTRACTION_H__
#define __CONTRACTION_H__

#include <algorithm>
#include <functional>
#include <set>
#include "character_traits.h"
#include "../../../SRC/Wisteria-Dataviz/src/debug/debug_assert.h"

namespace grammar
    {
    /// @brief Functor for determining if a word is a contraction.
    /// @todo add support for German and Spanish "'s" exclusion logic.
    class is_contraction
        {
    public:
        /** @returns @c true if text block is a contraction.
            @param text The text block to analyze.
            @param length The length of the text block to analyze. This will be the start of the
                   text block up to the end of the word.
            @param nextWord The word following this one, needed for a deeper analysis if the
                   contraction ends with "'s" and may be ambiguous. This is optional.
            @param nextWordLength The length of the following word.
                Required if @c nextWord is non-null.*/
        [[nodiscard]]
        bool operator()(const wchar_t* text, const size_t length,
                        const wchar_t* nextWord = nullptr, const size_t nextWordLength = 0) const
            {
            NON_UNIT_TEST_ASSERT(text);
            NON_UNIT_TEST_ASSERT(std::wcslen(text) == length);
            NON_UNIT_TEST_ASSERT((nextWord==nullptr) || (std::wcslen(nextWord) == nextWordLength));
            if (text == nullptr || text[0] == 0 || length < 3)
                { return false; }
            for (size_t i = 0; i < length; ++i)
                {
                if (characters::is_character::is_apostrophe(text[i]))
                    {
                    if (length > 2 && i == length-2 &&
                        traits::case_insensitive_ex::eq(text[length-1], L's'))
                        {
                        // if something like "that's", then we know it is "that is" and indeed a contraction
                        if (m_s_contractions.find(string_type(text,length)) != m_s_contractions.end())
                            { return true; }
                        // otherwise, it might be a possessive word, review it...
                        else
                            {
                            if (nextWord && nextWordLength > 0)
                                {
                                return m_s_contractions_following_word.find(string_type(nextWord,nextWordLength)) !=
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
            if (m_contraction_without_apostrophe.find(string_type(text, length)) !=
                    m_contraction_without_apostrophe.cend())
                { return true; }
            return false;
            }
    private:
        using string_type = traits::case_insensitive_wstring_ex;
        static std::set<string_type> m_s_contractions;
        static std::set<string_type> m_s_contractions_following_word;
        static std::set<string_type> m_contraction_without_apostrophe;
        };
    }

/** @}*/

#endif //__CONTRACTION_H__
