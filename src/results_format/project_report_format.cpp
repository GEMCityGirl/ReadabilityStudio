///////////////////////////////////////////////////////////////////////////////
// Name:        project_report_format.cpp
// Author:      Blake Madden
// Copyright:   (c) 2005-2023 Blake Madden
// Licence:     3-Clause BSD licence
// SPDX-License-Identifier: BSD-3-Clause
///////////////////////////////////////////////////////////////////////////////

#include "project_report_format.h"
#include "../app/readability_app.h"
#include "../projects/base_project_view.h"
#include "../projects/base_project_doc.h"

DECLARE_APP(ReadabilityApp)

//------------------------------------------------
wxString ProjectReportFormat::GetStemmingDisplayName(const stemming::stemming_type stemType)
    {
    switch (stemType)
        {
    case stemming::stemming_type::danish:
        return _(L"Danish");
        break;
    case stemming::stemming_type::dutch:
        return _(L"Dutch");
        break;
    case stemming::stemming_type::english:
        return _(L"English");
        break;
    case stemming::stemming_type::finnish:
        return _(L"Finnish");
        break;
    case stemming::stemming_type::french:
        return _(L"French");
        break;
    case stemming::stemming_type::german:
        return _(L"German");
        break;
    case stemming::stemming_type::italian:
        return _(L"Italian");
        break;
    case stemming::stemming_type::norwegian:
        return _(L"Norwegian");
        break;
    case stemming::stemming_type::portuguese:
        return _(L"Portuguese");
        break;
    case stemming::stemming_type::spanish:
        return _(L"Spanish");
        break;
    case stemming::stemming_type::swedish:
        return _(L"Swedish");
        break;
    case stemming::stemming_type::no_stemming:
    default:
        return _(L"Do not using stemming");
        break;
        }
    }

//------------------------------------------------
wxString ProjectReportFormat::FormatFormulaToHtml(const wxString& formula)
    {
    if (formula.length() == 0)
        { return formula; }
    wxString formattedFormula = L"<tt>";
    const wxString wordFormatStart = L"<span>";
    const wxString operatorFormatStart = L"<span style='font-weight:bold;'>";
    for (size_t i = 0; i < formula.length(); /*handled in lop*/)
        {
        if (string_util::is_one_of<wchar_t>(formula[i],
            L"ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz"))
            {
            const size_t endOfWord =
                formula.find_first_not_of(L"ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz1234567890", i);
            if (endOfWord == -1)
                {
                formattedFormula += wordFormatStart;
                formattedFormula += formula[i];
                formattedFormula += L"</span>";
                break;
                }
            formattedFormula += wordFormatStart;
            formattedFormula += formula.substr(i, endOfWord-i);
            formattedFormula += L"</span>";
            i = endOfWord;
            }
        else
            {
            if (string_util::is_one_of<wchar_t>(formula[i], L"-+*/%^,();"))
                {
                formattedFormula += operatorFormatStart;
                formattedFormula += formula[i];
                formattedFormula += L"</span>";
                }
            else if (formula[i] == L'<' && i+1 < formula.length() && formula[i+1] == L'=')
                {
                formattedFormula += operatorFormatStart + L"&le;</span>";
                ++i;
                }
            else if (formula[i] == L'>' && i+1 < formula.length() && formula[i+1] == L'=')
                {
                formattedFormula += operatorFormatStart + L"&ge;</span>";
                ++i;
                }
            else if (formula[i] == L'<')
                { formattedFormula += operatorFormatStart + L"&lt;</span>"; }
            else if (formula[i] == L'>')
                { formattedFormula += operatorFormatStart + L"&gt;</span>"; }
            else if (formula[i] == L'\'')
                { formattedFormula += L"&apos;"; }
            else if (formula[i] == L'\"')
                { formattedFormula += L"&quot;"; }
            else if (formula[i] == L' ')
                { formattedFormula += L"&nbsp;"; }
            else if (formula[i] == 10 || formula[i] == 13)
                {
                // treats CRLF combo as one break, so make one extra step for CRLF combination
                // so that it counts as only one line break
                if (i < formula.length()-1 &&
                    (formula[i+1] == 10 ||
                    formula[i+1] == 13) )
                    { ++i; }
                formattedFormula += L"<br />";
                }
            else
                { formattedFormula += formula[i]; }
            ++i;
            }
        }
    formattedFormula += L"</tt>";
    return formattedFormula;
    }

