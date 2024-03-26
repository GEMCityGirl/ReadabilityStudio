/** @addtogroup Indexing
    @brief Classes for parsing and indexing text.
    @date 2005-2023
    @copyright Oleander Software, Ltd.
    @author Blake Madden
    @details This program is free software; you can redistribute it and/or modify
     it under the terms of the 3-Clause BSD License.

     SPDX-License-Identifier: BSD-3-Clause
* @{*/

#ifndef __CHARACTER_SIMPLIFY_H__
#define __CHARACTER_SIMPLIFY_H__

#include "character_traits.h"
#include "characters.h"
#include "sentence.h"
#include <map>

namespace text_transform
    {
    /// @brief Lookup table for romanization.
    class romanization_conversion_table
        {
      public:
        romanization_conversion_table();

        [[nodiscard]]
        const std::map<wchar_t, std::wstring>& get_table() const noexcept
            {
            return m_replacements;
            }

      private:
        std::map<wchar_t, std::wstring> m_replacements;
        };

    /** @brief Class to encode a string into Romanized text.

        This includes replacing "fancy" punctuation and accented characters,
        such as changing smart quotes to straight quotes, German eszetts to 'ss,' etc.*/
    class romanize
        {
      public:
        romanize() noexcept
            : isEndOfSentence(
                  true /* used for ellipses, this says that sentences must be uppercased. */)
            {
            }

        /** @brief Encodes a string into Romanized text.
            @param text The text to encode.
            @param replace_extended_ascii_characters Whether to replace accented letters
                and other special symbols.
            @param remove_ellipses Whether ellipses should be replaced with spaces.
                If an ellipsis represents the end of a sentence,
                then it will be replaced with a period.
            @param remove_bullets Whether to remove bullets and list-item numbers.
            @param narrow_full_width_characters Whether to convert full-width characters
                to their narrow counterparts.
            @returns A string encoded to simpler text.*/
        [[nodiscard]]
        std::wstring
        operator()(std::wstring_view text, const bool replace_extended_ascii_characters,
                   const bool remove_ellipses, const bool remove_bullets,
                   const bool narrow_full_width_characters) const;

      private:
        static const romanization_conversion_table m_conversionTable;
        grammar::is_end_of_sentence isEndOfSentence;
        grammar::is_bulleted_text isBullet;
        };
    } // namespace text_transform

/** @} */

#endif //__CHARACTER_SIMPLIFY_H__
