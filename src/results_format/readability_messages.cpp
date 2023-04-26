///////////////////////////////////////////////////////////////////////////////
// Name:        readability_messages.cpp
// Author:      Blake Madden
// Copyright:   (c) 2005-2023 Blake Madden
// Licence:     3-Clause BSD licence
// SPDX-License-Identifier: BSD-3-Clause
///////////////////////////////////////////////////////////////////////////////

#include "readability_messages.h"

using namespace readability;
using namespace Wisteria;

//-----------------------------------------------
wxString ReadabilityMessages::GetGradeScaleLongLabel(size_t grade) const
    {
    if (GetGradeScale() == grade_scale::k12_plus_united_states)
        { return GetUSGradeScaleLabel(grade); }
    else if (GetGradeScale() == grade_scale::k12_plus_newfoundland_and_labrador)
        { return GetNewfoundlandAndLabradorScaleLabel(grade); }
    else if (GetGradeScale() == grade_scale::k12_plus_british_columbia)
        { return GetBritishColumbiaScaleLabel(grade); }
    else if (GetGradeScale() == grade_scale::k12_plus_newbrunswick)
        { return GetNewBrunswickScaleLabel(grade); }
    else if (GetGradeScale() == grade_scale::k12_plus_nova_scotia)
        { return GetNovaScotiaScaleLabel(grade); }
    else if (GetGradeScale() == grade_scale::k12_plus_ontario)
        { return GetOntarioScaleLabel(grade); }
    else if (GetGradeScale() == grade_scale::k12_plus_saskatchewan)
        { return GetSaskatchewanScaleLabel(grade); }
    else if (GetGradeScale() == grade_scale::k12_plus_prince_edward_island)
        { return GetPrinceEdwardIslandScaleLabel(grade); }
    else if (GetGradeScale() == grade_scale::k12_plus_manitoba)
        { return GetManitobaScaleLabel(grade); }
    else if (GetGradeScale() == grade_scale::k12_plus_northwest_territories)
        { return GetNorthwestTerritoriesScaleLabel(grade); }
    else if (GetGradeScale() == grade_scale::k12_plus_alberta)
        { return GetAlbertaScaleLabel(grade); }
    else if (GetGradeScale() == grade_scale::k12_plus_nunavut)
        { return GetNunavutScaleLabel(grade); }
    else if (GetGradeScale() == grade_scale::quebec)
        { return GetQuebecScaleLabel(grade); }
    else if (GetGradeScale() == grade_scale::key_stages_england_wales)
        { return GetEnglandWalesScaleLabel(grade); }
    else
        { return GetUSGradeScaleLabel(grade); }
    }

//-----------------------------------------------
wxString ReadabilityMessages::GetFormattedValue(const double value,
                                                const Wisteria::NumberFormatInfo& format) const
    {
    if (IsUsingLongGradeScaleFormat())
        {
        size_t grade, month;
        split_k12_plus_grade_score(value, grade, month);
        return GetGradeScaleLongLabel(grade) + L", " + GetMonthLabel(month);
        }
    else
        {
        return wxNumberFormatter::ToString(value, format.m_precision,
                                           wxNumberFormatter::Style::Style_NoTrailingZeroes);
        }
    }

