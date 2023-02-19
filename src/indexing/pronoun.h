/** @addtogroup Indexing
    @brief Classes for parsing and indexing text.
    @date 2003-2020
    @copyright Oleander Software, Ltd.
    @author Blake Madden
    @details This program is free software; you can redistribute it and/or modify
     it under the terms of the 3-Clause BSD License.

     SPDX-License-Identifier: BSD-3-Clause
* @{*/

#ifndef __PRONOUN_H__
#define __PRONOUN_H__

#include <algorithm>
#include <functional>
#include <set>
#include <string_view>
#include "character_traits.h"

namespace grammar
    {
    /// @brief Predicate for determining if a word is a personal pronoun (e.g., "he", "they").
    class is_personal_pronoun
        {
    public:
        /** @returns @c true if text block is a pronoun.
            @param text The text block to analyze.
            @todo add Spanish and German words too.*/
        [[nodiscard]]
        bool operator()(std::wstring_view text) const
            {
            if (text.empty())
                { return false; }
            return m_words.find(string_type(text.data(), text.length())) != m_words.cend();
            }
    private:
        using string_type = traits::case_insensitive_wstring_ex;
        static std::set<string_type> m_words;
        };
    }

/** @}*/

#endif //__PRONOUN_H__
