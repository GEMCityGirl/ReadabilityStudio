/** @addtogroup Helpers
    @brief Helper classes for tests and documents.
    @date 2005-2023
    @copyright Oleander Software, Ltd.
    @author Blake Madden
    @details This program is free software; you can redistribute it and/or modify
     it under the terms of the 3-Clause BSD License.

     SPDX-License-Identifier: BSD-3-Clause
* @{*/

#ifndef __TESTS_FUNCTIONAL_H__
#define __TESTS_FUNCTIONAL_H__

#include "../indexing/word_collection.h"
#include "../indexing/word_functional.h"
#include "../readability/custom_readability_test.h"
#include "../readability/dolch.h"
#include "../readability/readability.h"
#include <set>
#include <wx/string.h>

using CaseInSensitiveNonStemmingDocument = document<word_case_insensitive_no_stem>;

using CustomReadabilityTest = readability::custom_test<word_case_insensitive_no_stem>;

template<typename word_typeT>
class SyllableCountGreaterEqualWithHighlighting final
    : public syllable_count_greater_equal<word_typeT>
    {
  public:
    SyllableCountGreaterEqualWithHighlighting(size_t count, bool treat_numerals_as_monosyllabic,
                                              const wxString& beginHighlight,
                                              const wxString& endHighlight)
        : syllable_count_greater_equal<word_typeT>(count, treat_numerals_as_monosyllabic),
          m_beginHighlight(beginHighlight), m_endHighlight(endHighlight)
        {
        }

    /// @private
    SyllableCountGreaterEqualWithHighlighting() = delete;

    /// @returns The tag opening a highlighted section.
    [[nodiscard]]
    const wxString& GetHightlightBegin() const noexcept
        {
        return m_beginHighlight;
        }

    /// @returns The tag closing a highlighted section.
    [[nodiscard]]
    const wxString& GetHightlightEnd() const noexcept
        {
        return m_endHighlight;
        }

    /// @internal No-op, just needed for interface contract.
    /// @private
    // cppcheck-suppress functionStatic
    void Reset() {}

  private:
    wxString m_beginHighlight;
    wxString m_endHighlight;
    };

template<typename word_typeT>
class WordLengthGreaterEqualsWithHighlighting final
    : public word_length_excluding_punctuation_greater_equals<word_typeT>
    {
  public:
    WordLengthGreaterEqualsWithHighlighting(const size_t wordLength, const wxString& beginHighlight,
                                            const wxString& endHighlight)
        : word_length_excluding_punctuation_greater_equals<word_typeT>(wordLength),
          m_beginHighlight(beginHighlight), m_endHighlight(endHighlight)
        {
        }

    /// @private
    WordLengthGreaterEqualsWithHighlighting() = delete;

    /// @returns The tag opening a highlighted section.
    [[nodiscard]]
    const wxString& GetHightlightBegin() const noexcept
        {
        return m_beginHighlight;
        }

    /// @returns The tag closing a highlighted section.
    [[nodiscard]]
    const wxString& GetHightlightEnd() const noexcept
        {
        return m_endHighlight;
        }

    /// @internal No-op, just needed for interface contract.
    /// @private
    // cppcheck-suppress functionStatic
    void Reset() {}

  private:
    wxString m_beginHighlight;
    wxString m_endHighlight;
    };

/** @brief Determinant for whether a word is UNfamiliar,
        used for highlighting unfamiliar words in a document. */
