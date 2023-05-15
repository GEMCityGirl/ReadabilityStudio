/** @addtogroup ReadabilityStudio
    @brief Classes for main Readability Studio application.
    @date 2005-2023
    @copyright Oleander Software, Ltd.
    @author Blake Madden
    @details This program is free software; you can redistribute it and/or modify
     it under the terms of the 3-Clause BSD License.

     SPDX-License-Identifier: BSD-3-Clause
* @{*/

#ifndef __OPTION_ENUMS_H__
#define __OPTION_ENUMS_H__

// helper enumerations
//--------------------------------------------------
enum class InvalidTextFilterFormat
    {
    IncludeOnlyValidText,
    IncludeOnlyInvalidText,
    IncludeAllText
    };

enum class TextHighlight
    {
    HighlightBackground,
    HighlightForeground
    };

enum class TextSource
    {
    FromFile,
    EnteredText
    };

enum class TextStorage
    {
    EmbedText,
    NoEmbedText
    };

enum class LongSentence
    {
    LongerThanSpecifiedLength,
    OutlierLength
    };

enum class NumeralSyllabize
    {
    WholeWordIsOneSyllable,
    SoundOutEachDigit
    };

enum class FleschNumeralSyllabize
    {
    NumeralIsOneSyllable,
    NumeralUseSystemDefault
    };

enum class FleschKincaidNumeralSyllabize
    {
    FleschKincaidNumeralSoundOutEachDigit,
    FleschKincaidNumeralUseSystemDefault
    };

enum class ParagraphParse
    {
    OnlySentenceTerminatedNewLinesAreParagraphs,
    EachNewLineIsAParagraph
    };

enum class InvalidSentence
    {
    ExcludeFromAnalysis,
    IncludeAsFullSentences,
    ExcludeExceptForHeadings,
    INVALID_SENTENCE_METHOD_COUNT
    };

enum class TestRecommendation
    {
    BasedOnDocumentType,
    BasedOnIndustry,
    ManuallySelectTests,
    UseBundle,
    TEST_RECOMMENDATION_COUNT
    };

enum class SpecializedTestTextExclusion
    {
    ExcludeIncompleteSentencesExceptHeadings,
    UseSystemDefault,
    SPECIALIZED_TEST_TEXT_EXCLUSION_COUNT
    };

enum class VarianceMethod
    {
    SampleVariance,
    PopulationVariance
    };

/** @}*/

#endif //__OPTION_ENUMS_H__