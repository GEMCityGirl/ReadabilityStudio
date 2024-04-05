/** @addtogroup Indexing
    @brief Classes for parsing and indexing text.
    @date 2005-2023
    @copyright Oleander Software, Ltd.
    @author Blake Madden
    @details This program is free software; you can redistribute it and/or modify
     it under the terms of the 3-Clause BSD License.

     SPDX-License-Identifier: BSD-3-Clause
* @{*/

#ifndef __NEGATING_WORD_H__
#define __NEGATING_WORD_H__

#include "character_traits.h"
#include <algorithm>
#include <functional>
#include <set>
#include <string_view>

namespace grammar
    {
    /// @brief Functor for determining if a word is negative (e.g., "not", "doesn't").
    class is_negating
        {
      public:
        /** @returns @c true if text block is negative.
            @param text The text block to analyze.
            @todo add Spanish and German words too.*/
        [[nodiscard]]
        bool
        operator()(std::wstring_view text) const
            {
            if (text.empty())
                {
                return false;
                }
            return m_words.find(string_type{ text.data(), text.length() }) != m_words.cend();
            }

      private:
        using string_type = std::basic_string_view<wchar_t, traits::case_insensitive_ex>;
        static const std::set<string_type> m_words;
        };
    } // namespace grammar

/** @}*/

#endif //__NEGATING_WORD_H__