template<typename word_typeT, typename wordlistT, typename stemmerT>
class IsNotFamiliarWordWithHighlighting
    {
  public:
    /** @brief Constructor.
        @param wlist the list of familiar words.
        @param beginHighlight the beginning tag of a highlight
            (can be HTML or RTF code, for example).
        @param endHighlight the ending tag of a highlight
            (can be HTML or RTF code, for example).
        @param properNounMethod for determining whether a proper noun
            (not on the word list) should be familiar or not.*/
    IsNotFamiliarWordWithHighlighting(const wordlistT* wlist, const wxString& beginHighlight,
                                      const wxString& endHighlight,
                                      readability::proper_noun_counting_method properNounMethod)
        : m_is_FamiliarWord(wlist, properNounMethod, true), m_beginHighlight(beginHighlight),
          m_endHighlight(endHighlight)
        {
        }

    /// @private
    IsNotFamiliarWordWithHighlighting() = delete;

    /// @returns @c true if word is not familiar.
    /// @param the_word The word to review.
    [[nodiscard]]
    bool
    operator()(const word_typeT& the_word) const
        {
        return !m_is_FamiliarWord(the_word);
        }

    /// @returns The tag opening a highlighted section.
    [[nodiscard]]
    const wxString& GetHightlightBegin() const noexcept
        {
        return m_beginHighlight;
        }

    /// @returns The tag closing a highlighted section.
    [[nodiscard]]
    const wxString& GetHightlightEnd() const noexcept
        {
        return m_endHighlight;
        }

    /// @brief Resets the list of encountered proper nouns.
    void Reset() { m_is_FamiliarWord.clear_encountered_proper_nouns(); }

  private:
    readability::is_familiar_word<word_typeT, wordlistT, stemmerT> m_is_FamiliarWord;
    wxString m_beginHighlight;
    wxString m_endHighlight;
    };

/** @brief Determinant for whether a word is UNfamiliar,
        used for highlighting unfamiliar words in a document.
    @details Will also mark numbers as excluded (some tests forcibly ignore all numbers).*/
template<typename word_typeT, typename wordlistT, typename stemmerT>
class IsNotFamiliarWordExcludeNumeralsWithHighlighting
    {
  public:
    /** @brief Constructor.
        @param wlist the list of familiar words.
        @param beginHighlight the beginning tag of a highlight
            (can be HTML or RTF code, for example).
        @param endHighlight the ending tag of a highlight
            (can be HTML or RTF code, for example).
        @param beginExcludeHighlight the beginning tag of an excluded text highlight
            (can be HTML or RTF code, for example).
        @param endExcludeHighlight the ending tag of an excluded text highlight
            (can be HTML or RTF code, for example).
        @param properNounMethod for determining whether a proper noun (not on the word list)
            should be familiar or not.*/
    IsNotFamiliarWordExcludeNumeralsWithHighlighting(
        const wordlistT* wlist, const wxString& beginHighlight, const wxString& endHighlight,
        const wxString& beginExcludeHighlight, const wxString& endExcludeHighlight,
        readability::proper_noun_counting_method properNounMethod)
        : m_isFamiliarWord(wlist, properNounMethod, true), m_beginHighlight(beginHighlight),
          m_endHighlight(endHighlight), m_beginExcludeHighlight(beginExcludeHighlight),
          m_endExcludeHighlight(endExcludeHighlight)
        {
        }

    /// @private
    IsNotFamiliarWordExcludeNumeralsWithHighlighting() = delete;

    /// @returns @c true if word is not familiar.
    /// @param the_word The word to review.
    [[nodiscard]]
    bool
    operator()(const word_typeT& the_word) const
        {
        // reset
        m_inExcludeState = false;
        if (the_word.is_numeric())
            {
            m_inExcludeState = true;
            return true;
            }
        return !m_isFamiliarWord(the_word);
        }

    /// @returns The tag opening a highlighted section.
    [[nodiscard]]
    const wxString& GetHightlightBegin() const noexcept
        {
        return m_inExcludeState ? m_beginExcludeHighlight : m_beginHighlight;
        }

    /// @returns The tag closing a highlighted section.
    [[nodiscard]]
    const wxString& GetHightlightEnd() const noexcept
        {
        return m_inExcludeState ? m_endExcludeHighlight : m_endHighlight;
        }

    /// @brief Resets the previously encountered proper nouns.
    void Reset() { m_isFamiliarWord.clear_encountered_proper_nouns(); }

  private:
    readability::is_familiar_word<word_typeT, wordlistT, stemmerT> m_isFamiliarWord;
    wxString m_beginHighlight;
    wxString m_endHighlight;
    wxString m_beginExcludeHighlight;
    wxString m_endExcludeHighlight;
    mutable bool m_inExcludeState{ false };
    };