//------------------------------------------------
wxString ProjectReportFormat::FormatDolchStatisticsInfo(const BaseProject* project,
                                                        const bool includeExplanation,
                                                        const wxColour attentionColor,
                                                        ListCtrlExDataProviderBase* listData)
    {
    wxASSERT(project);
    if (!project)
        { return wxEmptyString; }
    if (!project->IsIncludingDolchSightWords())
        { return wxEmptyString; }
    size_t listDataItemCount = 0;
    if (listData)
        {
        listDataItemCount = listData->GetItemCount();
        listData->SetSize(listDataItemCount+MAX_SUMMARY_STAT_ROWS, 3);
        }

    const size_t totalDolchWords =
        project->GetDolchConjunctionCounts().second + project->GetDolchPrepositionWordCounts().second +
        project->GetDolchPronounCounts().second +
        project->GetDolchAdverbCounts().second + project->GetDolchAdjectiveCounts().second +
        project->GetDolchVerbsCounts().second + project->GetDolchNounCounts().second;
    const size_t totalDolchWordsExcludingNouns =
        project->GetDolchConjunctionCounts().second + project->GetDolchPrepositionWordCounts().second +
        project->GetDolchPronounCounts().second +
        project->GetDolchAdverbCounts().second + project->GetDolchAdjectiveCounts().second +
        project->GetDolchVerbsCounts().second;
    const double totalDolchPercentage = safe_divide<double>(totalDolchWords,project->GetTotalWords())*100;
    const double totalDolchExcludingNounsPercentage =
        safe_divide<double>(totalDolchWordsExcludingNouns,project->GetTotalWords())*100;

    const bool containsHighPercentageOfNonDochWords =
        ((totalDolchPercentage < 70) || (totalDolchExcludingNounsPercentage< 60));
    wxString HTMLText;

    if (!project->GetStatisticsReportInfo().HasDolchStatisticsEnabled())
        { HTMLText += _(L"No Dolch statistics <a href=\"#SelectStatistics\">currently selected</a>."); }

    wxBitmap bmp(100,100);
    wxMemoryDC measuringDc(bmp);
    measuringDc.SetFont(measuringDc.GetFont().Larger().Larger());
    // will likely be the longest label (even after translation)
    const auto labelColumnWidth =
        safe_divide<long>(measuringDc.GetTextExtent(_(L"Number of Dolch words (excluding nouns):")).GetWidth(),
            wxGetApp().GetDPIScaleFactor());
    // the total number of words would be the largest (i.e., widest)
    // number possible, so measure using that
    const auto numberColumnWidth = safe_divide<long>(
        measuringDc.GetTextExtent(std::to_wstring(project->GetTotalWords())).GetWidth(),
        wxGetApp().GetDPIScaleFactor());

    const wxString tableStart = L"<table class='minipage' border='0' style='width:100%'>";

    const auto formatHeader = [](const wxString& label)
        {
        return wxString::Format(
            L"\n<tr class='report-column-header' style='background:%s;'>"
             "<td colspan='3'><span style='color:%s;'>%s</span></td></tr>",
            GetReportHeaderColor().GetAsString(wxC2S_HTML_SYNTAX),
            GetReportHeaderFontColor().GetAsString(wxC2S_HTML_SYNTAX),
            label);
        };

    const auto formatRow = [labelColumnWidth, numberColumnWidth]
                            (const wxString& label, const wxString& value, const wxString& percent)
        {
        return wxString::Format(
            L"\n<tr><td style='min-width:%ipx; width:40%%;'>%s</td>"
             "<td style='text-align:right; width:%ipx;'>%s</td>"
             "<td style='text-align:left;'>%s</td></tr>",
            labelColumnWidth, label, numberColumnWidth, value, percent);
        };

    if (project->GetStatisticsReportInfo().IsDolchCoverageEnabled())
        {
        // list completions
        HTMLText += tableStart + formatHeader(_(L"Dolch Word Coverage"));

        const double dolchConjuctionPercentage =
            safe_divide<double>((MAX_DOLCH_CONJUNCTION_WORDS - project->GetUnusedDolchConjunctions()),
                MAX_DOLCH_CONJUNCTION_WORDS) * 100;
        const double dolchPronounsPercentage =
            safe_divide<double>((MAX_DOLCH_PRONOUN_WORDS - project->GetUnusedDolchPronouns()),
                MAX_DOLCH_PRONOUN_WORDS) * 100;
        const double dolchPrepositionsPercentage =
            safe_divide<double>((MAX_DOLCH_PREPOSITION_WORDS - project->GetUnusedDolchPrepositions()),
                MAX_DOLCH_PREPOSITION_WORDS) * 100;
        const double dolchAdverbsPercentage =
            safe_divide<double>((MAX_DOLCH_ADVERB_WORDS - project->GetUnusedDolchAdverbs()),
                MAX_DOLCH_ADVERB_WORDS) * 100;
        const double dolchAdjectivesPercentage =
            safe_divide<double>((MAX_DOLCH_ADJECTIVE_WORDS - project->GetUnusedDolchAdjectives()),
                MAX_DOLCH_ADJECTIVE_WORDS) * 100;
        const double dolchVerbsPercentage =
            safe_divide<double>((MAX_DOLCH_VERBS - project->GetUnusedDolchVerbs()),
                MAX_DOLCH_VERBS) * 100;
        const double dolchNounPercentage =
            safe_divide<double>((MAX_DOLCH_NOUNS - project->GetUnusedDolchNouns()),
                MAX_DOLCH_NOUNS) * 100;

        // Conjunctions
            {
            const wxString valueStr = (dolchConjuctionPercentage >= 75) ?
                wxString::Format(L"<span style=\"color:%s\">", attentionColor.GetAsString(wxC2S_HTML_SYNTAX)) +
                wxNumberFormatter::ToString(MAX_DOLCH_CONJUNCTION_WORDS - project->GetUnusedDolchConjunctions(), 0,
                    wxNumberFormatter::Style::Style_NoTrailingZeroes|
                    wxNumberFormatter::Style::Style_WithThousandsSep) +
                L"</span>" :
                wxNumberFormatter::ToString(MAX_DOLCH_CONJUNCTION_WORDS - project->GetUnusedDolchConjunctions(), 0,
                    wxNumberFormatter::Style::Style_NoTrailingZeroes|
                    wxNumberFormatter::Style::Style_WithThousandsSep);
            const wxString percentStr = (dolchConjuctionPercentage >= 75) ?
                wxString::Format(L"<span style=\"color:%s\">", attentionColor.GetAsString(wxC2S_HTML_SYNTAX)) +
                wxString(L"(" + wxNumberFormatter::ToString(
                    dolchConjuctionPercentage, 1,
                    wxNumberFormatter::Style::Style_NoTrailingZeroes) +
                    L"% " + _(L"of all Dolch conjunctions")) + L")</span>" :
                wxString(L"(" + wxNumberFormatter::ToString(
                    dolchConjuctionPercentage, 1,
                    wxNumberFormatter::Style::Style_NoTrailingZeroes) +
                    L"% " + _(L"of all Dolch conjunctions") + wxString(L")"));
            HTMLText += formatRow(_(L"Conjunctions used:"), valueStr, percentStr);

            if (listData)
                {
                listData->SetItemText(listDataItemCount, 0, _(L"Conjunctions used:"));
                listData->SetItemText(listDataItemCount, 1,
                    wxNumberFormatter::ToString(MAX_DOLCH_CONJUNCTION_WORDS-project->GetUnusedDolchConjunctions(), 0,
                        wxNumberFormatter::Style::Style_NoTrailingZeroes|
                        wxNumberFormatter::Style::Style_WithThousandsSep));
                listData->SetItemText(listDataItemCount++, 2, wxNumberFormatter::ToString(
                    dolchConjuctionPercentage, 1, wxNumberFormatter::Style::Style_NoTrailingZeroes) +
                    L"% " + _(L"of all Dolch conjunctions"));
                }
            }
        // Prepositions
            {
            const wxString valueStr = (dolchPrepositionsPercentage >= 75) ?
                wxString::Format(L"<span style=\"color:%s\">", attentionColor.GetAsString(wxC2S_HTML_SYNTAX)) +
                wxNumberFormatter::ToString(
                    MAX_DOLCH_PREPOSITION_WORDS-project->GetUnusedDolchPrepositions(), 0, true) + L"</span>" :
                wxNumberFormatter::ToString(
                    MAX_DOLCH_PREPOSITION_WORDS-project->GetUnusedDolchPrepositions(), 0,
                    wxNumberFormatter::Style::Style_NoTrailingZeroes|
                    wxNumberFormatter::Style::Style_WithThousandsSep);
            const wxString percentStr = (dolchPrepositionsPercentage >= 75) ?
                wxString::Format(L"<span style=\"color:%s\">", attentionColor.GetAsString(wxC2S_HTML_SYNTAX)) +
                wxString(L"(" +
                    wxNumberFormatter::ToString(dolchPrepositionsPercentage, 1,
                        wxNumberFormatter::Style::Style_NoTrailingZeroes) +
                    L"% " + _(L"of all Dolch prepositions")) + L")</span>" :
                wxString(L"(" +
                    wxNumberFormatter::ToString(dolchPrepositionsPercentage, 1,
                        wxNumberFormatter::Style::Style_NoTrailingZeroes) +
                    L"% " + _(L"of all Dolch prepositions") + wxString(L")"));
            HTMLText += formatRow(_(L"Prepositions used:"), valueStr, percentStr);

            if (listData)
                {
                listData->SetItemText(listDataItemCount, 0, _(L"Prepositions used:"));
                listData->SetItemText(listDataItemCount, 1,
                    wxNumberFormatter::ToString(MAX_DOLCH_PREPOSITION_WORDS-project->GetUnusedDolchPrepositions(), 0,
                        wxNumberFormatter::Style::Style_NoTrailingZeroes|
                        wxNumberFormatter::Style::Style_WithThousandsSep));
                listData->SetItemText(listDataItemCount++, 2,
                    wxNumberFormatter::ToString(
                        dolchPrepositionsPercentage, 1, wxNumberFormatter::Style::Style_NoTrailingZeroes) +
                    L"% " + _(L"of all Dolch prepositions"));
                }
            }
        // Pronouns
            {
            const wxString valueStr = (dolchPronounsPercentage >= 75) ?
                wxString::Format(L"<span style=\"color:%s\">", attentionColor.GetAsString(wxC2S_HTML_SYNTAX)) +
                wxNumberFormatter::ToString(
                    MAX_DOLCH_PRONOUN_WORDS-project->GetUnusedDolchPronouns(), 0,
                    wxNumberFormatter::Style::Style_NoTrailingZeroes|
                    wxNumberFormatter::Style::Style_WithThousandsSep) +
                L"</span>" :
                wxNumberFormatter::ToString(
                    MAX_DOLCH_PRONOUN_WORDS-project->GetUnusedDolchPronouns(), 0,
                    wxNumberFormatter::Style::Style_NoTrailingZeroes|
                    wxNumberFormatter::Style::Style_WithThousandsSep);
            const wxString percentStr = (dolchPronounsPercentage >= 75) ?
                wxString::Format(L"<span style=\"color:%s\">", attentionColor.GetAsString(wxC2S_HTML_SYNTAX)) +
                wxString(L"(" + wxNumberFormatter::ToString(
                    dolchPronounsPercentage, 1, wxNumberFormatter::Style::Style_NoTrailingZeroes) +
                L"% " + _(L"of all Dolch pronouns")) + L")</span>" :
                wxString(L"(" + wxNumberFormatter::ToString(
                    dolchPronounsPercentage, 1, wxNumberFormatter::Style::Style_NoTrailingZeroes) +
                L"% " + _(L"of all Dolch pronouns") + wxString(L")"));
            HTMLText += formatRow(_(L"Pronouns used:"), valueStr, percentStr);

            if (listData)
                {
                listData->SetItemText(listDataItemCount, 0, _(L"Pronouns used:"));
                listData->SetItemText(listDataItemCount, 1,
                    wxNumberFormatter::ToString(MAX_DOLCH_PRONOUN_WORDS-project->GetUnusedDolchPronouns(), 0,
                        wxNumberFormatter::Style::Style_NoTrailingZeroes|
                        wxNumberFormatter::Style::Style_WithThousandsSep));
                listData->SetItemText(listDataItemCount++, 2, wxNumberFormatter::ToString(
                    dolchPronounsPercentage, 1, wxNumberFormatter::Style::Style_NoTrailingZeroes) +
                L"% " + _(L"of all Dolch pronouns"));
                }
            }
        // Adverbs
            {
            const wxString valueStr = (dolchAdverbsPercentage >= 75) ?
                wxString::Format(L"<span style=\"color:%s\">", attentionColor.GetAsString(wxC2S_HTML_SYNTAX)) +
                wxNumberFormatter::ToString(MAX_DOLCH_ADVERB_WORDS-project->GetUnusedDolchAdverbs(), 0,
                    wxNumberFormatter::Style::Style_NoTrailingZeroes|
                    wxNumberFormatter::Style::Style_WithThousandsSep) +
                L"</span>" :
                wxNumberFormatter::ToString(MAX_DOLCH_ADVERB_WORDS-project->GetUnusedDolchAdverbs(), 0,
                    wxNumberFormatter::Style::Style_NoTrailingZeroes|
                    wxNumberFormatter::Style::Style_WithThousandsSep);
            const wxString percentStr = (dolchAdverbsPercentage >= 75) ?
                wxString::Format(L"<span style=\"color:%s\">", attentionColor.GetAsString(wxC2S_HTML_SYNTAX)) +
                wxString(L"(" + wxNumberFormatter::ToString(
                    dolchAdverbsPercentage, 1, wxNumberFormatter::Style::Style_NoTrailingZeroes) +
                    L"% " + _(L"of all Dolch adverbs")) + L")</span>" :
                wxString(L"(" + wxNumberFormatter::ToString(
                    dolchAdverbsPercentage, 1, wxNumberFormatter::Style::Style_NoTrailingZeroes) +
                    L"% " + _(L"of all Dolch adverbs") + wxString(L")"));
            HTMLText += formatRow(_(L"Adverbs used:"), valueStr, percentStr);

            if (listData)
                {
                listData->SetItemText(listDataItemCount, 0, _(L"Adverbs used:"));
                listData->SetItemText(listDataItemCount, 1,
                    wxNumberFormatter::ToString(MAX_DOLCH_ADVERB_WORDS-project->GetUnusedDolchAdverbs(), 0,
                        wxNumberFormatter::Style::Style_NoTrailingZeroes|
                        wxNumberFormatter::Style::Style_WithThousandsSep));
                listData->SetItemText(listDataItemCount++, 2, wxNumberFormatter::ToString(
                    dolchAdverbsPercentage, 1,
                    wxNumberFormatter::Style::Style_NoTrailingZeroes) + L"% " + _(L"of all Dolch adverbs"));
                }
            }
        // Adjectives
            {
            const wxString valueStr = (dolchAdjectivesPercentage >= 75) ?
                    wxString::Format(L"<span style=\"color:%s\">", attentionColor.GetAsString(wxC2S_HTML_SYNTAX)) +
                wxNumberFormatter::ToString(MAX_DOLCH_ADJECTIVE_WORDS-project->GetUnusedDolchAdjectives(), 0,
                    wxNumberFormatter::Style::Style_NoTrailingZeroes|
                    wxNumberFormatter::Style::Style_WithThousandsSep) +
                    L"</span>":
                wxNumberFormatter::ToString(MAX_DOLCH_ADJECTIVE_WORDS-project->GetUnusedDolchAdjectives(), 0,
                    wxNumberFormatter::Style::Style_NoTrailingZeroes|
                    wxNumberFormatter::Style::Style_WithThousandsSep);
            const wxString percentStr = (dolchAdjectivesPercentage >= 75) ?
                wxString::Format(L"<span style=\"color:%s\">", attentionColor.GetAsString(wxC2S_HTML_SYNTAX)) +
                wxString(L"(" + wxNumberFormatter::ToString(
                    dolchAdjectivesPercentage, 1, wxNumberFormatter::Style::Style_NoTrailingZeroes) +
                    L"% " + _(L"of all Dolch adjectives")) + L")</span>" :
                wxString(L"(" + wxNumberFormatter::ToString(
                    dolchAdjectivesPercentage, 1, wxNumberFormatter::Style::Style_NoTrailingZeroes) +
                    L"% " + _(L"of all Dolch adjectives") + wxString(L")"));
            HTMLText += formatRow(_(L"Adjectives used:"), valueStr, percentStr);

            if (listData)
                {
                listData->SetItemText(listDataItemCount, 0, _(L"Adjectives used:"));
                listData->SetItemText(listDataItemCount, 1,
                    wxNumberFormatter::ToString(MAX_DOLCH_ADJECTIVE_WORDS-project->GetUnusedDolchAdjectives(), 0,
                        wxNumberFormatter::Style::Style_NoTrailingZeroes|
                        wxNumberFormatter::Style::Style_WithThousandsSep));
                listData->SetItemText(listDataItemCount++, 2, wxNumberFormatter::ToString(
                    dolchAdjectivesPercentage, 1, wxNumberFormatter::Style::Style_NoTrailingZeroes) +
                    L"% " + _(L"of all Dolch adjectives"));
                }
            }
        // Verbs
            {
            const wxString valueStr = (dolchVerbsPercentage >= 75) ?
                wxString::Format(L"<span style=\"color:%s\">", attentionColor.GetAsString(wxC2S_HTML_SYNTAX)) +
                wxNumberFormatter::ToString(MAX_DOLCH_VERBS-project->GetUnusedDolchVerbs(), 0,
                    wxNumberFormatter::Style::Style_NoTrailingZeroes|
                    wxNumberFormatter::Style::Style_WithThousandsSep) +
                L"</span>":
                wxNumberFormatter::ToString(MAX_DOLCH_VERBS-project->GetUnusedDolchVerbs(), 0,
                    wxNumberFormatter::Style::Style_NoTrailingZeroes|
                    wxNumberFormatter::Style::Style_WithThousandsSep);
            const wxString percentStr = (dolchVerbsPercentage >= 75) ?
                wxString::Format(L"<span style=\"color:%s\">", attentionColor.GetAsString(wxC2S_HTML_SYNTAX)) +
                wxString(L"(" + wxNumberFormatter::ToString(
                    dolchVerbsPercentage, 1, wxNumberFormatter::Style::Style_NoTrailingZeroes) +
                    L"% " + _(L"of all Dolch verbs")) + L")</span>" :
                wxString(L"(" + wxNumberFormatter::ToString(
                    dolchVerbsPercentage, 1, wxNumberFormatter::Style::Style_NoTrailingZeroes) +
                    L"% " + _(L"of all Dolch verbs") + wxString(L")"));
            HTMLText += formatRow(_(L"Verbs used:"), valueStr, percentStr);

            if (listData)
                {
                listData->SetItemText(listDataItemCount, 0, _(L"Verbs used:"));
                listData->SetItemText(listDataItemCount, 1,
                    wxNumberFormatter::ToString(MAX_DOLCH_VERBS-project->GetUnusedDolchVerbs(), 0,
                    wxNumberFormatter::Style::Style_NoTrailingZeroes|
                        wxNumberFormatter::Style::Style_WithThousandsSep));
                listData->SetItemText(listDataItemCount++, 2, wxNumberFormatter::ToString(
                    dolchVerbsPercentage, 1, wxNumberFormatter::Style::Style_NoTrailingZeroes) +
                    L"% " + _(L"of all Dolch verbs"));
                }
            }
        // Nouns
            {
            const wxString valueStr = (dolchNounPercentage >= 75) ?
                wxString::Format(L"<span style=\"color:%s\">", attentionColor.GetAsString(wxC2S_HTML_SYNTAX)) +
                wxNumberFormatter::ToString(MAX_DOLCH_NOUNS-project->GetUnusedDolchNouns(), 0,
                    wxNumberFormatter::Style::Style_NoTrailingZeroes|
                    wxNumberFormatter::Style::Style_WithThousandsSep) +
                L"</span>" :
                wxNumberFormatter::ToString(MAX_DOLCH_NOUNS-project->GetUnusedDolchNouns(), 0,
                    wxNumberFormatter::Style::Style_NoTrailingZeroes|
                    wxNumberFormatter::Style::Style_WithThousandsSep);
            const wxString percentStr = (dolchNounPercentage >= 75) ?
                wxString::Format(L"<span style=\"color:%s\">", attentionColor.GetAsString(wxC2S_HTML_SYNTAX)) +
                wxString(L"(" + wxNumberFormatter::ToString(
                    dolchNounPercentage, 1,
                    wxNumberFormatter::Style::Style_NoTrailingZeroes) +
                    L"% " + _(L"of all Dolch nouns")) + L")</span>" :
                wxString(L"(" + wxNumberFormatter::ToString(
                    dolchNounPercentage, 1,
                    wxNumberFormatter::Style::Style_NoTrailingZeroes) +
                    L"% " + _(L"of all Dolch nouns") + wxString(L")"));
            HTMLText += formatRow(_(L"Nouns used:"), valueStr, percentStr);

            if (listData)
                {
                listData->SetItemText(listDataItemCount, 0, _(L"Nouns used:"));
                listData->SetItemText(listDataItemCount, 1,
                    wxNumberFormatter::ToString(MAX_DOLCH_NOUNS-project->GetUnusedDolchNouns(), 0,
                    wxNumberFormatter::Style::Style_NoTrailingZeroes|
                    wxNumberFormatter::Style::Style_WithThousandsSep));
                listData->SetItemText(listDataItemCount++, 2, wxNumberFormatter::ToString(
                    dolchNounPercentage, 1,
                    wxNumberFormatter::Style::Style_NoTrailingZeroes) + L"% " + _(L"of all Dolch nouns"));
                }
            }
        HTMLText += L"\n</table>";

        wxString useDescription;
        if (dolchConjuctionPercentage >= 75 ||
            dolchPrepositionsPercentage >= 75 ||
            dolchPronounsPercentage >= 75 ||
            dolchAdverbsPercentage >= 75 ||
            dolchAdjectivesPercentage >= 75 ||
            dolchVerbsPercentage >= 75 ||
            dolchNounPercentage >= 75)
            {
            useDescription = _(L"This document makes excellent use of:");
            if (dolchConjuctionPercentage >= 75)
                { useDescription += wxString::Format(L" %s, ", _(L"conjunctions")); }
            if (dolchPrepositionsPercentage >= 75)
                { useDescription += wxString::Format(L" %s, ", _(L"prepositions")); }
            if (dolchPronounsPercentage >= 75)
                { useDescription += wxString::Format(L" %s, ", _(L"pronouns")); }
            if (dolchAdverbsPercentage >= 75)
                { useDescription += wxString::Format(L" %s, ", _(L"adverbs")); }
            if (dolchAdjectivesPercentage >= 75)
                { useDescription += wxString::Format(L" %s, ", _(L"adjectives")); }
            if (dolchVerbsPercentage >= 75)
                { useDescription += wxString::Format(L" %s, ", _(L"verbs")); }
            if (dolchNounPercentage >= 75)
                { useDescription += wxString::Format(L" %s, ", _(L"nouns")); }
            useDescription.RemoveLast(2);
            useDescription += L".";
            if (containsHighPercentageOfNonDochWords)
                {
                useDescription +=
                    _(L" However, this document contains a high percentage of non-Dolch words "
                       "and may not be appropriate for using as a Dolch test aid.");
                }
            useDescription += L"<br /><br />";
            }
        HTMLText += FormatHtmlNoteSection(useDescription +
            _(L"Dolch words that are not being used in this document may be viewed in the "
               "<a href=\"#UnusedDolchWords\">Unused Dolch Words</a> output."));
        }

    // total words
    if (project->GetStatisticsReportInfo().IsDolchWordsEnabled())
        {
        HTMLText += tableStart + formatHeader(_(L"Dolch Words"));

        // number of Dolch words
            {
            const wxString valueStr = (totalDolchPercentage < 70) ?
                L"<span style=\"color:#FF0000\">" + wxNumberFormatter::ToString(totalDolchWords, 0,
                    wxNumberFormatter::Style::Style_NoTrailingZeroes|
                    wxNumberFormatter::Style::Style_WithThousandsSep) + L"</span>" :
                wxNumberFormatter::ToString(totalDolchWords, 0,
                    wxNumberFormatter::Style::Style_NoTrailingZeroes|
                    wxNumberFormatter::Style::Style_WithThousandsSep);
            const wxString percentStr = (totalDolchPercentage < 70) ?
                L"<span style=\"color:#FF0000\">" +
                wxString(L"(" + wxNumberFormatter::ToString(
                    totalDolchPercentage, 1,
                    wxNumberFormatter::Style::Style_NoTrailingZeroes|
                    wxNumberFormatter::Style::Style_WithThousandsSep) + L"% " + _(L"of all words)")) + L"</span>" :
                wxString(L"(" + wxNumberFormatter::ToString(
                    totalDolchPercentage, 1,
                    wxNumberFormatter::Style::Style_NoTrailingZeroes|
                    wxNumberFormatter::Style::Style_WithThousandsSep) + L"% " + _(L"of all words)"));
            HTMLText += formatRow(_(L"Number of Dolch words:"), valueStr, percentStr);

            if (listData)
                {
                listData->SetItemText(listDataItemCount, 0, _(L"Number of Dolch words:"));
                listData->SetItemText(listDataItemCount, 1,
                    wxNumberFormatter::ToString(totalDolchWords, 0,
                        wxNumberFormatter::Style::Style_NoTrailingZeroes|
                        wxNumberFormatter::Style::Style_WithThousandsSep));
                listData->SetItemText(listDataItemCount++, 2, wxNumberFormatter::ToString(
                    totalDolchPercentage, 1,
                    wxNumberFormatter::Style::Style_NoTrailingZeroes|
                    wxNumberFormatter::Style::Style_WithThousandsSep) + L"% " + _(L"of all words"));
                }
            }

        // number of Dolch words (excluding nouns)
            {
            const wxString valueStr = (totalDolchExcludingNounsPercentage< 60) ?
                L"<span style=\"color:#FF0000\">" + wxNumberFormatter::ToString(totalDolchWordsExcludingNouns, 0,
                    wxNumberFormatter::Style::Style_NoTrailingZeroes|
                    wxNumberFormatter::Style::Style_WithThousandsSep) + L"</span>" :
                wxNumberFormatter::ToString(totalDolchWordsExcludingNouns, 0,
                    wxNumberFormatter::Style::Style_NoTrailingZeroes|
                    wxNumberFormatter::Style::Style_WithThousandsSep);
            const wxString percentStr = (totalDolchExcludingNounsPercentage< 60) ?
                L"<span style=\"color:#FF0000\">" +
                wxString(L"(" + wxNumberFormatter::ToString(
                    totalDolchExcludingNounsPercentage, 1,
                    wxNumberFormatter::Style::Style_NoTrailingZeroes|
                    wxNumberFormatter::Style::Style_WithThousandsSep) + L"% " + _(L"of all words)")) + L"</span>" :
                wxString(L"(" + wxNumberFormatter::ToString(
                    totalDolchExcludingNounsPercentage, 1,
                    wxNumberFormatter::Style::Style_NoTrailingZeroes|
                    wxNumberFormatter::Style::Style_WithThousandsSep) + L"% " + _(L"of all words)"));
            HTMLText += formatRow(_(L"Number of Dolch words (excluding nouns):"), valueStr, percentStr);

            if (listData)
                {
                listData->SetItemText(listDataItemCount, 0, _(L"Number of Dolch words (excluding nouns):"));
                listData->SetItemText(listDataItemCount, 1,
                    wxNumberFormatter::ToString(totalDolchWordsExcludingNouns, 0,
                        wxNumberFormatter::Style::Style_NoTrailingZeroes|
                        wxNumberFormatter::Style::Style_WithThousandsSep));
                listData->SetItemText(listDataItemCount++, 2, wxNumberFormatter::ToString(
                    totalDolchExcludingNounsPercentage, 1,
                    wxNumberFormatter::Style::Style_NoTrailingZeroes|
                    wxNumberFormatter::Style::Style_WithThousandsSep) + L"% " + _(L"of all words"));
                }
            }

        // non-Dolch words
            {
            const wxString valueStr = (totalDolchPercentage < 70) ?
                L"<span style=\"color:#FF0000\">" +
                wxNumberFormatter::ToString(project->GetTotalWords()-totalDolchWords, 0,
                    wxNumberFormatter::Style::Style_NoTrailingZeroes|
                    wxNumberFormatter::Style::Style_WithThousandsSep) + L"</span>" :
                wxNumberFormatter::ToString(project->GetTotalWords()-totalDolchWords, 0,
                    wxNumberFormatter::Style::Style_NoTrailingZeroes|
                    wxNumberFormatter::Style::Style_WithThousandsSep);
            const wxString percentStr = (totalDolchPercentage < 70) ?
                L"<span style=\"color:#FF0000\">" +
                wxString(L"(" + wxNumberFormatter::ToString(
                    100-totalDolchPercentage, 1,
                    wxNumberFormatter::Style::Style_NoTrailingZeroes|
                    wxNumberFormatter::Style::Style_WithThousandsSep) + L"% " + _(L"of all words)")) + L"</span>" :
                wxString(L"(" + wxNumberFormatter::ToString(
                    100-totalDolchPercentage, 1,
                    wxNumberFormatter::Style::Style_NoTrailingZeroes|
                    wxNumberFormatter::Style::Style_WithThousandsSep) + L"% " + _(L"of all words)"));
            HTMLText += formatRow(_(L"Number of non-Dolch words:"), valueStr, percentStr);

            if (listData)
                {
                listData->SetItemText(listDataItemCount, 0, _(L"Number of non-Dolch words:"));
                listData->SetItemText(listDataItemCount, 1,
                    wxNumberFormatter::ToString(project->GetTotalWords()-totalDolchWords, 0,
                        wxNumberFormatter::Style::Style_NoTrailingZeroes|
                        wxNumberFormatter::Style::Style_WithThousandsSep));
                listData->SetItemText(listDataItemCount++, 2, wxNumberFormatter::ToString(
                    100-totalDolchPercentage, 1,
                    wxNumberFormatter::Style::Style_NoTrailingZeroes|
                    wxNumberFormatter::Style::Style_WithThousandsSep) + L"% " + _(L"of all words"));
                }
            }

        // Conjunctions words
            {
            const wxString valueStr = wxNumberFormatter::ToString(project->GetDolchConjunctionCounts().second, 0,
                wxNumberFormatter::Style::Style_NoTrailingZeroes|
                wxNumberFormatter::Style::Style_WithThousandsSep);
            const wxString percentStr = wxString(L"(" + wxNumberFormatter::ToString(
                safe_divide<double>(project->GetDolchConjunctionCounts().second,project->GetTotalWords())*100, 1,
                wxNumberFormatter::Style::Style_NoTrailingZeroes|
                wxNumberFormatter::Style::Style_WithThousandsSep) + L"% " + _(L"of all words)"));
            HTMLText += formatRow(_(L"Number of Dolch conjunctions:"), valueStr, percentStr);

            if (listData)
                {
                listData->SetItemText(listDataItemCount, 0, _(L"Number of Dolch conjunctions:"));
                listData->SetItemText(listDataItemCount, 1,
                    wxNumberFormatter::ToString(project->GetDolchConjunctionCounts().second, 0,
                        wxNumberFormatter::Style::Style_NoTrailingZeroes|
                        wxNumberFormatter::Style::Style_WithThousandsSep));
                listData->SetItemText(listDataItemCount++, 2, wxNumberFormatter::ToString(
                    safe_divide<double>(project->GetDolchConjunctionCounts().second, project->GetTotalWords())*100, 1,
                    wxNumberFormatter::Style::Style_NoTrailingZeroes|
                    wxNumberFormatter::Style::Style_WithThousandsSep) + L"% " + _(L"of all words"));
                }

            HTMLText += formatRow(_(L"Number of unique Dolch conjunctions:"),
                wxNumberFormatter::ToString(project->GetDolchConjunctionCounts().first, 0,
                    wxNumberFormatter::Style::Style_NoTrailingZeroes|
                    wxNumberFormatter::Style::Style_WithThousandsSep), L"");
            if (listData)
                {
                listData->SetItemText(listDataItemCount, 0, _(L"Number of unique Dolch conjunctions:"));
                listData->SetItemText(listDataItemCount++, 1,
                    wxNumberFormatter::ToString(project->GetDolchConjunctionCounts().first, 0,
                        wxNumberFormatter::Style::Style_NoTrailingZeroes|
                        wxNumberFormatter::Style::Style_WithThousandsSep));
                }
            }
        // Prepositions words
            {
            const wxString valueStr = wxNumberFormatter::ToString(project->GetDolchPrepositionWordCounts().second, 0,
                wxNumberFormatter::Style::Style_NoTrailingZeroes|
                wxNumberFormatter::Style::Style_WithThousandsSep);
            const wxString percentStr = wxString(L"(" + wxNumberFormatter::ToString(
                safe_divide<double>(project->GetDolchPrepositionWordCounts().second, project->GetTotalWords())*100, 1,
                wxNumberFormatter::Style::Style_NoTrailingZeroes|
                wxNumberFormatter::Style::Style_WithThousandsSep) + L"% " + _(L"of all words)"));
            HTMLText += formatRow(_(L"Number of Dolch prepositions:"), valueStr, percentStr);
            if (listData)
                {
                listData->SetItemText(listDataItemCount, 0, _(L"Number of Dolch prepositions:"));
                listData->SetItemText(listDataItemCount, 1,
                    wxNumberFormatter::ToString(project->GetDolchPrepositionWordCounts().second, 0,
                        wxNumberFormatter::Style::Style_NoTrailingZeroes|
                        wxNumberFormatter::Style::Style_WithThousandsSep));
                listData->SetItemText(listDataItemCount++, 2, wxNumberFormatter::ToString(
                    safe_divide<double>(
                        project->GetDolchPrepositionWordCounts().second, project->GetTotalWords()) * 100, 1,
                    wxNumberFormatter::Style::Style_NoTrailingZeroes|
                    wxNumberFormatter::Style::Style_WithThousandsSep) + L"% " + _(L"of all words"));
                }

            HTMLText += formatRow(_(L"Number of unique Dolch prepositions:"),
                wxNumberFormatter::ToString(project->GetDolchPrepositionWordCounts().first, 0,
                    wxNumberFormatter::Style::Style_NoTrailingZeroes|
                    wxNumberFormatter::Style::Style_WithThousandsSep), L"");
            if (listData)
                {
                listData->SetItemText(listDataItemCount, 0, _(L"Number of unique Dolch prepositions:"));
                listData->SetItemText(listDataItemCount++, 1,
                    wxNumberFormatter::ToString(project->GetDolchPrepositionWordCounts().first, 0,
                        wxNumberFormatter::Style::Style_NoTrailingZeroes|
                        wxNumberFormatter::Style::Style_WithThousandsSep));
                }
            }
        // Pronouns
            {
            const wxString valueStr = wxNumberFormatter::ToString(project->GetDolchPronounCounts().second, 0,
                wxNumberFormatter::Style::Style_NoTrailingZeroes|
                wxNumberFormatter::Style::Style_WithThousandsSep);
            const wxString percentStr = wxString(L"(" + wxNumberFormatter::ToString(
                safe_divide<double>(project->GetDolchPronounCounts().second,project->GetTotalWords())*100, 1,
                wxNumberFormatter::Style::Style_NoTrailingZeroes|
                wxNumberFormatter::Style::Style_WithThousandsSep) + L"% " + _(L"of all words)"));
            HTMLText += formatRow(_(L"Number of Dolch pronouns:"), valueStr, percentStr);

            if (listData)
                {
                listData->SetItemText(listDataItemCount, 0, _(L"Number of Dolch pronouns:"));
                listData->SetItemText(listDataItemCount, 1,
                    wxNumberFormatter::ToString(project->GetDolchPronounCounts().second, 0,
                        wxNumberFormatter::Style::Style_NoTrailingZeroes|
                        wxNumberFormatter::Style::Style_WithThousandsSep));
                listData->SetItemText(listDataItemCount++, 2, wxNumberFormatter::ToString(
                    safe_divide<double>(project->GetDolchPronounCounts().second,project->GetTotalWords())*100, 1,
                    wxNumberFormatter::Style::Style_NoTrailingZeroes|
                    wxNumberFormatter::Style::Style_WithThousandsSep) + L"% " + _(L"of all words"));
                }

            HTMLText += formatRow(_(L"Number of unique Dolch pronouns:"),
                wxNumberFormatter::ToString(project->GetDolchPronounCounts().first, 0,
                    wxNumberFormatter::Style::Style_NoTrailingZeroes|
                    wxNumberFormatter::Style::Style_WithThousandsSep), L"");
            if (listData)
                {
                listData->SetItemText(listDataItemCount, 0, _(L"Number of unique Dolch pronouns:"));
                listData->SetItemText(listDataItemCount++, 1,
                    wxNumberFormatter::ToString(project->GetDolchPronounCounts().first, 0,
                        wxNumberFormatter::Style::Style_NoTrailingZeroes|
                        wxNumberFormatter::Style::Style_WithThousandsSep));
                }
            }
        // Adverbs
            {
            const wxString valueStr = wxNumberFormatter::ToString(project->GetDolchAdverbCounts().second, 0,
                wxNumberFormatter::Style::Style_NoTrailingZeroes|
                wxNumberFormatter::Style::Style_WithThousandsSep);
            const wxString percentStr = wxString(L"(" + wxNumberFormatter::ToString(
                safe_divide<double>(project->GetDolchAdverbCounts().second,project->GetTotalWords())*100, 1,
                wxNumberFormatter::Style::Style_NoTrailingZeroes|
                wxNumberFormatter::Style::Style_WithThousandsSep) + L"% " + _(L"of all words)"));
            HTMLText += formatRow(_(L"Number of Dolch adverbs:"), valueStr, percentStr);

            if (listData)
                {
                listData->SetItemText(listDataItemCount, 0, _(L"Number of Dolch adverbs:"));
                listData->SetItemText(listDataItemCount, 1,
                    wxNumberFormatter::ToString(project->GetDolchAdverbCounts().second, 0,
                        wxNumberFormatter::Style::Style_NoTrailingZeroes|
                        wxNumberFormatter::Style::Style_WithThousandsSep));
                listData->SetItemText(listDataItemCount++, 2, wxNumberFormatter::ToString(
                    safe_divide<double>(project->GetDolchAdverbCounts().second,project->GetTotalWords())*100, 1,
                    wxNumberFormatter::Style::Style_NoTrailingZeroes|
                    wxNumberFormatter::Style::Style_WithThousandsSep) + L"% " + _(L"of all words"));
                }

            HTMLText += formatRow(_(L"Number of unique Dolch adverbs:"),
                wxNumberFormatter::ToString(project->GetDolchAdverbCounts().first, 0,
                    wxNumberFormatter::Style::Style_NoTrailingZeroes|
                    wxNumberFormatter::Style::Style_WithThousandsSep), L"");
            if (listData)
                {
                listData->SetItemText(listDataItemCount, 0, _(L"Number of unique Dolch adverbs:"));
                listData->SetItemText(listDataItemCount++, 1,
                    wxNumberFormatter::ToString(project->GetDolchAdverbCounts().first, 0,
                        wxNumberFormatter::Style::Style_NoTrailingZeroes|
                        wxNumberFormatter::Style::Style_WithThousandsSep));
                }
            }
        // adjectives
            {
            const wxString valueStr = wxNumberFormatter::ToString(project->GetDolchAdjectiveCounts().second, 0,
                wxNumberFormatter::Style::Style_NoTrailingZeroes|
                wxNumberFormatter::Style::Style_WithThousandsSep);
            const wxString percentStr = wxString(L"(" + wxNumberFormatter::ToString(
                safe_divide<double>(project->GetDolchAdjectiveCounts().second, project->GetTotalWords())*100, 1,
                wxNumberFormatter::Style::Style_NoTrailingZeroes|
                wxNumberFormatter::Style::Style_WithThousandsSep) + L"% " + _(L"of all words)"));
            HTMLText += formatRow(_(L"Number of Dolch adjectives:"), valueStr, percentStr);

            if (listData)
                {
                listData->SetItemText(listDataItemCount, 0, _(L"Number of Dolch adjectives:"));
                listData->SetItemText(listDataItemCount, 1, wxNumberFormatter::ToString(project->GetDolchAdjectiveCounts().second, 0, wxNumberFormatter::Style::Style_NoTrailingZeroes|wxNumberFormatter::Style::Style_WithThousandsSep));
                listData->SetItemText(listDataItemCount++, 2, wxNumberFormatter::ToString(
                    safe_divide<double>(project->GetDolchAdjectiveCounts().second,
                        project->GetTotalWords())*100, 1,
                    wxNumberFormatter::Style::Style_NoTrailingZeroes|
                    wxNumberFormatter::Style::Style_WithThousandsSep) + L"% " + _(L"of all words"));
                }

            HTMLText += formatRow(_(L"Number of unique Dolch adjectives:"),
                wxNumberFormatter::ToString(project->GetDolchAdjectiveCounts().first, 0,
                    wxNumberFormatter::Style::Style_NoTrailingZeroes|
                    wxNumberFormatter::Style::Style_WithThousandsSep), L"");
            if (listData)
                {
                listData->SetItemText(listDataItemCount, 0, _(L"Number of unique Dolch adjectives:"));
                listData->SetItemText(listDataItemCount++, 1,
                    wxNumberFormatter::ToString(project->GetDolchAdjectiveCounts().first, 0,
                        wxNumberFormatter::Style::Style_NoTrailingZeroes|
                        wxNumberFormatter::Style::Style_WithThousandsSep));
                }
            }
        // verbs
            {
            const wxString valueStr = wxNumberFormatter::ToString(project->GetDolchVerbsCounts().second, 0,
                wxNumberFormatter::Style::Style_NoTrailingZeroes|
                wxNumberFormatter::Style::Style_WithThousandsSep);
            const wxString percentStr = wxString(L"(" + wxNumberFormatter::ToString(
                safe_divide<double>(project->GetDolchVerbsCounts().second,
                    project->GetTotalWords())*100, 1,
                wxNumberFormatter::Style::Style_NoTrailingZeroes|
                wxNumberFormatter::Style::Style_WithThousandsSep) + L"% " + _(L"of all words)"));
            HTMLText += formatRow(_(L"Number of Dolch verbs:"), valueStr, percentStr);

            if (listData)
                {
                listData->SetItemText(listDataItemCount, 0, _(L"Number of Dolch verbs:"));
                listData->SetItemText(listDataItemCount, 1,
                    wxNumberFormatter::ToString(project->GetDolchVerbsCounts().second, 0,
                        wxNumberFormatter::Style::Style_NoTrailingZeroes|
                        wxNumberFormatter::Style::Style_WithThousandsSep));
                listData->SetItemText(listDataItemCount++, 2, wxNumberFormatter::ToString(
                    safe_divide<double>(project->GetDolchVerbsCounts().second,
                        project->GetTotalWords())*100, 1,
                    wxNumberFormatter::Style::Style_NoTrailingZeroes|
                    wxNumberFormatter::Style::Style_WithThousandsSep) + L"% " + _(L"of all words"));
                }

            HTMLText += formatRow(_(L"Number of unique Dolch verbs:"),
                wxNumberFormatter::ToString(project->GetDolchVerbsCounts().first, 0,
                    wxNumberFormatter::Style::Style_NoTrailingZeroes|
                    wxNumberFormatter::Style::Style_WithThousandsSep), L"");
            if (listData)
                {
                listData->SetItemText(listDataItemCount, 0, _(L"Number of unique Dolch verbs:"));
                listData->SetItemText(listDataItemCount++, 1,
                    wxNumberFormatter::ToString(project->GetDolchVerbsCounts().first, 0,
                        wxNumberFormatter::Style::Style_NoTrailingZeroes|
                        wxNumberFormatter::Style::Style_WithThousandsSep));
                }
            }
        // nouns
            {
            const wxString valueStr = wxNumberFormatter::ToString(project->GetDolchNounCounts().second, 0,
                wxNumberFormatter::Style::Style_NoTrailingZeroes|wxNumberFormatter::Style::Style_WithThousandsSep);
            const wxString percentStr = wxString(L"(" + wxNumberFormatter::ToString(
                safe_divide<double>(project->GetDolchNounCounts().second,
                    project->GetTotalWords())*100, 1,
                wxNumberFormatter::Style::Style_NoTrailingZeroes|
                wxNumberFormatter::Style::Style_WithThousandsSep) + L"% " + _(L"of all words)"));
            HTMLText += formatRow(_(L"Number of Dolch nouns:"), valueStr, percentStr);
            if (listData)
                {
                listData->SetItemText(listDataItemCount, 0, _(L"Number of Dolch nouns:"));
                listData->SetItemText(listDataItemCount, 1,
                    wxNumberFormatter::ToString(project->GetDolchNounCounts().second, 0,
                        wxNumberFormatter::Style::Style_NoTrailingZeroes|
                        wxNumberFormatter::Style::Style_WithThousandsSep));
                listData->SetItemText(listDataItemCount++, 2, wxNumberFormatter::ToString(
                    safe_divide<double>(project->GetDolchNounCounts().second,
                        project-> GetTotalWords())*100, 1,
                    wxNumberFormatter::Style::Style_NoTrailingZeroes|
                    wxNumberFormatter::Style::Style_WithThousandsSep) + L"% " + _(L"of all words"));
                }

            HTMLText += formatRow(_(L"Number of unique  Dolch nouns:"),
                wxNumberFormatter::ToString(project->GetDolchNounCounts().first, 0,
                    wxNumberFormatter::Style::Style_NoTrailingZeroes|
                    wxNumberFormatter::Style::Style_WithThousandsSep), L"");
            if (listData)
                {
                listData->SetItemText(listDataItemCount, 0, _(L"Number of unique Dolch nouns:"));
                listData->SetItemText(listDataItemCount++, 1,
                    wxNumberFormatter::ToString(project->GetDolchNounCounts().first, 0,
                        wxNumberFormatter::Style::Style_NoTrailingZeroes|
                        wxNumberFormatter::Style::Style_WithThousandsSep));
                }
            }
        HTMLText += L"\n</table>";

        if (containsHighPercentageOfNonDochWords)
            {
            HTMLText += FormatHtmlWarningSection(
                _(L"This document contains a high percentage of non-Dolch words and may not be appropriate "
                   "for using as a Dolch test aid."));
            }
        }

    if (includeExplanation && project->GetStatisticsReportInfo().IsDolchExplanationEnabled())
        {
        HTMLText += tableStart + formatHeader(_(L"Explanation"));
        HTMLText += L"\n<tr><td style='width:100%'><p>";
        HTMLText += _(L"The Dolch Sight Words represent the most frequently occurring service words<sup>1</sup> "
                       "in most text, especially children's literature. Early readers need to learn and recognize "
                       "these words to attain reading fluency. Many of these words cannot be sounded out or "
                       "represented by pictures; therefore, they must be learned by sight<sup>2</sup>.");
        HTMLText += L"</p>\n<p>";
        HTMLText += _(L"If you are writing educational materials for early readers, then it is recommended "
                       "to use as many of these words as possible to help readers practice them. "
                       "It is also recommended to keep the percentage of non-Dolch words low so that the "
                       "reader can better focus on the sight words.");
        HTMLText += L"</p>\n<p><sup>1</sup> ";
        HTMLText += _(L"Pronouns, adjectives, adverbs, prepositions, conjunctions, and verbs.");
        HTMLText += L"<br /><sup>2</sup> ";
        HTMLText += _(L"A separate list of nouns commonly found in children's literature is also included with the "
                      "Dolch collection. However, the sight words are generally the focus of most Dolch activities.");
        HTMLText += L"</p></td></tr>\n</table>";
        }

    if (listData)
        { listData->SetSize(listDataItemCount, 3); }

    return HTMLText;
    }