//-----------------------------------------------
wxString ReadabilityMessages::GetFormattedValue(const wxString& value,
                                                const Wisteria::NumberFormatInfo& format) const
    {
    // just return value if not using long format
    if (!IsUsingLongGradeScaleFormat())
        { return value; }

    // split up a list of multiple scores
    wxStringTokenizer tkzLis(value, L";", wxTOKEN_STRTOK);
    if (tkzLis.CountTokens() > 1)
        {
        wxString fullString;
        while (tkzLis.HasMoreTokens() )
            { fullString += GetFormattedValue(tkzLis.GetNextToken(), format) + L"; "; }
        // chop off the last "; "
        if (fullString.length() > 2)
            { fullString.RemoveLast(2); }
        return fullString;
        }

    double numericValue(0);
    wxStringTokenizer tkz(value, L"-", wxTOKEN_STRTOK);
    // grade ranges (e.g., 5-6) need to be split and analyzed atomically
    if (tkz.CountTokens() > 1)
        {
        wxString fullValue;
        while (tkz.HasMoreTokens() )
            {
            if (GetScoreValue(tkz.GetNextToken(), numericValue) )
                {
                size_t grade, month;
                // month is not used, grade ranges shouldn't have this
                split_k12_plus_grade_score(numericValue, grade, month);
                fullValue += GetGradeScaleLongLabel(grade) + L"-";
                }
            }
        if (fullValue.length() > 0)
            { fullValue.RemoveLast(); }
        return fullValue;
        }
    else if (GetScoreValue(value, numericValue))
        {
        size_t grade, month;
        split_k12_plus_grade_score(numericValue, grade, month);
        return GetGradeScaleLongLabel(grade) + L", " + GetMonthLabel(month);
        }
    else
        { return value; }
    }

//-----------------------------------------------
wxString ReadabilityMessages::GetMonthLabel(size_t month)
    {
    if (month > 9)
        { month = 9; }

    switch (month)
        {
    case 0:
        return _("first month of class");
        break;
    case 1:
        return _("first month of class completed");
        break;
    case 2:
        return _("second month of class completed");
        break;
    case 3:
        return _("third month of class completed");
        break;
    case 4:
        return _("fourth month of class completed");
        break;
    case 5:
        return _("fifth month of class completed");
        break;
    case 6:
        return _("sixth month of class completed");
        break;
    case 7:
        return _("seventh month of class completed");
        break;
    case 8:
        return _("eighth month of class completed");
        break;
    case 9:
        return _("ninth month of class completed");
        break;
        }

    return wxEmptyString;
    }

//-----------------------------------------------------
wxString ReadabilityMessages::GetAgeFromUSGrade(double value, const ReadingAgeDisplay displayFormat)
    {
    // round the grade score to the month before converting to an age
    value = round_decimal_place(value, 10);
    if (displayFormat == ReadingAgeDisplay::ReadingAgeRoundToSemester)
        {
        const size_t age = static_cast<size_t>(round_to_integer(value)) + 5;
        return std::to_wstring(age);
        }
    else
        {
        return wxString::Format(L"%zu-%zu",
            static_cast<size_t>(std::floor(value))+5,
            static_cast<size_t>(std::floor(value))+6);
        }
    }

//-----------------------------------------------------
wxString ReadabilityMessages::GetAgeFromUSGrade(const size_t firstGrade, const size_t secondGrade,
                                                const ReadingAgeDisplay displayFormat)
    {
    if (firstGrade == secondGrade)
        { return GetAgeFromUSGrade(firstGrade, displayFormat); }
    if (displayFormat == ReadingAgeDisplay::ReadingAgeRoundToSemester)
        {
        const double age = safe_divide<double>((firstGrade+secondGrade),2) + 5;
        return std::to_wstring(static_cast<size_t>(round_to_integer(age)) );
        }
    else
        { return wxString::Format(L"%zu-%zu", firstGrade+5, secondGrade+6); }
    }

//-----------------------------------------------------
wxString ReadabilityMessages::GetUSGradeScaleLabel(size_t value)
    {
    value = truncate_k12_plus_grade(value);

    switch (value)
        {
    case 0:
        return _("Kindergarten");
        break;
    case 1:
        return _("1st grade");
        break;
    case 2:
        return _("2nd grade");
        break;
    case 3:
        return _("3rd grade");
        break;
    case 4:
        return _("4th grade");
        break;
    case 5:
        return _("5th grade");
        break;
    case 6:
        return _("6th grade");
        break;
    case 7:
        return _("7th grade");
        break;
    case 8:
        return _("8th grade");
        break;
    case 9:
        return _("High school freshman");
        break;
    case 10:
        return _("High school sophomore");
        break;
    case 11:
        return _("High school junior");
        break;
    case 12:
        return _("High school senior");
        break;
    default:
        return GetPostSecondaryScaleLabel(value);
        };
    }