template<typename word_typeT>
class IsNeverFamiliarWordWithHighlighting final
    : public readability::is_never_familiar_word<word_typeT>
    {
  public:
    /// @brief Constructor.
    IsNeverFamiliarWordWithHighlighting() = default;

    /// @returns The tag opening a highlighted section.
    [[nodiscard]]
    const wxString& GetHightlightBegin() const noexcept
        {
        return m_emptyStr;
        }

    /// @returns The tag closing a highlighted section.
    [[nodiscard]]
    const wxString& GetHightlightEnd() const noexcept
        {
        return m_emptyStr;
        }

  private:
    wxString m_emptyStr{};
    };

/** @brief Functor used for the text highlighted windows.
    @details Specifically, highlights words that are Dolch words.*/
template<typename word_typeT>
class IsDolchWordWithLevelHighlighting final : public readability::is_dolch_word<word_typeT>
    {
  public:
    IsDolchWordWithLevelHighlighting(const readability::dolch_word_list* wlist,
                                     const wxString& beginConjunctions,
                                     const wxString& beginPrepositions,
                                     const wxString& beginPronouns, const wxString& beginAdverbs,
                                     const wxString& beginAdjectives, const wxString& beginVerbs,
                                     const wxString& beginNouns, const wxString& endHighlight)
        : readability::is_dolch_word<word_typeT>(wlist), m_beginConjunctions(beginConjunctions),
          m_beginPrepositions(beginPrepositions), m_beginPronouns(beginPronouns),
          m_beginAdverbs(beginAdverbs), m_beginAdjectives(beginAdjectives),
          m_beginVerbs(beginVerbs), m_beginNouns(beginNouns), m_endHighlight(endHighlight)
        {
        }

    /// @private
    IsDolchWordWithLevelHighlighting() = delete;

    /// @returns The tag opening a highlighted section.
    [[nodiscard]]
    const wxString& GetHightlightBegin() const
        {
        if (readability::is_dolch_word<word_typeT>::get_last_search_result() !=
            readability::is_dolch_word<word_typeT>::get_word_list()->get_words().end())
            {
            switch (readability::is_dolch_word<word_typeT>::get_last_search_result()->get_type())
                {
            case readability::sight_word_type::conjunction:
                return m_beginConjunctions;
            case readability::sight_word_type::preposition:
                return m_beginPrepositions;
            case readability::sight_word_type::pronoun:
                return m_beginPronouns;
            case readability::sight_word_type::adverb:
                return m_beginAdverbs;
            case readability::sight_word_type::adjective:
                return m_beginAdjectives;
            case readability::sight_word_type::verb:
                return m_beginVerbs;
            case readability::sight_word_type::noun:
                return m_beginNouns;
            default:
                return m_emptyStr;
                }
            }
        else
            {
            return m_emptyStr;
            }
        }

    /// @returns The tag closing a highlighted section.\n
    ///     If the current word being highlighted had highlighting
    ///     turned off then just return blank
    [[nodiscard]]
    const wxString& GetHightlightEnd() const
        {
        return (GetHightlightBegin().empty()) ? m_emptyStr : m_endHighlight;
        }

    /// @internal No-op, just needed for interface contract.
    /// @private
    // cppcheck-suppress functionStatic
    void Reset() {}

  private:
    wxString m_beginConjunctions;
    wxString m_beginPrepositions;
    wxString m_beginPronouns;
    wxString m_beginAdverbs;
    wxString m_beginAdjectives;
    wxString m_beginVerbs;
    wxString m_beginNouns;
    wxString m_endHighlight;
    wxString m_emptyStr{};
    };

/** @brief Functor used for the text highlighted windows.
    @details Specifically, highlights words that are not Dolch words.*/
template<typename word_typeT>
class IsNotDolchWordWithLevelHighlighting
    {
  public:
    IsNotDolchWordWithLevelHighlighting<word_typeT>(const readability::dolch_word_list* wlist,
                                                    const wxString& beginHighlight,
                                                    const wxString& endHighlight)
        : isDolch(wlist), m_beginHighlight(beginHighlight), m_endHighlight(endHighlight)
        {
        }

    /// @private
    IsNotDolchWordWithLevelHighlighting() = delete;

    /// @returns @c true if word is not Dolch familiar.
    /// @param the_word The word to review.
    [[nodiscard]]
    bool
    operator()(const word_typeT& the_word) const
        {
        return !isDolch(the_word);
        }

    /// @returns The tag opening a highlighted section.
    [[nodiscard]]
    const wxString& GetHightlightBegin() const noexcept
        {
        return m_beginHighlight;
        }

    /// @returns The tag closing a highlighted section.
    [[nodiscard]]
    const wxString& GetHightlightEnd() const noexcept
        {
        return m_endHighlight;
        }

    /// @internal No-op, just needed for interface contract.
    /// @private
    // cppcheck-suppress functionStatic
    void Reset() {}

  private:
    readability::is_dolch_word<word_typeT> isDolch;
    wxString m_beginHighlight;
    wxString m_endHighlight;
    };

