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

#ifndef __READENUMS_H__
#define __READENUMS_H__

namespace readability
    {
    /** @brief Methods for how familiar word analysis can handle proper nouns.*/
    enum class proper_noun_counting_method
        {
        /// @brief Proper nouns are always be unfamiliar
        ///     (unless explicitly on a familiar word list).
        all_proper_nouns_are_unfamiliar,
        /// @brief Proper nouns are always familiar.
        all_proper_nouns_are_familiar,
        /// @brief Only the first instance of a proper noun is unfamiliar
        ///     (if not on a familiar word list).
        ///     All subsequent instances of this word will be familiar. */
        only_count_first_instance_of_proper_noun_as_unfamiliar,
        /// @private
        PROPERNOUNCOUNTINGMETHOD_COUNT
        };
    } // namespace readability

#endif //__READENUMS_H__
