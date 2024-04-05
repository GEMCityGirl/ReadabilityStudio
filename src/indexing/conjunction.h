/** @addtogroup Indexing
    @brief Classes for parsing and indexing text.
    @date 2005-2023
    @copyright Oleander Software, Ltd.
    @author Blake Madden
    @details This program is free software; you can redistribute it and/or modify
     it under the terms of the 3-Clause BSD License.

     SPDX-License-Identifier: BSD-3-Clause
* @{*/

#ifndef __CONJUNCTION_H__
#define __CONJUNCTION_H__

#include "../Wisteria-Dataviz/src/i18n-check/src/string_util.h"
#include "character_traits.h"
#include <algorithm>
#include <functional>
#include <set>
#include <string_view>

namespace grammar
    {
    /// @brief Base class for determining if a word is a coordinating conjunction.
    class is_coordinating_conjunction
        {
      public:
        /// @private
        virtual ~is_coordinating_conjunction() {}

        /** @brief Determines if a word is a coordinating conjunction.
            @param text The word to review.
            @returns Whether or not this word is a coordinating conjunction.*/
        virtual bool operator()(const std::wstring_view text) const = 0;

      protected:
        /// @private
        using string_type = std::basic_string_view<wchar_t, traits::case_insensitive_ex>;
        };

    /** @brief Predicate for determining if a word is an
            English coordinating conjunction (case insensitive).*/
    class is_english_coordinating_conjunction final : public is_coordinating_conjunction
        {
      public:
        /** @brief Determines if a word is an English coordinating conjunction.
            @param text The word to review.
            @returns Whether or not this word is a coordinating conjunction.*/
        [[nodiscard]]
        bool
        operator()(const std::wstring_view text) const final
            {
            if (text.empty())
                {
                return false;
                }
            return m_conjunctions.find(string_type(text.data(), text.length())) !=
                   m_conjunctions.cend();
            }

      private:
        static const std::set<string_type> m_conjunctions;
        };

    /** Predicate for determining if a word is a Spanish coordinating
            conjunction (case insensitive).*/
    class is_spanish_coordinating_conjunction final : public is_coordinating_conjunction
        {
      public:
        /** @brief Determines if a word is a Spanish coordinating conjunction.
            @param text The word to review.
            @returns Whether or not this word is a coordinating conjunction.*/
        [[nodiscard]]
        bool
        operator()(const std::wstring_view text) const final
            {
            if (text.empty())
                {
                return false;
                }
            return m_conjunctions.find(string_type(text.data(), text.length())) !=
                   m_conjunctions.cend();
            }

      private:
        static const std::set<string_type> m_conjunctions;
        };

    /** @brief Predicate for determining if a word is a German coordinating
            conjunction (case insensitive).*/
    class is_german_coordinating_conjunction final : public is_coordinating_conjunction
        {
      public:
        /** @brief Determines if a word is a German coordinating conjunction.
            @param text The word to review.
            @returns Whether or not this word is a coordinating conjunction.*/
        [[nodiscard]]
        bool
        operator()(const std::wstring_view text) const final
            {
            if (text.empty())
                {
                return false;
                }
            return m_conjunctions.find(string_type(text.data(), text.length())) !=
                   m_conjunctions.cend();
            }

      private:
        static const std::set<string_type> m_conjunctions;
        };

    /// @brief Predicate for determining if a word is a Russian coordinating
    ///     conjunction (case insensitive).
    /// @todo add '&'
    /// @todo make this a binary searchable set
    class is_russian_coordinating_conjunction final : public is_coordinating_conjunction
        {
      public:
        /** @brief Determines if a word is a Russian coordinating conjunction.
            @param text The word to review.

            @returns Whether or not this word is a coordinating conjunction.*/
        [[nodiscard]]
        bool
        operator()(const std::wstring_view text) const noexcept final
            {
            if (text.empty())
                {
                return false;
                }
            // coordinating conjunctions
            return (text.length() == 1 &&
                    // i (and/both...and)
                    string_util::is_either<wchar_t>(text[0], 0x0418, 0x0438)) ||
                   // a (but)
                   (text.length() == 1 &&
                    string_util::is_either<wchar_t>(text[0], 0x0410, 0x0430)) ||
                   (text.length() == 2 &&
                    string_util::is_either<wchar_t>(text[0], 0x041D, 0x043D) &&
                    // ni (neither...nor)
                    string_util::is_either<wchar_t>(text[1], 0x0418, 0x0438)) ||
                   (text.length() == 2 &&
                    string_util::is_either<wchar_t>(text[0], 0x041D, 0x043D) &&
                    // no (but)
                    string_util::is_either<wchar_t>(text[1], 0x041E, 0x043E)) ||
                   (text.length() == 2 &&
                    string_util::is_either<wchar_t>(text[0], 0x0414, 0x0434) &&
                    // da (and)
                    string_util::is_either<wchar_t>(text[1], 0x0410, 0x0430)) ||
                   (text.length() == 2 &&
                    string_util::is_either<wchar_t>(text[0], 0x0422, 0x0442) &&
                    // to (first...then)
                    string_util::is_either<wchar_t>(text[1], 0x041E, 0x043E)) ||
                   (text.length() == 3 &&
                    string_util::is_either<wchar_t>(text[0], 0x041A, 0x043A) &&
                    string_util::is_either<wchar_t>(text[1], 0x0422, 0x0442) &&
                    // kto (some...other)
                    string_util::is_either<wchar_t>(text[2], 0x041E, 0x043E)) ||
                   (text.length() == 3 &&
                    string_util::is_either<wchar_t>(text[0], 0x0418, 0x0438) &&
                    string_util::is_either<wchar_t>(text[1], 0x041B, 0x043B) &&
                    // ili (either...or)
                    string_util::is_either<wchar_t>(text[2], 0x0418, 0x0438));
            }
        };
    } // namespace grammar

/** @}*/

#endif //__CONJUNCTION_H__
