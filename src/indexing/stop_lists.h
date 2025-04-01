/********************************************************************************
 * Copyright (c) 2005-2025 Blake Madden
 *
 * This program and the accompanying materials are made available under the
 * terms of the Eclipse Public License 2.0 which is available at
 * https://www.eclipse.org/legal/epl-2.0.
 *
 * SPDX-License-Identifier: EPL-2.0
 *
 * Contributors:
 *   Blake Madden - initial implementation
 ********************************************************************************/

#ifndef __STOP_WORD_LIST_H__
#define __STOP_WORD_LIST_H__

#include "word_list.h"

namespace grammar
    {
    /// @brief Determines if a possible proper word is really a simple word
    ///     that cannot be proper (e.g., "and").
    /// @details This is a user-defined list that caller needs to load.
    class is_non_proper_word
        {
      public:
        /** @returns The list of words following an 'an' that would be correct
                (that the analyzer might get wrong). This accesses the list so
                that you can edit or use it.*/
        [[nodiscard]]
        static word_list& get_word_list() noexcept
            {
            return m_word_list;
            }

      private:
        static word_list m_word_list;
        };
    } // namespace grammar

#endif //__STOP_WORD_LIST_H__
