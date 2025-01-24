/** @addtogroup Indexing
    @brief Classes for parsing and indexing text.
    @date 2005-2023
    @copyright Oleander Software, Ltd.
    @author Blake Madden
    @details This program is free software; you can redistribute it and/or modify
     it under the terms of the 3-Clause BSD License.

     SPDX-License-Identifier: BSD-3-Clause
* @{*/

#ifndef __CHARACTER_TRAITS_H__
#define __CHARACTER_TRAITS_H__

#include "../Wisteria-Dataviz/src/util/string_util.h"
#include "characters.h"
#include <cassert>
#include <string>

/// @brief String traits use special comparison logic.
namespace traits
    {
    /** @brief Case-insensitive comparison traits for std::basic_string<>, which also has special
            logic for comparing apostrophes and full-width characters.
        @details Also includes helper functions for case-sensitive comparison which includes the
            additional full-width and apostrophe logic.*/
    class case_insensitive_ex : public std::char_traits<wchar_t>
        {
      public:
        //-------------------------------------------------------------
        static constexpr bool eq_int_type(const int_type& i1, const int_type& i2) noexcept
            {
            return tolower(i1) == tolower(i2);
            }

        //-------------------------------------------------------------
        static constexpr char_type to_char_type(const int_type& i) noexcept
            {
            return static_cast<char_type>(i);
            }

        //-------------------------------------------------------------
        static constexpr int_type to_int_type(const char_type& c) noexcept
            {
            return static_cast<unsigned char>(c);
            }

        //-------------------------------------------------------------
        inline static bool eq(const char_type& first, const char_type& second) noexcept
            {
            // special logic for apostrophes
            if (characters::is_character::is_apostrophe(first) &&
                characters::is_character::is_apostrophe(second))
                {
                return true;
                }
            return (tolower(first) == tolower(second));
            }

        /// @todo Unit test
        //-------------------------------------------------------------
        inline static constexpr bool eq_case_sensitive(const char_type& first,
                                                       const char_type& second) noexcept
            {
            // special logic for apostrophes
            if (characters::is_character::is_apostrophe(first) &&
                characters::is_character::is_apostrophe(second))
                {
                return true;
                }
            return (string_util::full_width_to_narrow(first) ==
                    string_util::full_width_to_narrow(second));
            }

        //-------------------------------------------------------------
        inline static bool lt(const char_type& first, const char_type& second) noexcept
            {
            // special logic for apostrophes
            if (characters::is_character::is_apostrophe(first) &&
                characters::is_character::is_apostrophe(second))
                {
                return false;
                }
            else if (characters::is_character::is_apostrophe(first))
                {
                return (L'\'' < tolower(second));
                }
            else if (characters::is_character::is_apostrophe(second))
                {
                return (tolower(first) < L'\'');
                }
            return (tolower(first) < tolower(second));
            }

        /// @todo unit test!
        //-------------------------------------------------------------
        inline static bool lt_case_sensitive(const char_type& first,
                                             const char_type& second) noexcept
            {
            // special logic for apostrophes
            if (characters::is_character::is_apostrophe(first) &&
                characters::is_character::is_apostrophe(second))
                {
                return false;
                }
            else if (characters::is_character::is_apostrophe(first))
                {
                return (L'\'' < string_util::full_width_to_narrow(second));
                }
            else if (characters::is_character::is_apostrophe(second))
                {
                return (string_util::full_width_to_narrow(first) < L'\'');
                }
            return (string_util::full_width_to_narrow(first) <
                    string_util::full_width_to_narrow(second));
            }

        /// @todo Unit test
        //-------------------------------------------------------------
        inline static bool le(const char_type& first, const char_type& second) noexcept
            {
            // special logic for apostrophes
            if (characters::is_character::is_apostrophe(first) &&
                characters::is_character::is_apostrophe(second))
                {
                return false;
                }
            else if (characters::is_character::is_apostrophe(first))
                {
                return (L'\'' <= tolower(second));
                }
            else if (characters::is_character::is_apostrophe(second))
                {
                return (tolower(first) <= L'\'');
                }
            return (tolower(first) <= tolower(second));
            }

        /// @todo Unit test
        //-------------------------------------------------------------
        inline static bool ge(const char_type& first, const char_type& second) noexcept
            {
            // special logic for apostrophes
            if (characters::is_character::is_apostrophe(first) &&
                characters::is_character::is_apostrophe(second))
                {
                return false;
                }
            else if (characters::is_character::is_apostrophe(first))
                {
                return (L'\'' >= tolower(second));
                }
            else if (characters::is_character::is_apostrophe(second))
                {
                return (tolower(first) >= L'\'');
                }
            return (tolower(first) >= tolower(second));
            }

        //-------------------------------------------------------------
        static constexpr char_type tolower(const char_type& ch) noexcept
            {
            return characters::is_character::to_lower(string_util::full_width_to_narrow(ch));
            }

        //-------------------------------------------------------------
        static int compare(const char_type* s1, const char_type* s2, size_t n) noexcept
            {
            assert(s1);
            assert(s2);
            if (s1 == nullptr)
                {
                return -1;
                }
            else if (s2 == nullptr)
                {
                return 1;
                }
            for (size_t i = 0; i < n; ++i)
                {
                // cppcheck-suppress arrayIndexOutOfBounds
                if (!eq(s1[i], s2[i]))
                    {
                    return lt(s1[i], s2[i]) ? -1 : 1;
                    }
                }
            return 0;
            }

        /// @todo unit test!
        //-------------------------------------------------------------
        static int compare_case_sensitive(const char_type* s1, const char_type* s2,
                                          size_t n) noexcept
            {
            assert(s1);
            assert(s2);
            if (s1 == nullptr)
                {
                return -1;
                }
            else if (s2 == nullptr)
                {
                return 1;
                }
            for (size_t i = 0; i < n; ++i)
                {
                if (!eq_case_sensitive(s1[i], s2[i]))
                    {
                    return lt_case_sensitive(s1[i], s2[i]) ? -1 : 1;
                    }
                }
            return 0;
            }

        //-------------------------------------------------------------
        static const char_type* find(const char_type* s1, size_t n, const char_type ch) noexcept
            {
            assert(s1);
            if (s1 == nullptr)
                {
                return nullptr;
                }
            for (size_t i = 0; i < n; ++i)
                {
                if (eq(s1[i], ch))
                    {
                    return s1 + i;
                    }
                }
            return nullptr;
            }

        //-------------------------------------------------------------
        static const char_type* find(const char_type* s1, size_t n1, const char_type* s2,
                                     size_t n2) noexcept
            {
            assert(n1 && n2);
            assert(s1);
            assert(s2);
            if (s1 == nullptr || s2 == nullptr || (n2 > n1))
                {
                return nullptr;
                }
            size_t j = 1;
            for (size_t i = 0; i < n1; i += j)
                {
                // if the first character of the substring matches then start comparing
                if (eq(s1[i], s2[0]))
                    {
                    // if only looking for one character then return
                    if (n2 == 1)
                        {
                        return s1 + i;
                        }
                    // already know the first chars match, so start at next one
                    for (j = 1; j < n2; ++j)
                        {
                        if (!eq(s1[i + j], s2[j]))
                            {
                            break;
                            }
                        }
                    // if every character matched then return it
                    if (n2 == j)
                        {
                        return s1 + i;
                        }
                    }
                }
            return nullptr;
            }
        };

    using case_insensitive_wstring_ex = std::basic_string<wchar_t, traits::case_insensitive_ex>;
    } // namespace traits

/** @}*/

#endif //__CHARACTER_TRAITS_H__
