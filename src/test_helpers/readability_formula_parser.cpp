///////////////////////////////////////////////////////////////////////////////
// Name:        readability_formula_parser.cpp
// Author:      Blake Madden
// Copyright:   (c) 2005-2023 Blake Madden
// Licence:     3-Clause BSD licence
// SPDX-License-Identifier: BSD-3-Clause
///////////////////////////////////////////////////////////////////////////////

#include "readability_formula_parser.h"
#include "../projects/base_project.h"

/// @returns The total number of numerals from the document.
/// @param context The TinyEpr++ expression object.
[[nodiscard]]
static double NumeralCount(const te_expr* context)
    {
    return (dynamic_cast<const FormulaProject*>(context))->GetProject()->GetTotalNumerals();
    }

/// @returns The number of unique words from the document.
/// @param context The TinyEpr++ expression object.
[[nodiscard]]
static double UniqueWordCount(const te_expr* context)
    {
    return (dynamic_cast<const FormulaProject*>(context))->GetProject()->GetTotalUniqueWords();
    }

/// @returns The total number of unfamiliar Spache words from the document.
/// @param context The TinyEpr++ expression object.
[[nodiscard]]
static double UnfamiliarSpacheWordCount(const te_expr* context)
    {
    return (dynamic_cast<const FormulaProject*>(context))->GetProject()->GetTotalHardWordsSpache();
    }

/// @returns The total number of unique unfamiliar Spache words from the document.
/// @param context The TinyEpr++ expression object.
[[nodiscard]]
static double UniqueUnfamiliarSpacheWordCount(const te_expr* context)
    {
    return (dynamic_cast<const FormulaProject*>(context))->GetProject()->GetTotalUniqueHardWordsSpache();
    }

/// @returns The total number of familiar Spache words from the document.
/// @param context The TinyEpr++ expression object.
[[nodiscard]]
static double FamiliarSpacheWordCount(const te_expr* context)
    {
    const BaseProject* project = (dynamic_cast<const FormulaProject*>(context))->GetProject();
    return project->GetTotalWords() - project->GetTotalHardWordsSpache();
    }

/// @returns The total number of words consisting of six or more character from the document.
/// @param context The TinyEpr++ expression object.
[[nodiscard]]
static double SixCharacterPlusWordCount(const te_expr* context)
    {
    return (dynamic_cast<const FormulaProject*>(context))->GetProject()->GetTotalLongWords();
    }

/// @returns The number of unique words consisting of six or more character from the document.
/// @param context The TinyEpr++ expression object.
[[nodiscard]]
static double UniqueSixCharacterPlusWordCount(const te_expr* context)
    {
    return (dynamic_cast<const FormulaProject*>(context))->GetProject()->GetTotalUnique6CharsPlusWords();
    }

/// @returns The total number of words consisting of seven or more character from the document.
/// @param context The TinyEpr++ expression object.
[[nodiscard]]
static double SevenCharacterPlusWordCount(const te_expr* context)
    {
    return (dynamic_cast<const FormulaProject*>(context))->GetProject()->GetTotalHardLixRixWords();
    }

/// @returns The total number of miniwords from the document.
/// @param context The TinyEpr++ expression object.
[[nodiscard]]
static double MiniWordCount(const te_expr* context)
    {
    return (dynamic_cast<const FormulaProject*>(context))->GetProject()->GetTotalMiniWords();
    }

/// @returns The total number of hard Fog words from the document.
/// @param context The TinyEpr++ expression object.
[[nodiscard]]
static double HardFogWordCount(const te_expr* context)
    {
    return (dynamic_cast<const FormulaProject*>(context))->GetProject()->GetTotalHardWordsFog();
    }

/// @returns The total number of unfamiliar Dale-Chall words from the document.
/// @param context The TinyEpr++ expression object.
[[nodiscard]]
static double UnfamiliarDaleChallWordCount(const te_expr* context)
    {
    return (dynamic_cast<const FormulaProject*>(context))->GetProject()->GetTotalHardWordsDaleChall();
    }

