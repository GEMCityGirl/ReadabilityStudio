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

/** @}*/

#endif //__CONTRACTION_H__
