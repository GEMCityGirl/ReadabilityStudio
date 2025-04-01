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

#ifndef __DOUBLE_WORDS_H__
#define __DOUBLE_WORDS_H__

#include "character_traits.h"
#include <algorithm>
#include <cwctype>
#include <functional>
#include <set>
#include <string_view>

namespace grammar
    {
    /** @brief Class to determine if a set of double words is grammatically correct.
        @details Works for English and German text.*/
    class is_double_word_exception
        {
      public:
        /// @brief Determines if a word is grammatically correct if repeated.
        /// @param text The word to review.
        /// @returns @c true if this word is allowed to be repeated.
        [[nodiscard]]
        bool
        operator()(const std::wstring_view text) const;

      private:
        using string_type = std::basic_string_view<wchar_t, traits::case_insensitive_ex>;
        static const std::set<string_type> m_double_word_exceptions;
        };
    } // namespace grammar

#endif //__DOUBLE_WORDS_H__