/// @returns The total number of unique unfamiliar Dale-Chall words from the document.
/// @param context The TinyEpr++ expression object.
[[nodiscard]]
static double UniqueUnfamiliarDaleChallWordCount(const te_expr* context)
    {
    return (dynamic_cast<const FormulaProject*>(context))->GetProject()->GetTotalUniqueDCHardWords();
    }

/// Performs a New Dale-Chall test with a custom familiar word list.
/// @param context The TinyEpr++ expression object.
[[nodiscard]]
static double CustomNewDaleChall(const te_expr* context)
    {
    const BaseProject* project = (dynamic_cast<const FormulaProject*>(context))->GetProject();
    const wxString testName = project->GetCurrentCustomTest();
    if (!project->HasCustomTest(testName))
        { throw std::exception(_(L"Internal error: unable to find custom test by name.")); }
    if (!project->GetCustomTest(testName)->GetIterator()->is_using_familiar_words())
        {
        throw std::exception(
            _(L"Test has not defined what an unfamiliar word is. "
               "Custom unfamiliar word test cannot be calculated."));
        }
    if (project->GetProjectLanguage() != readability::test_language::english_test)
        {
        throw std::exception(
            wxString::Format(_(L"%s function can only be used for English projects."),
                ReadabilityFormulaParser::GetCustomNewDaleChallSignature()));
        }
    // lowest grade for DC
    double grade_value = 0;
    try
        {
        size_t gradeBegin = 0, gradeEnd = 0;
        // use whichever calculation regular DC is using
        if (project->GetDaleChallTextExclusionMode() ==
            SpecializedTestTextExclusion::ExcludeIncompleteSentencesExceptHeadings)
            {
            readability::new_dale_chall(gradeBegin, gradeEnd,
                        project->GetTotalWordsFromCompleteSentencesAndHeaders(),
                        project->GetCustomTest(testName)->GetUnfamiliarWordCount(),
                        project->GetTotalSentencesFromCompleteSentencesAndHeaders() );
            }
        else
            {
            readability::new_dale_chall(gradeBegin, gradeEnd,
                        project->GetTotalWords(),
                        project->GetCustomTest(testName)->GetUnfamiliarWordCount(),
                        project->GetTotalSentences() );
            }
        // Grade range will be combined into a single double value.
        // Client will need to split this value
        grade_value = static_cast<double>(join_int32s(static_cast<uint32_t>(gradeBegin),
                                                      static_cast<uint32_t>(gradeEnd)));
        }
    catch (std::domain_error)
        { throw std::exception(_(L"Unable to calculate custom New Dale Chall.")); }
    return grade_value;
    }

/// Performs a Spache Revised test with a custom familiar word list.
/// @param context The TinyEpr++ expression object.
[[nodiscard]]
static double CustomSpache(const te_expr* context)
    {
    const BaseProject* project = (dynamic_cast<const FormulaProject*>(context))->GetProject();
    const wxString testName = project->GetCurrentCustomTest();
    if (!project->HasCustomTest(testName))
        { throw std::exception(_(L"Internal error: unable to find custom test by name.")); }
    if (!project->GetCustomTest(testName)->GetIterator()->is_using_familiar_words())
        {
        throw std::exception(
            _(L"Test has not defined what an unfamiliar word is. "
               "Custom unfamiliar word test cannot be calculated."));
        }
    if (project->GetProjectLanguage() != readability::test_language::english_test)
        {
        throw std::exception(
            wxString::Format(_(L"%s function can only be used for English projects."),
            ReadabilityFormulaParser::GetCustomSpacheSignature()));
        }
    // lowest grade for Spache
    double grade_value = 0;
    try
        {
        grade_value = readability::spache(project->GetTotalWords(),
                project->GetCustomTest(testName)->GetUniqueUnfamiliarWordCount(),
                project->GetTotalSentences() );
        }
    catch (std::domain_error)
        { throw std::exception(_(L"Unable to calculate custom Spache.")); }
    return grade_value;
    }