//-----------------------------------------------------
wxString ReadabilityMessages::GetNewfoundlandAndLabradorScaleLabel(size_t value)
    {
    value = truncate_k12_plus_grade(value);

    switch (value)
        {
    case 0:
        return _("Kindergarten");
        break;
    case 1:
        return _("Grade 1 (Primary)");
        break;
    case 2:
        return _("Grade 2 (Primary)");
        break;
    case 3:
        return _("Grade 3 (Primary)");
        break;
    case 4:
        return _("Grade 4 (Elementary)");
        break;
    case 5:
        return _("Grade 5 (Elementary)");
        break;
    case 6:
        return _("Grade 6 (Elementary)");
        break;
    case 7:
        return _("Grade 7 (Intermediate)");
        break;
    case 8:
        return _("Grade 8 (Intermediate)");
        break;
    case 9:
        return _("Grade 9 (Intermediate)");
        break;
    case 10:
        return _("Senior High (Level I)");
        break;
    case 11:
        return _("Senior High (Level II)");
        break;
    case 12:
        return _("Senior High (Level III)");
        break;
    default:
        return GetPostSecondaryScaleLabel(value);
        };
    }

//-----------------------------------------------------
wxString ReadabilityMessages::GetBritishColumbiaScaleLabel(size_t value)
    {
    value = truncate_k12_plus_grade(value);

    switch (value)
        {
    case 0:
        return _("Kindergarten (Elementary)");
        break;
    case 1:
        return _("Grade 1 (Elementary)");
        break;
    case 2:
        return _("Grade 2 (Elementary)");
        break;
    case 3:
        return _("Grade 3 (Elementary)");
        break;
    case 4:
        return _("Grade 4 (Elementary)");
        break;
    case 5:
        return _("Grade 5 (Elementary)");
        break;
    case 6:
        return _("Grade 6 (Elementary)");
        break;
    case 7:
        return _("Grade 7 (Elementary)");
        break;
    case 8:
        return _("Grade 8 (Junior Secondary)");
        break;
    case 9:
        return _("Grade 9 (Junior Secondary)");
        break;
    case 10:
        return _("Grade 10 (Junior Secondary)");
        break;
    case 11:
        return _("Grade 11 (Senior Secondary)");
        break;
    case 12:
        return _("Grade 12 (Senior Secondary)");
        break;
    default:
        return GetPostSecondaryScaleLabel(value);
        };
    }

//-----------------------------------------------------
wxString ReadabilityMessages::GetNewBrunswickScaleLabel(size_t value)
    {
    value = truncate_k12_plus_grade(value);

    switch (value)
        {
    case 0:
        return _("Kindergarten (Elementary)");
        break;
    case 1:
        return _("Grade 1 (Elementary)");
        break;
    case 2:
        return _("Grade 2 (Elementary)");
        break;
    case 3:
        return _("Grade 3 (Elementary)");
        break;
    case 4:
        return _("Grade 4 (Elementary)");
        break;
    case 5:
        return _("Grade 5 (Elementary)");
        break;
    case 6:
        return _("Grade 6 (Middle School)");
        break;
    case 7:
        return _("Grade 7 (Middle School)");
        break;
    case 8:
        return _("Grade 8 (Middle School)");
        break;
    case 9:
        return _("Grade 9 (High School)");
        break;
    case 10:
        return _("Grade 10 (High School)");
        break;
    case 11:
        return _("Grade 11 (High School)");
        break;
    case 12:
        return _("Grade 12 (High School)");
        break;
    default:
        return GetPostSecondaryScaleLabel(value);
        };
    }

