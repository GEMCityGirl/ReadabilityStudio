/** @addtogroup Indexing
    @brief Classes for parsing and indexing text.
    @date 2003-2020
    @copyright Oleander Software, Ltd.
    @author Blake Madden
    @details This program is free software; you can redistribute it and/or modify
     it under the terms of the 3-Clause BSD License.

     SPDX-License-Identifier: BSD-3-Clause
* @{*/

#ifndef __STOP_WORD_LIST_H__
#define __STOP_WORD_LIST_H__

#include "word_list.h"

namespace grammar
    {
    /// @brief Determines if a possible proper word is really a simple word that cannot be proper (e.g., "and").
    /// This is a user-defined list that caller needs to load.
    class is_non_proper_word
        {
    public:
        /** Accesses the list of words following an 'an' that would be correct (that the analyzer might get wrong).*/
        [[nodiscard]] static word_list& get_word_list() noexcept
            { return m_word_list; }
    private:
        static word_list m_word_list;
        };
    }

/** @}*/

#endif //__STOP_WORD_LIST_H__