/// Performs a Harris-Jacobson test with a custom familiar word list.
/// @param context The TinyEpr++ expression object.
[[nodiscard]]
static double CustomHarrisJacobson(const te_expr* context)
    {
    const BaseProject* project = (dynamic_cast<const FormulaProject*>(context))->GetProject();
    const wxString testName = project->GetCurrentCustomTest();
    if (!project->HasCustomTest(testName))
        { throw std::exception(_(L"Internal error: unable to find custom test by name.")); }
    if (!project->GetCustomTest(testName)->GetIterator()->is_using_familiar_words())
        {
        throw std::exception(
            _(L"Test has not defined what an unfamiliar word is. "
               "Custom unfamiliar word test cannot be calculated."));
        }
    if (project->GetProjectLanguage() != readability::test_language::english_test)
        {
        throw std::exception(
            wxString::Format(_(L"%s function can only be used for English projects."),
            ReadabilityFormulaParser::GetCustomHarrisJacobsonSignature()));
        }
    double grade_value = 1; // lowest grade for HJ
    try
        {
        if (project->GetHarrisJacobsonTextExclusionMode() ==
            SpecializedTestTextExclusion::ExcludeIncompleteSentencesExceptHeadings)
            {
            grade_value = readability::harris_jacobson(
                project->GetTotalWordsFromCompleteSentencesAndHeaders() - project->GetTotalNumeralsFromCompleteSentencesAndHeaders(),
                project->GetCustomTest(testName)->GetUniqueUnfamiliarWordCount(),
                project->GetTotalSentencesFromCompleteSentencesAndHeaders());
            }
        else
            {
            grade_value = readability::harris_jacobson(
                project->GetTotalWords()-project->GetTotalNumerals(),
                project->GetCustomTest(testName)->GetUniqueUnfamiliarWordCount(),
                project->GetTotalSentences());
            }
        }
    catch (std::domain_error)
        { throw std::exception(_(L"Unable to calculate custom Harris-Jacobson.")); }
    return grade_value;
    }

/// @returns The total number of unfamiliar words (from a custom list) the document.
/// @param context The TinyEpr++ expression object.
[[nodiscard]]
static double UnfamiliarWordCount(const te_expr* context)
    {
    const BaseProject* project = (dynamic_cast<const FormulaProject*>(context))->GetProject();
    const wxString testName = project->GetCurrentCustomTest();
    if (!project->HasCustomTest(testName))
        { throw std::exception(_(L"Internal error: unable to find custom test by name.")); }
    return project->GetCustomTest(testName)->GetUnfamiliarWordCount();
    }

/// @returns The total number of unique unfamiliar words (from a custom list) from the document.
/// @param context The TinyEpr++ expression object.
[[nodiscard]]
static double UniqueUnfamiliarWordCount(const te_expr* context)
    {
    const BaseProject* project = (dynamic_cast<const FormulaProject*>(context))->GetProject();
    const wxString testName = project->GetCurrentCustomTest();
    if (!project->HasCustomTest(testName))
        { throw std::exception(_(L"Internal error: unable to find custom test by name.")); }
    return project->GetCustomTest(testName)->GetUniqueUnfamiliarWordCount();
    }

/// @returns The total number of familiar words (from a custom list) the document.
/// @param context The TinyEpr++ expression object.
[[nodiscard]]
static double FamiliarWordCount(const te_expr* context)
    {
    const BaseProject* project = (dynamic_cast<const FormulaProject*>(context))->GetProject();
    const wxString testName = project->GetCurrentCustomTest();
    if (!project->HasCustomTest(testName))
        { throw std::exception(_(L"Internal error: unable to find custom test by name.")); }
    return project->GetTotalWords() - project->GetCustomTest(testName)->GetUnfamiliarWordCount();
    }

/// @returns The total number of unfamiliar Harris-Jacobson words from the document.
/// @param context The TinyEpr++ expression object.
[[nodiscard]]
static double UnfamiliarHarrisJacobsonWordCount(const te_expr* context)
    {
    return (dynamic_cast<const FormulaProject*>(context))->GetProject()->GetTotalHardWordsHarrisJacobson();
    }