//-----------------------------------------------------
wxString ReadabilityMessages::GetNovaScotiaScaleLabel(size_t value)
    {
    value = truncate_k12_plus_grade(value);

    switch (value)
        {
    case 0:
        return _("Primary (Elementary)");
        break;
    case 1:
        return _("Grade 1 (Elementary)");
        break;
    case 2:
        return _("Grade 2 (Elementary)");
        break;
    case 3:
        return _("Grade 3 (Elementary)");
        break;
    case 4:
        return _("Grade 4 (Elementary)");
        break;
    case 5:
        return _("Grade 5 (Elementary)");
        break;
    case 6:
        return _("Grade 6 (Elementary)");
        break;
    case 7:
        return _("Grade 7 (Junior High)");
        break;
    case 8:
        return _("Grade 8 (Junior High)");
        break;
    case 9:
        return _("Grade 9 (Junior High)");
        break;
    case 10:
        return _("Grade 10 (Senior High)");
        break;
    case 11:
        return _("Grade 11 (Senior High)");
        break;
    case 12:
        return _("Grade 12 (Senior High)");
        break;
    default:
        return GetPostSecondaryScaleLabel(value);
        };
    }

//-----------------------------------------------------
wxString ReadabilityMessages::GetAlbertaScaleLabel(size_t value)
    {
    value = truncate_k12_plus_grade(value);

    switch (value)
        {
    case 0:
        return _("Kindergarten (Elementary)");
        break;
    case 1:
        return _("Grade 1 (Elementary)");
        break;
    case 2:
        return _("Grade 2 (Elementary)");
        break;
    case 3:
        return _("Grade 3 (Elementary)");
        break;
    case 4:
        return _("Grade 4 (Elementary)");
        break;
    case 5:
        return _("Grade 5 (Elementary)");
        break;
    case 6:
        return _("Grade 6 (Elementary)");
        break;
    case 7:
        return _("Grade 7 (Junior High)");
        break;
    case 8:
        return _("Grade 8 (Junior High)");
        break;
    case 9:
        return _("Grade 9 (Junior High)");
        break;
    case 10:
        return _("Grade 10 (Senior High)");
        break;
    case 11:
        return _("Grade 11 (Senior High)");
        break;
    case 12:
        return _("Grade 12 (Senior High)");
        break;
    default:
        return GetPostSecondaryScaleLabel(value);
        };
    }

//-----------------------------------------------------
wxString ReadabilityMessages::GetOntarioScaleLabel(size_t value)
    {
    value = truncate_k12_plus_grade(value);

    switch (value)
        {
    case 0:
        return _("Senior Kindergarten (Elementary)");
        break;
    case 1:
        return _("Grade 1 (Elementary)");
        break;
    case 2:
        return _("Grade 2 (Elementary)");
        break;
    case 3:
        return _("Grade 3 (Elementary)");
        break;
    case 4:
        return _("Grade 4 (Elementary)");
        break;
    case 5:
        return _("Grade 5 (Elementary)");
        break;
    case 6:
        return _("Grade 6 (Elementary)");
        break;
    case 7:
        return _("Grade 7 (Elementary)");
        break;
    case 8:
        return _("Grade 8 (Elementary)");
        break;
    case 9:
        return _("Grade 9 (Secondary)");
        break;
    case 10:
        return _("Grade 10 (Secondary)");
        break;
    case 11:
        return _("Grade 11 (Secondary)");
        break;
    case 12:
        return _("Grade 12 (Secondary)");
        break;
    default:
        return GetPostSecondaryScaleLabel(value);
        };
    }

//-----------------------------------------------------
wxString ReadabilityMessages::GetSaskatchewanScaleLabel(size_t value)
    {
    value = truncate_k12_plus_grade(value);

    switch (value)
        {
    case 0:
        return _("Kindergarten");
        break;
    case 1:
        return _("Grade 1 (Elementary Level)");
        break;
    case 2:
        return _("Grade 2 (Elementary Level)");
        break;
    case 3:
        return _("Grade 3 (Elementary Level)");
        break;
    case 4:
        return _("Grade 4 (Elementary Level)");
        break;
    case 5:
        return _("Grade 5 (Elementary Level)");
        break;
    case 6:
        return _("Grade 6 (Middle Level)");
        break;
    case 7:
        return _("Grade 7 (Middle Level)");
        break;
    case 8:
        return _("Grade 8 (Middle Level)");
        break;
    case 9:
        return _("Grade 9 (Middle Level)");
        break;
    case 10:
        return _("Grade 10 (Secondary Level)");
        break;
    case 11:
        return _("Grade 11 (Secondary Level)");
        break;
    case 12:
        return _("Grade 12 (Secondary Level)");
        break;
    default:
        return GetPostSecondaryScaleLabel(value);
        };
    }

