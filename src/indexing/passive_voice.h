/** @addtogroup Indexing
    @brief Classes for parsing and indexing text.
    @date 2005-2023
    @copyright Oleander Software, Ltd.
    @author Blake Madden
    @details This program is free software; you can redistribute it and/or modify
     it under the terms of the 3-Clause BSD License.

     SPDX-License-Identifier: BSD-3-Clause
* @{*/

#ifndef __PASSIVE_VOICE_H__
#define __PASSIVE_VOICE_H__

#include <algorithm>
#include <functional>
#include <set>
#include <string_view>
#include "word_list.h"
#include "character_traits.h"

namespace grammar
    {
    /** @brief Predicate for determining passive voice.*/
    class is_english_passive_voice
        {
    public:
        /// @private
        using string_type = std::basic_string_view<wchar_t, traits::case_insensitive_ex>;
        /** @brief Determines if a word combination is passive voice.
            @param words The list of words
                ("to be" verb and any past participles and what not after it).
            @param max_word_count The total number of words that can be analyzed.
            @param[out] word_count Parameter to store the number of words in this passive voice.
                Will be either @c 2 or @c 3.
            @returns Whether or not this word combination is passive.*/
        template<typename Tword_iter>
        [[nodiscard]]
        bool operator()(const Tword_iter& words, const size_t max_word_count,
                        size_t& word_count) const
            {
            word_count = 0;
            // where we start to review the words after "to be"
            size_t analyzePosition{ 1 };
            if (max_word_count < 2)
                { return false; }
            // see is the verb is a "to be" verb
            if (m_to_be_verbs.find(string_type{ words[0].c_str(), words[0].length() }) == m_to_be_verbs.cend())
                { return false; }
            // step over "not" after "to be"
            if (traits::case_insensitive_ex::compare(words[1].c_str(), L"not", words[1].length()) == 0)
                {
                analyzePosition = 2;
                // "am not" at the end of a sentence wouldn't be passive
                if (analyzePosition >= max_word_count)
                    { return false; }
                }

            // if this is [to be][not][past participle]
            if (is_past_participle(words[analyzePosition]) )
                {
                if (m_past_participle_exeptions.contains(words[analyzePosition].c_str()) )
                    {
                    // "ed" word should be followed by preposition showing
                    // that something is affecting "I".
                    // For example, "I am excited BY that." is passive,
                    // but "I am excited for them" is not.
                    if (analyzePosition+1 < max_word_count &&
                        traits::case_insensitive_ex::eq(
                            words[analyzePosition][words[analyzePosition].length()-1], L'd') &&
                        traits::case_insensitive_ex::compare(
                            words[analyzePosition+1].c_str(), L"by",
                            words[analyzePosition+1].length()) == 0)
                        {
                        word_count = analyzePosition+2;
                        return true;
                        }
                    else
                        { return false; }
                    }
                else
                    {
                    // see if what looks like a past participle is really a proper name
                    if (words[analyzePosition].length() >= 2 &&
                        characters::is_character::is_upper(words[analyzePosition][0]) &&
                        characters::is_character::is_lower(words[analyzePosition][1]) )
                        { return false; }
                    word_count = analyzePosition+1;
                    return true;
                    }
                }
            return false;
            }

        /// @brief Adjectives that can be mistaken for past-tense verbs.
        /// @brief These need to be followed by a "by" to be a past-tense verb
        ///     (at least for our purposes).\n
        ///     This should be filled by the client, and will certainly not a
        ///     complete list (but should at least contain the most common ones).
        /// @returns Access to the past participle word list to edit or use.
        [[nodiscard]]
        static word_list& get_past_participle_exeptions() noexcept
            { return m_past_participle_exeptions; }
    private:
        template<typename T>
        [[nodiscard]]
        static bool is_past_participle(const T& word)
            {
            return (word.length() >= 5 &&
                     // watch out for "was eightEen"
                     !traits::case_insensitive_ex::eq(word[word.length()-3], L'e') &&
                      traits::case_insensitive_ex::eq(word[word.length()-2], L'e') &&
                      (traits::case_insensitive_ex::eq(word[word.length()-1], L'd') ||
                       traits::case_insensitive_ex::eq(word[word.length()-1], L'n')) );
            }
        static std::set<string_type> m_to_be_verbs;
        static word_list m_past_participle_exeptions;
        };
    }

/** @}*/

#endif //__PASSIVE_VOICE_H__