/// @returns The total number of unique unfamiliar Harris-Jacobson words from the document.
/// @param context The TinyEpr++ expression object.
[[nodiscard]]
static double UniqueUnfamiliarHarrisJacobsonWordCount(const te_expr* context)
    {
    return (dynamic_cast<const FormulaProject*>(context))->GetProject()->GetTotalUniqueHarrisJacobsonHardWords();
    }

/// @returns The total number of familiar Harris-Jacobson words from the document.
/// @param context The TinyEpr++ expression object.
[[nodiscard]]
static double FamiliarHarrisJacobsonWordCount(const te_expr* context)
    {
    const BaseProject* project = (dynamic_cast<const FormulaProject*>(context))->GetProject();
    if (project->GetHarrisJacobsonTextExclusionMode() ==
        SpecializedTestTextExclusion::ExcludeIncompleteSentencesExceptHeadings)
        {
        return
            (project->GetTotalWordsFromCompleteSentencesAndHeaders()-project->GetTotalNumeralsFromCompleteSentencesAndHeaders()) -
             project->GetTotalHardWordsHarrisJacobson();
        }
    else
        {
        return
            (project->GetTotalWords()-project->GetTotalNumerals()) -
             project->GetTotalHardWordsHarrisJacobson();
        }
    }

/// @returns The number of unique monosyllabic words from the document.
/// @param context The TinyEpr++ expression object.
[[nodiscard]]
static double UniqueOneSyllableWordCount(const te_expr* context)
    {
    return (dynamic_cast<const FormulaProject*>(context))->GetProject()->GetTotalUniqueMonoSyllablicWords();
    }

/// @returns The total number of familiar Dale-Chall words from the document.
/// @param context The TinyEpr++ expression object.
[[nodiscard]]
static double FamiliarDaleChallWordCount(const te_expr* context)
    {
    const BaseProject* project = (dynamic_cast<const FormulaProject*>(context))->GetProject();
    if (project->GetDaleChallTextExclusionMode() ==
        SpecializedTestTextExclusion::ExcludeIncompleteSentencesExceptHeadings)
        {
        return project->GetTotalWordsFromCompleteSentencesAndHeaders() - project->GetTotalHardWordsDaleChall();
        }
    else
        {
        return project->GetTotalWords() - project->GetTotalHardWordsDaleChall();
        }
    }

/// @returns The total number of monosyllabic words from the document.
/// @param context The TinyEpr++ expression object.
[[nodiscard]]
static double OneSyllableWordCount(const te_expr* context)
    {
    return (dynamic_cast<const FormulaProject*>(context))->GetProject()->GetTotalMonoSyllabicWords();
    }

/// @returns The total number of units/independent clauses from the document.
/// A unit is defined as a section of text ending with a period, exclamation mark,
///     question mark, interrobang, colon, semicolon, or dash.
/// @param context The TinyEpr++ expression object.
[[nodiscard]]
static double IndependentClauseCount(const te_expr* context)
    {
    return (dynamic_cast<const FormulaProject*>(context))->GetProject()->GetTotalSentenceUnits();
    }

/// @returns The total number of characters and punctuation from the document.
/// @param context The TinyEpr++ expression object.
[[nodiscard]]
static double CharacterPlusPunctuationCount(const te_expr* context)
    { return (dynamic_cast<const FormulaProject*>(context))->GetProject()->GetTotalCharactersPlusPunctuation(); }

/// @returns The total number of proper nouns from the document.
/// @param context The TinyEpr++ expression object.
[[nodiscard]]
static double ProperNounCount(const te_expr* context)
    {
    if (dynamic_cast<const FormulaProject*>(context)->GetProject()->GetProjectLanguage() == readability::test_language::german_test)
        { throw std::exception(_(L"ProperNounCount() function not supported for German projects.")); }
    return (dynamic_cast<const FormulaProject*>(context))->GetProject()->GetTotalProperNouns();
    }