//-----------------------------------------------------
wxString ReadabilityMessages::GetPrinceEdwardIslandScaleLabel(size_t value)
    {
    value = truncate_k12_plus_grade(value);

    switch (value)
        {
    case 0:
        return _("Kindergarten");
        break;
    case 1:
        return _("Grade 1 (Elementary)");
        break;
    case 2:
        return _("Grade 2 (Elementary)");
        break;
    case 3:
        return _("Grade 3 (Elementary)");
        break;
    case 4:
        return _("Grade 4 (Elementary)");
        break;
    case 5:
        return _("Grade 5 (Elementary)");
        break;
    case 6:
        return _("Grade 6 (Elementary)");
        break;
    case 7:
        return _("Grade 7 (Intermediate School)");
        break;
    case 8:
        return _("Grade 8 (Intermediate School)");
        break;
    case 9:
        return _("Grade 9 (Intermediate School)");
        break;
    case 10:
        return _("Grade 10 (Senior High)");
        break;
    case 11:
        return _("Grade 11 (Senior High)");
        break;
    case 12:
        return _("Grade 12 (Senior High)");
        break;
    default:
        return GetPostSecondaryScaleLabel(value);
        };
    }

//-----------------------------------------------------
wxString ReadabilityMessages::GetManitobaScaleLabel(size_t value)
    {
    value = truncate_k12_plus_grade(value);

    switch (value)
        {
    case 0:
        return _("Kindergarten (Early)");
        break;
    case 1:
        return _("Grade 1 (Early)");
        break;
    case 2:
        return _("Grade 2 (Early)");
        break;
    case 3:
        return _("Grade 3 (Early)");
        break;
    case 4:
        return _("Grade 4 (Early)");
        break;
    case 5:
        return _("Grade 5 (Middle)");
        break;
    case 6:
        return _("Grade 6 (Middle)");
        break;
    case 7:
        return _("Grade 7 (Middle)");
        break;
    case 8:
        return _("Grade 8 (Middle)");
        break;
    case 9:
        return _("Grade 9 (Senior 1)");
        break;
    case 10:
        return _("Grade 10 (Senior 2)");
        break;
    case 11:
        return _("Grade 11 (Senior 3)");
        break;
    case 12:
        return _("Grade 12 (Senior 4)");
        break;
    default:
        return GetPostSecondaryScaleLabel(value);
        };
    }

//-----------------------------------------------------
wxString ReadabilityMessages::GetNorthwestTerritoriesScaleLabel(size_t value)
    {
    value = truncate_k12_plus_grade(value);

    switch (value)
        {
    case 0:
        return _("Kindergarten (Primary)");
        break;
    case 1:
        return _("Grade 1 (Primary)");
        break;
    case 2:
        return _("Grade 2 (Primary)");
        break;
    case 3:
        return _("Grade 3 (Primary)");
        break;
    case 4:
        return _("Grade 4 (Intermediate)");
        break;
    case 5:
        return _("Grade 5 (Intermediate)");
        break;
    case 6:
        return _("Grade 6 (Intermediate)");
        break;
    case 7:
        return _("Grade 7 (Junior Secondary)");
        break;
    case 8:
        return _("Grade 8 (Junior Secondary)");
        break;
    case 9:
        return _("Grade 9 (Junior Secondary)");
        break;
    case 10:
        return _("Grade 10 (Senior Secondary)");
        break;
    case 11:
        return _("Grade 11 (Senior Secondary)");
        break;
    case 12:
        return _("Grade 12 (Senior Secondary)");
        break;
    default:
        return GetPostSecondaryScaleLabel(value);
        };
    }

