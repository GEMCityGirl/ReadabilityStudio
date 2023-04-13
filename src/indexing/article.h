/** @addtogroup Indexing
    @brief Classes for parsing and indexing text.
    @date 2005-2023
    @copyright Oleander Software, Ltd.
    @author Blake Madden
    @details This program is free software; you can redistribute it and/or modify
     it under the terms of the 3-Clause BSD License.

     SPDX-License-Identifier: BSD-3-Clause
* @{*/

#ifndef __ARTICLE_H__
#define __ARTICLE_H__

#include <algorithm>
#include <functional>
#include <set>
#include "../Wisteria-Dataviz/src/i18n-check/src/string_util.h"
#include "character_traits.h"
#include "abbreviation.h"
#include "word_list.h"

namespace grammar
    {
    /** @brief Base (abstract) class for determining an invalid article/proceeding word pair.*/
    class is_incorrect_article
        {
    public:
        /// @private
        virtual ~is_incorrect_article() {}
        /** @brief Determines if an article/proceeding word pair is a mismatch.
            @param article The article.
            @param article_length The length of the article.
            @param word The word following the article.
            @param word_length The length of the word.
            @returns Whether an article/following word pair is a mismatch.*/
        [[nodiscard]]
        virtual bool operator()(const wchar_t* article, const size_t article_length,
                                const wchar_t* word, size_t word_length) const = 0;
        };

    /** @brief Predicate for determining an mismatching article/proceeding word pair.*/
    class is_incorrect_english_article final : public is_incorrect_article
        {
    public:
        /** @brief Determines if an article/proceeding word pair is a mismatch.
            @param article The article.
            @param article_length The length of the article.
            @param word The word following the article.
            @param word_length The length of the word.
            @returns Whether an article/following word pair is a mismatch.*/
        [[nodiscard]]
        bool operator()(const wchar_t* article, const size_t article_length,
                        const wchar_t* word, size_t word_length) const noexcept final;

        /** @returns The list of words following an 'a' that would be correct
                (that the analyzer might get wrong).*/
        [[nodiscard]]
        static word_list& get_a_exceptions() noexcept
            { return m_a_exceptions; }

        /** @returns The list of words following an 'an' that would be correct
                (that the analyzer might get wrong).*/
        [[nodiscard]]
        static word_list& get_an_exceptions() noexcept
            { return m_an_exceptions; }
    private:
        /** @returns @c true if a consonant-starting @c word is actually correct to
                appear after an "an".
            @param word The word to review.\n
                For example, "honor" would be an exception ("an honor").**/
        [[nodiscard]]
        static bool is_an_exception(const wchar_t* word, const size_t word_length);

        /** @returns @c true if a vowel-starting word is actually correct to
                appear after an 'a'.
            @param word The word to review.
            @param word_length The length of @c word.\n
                For example, "a Euro" or "a University".*/
        [[nodiscard]]
        static bool is_a_exception(const wchar_t* word, const size_t word_length);

        static word_list m_a_exceptions;
        static word_list m_an_exceptions;
        };
    }

/** @}*/

#endif //__ARTICLE_H__
