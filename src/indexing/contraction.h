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

#ifndef __CONTRACTION_H__
#define __CONTRACTION_H__

#include "character_traits.h"
#include <algorithm>
#include <functional>
#include <set>
#include <string_view>

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
        bool
        operator()(const std::wstring_view text,
                   const std::wstring_view nextWord = std::wstring_view{}) const;

      private:
        using string_type = std::basic_string_view<wchar_t, traits::case_insensitive_ex>;
        static const std::set<string_type> m_s_contractions;
        static const std::set<string_type> m_s_contractions_following_word;
        static const std::set<string_type> m_contraction_without_apostrophe;
        };
    } // namespace grammar

#endif //__CONTRACTION_H__
