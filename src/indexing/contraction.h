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
#include <string_view>
#include "character_traits.h"

namespace grammar
    {
    /// @brief Functor for determining if a word is a contraction.
    /// @todo add support for German and Spanish "'s" exclusion logic.
    class is_contraction
        {
    public:
        /** @returns @c true if text block is a contraction.
            @param text The text block to analyze.
            @param nextWord The word following this one, needed for a deeper analysis if the
                contraction ends with "'s" and may be ambiguous. This is optional.*/
        [[nodiscard]]
        bool operator()(const std::wstring_view text,
            const std::wstring_view nextWord = std::wstring_view{}) const
            {
            if (text.length() < 3)
                { return false; }
            for (size_t i = 0; i < text.length(); ++i)
                {
                if (characters::is_character::is_apostrophe(text[i]))
                    {
                    if (text.length() > 2 && i == text.length() - 2 &&
                        traits::case_insensitive_ex::eq(text.back(), L's'))
                        {
                        // If something like "that's", then we know it is "that is" and indeed a contraction.
                        if (m_s_contractions.find(
                                string_type{ text.data(), text.length() }) != m_s_contractions.cend())
                            { return true; }
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
                { return true; }
            return false;
            }
    private:
        using string_type = std::basic_string_view<wchar_t, traits::case_insensitive_ex>;
        static std::set<string_type> m_s_contractions;
        static std::set<string_type> m_s_contractions_following_word;
        static std::set<string_type> m_contraction_without_apostrophe;
        };
    }

/** @}*/

#endif //__CONTRACTION_H__
