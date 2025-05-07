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

#ifndef __OPTION_ENUMS_H__
#define __OPTION_ENUMS_H__

#include <wx/language.h>

// helper enumerations
//--------------------------------------------------
enum class UiLanguage
    {
    Default,
    English,
    Spanish
    };

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

/// @brief How text was retrieved from the project wizard.
enum class TextSource
    {
    FromFile,
    EnteredText
    };

/// @brief How the document's text is stored in the project.
/// @note EmbedText can refer to text that was entered by the user in the project
///     wizard, or from a file. NoEmbedText (aliased to LoadFromExternalDocument)
///     will always relate to an external file.
enum class TextStorage
    {
    EmbedText,
    NoEmbedText,
    LoadFromExternalDocument = NoEmbedText
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

#endif //__OPTION_ENUMS_H__
