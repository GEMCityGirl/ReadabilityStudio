#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>
#include "../../src/results_format/readability_messages.h"

using namespace Catch::Matchers;
using namespace readability;
using namespace Wisteria;

TEST_CASE("Readability Messages", "[messages][readability]")
    {
    SECTION("NovaScotiaScaleLabel")
        {
        ReadabilityMessages messages;
        messages.SetGradeScale(readability::grade_scale::k12_plus_nova_scotia);
        CHECK(messages.GetGradeScale() == readability::grade_scale::k12_plus_nova_scotia);
        messages.SetLongGradeScaleFormat(true);
        CHECK(messages.IsUsingLongGradeScaleFormat());
        CHECK(messages.GetFormattedValue(L"4.1", NumberFormatInfo(NumberFormatInfo::NumberFormatType::StandardFormatting, 1)) == L"Grade 4 (Elementary), first month of class completed");
        }
    SECTION("NewBrunswickScaleLabel")
        {
        ReadabilityMessages messages;
        messages.SetGradeScale(readability::grade_scale::k12_plus_newbrunswick);
        CHECK(messages.GetGradeScale() == readability::grade_scale::k12_plus_newbrunswick);
        messages.SetLongGradeScaleFormat(true);
        CHECK(messages.IsUsingLongGradeScaleFormat());
        CHECK(messages.GetFormattedValue(L"4.1", NumberFormatInfo(NumberFormatInfo::NumberFormatType::StandardFormatting, 1)) == L"Grade 4 (Elementary), first month of class completed");
        }
    SECTION("BritishColumbiaScaleLabel")
        {
        ReadabilityMessages messages;
        messages.SetGradeScale(readability::grade_scale::k12_plus_british_columbia);
        CHECK(messages.GetGradeScale() == readability::grade_scale::k12_plus_british_columbia);
        messages.SetLongGradeScaleFormat(true);
        CHECK(messages.IsUsingLongGradeScaleFormat());
        CHECK(messages.GetFormattedValue(L"4.1", NumberFormatInfo(NumberFormatInfo::NumberFormatType::StandardFormatting, 1)) == L"Grade 4 (Elementary), first month of class completed");
        }
    SECTION("NewfoundlandAndLabradorScaleLabel")
        {
        ReadabilityMessages messages;
        messages.SetGradeScale(readability::grade_scale::k12_plus_newfoundland_and_labrador);
        CHECK(messages.GetGradeScale() == readability::grade_scale::k12_plus_newfoundland_and_labrador);
        messages.SetLongGradeScaleFormat(true);
        CHECK(messages.IsUsingLongGradeScaleFormat());
        CHECK(messages.GetFormattedValue(L"4.1", NumberFormatInfo(NumberFormatInfo::NumberFormatType::StandardFormatting, 1)) == L"Grade 4 (Elementary), first month of class completed");
        }
    SECTION("AlbertaScaleLabel")
        {
        ReadabilityMessages messages;
        messages.SetGradeScale(readability::grade_scale::k12_plus_alberta);
        CHECK(messages.GetGradeScale() == readability::grade_scale::k12_plus_alberta);
        messages.SetLongGradeScaleFormat(true);
        CHECK(messages.IsUsingLongGradeScaleFormat());
        CHECK(messages.GetFormattedValue(L"4.1", NumberFormatInfo(NumberFormatInfo::NumberFormatType::StandardFormatting, 1)) == L"Grade 4 (Elementary), first month of class completed");
        }
    SECTION("OntarioScaleLabel")
        {
        ReadabilityMessages messages;
        messages.SetGradeScale(readability::grade_scale::k12_plus_ontario);
        CHECK(messages.GetGradeScale() == readability::grade_scale::k12_plus_ontario);
        messages.SetLongGradeScaleFormat(true);
        CHECK(messages.IsUsingLongGradeScaleFormat());
        CHECK(messages.GetFormattedValue(L"4.1", NumberFormatInfo(NumberFormatInfo::NumberFormatType::StandardFormatting, 1)) == L"Grade 4 (Elementary), first month of class completed");
        }
    SECTION("SaskatchewanScaleLabel")
        {
        ReadabilityMessages messages;
        messages.SetGradeScale(readability::grade_scale::k12_plus_saskatchewan);
        CHECK(messages.GetGradeScale() == readability::grade_scale::k12_plus_saskatchewan);
        messages.SetLongGradeScaleFormat(true);
        CHECK(messages.IsUsingLongGradeScaleFormat());
        CHECK(messages.GetFormattedValue(L"4.1", NumberFormatInfo(NumberFormatInfo::NumberFormatType::StandardFormatting, 1)) == L"Grade 4 (Elementary Level), first month of class completed");
        }
    SECTION("PrinceEdwardIslandScaleLabel")
        {
        ReadabilityMessages messages;
        messages.SetGradeScale(readability::grade_scale::k12_plus_prince_edward_island);
        CHECK(messages.GetGradeScale() == readability::grade_scale::k12_plus_prince_edward_island);
        messages.SetLongGradeScaleFormat(true);
        CHECK(messages.IsUsingLongGradeScaleFormat());
        CHECK(messages.GetFormattedValue(L"4.1", NumberFormatInfo(NumberFormatInfo::NumberFormatType::StandardFormatting, 1)) == L"Grade 4 (Elementary), first month of class completed");
        }
    SECTION("ManitobaScaleLabel")
        {
        ReadabilityMessages messages;
        messages.SetGradeScale(readability::grade_scale::k12_plus_manitoba);
        CHECK(messages.GetGradeScale() == readability::grade_scale::k12_plus_manitoba);
        messages.SetLongGradeScaleFormat(true);
        CHECK(messages.IsUsingLongGradeScaleFormat());
        CHECK(messages.GetFormattedValue(L"4.1", NumberFormatInfo(NumberFormatInfo::NumberFormatType::StandardFormatting, 1)) == L"Grade 4 (Early), first month of class completed");
        }
    SECTION("NorthwestTerritoriesScaleLabel")
        {
        ReadabilityMessages messages;
        messages.SetGradeScale(readability::grade_scale::k12_plus_northwest_territories);
        CHECK(messages.GetGradeScale() == readability::grade_scale::k12_plus_northwest_territories);
        messages.SetLongGradeScaleFormat(true);
        CHECK(messages.IsUsingLongGradeScaleFormat());
        CHECK(messages.GetFormattedValue(L"4.1", NumberFormatInfo(NumberFormatInfo::NumberFormatType::StandardFormatting, 1)) == L"Grade 4 (Intermediate), first month of class completed");
        }
    SECTION("NunavutScaleLabel")
        {
        ReadabilityMessages messages;
        messages.SetGradeScale(readability::grade_scale::k12_plus_nunavut);
        CHECK(messages.GetGradeScale() == readability::grade_scale::k12_plus_nunavut);
        messages.SetLongGradeScaleFormat(true);
        CHECK(messages.IsUsingLongGradeScaleFormat());
        CHECK(wxString(L"Grade 4, first month of class completed") == messages.GetFormattedValue(L"4.1", NumberFormatInfo(NumberFormatInfo::NumberFormatType::StandardFormatting, 1)));
        }
    SECTION("QuebecScaleLabel")
        {
        ReadabilityMessages messages;
        messages.SetGradeScale(readability::grade_scale::quebec);
        CHECK(messages.GetGradeScale() == readability::grade_scale::quebec);
        messages.SetLongGradeScaleFormat(true);
        CHECK(messages.IsUsingLongGradeScaleFormat());
        CHECK(wxString(L"Grade 4 (\xC9\x63\x6F\x6C\x65 Primaire), first month of class completed") ==
            messages.GetFormattedValue(L"4.1", NumberFormatInfo(NumberFormatInfo::NumberFormatType::StandardFormatting, 1)));
        }
    SECTION("GetAgeFromUSGrade")
        {
        CHECK(ReadabilityMessages::GetAgeFromUSGrade(5.4, ReadabilityMessages::ReadingAgeDisplay::ReadingAgeRoundToSemester) == L"10");
        CHECK(ReadabilityMessages::GetAgeFromUSGrade(5.4, ReadabilityMessages::ReadingAgeDisplay::ReadingAgeAsARange) == L"10-11");
        CHECK(ReadabilityMessages::GetAgeFromUSGrade(5.6, ReadabilityMessages::ReadingAgeDisplay::ReadingAgeRoundToSemester) == L"11");
        CHECK(ReadabilityMessages::GetAgeFromUSGrade(5.6, ReadabilityMessages::ReadingAgeDisplay::ReadingAgeAsARange) == L"10-11");
        }
    SECTION("GetAgeFromMultiUSGrade")
        {
        CHECK(ReadabilityMessages::GetAgeFromUSGrade(5,5, ReadabilityMessages::ReadingAgeDisplay::ReadingAgeRoundToSemester) == L"10");
        CHECK(ReadabilityMessages::GetAgeFromUSGrade(5,6, ReadabilityMessages::ReadingAgeDisplay::ReadingAgeRoundToSemester) == L"11");
        CHECK(ReadabilityMessages::GetAgeFromUSGrade(5,6, ReadabilityMessages::ReadingAgeDisplay::ReadingAgeAsARange) == L"10-12");
        CHECK(ReadabilityMessages::GetAgeFromUSGrade(13,15, ReadabilityMessages::ReadingAgeDisplay::ReadingAgeRoundToSemester) == L"19");
        CHECK(ReadabilityMessages::GetAgeFromUSGrade(13,15, ReadabilityMessages::ReadingAgeDisplay::ReadingAgeAsARange) == L"18-21");
        }
    SECTION("GetMonthLabel")
        {
        CHECK(ReadabilityMessages::GetMonthLabel(0) == _(L"first month of class"));
        CHECK(ReadabilityMessages::GetMonthLabel(9) == _(L"ninth month of class completed"));
        //don't have a 10th month in a school year
        CHECK(ReadabilityMessages::GetMonthLabel(10) == _(L"ninth month of class completed"));
        }
    SECTION("EnglandWales")
        {
        ReadabilityMessages messages;
        messages.SetGradeScale(readability::grade_scale::key_stages_england_wales);
        CHECK(messages.GetGradeScale() == readability::grade_scale::key_stages_england_wales);
        messages.SetLongGradeScaleFormat(true);
        CHECK(messages.IsUsingLongGradeScaleFormat());
        CHECK(messages.GetFormattedValue(L"4.1", NumberFormatInfo(NumberFormatInfo::NumberFormatType::StandardFormatting, 1)) == L"Key stage 2 (year 5), first month of class completed");
        }
    SECTION("GetScoreValue")
        {
        ReadabilityMessages messages;
        double value;
        CHECK(messages.GetScoreValue(L"1.2", value));
        CHECK_THAT(value, WithinRel(1.2, 0.01));
        CHECK(messages.GetScoreValue(L"13-15", value));
        CHECK_THAT(value, WithinRel(14, 0.01));
        CHECK(messages.GetScoreValue(L"4-5", value));
        CHECK_THAT(value, WithinRel(4.5, 0.01));
        CHECK(messages.GetScoreValue(L"", value) == false);
        CHECK_THAT(value, WithinRel(0, 0.01));
        CHECK(messages.GetScoreValue(L"bogus", value) == false);
        CHECK_THAT(value, WithinRel(0, 0.01));
        }
    SECTION("GetFormattedTextUSShortFormat")
        {
        ReadabilityMessages messages;
        messages.SetGradeScale(readability::grade_scale::k12_plus_united_states);
        CHECK(messages.IsUsingLongGradeScaleFormat() == false);
        CHECK(messages.GetFormattedValue(0.1, NumberFormatInfo(NumberFormatInfo::NumberFormatType::StandardFormatting, 1)) == L"0.1");
        CHECK(messages.GetFormattedValue(1.2, NumberFormatInfo(NumberFormatInfo::NumberFormatType::StandardFormatting, 1)) == L"1.2");
        CHECK(messages.GetFormattedValue(5.44, NumberFormatInfo(NumberFormatInfo::NumberFormatType::StandardFormatting, 1)) == L"5.4");
        CHECK(messages.GetFormattedValue(9.67, NumberFormatInfo(NumberFormatInfo::NumberFormatType::StandardFormatting, 1)) == L"9.7");
        CHECK(messages.GetFormattedValue(15.77, NumberFormatInfo(NumberFormatInfo::NumberFormatType::StandardFormatting, 1)) == L"15.8");
        CHECK(messages.GetGradeScaleLongLabel(0) == L"Kindergarten");
        }
    SECTION("GetFormattedTextUSLongFormat")
        {
        ReadabilityMessages messages;
        messages.SetGradeScale(readability::grade_scale::k12_plus_united_states);
        messages.SetLongGradeScaleFormat(true);
        CHECK(messages.IsUsingLongGradeScaleFormat());
        CHECK(messages.GetFormattedValue(0.1, NumberFormatInfo(NumberFormatInfo::NumberFormatType::StandardFormatting, 1)) == L"Kindergarten, first month of class completed");
        CHECK(messages.GetFormattedValue(9.67, NumberFormatInfo(NumberFormatInfo::NumberFormatType::StandardFormatting, 1)) == L"High school freshman, seventh month of class completed");
        CHECK(messages.GetFormattedValue(18.0, NumberFormatInfo(NumberFormatInfo::NumberFormatType::StandardFormatting, 1)) == L"Post-graduate sophomore, first month of class");
        CHECK(messages.GetGradeScaleLongLabel(0) == L"Kindergarten");
        CHECK(messages.GetGradeScaleLongLabel(5) == L"5th grade");
        CHECK(messages.GetGradeScaleLongLabel(13) == L"University freshman");
        }
    SECTION("GetFormattedTextFromStringUSLongFormat")
        {
        ReadabilityMessages messages;
        messages.SetGradeScale(readability::grade_scale::k12_plus_united_states);
        CHECK(messages.GetGradeScale() == readability::grade_scale::k12_plus_united_states);
        messages.SetLongGradeScaleFormat(true);
        CHECK(messages.IsUsingLongGradeScaleFormat());
        CHECK(messages.GetFormattedValue(L"0.1", NumberFormatInfo(NumberFormatInfo::NumberFormatType::StandardFormatting, 1)) == L"Kindergarten, first month of class completed");
        CHECK(messages.GetFormattedValue(L"9.67", NumberFormatInfo(NumberFormatInfo::NumberFormatType::StandardFormatting, 1)) == L"High school freshman, seventh month of class completed");
        //test formatting multiple strings
        CHECK(messages.GetFormattedValue(L"0.1;9.67", NumberFormatInfo(NumberFormatInfo::NumberFormatType::StandardFormatting, 1)) == L"Kindergarten, first month of class completed; High school freshman, seventh month of class completed");
        CHECK(messages.GetFormattedValue(L"0-1", NumberFormatInfo(NumberFormatInfo::NumberFormatType::StandardFormatting, 1)) == L"Kindergarten-1st grade");
        }
    }