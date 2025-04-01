/********************************************************************************
 * Copyright (c) 2005-2025 Blake Madden
 *
 * This program and the accompanying materials are made available under the
 * terms of the Eclipse Public License 2.0 which is available at
 * https://www.eclipse.org/legal/epl-2.0.
 *
 * SPDX-License-Identifier: EPL-2.0
 *
 * Contributors:
 *   Blake Madden - initial implementation
 ********************************************************************************/

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

#endif //__NEGATING_WORD_H__
