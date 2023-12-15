/** @addtogroup Indexing
    @brief Classes for parsing and indexing text.
    @date 2005-2020
    @copyright Oleander Software, Ltd.
    @author Blake Madden
    @details This program is free software; you can redistribute it and/or modify
     it under the terms of the 3-Clause BSD License.

     SPDX-License-Identifier: BSD-3-Clause
* @{*/

#ifndef __ABBREVIATION_H__
#define __ABBREVIATION_H__

#include "../OleanderStemmingLibrary/src/common_lang_constants.h"
#include "../Wisteria-Dataviz/src/i18n-check/src/string_util.h"
#include "character_traits.h"
#include "characters.h"
#include "word_list.h"
#include <algorithm>
#include <functional>
#include <vector>

/// @brief Namespace for grammar analysis.
namespace grammar
    {
    // @brief Predicate for determining if a word is an abbreviation (case insensitive).
    class is_abbreviation
        {
      public:
        /** @returns @c true if text block is an abbreviation
                (based on predefined list of abbreviations, or if text block is four
                consecutive consonants in front of a period).
            @param text The text block to analyze.
            @note The length of the text block to analyze should be
                the start of the text block up to the period.*/
        [[nodiscard]]
        bool
        operator()(const std::wstring_view text) const;

        /** @returns The list of abbreviations.
                This is where you can specify what is considered an abbreviation.
            @warning The abbreviations you add must include the trailing period.*/
        [[nodiscard]]
        static word_list& get_abbreviations() noexcept
            {
            return m_abbreviations;
            }

        /** @returns The list of words they may be seen as abbreviations but are not.
                This is where you can override the classifier.
            @warning The non-abbreviations you add must include the trailing period.*/
        [[nodiscard]]
        static word_list& get_non_abbreviations() noexcept
            {
            return m_nonAabbreviations;
            }

      private:
        static word_list m_abbreviations;
        static word_list m_nonAabbreviations;
        };

    /// @brief Predicate for determining if a word is an acronym.
    class is_acronym
        {
      public:
        /** @returns @c true if more than half of the letters are uppercased.
            @param text The text stream to analyze.*/
        [[nodiscard]]
        inline bool
        operator()(const std::wstring_view text) const noexcept
            {
            m_dot_count = 0;
            m_ends_with_lower_s = false;
            if (text.length() < 2)
                {
                return false;
                }
            size_t letterCount{ 0 }, upperCaseLetterCount{ 0 }, lowerCaseLetterCount{ 0 };
            for (size_t i = 0; i < text.length(); ++i)
                {
                if (text[i] == 0)
                    {
                    break;
                    }
                else if (characters::is_character::is_upper(text[i]))
                    {
                    ++letterCount;
                    ++upperCaseLetterCount;
                    }
                else if (characters::is_character::is_lower(text[i]))
                    {
                    ++letterCount;
                    ++lowerCaseLetterCount;
                    // If first character is a lower case letter then this can't possibly be an
                    // acronym. Same goes for if there is more than one lowercased letter.
                    if (i == 0 || lowerCaseLetterCount > 1)
                        {
                        return false;
                        }
                    }
                else if (traits::case_insensitive_ex::eq(text[i], common_lang_constants::PERIOD))
                    {
                    ++m_dot_count;
                    }
                }
            if (letterCount < 2)
                {
                return false;
                }
            m_ends_with_lower_s =
                (string_util::full_width_to_narrow(text.back()) == common_lang_constants::LOWER_S);
            return (upperCaseLetterCount * 2 > letterCount);
            }

        /** @returns The number of periods counted from the last call to operator().*/
        [[nodiscard]]
        size_t get_dot_count() const noexcept
            {
            return m_dot_count;
            }

        /** @returns Whether the last operator() ended with an 's'.
            @note This only applies to words longer than 2 letters,
                so "Is" would actually return false here because it is
                not an acronym and is irrelevant.*/
        [[nodiscard]]
        bool ends_with_lower_s() const noexcept
            {
            return m_ends_with_lower_s;
            }

        /** @returns Whether a word follows the Letter/Number and Dot pattern (e.g., "K.A.O.S.").
            @param text The text stream to read.*/
        [[nodiscard]]
        static bool is_dotted_acronym(const std::wstring_view text) noexcept
            {
            if (text.length() < 4)
                {
                return false;
                }
            size_t i = 0;
            for (; i + 1 < text.length(); i += 2)
                {
                if (characters::is_character::is_alpha(text[i]) &&
                    traits::case_insensitive_ex::eq(text[i + 1], common_lang_constants::PERIOD))
                    {
                    continue;
                    }
                else
                    {
                    break;
                    }
                }
            // see if followed by numbers (that can be part of the acronym, such as F.A.K.K.2)
            if (i < text.length())
                {
                while (i < text.length() && characters::is_character::is_numeric(text[i]))
                    {
                    ++i;
                    }
                }
            return (i == text.length());
            }

      private:
        mutable size_t m_dot_count{ 0 };
        mutable bool m_ends_with_lower_s{ false };
        };
    } // namespace grammar

/** @}*/

#endif //__ABBREVIATION_H__