//-----------------------------------------------------
wxString ReadabilityMessages::GetNunavutScaleLabel(size_t value)
    {
    value = truncate_k12_plus_grade(value);

    switch (value)
        {
    case 0:
        return _("Kindergarten");
        break;
    case 1:
        return _("Grade 1");
        break;
    case 2:
        return _("Grade 2");
        break;
    case 3:
        return _("Grade 3");
        break;
    case 4:
        return _("Grade 4");
        break;
    case 5:
        return _("Grade 5");
        break;
    case 6:
        return _("Grade 6");
        break;
    case 7:
        return _("Grade 7");
        break;
    case 8:
        return _("Grade 8");
        break;
    case 9:
        return _("Grade 9");
        break;
    case 10:
        return _("Grade 10");
        break;
    case 11:
        return _("Grade 11");
        break;
    case 12:
        return _("Grade 12");
        break;
    default:
        return GetPostSecondaryScaleLabel(value);
        };
    }

//-----------------------------------------------
wxString ReadabilityMessages::GetQuebecScaleLabel(size_t value)
    {
    // there are 20 grades in Quebec, just broken up differently
    value = truncate_k12_plus_grade(value);

    switch (value)
        {
    case 0:
        return _("Maternelle");
        break;
    case 1:
        return _("Grade 1 (\U000000C9col Primaire)");
        break;
    case 2:
        return _("Grade 2 (\U000000C9col Primaire)");
        break;
    case 3:
        return _("Grade 3 (\U000000C9col Primaire)");
        break;
    case 4:
        return _("Grade 4 (\U000000C9col Primaire)");
        break;
    case 5:
        return _("Grade 5 (\U000000C9col Primaire)");
        break;
    case 6:
        return _("Grade 6 (\U000000C9col Primaire)");
        break;
    case 7:
        return _("1 Secondaire (\U000000C9col Secondaire)");
        break;
    case 8:
        return _("2 Secondaire (\U000000C9col Secondaire)");
        break;
    case 9:
        return _("3 Secondaire (\U000000C9col Secondaire)");
        break;
    case 10:
        return _("4 Secondaire (\U000000C9col Secondaire)");
        break;
    case 11:
        return _("5 Secondaire (\U000000C9col Secondaire)");
        break;
    case 12:
        return _("CEGEP, year 1");
        break;
    case 13:
        return _("CEGEP, year 2");
        break;
    case 14:
        return _("University, year 1");
        break;
    case 15:
        return _("University, year 2");
        break;
    case 16:
        return _("University, year 3");
        break;
    // university graduate  system is the same in Quebec as K-12
    default:
        return GetPostSecondaryScaleLabel(value);
        };
    }

//-----------------------------------------------
wxString ReadabilityMessages::GetEnglandWalesScaleLabel(size_t value)
    {
    // there are 20 grades in the U.K., just broken up differently
    value = truncate_k12_plus_grade(value);

    switch (value)
        {
    case 0:
        return _("Key stage 1 (year 1)");
        break;
    case 1:
        return _("Key stage 1 (year 2)");
        break;
    case 2:
        return _("Key stage 2 (year 3)");
        break;
    case 3:
        return _("Key stage 2 (year 4)");
        break;
    case 4:
        return _("Key stage 2 (year 5)");
        break;
    case 5:
        return _("Key stage 2 (year 6)");
        break;
    case 6:
        return _("Key stage 3 (year 7)");
        break;
    case 7:
        return _("Key stage 3 (year 8)");
        break;
    case 8:
        return _("Key stage 3 (year 9)");
        break;
    case 9:
        return _("Key stage 4 (year 10)");
        break;
    case 10:
        return _("Key stage 4 (year 11)");
        break;
    case 11:
        return _("Sixth Form, year 1");
        break;
    case 12:
        return _("Sixth Form, year 2");
        break;
    // university graduate  system is the same in Quebec as K-12
    default:
        return GetPostSecondaryScaleLabel(value);
        };
    }