//------------------------------------------------
wxString ProjectReportFormat::FormatHtmlReportStart(
    [[maybe_unused]] const wxColour bgColor,
    [[maybe_unused]] const wxColour textColor,
    const wxString& title /*= wxEmptyString*/)
    {
    return wxString::Format(
        L"<!DOCTYPE html>"
        "\n<html>"
        "\n<head>"
        "\n    <meta http-equiv='content-type' content='text/html; charset=UTF-8' />"
        "\n    <title>%s</title>"
        "\n    <style>"
        "\n    </style>"
        "\n</head>"
        "\n<body bgcolor=%s text=%s link=%s>",
            title,
#ifdef __WXOSX__
        wxSystemSettings::GetColour(wxSYS_COLOUR_WINDOW).GetAsString(wxC2S_HTML_SYNTAX),
        wxSystemSettings::GetColour(wxSYS_COLOUR_WINDOWTEXT).GetAsString(wxC2S_HTML_SYNTAX),
#else
        bgColor.GetAsString(wxC2S_HTML_SYNTAX),
        textColor.GetAsString(wxC2S_HTML_SYNTAX),
#endif
        wxSystemSettings::GetColour(wxSYS_COLOUR_HOTLIGHT).GetAsString(wxC2S_HTML_SYNTAX)
        );
    }