/// @returns The total number of words from the document.
/// @param context The TinyEpr++ expression object.
[[nodiscard]]
static double WordCount(const te_expr* context, const double wordType)
    {
    if (std::isnan(wordType) || wordType == 0/*Default*/)
        { return (dynamic_cast<const FormulaProject*>(context))->GetProject()->GetTotalWords(); }
    else if (wordType == 1/*DaleChall*/)
        {
        const BaseProject* project = (dynamic_cast<const FormulaProject*>(context))->GetProject();
        return (project->GetDaleChallTextExclusionMode() ==
                SpecializedTestTextExclusion::ExcludeIncompleteSentencesExceptHeadings) ?
            project->GetTotalWordsFromCompleteSentencesAndHeaders() : project->GetTotalWords();
        }
    else if (wordType == 2/*HarrisJacobson*/)
        {
        const BaseProject* project = (dynamic_cast<const FormulaProject*>(context))->GetProject();
        return (project->GetHarrisJacobsonTextExclusionMode() ==
                SpecializedTestTextExclusion::ExcludeIncompleteSentencesExceptHeadings) ?
            project->GetTotalWordsFromCompleteSentencesAndHeaders() : project->GetTotalWords();
        }
    else
        {
        throw std::exception(wxString::Format(_(L"Invalid value used in %s"),
            wxString(__WXFUNCTION__)));
        }
    }

/// @returns The total number of words consisting of three or more syllables from the document.
/// @param context The TinyEpr++ expression object.
[[nodiscard]]
static double ThreeSyllablePlusWordCount(const te_expr* context, const double wordType)
    {
    if (std::isnan(wordType) || wordType == 0/*Default*/)
        { return (dynamic_cast<const FormulaProject*>(context))->GetProject()->GetTotal3PlusSyllabicWords(); }
    else if (wordType == 1/*NumeralsFullySyllabized*/)
        { return (dynamic_cast<const FormulaProject*>(context))->GetProject()->GetTotal3PlusSyllabicWordsNumeralsFullySyllabized(); }
    else
        {
        throw std::exception(wxString::Format(_(L"Invalid value used in %s"),
            wxString(__WXFUNCTION__)));
        }
    }

/// @returns The number of unique words consisting of three or more syllables from the document.
/// @param context The TinyEpr++ expression object.
[[nodiscard]]
static double UniqueThreeSyllablePlusWordCount(const te_expr* context, const double wordType)
    {
    if (std::isnan(wordType) || wordType == 0/*Default*/)
        { return (dynamic_cast<const FormulaProject*>(context))->GetProject()->GetTotalUnique3PlusSyllableWords(); }
    else if (wordType == 1/*NumeralsFullySyllabized*/)
        { return (dynamic_cast<const FormulaProject*>(context))->GetProject()->GetUnique3PlusSyllabicWordsNumeralsFullySyllabized(); }
    else
        {
        throw std::exception(wxString::Format(_(L"Invalid value used in %s"),
            wxString(__WXFUNCTION__)));
        }
    }

/// @returns The total number of syllables from the document.
/// @param context The TinyEpr++ expression object.
[[nodiscard]]
static double SyllableCount(const te_expr* context, const double wordType)
    {
    if (std::isnan(wordType) || wordType == 0/*Default*/)
        { return (dynamic_cast<const FormulaProject*>(context))->GetProject()->GetTotalSyllables(); }
    else if (wordType == 1/*NumeralsFullySyllabized*/)
        { return (dynamic_cast<const FormulaProject*>(context))->GetProject()->GetTotalSyllablesNumeralsFullySyllabized(); }
    else if (wordType == 2/*NumeralsAreOneSyllable*/)
        { return (dynamic_cast<const FormulaProject*>(context))->GetProject()->GetTotalSyllablesNumeralsOneSyllable(); }
    else
        {
        throw std::exception(wxString::Format(_(L"Invalid value used in %s"),
            wxString(__WXFUNCTION__)));
        }
    }