/** @brief Functor used for the text highlighted windows.
    @details Specifically, highlights words that are not familiar
        to custom tests.*/
template<typename CustomReadabilityTestInterfaceT>
class IsNotCustomFamiliarWordWithHighlighting
    {
  public:
    IsNotCustomFamiliarWordWithHighlighting(CustomReadabilityTestInterfaceT wordTest,
                                            const wxString& beginHighlight,
                                            const wxString& endHighlight)
        : m_wordTest(wordTest), m_beginHighlight(beginHighlight), m_endHighlight(endHighlight)
        {
        }

    /// @returns @c true if word is not familiar.
    /// @param the_word The word to review.
    [[nodiscard]]
    inline bool
    operator()(const word_case_insensitive_no_stem& the_word) const
        {
        return !m_wordTest->operator()(the_word);
        }

    /// @returns The tag opening a highlighted section.
    [[nodiscard]]
    const wxString& GetHightlightBegin() const noexcept
        {
        return m_beginHighlight;
        }

    /// @returns The tag closing a highlighted section.
    [[nodiscard]]
    const wxString& GetHightlightEnd() const noexcept
        {
        return m_endHighlight;
        }

    /// @brief Clears the cached first occurrences of proper nouns.
    void Reset() { m_wordTest->ResetIterator(); }

  private:
    CustomReadabilityTestInterfaceT m_wordTest;
    wxString m_beginHighlight;
    wxString m_endHighlight;
    };

/** @brief Functor used for the text highlighted windows.
    @details Specifically, highlights words that are not familiar
        to custom tests (excluding numerals).*/
template<typename CustomReadabilityTestInterfaceT>
class IsNotCustomFamiliarWordExcludeNumeralsWithHighlighting
    {
  public:
    IsNotCustomFamiliarWordExcludeNumeralsWithHighlighting(CustomReadabilityTestInterfaceT wordTest,
                                                           const wxString& beginHighlight,
                                                           const wxString& endHighlight,
                                                           const wxString& beginExcludeHighlight,
                                                           const wxString& endExcludeHighlight)
        : m_wordTest(wordTest), m_beginHighlight(beginHighlight), m_endHighlight(endHighlight),
          m_beginExcludeHighlight(beginExcludeHighlight), m_endExcludeHighlight(endExcludeHighlight)
        {
        }

    /// @returns @c true if word is not familiar.
    /// @param the_word The word to review.
    [[nodiscard]]
    inline bool
    operator()(const word_case_insensitive_no_stem& the_word) const
        {
        // reset
        m_inExcludeState = false;
        if (the_word.is_numeric())
            {
            m_inExcludeState = true;
            return true;
            }
        return !m_wordTest->operator()(the_word);
        }

    /// @returns The tag opening a highlighted section.
    [[nodiscard]]
    const wxString& GetHightlightBegin() const noexcept
        {
        return m_inExcludeState ? m_beginExcludeHighlight : m_beginHighlight;
        }

    /// @returns The tag closing a highlighted section.
    [[nodiscard]]
    const wxString& GetHightlightEnd() const noexcept
        {
        return m_inExcludeState ? m_endExcludeHighlight : m_endHighlight;
        }

    /// @brief Resests the internal iterators.
    void Reset() { m_wordTest->ResetIterator(); }

  private:
    CustomReadabilityTestInterfaceT m_wordTest;
    wxString m_beginHighlight;
    wxString m_endHighlight;
    wxString m_beginExcludeHighlight;
    wxString m_endExcludeHighlight;
    mutable bool m_inExcludeState{ false };
    };

    /** @}*/

#endif // __TESTS_FUNCTIONAL_H__