//-----------------------------------------------------
wxString ReadabilityMessages::GetPostSecondaryScaleLabel(size_t value)
    {
    switch (value)
        {
    case 13:
        return _("University freshman");
        break;
    case 14:
        return _("University sophomore");
        break;
    case 15:
        return _("University junior");
        break;
    case 16:
        return _("University senior");
        break;
    case 17:
        return _("Post-graduate freshman");
        break;
    case 18:
        return _("Post-graduate sophomore");
        break;
    case 19:
        return _("Doctorate");
        break;
    default:
        return wxEmptyString;
        };
    }

//-----------------------------------------------------
wxString ReadabilityMessages::GetGradeScaleDescription(double value) const
    {
    size_t grade, month;
    split_k12_plus_grade_score(value, grade, month);

    return wxString::Format(
        _("This document is at least suitable for a reader at the %s%s, %s%s level."),
        GetHighlightBegin(),
        GetGradeScaleLongLabel(grade),
        GetMonthLabel(month),
        GetHighlightEnd());
    }

//-----------------------------------------------------
wxString ReadabilityMessages::GetGradeScaleDescription(size_t value) const
    {
    // make sure that reasonable values were entered
    value = truncate_k12_plus_grade(value);

    return wxString::Format(
        _("This document is at least suitable for a reader at the %s%s%s level."),
        GetHighlightBegin(),
        GetGradeScaleLongLabel(value),
        GetHighlightEnd());
    }

//-----------------------------------------------------
wxString ReadabilityMessages::GetGradeScaleDescription(size_t firstGrade, size_t secondGrade) const
    {
    // make sure that reasonable values were entered
    firstGrade = truncate_k12_plus_grade(firstGrade);
    secondGrade = truncate_k12_plus_grade(secondGrade);

    return wxString::Format(
        _("This document is at least suitable for a reader between the %s%s%s to %s%s%s level."),
        GetHighlightBegin(),
        GetGradeScaleLongLabel(firstGrade),
        GetHighlightEnd(),
        GetHighlightBegin(),
        GetGradeScaleLongLabel(secondGrade),
        GetHighlightEnd());
    }

//-----------------------------------------------------
wxString ReadabilityMessages::GetDanielsonBryan2Description(const readability::flesch_difficulty diffLevel)
    {
    wxString description;
    switch (diffLevel)
        {
    case readability::flesch_difficulty::flesch_very_difficult:
        description = _("Text is very difficult to read, college level.");
        break;
    case readability::flesch_difficulty::flesch_difficult:
        description = _("Text is difficult to read, high school level.");
        break;
    case readability::flesch_difficulty::flesch_fairly_difficult:
        description = _("Text is fairly difficult to read, junior high school level.");
        break;
    case readability::flesch_difficulty::flesch_standard:
        description = _("Text is average in terms of reading difficulty, sixth-grade level.");
        break;
    case readability::flesch_difficulty::flesch_fairly_easy:
        description = _("Text is fairly easy to read, fifth-grade level.");
        break;
    case readability::flesch_difficulty::flesch_easy:
        description = _("Text is easy to read, fourth-grade level.");
        break;
    case readability::flesch_difficulty::flesch_very_easy:
        description = _("Text is very easy to read, third-grade level.");
        break;
    default:
        description = _("Unable to determine difficulty.");
        break;
        };
    return description;
    }

//-----------------------------------------------------
wxString ReadabilityMessages::GetFleschDescription(const readability::flesch_difficulty diffLevel)
    {
    wxString description;
    switch (diffLevel)
        {
    case readability::flesch_difficulty::flesch_very_difficult:
        description = _("Text is very difficult to read.");
        break;
    case readability::flesch_difficulty::flesch_difficult:
        description = _("Text is difficult to read.");
        break;
    case readability::flesch_difficulty::flesch_fairly_difficult:
        description = _("Text is fairly difficult to read.");
        break;
    case readability::flesch_difficulty::flesch_standard:
        description = _("Text is average (i.e., \"Plain English\") in terms of reading difficulty.");
        break;
    case readability::flesch_difficulty::flesch_fairly_easy:
        description = _("Text is fairly easy to read.");
        break;
    case readability::flesch_difficulty::flesch_easy:
        description = _("Text is easy to read.");
        break;
    case readability::flesch_difficulty::flesch_very_easy:
        description = _("Text is very easy to read.");
        break;
    default:
        description = _("Unable to determine difficulty.");
        break;
        };
    return description;
    }