//------------------------------------------------
wxString ProjectReportFormat::FormatHtmlReportEnd()
    {
    return wxString(L"\n</body>\n</html>");
    }

//------------------------------------------------
wxString ProjectReportFormat::FormatStatisticsInfo(const BaseProject* project,
                                                   const wxColour attentionColor,
                                                   ListCtrlExDataProviderBase* listData)
    {
    PROFILE();
    if (!project)
        { return wxEmptyString; }
    wxString currentLabel;
    lily_of_the_valley::html_encode_text htmlEncode;
    lily_of_the_valley::html_extract_text htmlStrip;
    size_t listDataItemCount = 0;
    if (listData)
        {
        listData->DeleteAllItems();
        listDataItemCount = listData->GetItemCount();
        listData->SetSize(listDataItemCount+MAX_SUMMARY_STAT_ROWS, 3);
        }

    wxString HTMLText;

    // Number of difficult sentences
    const double overlyLongSentencePercentage =
        (project->GetTotalOverlyLongSentences() == 0) ? 0 :
            safe_divide<double>(project->GetTotalOverlyLongSentences(),project->GetTotalSentences())*100;
    // Number of exclamatory sentences
    const double exclamatorySentencePercentage =
        (project->GetTotalExclamatorySentences() == 0) ? 0 :
            safe_divide<double>(project->GetTotalExclamatorySentences(),project->GetTotalSentences())*100;
    const double averageCharacterCount = safe_divide<double>(project->GetTotalCharacters(),project->GetTotalWords());
    const double averageSyllableCount = safe_divide<double>(project->GetTotalSyllables(),project->GetTotalWords());

    if (!project->GetStatisticsReportInfo().HasStatisticsEnabled())
        { HTMLText += _(L"No statistics <a href=\"#SelectStatistics\">currently selected</a>."); }

    wxBitmap bmp(100, 100);
    wxMemoryDC measuringDc(bmp);
    measuringDc.SetFont(measuringDc.GetFont().Larger().Larger());
    // one of the longer labels
    const auto labelColumnWidth = safe_divide<long>(measuringDc.GetTextExtent(
        _(L"Number of difficult sentences (more than 20 words):")).GetWidth(),
        wxGetApp().GetDPIScaleFactor());
    // the total number of words would be the largest (i.e., widest) number possible, so measure using that
    const auto numberColumnWidth =
        safe_divide<long>(measuringDc.GetTextExtent(std::to_wstring(project->GetTotalWords())).GetWidth(),
        wxGetApp().GetDPIScaleFactor());

    const wxString tableStart = L"<table class='minipage' border='0' style='width:100%'>";

    const auto formatHeader = [](const wxString& label)
        {
        return wxString::Format(
            L"\n<tr class='report-column-header' style='background:%s;'>"
             "<td colspan='3'><span style='color:%s;'>%s</span></td></tr>",
            GetReportHeaderColor().GetAsString(wxC2S_HTML_SYNTAX),
            GetReportHeaderFontColor().GetAsString(wxC2S_HTML_SYNTAX),
            label);
        };

    const auto formatNoteHeader = [](const wxString& label)
        {
        return wxString::Format(
            L"\n<tr style='background:%s;'><td colspan='3'><span style='color:white;'>%s</span></td></tr>",
            GetReportNoteHeaderColor().GetAsString(wxC2S_HTML_SYNTAX),
            label);
        };

    const auto formatRow = [labelColumnWidth, numberColumnWidth]
                            (const wxString& label, const wxString& value, const wxString& percent = wxEmptyString)
        {
        return wxString::Format(L"\n<tr><td style='min-width:%ipx; width:40%%;'>%s</td>"
                                 "<td style='text-align:right; width:%ipx;'>%s</td>"
                                 "<td style='text-align:left;'>%s</td></tr>",
            labelColumnWidth, label, numberColumnWidth, value, percent);
        };

    if (project->GetStatisticsReportInfo().IsParagraphEnabled())
        {
        // number of paragraphs
        HTMLText += tableStart + formatHeader(_(L"Paragraphs")) +
                    formatRow(_(L"Number of paragraphs:"),
                        wxNumberFormatter::ToString(project->GetTotalParagraphs(), 0,
                            wxNumberFormatter::Style::Style_NoTrailingZeroes|
                            wxNumberFormatter::Style::Style_WithThousandsSep));

        if (listData)
            {
            listData->SetItemText(listDataItemCount, 0, _(L"Number of paragraphs"));
            listData->SetItemText(listDataItemCount++, 1,
                wxNumberFormatter::ToString(project->GetTotalParagraphs(), 0,
                    wxNumberFormatter::Style::Style_NoTrailingZeroes|
                    wxNumberFormatter::Style::Style_WithThousandsSep));
            }

        // average paragraph length
        HTMLText += formatRow(_(L"Average number of sentences per paragraph:"),
            wxNumberFormatter::ToString(safe_divide<double>(project->GetTotalSentences(),
                project->GetTotalParagraphs()), 1,
                wxNumberFormatter::Style::Style_NoTrailingZeroes|wxNumberFormatter::Style::Style_WithThousandsSep));
        HTMLText += L"\n</table>";

        if (listData)
            {
            listData->SetItemText(listDataItemCount, 0, _(L"Average number of sentences per paragraph"));
            listData->SetItemText(listDataItemCount++, 1,
                wxNumberFormatter::ToString(
                    safe_divide<double>(project->GetTotalSentences(),project->GetTotalParagraphs()), 1,
                    wxNumberFormatter::Style::Style_NoTrailingZeroes|
                    wxNumberFormatter::Style::Style_WithThousandsSep));
            }
        }

    if (project->GetStatisticsReportInfo().IsSentencesEnabled())
        {
        // Number of sentences
        HTMLText += tableStart + formatHeader(_(L"Sentences"));

        if (project->GetInvalidSentenceMethod() == InvalidSentence::ExcludeFromAnalysis)
            {
            currentLabel =
                _(L"Number of sentences (excluding incomplete sentences, "
                   "see notes <a href=\"#incompsent\">below</a>):");
            }
        else if (project->GetInvalidSentenceMethod() == InvalidSentence::ExcludeExceptForHeadings)
            {
            currentLabel =
                _(L"Number of sentences (excluding lists and tables, see notes <a href=\"#incompsent\">below</a>):");
            }
        else
            {
            currentLabel =
                _(L"Number of sentences:");
            }
        HTMLText += formatRow(currentLabel, wxNumberFormatter::ToString(project->GetTotalSentences(), 0,
            wxNumberFormatter::Style::Style_NoTrailingZeroes|
            wxNumberFormatter::Style::Style_WithThousandsSep));

        if (listData)
            {
            currentLabel = _(L"Number of sentences");
            listData->SetItemText(listDataItemCount, 0, htmlStrip(currentLabel, currentLabel.length(), true, false));
            listData->SetItemText(listDataItemCount++, 1,
                wxNumberFormatter::ToString(project->GetTotalSentences(), 0,
                    wxNumberFormatter::Style::Style_NoTrailingZeroes|
                    wxNumberFormatter::Style::Style_WithThousandsSep));
            }

        // Number of sentence units
        if (project->GetStatisticsReportInfo().IsExtendedInformationEnabled())
            {
            if (project->GetInvalidSentenceMethod() == InvalidSentence::ExcludeFromAnalysis)
                {
                currentLabel =
                    _(L"Number of units/independent clauses (excluding incomplete sentences, see notes "
                       "<a href=\"#incompsent\">below</a>):");
                }
            else if (project->GetInvalidSentenceMethod() == InvalidSentence::ExcludeExceptForHeadings)
                {
                currentLabel =
                    _(L"Number of units/independent clauses (excluding lists and tables, see notes "
                       "<a href=\"#incompsent\">below</a>):");
                }
            else
                { currentLabel = _(L"Number of units/independent clauses:"); }
            HTMLText += formatRow(currentLabel, wxNumberFormatter::ToString(project->GetTotalSentenceUnits(), 0,
                wxNumberFormatter::Style::Style_NoTrailingZeroes|
                wxNumberFormatter::Style::Style_WithThousandsSep));

            if (listData)
                {
                currentLabel = _(L"Number of units/independent clauses");
                listData->SetItemText(listDataItemCount, 0,
                    htmlStrip(currentLabel, currentLabel.length(), true, false));
                listData->SetItemText(listDataItemCount++, 1,
                    wxNumberFormatter::ToString(project->GetTotalSentenceUnits(), 0,
                        wxNumberFormatter::Style::Style_NoTrailingZeroes|
                        wxNumberFormatter::Style::Style_WithThousandsSep));
                }
            }

        if (project->GetTotalOverlyLongSentences() > 0)
            {
            currentLabel = wxString::Format(
                _(L"Number of <a href=\"#DifficultSentences\">difficult sentences</a> (more than %d words):"),
                project->GetDifficultSentenceLength());
            }
        else
            { currentLabel = wxString::Format(
                _(L"Number of difficult sentences (more than %d words):"), project->GetDifficultSentenceLength()); }
        wxString currentValue = (overlyLongSentencePercentage >= 40) ?
            L"<span style=\"color:#FF0000\">" +
            wxNumberFormatter::ToString(project->GetTotalOverlyLongSentences(), 0,
                wxNumberFormatter::Style::Style_NoTrailingZeroes|wxNumberFormatter::Style::Style_WithThousandsSep) +
            L"</span>" :
            wxNumberFormatter::ToString(project->GetTotalOverlyLongSentences(), 0,
                wxNumberFormatter::Style::Style_NoTrailingZeroes|wxNumberFormatter::Style::Style_WithThousandsSep);
        wxString currentPercent = (overlyLongSentencePercentage >= 40) ?
            L"<span style=\"color:#FF0000\">(" +
            wxNumberFormatter::ToString(overlyLongSentencePercentage, 1,
                wxNumberFormatter::Style::Style_NoTrailingZeroes|wxNumberFormatter::Style::Style_WithThousandsSep) +
            L"% " + _(L"of all sentences)") + L"</span>" :
            L"(" + wxNumberFormatter::ToString(overlyLongSentencePercentage, 1,
                wxNumberFormatter::Style::Style_NoTrailingZeroes|wxNumberFormatter::Style::Style_WithThousandsSep) +
            L"% " + _(L"of all sentences)");
        HTMLText += formatRow(currentLabel, currentValue, currentPercent);

        if (listData)
            {
            wxString sentLabelForTable = htmlStrip(currentLabel, currentLabel.length(), true, false);
            if (sentLabelForTable.Last() == L':')
                { sentLabelForTable.RemoveLast(1); }
            listData->SetItemText(listDataItemCount, 0, sentLabelForTable);
            listData->SetItemText(listDataItemCount, 1,
                wxNumberFormatter::ToString(project->GetTotalOverlyLongSentences(), 0,
                    wxNumberFormatter::Style::Style_NoTrailingZeroes|
                    wxNumberFormatter::Style::Style_WithThousandsSep));
            listData->SetItemText(listDataItemCount++, 2,
                wxNumberFormatter::ToString(overlyLongSentencePercentage, 1,
                    wxNumberFormatter::Style::Style_NoTrailingZeroes|
                    wxNumberFormatter::Style::Style_WithThousandsSep) + L"% " + _(L"of all sentences"));
            }

        // Longest sentence
        if (project->GetTotalSentences() > 0) // this should usually be the case
            {
            currentValue = (project->GetLongestSentence() >= 30) ?
                L"<span style=\"color:#FF0000\">" +
                wxNumberFormatter::ToString(project->GetLongestSentence(), 0,
                    wxNumberFormatter::Style::Style_NoTrailingZeroes|
                    wxNumberFormatter::Style::Style_WithThousandsSep) + L"</span>" :
                wxNumberFormatter::ToString(project->GetLongestSentence(), 0,
                    wxNumberFormatter::Style::Style_NoTrailingZeroes|wxNumberFormatter::Style::Style_WithThousandsSep);
            HTMLText += formatRow(wxString::Format(_(L"Longest sentence (sentence #%s):"),
                // add 1 to display it as 1 based
                wxNumberFormatter::ToString(project->GetLongestSentenceIndex() + 1, 0, true)),
                    currentValue);

            if (listData)
                {
                listData->SetItemText(listDataItemCount, 0, _(L"Longest sentence"));
                listData->SetItemText(listDataItemCount++, 1,
                    wxNumberFormatter::ToString(project->GetLongestSentence(), 0,
                        wxNumberFormatter::Style::Style_NoTrailingZeroes|
                        wxNumberFormatter::Style::Style_WithThousandsSep));
                }
            }
        else
            {
            HTMLText += L"<tr><td style=\"width:60%\">";
            HTMLText += _(L"Longest sentence:");
            HTMLText += L"</td><td style=\"text-align:right; width:10%\">";
            HTMLText += _(L"N/A");
            HTMLText += L"</td><td></td></tr>\n";
            }
        // average sentence length
        currentValue = (safe_divide<double>(project->GetTotalWords(), project->GetTotalSentences()) > 20) ?
            L"<span style=\"color:#FF0000\">" +
            wxNumberFormatter::ToString(safe_divide<double>(project->GetTotalWords(),
                project->GetTotalSentences()), 1,
                wxNumberFormatter::Style::Style_NoTrailingZeroes|
                wxNumberFormatter::Style::Style_WithThousandsSep) + L"</span>" :
            wxNumberFormatter::ToString(safe_divide<double>(project->GetTotalWords(),
                project->GetTotalSentences()), 1,
                wxNumberFormatter::Style::Style_NoTrailingZeroes|
                wxNumberFormatter::Style::Style_WithThousandsSep);
        HTMLText += formatRow(_(L"Average sentence length:"), currentValue);

        if (listData)
            {
            listData->SetItemText(listDataItemCount, 0, _(L"Average sentence length"));
            listData->SetItemText(listDataItemCount++, 1,
                wxNumberFormatter::ToString(safe_divide<double>(project->GetTotalWords(),
                    project->GetTotalSentences()), 1,
                    wxNumberFormatter::Style::Style_NoTrailingZeroes|
                    wxNumberFormatter::Style::Style_WithThousandsSep));
            }
        // Number of interrogative sentences
        const double interrogativeSentencePercentage =
            (project->GetTotalInterrogativeSentences() == 0) ? 0 :
                safe_divide<double>(project->GetTotalInterrogativeSentences(),project->GetTotalSentences()) * 100;
        HTMLText += formatRow(_(L"Number of interrogative sentences (questions):"),
            wxNumberFormatter::ToString(project->GetTotalInterrogativeSentences(), 0,
                wxNumberFormatter::Style::Style_NoTrailingZeroes|
                wxNumberFormatter::Style::Style_WithThousandsSep),
            L"(" + wxNumberFormatter::ToString(interrogativeSentencePercentage, 1,
                wxNumberFormatter::Style::Style_NoTrailingZeroes|
                wxNumberFormatter::Style::Style_WithThousandsSep) + L"% " + _(L"of all sentences)"));

        if (listData)
            {
            listData->SetItemText(listDataItemCount, 0, _(L"Number of interrogative sentences (questions)"));
            listData->SetItemText(listDataItemCount, 1,
                wxNumberFormatter::ToString(project->GetTotalInterrogativeSentences(), 0,
                    wxNumberFormatter::Style::Style_NoTrailingZeroes|
                    wxNumberFormatter::Style::Style_WithThousandsSep));
            listData->SetItemText(listDataItemCount++, 2,
                wxNumberFormatter::ToString(interrogativeSentencePercentage, 1,
                    wxNumberFormatter::Style::Style_NoTrailingZeroes|
                    wxNumberFormatter::Style::Style_WithThousandsSep) + L"% " + _(L"of all sentences"));
            }

        // exclamatory sentences
        currentValue = (exclamatorySentencePercentage >= 25) ?
            L"<span style=\"color:#FF0000\">" +
            wxNumberFormatter::ToString(project->GetTotalExclamatorySentences(), 0,
                wxNumberFormatter::Style::Style_NoTrailingZeroes|
                wxNumberFormatter::Style::Style_WithThousandsSep) + L"</span>" :
            wxNumberFormatter::ToString(project->GetTotalExclamatorySentences(), 0,
                wxNumberFormatter::Style::Style_NoTrailingZeroes|
                wxNumberFormatter::Style::Style_WithThousandsSep);
        currentPercent = (exclamatorySentencePercentage >= 25) ?
            L"<span style=\"color:#FF0000\">(" +
            wxNumberFormatter::ToString(exclamatorySentencePercentage, 1,
                wxNumberFormatter::Style::Style_NoTrailingZeroes|
                wxNumberFormatter::Style::Style_WithThousandsSep) + L"% " + _(L"of all sentences)") + L"</span>" :
            L"(" + wxNumberFormatter::ToString(exclamatorySentencePercentage, 1,
                wxNumberFormatter::Style::Style_NoTrailingZeroes|
                wxNumberFormatter::Style::Style_WithThousandsSep) + L"% " + _(L"of all sentences)");
        HTMLText += formatRow(_(L"Number of exclamatory sentences:"), currentValue, currentPercent);
        HTMLText += L"\n</table>";

        if (listData)
            {
            listData->SetItemText(listDataItemCount, 0, _(L"Number of exclamatory sentences"));
            listData->SetItemText(listDataItemCount, 1,
                wxNumberFormatter::ToString(project->GetTotalExclamatorySentences(), 0,
                wxNumberFormatter::Style::Style_NoTrailingZeroes|wxNumberFormatter::Style::Style_WithThousandsSep));
            listData->SetItemText(listDataItemCount++, 2,
                wxNumberFormatter::ToString(exclamatorySentencePercentage, 1,
                wxNumberFormatter::Style::Style_NoTrailingZeroes|wxNumberFormatter::Style::Style_WithThousandsSep) +
                L"% " + _(L"of all sentences"));
            }
        // add any necessary notes
        if (overlyLongSentencePercentage >= 40)
            {
            HTMLText += FormatHtmlWarningSection(_(L"A large percentage of sentences are overly long."));
            }
        if (exclamatorySentencePercentage >= 25)
            {
            HTMLText += FormatHtmlNoteSection(
                _(L"A large percentage of sentences are exclamatory, giving the document an "
                   "overall angry/excited tone. Is this intended?"));
            }
        }

    if (project->GetStatisticsReportInfo().IsWordsEnabled())
        {
        // Number of words
        HTMLText += tableStart + formatHeader(_(L"Words"));

        wxString currentValue = (project->GetTotalWords() < 300) ?
            L"<span style=\"color:#FF0000\">" +
            wxNumberFormatter::ToString(project->GetTotalWords(), 0,
                wxNumberFormatter::Style::Style_NoTrailingZeroes|
                wxNumberFormatter::Style::Style_WithThousandsSep) + L"</span>" :
            wxNumberFormatter::ToString(project->GetTotalWords(), 0,
                wxNumberFormatter::Style::Style_NoTrailingZeroes|wxNumberFormatter::Style::Style_WithThousandsSep);
        HTMLText += formatRow(_(L"Number of words:"), currentValue);

        if (listData)
            {
            listData->SetItemText(listDataItemCount, 0, _(L"Number of words"));
            listData->SetItemText(listDataItemCount++, 1,
                wxNumberFormatter::ToString(project->GetTotalWords(), 0,
                    wxNumberFormatter::Style::Style_NoTrailingZeroes|
                    wxNumberFormatter::Style::Style_WithThousandsSep));
            }
        // Number of unique words
        HTMLText += formatRow(_(L"Number of unique words:"),
            wxNumberFormatter::ToString(project->GetTotalUniqueWords(), 0,
                wxNumberFormatter::Style::Style_NoTrailingZeroes|wxNumberFormatter::Style::Style_WithThousandsSep));

        if (listData)
            {
            listData->SetItemText(listDataItemCount, 0, _(L"Number of unique words"));
            listData->SetItemText(listDataItemCount++, 1,
                wxNumberFormatter::ToString(project->GetTotalUniqueWords(), 0,
                    wxNumberFormatter::Style::Style_NoTrailingZeroes|
                    wxNumberFormatter::Style::Style_WithThousandsSep));
            }
        // Number of total syllables
        HTMLText += formatRow(_(L"Number of syllables:"),
            wxNumberFormatter::ToString(project->GetTotalSyllables(), 0,
                wxNumberFormatter::Style::Style_NoTrailingZeroes|wxNumberFormatter::Style::Style_WithThousandsSep));

        if (listData)
            {
            listData->SetItemText(listDataItemCount, 0, _(L"Number of syllables"));
            listData->SetItemText(listDataItemCount++, 1,
                wxNumberFormatter::ToString(project->GetTotalSyllables(), 0,
                    wxNumberFormatter::Style::Style_NoTrailingZeroes|
                    wxNumberFormatter::Style::Style_WithThousandsSep));
            }
        // Number of total characters
        HTMLText += formatRow(_(L"Number of characters (punctuation excluded):"),
            wxNumberFormatter::ToString(project->GetTotalCharacters(), 0,
                wxNumberFormatter::Style::Style_NoTrailingZeroes|wxNumberFormatter::Style::Style_WithThousandsSep));

        if (listData)
            {
            listData->SetItemText(listDataItemCount, 0, _(L"Number of characters (punctuation excluded)"));
            listData->SetItemText(listDataItemCount++, 1,
                wxNumberFormatter::ToString(project->GetTotalCharacters(), 0,
                    wxNumberFormatter::Style::Style_NoTrailingZeroes|
                    wxNumberFormatter::Style::Style_WithThousandsSep));
            }
        HTMLText += formatRow(_(L"Number of characters + punctuation:"),
            wxNumberFormatter::ToString(project->GetTotalCharactersPlusPunctuation(), 0,
                wxNumberFormatter::Style::Style_NoTrailingZeroes|
                wxNumberFormatter::Style::Style_WithThousandsSep));

        HTMLText += L"\n<tr><td colspan=\"3\">";
        HTMLText += FormatHtmlNoteSection(
            _(L"Sentence-ending punctuation is excluded from this statistic. "
               "Tests that include punctuation counts instruct to not include any punctuation that ends a sentence."));
        HTMLText += L"</td></tr>";

        if (listData)
            {
            listData->SetItemText(listDataItemCount, 0, _(L"Number of characters + punctuation"));
            listData->SetItemText(listDataItemCount++, 1,
                wxNumberFormatter::ToString(project->GetTotalCharactersPlusPunctuation(), 0,
                    wxNumberFormatter::Style::Style_NoTrailingZeroes|
                    wxNumberFormatter::Style::Style_WithThousandsSep));
            }
        // average number of characters
        currentValue = (averageCharacterCount >= 6) ?
            L"<span style=\"color:#FF0000\">" +
            wxNumberFormatter::ToString(averageCharacterCount, 1,
                wxNumberFormatter::Style::Style_NoTrailingZeroes|
                wxNumberFormatter::Style::Style_WithThousandsSep) +
                L"</span>" :
            wxNumberFormatter::ToString(averageCharacterCount, 1,
                wxNumberFormatter::Style::Style_NoTrailingZeroes|
                wxNumberFormatter::Style::Style_WithThousandsSep);
        HTMLText += formatRow(_(L"Average number of characters:"), currentValue);

        if (listData)
            {
            listData->SetItemText(listDataItemCount, 0, _(L"Average number of characters"));
            listData->SetItemText(listDataItemCount++, 1,
                wxNumberFormatter::ToString(averageCharacterCount, 1,
                    wxNumberFormatter::Style::Style_NoTrailingZeroes|
                    wxNumberFormatter::Style::Style_WithThousandsSep));
            }
        // average number of syllables
        currentValue = (averageSyllableCount >= 4) ?
            L"<span style=\"color:#FF0000\">" +
            wxNumberFormatter::ToString(averageSyllableCount, 1,
                wxNumberFormatter::Style::Style_NoTrailingZeroes|
                wxNumberFormatter::Style::Style_WithThousandsSep) + L"</span>" :
            wxNumberFormatter::ToString(averageSyllableCount, 1,
                wxNumberFormatter::Style::Style_NoTrailingZeroes|
                wxNumberFormatter::Style::Style_WithThousandsSep);
        HTMLText += formatRow(_(L"Average number of syllables:"), currentValue);
        HTMLText += L"\n</table>";

        if (listData)
            {
            listData->SetItemText(listDataItemCount, 0, _(L"Average number of syllables"));
            listData->SetItemText(listDataItemCount++, 1,
                wxNumberFormatter::ToString(averageSyllableCount, 1,
                    wxNumberFormatter::Style::Style_NoTrailingZeroes|
                    wxNumberFormatter::Style::Style_WithThousandsSep));
            }
        // supplementary info about these stats
        if ((project->GetTotalWords() < 300) ||
            (averageCharacterCount >= 6) ||
            (averageSyllableCount >= 4))
            {
            if (project->GetTotalWords() < 300)
                {
                HTMLText += FormatHtmlWarningSection(wxString::Format(
                    _(L"This document only contains %s words. "
                       "Most readability tests require a minimum of 300 words to generate meaningful results."),
                    wxNumberFormatter::ToString(project->GetTotalWords(), 0,
                    wxNumberFormatter::Style::Style_NoTrailingZeroes|
                    wxNumberFormatter::Style::Style_WithThousandsSep)));
                }
            if (averageCharacterCount >= 6)
                {
                HTMLText += FormatHtmlWarningSection(_(L"The average word length is high."));
                }
            if (averageSyllableCount >= 4)
                {
                HTMLText += FormatHtmlWarningSection(_(L"The average word complexity (syllable count) is high."));
                }
            }
        }

    if (project->GetStatisticsReportInfo().IsExtendedWordsEnabled())
        {
        // Numeric words
        HTMLText += tableStart + formatHeader(_(L"Numerals (Numbers, Dates, Currency, etc.)")) +
                    formatRow(_(L"Number of numerals:"),
            wxNumberFormatter::ToString(project->GetTotalNumerals(), 0,
                wxNumberFormatter::Style::Style_NoTrailingZeroes|
                wxNumberFormatter::Style::Style_WithThousandsSep),
            L"(" + wxNumberFormatter::ToString(
                safe_divide<double>(project->GetTotalNumerals(),
                    project->GetTotalWords())*100, 1,
                wxNumberFormatter::Style::Style_NoTrailingZeroes|
                wxNumberFormatter::Style::Style_WithThousandsSep) + L"% " + _(L"of all words)"));
        HTMLText += L"\n</table>";

        if (listData)
            {
            listData->SetItemText(listDataItemCount, 0, _(L"Number of numerals"));
            listData->SetItemText(listDataItemCount, 1,
                wxNumberFormatter::ToString(project->GetTotalNumerals(), 0,
                    wxNumberFormatter::Style::Style_NoTrailingZeroes|
                    wxNumberFormatter::Style::Style_WithThousandsSep));
            listData->SetItemText(listDataItemCount++, 2, wxNumberFormatter::ToString(
                safe_divide<double>(project->GetTotalNumerals(),
                    project->GetTotalWords())*100, 1,
                wxNumberFormatter::Style::Style_NoTrailingZeroes|
                wxNumberFormatter::Style::Style_WithThousandsSep) + L"% " + _(L"of all words"));
            }

        // Proper nouns (not supported by German)
        if (project->GetProjectLanguage() != readability::test_language::german_test)
            {
            HTMLText += tableStart + formatHeader(_(L"Proper Nouns"));
            HTMLText += formatRow(_(L"Number of proper nouns:"),
                wxNumberFormatter::ToString(project->GetTotalProperNouns(), 0,
                    wxNumberFormatter::Style::Style_NoTrailingZeroes|
                    wxNumberFormatter::Style::Style_WithThousandsSep),
                L"(" + wxNumberFormatter::ToString(
                    safe_divide<double>(project->GetTotalProperNouns(),
                        project->GetTotalWords())*100, 1,
                    wxNumberFormatter::Style::Style_NoTrailingZeroes|
                    wxNumberFormatter::Style::Style_WithThousandsSep) + L"% " + _(L"of all words)"));
            HTMLText += L"\n</table>";

            if (listData)
                {
                listData->SetItemText(listDataItemCount, 0, _(L"Number of proper nouns"));
                listData->SetItemText(listDataItemCount, 1,
                    wxNumberFormatter::ToString(project->GetTotalProperNouns(), 0,
                        wxNumberFormatter::Style::Style_NoTrailingZeroes|
                        wxNumberFormatter::Style::Style_WithThousandsSep));
                listData->SetItemText(listDataItemCount++, 2, wxNumberFormatter::ToString(
                    safe_divide<double>(project->GetTotalProperNouns(),
                        project->GetTotalWords())*100, 1,
                    wxNumberFormatter::Style::Style_NoTrailingZeroes|
                    wxNumberFormatter::Style::Style_WithThousandsSep) + L"% " + _(L"of all words"));
                }
            }

        // Monosyllabic words
        HTMLText += tableStart + formatHeader(_(L"Monosyllabic Words")) +
                    formatRow(_(L"Number of monosyllabic words:"),
            wxNumberFormatter::ToString(project->GetTotalMonoSyllabicWords(), 0,
                wxNumberFormatter::Style::Style_NoTrailingZeroes|
                wxNumberFormatter::Style::Style_WithThousandsSep),
            L"(" + wxNumberFormatter::ToString(
                    safe_divide<double>(project->GetTotalMonoSyllabicWords(),
                        project->GetTotalWords())*100, 1,
                wxNumberFormatter::Style::Style_NoTrailingZeroes|
                wxNumberFormatter::Style::Style_WithThousandsSep) + L"% " + _(L"of all words)"));

        if (listData)
            {
            listData->SetItemText(listDataItemCount, 0, _(L"Number of monosyllabic words"));
            listData->SetItemText(listDataItemCount, 1,
                wxNumberFormatter::ToString(project->GetTotalMonoSyllabicWords(), 0,
                    wxNumberFormatter::Style::Style_NoTrailingZeroes|
                    wxNumberFormatter::Style::Style_WithThousandsSep));
            listData->SetItemText(listDataItemCount++, 2, wxNumberFormatter::ToString(
                    safe_divide<double>(project->GetTotalMonoSyllabicWords(),
                        project->GetTotalWords())*100, 1,
                wxNumberFormatter::Style::Style_NoTrailingZeroes|
                wxNumberFormatter::Style::Style_WithThousandsSep) + L"% " + _(L"of all words"));
            }

        // unique monosyllabic words
        HTMLText += formatRow(_(L"Number of unique monosyllabic words:"),
            wxNumberFormatter::ToString(project->GetTotalUniqueMonoSyllablicWords(), 0,
                wxNumberFormatter::Style::Style_NoTrailingZeroes|
                wxNumberFormatter::Style::Style_WithThousandsSep)) +
            L"\n</table>";

        if (listData)
            {
            listData->SetItemText(listDataItemCount, 0, _(L"Number of unique monosyllabic words"));
            listData->SetItemText(listDataItemCount++, 1,
                wxNumberFormatter::ToString(project->GetTotalUniqueMonoSyllablicWords(), 0,
                    wxNumberFormatter::Style::Style_NoTrailingZeroes|
                    wxNumberFormatter::Style::Style_WithThousandsSep));
            }

        // 3+ syllable
        HTMLText += tableStart + formatHeader(_(L"Complex Words"));
        if (project->GetTotal3PlusSyllabicWords() > 0)
            { currentLabel = _(L"Number of <a href=\"#HardWords\">complex (3+ syllable)</a> words:"); }
        else
            { currentLabel = _(L"Number of complex (3+ syllable) words:"); }
        HTMLText += formatRow(currentLabel,
            wxNumberFormatter::ToString(project->GetTotal3PlusSyllabicWords(), 0,
                wxNumberFormatter::Style::Style_NoTrailingZeroes|
                wxNumberFormatter::Style::Style_WithThousandsSep),
            wxString(L"(" + wxNumberFormatter::ToString(
                safe_divide<double>(project->GetTotal3PlusSyllabicWords(),
                    project->GetTotalWords())*100, 1,
                wxNumberFormatter::Style::Style_NoTrailingZeroes|
                wxNumberFormatter::Style::Style_WithThousandsSep) + L"% " + _(L"of all words)")));

        if (listData)
            {
            listData->SetItemText(listDataItemCount, 0, _(L"Number of complex (3+ syllable) words"));
            listData->SetItemText(listDataItemCount, 1,
                wxNumberFormatter::ToString(project->GetTotal3PlusSyllabicWords(), 0,
                    wxNumberFormatter::Style::Style_NoTrailingZeroes|
                    wxNumberFormatter::Style::Style_WithThousandsSep));
            listData->SetItemText(listDataItemCount++, 2, wxNumberFormatter::ToString(
                safe_divide<double>(project->GetTotal3PlusSyllabicWords(),project->GetTotalWords())*100, 1,
                wxNumberFormatter::Style::Style_NoTrailingZeroes|
                wxNumberFormatter::Style::Style_WithThousandsSep) + L"% " + _(L"of all words"));
            }
        // unique 3+ syllable words
        HTMLText += formatRow(_(L"Number of unique 3+ syllable words:"),
            wxNumberFormatter::ToString(project->GetTotalUnique3PlusSyllableWords(), 0,
                wxNumberFormatter::Style::Style_NoTrailingZeroes|
                wxNumberFormatter::Style::Style_WithThousandsSep)) +
            L"\n</table>";

        if (listData)
            {
            listData->SetItemText(listDataItemCount, 0, _(L"Number of unique 3+ syllable words"));
            listData->SetItemText(listDataItemCount++, 1,
                wxNumberFormatter::ToString(project->GetTotalUnique3PlusSyllableWords(), 0,
                    wxNumberFormatter::Style::Style_NoTrailingZeroes|
                    wxNumberFormatter::Style::Style_WithThousandsSep));
            }

        // long words (6 characters or more)
        HTMLText += tableStart + formatHeader(_(L"Long Words"));
        if (project->GetTotalLongWords() > 0)
            { currentLabel = _(L"Number of <a href=\"#LongWords\">long (6+ characters)</a> words:"); }
        else
            { currentLabel = _(L"Number of long (6+ characters) words:"); }
        HTMLText += formatRow(currentLabel,
            wxNumberFormatter::ToString(project->GetTotalLongWords(), 0,
                wxNumberFormatter::Style::Style_NoTrailingZeroes|wxNumberFormatter::Style::Style_WithThousandsSep),
            wxString(L"(" + wxNumberFormatter::ToString(
                safe_divide<double>(project->GetTotalLongWords(),project->GetTotalWords())*100, 1,
                wxNumberFormatter::Style::Style_NoTrailingZeroes|
                wxNumberFormatter::Style::Style_WithThousandsSep) + L"% " + _(L"of all words)")));

        if (listData)
            {
            listData->SetItemText(listDataItemCount, 0, _(L"Number of long (6+ characters) words"));
            listData->SetItemText(listDataItemCount, 1,
                wxNumberFormatter::ToString(project->GetTotalLongWords(), 0,
                    wxNumberFormatter::Style::Style_NoTrailingZeroes|
                    wxNumberFormatter::Style::Style_WithThousandsSep));
            listData->SetItemText(listDataItemCount++, 2, wxNumberFormatter::ToString(
                safe_divide<double>(project->GetTotalLongWords(),project->GetTotalWords())*100, 1,
                wxNumberFormatter::Style::Style_NoTrailingZeroes|
                wxNumberFormatter::Style::Style_WithThousandsSep) + L"% " + _(L"of all words"));
            }
        // unique long words
        HTMLText += formatRow(_(L"Number of unique long words:"),
            wxNumberFormatter::ToString(project->GetTotalUnique6CharsPlusWords(), 0,
                wxNumberFormatter::Style::Style_NoTrailingZeroes|
                wxNumberFormatter::Style::Style_WithThousandsSep)) +
            L"\n</table>";
        if (listData)
            {
            listData->SetItemText(listDataItemCount, 0, _(L"Number of unique long words"));
            listData->SetItemText(listDataItemCount++, 1,
                wxNumberFormatter::ToString(project->GetTotalUnique6CharsPlusWords(), 0,
                    wxNumberFormatter::Style::Style_NoTrailingZeroes|
                    wxNumberFormatter::Style::Style_WithThousandsSep));
            }

        if (project->IsSmogLikeTestIncluded())
            {
            // SMOG
            HTMLText += tableStart + formatHeader(_(L"SMOG Words"));
            HTMLText += formatRow(_(L"Number of SMOG hard words (3+ syllables, numerals fully syllabized):"),
                        wxNumberFormatter::ToString(project->GetTotal3PlusSyllabicWordsNumeralsFullySyllabized(), 0,
                            wxNumberFormatter::Style::Style_NoTrailingZeroes|
                            wxNumberFormatter::Style::Style_WithThousandsSep),
                        wxString(L"(" + wxNumberFormatter::ToString(
                            safe_divide<double>(project->GetTotal3PlusSyllabicWordsNumeralsFullySyllabized(),
                                project->GetTotalWords())*100, 1,
                            wxNumberFormatter::Style::Style_NoTrailingZeroes|
                            wxNumberFormatter::Style::Style_WithThousandsSep) + L"% " + _(L"of all words)")));

            if (listData)
                {
                listData->SetItemText(listDataItemCount, 0,
                    _(L"Number of SMOG hard words (3+ syllables, numerals fully syllabized)"));
                listData->SetItemText(listDataItemCount, 1,
                    wxNumberFormatter::ToString(project->GetTotal3PlusSyllabicWordsNumeralsFullySyllabized(), 0,
                        wxNumberFormatter::Style::Style_NoTrailingZeroes|
                        wxNumberFormatter::Style::Style_WithThousandsSep));
                listData->SetItemText(listDataItemCount++, 2, wxNumberFormatter::ToString(
                    safe_divide<double>(project->GetTotal3PlusSyllabicWordsNumeralsFullySyllabized(),
                        project->GetTotalWords())*100, 1,
                    wxNumberFormatter::Style::Style_NoTrailingZeroes|
                    wxNumberFormatter::Style::Style_WithThousandsSep) + L"% " + _(L"of all words"));
                }
            // unique SMOG words
            HTMLText += formatRow(_(L"Number of unique SMOG hard words:"),
                wxNumberFormatter::ToString(project->GetUnique3PlusSyllabicWordsNumeralsFullySyllabized(), 0,
                    wxNumberFormatter::Style::Style_NoTrailingZeroes|
                    wxNumberFormatter::Style::Style_WithThousandsSep)) +
                L"\n</table>";
            if (listData)
                {
                listData->SetItemText(listDataItemCount, 0, _(L"Number of unique SMOG hard words"));
                listData->SetItemText(listDataItemCount++, 1,
                    wxNumberFormatter::ToString(project->GetUnique3PlusSyllabicWordsNumeralsFullySyllabized(), 0,
                        wxNumberFormatter::Style::Style_NoTrailingZeroes|
                        wxNumberFormatter::Style::Style_WithThousandsSep));
                }
            }
        if (project->GetReadabilityTests().is_test_included(ReadabilityMessages::GUNNING_FOG()))
            {
            // Fog
            HTMLText += tableStart + formatHeader(_(L"Fog Words")) +
                formatRow(_(L"Number of Fog hard words (3+ syllables, with <a href=\"#FogHelp\">exceptions</a>):"),
                wxNumberFormatter::ToString(project->GetTotalHardWordsFog(), 0,
                    wxNumberFormatter::Style::Style_NoTrailingZeroes|
                    wxNumberFormatter::Style::Style_WithThousandsSep),
                L"(" + wxNumberFormatter::ToString(
                    safe_divide<double>(project->GetTotalHardWordsFog(),project->GetTotalWords())*100, 1,
                    wxNumberFormatter::Style::Style_NoTrailingZeroes|
                    wxNumberFormatter::Style::Style_WithThousandsSep) + L"% " + _(L"of all words)"));

            if (listData)
                {
                listData->SetItemText(listDataItemCount, 0,
                    _(L"Number of Fog hard words (3+ syllables, with exceptions)"));
                listData->SetItemText(listDataItemCount, 1,
                    wxNumberFormatter::ToString(project->GetTotalHardWordsFog(), 0,
                        wxNumberFormatter::Style::Style_NoTrailingZeroes|
                        wxNumberFormatter::Style::Style_WithThousandsSep));
                listData->SetItemText(listDataItemCount++, 2, wxNumberFormatter::ToString(
                    safe_divide<double>(project->GetTotalHardWordsFog(),project->GetTotalWords())*100, 1,
                    wxNumberFormatter::Style::Style_NoTrailingZeroes|
                    wxNumberFormatter::Style::Style_WithThousandsSep) + L"% " + _(L"of all words"));
                }
            // unique fog words
            HTMLText += formatRow(_(L"Number of unique Fog hard words:"),
                wxNumberFormatter::ToString(project->GetTotalUniqueHardFogWords(), 0,
                    wxNumberFormatter::Style::Style_NoTrailingZeroes|
                    wxNumberFormatter::Style::Style_WithThousandsSep)) +
                L"\n</table>";
            if (listData)
                {
                listData->SetItemText(listDataItemCount, 0, _(L"Number of unique Fog hard words"));
                listData->SetItemText(listDataItemCount++, 1,
                    wxNumberFormatter::ToString(project->GetTotalUniqueHardFogWords(), 0,
                        wxNumberFormatter::Style::Style_NoTrailingZeroes|
                        wxNumberFormatter::Style::Style_WithThousandsSep));
                }
            }

        if (project->GetStatisticsReportInfo().IsExtendedInformationEnabled())
            {
            // resize the list grid (if using one) to fit its data, then the call to Dolch
            // stats will append to that and resize it again. Then we will add some more rows
            // for the rest of the stats.
            if (listData)
                { listData->SetSize(listDataItemCount, 3); }
            HTMLText += FormatDolchStatisticsInfo(project, false, attentionColor, listData);
            if (listData)
                {
                listDataItemCount = listData->GetItemCount();
                listData->SetSize(listDataItemCount+MAX_SUMMARY_STAT_ROWS, 3);
                }
            }

        const size_t totalWordCountForDC =
            (project->GetDaleChallTextExclusionMode() ==
                SpecializedTestTextExclusion::ExcludeIncompleteSentencesExceptHeadings) ?
                project->GetTotalWordsFromCompleteSentencesAndHeaders() : project->GetTotalWords();
        if (project->IsDaleChallLikeTestIncluded() )
            {
            // DC
            HTMLText += tableStart + formatHeader(_(L"Dale-Chall Unfamiliar Words"));

            if (project->GetTotalHardWordsDaleChall() > 0)
                { currentLabel = _(L"Number of <a href=\"#DaleChallWords\">Dale-Chall</a> unfamiliar words:"); }
            else
                { currentLabel = _(L"Number of Dale-Chall unfamiliar words:"); }
            HTMLText += formatRow(currentLabel,
                wxNumberFormatter::ToString(project->GetTotalHardWordsDaleChall(), 0,
                    wxNumberFormatter::Style::Style_NoTrailingZeroes|
                    wxNumberFormatter::Style::Style_WithThousandsSep),
                wxString(L"(" + wxNumberFormatter::ToString(
                    safe_divide<double>(project->GetTotalHardWordsDaleChall(), totalWordCountForDC)*100, 1,
                    wxNumberFormatter::Style::Style_NoTrailingZeroes|
                    wxNumberFormatter::Style::Style_WithThousandsSep) +
                    L"% " + _(L"of all words)")));

            if (listData)
                {
                listData->SetItemText(listDataItemCount, 0, _(L"Number of Dale-Chall unfamiliar words"));
                listData->SetItemText(listDataItemCount, 1,
                    wxNumberFormatter::ToString(project->GetTotalHardWordsDaleChall(), 0,
                        wxNumberFormatter::Style::Style_NoTrailingZeroes|
                        wxNumberFormatter::Style::Style_WithThousandsSep));
                listData->SetItemText(listDataItemCount++, 2, wxNumberFormatter::ToString(
                    safe_divide<double>(project->GetTotalHardWordsDaleChall(), totalWordCountForDC)*100, 1,
                    wxNumberFormatter::Style::Style_NoTrailingZeroes|
                    wxNumberFormatter::Style::Style_WithThousandsSep) +
                    L"% " + _(L"of all words"));
                }
            // unique Dale Chall hard words
            HTMLText += formatRow(_(L"Number of unique Dale-Chall unfamiliar words:"),
                wxNumberFormatter::ToString(project->GetTotalUniqueDCHardWords(), 0,
                    wxNumberFormatter::Style::Style_NoTrailingZeroes|
                    wxNumberFormatter::Style::Style_WithThousandsSep)) +
                L"\n</table>";
            if (listData)
                {
                listData->SetItemText(listDataItemCount, 0, _(L"Number of unique Dale-Chall unfamiliar words"));
                listData->SetItemText(listDataItemCount++, 1,
                    wxNumberFormatter::ToString(project->GetTotalUniqueDCHardWords(), 0,
                        wxNumberFormatter::Style::Style_NoTrailingZeroes|
                        wxNumberFormatter::Style::Style_WithThousandsSep));
                }

            if (project->GetDaleChallTextExclusionMode() ==
                SpecializedTestTextExclusion::ExcludeIncompleteSentencesExceptHeadings)
                {
                HTMLText += FormatHtmlNoteSection(
                    _(L"New Dale-Chall treats headers and sub-headers as full sentences and excludes all "
                       "other incomplete sentences (regardless of your current analysis settings). "
                       "This is taken into account when calculating the unfamiliar word count and percentage."));
                }
            if (project->IsIncludingStockerCatholicSupplement())
                {
                HTMLText += FormatHtmlNoteSection(
                    _(L"Stocker's Catholic supplement is being included with the standard New Dale-Chall word list."));
                }
            }

        const size_t totalWordCountForHJ =
            (project->GetHarrisJacobsonTextExclusionMode() ==
                SpecializedTestTextExclusion::ExcludeIncompleteSentencesExceptHeadings) ?
            project->GetTotalWordsFromCompleteSentencesAndHeaders() : project->GetTotalWords();
        const size_t totalNumeralCountForHJ =
            (project->GetHarrisJacobsonTextExclusionMode() ==
                SpecializedTestTextExclusion::ExcludeIncompleteSentencesExceptHeadings) ?
            project->GetTotalNumeralsFromCompleteSentencesAndHeaders() : project->GetTotalNumerals();
        if (project->GetReadabilityTests().is_test_included(ReadabilityMessages::HARRIS_JACOBSON()) )
            {
            // Harris-Jacobson
            HTMLText += tableStart + formatHeader(_(L"Harris-Jacobson Unfamiliar Words"));
            if (project->GetTotalHardWordsHarrisJacobson() > 0)
                {
                currentLabel = _(L"Number of <a href=\"#HarrisJacobsonWords\">Harris-Jacobson</a> unfamiliar words:");
                }
            else
                { currentLabel = _(L"Number of Harris-Jacobson unfamiliar words:"); }
            HTMLText += formatRow(currentLabel,
                wxNumberFormatter::ToString(project->GetTotalHardWordsHarrisJacobson(), 0,
                    wxNumberFormatter::Style::Style_NoTrailingZeroes|
                    wxNumberFormatter::Style::Style_WithThousandsSep),
                L"(" + wxNumberFormatter::ToString(
                    safe_divide<double>(project->GetTotalHardWordsHarrisJacobson(),
                        totalWordCountForHJ-totalNumeralCountForHJ)*100, 1,
                    wxNumberFormatter::Style::Style_NoTrailingZeroes|
                    wxNumberFormatter::Style::Style_WithThousandsSep) + L"% " + _(L"of all words)"));

            if (listData)
                {
                listData->SetItemText(listDataItemCount, 0, _(L"Number of Harris-Jacobson unfamiliar words"));
                listData->SetItemText(listDataItemCount, 1,
                    wxNumberFormatter::ToString(project->GetTotalHardWordsHarrisJacobson(), 0,
                        wxNumberFormatter::Style::Style_NoTrailingZeroes|
                        wxNumberFormatter::Style::Style_WithThousandsSep));
                listData->SetItemText(listDataItemCount++, 2, wxNumberFormatter::ToString(
                    safe_divide<double>(project->GetTotalHardWordsHarrisJacobson(),
                        totalWordCountForHJ-totalNumeralCountForHJ)*100, 1,
                    wxNumberFormatter::Style::Style_NoTrailingZeroes|
                    wxNumberFormatter::Style::Style_WithThousandsSep) +
                    L"% " + _(L"of all words"));
                }

            // unique Harris-Jacobson hard words
            HTMLText += formatRow(_(L"Number of unique Harris-Jacobson unfamiliar words:"),
                wxNumberFormatter::ToString(project->GetTotalUniqueHarrisJacobsonHardWords(), 0,
                    wxNumberFormatter::Style::Style_NoTrailingZeroes|
                    wxNumberFormatter::Style::Style_WithThousandsSep)) +
                L"\n</table>";
            if (listData)
                {
                listData->SetItemText(listDataItemCount, 0, _(L"Number of unique Harris-Jacobson unfamiliar words"));
                listData->SetItemText(listDataItemCount++, 1,
                    wxNumberFormatter::ToString(project->GetTotalUniqueHarrisJacobsonHardWords(), 0,
                        wxNumberFormatter::Style::Style_NoTrailingZeroes|
                        wxNumberFormatter::Style::Style_WithThousandsSep));
                }

            if (project->GetHarrisJacobsonTextExclusionMode() ==
                SpecializedTestTextExclusion::ExcludeIncompleteSentencesExceptHeadings)
                {
                HTMLText += FormatHtmlNoteSection(
                    _(L"Harris-Jacobson treats headers and sub-headers as full sentences and excludes all other "
                       "incomplete sentences (regardless of your current analysis settings). "
                       "It also excludes all numerals from the total word count&mdash;this is taken into account "
                       "when calculating the unfamiliar word count and percentage."));
                }
            }

        if (project->GetReadabilityTests().is_test_included(ReadabilityMessages::SPACHE()) )
            {
            // Spache
            HTMLText += tableStart + formatHeader(_(L"Spache Unfamiliar Words"));
            if (project->GetTotalHardWordsSpache() > 0)
                { currentLabel = _(L"Number of <a href=\"#SpacheWords\">Spache</a> unfamiliar words:"); }
            else
                { currentLabel = _(L"Number of Spache unfamiliar words:"); }
            HTMLText += formatRow(currentLabel,
                wxNumberFormatter::ToString(project->GetTotalHardWordsSpache(), 0,
                    wxNumberFormatter::Style::Style_NoTrailingZeroes|
                    wxNumberFormatter::Style::Style_WithThousandsSep),
                L"(" + wxNumberFormatter::ToString(
                    safe_divide<double>(project->GetTotalHardWordsSpache(),project->GetTotalWords())*100, 1,
                    wxNumberFormatter::Style::Style_NoTrailingZeroes|
                    wxNumberFormatter::Style::Style_WithThousandsSep) +
                L"% " + _(L"of all words)"));

            if (listData)
                {
                listData->SetItemText(listDataItemCount, 0, _(L"Number of Spache unfamiliar words"));
                listData->SetItemText(listDataItemCount, 1,
                    wxNumberFormatter::ToString(project->GetTotalHardWordsSpache(), 0,
                        wxNumberFormatter::Style::Style_NoTrailingZeroes|
                        wxNumberFormatter::Style::Style_WithThousandsSep));
                listData->SetItemText(listDataItemCount++, 2, wxNumberFormatter::ToString(
                    safe_divide<double>(project->GetTotalHardWordsSpache(),project->GetTotalWords())*100, 1,
                    wxNumberFormatter::Style::Style_NoTrailingZeroes|
                    wxNumberFormatter::Style::Style_WithThousandsSep) +
                    L"% " + _(L"of all words"));
                }

            // unique Spache hard words
            HTMLText += formatRow(_(L"Number of unique Spache unfamiliar words:"),
                wxNumberFormatter::ToString(project->GetTotalUniqueHardWordsSpache(), 0,
                    wxNumberFormatter::Style::Style_NoTrailingZeroes|
                    wxNumberFormatter::Style::Style_WithThousandsSep)) +
                L"\n</table>";
            if (listData)
                {
                listData->SetItemText(listDataItemCount, 0, _(L"Number of unique Spache unfamiliar words"));
                listData->SetItemText(listDataItemCount++, 1,
                    wxNumberFormatter::ToString(project->GetTotalUniqueHardWordsSpache(), 0,
                        wxNumberFormatter::Style::Style_NoTrailingZeroes|
                        wxNumberFormatter::Style::Style_WithThousandsSep));
                }
            }
        if (project->GetReadabilityTests().is_test_included(ReadabilityMessages::EFLAW()))
            {
            // EFLAW Miniwords
            HTMLText += tableStart + formatHeader(_(L"McAlpine EFLAW Miniwords"));
            HTMLText += formatRow(_(L"Number of McAlpine EFLAW miniwords (1-3 characters, except for numerals):"),
                wxNumberFormatter::ToString(project->GetTotalMiniWords(), 0,
                    wxNumberFormatter::Style::Style_NoTrailingZeroes|
                    wxNumberFormatter::Style::Style_WithThousandsSep),
                L"(" + wxNumberFormatter::ToString(
                    safe_divide<double>(project->GetTotalMiniWords(),project->GetTotalWords())*100, 1,
                    wxNumberFormatter::Style::Style_NoTrailingZeroes|
                    wxNumberFormatter::Style::Style_WithThousandsSep) +
                L"% " + _(L"of all words)"));

            if (listData)
                {
                listData->SetItemText(listDataItemCount, 0,
                    _(L"Number of McAlpine EFLAW miniwords (1-3 characters, except for numerals)"));
                listData->SetItemText(listDataItemCount, 1,
                    wxNumberFormatter::ToString(project->GetTotalMiniWords(), 0,
                        wxNumberFormatter::Style::Style_NoTrailingZeroes|
                        wxNumberFormatter::Style::Style_WithThousandsSep));
                listData->SetItemText(listDataItemCount++, 2, wxNumberFormatter::ToString(
                    safe_divide<double>(project->GetTotalMiniWords(),project->GetTotalWords())*100, 1,
                    wxNumberFormatter::Style::Style_NoTrailingZeroes|
                    wxNumberFormatter::Style::Style_WithThousandsSep) +
                    L"% " + _(L"of all words"));
                }

            // unique EFLAW miniwords words
            HTMLText += formatRow(_(L"Number of unique McAlpine EFLAW miniwords words:"),
                wxNumberFormatter::ToString(project->GetTotalUniqueMiniWords(), 0,
                    wxNumberFormatter::Style::Style_NoTrailingZeroes|
                    wxNumberFormatter::Style::Style_WithThousandsSep)) +
                L"\n</table>";
            if (listData)
                {
                listData->SetItemText(listDataItemCount, 0, _(L"Number of unique McAlpine EFLAW miniwords words"));
                listData->SetItemText(listDataItemCount++, 1,
                    wxNumberFormatter::ToString(project->GetTotalUniqueMiniWords(), 0,
                        wxNumberFormatter::Style::Style_NoTrailingZeroes|
                        wxNumberFormatter::Style::Style_WithThousandsSep));
                }
            }

        // go through the custom readability tests
        for(auto pos = project->GetCustTestsInUse().cbegin();
            pos != project->GetCustTestsInUse().cend();
            ++pos)
            {
            if (!pos->GetIterator()->is_using_familiar_words())
                { continue; }

            // special logic for calculating word percentage if test is based on DC or HJ
            const size_t totalWordCountForCustomTest =
                pos->IsDaleChallFormula() ?
                totalWordCountForDC :
                pos->IsHarrisJacobsonFormula() ?
                totalWordCountForHJ - totalNumeralCountForHJ : project->GetTotalWords();

            wxString testName(pos->GetIterator()->get_name().c_str());
            testName = htmlEncode({ testName.wc_str(), testName.length() }, true).c_str();
            // total unfamiliar words
            HTMLText += tableStart + formatHeader(testName) +
                        formatRow(wxString::Format(_(L"Number of %s unfamiliar words:"), testName),
                wxNumberFormatter::ToString(pos->GetUnfamiliarWordCount(), 0,
                    wxNumberFormatter::Style::Style_NoTrailingZeroes|
                    wxNumberFormatter::Style::Style_WithThousandsSep),
                L"(" + wxNumberFormatter::ToString(
                    safe_divide<double>(pos->GetUnfamiliarWordCount(),totalWordCountForCustomTest)*100, 1,
                    wxNumberFormatter::Style::Style_NoTrailingZeroes|
                    wxNumberFormatter::Style::Style_WithThousandsSep) +
                            L"% " + _(L"of all words)"));

            if (listData)
                {
                listData->SetItemText(listDataItemCount, 0,
                    wxString::Format(_(L"Number of %s unfamiliar words"), pos->GetIterator()->get_name().c_str()));
                listData->SetItemText(listDataItemCount, 1,
                    wxNumberFormatter::ToString(pos->GetUnfamiliarWordCount(), 0,
                        wxNumberFormatter::Style::Style_NoTrailingZeroes|
                        wxNumberFormatter::Style::Style_WithThousandsSep));
                listData->SetItemText(listDataItemCount++, 2, wxNumberFormatter::ToString(
                    safe_divide<double>(pos->GetUnfamiliarWordCount(),totalWordCountForCustomTest)*100, 1,
                    wxNumberFormatter::Style::Style_NoTrailingZeroes|
                    wxNumberFormatter::Style::Style_WithThousandsSep) +
                    L"% " + _(L"of all words"));
                }

            // unique unfamiliar words
            HTMLText += formatRow(wxString::Format(_(L"Number of unique %s unfamiliar words:"), testName),
                wxNumberFormatter::ToString(pos->GetUniqueUnfamiliarWordCount(), 0,
                    wxNumberFormatter::Style::Style_NoTrailingZeroes|
                    wxNumberFormatter::Style::Style_WithThousandsSep)) +
                L"\n</table>";
            if (listData)
                {
                listData->SetItemText(listDataItemCount, 0,
                    wxString::Format(_(L"Number of unique %s unfamiliar words"),
                        pos->GetIterator()->get_name().c_str()));
                listData->SetItemText(listDataItemCount++, 1,
                    wxNumberFormatter::ToString(pos->GetUniqueUnfamiliarWordCount(), 0,
                        wxNumberFormatter::Style::Style_NoTrailingZeroes|
                        wxNumberFormatter::Style::Style_WithThousandsSep));
                }
            }
        }

    if (project->GetStatisticsReportInfo().IsGrammarEnabled() &&
        project->GetGrammarInfo().IsAnyFeatureEnabled())
        {
        // grammar section
        HTMLText += tableStart + formatHeader(_(L"Grammar"));
        // misspelled words
        if (project->GetProjectLanguage() == readability::test_language::english_test &&
            project->GetGrammarInfo().IsMisspellingsEnabled())
            {
            currentLabel.clear();
            if (project->GetMisspelledWordCount())
                { currentLabel += L"<a href=\"#Misspellings\">"; }
            currentLabel += _(L"Misspellings");
            if (project->GetMisspelledWordCount())
                { currentLabel += L"</a>"; }
            HTMLText += formatRow(currentLabel,
                        wxNumberFormatter::ToString(project->GetMisspelledWordCount(), 0,
                            wxNumberFormatter::Style::Style_NoTrailingZeroes|
                            wxNumberFormatter::Style::Style_WithThousandsSep));

            if (listData)
                {
                listData->SetItemText(listDataItemCount, 0, _(L"Misspellings"));
                listData->SetItemText(listDataItemCount++, 1,
                    wxNumberFormatter::ToString(project->GetMisspelledWordCount(), 0,
                        wxNumberFormatter::Style::Style_NoTrailingZeroes|
                        wxNumberFormatter::Style::Style_WithThousandsSep));
                }
            }
        // repeated words
        if (project->GetGrammarInfo().IsRepeatedWordsEnabled())
            {
            currentLabel.clear();
            if (project->GetDuplicateWordCount())
                { currentLabel += L"<a href=\"#RepeatedWords\">"; }
            currentLabel += _(L"Repeated words");
            if (project->GetDuplicateWordCount())
                { currentLabel += L"</a>"; }
            HTMLText += formatRow(currentLabel,
                        wxNumberFormatter::ToString(project->GetDuplicateWordCount(), 0,
                            wxNumberFormatter::Style::Style_NoTrailingZeroes|
                            wxNumberFormatter::Style::Style_WithThousandsSep));

            if (listData)
                {
                listData->SetItemText(listDataItemCount, 0, _(L"Repeated words"));
                listData->SetItemText(listDataItemCount++, 1,
                    wxNumberFormatter::ToString(project->GetDuplicateWordCount(), 0,
                        wxNumberFormatter::Style::Style_NoTrailingZeroes|
                        wxNumberFormatter::Style::Style_WithThousandsSep));
                }
            }
        /// @todo Need to add these grammar features to other languages too
        if (project->GetProjectLanguage() == readability::test_language::english_test)
            {
            // mismatched articles
            if (project->GetGrammarInfo().IsArticleMismatchesEnabled())
                {
                currentLabel.clear();
                if (project->GetMismatchedArticleCount())
                    { currentLabel += L"<a href=\"#MismatchedArtcles\">"; }
                currentLabel += _(L"Article mismatches");
                if (project->GetMismatchedArticleCount())
                    { currentLabel += L"</a>"; }
                HTMLText += formatRow(currentLabel,
                            wxNumberFormatter::ToString(project->GetMismatchedArticleCount(), 0,
                                wxNumberFormatter::Style::Style_NoTrailingZeroes|
                                wxNumberFormatter::Style::Style_WithThousandsSep));

                if (listData)
                    {
                    listData->SetItemText(listDataItemCount, 0, _(L"Article mismatches"));
                    listData->SetItemText(listDataItemCount++, 1,
                        wxNumberFormatter::ToString(project->GetMismatchedArticleCount(), 0,
                            wxNumberFormatter::Style::Style_NoTrailingZeroes|
                            wxNumberFormatter::Style::Style_WithThousandsSep));
                    }
                }
            // Wording Errors
            if (project->GetGrammarInfo().IsWordingErrorsEnabled())
                {
                currentLabel.clear();
                if (project->GetWordingErrorCount())
                    { currentLabel += L"<a href=\"#WordingErrors\">"; }
                currentLabel += _(L"Wording errors");
                if (project->GetWordingErrorCount())
                    { currentLabel += L"</a>"; }
                HTMLText += formatRow(currentLabel,
                            wxNumberFormatter::ToString(project->GetWordingErrorCount(), 0,
                                wxNumberFormatter::Style::Style_NoTrailingZeroes|
                                wxNumberFormatter::Style::Style_WithThousandsSep));

                if (listData)
                    {
                    listData->SetItemText(listDataItemCount, 0, _(L"Wording errors"));
                    listData->SetItemText(listDataItemCount++, 1,
                        wxNumberFormatter::ToString(project->GetWordingErrorCount(), 0,
                            wxNumberFormatter::Style::Style_NoTrailingZeroes|
                            wxNumberFormatter::Style::Style_WithThousandsSep));
                    }
                }
            // redundant phrase count
            if (project->GetGrammarInfo().IsRedundantPhrasesEnabled())
                {
                currentLabel.clear();
                if (project->GetRedundantPhraseCount())
                    { currentLabel += L"<a href=\"#RedundantPhrases\">"; }
                currentLabel += _(L"Redundant phrases");
                if (project->GetRedundantPhraseCount())
                    { currentLabel += L"</a>"; }
                HTMLText += formatRow(currentLabel,
                            wxNumberFormatter::ToString(project->GetRedundantPhraseCount(), 0,
                                wxNumberFormatter::Style::Style_NoTrailingZeroes|
                                wxNumberFormatter::Style::Style_WithThousandsSep));

                if (listData)
                    {
                    listData->SetItemText(listDataItemCount, 0, _(L"Redundant phrases"));
                    listData->SetItemText(listDataItemCount++, 1,
                        wxNumberFormatter::ToString(project->GetRedundantPhraseCount(), 0,
                            wxNumberFormatter::Style::Style_NoTrailingZeroes|
                            wxNumberFormatter::Style::Style_WithThousandsSep));
                    }
                }
            // overused words (by sentence)
            if (project->GetGrammarInfo().IsOverUsedWordsBySentenceEnabled())
                {
                currentLabel.clear();
                if (project->GetOverusedWordsBySentenceCount())
                    { currentLabel += L"<a href=\"#OverusedWordsBySentence\">"; }
                currentLabel += _(L"Overused words (x sentence)");
                if (project->GetOverusedWordsBySentenceCount())
                    { currentLabel += L"</a>"; }
                HTMLText += formatRow(currentLabel,
                            wxNumberFormatter::ToString(project->GetOverusedWordsBySentenceCount(), 0,
                                wxNumberFormatter::Style::Style_NoTrailingZeroes|
                                wxNumberFormatter::Style::Style_WithThousandsSep));

                if (listData)
                    {
                    listData->SetItemText(listDataItemCount, 0, _(L"Overused words (x sentence)"));
                    listData->SetItemText(listDataItemCount++, 1,
                        wxNumberFormatter::ToString(project->GetOverusedWordsBySentenceCount(), 0,
                            wxNumberFormatter::Style::Style_NoTrailingZeroes|
                            wxNumberFormatter::Style::Style_WithThousandsSep));
                    }
                }
            // wordiness
            if (project->GetGrammarInfo().IsWordyPhrasesEnabled())
                {
                currentLabel.clear();
                if (project->GetWordyPhraseCount())
                    { currentLabel += L"<a href=\"#WordyPhrases\">"; }
                currentLabel += _(L"Wordy items");
                if (project->GetWordyPhraseCount())
                    { currentLabel += L"</a>"; }
                HTMLText += formatRow(currentLabel,
                            wxNumberFormatter::ToString(project->GetWordyPhraseCount(), 0,
                                wxNumberFormatter::Style::Style_NoTrailingZeroes|
                                wxNumberFormatter::Style::Style_WithThousandsSep));

                if (listData)
                    {
                    listData->SetItemText(listDataItemCount, 0, _(L"Wordy items"));
                    listData->SetItemText(listDataItemCount++, 1,
                        wxNumberFormatter::ToString(project->GetWordyPhraseCount(), 0,
                            wxNumberFormatter::Style::Style_NoTrailingZeroes|
                            wxNumberFormatter::Style::Style_WithThousandsSep));
                    }
                }
            // cliches
            if (project->GetGrammarInfo().IsClichesEnabled())
                {
                currentLabel.clear();
                if (project->GetClicheCount() > 0)
                    { currentLabel += L"<a href=\"#Cliches\">"; }
                currentLabel += BaseProjectView::GetClichesTabLabel();
                if (project->GetClicheCount())
                    { currentLabel += L"</a>"; }
                HTMLText += formatRow(currentLabel,
                            wxNumberFormatter::ToString(project->GetClicheCount(), 0,
                                wxNumberFormatter::Style::Style_NoTrailingZeroes|
                                wxNumberFormatter::Style::Style_WithThousandsSep));

                if (listData)
                    {
                    listData->SetItemText(listDataItemCount, 0, BaseProjectView::GetClichesTabLabel());
                    listData->SetItemText(listDataItemCount++, 1,
                        wxNumberFormatter::ToString(project->GetClicheCount(), 0,
                            wxNumberFormatter::Style::Style_NoTrailingZeroes|
                            wxNumberFormatter::Style::Style_WithThousandsSep));
                    }
                }
            // passive voice
            if (project->GetGrammarInfo().IsPassiveVoiceEnabled())
                {
                currentLabel.clear();
                if (project->GetPassiveVoicesCount())
                    { currentLabel += L"<a href=\"#PassiveVoice\">"; }
                currentLabel += _(L"Passive voice");
                if (project->GetPassiveVoicesCount())
                    { currentLabel += L"</a>"; }
                HTMLText += formatRow(currentLabel,
                            wxNumberFormatter::ToString(project->GetPassiveVoicesCount(), 0,
                                wxNumberFormatter::Style::Style_NoTrailingZeroes|
                                wxNumberFormatter::Style::Style_WithThousandsSep));

                if (listData)
                    {
                    listData->SetItemText(listDataItemCount, 0, _(L"Passive voice"));
                    listData->SetItemText(listDataItemCount++, 1,
                        wxNumberFormatter::ToString(project->GetPassiveVoicesCount(), 0,
                            wxNumberFormatter::Style::Style_NoTrailingZeroes|
                            wxNumberFormatter::Style::Style_WithThousandsSep));
                    }
                }
            }
        // conjunction starting sentences
        if (project->GetGrammarInfo().IsConjunctionStartingSentencesEnabled())
            {
            currentLabel.clear();
            if (project->GetSentenceStartingWithConjunctionsCount() > 0)
                { currentLabel += L"<a href=\"#SentenceStartingWithConjunctions\">"; }
            currentLabel += BaseProjectView::GetSentenceStartingWithConjunctionsLabel();
            if (project->GetSentenceStartingWithConjunctionsCount())
                { currentLabel += L"</a>"; }
            HTMLText += formatRow(currentLabel,
                        wxNumberFormatter::ToString(
                            project->GetSentenceStartingWithConjunctionsCount(), 0,
                            wxNumberFormatter::Style::Style_NoTrailingZeroes|
                            wxNumberFormatter::Style::Style_WithThousandsSep));

            if (listData)
                {
                listData->SetItemText(listDataItemCount, 0,
                    BaseProjectView::GetSentenceStartingWithConjunctionsLabel());
                listData->SetItemText(listDataItemCount++, 1,
                    wxNumberFormatter::ToString(project->GetSentenceStartingWithConjunctionsCount(), 0,
                        wxNumberFormatter::Style::Style_NoTrailingZeroes|
                        wxNumberFormatter::Style::Style_WithThousandsSep));
                }
            }
        // lowercased sentences
        if (project->GetGrammarInfo().IsLowercaseSentencesEnabled())
            {
            currentLabel.clear();
            if (project->GetSentenceStartingWithLowercaseCount() > 0)
                { currentLabel += L"<a href=\"#SentenceStartingWithLowercase\">"; }
            if (project->GetSentenceStartMustBeUppercased())
                { currentLabel += L"* "; }
            currentLabel += BaseProjectView::GetSentenceStartingWithLowercaseLabel();
            if (project->GetSentenceStartingWithLowercaseCount())
                { currentLabel += L"</a>"; }
            HTMLText += formatRow(currentLabel,
                        wxNumberFormatter::ToString(project->GetSentenceStartingWithLowercaseCount(), 0,
                            wxNumberFormatter::Style::Style_NoTrailingZeroes|
                            wxNumberFormatter::Style::Style_WithThousandsSep));

            if (listData)
                {
                listData->SetItemText(listDataItemCount, 0,
                    BaseProjectView::GetSentenceStartingWithLowercaseLabel());
                listData->SetItemText(listDataItemCount++, 1,
                    wxNumberFormatter::ToString(project->GetSentenceStartingWithLowercaseCount(), 0,
                        wxNumberFormatter::Style::Style_NoTrailingZeroes|
                        wxNumberFormatter::Style::Style_WithThousandsSep));
                }
            }

        HTMLText += L"\n<tr><td colspan=\"3\">" +
            FormatHtmlNoteSection(
                _(L"Grammar statistics do not directly factor into readability formulas; however, "
                   "they can be useful suggestions for improving the document.")) +
            L"</td></tr>\n";

        if (project->GetSentenceStartMustBeUppercased())
            {
            HTMLText += L"\n<tr><td colspan=\"3\">" +
                FormatHtmlNoteSection(
                    _(L"* This project's sentence-deduction method is set to only accept capitalized sentences. "
                       "Lowercased-sentence detection will be limited to sentences that begin new paragraphs.")) +
                L"</td></tr>\n";
            }

        HTMLText += L"\n</table>";
        }

    if (project->GetStatisticsReportInfo().IsExtendedInformationEnabled())
        {
        // file/text stream info section
        HTMLText += tableStart +
            formatHeader((project->GetOriginalDocumentFilePath().length() ?
                _(L"File Information") : _(L"Text Information")));

        // file path (if not manually entered text)
        if (project->GetOriginalDocumentFilePath().length())
            {
            HTMLText += formatRow(_(L"Path"), wxEmptyString,
                project->GetOriginalDocumentFilePath());
            }
        if (project->GetAppendedDocumentFilePath().length())
            {
            HTMLText += formatRow(_(L"Additional Document Path"), wxEmptyString,
                project->GetAppendedDocumentFilePath());
            }
        // text size
        HTMLText += formatRow(_(L"Text size"), wxEmptyString,
            wxNumberFormatter::ToString(safe_divide<double>(project->GetTextSize(),1024), 2,
                wxNumberFormatter::Style::Style_NoTrailingZeroes|wxNumberFormatter::Style::Style_WithThousandsSep) +
                    _(L" Kbs.")) + L"\n</table>";

        if (listData)
            {
            listData->SetItemText(listDataItemCount, 0, _(L"Text size"));
            listData->SetItemText(listDataItemCount++, 1,
                wxNumberFormatter::ToString(safe_divide<double>(project->GetTextSize(),1024), 2,
                wxNumberFormatter::Style::Style_NoTrailingZeroes|wxNumberFormatter::Style::Style_WithThousandsSep) +
                _(L" Kbs."));
            }
        }

    // Notes section
    if (project->GetStatisticsReportInfo().IsNotesEnabled() &&
        (project->GetInvalidSentenceMethod() == InvalidSentence::ExcludeFromAnalysis ||
        project->GetInvalidSentenceMethod() == InvalidSentence::ExcludeExceptForHeadings ||
        project->GetTotalWords() < 300 ||
        overlyLongSentencePercentage >= 40 ||
        exclamatorySentencePercentage >= 25 ||
        averageCharacterCount >= 6 ||
        averageSyllableCount >= 4 ||
        project->GetAppendedDocumentFilePath().length()))
        {
        HTMLText += tableStart + formatNoteHeader(_(L"Notes"));
        if (project->GetInvalidSentenceMethod() == InvalidSentence::ExcludeFromAnalysis)
            {
            HTMLText += L"\n<tr><td style='width:100%;'><a name=\"incompsent\">";
            HTMLText += _(L"Incomplete sentences have been excluded from the analysis. "
                           "All words from incomplete sentences were ignored and not factored into "
                           "these statistics (except for grammar information).");
            HTMLText += L"</a></td></tr>";
            }
        else if (project->GetInvalidSentenceMethod() == InvalidSentence::ExcludeExceptForHeadings)
            {
            HTMLText += L"\n<tr><td style='width:100%;'><a name=\"incompsent\">";
            HTMLText += _(L"Lists and tables have been excluded from the analysis. "
                           "All words from these sentences were ignored and not factored into these "
                           "statistics (except for grammar information).");
            HTMLText += L"</a></td></tr>";
            }
        if (project->GetAppendedDocumentFilePath().length())
            {
            HTMLText += L"\n<tr><td style='width:100%;'>";
            HTMLText += wxString::Format(_(L"An additional document (\"%s\") has been appended and "
                        "included in the analysis."),
                        wxFileName(project->GetAppendedDocumentFilePath()).GetFullName());
            HTMLText += L"</td></tr>";
            }
        if (project->GetStatisticsReportInfo().IsExtendedInformationEnabled())
            {
            HTMLText += L"\n<tr><td style='width:100%;'>";
            HTMLText += _(L"Averages are calculated using arithmetic mean "
                          "(the summation of all values in a range divided by the number of items).");
            HTMLText += L"</td></tr>";
            }
        if (project->GetTotalWords() < 300)
            {
            HTMLText += L"\n<tr><td style='width:100%;'>";
            HTMLText += wxString::Format(
                _(L"This document only contains %s words. "
                  "Most readability tests require a minimum of 300 words to be able to generate "
                  "meaningful results."),
                wxNumberFormatter::ToString(project->GetTotalWords(), 0,
                    wxNumberFormatter::Style::Style_NoTrailingZeroes));
            HTMLText += L"</td></tr>";
            }
        if (overlyLongSentencePercentage >= 40)
            {
            HTMLText += L"\n<tr><td style='width:100%;'>";
            HTMLText += _(L"A large percentage of sentences are overly long.");
            HTMLText += L"</td></tr>";
            }
        if (exclamatorySentencePercentage >= 25)
            {
            HTMLText += L"\n<tr><td style='width:100%;'>";
            HTMLText += _(L"A large percentage of sentences are exclamatory, "
                          "giving the document an overall angry/excited tone. Is this intended?");
            HTMLText += L"</td></tr>";
            }
        if (averageCharacterCount >= 6)
            {
            HTMLText += L"\n<tr><td style='width:100%;'>";
            HTMLText += _(L"The average word length is high.");
            HTMLText += L"</td></tr>";
            }
        if (averageSyllableCount >= 4)
            {
            HTMLText += L"\n<tr><td style='width:100%;'>";
            HTMLText += _(L"The average word complexity (syllable count) is high.");
            HTMLText += L"</td></tr>";
            }
        HTMLText += L"\n</table>";
        }

    // trim off any trailing breaks
    HTMLText = TrimTrailingBreaks(HTMLText);

    if (listData)
        { listData->SetSize(listDataItemCount, listData->GetColumnCount()); }

    return HTMLText;
    }