/// @returns The total number of characters (i.e., letters and numbers) from the document.
/// @param context The TinyEpr++ expression object.
[[nodiscard]]
static double CharacterCount(const te_expr* context, const double wordType)
    {
    if (std::isnan(wordType) || wordType == 0/*Default*/)
        { return (dynamic_cast<const FormulaProject*>(context))->GetProject()->GetTotalCharacters(); }
    else if (wordType == 1/*DaleChall*/)
        {
        const BaseProject* project = (dynamic_cast<const FormulaProject*>(context))->GetProject();
        return (project->GetDaleChallTextExclusionMode() ==
                SpecializedTestTextExclusion::ExcludeIncompleteSentencesExceptHeadings) ?
            project->GetTotalCharactersFromCompleteSentencesAndHeaders() : project->GetTotalCharacters();
        }
    else if (wordType == 2/*HarrisJacobson*/)
        {
        const BaseProject* project = (dynamic_cast<const FormulaProject*>(context))->GetProject();
        return (project->GetHarrisJacobsonTextExclusionMode() ==
                SpecializedTestTextExclusion::ExcludeIncompleteSentencesExceptHeadings) ?
            project->GetTotalCharactersFromCompleteSentencesAndHeaders() : project->GetTotalCharacters();
        }
    else
        {
        throw std::exception(wxString::Format(_(L"Invalid value used in %s"),
            wxString(__WXFUNCTION__)));
        }
    }

/// @returns The total number of sentences from the document.
/// @param context The TinyEpr++ expression object.
[[nodiscard]]
static double SentenceCount(const te_expr* context, const double wordType)
    {
    if (std::isnan(wordType) || wordType == 0/*Default*/)
        { return (dynamic_cast<const FormulaProject*>(context))->GetProject()->GetTotalSentences(); }
    else if (wordType == 1/*DaleChall*/)
        {
        const BaseProject* project = (dynamic_cast<const FormulaProject*>(context))->GetProject();
        return (project->GetDaleChallTextExclusionMode() ==
                SpecializedTestTextExclusion::ExcludeIncompleteSentencesExceptHeadings) ?
            project->GetTotalSentencesFromCompleteSentencesAndHeaders() : project->GetTotalSentences();
        }
    else if (wordType == 2/*HarrisJacobson*/)
        {
        const BaseProject* project = (dynamic_cast<const FormulaProject*>(context))->GetProject();
        return (project->GetHarrisJacobsonTextExclusionMode() ==
                SpecializedTestTextExclusion::ExcludeIncompleteSentencesExceptHeadings) ?
            project->GetTotalSentencesFromCompleteSentencesAndHeaders() : project->GetTotalSentences();
        }
    // Fog has special rules for units vs. traditional sentences
    else if (wordType == 3/*GunningFog*/)
        {
        const BaseProject* project = (dynamic_cast<const FormulaProject*>(context))->GetProject();
        return project->FogUseSentenceUnits() ?
            project->GetTotalSentenceUnits() : project->GetTotalSentences();
        }
    else
        {
        throw std::exception(wxString::Format(_(L"Invalid value used in %s"),
            wxString(__WXFUNCTION__)));
        }
    }

//------------------------------------------------
void ReadabilityFormulaParser::UpdateVariables()
    {
    const double dcWordCount =
        (m_formualProject.GetProject()->GetDaleChallTextExclusionMode() == SpecializedTestTextExclusion::ExcludeIncompleteSentencesExceptHeadings) ?
        (m_formualProject.GetProject()->GetTotalWordsFromCompleteSentencesAndHeaders() - m_formualProject.GetProject()->GetTotalHardWordsDaleChall()) :
        (m_formualProject.GetProject()->GetTotalWords() - m_formualProject.GetProject()->GetTotalHardWordsDaleChall());

    set_constant(_DT("D"), dcWordCount);
    }

