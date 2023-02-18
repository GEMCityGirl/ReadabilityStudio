/** @addtogroup Readability
    @brief Classes for readability tests.
    @date 2004-2020
    @copyright Oleander Software, Ltd.
    @author Blake Madden
    @details This program is free software; you can redistribute it and/or modify
     it under the terms of the 3-Clause BSD License.

     SPDX-License-Identifier: BSD-3-Clause
* @{*/

#ifndef __READENUMS_H__
#define __READENUMS_H__

namespace readability
    {
    /** Methods for how familiar word analysis can handle proper nouns.*/
    enum class proper_noun_counting_method
        {
        all_proper_nouns_are_unfamiliar, /*!< Proper nouns are always be unfamiliar (unless explicitly on a familiar word list). */
        all_proper_nouns_are_familiar, /*!< Proper nouns are always familiar. */
        only_count_first_instance_of_proper_noun_as_unfamiliar, /*!< Only the first instance of a proper noun is unfamiliar (if not on a familiar word list). All subsequent instances of this word will be familiar. */
        PROPERNOUNCOUNTINGMETHOD_COUNT
        };
    }

/** @} */

#endif //__READENUMS_H__