//------------------------------------------------
wxString ProjectReportFormat::FormatTestResult(const wxString& score,
    const readability::readability_test& theTest,
    const wxString& note /*= wxEmptyString*/)
    {
    return wxString::Format(
        L"<tr><td style='vertical-align:top; width:60%%;'><p>%s</p><p>%s</p></td><td>%s</td></tr>\n",
        score,
        theTest.get_description().c_str(),
        ProjectReportFormat::FormatTestFactors(theTest)) +
        (note.length() ?
            wxString::Format(L"<tr><td colspan='2'>%s</td></tr>\n",
                ProjectReportFormat::FormatHtmlNoteSection(note)) :
            L"");
    }

//------------------------------------------------
wxString ProjectReportFormat::FormatTestFactors(const readability::readability_test& test)
    {
    wxString HTMLText =
        wxString::Format(
            L"<table border='0' style='width:100%%'>\n"
             "<tr><th colspan='2' style='text-align:left;'>%s</th></tr>",
            _(L"Difficulty Prediction Factors"));

    const auto formatRow = [](const wxString& label, const bool value, const bool gray) -> wxString
        {
        return wxString::Format(
            L"\n<tr style='%s'><td><span style='%s'>%s</span></td>"
             "<td style='text-align:center;'><span style='%s'>%s</span></td></tr>",
            (gray ? L"background:#F2F2F2;" : L""),
            (gray ? L"color:#000000;" : L""),
            label,
            (gray ? L"color:#000000;" : L""),
            (value ? L"&nbsp;X&nbsp;" : L"&nbsp;&nbsp;&nbsp;"));
        };

    // word complexity
    if (test.has_factor(readability::test_factor::word_complexity_2_plus_syllables))
        {
        HTMLText += formatRow(_(L"Word complexity (2 or more syllables)"), true, true);
        }
    else if (test.has_factor(readability::test_factor::word_complexity_3_plus_syllables))
        {
        HTMLText += formatRow(_(L"Word complexity (3 or more syllables)"), true, true);
        }
    else if (test.has_factor(readability::test_factor::word_complexity_density))
        {
        HTMLText += formatRow(_(L"Density of complex words"), true, true);
        }
    else
        {
        HTMLText += formatRow(_(L"Word complexity (syllable counts)"),
            test.has_factor(readability::test_factor::word_complexity),
            true);
        }

    // word length
    if (test.has_factor(readability::test_factor::word_length_3_less))
        {
        HTMLText += formatRow(_(L"Word length (3 or less characters)"), true, false);
        }
    else if (test.has_factor(readability::test_factor::word_length_6_plus))
        {
        HTMLText += formatRow(_(L"Word length (6 or more characters)"), true, false);
        }
    else if (test.has_factor(readability::test_factor::word_length_7_plus))
        {
        HTMLText += formatRow(_(L"Word length (7 or more characters)"), true, false);
        }
    else
        {
        HTMLText += formatRow(_(L"Word length"),
            test.has_factor(readability::test_factor::word_length),
            false);
        }

    // word familiarity
    if (test.has_factor(readability::test_factor::word_familiarity_spache))
        {
        HTMLText += formatRow(wxString::Format(_(L"Word familiarity (%s rules)"),
            _DT(L"Spache")), true, true);
        }
    else if (test.has_factor(readability::test_factor::word_familiarity_dale_chall))
        {
        HTMLText += formatRow(wxString::Format(_(L"Word familiarity (%s rules)"),
            _DT(L"Dale-Chall")), true, true);
        }
    else if (test.has_factor(readability::test_factor::word_familiarity_harris_jacobson))
        {
        HTMLText += formatRow(wxString::Format(_(L"Word familiarity (%s rules)"),
            _DT(L"Harris-Jacobson")), true, true);
        }
    else
        {
        HTMLText += formatRow(_(L"Word familiarity"), false, true);
        }

    // sentence length
    HTMLText += formatRow(_(L"Sentence length"),
        test.has_factor(readability::test_factor::sentence_length),
        false);

    HTMLText += L"\n</table>\n";
    return HTMLText;
    }