//------------------------------------------------
ReadabilityFormulaParser::ReadabilityFormulaParser(const BaseProject* project,
    const wchar_t decimalSeparator, const wchar_t listSeparator) : m_formualProject(project)
    {
    set_decimal_separator(decimalSeparator);
    set_list_separator(listSeparator);

    const double dcWordCount =
        (m_formualProject.GetProject()->GetDaleChallTextExclusionMode() == SpecializedTestTextExclusion::ExcludeIncompleteSentencesExceptHeadings) ?
        (m_formualProject.GetProject()->GetTotalWordsFromCompleteSentencesAndHeaders() - m_formualProject.GetProject()->GetTotalHardWordsDaleChall()) :
        (m_formualProject.GetProject()->GetTotalWords() - m_formualProject.GetProject()->GetTotalHardWordsDaleChall());

    set_variables_and_functions({
        // note that these constants must be char* (not whcar_t*)
        { _DT("UNIQUETHREESYLLABLEPLUSWORDCOUNT"), static_cast<te_confun1>(UniqueThreeSyllablePlusWordCount), TE_DEFAULT, &m_formualProject },
        { _DT("THREESYLLABLEPLUSWORDCOUNT"), static_cast<te_confun1>(ThreeSyllablePlusWordCount), TE_DEFAULT, &m_formualProject },
        { _DT("SYLLABLECOUNT"), static_cast<te_confun1>(SyllableCount), TE_DEFAULT, &m_formualProject },
        { _DT("CHARACTERCOUNT"), static_cast<te_confun1>(CharacterCount), TE_DEFAULT, &m_formualProject },
        { _DT("SENTENCECOUNT"), static_cast<te_confun1>(SentenceCount), TE_DEFAULT, &m_formualProject },
        { _DT("WORDCOUNT"), static_cast<te_confun1>(WordCount), TE_DEFAULT, &m_formualProject },
        { _DT("MINIWORDCOUNT"), static_cast<te_confun0>(MiniWordCount), TE_DEFAULT, &m_formualProject },
        { _DT("HARDFOGWORDCOUNT"), static_cast<te_confun0>(HardFogWordCount), TE_DEFAULT, &m_formualProject },
        { _DT("CHARACTERPLUSPUNCTUATIONCOUNT"), static_cast<te_confun0>(CharacterPlusPunctuationCount), TE_DEFAULT, &m_formualProject },
        { _DT("NUMERALCOUNT"), static_cast<te_confun0>(NumeralCount), TE_DEFAULT, &m_formualProject },
        { _DT("UNIQUEWORDCOUNT"), static_cast<te_confun0>(UniqueWordCount), TE_DEFAULT, &m_formualProject },
        { _DT("UNIQUESIXCHARACTERPLUSWORDCOUNT"), static_cast<te_confun0>(UniqueSixCharacterPlusWordCount), TE_DEFAULT, &m_formualProject },
        { _DT("UNIQUEONESYLLABLEWORDCOUNT"), static_cast<te_confun0>(UniqueOneSyllableWordCount), TE_DEFAULT, &m_formualProject },
        { _DT("FAMILIARWORDCOUNT"), static_cast<te_confun0>(FamiliarWordCount), TE_DEFAULT, &m_formualProject },
        { _DT("UNFAMILIARWORDCOUNT"), static_cast<te_confun0>(UnfamiliarWordCount), TE_DEFAULT, &m_formualProject },
        { _DT("UNIQUEUNFAMILIARWORDCOUNT"), static_cast<te_confun0>(UniqueUnfamiliarWordCount), TE_DEFAULT, &m_formualProject },
        { _DT("UNFAMILIARHARRISJACOBSONWORDCOUNT"), static_cast<te_confun0>(UnfamiliarHarrisJacobsonWordCount), TE_DEFAULT, &m_formualProject },
        { _DT("UNIQUEUNFAMILIARHARRISJACOBSONWORDCOUNT"), static_cast<te_confun0>(UniqueUnfamiliarHarrisJacobsonWordCount), TE_DEFAULT, &m_formualProject },
        { _DT("FAMILIARHARRISJACOBSONWORDCOUNT"), static_cast<te_confun0>(FamiliarHarrisJacobsonWordCount), TE_DEFAULT, &m_formualProject },
        { _DT("UNFAMILIARDALECHALLWORDCOUNT"), static_cast<te_confun0>(UnfamiliarDaleChallWordCount), TE_DEFAULT, &m_formualProject },
        { _DT("UNIQUEUNFAMILIARDALECHALLWORDCOUNT"), static_cast<te_confun0>(UniqueUnfamiliarDaleChallWordCount), TE_DEFAULT, &m_formualProject },
        { _DT("FAMILIARDALECHALLWORDCOUNT"), static_cast<te_confun0>(FamiliarDaleChallWordCount), TE_DEFAULT, &m_formualProject },
        { _DT("UNFAMILIARSPACHEWORDCOUNT"), static_cast<te_confun0>(UnfamiliarSpacheWordCount), TE_DEFAULT, &m_formualProject },
        { _DT("UNIQUEUNFAMILIARSPACHEWORDCOUNT"), static_cast<te_confun0>(UniqueUnfamiliarSpacheWordCount), TE_DEFAULT, &m_formualProject },
        { _DT("FAMILIARSPACHEWORDCOUNT"), static_cast<te_confun0>(FamiliarSpacheWordCount), TE_DEFAULT, &m_formualProject },
        { _DT("SIXCHARACTERPLUSWORDCOUNT"), static_cast<te_confun0>(SixCharacterPlusWordCount), TE_DEFAULT, &m_formualProject },
        { _DT("SEVENCHARACTERPLUSWORDCOUNT"), static_cast<te_confun0>(SevenCharacterPlusWordCount), TE_DEFAULT, &m_formualProject },
        { _DT("ONESYLLABLEWORDCOUNT"), static_cast<te_confun0>(OneSyllableWordCount), TE_DEFAULT, &m_formualProject },
        { _DT("INDEPENDENTCLAUSECOUNT"), static_cast<te_confun0>(IndependentClauseCount), TE_DEFAULT, &m_formualProject },
        { _DT("PROPERNOUNCOUNT"), static_cast<te_confun0>(ProperNounCount), TE_DEFAULT, &m_formualProject },
        { _DT("CUSTOMHARRISJACOBSON"), static_cast<te_confun0>(CustomHarrisJacobson), TE_DEFAULT, &m_formualProject },
        { _DT("CUSTOMSPACHE"), static_cast<te_confun0>(CustomSpache), TE_DEFAULT, &m_formualProject },
        { _DT("CUSTOMNEWDALECHALL"), static_cast<te_confun0>(CustomNewDaleChall), TE_DEFAULT, &m_formualProject },
        // shortcuts
        { "B", &project->GetTotalSyllables() },
        { "S", &project->GetTotalSentences() },
        { "W", &project->GetTotalWords() },
        // a dynamic constant, call UpdateVariables() prior to interpret() to update
        { "D", dcWordCount },
        { "R", &project->GetTotalCharacters() },
        { _DT("RP"), &project->GetTotalCharactersPlusPunctuation() },
        { "M", &project->GetTotalMonoSyllabicWords() },
        { "C", &project->GetTotal3PlusSyllabicWords() },
        { "L", &project->GetTotalLongWords() },
        { "X", &project->GetTotalHardLixRixWords() },
        { "U", &project->GetTotalSentenceUnits() },
        { _DT("UDC"), &project->GetTotalHardWordsDaleChall() },
        { _DT("UUS"), &project->GetTotalUniqueHardWordsSpache() },
        { "T", &project->GetTotalMiniWords() },
        { "F", &project->GetTotalHardWordsFog() },
        // constants for text exclusion/word & sentence counting
        { _DT("Default"), 0 },
        { "DaleChall", 1 },
        { "HarrisJacobson", 2 },
        { "GunningFog", 3 },
        // constants for syllable counting
        { "NumeralsFullySyllabized", 1 },
        { "NumeralsAreOneSyllable", 2 }
        });
    }
