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

#include <algorithm>
#include <functional>
#include <set>
#include "../../../SRC/Wisteria-Dataviz/src/i18n-check/src/string_util.h"
#include "character_traits.h"

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
            @param length The length of the word.
            @returns Whether or not this word is a coordinating conjunction.*/
        virtual bool operator()(const wchar_t* text, const size_t length) const = 0;
        };

    /** @brief Predicate for determining if a word is an
            English coordinating conjunction (case insensitive).*/
    class is_english_coordinating_conjunction final : public is_coordinating_conjunction
        {
    public:
        /** @brief Determines if a word is an English coordinating conjunction.
            @param text The word to review.
            @param length The length of the word.
            @returns Whether or not this word is a coordinating conjunction.*/
        [[nodiscard]]
        bool operator()(const wchar_t* text, const size_t length) const final
            {
            if (text == nullptr || text[0] == 0 || length == 0)
                { return false; }
            return m_conjunctions.find(string_type(text,length)) != m_conjunctions.cend();
            }
    private:
        using string_type = std::basic_string<wchar_t, traits::case_insensitive_ex>;
        static std::set<string_type> m_conjunctions;
        };

    /** Predicate for determining if a word is a Spanish coordinating
            conjunction (case insensitive).*/
    class is_spanish_coordinating_conjunction final : public is_coordinating_conjunction
        {
    public:
        /** @brief Determines if a word is a Spanish coordinating conjunction.
            @param text The word to review.
            @param length The length of the word.
            @returns Whether or not this word is a coordinating conjunction.*/
        [[nodiscard]]
        bool operator()(const wchar_t* text, const size_t length) const final
            {
            if (text == nullptr || text[0] == 0 || length == 0)
                { return false; }
            return m_conjunctions.find(string_type(text,length)) != m_conjunctions.cend();
            }
    private:
        using string_type = std::basic_string<wchar_t, traits::case_insensitive_ex>;
        static std::set<string_type> m_conjunctions;
        };

    /** @brief Predicate for determining if a word is a German coordinating 
            conjunction (case insensitive).*/
    class is_german_coordinating_conjunction final : public is_coordinating_conjunction
        {
    public:
        /** @brief Determines if a word is a German coordinating conjunction.
            @param text The word to review.
            @param length The length of the word.
            @returns Whether or not this word is a coordinating conjunction.*/
        [[nodiscard]]
        bool operator()(const wchar_t* text, const size_t length) const final
            {
            if (text == nullptr || text[0] == 0 || length == 0)
                { return false; }
            return m_conjunctions.find(string_type(text,length)) != m_conjunctions.cend();
            }
    private:
        using string_type = traits::case_insensitive_wstring_ex;
        static std::set<string_type> m_conjunctions;
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
            @param length The length of the word.
            @returns Whether or not this word is a coordinating conjunction.*/
        [[nodiscard]]
        bool operator()(const wchar_t* text, const size_t length) const noexcept final
            {
            if (text == nullptr || text[0] == 0 || length == 0)
                { return false; }
            // coordinating conjunctions
            return (length == 1 && string_util::is_either<wchar_t>(text[0], 0x0418, 0x0438)) || // i (and/both...and)
                (length == 1 && string_util::is_either<wchar_t>(text[0], 0x0410, 0x0430)) || // a (but)
                (length == 2 && string_util::is_either<wchar_t>(text[0], 0x041D, 0x043D) && string_util::is_either<wchar_t>(text[1], 0x0418, 0x0438)) || //ni (neither...nor)
                (length == 2 && string_util::is_either<wchar_t>(text[0], 0x041D, 0x043D) && string_util::is_either<wchar_t>(text[1], 0x041E, 0x043E)) || //no (but)
                (length == 2 && string_util::is_either<wchar_t>(text[0], 0x0414, 0x0434) && string_util::is_either<wchar_t>(text[1], 0x0410, 0x0430)) || //da (and)
                (length == 2 && string_util::is_either<wchar_t>(text[0], 0x0422, 0x0442) && string_util::is_either<wchar_t>(text[1], 0x041E, 0x043E)) || //to (first...then)
                (length == 3 && string_util::is_either<wchar_t>(text[0], 0x041A, 0x043A) && string_util::is_either<wchar_t>(text[1], 0x0422, 0x0442) && string_util::is_either<wchar_t>(text[2], 0x041E, 0x043E)) || //kto (some...other)
                (length == 3 && string_util::is_either<wchar_t>(text[0], 0x0418, 0x0438) && string_util::is_either<wchar_t>(text[1], 0x041B, 0x043B) && string_util::is_either<wchar_t>(text[2], 0x0418, 0x0438)); //ili (either...or)
            }
        };
    }

/** @}*/

#endif //__CONJUNCTION_H__