//------------------------------------------------
wxString ProjectReportFormat::TrimTrailingBreaks(const wxString& text)
    {
    wxString trimmedText = text;
    trimmedText.Trim();
    const wxString trailingBreaks = L"<br />";
    while (trimmedText.ends_with(trailingBreaks))
        { trimmedText.RemoveLast(trailingBreaks.length()); }
    return trimmedText;
    }

//------------------------------------------------
wxString ProjectReportFormat::FormatHtmlWarningSection(const wxString& note)
    {
    return wxString::Format(
        L"\n<table class='minipage' style='width:100%%;' border='0'>\n"
         "<tr><th rowspan='2' style='width:20%%;'></th>"
         "<th style='text-align:left; background:yellow;'><span style='color:black;'>%s</span></th></tr>\n"
         "<tr><td style='text-align:left'>%s</td></tr></table>\n",
                            wxString(BaseProjectView::GetWarningEmoji() + _(L" Warning")),
                            note);
    }

//------------------------------------------------
wxString ProjectReportFormat::FormatHtmlNoteSection(const wxString& note)
    {
    // note that wxHTML doesn't support 'color' style on the <tr> element,
    // so we need to use a <span> for that
    return wxString::Format(
        L"\n<table class='minipage' style='width:100%%;' border='0'>\n"
         "<tr><th rowspan='2' style='width:20%%;'></th>"
         "<th style='text-align:left; background:%s;'><span style='color:white;'>%s</span></th></tr>\n"
         "<tr><td style='text-align:left'>%s</td></tr></table>\n",
                            GetReportNoteHeaderColor().GetAsString(wxC2S_HTML_SYNTAX),
                            // can't use note emoji because wxHTML printing system can't handle it
                            _(L"Note"),
                            note);
    }

