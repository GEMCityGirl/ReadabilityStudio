/** @addtogroup Indexing
    @brief Classes for parsing and indexing text.
    @date 2005-2023
    @copyright Oleander Software, Ltd.
    @author Blake Madden
    @details This program is free software; you can redistribute it and/or modify
     it under the terms of the 3-Clause BSD License.

     SPDX-License-Identifier: BSD-3-Clause
* @{*/


#ifndef __DOUBLE_WORDS_H__
#define __DOUBLE_WORDS_H__

#include <algorithm>
#include <functional>
#include <set>
#include <cwctype>
#include "character_traits.h"

namespace grammar
    {
    /** @brief Class to determine if a set of double words is grammatically correct.

        Works for English and German text.*/
    class is_double_word_exception
        {
    public:
        /// @brief Determines if a word is grammatically correct if repeated.
        /// @param text The word to review.
        /// @param length The length of the word.
        /// @returns @c true if this word is allowed to be repeated.
        [[nodiscard]]
        bool operator()(const wchar_t* text, const size_t length) const
            {
            if (text == nullptr || text[0] == 0 || length == 0 ||
                (length == 1 && characters::is_character::is_punctuation(text[0])))
                { return true; }
            return m_double_word_exceptions.find(string_type(text,length)) !=
                m_double_word_exceptions.cend();
            }
    private:
        using string_type = traits::case_insensitive_wstring_ex;
        static std::set<string_type> m_double_word_exceptions;
        };
    }

/** @}*/

#endif //__DOUBLE_WORDS_H__
