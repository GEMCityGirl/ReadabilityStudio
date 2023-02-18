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
#include "character_traits.h"
#include "../../../SRC/Wisteria-Dataviz/src/debug/debug_assert.h"

namespace grammar
    {
    /// @brief Functor for determining if a word is a personal pronoun (e.g., "he", "they").
    class is_personal_pronoun
        {
    public:
        /** @returns @c true if text block is a pronoun.
            @param text The text block to analyze.
            @param length The length of the text block to analyze. This will be the start of the
             text block up to the end of the word.
            @todo add Spanish and German words too.*/
        [[nodiscard]] bool operator()(const wchar_t* text, const size_t length) const
            {
            NON_UNIT_TEST_ASSERT(text);
            NON_UNIT_TEST_ASSERT(std::wcslen(text) == length);
            if (text == nullptr || text[0] == 0)
                { return false; }
            return m_words.find(string_type(text,length)) != m_words.cend();
            }
    private:
        using string_type = traits::case_insensitive_wstring_ex;
        static std::set<string_type> m_words;
        };
    }

/** @}*/

#endif //__PRONOUN_H__