//---------------------------------------------
wxString ProjectReportFormat::FormatSentence(const BaseProject* project,
    const grammar::sentence_info& sentence,
    std::vector<punctuation::punctuation_mark>::const_iterator& punctStart,
    const std::vector<punctuation::punctuation_mark>::const_iterator& punctEnd)
    {
    wxASSERT(project);
    if (!project)
        { return wxEmptyString; }
    wxString currentSentence;
    for (size_t i = sentence.get_first_word_index(); i <= sentence.get_last_word_index(); ++i)
        {
        // append any punctuation that should be in front of this word.
        // first, play "catch up" to get the punctuation iterator up to the current word.
        while (punctStart != punctEnd &&
            punctStart->get_word_position() < i)
            { ++punctStart; }
        while (punctStart != punctEnd &&
            punctStart->get_word_position() == i)
            {
            if (!punctStart->is_connected_to_previous_word())
                { currentSentence += punctStart->get_punctuation_mark(); }
            ++punctStart;
            }
        currentSentence += project->GetWords()->get_words()[i].c_str();
        // append any punctuation that should be after this word
        while (punctStart != punctEnd &&
            punctStart->get_word_position() == i+1 &&
            punctStart->is_connected_to_previous_word() )
            {
            currentSentence += punctStart->get_punctuation_mark();
            ++punctStart;
            }
        currentSentence += L' ';
        }
    currentSentence.Trim();

    bool quoteBeingMoved = false;
    wchar_t quoteChar{ L'\"' };
    if (currentSentence.length() > 0 && characters::is_character::is_quote(currentSentence.Last()))
        {
        quoteChar = currentSentence.Last();
        quoteBeingMoved = true;
        currentSentence.RemoveLast();
        }
    currentSentence += sentence.get_ending_punctuation();
    if (quoteBeingMoved)
        { currentSentence += quoteChar; }

    return currentSentence;
    }
