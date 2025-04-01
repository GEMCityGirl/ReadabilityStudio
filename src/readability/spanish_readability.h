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

#ifndef __SPANISH_READABILITY_H__
#define __SPANISH_READABILITY_H__

#include "../Wisteria-Dataviz/src/math/mathematics.h"
#include "english_readability.h"
#include "grade_scales.h"
#include "readability_enums.h"
#include <algorithm>
#include <cmath>
#include <functional>
#include <stdexcept>

namespace readability
    {
    /** @brief Crawford Spanish grade level test.
        @details This test is meant for primary-age reading materials.
            There is also a Crawford graph, which basically just plots
            the score where the factors' values intersect.
        @param number_of_words The number of words.
        @param number_of_syllables The number of syllables.
        @param number_of_sentences The number of sentences.
        @returns The grade-level score.
        @throws std::domain_error If @c number_of_words is @c 0,
                throws an exception.*/
    [[nodiscard]]
    inline double crawford(const uint32_t number_of_words, const uint32_t number_of_syllables,
                           const uint32_t number_of_sentences)
        {
        if (number_of_words == 0)
            {
            throw std::domain_error("invalid word count");
            }
        const double normalizationFactor = safe_divide<double>(100, number_of_words);

        const double result = (-.205 * (number_of_sentences * normalizationFactor)) +
                              (.049 * (number_of_syllables * normalizationFactor)) - 3.407;
        return truncate_k12_plus_grade(result);
        }

    /** @brief Spanish SMOG test.
        @details This test calculates a (high-precision) SMOG score from Spanish text
            and then adjusts to the appropriate grade level.\n
            This test explicitly excludes lists, but includes headers and footers
            (SMOG does not explicitly state this).\n
            Numbers should be sounded out, just like in English SMOG.
        @param number_of_big_words The number of 3+ syllable words.
        @param number_of_sentences The number of sentences.
        @returns The grade-level score.
        @note Number of sentences is not a part of the SMOG formulas because they
            expect 10-sentence samples. @c number_of_sentences is therefore used to
            standardize @c number_of_big_words to what it would be if it came
            from a 10-sentence sample.
        @throws std::domain_error If @c number_of_sentences is @c 0,
                throws an exception.*/
    [[nodiscard]]
    inline double sol_spanish(const uint32_t number_of_big_words,
                              const uint32_t number_of_sentences)
        {
        if (number_of_sentences == 0)
            {
            throw std::domain_error("invalid sentence count.");
            }
        // get the raw SMOG grade score (which can be high [e.g., 25]) then plug into the
        // Spanish -> English grade level formula (below)...
        const double solScore = -2.51 + .74 * smog(number_of_big_words, number_of_sentences, false);
        // ...then clip to the regular 0-19 grade level range
        return truncate_k12_plus_grade(solScore);
        }
    } // namespace readability

#endif //__SPANISH_READABILITY_H__
