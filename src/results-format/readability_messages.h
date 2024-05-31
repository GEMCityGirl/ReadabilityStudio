/** @addtogroup Formatting
    @brief Classes for parsing and formatting results output.
    @date 2005-2023
    @copyright Oleander Software, Ltd.
    @author Blake Madden
    @details This program is free software; you can redistribute it and/or modify
     it under the terms of the 3-Clause BSD License.

     SPDX-License-Identifier: BSD-3-Clause
* @{*/

#ifndef __READABILITY_MESSAGES_H__
#define __READABILITY_MESSAGES_H__

#include "../Wisteria-Dataviz/src/i18n-check/src/donttranslate.h"
#include "../Wisteria-Dataviz/src/i18n-check/src/string_util.h"
#include "../Wisteria-Dataviz/src/util/formulaformat.h"
#include "../Wisteria-Dataviz/src/util/numberformat.h"
#include "../readability/english_readability.h"
#include "../readability/german_readability.h"
#include <cmath>
#include <wx/string.h>
#include <wx/tokenzr.h>
#include <wx/wx.h>

/// @brief Readability test name, description, and display management class.
class ReadabilityMessages final : public Wisteria::NumberFormat<wxString>
    {
  public:
    /// @brief Methods for how to display a reading age.
    enum class ReadingAgeDisplay
        {
        ReadingAgeAsARange,       /*!< Convert a grade level like 5.6 to "10-11".*/
        ReadingAgeRoundToSemester /*!< Convert a grade level like 5.6 to "11".*/
        };

    /// @returns Dolch display label.
    [[nodiscard]]
    static const wxString GetDolchLabel()
        {
        return _(L"Dolch Sight Words");
        }

    // Do NOT change or translate any of these!
    // They are hardwired to external help topics and icons by the same name.
    /// @returns ARI key.
    [[nodiscard]]
    static const wxString ARI()
        {
        return wxString(_DT(L"ari-test"));
        }

    /// @returns COLEMAN_LIAU key.
    [[nodiscard]]
    static const wxString COLEMAN_LIAU()
        {
        return wxString(_DT(L"coleman-liau-test"));
        }

    /// @returns DALE_CHALL key.
    [[nodiscard]]
    static const wxString DALE_CHALL()
        {
        return wxString(_DT(L"dale-chall-test"));
        }

    /// @returns FORCAST key.
    [[nodiscard]]
    static const wxString FORCAST()
        {
        return wxString(_DT(L"forcast-test"));
        }

    /// @returns FLESCH key.
    [[nodiscard]]
    static const wxString FLESCH()
        {
        return wxString(_DT(L"flesch-test"));
        }

    /// @returns FLESCH_KINCAID key.
    [[nodiscard]]
    static const wxString FLESCH_KINCAID()
        {
        return wxString(_DT(L"flesch-kincaid-test"));
        }

    /// @returns FRY key.
    [[nodiscard]]
    static const wxString FRY()
        {
        return wxString(_DT(L"fry-test"));
        }

    /// @returns GUNNING_FOG key.
    [[nodiscard]]
    static const wxString GUNNING_FOG()
        {
        return wxString(_DT(L"gunning-fog-test"));
        }

    /// @returns LIX key.
    [[nodiscard]]
    static const wxString LIX()
        {
        return wxString(_DT(L"lix-test"));
        }

    /// @returns EFLAW key.
    [[nodiscard]]
    static const wxString EFLAW()
        {
        return wxString(_DT(L"eflaw-test"));
        }

    /// @returns NEW_FOG key.
    [[nodiscard]]
    static const wxString NEW_FOG()
        {
        return wxString(_DT(L"new-fog-count-test"));
        }

    /// @returns RAYGOR key.
    [[nodiscard]]
    static const wxString RAYGOR()
        {
        return wxString(_DT(L"raygor-test"));
        }

    /// @returns RIX key.
    [[nodiscard]]
    static const wxString RIX()
        {
        return wxString(_DT(L"rix-test"));
        }

    /// @returns SIMPLE_ARI key.
    [[nodiscard]]
    static const wxString SIMPLE_ARI()
        {
        return wxString(_DT(L"new-ari-simplified"));
        }

    /// @returns SPACHE key.
    [[nodiscard]]
    static const wxString SPACHE()
        {
        return wxString(_DT(L"spache-test"));
        }

    /// @returns SMOG key.
    [[nodiscard]]
    static const wxString SMOG()
        {
        return wxString(_DT(L"smog-test"));
        }

    /// @returns SMOG_SIMPLIFIED key.
    [[nodiscard]]
    static const wxString SMOG_SIMPLIFIED()
        {
        return wxString(_DT(L"smog-test-simplified"));
        }

    /// @returns MODIFIED_SMOG key.
    [[nodiscard]]
    static const wxString MODIFIED_SMOG()
        {
        return wxString(_DT(L"modified-smog"));
        }

    /// @returns PSK_FLESCH key.
    [[nodiscard]]
    static const wxString PSK_FLESCH()
        {
        return wxString(_DT(L"psk-test"));
        }

    /// @returns HARRIS_JACOBSON key.
    [[nodiscard]]
    static const wxString HARRIS_JACOBSON()
        {
        return wxString(_DT(L"harris-jacobson"));
        }

    /// @returns PSK_DALE_CHALL key.
    [[nodiscard]]
    static const wxString PSK_DALE_CHALL()
        {
        return wxString(_DT(L"psk-dale-chall"));
        }

    /// @returns BORMUTH_CLOZE_MEAN key.
    [[nodiscard]]
    static const wxString BORMUTH_CLOZE_MEAN()
        {
        return wxString(_DT(L"bormuth-cloze-mean-machine-passage"));
        }

    /// @returns BORMUTH_GRADE_PLACEMENT_35 key.
    [[nodiscard]]
    static const wxString BORMUTH_GRADE_PLACEMENT_35()
        {
        return wxString(_DT(L"bormuth-grade-placement-35-machine-passage"));
        }

    /// @returns PSK_GUNNING_FOG key.
    [[nodiscard]]
    static const wxString PSK_GUNNING_FOG()
        {
        return wxString(_DT(L"psk-fog"));
        }

    /// @returns FARR_JENKINS_PATERSON key.
    [[nodiscard]]
    static const wxString FARR_JENKINS_PATERSON()
        {
        return wxString(_DT(L"farr-jenkins-paterson"));
        }

    /// @returns NEW_FARR_JENKINS_PATERSON key.
    [[nodiscard]]
    static const wxString NEW_FARR_JENKINS_PATERSON()
        {
        return wxString(_DT(L"new-farr-jenkins-paterson"));
        }

    /// @returns PSK_FARR_JENKINS_PATERSON key.
    [[nodiscard]]
    static const wxString PSK_FARR_JENKINS_PATERSON()
        {
        return wxString(_DT(L"psk-farr-jenkins-paterson"));
        }

    /// @returns WHEELER_SMITH key.
    [[nodiscard]]
    static const wxString WHEELER_SMITH()
        {
        return wxString(_DT(L"wheeler-smith"));
        }

    /// @returns GPM_FRY key.
    [[nodiscard]]
    static const wxString GPM_FRY()
        {
        return wxString(_DT(L"gilliam-pena-mountain-fry-graph"));
        }

    /// @returns FRASE key.
    [[nodiscard]]
    static const wxString FRASE()
        {
        return wxString(_DT(L"frase"));
        }

    /// @returns CRAWFORD key.
    [[nodiscard]]
    static const wxString CRAWFORD()
        {
        return wxString(_DT(L"crawford"));
        }

    /// @returns SOL_SPANISH key.
    [[nodiscard]]
    static const wxString SOL_SPANISH()
        {
        return wxString(_DT(L"sol-spanish"));
        }

    /// @returns DEGREES_OF_READING_POWER key.
    [[nodiscard]]
    static const wxString DEGREES_OF_READING_POWER()
        {
        return wxString(_DT(L"degrees-of-reading-power"));
        }

    /// @returns DEGREES_OF_READING_POWER_GE key.
    [[nodiscard]]
    static const wxString DEGREES_OF_READING_POWER_GE()
        {
        return wxString(_DT(L"degrees-of-reading-power-grade-equivalent"));
        }

    /// @returns NEW_ARI key.
    [[nodiscard]]
    static const wxString NEW_ARI()
        {
        return wxString(_DT(L"new-ari"));
        }

    /// @returns FLESCH_KINCAID_SIMPLIFIED key.
    [[nodiscard]]
    static const wxString FLESCH_KINCAID_SIMPLIFIED()
        {
        return wxString(_DT(L"flesch-kincaid-test-simplified"));
        }

    /// @returns DOLCH key.
    /// @note This is not technically a test, but store a constant here for it.
    [[nodiscard]]
    static const wxString DOLCH()
        {
        return wxString(_DT(L"dolch"));
        }

    /// @returns AMSTAD key.
    [[nodiscard]]
    static const wxString AMSTAD()
        {
        return wxString(_DT(L"amstad"));
        }

    /// @returns SMOG_BAMBERGER_VANECEK key.
    [[nodiscard]]
    static const wxString SMOG_BAMBERGER_VANECEK()
        {
        return wxString(_DT(L"smog-bamberger-vanecek"));
        }

    /// @returns WHEELER_SMITH_BAMBERGER_VANECEK key.
    [[nodiscard]]
    static const wxString WHEELER_SMITH_BAMBERGER_VANECEK()
        {
        return wxString(_DT(L"wheeler-smith-bamberger-vanecek"));
        }

    /// @returns QU key.
    [[nodiscard]]
    static const wxString QU()
        {
        return wxString(_DT(L"qu-bamberger-vanecek"));
        }

    /// @returns NEUE_WIENER_SACHTEXTFORMEL1 key.
    [[nodiscard]]
    static const wxString NEUE_WIENER_SACHTEXTFORMEL1()
        {
        return wxString(_DT(L"neue-wiener-sachtextformel1"));
        }

    /// @returns NEUE_WIENER_SACHTEXTFORMEL2 key.
    [[nodiscard]]
    static const wxString NEUE_WIENER_SACHTEXTFORMEL2()
        {
        return wxString(_DT(L"neue-wiener-sachtextformel2"));
        }

    /// @returns NEUE_WIENER_SACHTEXTFORMEL3 key.
    [[nodiscard]]
    static const wxString NEUE_WIENER_SACHTEXTFORMEL3()
        {
        return wxString(_DT(L"neue-wiener-sachtextformel3"));
        }

    /// @returns LIX_GERMAN_CHILDRENS_LITERATURE key.
    [[nodiscard]]
    static const wxString LIX_GERMAN_CHILDRENS_LITERATURE()
        {
        return wxString(_DT(L"lix-german-childrens-literature"));
        }

    /// @returns LIX_GERMAN_TECHNICAL key.
    [[nodiscard]]
    static const wxString LIX_GERMAN_TECHNICAL()
        {
        return wxString(_DT(L"lix-german-technical"));
        }

    /// @returns RIX_GERMAN_FICTION key.
    [[nodiscard]]
    static const wxString RIX_GERMAN_FICTION()
        {
        return wxString(_DT(L"rix-german-fiction"));
        }

    /// @returns RIX_GERMAN_NONFICTION key.
    [[nodiscard]]
    static const wxString RIX_GERMAN_NONFICTION()
        {
        return wxString(_DT(L"rix-german-nonfiction"));
        }

    /// @returns SCHWARTZ key.
    [[nodiscard]]
    static const wxString SCHWARTZ()
        {
        return wxString(_DT(L"schwartz"));
        }

    /// @returns ELF key.
    [[nodiscard]]
    static const wxString ELF()
        {
        return wxString(_DT(L"easy-listening-formula"));
        }

    /// @returns DANIELSON_BRYAN_1 key.
    [[nodiscard]]
    static const wxString DANIELSON_BRYAN_1()
        {
        return wxString(_DT(L"danielson-bryan-1"));
        }

    /// @returns DANIELSON_BRYAN_2 key.
    [[nodiscard]]
    static const wxString DANIELSON_BRYAN_2()
        {
        return wxString(_DT(L"danielson-bryan-2"));
        }

    // test bundle names
    //------------------
    /// @returns PSK bundle name.
    [[nodiscard]]
    static const wxString GetPskBundleName()
        {
        return wxString(_DT(L"PSK"));
        }

    /// @returns Kincaid bundle name.
    [[nodiscard]]
    static const wxString GetKincaidNavyBundleName()
        {
        return _(L"Kincaid's Tests for Navy-Personnel Materials");
        }

    /// @returns Consent Forms bundle name.
    [[nodiscard]]
    static const wxString GetConsentFormsBundleName()
        {
        return _(L"Patient Consent Forms");
        }

    // test notes and descriptions
    //----------------------------
    /// @returns A label describing how numbers are fully syllabized.
    [[nodiscard]]
    static wxString GetNumeralAreFullySyllabizedNote()
        {
        return _(L"All numerals are fully syllabized for this test.");
        }

    /// @returns A description for FORCAST.
    [[nodiscard]]
    static wxString GetForcastNote()
        {
        return _(L"FORCAST results may be slightly different from other tests because it does "
                 "not take sentence length into account. If your document is structured mostly "
                 "with tables and lists then expect some variance between the FORCAST grade level "
                 "and other tests' grade levels.");
        }

    /// @returns A label describing how punctuation is ignored.
    [[nodiscard]]
    static wxString GetPunctuationIgnoredNote()
        {
        return _(L"Punctuation (e.g., apostrophes) is ignored when counting word lengths.");
        }

    /// @returns A description for Harris-Jacobson.
    [[nodiscard]]
    static wxString GetHarrisJacobsonNote()
        {
        return _(L"This test includes headers and sub-headers as full sentences and excludes "
                 "list items and tables. Numerals are also entirely ignored and subtracted from "
                 "the total word count. These settings used specifically for this test may differ "
                 "from the analysis configurations used for other tests.");
        }

    /// @returns A label describing sentence units.
    [[nodiscard]]
    static wxString GetUnitNote()
        {
        return _(
            L"A unit is a section of text ending with a period, exclamation mark, question mark, "
            "interrobang, colon, semicolon, or dash. Lengthy, complex sentences generally "
            "contain more than one unit.");
        }

    /// @returns A label describing predicted cloze scores.
    [[nodiscard]]
    static wxString GetPredictedClozeNote()
        {
        return _(L"A predicted cloze score is the estimation of a passage's mean score if it "
                 "were given as a cloze test to a group of students. Scores range from 0 "
                 "(zero comprehension) to 100 (total comprehension).");
        }

    /// @returns A description for Degrees of Reading Power.
    [[nodiscard]]
    static wxString GetDrpNote()
        {
        return _(L"A DRP score measures the difficulty of text in DRP units. These units "
                 "range from 0 (easy) to 100 (difficult). This score can help match a "
                 "document to a student based on his/her DRP ability score.");
        }

    /// @returns A label describing an FRE difficulty level.
    /// @param diffLevel The difficulty level.
    [[nodiscard]]
    static wxString GetFleschDescription(const readability::flesch_difficulty diffLevel);
    /** @returns A label describing a DB2 difficulty level.
        @param diffLevel The difficulty level.
        @note DB2's scale is the same as Flesch, but its descriptions
            (from the article) are a little more verbose.*/
    [[nodiscard]]
    static wxString GetDanielsonBryan2Description(const readability::flesch_difficulty diffLevel);
    /// @returns A label describing an EFLAW difficulty level.
    /// @param diffLevel The difficulty level.
    [[nodiscard]]
    static wxString GetEflawDescription(const readability::eflaw_difficulty diffLevel);
    /// @returns A label describing a Lix difficulty level.
    /// @param diffLevel The difficulty level.
    [[nodiscard]]
    static wxString GetLixDescription(const readability::lix_difficulty diffLevel);
    /// @returns A label describing a Lix (German version) difficulty level.
    /// @param diffLevel The difficulty level.
    [[nodiscard]]
    static wxString GetLixDescription(const readability::german_lix_difficulty diffLevel);
    /// Negative cloze scores make little sense, but are published in the
    ///     New DC workbook, so allow them
    [[nodiscard]]
    static wxString GetPredictedClozeDescription(const int clozeScore);
    [[nodiscard]]
    static wxString GetDrpUnitDescription(const size_t drpScore);
    /// @returns A description for FRASE.
    /// @param frazeLevel The FRASE level to describe.
    [[nodiscard]]
    static wxString GetFraseDescription(const size_t frazeLevel);

    /// @returns The month of completion from a given 0-9 value (the mantissa of a grade score).
    ///     This is grade scale independent.
    /// @param month The month to get a label for.
    [[nodiscard]]
    static wxString GetMonthLabel(const size_t month);
    /// @returns The age from a U.S. grade level.
    /// @param value The grade level score.
    /// @param displayFormat A ReadingAgeDisplay object specifying how to display the age.
    [[nodiscard]]
    static wxString GetAgeFromUSGrade(double value, const ReadingAgeDisplay displayFormat);
    /// @returns The age from two U.S. grade levels.\n
    ///     This is used for tests that return a grade range (e.g., New Dale-Chall).\n
    /// @note This range represents the lowest age from the first grade to the
    ///     highest age from the second grade, so grades 5-6 would be ages 10-12.
    /// @param firstGrade The first grade level score.
    /// @param secondGrade The second grade level score.
    /// @param displayFormat A ReadingAgeDisplay object specifying how to display the age.
    [[nodiscard]]
    static wxString GetAgeFromUSGrade(const size_t firstGrade, const size_t secondGrade,
                                      const ReadingAgeDisplay displayFormat);
    /// @returns a formatted description of a grade scale score (score will be in long format).
    /// @param value The grade level score.
    [[nodiscard]]
    wxString GetGradeScaleDescription(double value) const;
    /// @returns a formatted description of a grade scale score (score will be in long format).
    /// @param value The grade level score.
    [[nodiscard]]
    wxString GetGradeScaleDescription(const size_t value) const;
    /// @returns a formatted description of a grade scale range (score will be in long format).
    /// @param firstGrade The first grade level in the range.
    /// @param secondGrade The second grade level in the range.
    [[nodiscard]]
    wxString GetGradeScaleDescription(size_t firstGrade, size_t secondGrade) const;

    /// @returns The grade-scale formatted value(s) of a K12+ score (if long formatting is enabled).
    /// @param value The grade score.\n
    ///     Note that values such as "5-6" will be returned as something like "5th grade-6th grade".
    ///     Also, multiple tests (separated by a ';') can also be used here.
    /// @param format A wxNumberFormatInfo object, specifying the precision
    ///     to format the number into.\n
    ///     This only applies if using short format.
    /// @returns The grade level, formatted as a string.
    [[nodiscard]]
    wxString GetFormattedValue(const wxString& value,
                               const Wisteria::NumberFormatInfo& format) const final;
    /// @brief Returns a K12+ grade score as a string.
    /// @details If using short format, then the score is returned as the number.\n
    ///     If using long format, a description of the score's grade level
    ///     (e.g., "1st grade, second month of class") is returned.
    /// @param value The grade score.
    /// @param format A wxNumberFormatInfo object, specifying the precision to format
    ///     the number into. This only applies if using short format.
    /// @returns The grade level, formatted as a string.
    [[nodiscard]]
    wxString GetFormattedValue(const double value,
                               const Wisteria::NumberFormatInfo& format) const final;
    /// @param grade The grade level.
    /// @returns A long-format string value of a grade, regardless of whether long formatting is
    /// enabled
    [[nodiscard]]
    wxString GetGradeScaleLongLabel(size_t grade) const;

    /// @returns The active grade scale.
    [[nodiscard]]
    readability::grade_scale GetGradeScale() const noexcept
        {
        return m_gradeScale;
        }

    /** @brief Sets the grade scale to use for displaying test scores.
        @param gradeScale The grade scale to use.*/
    void SetGradeScale(const readability::grade_scale gradeScale) noexcept
        {
        m_gradeScale = gradeScale;
        }

    /// @return Whether grade values will be returned in long format
    ///     (e.g., "Kindergarten"). If false, then grade are simply formatted as numbers.
    [[nodiscard]]
    bool IsUsingLongGradeScaleFormat() const noexcept
        {
        return m_useLongFormatGradeScale;
        }

    /// @brief Specifies whether grades should be formatted into long format.
    /// @param useLongFormat @c true to enable long-format grade formatting,
    ///     @c false to format scores as numbers.
    void SetLongGradeScaleFormat(const bool useLongFormat) noexcept
        {
        m_useLongFormatGradeScale = useLongFormat;
        }

    /// @returns The age display for test scores to display.
    [[nodiscard]]
    ReadabilityMessages::ReadingAgeDisplay GetReadingAgeDisplay() const noexcept
        {
        return m_readingAgeDisplay;
        }

    /** @brief Sets the age display to use for test scores.
        @param displayFormat The age-display format to use.*/
    void SetReadingAgeDisplay(const ReadabilityMessages::ReadingAgeDisplay& displayFormat) noexcept
        {
        m_readingAgeDisplay = displayFormat;
        }

    /** @brief Converts a string score into a double.
        @details Doesn't use wxString::ToDouble because it expects the entire string to be a number
            and fails on "5-6".\n
            Instead, it uses `string_util::strtod_ex()` to handle range values
            (e.g., "13-15") more intelligently.
        @param valueStr The grade score (in string format) that needs to be parsed into a number.
        @param[out] value The double to write the score to.
        @returns @c true if conversion was successful, @c false otherwise.*/
    [[nodiscard]]
    static bool GetScoreValue(const wxString& valueStr, double& value);

  private:
    [[nodiscard]]
    static const wxString GetHighlightBegin()
        {
        return L"<span style='font-weight:bold;'><span style='font-style:italic;'>";
        }

    [[nodiscard]]
    static const wxString GetHighlightEnd()
        {
        return L"</span></span>";
        }

    [[nodiscard]]
    static wxString GetUSGradeScaleLabel(size_t value);
    // Canada
    [[nodiscard]]
    static wxString GetNewfoundlandAndLabradorScaleLabel(size_t value);
    [[nodiscard]]
    static wxString GetBritishColumbiaScaleLabel(size_t value);
    [[nodiscard]]
    static wxString GetNewBrunswickScaleLabel(size_t value);
    [[nodiscard]]
    static wxString GetNovaScotiaScaleLabel(size_t value);
    [[nodiscard]]
    static wxString GetOntarioScaleLabel(size_t value);
    [[nodiscard]]
    static wxString GetSaskatchewanScaleLabel(size_t value);
    [[nodiscard]]
    static wxString GetPrinceEdwardIslandScaleLabel(size_t value);
    [[nodiscard]]
    static wxString GetManitobaScaleLabel(size_t value);
    [[nodiscard]]
    static wxString GetNorthwestTerritoriesScaleLabel(size_t value);
    [[nodiscard]]
    static wxString GetAlbertaScaleLabel(size_t value);
    [[nodiscard]]
    static wxString GetNunavutScaleLabel(size_t value);
    [[nodiscard]]
    static wxString GetQuebecScaleLabel(size_t value);
    // United Kingdom
    [[nodiscard]]
    static wxString GetEnglandWalesScaleLabel(size_t value);

    [[nodiscard]]
    static wxString GetPostSecondaryScaleLabel(size_t value);

    readability::grade_scale m_gradeScale{ readability::grade_scale::k12_plus_united_states };
    bool m_useLongFormatGradeScale{ false };
    ReadabilityMessages::ReadingAgeDisplay m_readingAgeDisplay{
        ReadingAgeDisplay::ReadingAgeAsARange
    };
    };

    /** @}*/

#endif //__READABILITY_MESSAGES_H__