//-----------------------------------------------------
wxString ReadabilityMessages::GetEflawDescription(const readability::eflaw_difficulty diffLevel)
    {
    switch (diffLevel)
        {
    case readability::eflaw_difficulty::eflaw_very_confusing:
        return _("Text is very confusing to read.");
        break;
    case readability::eflaw_difficulty::eflaw_confusing:
        return _("Text is confusing to read.");
        break;
    case readability::eflaw_difficulty::eflaw_easy:
        return _("Text is easy to read.");
        break;
    case readability::eflaw_difficulty::eflaw_very_easy:
        return _("Text is very easy to read.");
        break;
    default:
        return _("Unable to determine difficulty.");
        };
    }

//-----------------------------------------------------
wxString ReadabilityMessages::GetLixDescription(const readability::lix_difficulty diffLevel)
    {
    switch (diffLevel)
        {
    case readability::lix_difficulty::lix_very_difficult:
        return _("Text is very difficult to read.");
    case readability::lix_difficulty::lix_difficult:
        return _("Text is difficult to read.");
    case readability::lix_difficulty::lix_average:
        return _("Text is average in terms of reading difficulty.");
    case readability::lix_difficulty::lix_easy:
        return _("Text is easy to read.");
    case readability::lix_difficulty::lix_very_easy:
        return _("Text is very easy to read.");
    default:
        return _("Unable to determine difficulty.");
        };
    }

//-----------------------------------------------------
wxString ReadabilityMessages::GetLixDescription(const readability::german_lix_difficulty diffLevel)
    {
    switch (diffLevel)
        {
    case readability::german_lix_difficulty::german_lix_very_easy:
        return _("Text is very easy to read.");
    case readability::german_lix_difficulty::german_lix_children_and_youth:
        return _("Text is at a children and young adult level of difficulty.");
    case readability::german_lix_difficulty::german_lix_easy:
        return _("Text is easy to read.");
    case readability::german_lix_difficulty::german_lix_adult_fiction:
        return _("Text is at an adult fiction level of difficulty.");
    case readability::german_lix_difficulty::german_lix_average:
        return _("Text is average in terms of reading difficulty.");
    case readability::german_lix_difficulty::german_lix_nonfiction:
        return _("Text is at a non-fiction level of difficulty.");
    case readability::german_lix_difficulty::german_lix_difficult:
        return _("Text is difficult to read.");
    case readability::german_lix_difficulty::german_lix_technical:
        return _("Text is at a technical document level of difficulty.");
    case readability::german_lix_difficulty::german_lix_very_difficult:
        return _("Text is very difficult to read.");
    default:
        return _("Unable to determine difficulty.");
        };
    }

//-------------------------------------------------------
wxString ReadabilityMessages::GetFraseDescription(const size_t frazeLevel)
    {
    switch (frazeLevel)
        {
    case 1:
        return _("Beginning (Level I)");
    case 2:
        return _("Intermediate (Level II)");
    case 3:
        return _("Advanced Intermediate (Level III)");
    case 4:
        return _("Advanced (Level IV)");
    default:
        return _("Unknown Level");
        };
    }

//-------------------------------------------------------
wxString ReadabilityMessages::GetDrpUnitDescription(const size_t drpScore)
    {
    return wxString::Format(_("DRP (difficulty) Units: %zu"), drpScore);
    }

//-------------------------------------------------------
wxString ReadabilityMessages::GetPredictedClozeDescription(const int clozeScore)
    {
    return wxString::Format(_("Predicted mean cloze score: %i%%"), clozeScore);
    }

//-------------------------------------------------------
bool ReadabilityMessages::GetScoreValue(const wxString& valueStr, double& value)
    {
    const wxChar* const valueStrPtr = valueStr.wc_str();
    wxChar* end = nullptr;

    value = string_util::strtod_ex(valueStrPtr, &end);
    if (valueStrPtr != end)
        { return true; }
    else
        { return false; }
    }
