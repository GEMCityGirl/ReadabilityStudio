#include "base_project.h"
#include "base_project_view.h"
#include "../app/readability_app.h"
#include "../results_format/project_report_format.h"
#include "../results_format/word_collectiont_text_formatting.h"
#include "../ui/dialogs/filtered_text_preview_dlg.h"
#include "../indexing/romanize.h"
#include "../Wisteria-Dataviz/src/import/html_encode.h"
#include "../Wisteria-Dataviz/src/import/idl_extract_text.h"
#include "../Wisteria-Dataviz/src/import/pptx_extract_text.h"
#include "../Wisteria-Dataviz/src/import/cpp_extract_text.h"

/***************************************************************************
                          BaseProject.h  -  description
                             -------------------
    copyright            : (C) 2006 by Blake Madden, Oleander Software
 ***************************************************************************/

wxDECLARE_APP(ReadabilityApp);

grammar::phrase_collection BaseProject::english_wordy_phrases;
grammar::phrase_collection BaseProject::spanish_wordy_phrases;
grammar::phrase_collection BaseProject::german_wordy_phrases;
grammar::phrase_collection BaseProject::copyright_notice_phrases;
grammar::phrase_collection BaseProject::citation_phrases;
word_list BaseProject::known_proper_nouns;
word_list BaseProject::known_personal_nouns;
word_list BaseProject::known_english_spellings;
word_list BaseProject::known_programming_spellings;
word_list BaseProject::known_custom_english_spellings;
word_list BaseProject::known_spanish_spellings;
word_list BaseProject::known_custom_spanish_spellings;
word_list BaseProject::known_german_spellings;
word_list BaseProject::known_custom_german_spellings;
word_list BaseProject::m_dale_chall_word_list;
word_list BaseProject::m_dale_chall_plus_stocker_catholic_word_list;
word_list BaseProject::m_stocker_catholic_word_list;
word_list BaseProject::m_spache_word_list;
word_list BaseProject::m_harris_jacobson_word_list;
word_list BaseProject::english_stoplist;
word_list BaseProject::spanish_stoplist;
word_list BaseProject::german_stoplist;
word_list_with_replacements BaseProject::dale_chall_replacement_list;
word_list_with_replacements BaseProject::spache_replacement_list;
word_list_with_replacements BaseProject::harris_jacobson_replacement_list;
word_list_with_replacements BaseProject::difficult_word_replacement_list;
readability::dolch_word_list BaseProject::m_dolch_word_list;
CustomReadabilityTestCollection BaseProject::m_custom_word_tests;
readability::readability_test_collection<readability::readability_test> BaseProject::m_defaultReadabilityTestsTemplate;
std::set<TestBundle> BaseProject::m_testBundles;
// defines the stat goals' internal labels, display labels, and lambdas to calculate them
std::map<comparable_first_pair<Goal::string_type, Goal::string_type>, std::function<double(const BaseProject*)>> BaseProject::m_statGoalLabels =
    {
        { { _DT(L"average-sentence-length"), _(L"Average Sentence Length").wc_str() },
            [](const BaseProject* proj) { return safe_divide<double>(proj->GetTotalWords(), proj->GetTotalSentences()); } },
        { { _DT(L"sentences-per-100-words"), _(L"Sentences per 100 Words").wc_str() },
            [](const BaseProject* proj) { return safe_divide<double>(100, proj->GetTotalWords()) * proj->GetTotalSentences(); } },
        { { _DT(L"syllables-per-100-words"), _(L"Syllables per 100 Words").wc_str() },
            [](const BaseProject* proj) { return safe_divide<double>(100, proj->GetTotalWords()) * proj->GetTotalSyllables(); } }
    };

using namespace lily_of_the_valley;
using namespace Wisteria;

//------------------------------------------------
bool BaseProject::LoadAppendedDocument()
    {
    if (GetAppendedDocumentFilePath().length())
        {
        if (!wxFile::Exists(GetAppendedDocumentFilePath()) )
            {
            wxString fileBySameNameInProjectDirectory;
            if (FindMissingFile(GetAppendedDocumentFilePath(), fileBySameNameInProjectDirectory))
                { SetAppendedDocumentFilePath(fileBySameNameInProjectDirectory); }
            else
                {
                LogMessage(wxString::Format(_(L"\"%s\": appended template file not found."), GetAppendedDocumentFilePath()), 
                     wxGetApp().GetAppName(), wxOK|wxICON_EXCLAMATION);
                SetAppendedDocumentText(wxEmptyString);
                return false;
                }
            }
        MemoryMappedFile sourceFile(GetAppendedDocumentFilePath(), true, true);
        const std::pair<bool,wxString> extractResult = ExtractRawText(static_cast<const char*>(sourceFile.GetStream()), sourceFile.GetMapSize(), wxFileName(GetAppendedDocumentFilePath()).GetExt());
        SetAppendedDocumentText(extractResult.first ? extractResult.second : wxString{});
        return extractResult.first;
        }
    else
        { SetAppendedDocumentText(wxEmptyString); }
    return true;
    }

void BaseProject::UpdateDocumentSettings()
    {
    wxASSERT(GetWords());
    GetWords()->set_allowable_incomplete_sentence_size(GetIncludeIncompleteSentencesIfLongerThanValue());
    GetWords()->set_aggressive_exclusion(IsExcludingAggressively());
    GetWords()->ignore_trailing_copyright_notice_paragraphs(IsIgnoringTrailingCopyrightNoticeParagraphs());
    GetWords()->ignore_citation_sections(IsIgnoringTrailingCitations());
    GetWords()->treat_header_words_as_valid(GetInvalidSentenceMethod() == InvalidSentence::ExcludeExceptForHeadings);
    GetWords()->treat_eol_as_eos((m_paragraphsParsingMethod == ParagraphParse::EachNewLineIsAParagraph));
    GetWords()->ignore_blank_lines_when_determing_paragraph_split(GetIgnoreBlankLinesForParagraphsParser());
    GetWords()->ignore_indenting_when_determing_paragraph_split(GetIgnoreIndentingForParagraphsParser());
    GetWords()->sentence_start_must_be_uppercased(GetSentenceStartMustBeUppercased());
    GetWords()->set_copyright_phrase_function(&copyright_notice_phrases);
    GetWords()->set_citation_phrase_function(&citation_phrases);
    GetWords()->set_known_proper_nouns(&known_proper_nouns);
    GetWords()->set_known_personal_nouns(&known_personal_nouns);
    GetWords()->get_spell_checker().set_programmer_word_list(&known_programming_spellings);
    GetWords()->get_spell_checker().ignore_proper_nouns(SpellCheckIsIgnoringProperNouns());
    GetWords()->get_spell_checker().ignore_uppercased(SpellCheckIsIgnoringUppercased());
    GetWords()->get_spell_checker().ignore_numerals(SpellCheckIsIgnoringNumerals());
    GetWords()->get_spell_checker().ignore_file_addresses(SpellCheckIsIgnoringFileAddresses());
    GetWords()->get_spell_checker().ignore_programmer_code(SpellCheckIsIgnoringProgrammerCode());
    GetWords()->get_spell_checker().allow_colloquialisms(SpellCheckIsAllowingColloquialisms());
    GetWords()->get_spell_checker().ignore_social_media_tags(SpellCheckIsIgnoringSocialMediaTags());
    GetWords()->exclude_file_addresses(IsIgnoringFileAddresses());
    GetWords()->exclude_numerals(IsIgnoringNumerals());
    GetWords()->exclude_proper_nouns(IsIgnoringProperNouns());
    GetWords()->set_excluded_phrase_function(m_excluded_phrases);
    GetWords()->include_excluded_phrase_first_occurrence(IsIncludingExcludedPhraseFirstOccurrence());
    GetWords()->clear_exclusion_block_tags();
    for (std::vector<std::pair<wchar_t,wchar_t>>::const_iterator tagPos = m_exclusionBlockTags.begin();
        tagPos != m_exclusionBlockTags.end();
        ++tagPos)
        { GetWords()->add_exclusion_block_tags(tagPos->first, tagPos->second); }
    //language-specific settings
    if (GetProjectLanguage() == readability::test_language::spanish_test &&
        (wxGetApp().GetLicenseAdmin().IsFeatureEnabled(wxGetApp().FeatureLanguagePackCode()) ||
        wxGetApp().GetLicenseAdmin().IsFeatureEnabled(wxGetApp().FeatureProfessionalCode())) )
        {
        GetWords()->set_syllabizer(&m_spanish_syllabize);
        GetWords()->set_stemmer(&m_spanish_stem);
        GetWords()->set_stop_list(&GetStopList());
        GetWords()->set_conjunction_function(&m_spanish_conjunction);
        GetWords()->set_known_phrase_function(&spanish_wordy_phrases);
        GetWords()->get_spell_checker().set_word_list(&known_spanish_spellings);
        GetWords()->get_spell_checker().set_secondary_word_list(&known_custom_spanish_spellings);
        GetWords()->set_search_for_proper_nouns(true);
        GetWords()->set_mismatched_article_function(nullptr);
        GetWords()->set_search_for_passive_voice(false);
        }
    else if (GetProjectLanguage() == readability::test_language::german_test &&
        (wxGetApp().GetLicenseAdmin().IsFeatureEnabled(wxGetApp().FeatureLanguagePackCode()) ||
        wxGetApp().GetLicenseAdmin().IsFeatureEnabled(wxGetApp().FeatureProfessionalCode())) )
        {
        GetWords()->set_syllabizer(&m_german_syllabize);
        GetWords()->set_stemmer(&m_german_stem);
        GetWords()->set_stop_list(&GetStopList());
        GetWords()->set_conjunction_function(&m_german_conjunction);
        GetWords()->set_known_phrase_function(&german_wordy_phrases);
        GetWords()->get_spell_checker().set_word_list(&known_german_spellings);
        GetWords()->get_spell_checker().set_secondary_word_list(&known_custom_german_spellings);
        GetWords()->set_search_for_proper_nouns(false);
        GetWords()->set_mismatched_article_function(nullptr);
        GetWords()->set_search_for_passive_voice(false);
        }
    else
        {
        GetWords()->set_syllabizer(&m_english_syllabize);
        GetWords()->set_stemmer(&m_english_stem);
        GetWords()->set_stop_list(&GetStopList());
        GetWords()->set_conjunction_function(&m_english_conjunction);
        GetWords()->set_known_phrase_function(&english_wordy_phrases);
        GetWords()->get_spell_checker().set_word_list(&known_english_spellings);
        GetWords()->get_spell_checker().set_secondary_word_list(&known_custom_english_spellings);
        GetWords()->set_search_for_proper_nouns(true);
        GetWords()->set_mismatched_article_function(&m_english_mismatched_article);
        GetWords()->set_search_for_passive_voice(true);
        }
    }

void BaseProject::LogMessage(wxString message, const wxString& title, const int icon,
                             const wxString& messageId /*= wxEmptyString*/,
                             const bool queue /*= false*/)
    {
    //skip warning if it was asked to be suppressed already
    std::vector<WarningMessage>::iterator warningIter =
            wxGetApp().GetAppOptions().GetWarning(messageId);
    if (warningIter != wxGetApp().GetAppOptions().GetWarnings().end() &&
        warningIter->ShouldBeShown() == false)
        { return; }

    if (queue)
        { AddQueuedMessage(WarningMessage(messageId,message,title,wxEmptyString,icon)); }
    else if (HasUI())
        {
        wxRichMessageDialog msg(wxGetApp().GetMainFrame(), message,
            (title.length() ? title : wxGetApp().GetAppDisplayName()), icon);
        msg.ShowCheckBox(messageId.length() ? _(L"Do not show this again") : wxString{});
        const int dlgResponse = msg.ShowModal();
        if (dlgResponse != wxID_NO &&
            warningIter != wxGetApp().GetAppOptions().GetWarnings().end() &&
            msg.IsCheckBoxChecked())
            {
            warningIter->Show(false);
            warningIter->SetPreviousResponse(dlgResponse);
            }
        }
    else
        { AddQuietSubProjectMessage(message,icon); }
    message.Replace(L"\n", L" ", true);
    if (icon & wxICON_ERROR)
        { wxLogError(L"%s", message); }
    else if (icon & wxICON_EXCLAMATION)
        { wxLogWarning(L"%s", message); }
    else
        { wxLogVerbose(L"%s", message); }
    }

void BaseProject::FormatFilteredText(wxString& text, const bool romanizeText,
                                     const bool removeEllipses, const bool removeBullets,
                                     const bool removeFilePaths, const bool stripAbbreviations,
                                     const bool narrowFullWithText) const
    {
    if (!wxGetApp().GetLicenseAdmin().IsFeatureEnabled(wxGetApp().FeatureProfessionalCode()))
        {
        wxMessageBox(_(L"Filtered document exporting is only available in the Professional Edition of Readability Studio."), 
            _(L"Feature Not Licensed"), wxOK|wxICON_INFORMATION);
        return;
        }

    auto project = std::make_unique<BaseProject>();
    project->CopySettings(*this);
    project->SetAppendedDocumentText(GetAppendedDocumentText());
    project->ShareExcludePhrases(*this);
    project->SetUIMode(false);
    project->SetOriginalDocumentFilePath(GetOriginalDocumentFilePath());

    if (project->GetDocumentStorageMethod() == TextStorage::NoEmbedText)
        { project->FreeDocumentText(); }
    if (!project->LoadDocumentAsSubProject(project->GetOriginalDocumentFilePath(), project->GetDocumentText(), 1) ||
        project->GetDocumentText().length() == 0)
        { return; }
    const size_t textLength = ((project->GetDocumentText().length()+project->GetAppendedDocumentText().length())*2);
    FormatFilteredWordCollection(project->GetWords(),
        wxStringBuffer(text, textLength),
        textLength,
        (GetInvalidSentenceMethod() == InvalidSentence::IncludeAsFullSentences) ? InvalidTextFilterFormat::IncludeAllText : InvalidTextFilterFormat::IncludeOnlyValidText,
        removeFilePaths, stripAbbreviations);
    text.Trim(false); text.Trim(true); text.Prepend(L"\t");

    const text_transform::romanize Romanize;
    text = Romanize(text, text.length(), romanizeText, removeEllipses, removeBullets, narrowFullWithText);
    }

/// Indicates whether a test is included which includes a cloze score
bool BaseProject::IsIncludingClozeTest() const
    {
    for (auto rTests = GetReadabilityTests().get_tests().begin();
        rTests != GetReadabilityTests().get_tests().end();
        ++rTests)
        {
        if (rTests->is_included() &&
            (rTests->get_test().get_test_type() == readability::readability_test_type::predicted_cloze_score ||
            rTests->get_test().get_test_type() == readability::readability_test_type::grade_level_and_predicted_cloze_score))
            { return true; }
        }
    for (std::vector<CustomReadabilityTestInterface>::const_iterator customTestPos = GetCustTestsInUse().begin();
            customTestPos != GetCustTestsInUse().end();
            ++customTestPos)
        {
        if (customTestPos->GetIterator()->get_test_type() == readability::readability_test_type::predicted_cloze_score)
            { return true; }
        }
    return false;
    }

///Indicates whether a test is included which includes a grade-level score
bool BaseProject::IsIncludingGradeTest() const
    {
    for (auto rTests = GetReadabilityTests().get_tests().begin();
        rTests != GetReadabilityTests().get_tests().end();
        ++rTests)
        {
        if (rTests->is_included() &&
            (rTests->get_test().get_test_type() == readability::readability_test_type::grade_level ||
            rTests->get_test().get_test_type() == readability::readability_test_type::index_value_and_grade_level ||
            rTests->get_test().get_test_type() == readability::readability_test_type::grade_level_and_predicted_cloze_score))
            { return true; }
        }
    for (std::vector<CustomReadabilityTestInterface>::const_iterator customTestPos = GetCustTestsInUse().begin();
            customTestPos != GetCustTestsInUse().end();
            ++customTestPos)
        {
        if (customTestPos->GetIterator()->get_test_type() == readability::readability_test_type::grade_level)
            { return true; }
        }
    return false;
    }

BaseProject::BaseProject() :
    m_minDocWordCountForBatch(wxGetApp().GetAppOptions().GetMinDocWordCountForBatch()),
    m_includeIncompleteSentencesIfLongerThan(wxGetApp().GetAppOptions().GetIncludeIncompleteSentencesIfLongerThanValue()),

    m_difficultSentenceLength(wxGetApp().GetAppOptions().GetDifficultSentenceLength()),

    m_numeralSyllabicationMethod(wxGetApp().GetAppOptions().GetNumeralSyllabicationMethod()),
    m_longSentenceMethod(wxGetApp().GetAppOptions().GetLongSentenceMethod()),
    m_paragraphsParsingMethod(wxGetApp().GetAppOptions().GetParagraphsParsingMethod()),
    m_invalidSentenceMethod(wxGetApp().GetAppOptions().GetInvalidSentenceMethod()),
    m_textSource(wxGetApp().GetAppOptions().GetTextSource()),
    m_documentStorageMethod(wxGetApp().GetAppOptions().GetDocumentStorageMethod()),
    m_varianceMethod(wxGetApp().GetAppOptions().GetVarianceMethod()),
    // readability scores options
    m_includeScoreSummaryReport(wxGetApp().GetAppOptions().IsIncludingScoreSummaryReport()),
    // test options
    m_hjTextExclusion(wxGetApp().GetAppOptions().GetHarrisJacobsonTextExclusionMode()),
    m_dcTextExclusion(wxGetApp().GetAppOptions().GetDaleChallTextExclusionMode()),
    m_dcProperNounCountingMethod(wxGetApp().GetAppOptions().GetDaleChallProperNounCountingMethod()),
    m_fleschNumeralSyllabizeMethod(wxGetApp().GetAppOptions().GetFleschNumeralSyllabizeMethod()),
    m_fleschKincaidNumeralSyllabizeMethod(wxGetApp().GetAppOptions().GetFleschKincaidNumeralSyllabizeMethod()),
    // language options
    m_language(wxGetApp().GetAppOptions().GetProjectLanguage()),
    m_reviewer(wxGetApp().GetAppOptions().GetReviewer()),
    m_status(wxGetApp().GetAppOptions().GetStatus()),
    m_appendedDocumentFilePath(wxGetApp().GetAppOptions().GetAppendedDocumentFilePath()),
    // grammar
    m_spellcheck_ignore_proper_nouns(wxGetApp().GetAppOptions().SpellCheckIsIgnoringProperNouns()),
    m_spellcheck_ignore_uppercased(wxGetApp().GetAppOptions().SpellCheckIsIgnoringUppercased()),
    m_spellcheck_ignore_numerals(wxGetApp().GetAppOptions().SpellCheckIsIgnoringNumerals()),
    m_spellcheck_ignore_file_addresses(wxGetApp().GetAppOptions().SpellCheckIsIgnoringFileAddresses()),
    m_spellcheck_ignore_programmer_code(wxGetApp().GetAppOptions().SpellCheckIsIgnoringProgrammerCode()),
    m_allow_colloquialisms(wxGetApp().GetAppOptions().SpellCheckIsAllowingColloquialisms()),
    m_spellcheck_ignore_social_media_tags(wxGetApp().GetAppOptions().SpellCheckIsIgnoringSocialMediaTags()),

    m_ignoreBlankLinesForParagraphsParser(wxGetApp().GetAppOptions().GetIgnoreBlankLinesForParagraphsParser()),
    m_ignoreIndentingForParagraphsParser(wxGetApp().GetAppOptions().GetIgnoreIndentingForParagraphsParser()),
    m_sentenceStartMustBeUppercased(wxGetApp().GetAppOptions().GetSentenceStartMustBeUppercased()),
    m_aggressiveExclusion(wxGetApp().GetAppOptions().IsExcludingAggressively()),
    m_ignoreTrailingCopyrightNoticeParagraphs(wxGetApp().GetAppOptions().IsIgnoringTrailingCopyrightNoticeParagraphs()),
    m_ignoreTrailingCitations(wxGetApp().GetAppOptions().IsIgnoringTrailingCitations()),
    m_ignoreFileAddresses(wxGetApp().GetAppOptions().IsIgnoringFileAddresses()),
    m_ignoreNumerals(wxGetApp().GetAppOptions().IsIgnoringNumerals()),
    m_ignoreProperNouns(wxGetApp().GetAppOptions().IsIgnoringProperNouns()),
    m_includeExcludedPhraseFirstOccurrence(wxGetApp().GetAppOptions().IsIncludingExcludedPhraseFirstOccurrence()),

    m_fogUseSentenceUnits(wxGetApp().GetAppOptions().FogUseSentenceUnits()),
    m_includeStockerCatholicDCSupplement(wxGetApp().GetAppOptions().IsIncludingStockerCatholicSupplement()),

    m_statsInfo(wxGetApp().GetAppOptions().GetStatisticsInfo()),
    m_statsReportInfo(wxGetApp().GetAppOptions().GetStatisticsReportInfo()),
    m_grammarInfo(wxGetApp().GetAppOptions().GetGrammarInfo()),
    m_wordsBreakdownInfo(wxGetApp().GetAppOptions().GetWordsBreakdownInfo()),
    m_sentencesBreakdownInfo(wxGetApp().GetAppOptions().GetSentencesBreakdownInfo()),
    m_excludedPhrasesPath(wxGetApp().GetAppOptions().GetExcludedPhrasesPath()),
    m_exclusionBlockTags(wxGetApp().GetAppOptions().GetExclusionBlockTags())
    {
    ResetStandardReadabilityTests(m_readabilityTests);
    // bind the standard test functions with their respective IDs
    m_standardTestFunctions.reserve(GetDefaultReadabilityTestsTemplate().get_test_count());
    //DEGREES_OF_READING_POWER
    std::pair<std::vector<readability::readability_test>::const_iterator, bool> testPos =
        GetDefaultReadabilityTestsTemplate().find_test(ReadabilityMessages::DEGREES_OF_READING_POWER());
    if (testPos.second)
        {
        m_standardTestFunctions.push_back(comparable_first_pair<int, AddTestFunction>(testPos.first->get_interface_id(),
            &BaseProject::AddDegreesOfReadingPowerTest));
        }
    //DEGREES_OF_READING_POWER_GE
    testPos = GetDefaultReadabilityTestsTemplate().find_test(ReadabilityMessages::DEGREES_OF_READING_POWER_GE());
    if (testPos.second)
        {
        m_standardTestFunctions.push_back(comparable_first_pair<int, AddTestFunction>(testPos.first->get_interface_id(),
            &BaseProject::AddDegreesOfReadingPowerGeTest));
        }
    //SOL_SPANISH
    testPos = GetDefaultReadabilityTestsTemplate().find_test(ReadabilityMessages::SOL_SPANISH());
    if (testPos.second)
        {
        m_standardTestFunctions.push_back(comparable_first_pair<int, AddTestFunction>(testPos.first->get_interface_id(),
            &BaseProject::AddSolSpanishTest));
        }
    //CRAWFORD
    testPos = GetDefaultReadabilityTestsTemplate().find_test(ReadabilityMessages::CRAWFORD());
    if (testPos.second)
        {
        m_standardTestFunctions.push_back(comparable_first_pair<int, AddTestFunction>(testPos.first->get_interface_id(),
            &BaseProject::AddCrawfordTest));
        }
    //FRASE
    testPos = GetDefaultReadabilityTestsTemplate().find_test(ReadabilityMessages::FRASE());
    if (testPos.second)
        {
        m_standardTestFunctions.push_back(comparable_first_pair<int, AddTestFunction>(testPos.first->get_interface_id(),
            &BaseProject::AddFraseTest));
        }
    //GPM_FRY
    testPos = GetDefaultReadabilityTestsTemplate().find_test(ReadabilityMessages::GPM_FRY());
    if (testPos.second)
        {
        m_standardTestFunctions.push_back(comparable_first_pair<int, AddTestFunction>(testPos.first->get_interface_id(),
            &BaseProject::AddGilliamPenaMountainFryTest));
        }
    //PSK_FARR_JENKINS_PATERSON
    testPos = GetDefaultReadabilityTestsTemplate().find_test(ReadabilityMessages::PSK_FARR_JENKINS_PATERSON());
    if (testPos.second)
        {
        m_standardTestFunctions.push_back(comparable_first_pair<int, AddTestFunction>(testPos.first->get_interface_id(),
            &BaseProject::AddPskFarrJenkinsPatersonTest));
        }
    //SPACHE
    testPos = GetDefaultReadabilityTestsTemplate().find_test(ReadabilityMessages::SPACHE());
    if (testPos.second)
        {
        m_standardTestFunctions.push_back(comparable_first_pair<int, AddTestFunction>(testPos.first->get_interface_id(),
            &BaseProject::AddSpacheTest));
        }
    //HARRIS_JACOBSON
    testPos = GetDefaultReadabilityTestsTemplate().find_test(ReadabilityMessages::HARRIS_JACOBSON());
    if (testPos.second)
        {
        m_standardTestFunctions.push_back(comparable_first_pair<int, AddTestFunction>(testPos.first->get_interface_id(),
            &BaseProject::AddHarrisJacobsonTest));
        }
    //ARI
    testPos = GetDefaultReadabilityTestsTemplate().find_test(ReadabilityMessages::ARI());
    if (testPos.second)
        {
        m_standardTestFunctions.push_back(comparable_first_pair<int, AddTestFunction>(testPos.first->get_interface_id(),
            &BaseProject::AddAriTest));
        }
    //GUNNING_FOG
    testPos = GetDefaultReadabilityTestsTemplate().find_test(ReadabilityMessages::GUNNING_FOG());
    if (testPos.second)
        {
        m_standardTestFunctions.push_back(comparable_first_pair<int, AddTestFunction>(testPos.first->get_interface_id(),
            &BaseProject::AddFogTest));
        }
    //LIX
    testPos = GetDefaultReadabilityTestsTemplate().find_test(ReadabilityMessages::LIX());
    if (testPos.second)
        {
        m_standardTestFunctions.push_back(comparable_first_pair<int, AddTestFunction>(testPos.first->get_interface_id(),
            &BaseProject::AddLixTest));
        }
    //RIX
    testPos = GetDefaultReadabilityTestsTemplate().find_test(ReadabilityMessages::RIX());
    if (testPos.second)
        {
        m_standardTestFunctions.push_back(comparable_first_pair<int, AddTestFunction>(testPos.first->get_interface_id(),
            &BaseProject::AddRixTest));
        }
    //DALE_CHALL
    testPos = GetDefaultReadabilityTestsTemplate().find_test(ReadabilityMessages::DALE_CHALL());
    if (testPos.second)
        {
        m_standardTestFunctions.push_back(comparable_first_pair<int, AddTestFunction>(testPos.first->get_interface_id(),
            &BaseProject::AddNewDaleChallTest));
        }
    //COLEMAN_LIAU
    testPos = GetDefaultReadabilityTestsTemplate().find_test(ReadabilityMessages::COLEMAN_LIAU());
    if (testPos.second)
        {
        m_standardTestFunctions.push_back(comparable_first_pair<int, AddTestFunction>(testPos.first->get_interface_id(),
            &BaseProject::AddColemanLiauTest));
        }
    //FORCAST
    testPos = GetDefaultReadabilityTestsTemplate().find_test(ReadabilityMessages::FORCAST());
    if (testPos.second)
        {
        m_standardTestFunctions.push_back(comparable_first_pair<int, AddTestFunction>(testPos.first->get_interface_id(),
            &BaseProject::AddForcastTest));
        }
    //FLESCH
    testPos = GetDefaultReadabilityTestsTemplate().find_test(ReadabilityMessages::FLESCH());
    if (testPos.second)
        {
        m_standardTestFunctions.push_back(comparable_first_pair<int, AddTestFunction>(testPos.first->get_interface_id(),
            &BaseProject::AddFleschTest));
        }
    //NEW_FOG
    testPos = GetDefaultReadabilityTestsTemplate().find_test(ReadabilityMessages::NEW_FOG());
    if (testPos.second)
        {
        m_standardTestFunctions.push_back(comparable_first_pair<int, AddTestFunction>(testPos.first->get_interface_id(),
            &BaseProject::AddNewFogCountTest));
        }
    //FLESCH_KINCAID_SIMPLIFIED
    testPos = GetDefaultReadabilityTestsTemplate().find_test(ReadabilityMessages::FLESCH_KINCAID_SIMPLIFIED());
    if (testPos.second)
        {
        m_standardTestFunctions.push_back(comparable_first_pair<int, AddTestFunction>(testPos.first->get_interface_id(),
            &BaseProject::AddFleschKincaidSimplifiedTest));
        }
    //FLESCH_KINCAID
    testPos = GetDefaultReadabilityTestsTemplate().find_test(ReadabilityMessages::FLESCH_KINCAID());
    if (testPos.second)
        {
        m_standardTestFunctions.push_back(comparable_first_pair<int, AddTestFunction>(testPos.first->get_interface_id(),
            &BaseProject::AddFleschKincaidTest));
        }
    //EFLAW
    testPos = GetDefaultReadabilityTestsTemplate().find_test(ReadabilityMessages::EFLAW());
    if (testPos.second)
        {
        m_standardTestFunctions.push_back(comparable_first_pair<int, AddTestFunction>(testPos.first->get_interface_id(),
            &BaseProject::AddEflawTest));
        }
    //SMOG
    testPos = GetDefaultReadabilityTestsTemplate().find_test(ReadabilityMessages::SMOG());
    if (testPos.second)
        {
        m_standardTestFunctions.push_back(comparable_first_pair<int, AddTestFunction>(testPos.first->get_interface_id(),
            &BaseProject::AddSmogTest));
        }
    //SMOG_BAMBERGER_VANECEK
    testPos = GetDefaultReadabilityTestsTemplate().find_test(ReadabilityMessages::SMOG_BAMBERGER_VANECEK());
    if (testPos.second)
        {
        m_standardTestFunctions.push_back(comparable_first_pair<int, AddTestFunction>(testPos.first->get_interface_id(),
            &BaseProject::AddSmogBambergerVanecekTest));
        }
    //SCHWARTZ
    testPos = GetDefaultReadabilityTestsTemplate().find_test(ReadabilityMessages::SCHWARTZ());
    if (testPos.second)
        {
        m_standardTestFunctions.push_back(comparable_first_pair<int, AddTestFunction>(testPos.first->get_interface_id(),
            &BaseProject::AddSchwartzTest));
        }
    //QU
    testPos = GetDefaultReadabilityTestsTemplate().find_test(ReadabilityMessages::QU());
    if (testPos.second)
        {
        m_standardTestFunctions.push_back(comparable_first_pair<int, AddTestFunction>(testPos.first->get_interface_id(),
            &BaseProject::AddQuBambergerVanecekTest));
        }
    //MODIFIED_SMOG
    testPos = GetDefaultReadabilityTestsTemplate().find_test(ReadabilityMessages::MODIFIED_SMOG());
    if (testPos.second)
        {
        m_standardTestFunctions.push_back(comparable_first_pair<int, AddTestFunction>(testPos.first->get_interface_id(),
            &BaseProject::AddModifiedSmogTest));
        }
    //SMOG_SIMPLIFIED
    testPos = GetDefaultReadabilityTestsTemplate().find_test(ReadabilityMessages::SMOG_SIMPLIFIED());
    if (testPos.second)
        {
        m_standardTestFunctions.push_back(comparable_first_pair<int, AddTestFunction>(testPos.first->get_interface_id(),
            &BaseProject::AddSmogSimplifiedTest));
        }
    //SIMPLE_ARI
    testPos = GetDefaultReadabilityTestsTemplate().find_test(ReadabilityMessages::SIMPLE_ARI());
    if (testPos.second)
        {
        m_standardTestFunctions.push_back(comparable_first_pair<int, AddTestFunction>(testPos.first->get_interface_id(),
            &BaseProject::AddSimplifiedAriTest));
        }
    //NEW_ARI
    testPos = GetDefaultReadabilityTestsTemplate().find_test(ReadabilityMessages::NEW_ARI());
    if (testPos.second)
        {
        m_standardTestFunctions.push_back(comparable_first_pair<int, AddTestFunction>(testPos.first->get_interface_id(),
            &BaseProject::AddNewAriTest));
        }
    //PSK_FLESCH
    testPos = GetDefaultReadabilityTestsTemplate().find_test(ReadabilityMessages::PSK_FLESCH());
    if (testPos.second)
        {
        m_standardTestFunctions.push_back(comparable_first_pair<int, AddTestFunction>(testPos.first->get_interface_id(),
            &BaseProject::AddPskFleschTest));
        }
    //FRY
    testPos = GetDefaultReadabilityTestsTemplate().find_test(ReadabilityMessages::FRY());
    if (testPos.second)
        {
        m_standardTestFunctions.push_back(comparable_first_pair<int, AddTestFunction>(testPos.first->get_interface_id(),
            &BaseProject::AddFryTest));
        }
    //RAYGOR
    testPos = GetDefaultReadabilityTestsTemplate().find_test(ReadabilityMessages::RAYGOR());
    if (testPos.second)
        {
        m_standardTestFunctions.push_back(comparable_first_pair<int, AddTestFunction>(testPos.first->get_interface_id(),
            &BaseProject::AddRaygorTest));
        }
    //PSK_DALE_CHALL
    testPos = GetDefaultReadabilityTestsTemplate().find_test(ReadabilityMessages::PSK_DALE_CHALL());
    if (testPos.second)
        {
        m_standardTestFunctions.push_back(comparable_first_pair<int, AddTestFunction>(testPos.first->get_interface_id(),
            &BaseProject::AddPskDaleChallTest));
        }
    //BORMUTH_CLOZE_MEAN
    testPos = GetDefaultReadabilityTestsTemplate().find_test(ReadabilityMessages::BORMUTH_CLOZE_MEAN());
    if (testPos.second)
        {
        m_standardTestFunctions.push_back(comparable_first_pair<int, AddTestFunction>(testPos.first->get_interface_id(),
            &BaseProject::AddBormuthClozeMeanTest));
        }
    //BORMUTH_GRADE_PLACEMENT_35
    testPos = GetDefaultReadabilityTestsTemplate().find_test(ReadabilityMessages::BORMUTH_GRADE_PLACEMENT_35());
    if (testPos.second)
        {
        m_standardTestFunctions.push_back(comparable_first_pair<int, AddTestFunction>(testPos.first->get_interface_id(),
            &BaseProject::AddBormuthGradePlacement35Test));
        }
    //FARR_JENKINS_PATERSON
    testPos = GetDefaultReadabilityTestsTemplate().find_test(ReadabilityMessages::FARR_JENKINS_PATERSON());
    if (testPos.second)
        {
        m_standardTestFunctions.push_back(comparable_first_pair<int, AddTestFunction>(testPos.first->get_interface_id(),
            &BaseProject::AddFarrJenkinsPatersonTest));
        }
    //PSK_GUNNING_FOG
    testPos = GetDefaultReadabilityTestsTemplate().find_test(ReadabilityMessages::PSK_GUNNING_FOG());
    if (testPos.second)
        {
        m_standardTestFunctions.push_back(comparable_first_pair<int, AddTestFunction>(testPos.first->get_interface_id(),
            &BaseProject::AddPskFogTest));
        }
    //NEW_FARR_JENKINS_PATERSON
    testPos = GetDefaultReadabilityTestsTemplate().find_test(ReadabilityMessages::NEW_FARR_JENKINS_PATERSON());
    if (testPos.second)
        {
        m_standardTestFunctions.push_back(comparable_first_pair<int, AddTestFunction>(testPos.first->get_interface_id(),
            &BaseProject::AddNewFarrJenkinsPatersonTest));
        }
    //WHEELER_SMITH
    testPos = GetDefaultReadabilityTestsTemplate().find_test(ReadabilityMessages::WHEELER_SMITH());
    if (testPos.second)
        {
        m_standardTestFunctions.push_back(comparable_first_pair<int, AddTestFunction>(testPos.first->get_interface_id(),
            &BaseProject::AddWheelerSmithTest));
        }
    //AMSTAD
    testPos = GetDefaultReadabilityTestsTemplate().find_test(ReadabilityMessages::AMSTAD());
    if (testPos.second)
        {
        m_standardTestFunctions.push_back(comparable_first_pair<int, AddTestFunction>(testPos.first->get_interface_id(),
            &BaseProject::AddAmstadTest));
        }
    //WHEELER_SMITH_BAMBERGER_VANECEK
    testPos = GetDefaultReadabilityTestsTemplate().find_test(ReadabilityMessages::WHEELER_SMITH_BAMBERGER_VANECEK());
    if (testPos.second)
        {
        m_standardTestFunctions.push_back(comparable_first_pair<int, AddTestFunction>(testPos.first->get_interface_id(),
            &BaseProject::AddWheelerSmithBambergerVanecekTest));
        }
    //NEUE_WIENER_SACHTEXTFORMEL1
    testPos = GetDefaultReadabilityTestsTemplate().find_test(ReadabilityMessages::NEUE_WIENER_SACHTEXTFORMEL1());
    if (testPos.second)
        {
        m_standardTestFunctions.push_back(comparable_first_pair<int, AddTestFunction>(testPos.first->get_interface_id(),
            &BaseProject::AddNeueWienerSachtextformel1));
        }
    //NEUE_WIENER_SACHTEXTFORMEL2
    testPos = GetDefaultReadabilityTestsTemplate().find_test(ReadabilityMessages::NEUE_WIENER_SACHTEXTFORMEL2());
    if (testPos.second)
        {
        m_standardTestFunctions.push_back(comparable_first_pair<int, AddTestFunction>(testPos.first->get_interface_id(),
            &BaseProject::AddNeueWienerSachtextformel2));
        }
    //NEUE_WIENER_SACHTEXTFORMEL3
    testPos = GetDefaultReadabilityTestsTemplate().find_test(ReadabilityMessages::NEUE_WIENER_SACHTEXTFORMEL3());
    if (testPos.second)
        {
        m_standardTestFunctions.push_back(comparable_first_pair<int, AddTestFunction>(testPos.first->get_interface_id(),
            &BaseProject::AddNeueWienerSachtextformel3));
        }
    //LIX_GERMAN_CHILDRENS_LITERATURE
    testPos = GetDefaultReadabilityTestsTemplate().find_test(ReadabilityMessages::LIX_GERMAN_CHILDRENS_LITERATURE());
    if (testPos.second)
        {
        m_standardTestFunctions.push_back(comparable_first_pair<int, AddTestFunction>(testPos.first->get_interface_id(),
            &BaseProject::AddLixGermanChildrensLiterature));
        }
    //LIX_GERMAN_TECHNICAL
    testPos = GetDefaultReadabilityTestsTemplate().find_test(ReadabilityMessages::LIX_GERMAN_TECHNICAL());
    if (testPos.second)
        {
        m_standardTestFunctions.push_back(comparable_first_pair<int, AddTestFunction>(testPos.first->get_interface_id(),
            &BaseProject::AddLixGermanTechnical));
        }
    //RIX_GERMAN_FICTION
    testPos = GetDefaultReadabilityTestsTemplate().find_test(ReadabilityMessages::RIX_GERMAN_FICTION());
    if (testPos.second)
        {
        m_standardTestFunctions.push_back(comparable_first_pair<int, AddTestFunction>(testPos.first->get_interface_id(),
            &BaseProject::AddRixGermanFiction));
        }
    //RIX_GERMAN_NONFICTION
    testPos = GetDefaultReadabilityTestsTemplate().find_test(ReadabilityMessages::RIX_GERMAN_NONFICTION());
    if (testPos.second)
        {
        m_standardTestFunctions.push_back(comparable_first_pair<int, AddTestFunction>(testPos.first->get_interface_id(),
            &BaseProject::AddRixGermanNonFiction));
        }
    //ELF
    testPos = GetDefaultReadabilityTestsTemplate().find_test(ReadabilityMessages::ELF());
    if (testPos.second)
        {
        m_standardTestFunctions.push_back(comparable_first_pair<int, AddTestFunction>(testPos.first->get_interface_id(),
            &BaseProject::AddElfTest));
        }
    //DANIELSON_BRYAN_1
    testPos = GetDefaultReadabilityTestsTemplate().find_test(ReadabilityMessages::DANIELSON_BRYAN_1());
    if (testPos.second)
        {
        m_standardTestFunctions.push_back(comparable_first_pair<int, AddTestFunction>(testPos.first->get_interface_id(),
            &BaseProject::AddDanielsonBryan1Test));
        }
    //DANIELSON_BRYAN_2
    testPos = GetDefaultReadabilityTestsTemplate().find_test(ReadabilityMessages::DANIELSON_BRYAN_2());
    if (testPos.second)
        {
        m_standardTestFunctions.push_back(comparable_first_pair<int, AddTestFunction>(testPos.first->get_interface_id(),
            &BaseProject::AddDanielsonBryan2Test));
        }

    std::sort(m_standardTestFunctions.begin(), m_standardTestFunctions.end());

    m_readMessages.SetReadingAgeDisplay(wxGetApp().GetAppOptions().GetReadabilityMessageCatalog().GetReadingAgeDisplay());
    m_readMessages.SetGradeScale(wxGetApp().GetAppOptions().GetReadabilityMessageCatalog().GetGradeScale());
    m_readMessages.SetLongGradeScaleFormat(wxGetApp().GetAppOptions().GetReadabilityMessageCatalog().IsUsingLongGradeScaleFormat());
    }

void BaseProject::ResetStandardReadabilityTests(TestCollectionType& readabilityTests)
    {
    readabilityTests.clear();
    readabilityTests.reserve(m_defaultReadabilityTestsTemplate.get_test_count());
    readabilityTests.add_tests(m_defaultReadabilityTestsTemplate.get_tests());
    }

void BaseProject::InitializeStandardReadabilityTests()
    {
    wxASSERT_MSG(m_defaultReadabilityTestsTemplate.get_test_count() == 0, __WXFUNCTION__ + wxString(" called twice?"));
    m_defaultReadabilityTestsTemplate.clear();
    // degrees of reading power
        {
        readability::readability_test test(ReadabilityMessages::DEGREES_OF_READING_POWER(),
            XRCID("ID_DEGREES_OF_READING_POWER"),
            _DT(L"Degrees of Reading Power"), _DT(L"Degrees of Reading Power"),
            _(L"Degrees of Reading Power is designed for matching documents to a student's reading ability (based on his/her DRP score). This test is influenced by sentence length, word length, and number of familiar Dale-Chall words."),
            readability::readability_test_type::index_value, true,
            _DT(L"ROUND(100 - 100*(.886593 - .083640*(R/W) + .161911*POWER((D/W),3) -\n.021401*(W/S) + .000577*POWER((W/S), 2) - .000005*POWER((W/S), 3)))"));
        test.add_industry_classification(readability::industry_classification::adult_publishing_industry, true);
        test.add_teaching_level(readability::test_teaching_level::primary_grade);
        test.add_teaching_level(readability::test_teaching_level::secondary_grade);
        test.add_language(readability::test_language::english_test);
        test.add_factor(readability::test_factor::word_familiarity_dale_chall);
        test.add_factor(readability::test_factor::word_length);
        test.add_factor(readability::test_factor::sentence_length);
        m_defaultReadabilityTestsTemplate.add_test(test);
        }

    // degrees of reading power (grade equivalent)
        {
        readability::readability_test test(ReadabilityMessages::DEGREES_OF_READING_POWER_GE(),
            XRCID("ID_DEGREES_OF_READING_POWER_GE"),
            _DT(L"Degrees of Reading Power (GE)"), _DT(L"Degrees of Reading Power (grade equivalent)"),
            _(L"Degrees of Reading Power (GE) is designed for matching documents to a student's reading ability (based on his/her DRP score). This test is a conversion of a DRP (difficulty) score into a grade level."),
            readability::readability_test_type::grade_level, false,
            L"");
        test.add_industry_classification(readability::industry_classification::adult_publishing_industry, true);
        test.add_teaching_level(readability::test_teaching_level::primary_grade);
        test.add_teaching_level(readability::test_teaching_level::secondary_grade);
        test.add_language(readability::test_language::english_test);
        test.add_factor(readability::test_factor::word_familiarity_dale_chall);
        test.add_factor(readability::test_factor::word_length);
        test.add_factor(readability::test_factor::sentence_length);
        m_defaultReadabilityTestsTemplate.add_test(test);
        }

    // SOL (Spanish SMOG)
        {
        readability::readability_test test(ReadabilityMessages::SOL_SPANISH(),
            XRCID("ID_SOL_SPANISH"),
            _DT(L"SOL"), _DT(L"SOL (Spanish SMOG)"),
            _(L"SOL is meant for secondary-age (4th grade to college level) Spanish reading materials. It is a modified version of SMOG that was recalibrated for Spanish text."),
            readability::readability_test_type::grade_level, false,
            _DT(L"(1.0430*SQRT(C*(30/S)) + 3.1291)*.74 - 2.51"));
        test.add_document_classification(readability::document_classification::general_document, true);
        test.add_document_classification(readability::document_classification::adult_literature_document, true);
        test.add_industry_classification(readability::industry_classification::childrens_publishing_industry, true);
        test.add_industry_classification(readability::industry_classification::adult_publishing_industry, true);
        test.add_industry_classification(readability::industry_classification::childrens_healthcare_industry, true);
        test.add_industry_classification(readability::industry_classification::adult_healthcare_industry, true);
        test.add_teaching_level(readability::test_teaching_level::secondary_grade);
        test.add_teaching_level(readability::test_teaching_level::adult_level);
        test.add_language(readability::test_language::spanish_test);
        test.add_factor(readability::test_factor::word_complexity_3_plus_syllables);
        test.add_factor(readability::test_factor::sentence_length);
        m_defaultReadabilityTestsTemplate.add_test(test);
        }

    // GPM (Spanish) fry graph
        {
        readability::readability_test test(ReadabilityMessages::GPM_FRY(),
            XRCID("ID_GPM_FRY"),
            _DT(L"Gilliam-Pe\U000000F1a-Mountain"), _DT(L"Gilliam-Pe\U000000F1a-Mountain Graph"),
            _(L"The <a href=\"#GPMFryGraph\">Gilliam-Pe\U000000F1a-Mountain graph</a> is designed for most text, including literature and technical documents."),
            readability::readability_test_type::grade_level, true,
            L"");
        test.add_document_classification(readability::document_classification::general_document, true);
        test.add_document_classification(readability::document_classification::adult_literature_document, true);
        test.add_document_classification(readability::document_classification::childrens_literature_document, true);
        test.add_industry_classification(readability::industry_classification::childrens_publishing_industry, true);
        test.add_industry_classification(readability::industry_classification::adult_publishing_industry, true);
        test.add_industry_classification(readability::industry_classification::childrens_healthcare_industry, true);
        test.add_industry_classification(readability::industry_classification::adult_healthcare_industry, true);
        test.add_teaching_level(readability::test_teaching_level::primary_grade);
        test.add_teaching_level(readability::test_teaching_level::secondary_grade);
        test.add_language(readability::test_language::spanish_test);
        test.add_factor(readability::test_factor::word_complexity);
        test.add_factor(readability::test_factor::sentence_length);
        m_defaultReadabilityTestsTemplate.add_test(test);
        }

    // Frase (Spanish graph)
        {
        readability::readability_test test(ReadabilityMessages::FRASE(),
            XRCID("ID_FRASE"),
            _DT(L"FRASE"), _DT(L"FRASE Graph"),
            _(L"The <a href=\"#FRASE\">FRASE graph</a> is designed for educational materials (primarily Spanish as a secondary language)."),
            readability::readability_test_type::index_value, true,
            L"");
        test.add_document_classification(readability::document_classification::childrens_literature_document, true);
        test.add_industry_classification(readability::industry_classification::childrens_publishing_industry, true);
        test.add_industry_classification(readability::industry_classification::sedondary_language_industry, true);
        test.add_teaching_level(readability::test_teaching_level::primary_grade);
        test.add_teaching_level(readability::test_teaching_level::secondary_grade);
        test.add_teaching_level(readability::test_teaching_level::second_language);
        test.add_language(readability::test_language::spanish_test);
        test.add_factor(readability::test_factor::word_complexity);
        test.add_factor(readability::test_factor::sentence_length);
        m_defaultReadabilityTestsTemplate.add_test(test);
        }

    // Crawford
        {
        readability::readability_test test(ReadabilityMessages::CRAWFORD(),
            XRCID("ID_CRAWFORD"),
            _DT(L"Crawford"), _DT(L"Crawford"),
            _(L"The Crawford test is designed for primary-age Spanish reading materials. A <a href=\"#CrawfordGraph\">chart</a> is also available to visualize the score."),
            readability::readability_test_type::grade_level, false,
            _DT(L"(S*-.205) + (B*.049) - 3.407"));
        test.add_document_classification(readability::document_classification::childrens_literature_document, true);
        test.add_industry_classification(readability::industry_classification::childrens_publishing_industry, true);
        test.add_industry_classification(readability::industry_classification::childrens_healthcare_industry, true);
        test.add_teaching_level(readability::test_teaching_level::primary_grade);
        test.add_language(readability::test_language::spanish_test);
        test.add_factor(readability::test_factor::word_complexity);
        test.add_factor(readability::test_factor::sentence_length);
        m_defaultReadabilityTestsTemplate.add_test(test);
        }

    // Bormuth grade placement 35
        {
        readability::readability_test test(ReadabilityMessages::BORMUTH_GRADE_PLACEMENT_35(),
            XRCID("ID_BORMUTH_GRADE_PLACEMENT_35"),
            _DT(L"Bormuth Grade Placement"), _DT(L"Bormuth Grade Placement"),
            _(L"Bormuth Grade Placement is designed for students and is highly regarded for its accuracy because it uses three variables (rather than the traditional two variables)."),
            readability::readability_test_type::grade_level, false,
            _DT(L"3.761864 + 1.053153*(R/W) - 2.138595*POWER((D/W),3) +\n.152832*(W/S) - .002077*POWER((W/S), 2)"));
        test.add_document_classification(readability::document_classification::adult_literature_document, true);
        test.add_teaching_level(readability::test_teaching_level::secondary_grade);
        test.add_language(readability::test_language::english_test);
        test.add_factor(readability::test_factor::word_familiarity_dale_chall);
        test.add_factor(readability::test_factor::word_length);
        test.add_factor(readability::test_factor::sentence_length);
        m_defaultReadabilityTestsTemplate.add_test(test);
        }

    // Bormuth cloze mean
        {
        readability::readability_test test(ReadabilityMessages::BORMUTH_CLOZE_MEAN(),
            XRCID("ID_BORMUTH_CLOZE_MEAN"),
            _DT(L"Bormuth Cloze Mean"), _DT(L"Bormuth Cloze Mean"),
            _(L"Bormuth Cloze Mean is designed for students and is highly regarded for its accuracy because it uses three variables (rather than the traditional two variables)."),
            readability::readability_test_type::predicted_cloze_score, true,
            _DT(L"(.886593 - .083640*(R/W) + .161911*POWER((D/W),3) -\n.021401*(W/S) + .000577*POWER((W/S), 2) - .000005*POWER((W/S), 3))*100"));
        test.add_document_classification(readability::document_classification::adult_literature_document, true);
        test.add_teaching_level(readability::test_teaching_level::secondary_grade);
        test.add_language(readability::test_language::english_test);
        test.add_factor(readability::test_factor::word_familiarity_dale_chall);
        test.add_factor(readability::test_factor::word_length);
        test.add_factor(readability::test_factor::sentence_length);
        m_defaultReadabilityTestsTemplate.add_test(test);
        }

    // Schwartz (German graph)
        {
        readability::readability_test test(ReadabilityMessages::SCHWARTZ(),
            XRCID("ID_SCHWARTZ"),
            _DT(L"Schwartz"), _DT(L"Schwartz German Readability Graph"),
            _(L"The <a href=\"#Schwartz\">Schwartz German Readability Graph</a> is designed for classroom instructional materials."),
            readability::readability_test_type::grade_level, true,
            L"");
        test.add_document_classification(readability::document_classification::childrens_literature_document, true);
        test.add_industry_classification(readability::industry_classification::childrens_publishing_industry, true);
        test.add_teaching_level(readability::test_teaching_level::primary_grade);
        test.add_teaching_level(readability::test_teaching_level::secondary_grade);
        test.add_language(readability::test_language::german_test);
        test.add_factor(readability::test_factor::word_complexity);
        test.add_factor(readability::test_factor::sentence_length);
        m_defaultReadabilityTestsTemplate.add_test(test);
        }

    // Neue wiener sachtextformel 1
        {
        readability::readability_test test(ReadabilityMessages::NEUE_WIENER_SACHTEXTFORMEL1(),
            XRCID("ID_NEUE_WIENER_SACHTEXTFORMEL1"),
            _DT(L"1.nWS"), _DT(L"Neue Wiener Sachtextformel (1)"),
            _(L"This test is meant for secondary-age non-fiction (specifically, 6-10 grade materials)."),
            readability::readability_test_type::grade_level, false,
            _DT(L"0.1935*((C/W)*100) + 0.1672*(W/S) + 0.1297*((X/W)*100) - 0.0327*((M/W)*100) - 0.875"));
        test.add_document_classification(readability::document_classification::childrens_literature_document, true);
        test.add_industry_classification(readability::industry_classification::childrens_publishing_industry, true);
        test.add_teaching_level(readability::test_teaching_level::secondary_grade);
        test.add_language(readability::test_language::german_test);
        test.add_factor(readability::test_factor::word_complexity_3_plus_syllables);
        test.add_factor(readability::test_factor::word_length_7_plus);
        test.add_factor(readability::test_factor::sentence_length);
        m_defaultReadabilityTestsTemplate.add_test(test);
        }

    // neue wiener sachtextformel 2
        {
        readability::readability_test test(ReadabilityMessages::NEUE_WIENER_SACHTEXTFORMEL2(),
            XRCID("ID_NEUE_WIENER_SACHTEXTFORMEL2"),
            _DT(L"2.nWS"), _DT(L"Neue Wiener Sachtextformel (2)"),
            _(L"This test is meant for primary-age non-fiction (specifically, light materials up to the 5th grade)."),
            readability::readability_test_type::grade_level, false,
            _DT(L"0.2007*((C/W)*100) + 0.1682*(W/S) + 0.1373*((X/W)*100) - 2.779"));
        test.add_document_classification(readability::document_classification::childrens_literature_document, true);
        test.add_industry_classification(readability::industry_classification::childrens_publishing_industry, true);
        test.add_teaching_level(readability::test_teaching_level::primary_grade);
        test.add_language(readability::test_language::german_test);
        test.add_factor(readability::test_factor::word_complexity_3_plus_syllables);
        test.add_factor(readability::test_factor::word_length_7_plus);
        test.add_factor(readability::test_factor::sentence_length);
        m_defaultReadabilityTestsTemplate.add_test(test);
        }

    // neue wiener sachtextformel 3
        {
        readability::readability_test test(ReadabilityMessages::NEUE_WIENER_SACHTEXTFORMEL3(),
            XRCID("ID_NEUE_WIENER_SACHTEXTFORMEL3"),
            _DT(L"3.nWS"), _DT(L"Neue Wiener Sachtextformel (3)"),
            _(L"This test is meant for primary-age non-fiction (specifically, light materials up to the 5th grade)."),
            readability::readability_test_type::grade_level, false,
            _DT(L"0.2963*((C/W)*100) + 0.1905*(W/S) - 1.1144"));
        test.add_document_classification(readability::document_classification::childrens_literature_document, true);
        test.add_industry_classification(readability::industry_classification::childrens_publishing_industry, true);
        test.add_teaching_level(readability::test_teaching_level::primary_grade);
        test.add_language(readability::test_language::german_test);
        test.add_factor(readability::test_factor::word_complexity_3_plus_syllables);
        test.add_factor(readability::test_factor::sentence_length);
        m_defaultReadabilityTestsTemplate.add_test(test);
        }

    // wheeler-smith (BV)
        {
        readability::readability_test test(ReadabilityMessages::WHEELER_SMITH_BAMBERGER_VANECEK(),
            XRCID("ID_WHEELER_SMITH_BAMBERGER_VANECEK"),
            _DT(L"Wheeler-Smith (Bamberger-Vanecek)"), _DT(L"Wheeler-Smith (Bamberger-Vanecek)"),
            _(L"German variation of Wheeler-Smith, which is meant for primary-age reading materials."),
            readability::readability_test_type::grade_level, false,
            L"");
        test.add_document_classification(readability::document_classification::childrens_literature_document, true);
        test.add_industry_classification(readability::industry_classification::childrens_publishing_industry, true);
        test.add_industry_classification(readability::industry_classification::childrens_healthcare_industry, true);
        test.add_teaching_level(readability::test_teaching_level::primary_grade);
        test.add_language(readability::test_language::german_test);
        test.add_factor(readability::test_factor::word_complexity_3_plus_syllables);
        test.add_factor(readability::test_factor::sentence_length);
        m_defaultReadabilityTestsTemplate.add_test(test);
        }

    // wheeler-smith
        {
        readability::readability_test test(ReadabilityMessages::WHEELER_SMITH(),
            XRCID("ID_WHEELER_SMITH"),
            _DT(L"Wheeler-Smith"), _DT(L"Wheeler-Smith"),
            _(L"Wheeler-Smith is meant for primary-age (Kindergarten to 4th grade) reading materials."),
            readability::readability_test_type::grade_level, false,
            L"");
        test.add_document_classification(readability::document_classification::childrens_literature_document, true);
        test.add_industry_classification(readability::industry_classification::childrens_publishing_industry, true);
        test.add_industry_classification(readability::industry_classification::childrens_healthcare_industry, true);
        test.add_teaching_level(readability::test_teaching_level::primary_grade);
        test.add_language(readability::test_language::english_test);
        test.add_factor(readability::test_factor::word_complexity_2_plus_syllables);
        test.add_factor(readability::test_factor::sentence_length);
        m_defaultReadabilityTestsTemplate.add_test(test);
        }

    // Harris-Jacobson
        {
        readability::readability_test test(ReadabilityMessages::HARRIS_JACOBSON(),
            XRCID("ID_HARRIS_JACOBSON"),
            _DT(L"Harris-Jacobson"), _DT(L"Harris-Jacobson Wide Range Formula"),
            _(L"Harris-Jacobson is generally used for primary and secondary age (Kindergarten to 11th grade) readers."),
            readability::readability_test_type::grade_level, false,
            L"");
        test.add_document_classification(readability::document_classification::childrens_literature_document, true);
        test.add_industry_classification(readability::industry_classification::childrens_publishing_industry, true);
        test.add_document_classification(readability::document_classification::adult_literature_document, true);
        test.add_industry_classification(readability::industry_classification::childrens_healthcare_industry, true);
        test.add_teaching_level(readability::test_teaching_level::primary_grade);
        test.add_teaching_level(readability::test_teaching_level::secondary_grade);
        test.add_language(readability::test_language::english_test);
        test.add_factor(readability::test_factor::word_familiarity_harris_jacobson);
        test.add_factor(readability::test_factor::sentence_length);
        m_defaultReadabilityTestsTemplate.add_test(test);
        }

    // spache
        {
        readability::readability_test test(ReadabilityMessages::SPACHE(),
            XRCID("ID_SPACHE"),
            _DT(L"Spache Revised"), _DT(L"Spache Revised"),
            _(L"Spache is generally used for primary age (Kindergarten to 7th grade) readers to help classify school textbooks and literature."),
            readability::readability_test_type::grade_level, false,
            _DT(L"(.121 * (W/S)) + (.082 * UUS*(100/W)) + .659"));
        test.add_document_classification(readability::document_classification::childrens_literature_document, true);
        test.add_industry_classification(readability::industry_classification::childrens_publishing_industry, true);
        test.add_teaching_level(readability::test_teaching_level::primary_grade);
        test.add_language(readability::test_language::english_test);
        test.add_factor(readability::test_factor::word_familiarity_spache);
        test.add_factor(readability::test_factor::sentence_length);
        m_defaultReadabilityTestsTemplate.add_test(test);
        }

    // ari
        {
        readability::readability_test test(ReadabilityMessages::ARI(),
            XRCID("ID_ARI"),
            _DT(L"ARI"), _DT(L"Automated Readability Index"),
            _(L"Automated Readability Index, also known as \"ARI\" or \"auto\", was originally created for U.S. Air Force materials and was designed for technical documents and manuals."),
            readability::readability_test_type::grade_level, false,
            _DT(L"(4.71 * (RP/W)) + (0.5 * (W/S)) - 21.43"));
        test.add_document_classification(readability::document_classification::technical_document, true);
        test.add_industry_classification(readability::industry_classification::military_government_industry, true); //Air Force
        test.add_teaching_level(readability::test_teaching_level::adult_level);
        test.add_language(readability::test_language::english_test);
        test.add_factor(readability::test_factor::word_length);
        test.add_factor(readability::test_factor::sentence_length);
        m_defaultReadabilityTestsTemplate.add_test(test);
        }

    // ELF
        {
        readability::readability_test test(ReadabilityMessages::ELF(),
            XRCID("ID_ELF"),
            _DT(L"ELF"), _DT(L"Easy Listening Formula"),
            _(L"ELF is designed for \"listenability\" and is meant for radio and television broadcasts."),
            readability::readability_test_type::grade_level, false,
            _DT(L"(B-W) / S"));
        test.add_industry_classification(readability::industry_classification::broadcasting_industry, true);
        test.add_teaching_level(readability::test_teaching_level::adult_level);
        test.add_language(readability::test_language::english_test);
        test.add_factor(readability::test_factor::word_complexity_density);
        m_defaultReadabilityTestsTemplate.add_test(test);
        }

    // fog
        {
        readability::readability_test test(ReadabilityMessages::GUNNING_FOG(),
            XRCID("ID_FOG"),
            _DT(L"Gunning Fog"), _DT(L"Gunning Fog"),
            _(L"Gunning Fog Index is generally recommended for business publications and journals."),
            readability::readability_test_type::grade_level, false,
            _DT(L".4*(WordCount()/SentenceCount(GunningFog)+((HardFogWordCount()/WordCount())*100))"));
        test.add_document_classification(readability::document_classification::technical_document, true);
        test.add_document_classification(readability::document_classification::general_document, true);
        test.add_document_classification(readability::document_classification::adult_literature_document, true);
        test.add_industry_classification(readability::industry_classification::childrens_publishing_industry, true);
        test.add_industry_classification(readability::industry_classification::adult_publishing_industry, true);
        test.add_industry_classification(readability::industry_classification::childrens_healthcare_industry, true);
        test.add_industry_classification(readability::industry_classification::adult_healthcare_industry, true);
        test.add_teaching_level(readability::test_teaching_level::secondary_grade);
        test.add_teaching_level(readability::test_teaching_level::adult_level);
        test.add_language(readability::test_language::english_test);
        test.add_factor(readability::test_factor::word_complexity_3_plus_syllables);
        test.add_factor(readability::test_factor::sentence_length);
        m_defaultReadabilityTestsTemplate.add_test(test);
        }

    // psk fog
        {
        readability::readability_test test(ReadabilityMessages::PSK_GUNNING_FOG(),
            XRCID("ID_PSK_FOG"),
            _DT(L"PSK Gunning Fog"), _DT(L"Powers-Sumner-Kearl (Gunning Fog)"),
            _(L"PSK Gunning Fog Index is generally recommended for business publications and journals."),
            readability::readability_test_type::grade_level, false,
            _DT(L"3.0680 + (.0877*(WordCount(Default)/SentenceCount(GunningFog))) +\n(.0984*((HardFogWordCount()/WordCount(Default))*100))"));
        test.add_teaching_level(readability::test_teaching_level::secondary_grade);
        test.add_language(readability::test_language::english_test);
        test.add_factor(readability::test_factor::word_complexity_3_plus_syllables);
        test.add_factor(readability::test_factor::sentence_length);
        m_defaultReadabilityTestsTemplate.add_test(test);
        }

    // lix
        {
        readability::readability_test test(ReadabilityMessages::LIX(),
            XRCID("ID_LIX"),
            _DT(L"Lix"), _DT(L"L\U000000E4sbarhetsindex (Lix)"),
            _(L"The L\U000000E4sbarhetsindex (Lix) formula can be used on documents of any Western European language. A <a href=\"#LixGauge\">chart</a> is also available to visualize the score."),
            readability::readability_test_type::index_value_and_grade_level, true,
            L"");
        test.add_document_classification(readability::document_classification::adult_literature_document, true);
        test.add_industry_classification(readability::industry_classification::childrens_publishing_industry, true);
        test.add_industry_classification(readability::industry_classification::adult_publishing_industry, true);
        test.add_teaching_level(readability::test_teaching_level::secondary_grade);
        test.add_teaching_level(readability::test_teaching_level::adult_level);
        test.add_language(readability::test_language::english_test);
        test.add_language(readability::test_language::french_test);
        test.add_language(readability::test_language::spanish_test);
        test.add_language(readability::test_language::german_test);
        test.add_factor(readability::test_factor::word_length_7_plus);
        test.add_factor(readability::test_factor::sentence_length);
        m_defaultReadabilityTestsTemplate.add_test(test);
        }

    // rix (German fiction)
        {
        readability::readability_test test(ReadabilityMessages::RIX_GERMAN_FICTION(),
            XRCID("ID_RIX_GERMAN_FICTION"),
            _(L"Rix (German fiction)"), _(L"Rix (German fiction)"),
            _(L"This test is a variation of Rix meant for German fiction (specifically, grades 1-11)."),
            readability::readability_test_type::index_value_and_grade_level, true,
            // needs a table to convert formula to grade level, so leave this blank
            L"");
        test.add_document_classification(readability::document_classification::childrens_literature_document, true);
        test.add_document_classification(readability::document_classification::adult_literature_document, true);
        test.add_industry_classification(readability::industry_classification::childrens_publishing_industry, true);
        test.add_industry_classification(readability::industry_classification::adult_publishing_industry, true);
        test.add_teaching_level(readability::test_teaching_level::primary_grade);
        test.add_teaching_level(readability::test_teaching_level::secondary_grade);
        test.add_language(readability::test_language::german_test);
        test.add_factor(readability::test_factor::word_length_7_plus);
        test.add_factor(readability::test_factor::sentence_length);
        m_defaultReadabilityTestsTemplate.add_test(test);
        }

    // rix (German non-fiction)
        {
        readability::readability_test test(ReadabilityMessages::RIX_GERMAN_NONFICTION(),
            XRCID("ID_RIX_GERMAN_NONFICTION"),
            _(L"Rix (German non-fiction)"), _(L"Rix (German non-fiction)"),
            _(L"This test is a variation of Rix meant for German non-fiction (specifically, secondary-age to adult level materials)."),
            readability::readability_test_type::index_value_and_grade_level, true,
            L"");
        test.add_document_classification(readability::document_classification::adult_literature_document, true);
        test.add_industry_classification(readability::industry_classification::adult_publishing_industry, true);
        test.add_teaching_level(readability::test_teaching_level::secondary_grade);
        test.add_teaching_level(readability::test_teaching_level::adult_level);
        test.add_language(readability::test_language::german_test);
        test.add_factor(readability::test_factor::word_length_7_plus);
        test.add_factor(readability::test_factor::sentence_length);
        m_defaultReadabilityTestsTemplate.add_test(test);
        }

    // lix (German children's literature)
        {
        readability::readability_test test(ReadabilityMessages::LIX_GERMAN_CHILDRENS_LITERATURE(),
            XRCID("ID_LIX_GERMAN_CHILDRENS_LITERATURE"),
            _(L"Lix (German children's literature)"), _(L"Lix (German children's literature)"),
            _(L"This test is a variation of Lix meant for German children's literature (specifically, grades 1-8). A <a href=\"#GermanLixGauge\">chart</a> is also available to visualize the score."),
            readability::readability_test_type::index_value_and_grade_level, true,
            L"");
        test.add_document_classification(readability::document_classification::childrens_literature_document, true);
        test.add_industry_classification(readability::industry_classification::childrens_publishing_industry, true);
        test.add_teaching_level(readability::test_teaching_level::primary_grade);
        test.add_teaching_level(readability::test_teaching_level::secondary_grade);
        test.add_language(readability::test_language::german_test);
        test.add_factor(readability::test_factor::word_length_7_plus);
        test.add_factor(readability::test_factor::sentence_length);
        m_defaultReadabilityTestsTemplate.add_test(test);
        }

    // lix (German technical books)
        {
        readability::readability_test test(ReadabilityMessages::LIX_GERMAN_TECHNICAL(),
            XRCID("ID_LIX_GERMAN_TECHNICAL"),
            _(L"Lix (German technical literature)"), _(L"Lix (German technical literature)"),
            _(L"This test is a variation of Lix meant for German technical/non-fiction literature. A <a href=\"#GermanLixGauge\">chart</a> is also available to visualize the score."),
            readability::readability_test_type::index_value_and_grade_level, true,
            L"");
        test.add_document_classification(readability::document_classification::technical_document, true);
        test.add_document_classification(readability::document_classification::adult_literature_document, true);
        test.add_industry_classification(readability::industry_classification::adult_publishing_industry, true);
        test.add_teaching_level(readability::test_teaching_level::secondary_grade);
        test.add_teaching_level(readability::test_teaching_level::adult_level);
        test.add_language(readability::test_language::german_test);
        test.add_factor(readability::test_factor::word_length_7_plus);
        test.add_factor(readability::test_factor::sentence_length);
        m_defaultReadabilityTestsTemplate.add_test(test);
        }

    // rix
        {
        readability::readability_test test(ReadabilityMessages::RIX(),
            XRCID("ID_RIX"),
            _DT(L"Rix"), _DT(L"Rate Index (Rix)"),
            _(L"The Rate Index (Rix) formula is a variation of Lix and can be used on documents of any Western European language."),
            readability::readability_test_type::index_value_and_grade_level, false,
            L"");
        test.add_document_classification(readability::document_classification::adult_literature_document, true);
        test.add_industry_classification(readability::industry_classification::childrens_publishing_industry, true);
        test.add_industry_classification(readability::industry_classification::adult_publishing_industry, true);
        test.add_teaching_level(readability::test_teaching_level::secondary_grade);
        test.add_teaching_level(readability::test_teaching_level::adult_level);
        test.add_language(readability::test_language::english_test);
        test.add_language(readability::test_language::french_test);
        test.add_language(readability::test_language::spanish_test);
        test.add_language(readability::test_language::german_test);
        test.add_factor(readability::test_factor::word_length_7_plus);
        test.add_factor(readability::test_factor::sentence_length);
        m_defaultReadabilityTestsTemplate.add_test(test);
        }

    // coleman-liau
        {
        readability::readability_test test(ReadabilityMessages::COLEMAN_LIAU(),
            XRCID("ID_COLEMAN_LIAU"),
            _DT(L"Coleman-Liau"), _DT(L"Coleman-Liau"),
            _(L"Coleman-Liau is meant for secondary age (4th grade to college level) readers. This formula is based on text from the .4 to 16.3 grade level range. This test usually yields the lowest grade when applied to technical documents."),
            readability::readability_test_type::grade_level_and_predicted_cloze_score, false,
            // just returns the grade level, not the predicted Cloze score
            _DT(L"(-27.4004*(((141.8401 - (.214590*R*(100/W))) + (1.079812*S*(100/W)))/100)) + 23.06395"));
        test.add_document_classification(readability::document_classification::adult_literature_document, true);
        test.add_industry_classification(readability::industry_classification::childrens_publishing_industry, true);
        test.add_industry_classification(readability::industry_classification::adult_publishing_industry, true);
        test.add_teaching_level(readability::test_teaching_level::secondary_grade);
        test.add_language(readability::test_language::english_test);
        test.add_factor(readability::test_factor::word_length);
        test.add_factor(readability::test_factor::sentence_length);
        m_defaultReadabilityTestsTemplate.add_test(test);
        }

    // new dale-chall
        {
        readability::readability_test test(ReadabilityMessages::DALE_CHALL(),
            XRCID("ID_NEW_DALE_CHALL"),
            _DT(L"New Dale-Chall"), _DT(L"New Dale-Chall"),
            _(L"New Dale-Chall is generally used for primary and secondary age readers to help classify school text books and literature."),
            readability::readability_test_type::grade_level, false,
            L"");
        test.add_document_classification(readability::document_classification::adult_literature_document, true);
        test.add_industry_classification(readability::industry_classification::childrens_publishing_industry, true);
        test.add_industry_classification(readability::industry_classification::adult_publishing_industry, true);
        test.add_industry_classification(readability::industry_classification::childrens_healthcare_industry, true);
        test.add_teaching_level(readability::test_teaching_level::secondary_grade);
        test.add_language(readability::test_language::english_test);
        test.add_factor(readability::test_factor::word_familiarity_dale_chall);
        test.add_factor(readability::test_factor::sentence_length);
        m_defaultReadabilityTestsTemplate.add_test(test);
        }

    // danielson-bryan 1
        {
        readability::readability_test test(ReadabilityMessages::DANIELSON_BRYAN_1(),
            XRCID("ID_DANIELSON_BRYAN_1"),
            _DT(L"Danielson-Bryan 1"), _DT(L"Danielson-Bryan 1"),
            _(L"Danielson-Bryan 1 is designed for student materials."),
            readability::readability_test_type::grade_level, false,
            _DT(L"1.0364*(RP/W) + .0194*(RP/S) - .6059"));
        test.add_teaching_level(readability::test_teaching_level::secondary_grade);
        test.add_language(readability::test_language::english_test);
        test.add_factor(readability::test_factor::word_length);
        test.add_factor(readability::test_factor::sentence_length);
        m_defaultReadabilityTestsTemplate.add_test(test);
        }

    // danielson-bryan 2
        {
        readability::readability_test test(ReadabilityMessages::DANIELSON_BRYAN_2(),
            XRCID("ID_DANIELSON_BRYAN_2"),
            _DT(L"Danielson-Bryan 2"), _DT(L"Danielson-Bryan 2"),
            _(L"Danielson-Bryan 2 is designed for student materials. It is a variation of Flesch Reading Ease that uses character counts instead of syllable counts. A <a href=\"#DB2\">chart</a> is also available to visualize the score."),
            readability::readability_test_type::index_value, true,
            _DT(L"ROUND(131.059 - 10.364*(RP/W) - .194*(RP/S))"));
        test.add_teaching_level(readability::test_teaching_level::secondary_grade);
        test.add_language(readability::test_language::english_test);
        test.add_factor(readability::test_factor::word_length);
        test.add_factor(readability::test_factor::sentence_length);
        m_defaultReadabilityTestsTemplate.add_test(test);
        }

    // PSK (new dale-chall)
        {
        readability::readability_test test(ReadabilityMessages::PSK_DALE_CHALL(),
            XRCID("ID_PSK_DALE_CHALL"),
            _DT(L"PSK Dale-Chall"), _DT(L"Powers-Sumner-Kearl (Dale-Chall)"),
            _(L"PSK Dale-Chall is generally used for primary and secondary age readers to help classify school text books and literature."),
            readability::readability_test_type::grade_level, false,
            _DT(L"3.2672 + (.0596*(W/S)) + (.1155*((UDC/W)*100))"));
        //don't associate this test with anything, users should just be offered New Dale-Chall from the project wizards instead
        test.add_teaching_level(readability::test_teaching_level::secondary_grade);
        test.add_language(readability::test_language::english_test);
        test.add_factor(readability::test_factor::word_familiarity_dale_chall);
        test.add_factor(readability::test_factor::sentence_length);
        m_defaultReadabilityTestsTemplate.add_test(test);
        }

    // forcast
        {
        readability::readability_test test(ReadabilityMessages::FORCAST(),
            XRCID("ID_FORCAST"),
            _DT(L"FORCAST"), _DT(L"FORCAST"),
            _(L"FORCAST was devised for assessing U.S. Army technical manuals and forms. It is the only test not designed for running narrative, so it is mostly appropriate for multiple-choice quizzes, applications, entrance forms, etc."),
            readability::readability_test_type::grade_level, false,
            _DT(L"20-(M/10)"));
        test.add_document_classification(readability::document_classification::nonnarrative_document, true);
        test.add_industry_classification(readability::industry_classification::childrens_healthcare_industry, true);
        test.add_industry_classification(readability::industry_classification::adult_healthcare_industry, true);
        test.add_industry_classification(readability::industry_classification::military_government_industry, true);//Army
        test.add_teaching_level(readability::test_teaching_level::adult_level);
        test.add_language(readability::test_language::english_test);
        test.add_factor(readability::test_factor::word_complexity_2_plus_syllables);
        m_defaultReadabilityTestsTemplate.add_test(test);
        }

    // Farr-Jenkins-Paterson
        {
        readability::readability_test test(ReadabilityMessages::FARR_JENKINS_PATERSON(),
            XRCID("ID_FARR_JENKINS_PATERSON"),
            _DT(L"Farr-Jenkins-Paterson"), _DT(L"Farr-Jenkins-Paterson"),
            _(L"Farr-Jenkins-Paterson is a variation of the Flesch Reading Ease test, which uses the mono-syllabic word count instead of the overall syllable count. Scores range from 0-100 (the higher the score, the easier to read). Average documents should be within the range of 60-70."),
            readability::readability_test_type::index_value, true,
            _DT(L"ROUND(-31.517 - (1.015*(W/S)) + (1.599*((M/W)*100)))"));
        test.add_teaching_level(readability::test_teaching_level::secondary_grade);
        test.add_language(readability::test_language::english_test);
        test.add_factor(readability::test_factor::word_complexity_2_plus_syllables);
        test.add_factor(readability::test_factor::sentence_length);
        m_defaultReadabilityTestsTemplate.add_test(test);
        }

    // New Farr-Jenkins-Paterson (Kincaid)
        {
        readability::readability_test test(ReadabilityMessages::NEW_FARR_JENKINS_PATERSON(),
            XRCID("ID_NEW_FARR_JENKINS_PATERSON"),
            _DT(L"New Farr-Jenkins-Paterson"), _DT(L"New Farr-Jenkins-Paterson (Kincaid)"),
            _(L"A modified version of Farr-Jenkins-Paterson designed for U.S. Navy technical manuals and forms."),
            readability::readability_test_type::grade_level, true,
            _DT(L"22.05 + (.387*(W/S)) - (.307*((M/W)*100))"));
        test.add_teaching_level(readability::test_teaching_level::adult_level);
        test.add_language(readability::test_language::english_test);
        test.add_factor(readability::test_factor::word_complexity_2_plus_syllables);
        test.add_factor(readability::test_factor::sentence_length);
        m_defaultReadabilityTestsTemplate.add_test(test);
        }

    // Farr-Jenkins-Paterson (PSK)
        {
        readability::readability_test test(ReadabilityMessages::PSK_FARR_JENKINS_PATERSON(),
            XRCID("ID_PSK_FARR_JENKINS_PATERSON"),
            _DT(L"PSK Farr-Jenkins-Paterson"), _DT(L"Powers-Sumner-Kearl (Farr-Jenkins-Paterson)"),
            _(L"PSK Farr-Jenkins-Paterson is a variation of the Farr-Jenkins-Paterson test, which returns a grade score instead of a Flesch difficulty level."),
            readability::readability_test_type::grade_level, false,
            _DT(L"8.4335 + (.0923*(W/S)) - (.0648*((M/W)*100))"));
        test.add_teaching_level(readability::test_teaching_level::secondary_grade);
        test.add_language(readability::test_language::english_test);
        test.add_factor(readability::test_factor::word_complexity_2_plus_syllables);
        test.add_factor(readability::test_factor::sentence_length);
        m_defaultReadabilityTestsTemplate.add_test(test);
        }

    // Flesch Reading Ease
        {
        readability::readability_test test(ReadabilityMessages::FLESCH(),
            XRCID("ID_FLESCH"),
            _DT(L"Flesch Reading Ease"), _DT(L"Flesch Reading Ease"),
            _(L"Flesch Reading Ease is meant for secondary and adult-level text and is a standard used by many U.S. government agencies, including the U.S. Department of Defense. Scores range from 0-100 (the higher the score, the easier to read). Average documents should be within the range of 60-70. A <a href=\"#FleschChart\">chart</a> is also available to visualize the score."),
            readability::readability_test_type::index_value, true,
            _DT(L"ROUND(206.835 - (84.6*(B/W)) - (1.015*(W/S)))"));
        test.add_document_classification(readability::document_classification::general_document, true);
        test.add_document_classification(readability::document_classification::technical_document, true);
        test.add_document_classification(readability::document_classification::adult_literature_document, true);
        test.add_industry_classification(readability::industry_classification::childrens_publishing_industry, true);
        test.add_industry_classification(readability::industry_classification::adult_publishing_industry, true);
        test.add_industry_classification(readability::industry_classification::childrens_healthcare_industry, true);
        test.add_industry_classification(readability::industry_classification::adult_healthcare_industry, true);
        test.add_industry_classification(readability::industry_classification::military_government_industry, true);//used by U.S. DoD
        test.add_teaching_level(readability::test_teaching_level::secondary_grade);
        test.add_teaching_level(readability::test_teaching_level::adult_level);
        test.add_language(readability::test_language::english_test);
        test.add_factor(readability::test_factor::word_complexity);
        test.add_factor(readability::test_factor::sentence_length);
        m_defaultReadabilityTestsTemplate.add_test(test);
        }

    // Amstad
        {
        readability::readability_test test(ReadabilityMessages::AMSTAD(),
            XRCID("ID_AMSTAD"),
            _DT(L"Amstad"), _DT(L"Amstad"),
            _(L"Amstad is a recalculation of Flesch Reading Ease for German text. It is meant for secondary and adult-level text. Scores range from 0-100 (the higher the score, the easier to read). Average documents should be within the range of 60-70."),
            readability::readability_test_type::index_value, true,
            _DT(L"180 - (W/S) - (58.5*(B/W))"));
        test.add_document_classification(readability::document_classification::general_document, true);
        test.add_document_classification(readability::document_classification::technical_document, true);
        test.add_document_classification(readability::document_classification::adult_literature_document, true);
        test.add_industry_classification(readability::industry_classification::childrens_publishing_industry, true);
        test.add_industry_classification(readability::industry_classification::adult_publishing_industry, true);
        test.add_industry_classification(readability::industry_classification::childrens_healthcare_industry, true);
        test.add_industry_classification(readability::industry_classification::adult_healthcare_industry, true);
        test.add_industry_classification(readability::industry_classification::military_government_industry, true);
        test.add_teaching_level(readability::test_teaching_level::secondary_grade);
        test.add_teaching_level(readability::test_teaching_level::adult_level);
        test.add_language(readability::test_language::german_test);
        test.add_factor(readability::test_factor::word_complexity);
        test.add_factor(readability::test_factor::sentence_length);
        m_defaultReadabilityTestsTemplate.add_test(test);
        }

    // flesch-kincaid
        {
        readability::readability_test test(ReadabilityMessages::FLESCH_KINCAID(),
            XRCID("ID_FLESCH_KINCAID"),
            _DT(L"Flesch-Kincaid"), _DT(L"Flesch-Kincaid"),
            _(L"Flesch-Kincaid is designed for technical documents and is mostly applicable to manuals and forms, rather than schoolbook text or literary works.\n\nThis test is part of the Kincaid Navy Personnel collection of tests."),
            readability::readability_test_type::grade_level, false,
            _DT(L"(11.8*(B/W)) + (.39*(W/S)) - 15.59"));
        test.add_document_classification(readability::document_classification::general_document, true);
        test.add_document_classification(readability::document_classification::technical_document, true);
        test.add_industry_classification(readability::industry_classification::military_government_industry, true);//Navy
        test.add_teaching_level(readability::test_teaching_level::secondary_grade);
        test.add_teaching_level(readability::test_teaching_level::adult_level);
        test.add_language(readability::test_language::english_test);
        test.add_factor(readability::test_factor::word_complexity);
        test.add_factor(readability::test_factor::sentence_length);
        m_defaultReadabilityTestsTemplate.add_test(test);
        }

    // flesch-kincaid (simplified)
        {
        readability::readability_test test(ReadabilityMessages::FLESCH_KINCAID_SIMPLIFIED(),
            XRCID("ID_FLESCH_KINCAID_SIMPLIFIED"),
            _DT(L"Flesch-Kincaid (simplified)"), _DT(L"Flesch-Kincaid (simplified)"),
            _(L"Flesch-Kincaid is designed for technical documents and is mostly applicable to manuals and forms, rather than schoolbook text or literary works.\n\nThis test is part of the Kincaid Navy Personnel collection of tests."),
            readability::readability_test_type::grade_level, false,
            _DT(L"(12*(B/W)) + (.4*(W/S)) - 16"));
        test.add_teaching_level(readability::test_teaching_level::secondary_grade);
        test.add_teaching_level(readability::test_teaching_level::adult_level);
        test.add_language(readability::test_language::english_test);
        test.add_factor(readability::test_factor::word_complexity);
        test.add_factor(readability::test_factor::sentence_length);
        m_defaultReadabilityTestsTemplate.add_test(test);
        }

    // new fog count
        {
        readability::readability_test test(ReadabilityMessages::NEW_FOG(),
            XRCID("ID_NEW_FOG_COUNT"),
            _DT(L"New Fog Count"), _DT(L"New Fog Count (Kincaid)"),
            _(L"New Fog Count is a modified version of the Gunning Fog Index created for the U.S. Navy and was designed for technical documents and manuals.\n\nThis test is part of the Kincaid Navy Personnel collection of tests."),
            readability::readability_test_type::grade_level, false,
            _DT(L"(((((WordCount(Default)-HardFogWordCount())) + (3 * HardFogWordCount())) / SentenceCount(GunningFog)) - 3) / 2"));
        test.add_industry_classification(readability::industry_classification::military_government_industry, true);//Navy
        test.add_teaching_level(readability::test_teaching_level::adult_level);
        test.add_language(readability::test_language::english_test);
        test.add_factor(readability::test_factor::word_complexity_3_plus_syllables);
        test.add_factor(readability::test_factor::sentence_length);
        m_defaultReadabilityTestsTemplate.add_test(test);
        }

    // eflaw
        {
        readability::readability_test test(ReadabilityMessages::EFLAW(),
            XRCID("ID_EFLAW"),
            _DT(L"EFLAW"), _DT(L"McAlpine EFLAW"),
            _(L"The McAlpine EFLAW formula is used to determine the ease of reading English text for ESL/EFL (English as a Second/Foreign Language) readers. This test is influenced by sentence length and words consisting of three or less characters. To lower a score, try using shorter sentences and longer, less colloquial words."),
            readability::readability_test_type::index_value, true,
            _DT(L"ROUND((W+T)/S)"));
        test.add_industry_classification(readability::industry_classification::sedondary_language_industry, true);
        test.add_teaching_level(readability::test_teaching_level::second_language);
        test.add_language(readability::test_language::english_test);
        test.add_factor(readability::test_factor::word_length_3_less);
        test.add_factor(readability::test_factor::sentence_length);
        m_defaultReadabilityTestsTemplate.add_test(test);
        }

    // Qu (german)
        {
        readability::readability_test test(ReadabilityMessages::QU(),
            XRCID("ID_QU"),
            _DT(L"Qu"), _DT(L"Qu (Quadratwurzelverfahren)"),
            _(L"Qu is generally appropriate for secondary age (4th grade to college level) readers."),
            readability::readability_test_type::grade_level, false,
            _DT(L"TRUNC(SQRT((C*(100/W))/(S*(100/W))*30) - 2)"));
        test.add_teaching_level(readability::test_teaching_level::secondary_grade);
        test.add_language(readability::test_language::german_test);
        test.add_factor(readability::test_factor::word_complexity_3_plus_syllables);
        test.add_factor(readability::test_factor::sentence_length);
        m_defaultReadabilityTestsTemplate.add_test(test);
        }

    // smog (german)
        {
        readability::readability_test test(ReadabilityMessages::SMOG_BAMBERGER_VANECEK(),
            XRCID("ID_SMOG_BAMBERGER_VANECEK"),
            _DT(L"SMOG (Bamberger-Vanecek)"), _DT(L"SMOG (Bamberger-Vanecek)"),
            _(L"A German adaption of SMOG, which is generally appropriate for secondary age (4th grade to college level) readers. SMOG tests for 100% comprehension, whereas most formulas test for around 50%-75% comprehension."),
            readability::readability_test_type::grade_level, false,
            _DT(L"TRUNC(SQRT(C*(30/S)) - 2)"));
        test.add_document_classification(readability::document_classification::general_document, true);
        test.add_document_classification(readability::document_classification::adult_literature_document, true);
        test.add_industry_classification(readability::industry_classification::childrens_publishing_industry, true);
        test.add_industry_classification(readability::industry_classification::adult_publishing_industry, true);
        test.add_industry_classification(readability::industry_classification::childrens_healthcare_industry, true);
        test.add_industry_classification(readability::industry_classification::adult_healthcare_industry, true);
        test.add_teaching_level(readability::test_teaching_level::secondary_grade);
        test.add_teaching_level(readability::test_teaching_level::adult_level);
        test.add_language(readability::test_language::german_test);
        test.add_factor(readability::test_factor::word_complexity_3_plus_syllables);
        test.add_factor(readability::test_factor::sentence_length);
        m_defaultReadabilityTestsTemplate.add_test(test);
        }

    // smog
        {
        readability::readability_test test(ReadabilityMessages::SMOG(),
            XRCID("ID_SMOG"),
            _DT(L"SMOG"), _DT(L"SMOG"),
            _(L"SMOG (colloquially referred to as Simple Measure of Gobbledygook) is generally appropriate for secondary age (4th grade to college level) readers. SMOG tests for 100% comprehension, whereas most formulas test for around 50%-75% comprehension."),
            readability::readability_test_type::grade_level, false,
            _DT(L"1.0430*SQRT(C*(30/S)) + 3.1291"));
        test.add_document_classification(readability::document_classification::general_document, true);
        test.add_document_classification(readability::document_classification::adult_literature_document, true);
        test.add_industry_classification(readability::industry_classification::childrens_publishing_industry, true);
        test.add_industry_classification(readability::industry_classification::adult_publishing_industry, true);
        test.add_industry_classification(readability::industry_classification::childrens_healthcare_industry, true);
        test.add_industry_classification(readability::industry_classification::adult_healthcare_industry, true);
        test.add_teaching_level(readability::test_teaching_level::secondary_grade);
        test.add_teaching_level(readability::test_teaching_level::adult_level);
        test.add_language(readability::test_language::english_test);
        test.add_factor(readability::test_factor::word_complexity_3_plus_syllables);
        test.add_factor(readability::test_factor::sentence_length);
        m_defaultReadabilityTestsTemplate.add_test(test);
        }

    // smog (simplified)
        {
        readability::readability_test test(ReadabilityMessages::SMOG_SIMPLIFIED(),
            XRCID("ID_SMOG_SIMPLIFIED"),
            _(L"SMOG (simplified)"), _(L"SMOG (simplified)"),
            _(L"SMOG (colloquially referred to as Simple Measure of Gobbledygook) is generally appropriate for secondary age (4th grade to college level) readers. SMOG tests for 100% comprehension, whereas most formulas test for around 50%-75% comprehension."),
            readability::readability_test_type::grade_level, false,
            _DT(L"FLOOR(SQRT(C*(30/S))) + 3"));
        test.add_teaching_level(readability::test_teaching_level::secondary_grade);
        test.add_teaching_level(readability::test_teaching_level::adult_level);
        test.add_language(readability::test_language::english_test);
        test.add_factor(readability::test_factor::word_complexity_3_plus_syllables);
        test.add_factor(readability::test_factor::sentence_length);
        m_defaultReadabilityTestsTemplate.add_test(test);
        }

    // smog (modified for primary readers)
        {
        readability::readability_test test(ReadabilityMessages::MODIFIED_SMOG(),
            XRCID("ID_SMOG_MODIFIED"),
            _DT(L"Modified SMOG"), _DT(L"Modified SMOG"),
            _(L"Modified SMOG is a variation of SMOG that is adjusted for primary-age materials."),
            readability::readability_test_type::grade_level, false,
            L"");
        test.add_teaching_level(readability::test_teaching_level::primary_grade);
        test.add_language(readability::test_language::english_test);
        test.add_factor(readability::test_factor::word_complexity_3_plus_syllables);
        test.add_factor(readability::test_factor::sentence_length);
        m_defaultReadabilityTestsTemplate.add_test(test);
        }

    // New ARI (simplified)
        {
        readability::readability_test test(ReadabilityMessages::SIMPLE_ARI(),
            XRCID("ID_SIMPLIFIED_ARI"),
            _DT(L"SARI"), _DT(L"New Automated Readability Index (Kincaid, simplified)"),
            _(L"New Automated Readability Index is a modified version of ARI created for U.S. Navy materials and was designed for technical documents and manuals.\n\nThis test is part of the Kincaid Navy Personnel collection of tests."),
            readability::readability_test_type::grade_level, false,
            _DT(L"(6*(RP/W)) + (.4*(W/S)) - 27.4"));
        test.add_teaching_level(readability::test_teaching_level::adult_level);
        test.add_language(readability::test_language::english_test);
        test.add_factor(readability::test_factor::word_length);
        test.add_factor(readability::test_factor::sentence_length);
        m_defaultReadabilityTestsTemplate.add_test(test);
        }

    // new ARI
        {
        readability::readability_test test(ReadabilityMessages::NEW_ARI(),
            XRCID("ID_NEW_ARI"),
            _DT(L"New ARI"), _DT(L"New Automated Readability Index (Kincaid)"),
            _(L"New Automated Readability Index is a modified version of ARI created for U.S. Navy materials and was designed for technical documents and manuals.\n\nThis test is part of the Kincaid Navy Personnel collection of tests."),
            readability::readability_test_type::grade_level, false,
            _DT(L"(5.84*(RP/W)) + (.37*(W/S)) - 26.01"));
        test.add_industry_classification(readability::industry_classification::military_government_industry, true);//Navy
        test.add_teaching_level(readability::test_teaching_level::adult_level);
        test.add_language(readability::test_language::english_test);
        test.add_factor(readability::test_factor::word_length);
        test.add_factor(readability::test_factor::sentence_length);
        m_defaultReadabilityTestsTemplate.add_test(test);
        }

    // psk flesch
        {
        readability::readability_test test(ReadabilityMessages::PSK_FLESCH(),
            XRCID("ID_PSK_FLESCH"),
            _DT(L"PSK Flesch"), _DT(L"Powers-Sumner-Kearl (Flesch)"),
            _(L"PSK Flesch is used for student readers."),
            readability::readability_test_type::grade_level, false,
            _DT(L"((W/S) * .0778) + ((B*(100/W)) * .0455) - 2.2029"));
        test.add_document_classification(readability::document_classification::childrens_literature_document, true);
        test.add_industry_classification(readability::industry_classification::childrens_publishing_industry, true);
        test.add_industry_classification(readability::industry_classification::childrens_healthcare_industry, true);
        test.add_teaching_level(readability::test_teaching_level::secondary_grade);
        test.add_language(readability::test_language::english_test);
        test.add_factor(readability::test_factor::word_complexity);
        test.add_factor(readability::test_factor::sentence_length);
        m_defaultReadabilityTestsTemplate.add_test(test);
        }

    // fry
        {
        readability::readability_test test(ReadabilityMessages::FRY(),
            XRCID("ID_FRY"),
            _DT(L"Fry"), _DT(L"Fry"),
            _(L"The <a href=\"#FryGraph\">Fry graph</a> is designed for most text, including literature and technical documents."),
            readability::readability_test_type::grade_level, true,
            L"");
        test.add_document_classification(readability::document_classification::general_document, true);
        test.add_document_classification(readability::document_classification::adult_literature_document, true);
        test.add_document_classification(readability::document_classification::childrens_literature_document, true);
        test.add_industry_classification(readability::industry_classification::childrens_publishing_industry, true);
        test.add_industry_classification(readability::industry_classification::adult_publishing_industry, true);
        test.add_industry_classification(readability::industry_classification::childrens_healthcare_industry, true);
        test.add_industry_classification(readability::industry_classification::adult_healthcare_industry, true);
        test.add_teaching_level(readability::test_teaching_level::primary_grade);
        test.add_teaching_level(readability::test_teaching_level::secondary_grade);
        test.add_language(readability::test_language::english_test);
        test.add_factor(readability::test_factor::word_complexity);
        test.add_factor(readability::test_factor::sentence_length);
        m_defaultReadabilityTestsTemplate.add_test(test);
        }

    // raygor
        {
        readability::readability_test test(ReadabilityMessages::RAYGOR(),
            XRCID("ID_RAYGOR"),
            _DT(L"Raygor Estimate"), _DT(L"Raygor Estimate"),
            _(L"The <a href=\"#RaygorGraph\">Raygor estimate graph</a> is designed for most text, including literature and technical documents."),
            readability::readability_test_type::grade_level, true,
            L"");
        test.add_document_classification(readability::document_classification::general_document, true);
        test.add_document_classification(readability::document_classification::adult_literature_document, true);
        test.add_document_classification(readability::document_classification::childrens_literature_document, true);
        test.add_industry_classification(readability::industry_classification::childrens_publishing_industry, true);
        test.add_industry_classification(readability::industry_classification::adult_publishing_industry, true);
        test.add_industry_classification(readability::industry_classification::childrens_healthcare_industry, true);
        test.add_industry_classification(readability::industry_classification::adult_healthcare_industry, true);
        test.add_teaching_level(readability::test_teaching_level::secondary_grade);
        test.add_language(readability::test_language::english_test);
        test.add_factor(readability::test_factor::word_length_6_plus);
        test.add_factor(readability::test_factor::sentence_length);
        m_defaultReadabilityTestsTemplate.add_test(test);
        }
    }

//-------------------------------------------------------
void BaseProject::AddCustomReadabilityTests()
    {
    //run through the tests
    for (const auto& pos : m_customTestsInUse)
        {
        AddCustomReadabilityTest(pos.GetTestName(), true);
        }
    }

//-------------------------------------------------------
void BaseProject::LoadHardWords()
    {
    PROFILE();
    //complex words (3+ syllable)
    if (HasUI())
        {
        if (Get3SyllablePlusData() == nullptr)
            { m_3SybPlusData = new ListCtrlExNumericDataProvider; }
        Get3SyllablePlusData()->DeleteAllItems();
        Get3SyllablePlusData()->SetSize(m_word_frequency_map->get_data().size(), 4);
        }

    m_total3plusSyllableWords = 0;
    m_unique3PlusSyllableWords = 0;

    //long words (6+ characters)
    if (HasUI())
        {
        if (Get6CharacterPlusData() == nullptr)
            { m_6CharPlusData = new ListCtrlExNumericDataProvider; }
        Get6CharacterPlusData()->DeleteAllItems();
        Get6CharacterPlusData()->SetSize(m_word_frequency_map->get_data().size(), 4);
        }

    m_totalLongWords = 0;
    m_totalSixPlusCharacterWordsIgnoringNumerals = 0;
    m_unique6CharsPlusWords = 0;

    //hard words (DC)
    if (HasUI())
        {
        if (GetDaleChallHardWordData() == nullptr)
            { m_DCHardWordsData = new ListCtrlExNumericDataProvider; }
        GetDaleChallHardWordData()->DeleteAllItems();
        GetDaleChallHardWordData()->SetSize(m_word_frequency_map->get_data().size(), 3);
        }

    readability::is_familiar_word<word_case_insensitive_no_stem,
                     const word_list,
                     stemming::no_op_stem<word_case_insensitive_no_stem>>
                     isDCWord(IsIncludingStockerCatholicSupplement() ? &m_dale_chall_plus_stocker_catholic_word_list : &m_dale_chall_word_list,
                        //because we are reviewing a frequency set, turn off proper noun support
                        //and handle it ourselves
                        readability::proper_noun_counting_method::all_proper_nouns_are_unfamiliar, true);
    m_totalHardWordsDaleChall = 0;
    m_uniqueDCHardWords = 0;

    m_total3PlusSyllabicWordsNumeralsFullySyllabized = 0;
    m_totalHardWordsSol = 0;
    m_unique3PlusSyllabicWordsNumeralsFullySyllabized = 0;
    m_totalHardWordsFog = 0;
    m_uniqueHardFogWords = 0;

    //hard words (Spache)
    if (HasUI())
        {
        if (GetSpacheHardWordData() == nullptr)
            { m_SpacheHardWordsData = new ListCtrlExNumericDataProvider; }
        GetSpacheHardWordData()->DeleteAllItems();
        GetSpacheHardWordData()->SetSize(m_word_frequency_map->get_data().size(), 3);
        }

    readability::is_familiar_word<word_case_insensitive_no_stem,
                     const word_list,
                     stemming::no_op_stem<word_case_insensitive_no_stem>>
                        isSpacheWord(&m_spache_word_list, readability::proper_noun_counting_method::all_proper_nouns_are_unfamiliar, true);
    m_totalHardWordsSpache = 0;
    m_uniqueSpacheHardWords = 0;

    m_uniqueWords = 0;

    //mini words (EFLAW)
    m_uniqueMiniWords = 0;
    m_totalMiniWords = 0;

    //Monosyllabic words
    m_uniqueMonoSyllablicWords = 0;

    //hard words (Harris-Jacobson)
    readability::is_familiar_word<word_case_insensitive_no_stem,
                     const word_list,
                     stemming::no_op_stem<word_case_insensitive_no_stem>>
                        isHarrisJacobsonWord(&m_harris_jacobson_word_list,
                        readability::proper_noun_counting_method::all_proper_nouns_are_unfamiliar, true);
    m_uniqueHarrisJacobsonHardWords = m_totalHardWordsHarrisJacobson = 0;
    if (HasUI())
        {
        if (GetHarrisJacobsonHardWordDataData() == nullptr)
            { m_harrisJacobsonHardWordsData = new ListCtrlExNumericDataProvider; }
        GetHarrisJacobsonHardWordDataData()->DeleteAllItems();
        GetHarrisJacobsonHardWordDataData()->SetSize(m_word_frequency_map->get_data().size(), 3);
        }

    //dolch words
    if (HasUI())
        {
        if (GetDolchWordData() == nullptr)
            { m_dolchWordsData = new ListCtrlExNumericDataProvider; }
        GetDolchWordData()->DeleteAllItems();
        GetDolchWordData()->SetSize(m_word_frequency_map->get_data().size(), 4);
        }
    size_t uniqueDolchWords = 0;

    if (HasUI())
        {
        if (GetNonDolchWordData() == nullptr)
            { m_nonDolchWordsData = new ListCtrlExNumericDataProvider; }
        GetNonDolchWordData()->DeleteAllItems();
        GetNonDolchWordData()->SetSize(m_word_frequency_map->get_data().size(), 2);
        }
    size_t nonUniqueDolchWords = 0;

    m_dolchConjunctionCounts = m_dolchPrepositionCounts = m_dolchPronounCounts =
        m_dolchAdverbCounts = m_dolchAdjectiveCounts = m_dolchVerbCounts = m_dolchNounCounts = std::pair<size_t,size_t>(0,0);

    //reset the custom tests
    for (std::vector<CustomReadabilityTestInterface>::iterator pos = m_customTestsInUse.begin();
        pos != m_customTestsInUse.end();
        ++pos)
        {
        pos->Reset();
        if (HasUI() && pos->GetIterator()->is_using_familiar_words())
            { pos->GetListViewData()->SetSize(m_word_frequency_map->get_data().size(), 2); }
        }

    //calculate the Dolch level completions
    std::set<readability::sight_word> unusedDolchWords;
    for (std::set<readability::sight_word>::const_iterator DolchPos = BaseProjectDoc::m_dolch_word_list.get_words().begin();
         DolchPos != BaseProjectDoc::m_dolch_word_list.get_words().end();
         ++DolchPos)
        {
        if (m_word_frequency_map->get_data().find(
                word_case_insensitive_no_stem(DolchPos->get_word().c_str(),
                    DolchPos->get_word().length(),
                    /*filler arguments not used*/0, 0, 0, false, false, false, false, 0, 0)) == m_word_frequency_map->get_data().end())
            { unusedDolchWords.insert(*DolchPos); }
        }

    m_unusedDolchConjunctions = m_unusedDolchPrepositions = m_unusedDolchPronouns = m_unusedDolchAdverbs =
        m_unusedDolchAdjectives = m_unusedDolchVerbs = m_unusedDolchNounsWords = 0;

    if (HasUI())
        {
        if (GetUnusedDolchWordData() == nullptr)
            { m_unusedDolchWordsData = new ListCtrlExDataProvider; }
        GetUnusedDolchWordData()->DeleteAllItems();
        GetUnusedDolchWordData()->SetSize(unusedDolchWords.size(), 2);
        }
    size_t wordCounter = 0;
    for (std::set<readability::sight_word>::const_iterator pos = unusedDolchWords.begin();
        pos != unusedDolchWords.end();
        ++pos, ++wordCounter)
        {
        wxString classificationLabel;
        switch (pos->get_type())
            {
        case readability::sight_word_type::conjunction:
            classificationLabel = _(L"Conjunction");
            ++m_unusedDolchConjunctions;
            break;
        case readability::sight_word_type::preposition:
            classificationLabel = _(L"Preposition");
            ++m_unusedDolchPrepositions;
            break;
        case readability::sight_word_type::pronoun:
            classificationLabel = _(L"Pronoun");
            ++m_unusedDolchPronouns;
            break;
        case readability::sight_word_type::adverb:
            classificationLabel = _(L"Adverb");
            ++m_unusedDolchAdverbs;
            break;
        case readability::sight_word_type::adjective:
            classificationLabel = _(L"Adjective");
            ++m_unusedDolchAdjectives;
            break;
        case readability::sight_word_type::verb:
            classificationLabel = _(L"Verb");
            ++m_unusedDolchVerbs;
            break;
        case readability::sight_word_type::noun:
            classificationLabel = _(L"Noun");
            ++m_unusedDolchNounsWords;
            break;
            };
        if (HasUI())
            {
            GetUnusedDolchWordData()->SetItemText(wordCounter, 0, pos->get_word().c_str());
            GetUnusedDolchWordData()->SetItemText(wordCounter, 1, classificationLabel);
            }
        }

    if (HasUI())
        {
        if (GetProperNounsData() == nullptr)
            { m_ProperNounsData = new ListCtrlExNumericDataProvider; }
        GetProperNounsData()->DeleteAllItems();
        GetProperNounsData()->SetSize(GetWordsWithFrequencies()->get_data().size(), 3);

        if (GetContractionsData() == nullptr)
            { m_contractionsData = new  ListCtrlExNumericDataProvider; }
        GetContractionsData()->DeleteAllItems();
        GetContractionsData()->SetSize(GetWordsWithFrequencies()->get_data().size(), 2);

        if (GetAllWordsBaseData() == nullptr)
            { m_AllWordsBaseData = new ListCtrlExNumericDataProvider; }
        GetAllWordsBaseData()->DeleteAllItems();
        GetAllWordsBaseData()->SetSize(GetWordsWithFrequencies()->get_data().size(), 4);
        }

    m_uniqueWords = m_word_frequency_map->get_data().size();

    // Load proper nouns and all words lists
    multi_value_frequency_aggregate_map<traits::case_insensitive_wstring_ex,
                                        traits::case_insensitive_wstring_ex> importantWordsStemmedWithCounts;
    auto stemmer = CreateStemmer();

    size_t uniqueProperNouns(0), uniqueContractions(0);
    size_t i = 0;
    for (auto wordPos = GetWordsWithFrequencies()->get_data().begin();
        wordPos != GetWordsWithFrequencies()->get_data().end();
        ++wordPos, ++i)
        {
        // the values are the words are frequency count (first) and the number
        // of those that are proper second)
        wxASSERT_LEVEL_2(wordPos->second.first >= wordPos->second.second);
        /* subtract number of times word is proper from total count of word
           to see if at least on instance is NOT proper. If they are equal, then
           all instances are proper and therefore cannot be an UNfamiliar word.*/
        const bool allInstancesAreProper = (wordPos->second.first == wordPos->second.second);
        const size_t nonProperCount = (wordPos->second.first - wordPos->second.second);
        if (wordPos->first.get_syllable_count() == 1 ||
            (wordPos->first.is_numeric() && GetNumeralSyllabicationMethod() == NumeralSyllabize::WholeWordIsOneSyllable))
            {
            ++m_uniqueMonoSyllablicWords;
            }
        //proper
        if (HasUI() && wordPos->second.second > 0)
            {
            //Make sure first letter is capitalized (just looks nicer).
            //Can't use MakeCapitalized() because it messes up Roman numerals.
            wxString theWord(wordPos->first.c_str());
            if (theWord.length())
                { theWord[0] = std::towupper(theWord[0]); }
            GetProperNounsData()->SetItemText(uniqueProperNouns, 0, theWord);
            //if proper occurrences are less than the total occurrences, then there must be non-proper instances
            if (wordPos->second.first > wordPos->second.second)
                {
                GetProperNounsData()->SetItemText(uniqueProperNouns, 1, wxString::Format(_(L"%zu (%zu additional occurrences not proper)"), wordPos->second.second,
                    (wordPos->second.first-wordPos->second.second)), NumberFormatInfo::NumberFormatType::StandardFormatting, wordPos->second.second);
                }
            else
                { GetProperNounsData()->SetItemValue(uniqueProperNouns, 1, wordPos->second.second); }
            GetProperNounsData()->SetItemText(uniqueProperNouns, 2, wordPos->first.is_personal() ? _(L"Yes") : _(L"No"));
            ++uniqueProperNouns;
            }
        if (HasUI() && wordPos->first.is_contraction())
            {
            GetContractionsData()->SetItemText(uniqueContractions, 0, wordPos->first.c_str());
            GetContractionsData()->SetItemValue(uniqueContractions, 1, wordPos->second.first);
            ++uniqueContractions;
            }
        // all words
        if (HasUI())
            {
            GetAllWordsBaseData()->SetItemText(i, 0, wordPos->first.c_str() );
            GetAllWordsBaseData()->SetItemValue(i, 1, wordPos->second.first);
            /*if a numeric string then check to see if we are supposed to treat numerals as monosyllabic*/
            if (wordPos->first.is_numeric() && GetNumeralSyllabicationMethod() == NumeralSyllabize::WholeWordIsOneSyllable)
                { GetAllWordsBaseData()->SetItemValue(i, 2, 1); }
            else
                { GetAllWordsBaseData()->SetItemValue(i, 2,wordPos->first.get_syllable_count()); }
            GetAllWordsBaseData()->SetItemValue(i, 3, wordPos->first.get_length_excluding_punctuation());
            }
        // important (stemmed) words with a list of their variations occurring in the document
        if (HasUI() &&
            !wordPos->first.is_file_address() &&
            !wordPos->first.is_numeric() &&
            !GetWords()->is_word_common(wordPos->first.c_str()) )
            {
            traits::case_insensitive_wstring_ex currentWord(wordPos->first.c_str());
            (*stemmer)(currentWord);
            importantWordsStemmedWithCounts.insert(
                // the stem and original word
                currentWord, wordPos->first,
                // overall frequency of current word (not the proper noun count)
                wordPos->second.first);
            }
        //dolch sight words
        std::set<readability::sight_word>::const_iterator dolchIter = m_dolch_word_list(wordPos->first.c_str());
        if (dolchIter != m_dolch_word_list.get_words().end())
            {
            wxString classificationLabel;
            switch (dolchIter->get_type())
                {
            case readability::sight_word_type::conjunction:
                classificationLabel = _(L"Conjunction");
                ++m_dolchConjunctionCounts.first;
                m_dolchConjunctionCounts.second += wordPos->second.first;
                break;
            case readability::sight_word_type::preposition:
                classificationLabel = _(L"Preposition");
                ++m_dolchPrepositionCounts.first;
                m_dolchPrepositionCounts.second += wordPos->second.first;
                break;
            case readability::sight_word_type::pronoun:
                classificationLabel = _(L"Pronoun");
                ++m_dolchPronounCounts.first;
                m_dolchPronounCounts.second += wordPos->second.first;
                break;
            case readability::sight_word_type::adverb:
                classificationLabel = _(L"Adverb");
                ++m_dolchAdverbCounts.first;
                m_dolchAdverbCounts.second += wordPos->second.first;
                break;
            case readability::sight_word_type::adjective:
                classificationLabel = _(L"Adjective");
                ++m_dolchAdjectiveCounts.first;
                m_dolchAdjectiveCounts.second += wordPos->second.first;
                break;
            case readability::sight_word_type::verb:
                classificationLabel = _(L"Verb");
                ++m_dolchVerbCounts.first;
                m_dolchVerbCounts.second += wordPos->second.first;
                break;
            case readability::sight_word_type::noun:
                classificationLabel = _(L"Noun");
                ++m_dolchNounCounts.first;
                m_dolchNounCounts.second += wordPos->second.first;
                break;
                };
            if (HasUI())
                {
                GetDolchWordData()->SetItemText(uniqueDolchWords, 0, wordPos->first.c_str() );
                GetDolchWordData()->SetItemValue(uniqueDolchWords, 1, wordPos->second.first);
                GetDolchWordData()->SetItemText(uniqueDolchWords, 2, classificationLabel);
                }
            ++uniqueDolchWords;
            }
        else
            {
            if (HasUI())
                {
                GetNonDolchWordData()->SetItemText(nonUniqueDolchWords, 0, wordPos->first.c_str() );
                GetNonDolchWordData()->SetItemValue(nonUniqueDolchWords, 1, wordPos->second.first);
                }
            ++nonUniqueDolchWords;
            }
        //3+ syllable words
        if (wordPos->first.get_syllable_count() >= 3)
            {
            /*if not a numeric string then it is OK. If it is numeric then check
            to see if we are supposed to treat numerals as monosyllabic*/
            if (!wordPos->first.is_numeric() ||
                GetNumeralSyllabicationMethod() != NumeralSyllabize::WholeWordIsOneSyllable)
                {
                //only load the data for standard projects
                if (HasUI())
                    {
                    Get3SyllablePlusData()->SetItemText(GetTotalUnique3PlusSyllableWords(), 0, wordPos->first.c_str() );
                    Get3SyllablePlusData()->SetItemValue(GetTotalUnique3PlusSyllableWords(), 1, wordPos->first.get_syllable_count() );
                    Get3SyllablePlusData()->SetItemValue(GetTotalUnique3PlusSyllableWords(), 2, wordPos->second.first);
                    std::pair<bool,word_list_with_replacements::word_type> replacement = difficult_word_replacement_list.find(wordPos->first.c_str());
                    if (replacement.first)
                        { Get3SyllablePlusData()->SetItemText(GetTotalUnique3PlusSyllableWords(), 3, replacement.second.c_str() ); }
                    }
                ++m_unique3PlusSyllableWords;
                m_total3plusSyllableWords += wordPos->second.first;
                }
            }
        //Fog
        if (wordPos->first.get_syllable_count() >= 3 &&
            //if all instances are proper then it can't be a hard Fog word
            !allInstancesAreProper)
            {
            /*if not a numeric string then it is OK. If it is numeric then check
            to see if we are supposed to treat numerals as monosyllabic*/
            if (wordPos->first.is_numeric() && GetNumeralSyllabicationMethod() == NumeralSyllabize::WholeWordIsOneSyllable)
                { /*NOOP*/ }
            else if (!readability::is_easy_gunning_fog_word(wordPos->first.c_str(),
                                                            wordPos->first.length(),
                                                            wordPos->first.get_syllable_count()) )
                {
                ++m_uniqueHardFogWords;
                m_totalHardWordsFog += nonProperCount;
                }
            }
        //SMOG
        if (wordPos->first.get_syllable_count() >= 3)
            {
            ++m_unique3PlusSyllabicWordsNumeralsFullySyllabized;
            m_total3PlusSyllabicWordsNumeralsFullySyllabized += wordPos->second.first;
            }
        if (wordPos->first.get_length_excluding_punctuation() >= 6)
            {
            //only load the data for standard projects
            if (HasUI())
                {
                Get6CharacterPlusData()->SetItemText(GetTotalUnique6CharsPlusWords(), 0, wordPos->first.c_str() );
                Get6CharacterPlusData()->SetItemValue(GetTotalUnique6CharsPlusWords(), 1, wordPos->first.get_length_excluding_punctuation() );
                Get6CharacterPlusData()->SetItemValue(GetTotalUnique6CharsPlusWords(), 2, wordPos->second.first);
                std::pair<bool,word_list_with_replacements::word_type> replacement = difficult_word_replacement_list.find(wordPos->first.c_str());
                if (replacement.first)
                    { Get6CharacterPlusData()->SetItemText(GetTotalUnique6CharsPlusWords(), 3, replacement.second.c_str() ); }
                }
            ++m_unique6CharsPlusWords;
            m_totalLongWords += wordPos->second.first;
            if (!wordPos->first.is_numeric())
                { m_totalSixPlusCharacterWordsIgnoringNumerals += wordPos->second.first; }
            }
        if (wordPos->first.get_length_excluding_punctuation() <= 3 && !wordPos->first.is_numeric() )
            {
            ++m_uniqueMiniWords;
            m_totalMiniWords += wordPos->second.first;
            }
        if (!allInstancesAreProper &&
            (GetHarrisJacobsonTextExclusionMode() == SpecializedTestTextExclusion::UseSystemDefault) &&
            !isHarrisJacobsonWord(wordPos->first))
            {
            //only load the data for standard projects
            if (HasUI())
                {
                GetHarrisJacobsonHardWordDataData()->SetItemText(GetTotalUniqueHarrisJacobsonHardWords(), 0, wordPos->first.c_str() );
                if (wordPos->second.first == nonProperCount)
                    { GetHarrisJacobsonHardWordDataData()->SetItemValue(GetTotalUniqueHarrisJacobsonHardWords(), 1, nonProperCount); }
                else
                    {
                    GetHarrisJacobsonHardWordDataData()->SetItemText(GetTotalUniqueHarrisJacobsonHardWords(), 1, wxString::Format(_(L"%zu (%zu total occurrences, %zu proper and familiar, %zu non-proper and unfamiliar)"),
                        nonProperCount, wordPos->second.first, (wordPos->second.first-nonProperCount), nonProperCount), NumberFormatInfo::NumberFormatType::StandardFormatting, nonProperCount);
                    }
                std::pair<bool,word_list_with_replacements::word_type> replacement = harris_jacobson_replacement_list.find(wordPos->first.c_str());
                if (replacement.first)
                    { GetHarrisJacobsonHardWordDataData()->SetItemText(GetTotalUniqueHarrisJacobsonHardWords(), 2, replacement.second.c_str() ); }
                }
            ++m_uniqueHarrisJacobsonHardWords;
            m_totalHardWordsHarrisJacobson += nonProperCount;
            }
        if ((GetDaleChallTextExclusionMode() == SpecializedTestTextExclusion::UseSystemDefault) &&
            !isDCWord(wordPos->first))
            {
            //all forms of word are proper and proper words are familiar? Then it can't be a hard word at all.
            if (allInstancesAreProper && GetDaleChallProperNounCountingMethod() == readability::proper_noun_counting_method::all_proper_nouns_are_familiar)
                { /*no-op*/; }
            else if (GetDaleChallProperNounCountingMethod() == readability::proper_noun_counting_method::all_proper_nouns_are_familiar)
                {
                //only load the data for standard projects
                if (HasUI())
                    {
                    GetDaleChallHardWordData()->SetItemText(GetTotalUniqueDCHardWords(), 0, wordPos->first.c_str() );
                    if (wordPos->second.first == nonProperCount)
                        { GetDaleChallHardWordData()->SetItemValue(GetTotalUniqueDCHardWords(), 1, nonProperCount); }
                    else
                        {
                        GetDaleChallHardWordData()->SetItemText(GetTotalUniqueDCHardWords(), 1, wxString::Format(_(L"%zu (%zu total occurrences, %zu proper and familiar, %zu non-proper and unfamiliar)"),
                            nonProperCount, wordPos->second.first, (wordPos->second.first-nonProperCount), nonProperCount), NumberFormatInfo::NumberFormatType::StandardFormatting, nonProperCount);
                        }
                    std::pair<bool,word_list_with_replacements::word_type> replacement = dale_chall_replacement_list.find(wordPos->first.c_str());
                    if (replacement.first)
                        { GetDaleChallHardWordData()->SetItemText(GetTotalUniqueDCHardWords(), 2, replacement.second.c_str() ); }
                    }
                ++m_uniqueDCHardWords;
                m_totalHardWordsDaleChall += nonProperCount;
                }
            else if (GetDaleChallProperNounCountingMethod() == readability::proper_noun_counting_method::all_proper_nouns_are_unfamiliar)
                {
                //only load the data for standard projects
                if (HasUI())
                    {
                    GetDaleChallHardWordData()->SetItemText(GetTotalUniqueDCHardWords(), 0, wordPos->first.c_str() );
                    GetDaleChallHardWordData()->SetItemValue(GetTotalUniqueDCHardWords(), 1, wordPos->second.first);
                    std::pair<bool,word_list_with_replacements::word_type> replacement = dale_chall_replacement_list.find(wordPos->first.c_str());
                    if (replacement.first)
                        { GetDaleChallHardWordData()->SetItemText(GetTotalUniqueDCHardWords(), 2, replacement.second.c_str() ); }
                    }
                ++m_uniqueDCHardWords;
                m_totalHardWordsDaleChall += wordPos->second.first;
                }
            else if (GetDaleChallProperNounCountingMethod() == readability::proper_noun_counting_method::only_count_first_instance_of_proper_noun_as_unfamiliar)
                {
                //only load the data for standard projects
                if (HasUI())
                    {
                    GetDaleChallHardWordData()->SetItemText(GetTotalUniqueDCHardWords(), 0, wordPos->first.c_str() );
                    if (wordPos->second.first == nonProperCount || wordPos->second.first == 1)
                        { GetDaleChallHardWordData()->SetItemValue(GetTotalUniqueDCHardWords(), 1, wordPos->second.first); }
                    else if (nonProperCount == 0)
                        {
                        GetDaleChallHardWordData()->SetItemText(GetTotalUniqueDCHardWords(), 1, wxString::Format(_(L"1 (%zu total occurrences, only first occurrence unfamiliar)"), wordPos->second.first),
                            NumberFormatInfo::NumberFormatType::StandardFormatting, 1);
                        }
                    else
                        {
                        GetDaleChallHardWordData()->SetItemText(GetTotalUniqueDCHardWords(), 1, wxString::Format(_(L"%zu (%zu total occurrences. First proper occurrence unfamiliar, %zu non-proper and unfamiliar)"),
                            nonProperCount+1, wordPos->second.first, nonProperCount), NumberFormatInfo::NumberFormatType::StandardFormatting, nonProperCount+1);
                        }
                    std::pair<bool,word_list_with_replacements::word_type> replacement = dale_chall_replacement_list.find(wordPos->first.c_str());
                    if (replacement.first)
                        { GetDaleChallHardWordData()->SetItemText(GetTotalUniqueDCHardWords(), 2, replacement.second.c_str() ); }
                    }
                ++m_uniqueDCHardWords;
                m_totalHardWordsDaleChall += (wordPos->second.first == nonProperCount) ? wordPos->second.first : nonProperCount+1;
                }
            }
        if (!allInstancesAreProper &&
            !isSpacheWord(wordPos->first))
            {
            //only load the data for standard projects
            if (HasUI())
                {
                GetSpacheHardWordData()->SetItemText(GetTotalUniqueHardWordsSpache(), 0, wordPos->first.c_str() );
                if (wordPos->second.first == nonProperCount)
                    { GetSpacheHardWordData()->SetItemValue(GetTotalUniqueHardWordsSpache(), 1, nonProperCount); }
                else
                    {
                    GetSpacheHardWordData()->SetItemText(GetTotalUniqueHardWordsSpache(), 1, wxString::Format(_(L"%zu (%zu total occurrences, %zu proper and familiar, %zu non-proper and unfamiliar)"),
                        nonProperCount, wordPos->second.first, (wordPos->second.first-nonProperCount), nonProperCount), NumberFormatInfo::NumberFormatType::StandardFormatting, nonProperCount);
                    }
                std::pair<bool,word_list_with_replacements::word_type> replacement = spache_replacement_list.find(wordPos->first.c_str());
                if (replacement.first)
                    { GetSpacheHardWordData()->SetItemText(GetTotalUniqueHardWordsSpache(), 2, replacement.second.c_str() ); }
                }
            ++m_uniqueSpacheHardWords;
            m_totalHardWordsSpache += nonProperCount;
            }
        //go through the custom tests
        for (std::vector<CustomReadabilityTestInterface>::iterator pos = m_customTestsInUse.begin();
            pos != m_customTestsInUse.end();
            ++pos)
            {
            /*if not using familiar word then skip. Also need to skip if using CustomHJ or
            CustomDC (if regular DC is excluding only list items)--in that case, it will be handled in next loop.*/ 
            if (!pos->GetIterator()->is_using_familiar_words() ||
                (pos->IsHarrisJacobsonFormula() && GetHarrisJacobsonTextExclusionMode() == SpecializedTestTextExclusion::ExcludeIncompleteSentencesExceptHeadings) ||
                (pos->IsDaleChallFormula() && GetDaleChallTextExclusionMode() == SpecializedTestTextExclusion::ExcludeIncompleteSentencesExceptHeadings))
                { continue; }
            if (!pos->GetIterator()->is_word_familiar(wordPos->first,
                    false, pos->GetIterator()->is_including_numeric_as_familiar()))
                {
                if (allInstancesAreProper && pos->GetIterator()->get_proper_noun_method() == readability::proper_noun_counting_method::all_proper_nouns_are_familiar)
                    { continue; }
                else if (pos->GetIterator()->get_proper_noun_method() == readability::proper_noun_counting_method::all_proper_nouns_are_familiar)
                    {
                    //only load the data if user asked for this view
                    if (HasUI())
                        {
                        pos->GetListViewData()->SetItemText(pos->GetUniqueUnfamiliarWordCount(), 0, wordPos->first.c_str() );
                        if (wordPos->second.first == nonProperCount)
                            { pos->GetListViewData()->SetItemValue(pos->GetUniqueUnfamiliarWordCount(), 1, nonProperCount); }
                        else
                            {
                            pos->GetListViewData()->SetItemText(pos->GetUniqueUnfamiliarWordCount(), 1, wxString::Format(_(L"%zu (%zu total occurrences, %zu proper and familiar, %zu non-proper and unfamiliar)"),
                                nonProperCount, wordPos->second.first, (wordPos->second.first-nonProperCount), nonProperCount), NumberFormatInfo::NumberFormatType::StandardFormatting, nonProperCount);
                            }
                        }
                    pos->IncrementUnfamiliarWordCount(nonProperCount);
                    }
                else if (pos->GetIterator()->get_proper_noun_method() == readability::proper_noun_counting_method::all_proper_nouns_are_unfamiliar)
                    {
                    //only load the data if user asked for this view
                    if (HasUI())
                        {
                        pos->GetListViewData()->SetItemText(pos->GetUniqueUnfamiliarWordCount(), 0, wordPos->first.c_str() );
                        pos->GetListViewData()->SetItemValue(pos->GetUniqueUnfamiliarWordCount(), 1, wordPos->second.first);
                        }
                    pos->IncrementUnfamiliarWordCount(wordPos->second.first);
                    }
                else if (pos->GetIterator()->get_proper_noun_method() == readability::proper_noun_counting_method::only_count_first_instance_of_proper_noun_as_unfamiliar)
                    {
                    //only load the data if user asked for this view
                    if (HasUI())
                        {
                        pos->GetListViewData()->SetItemText(pos->GetUniqueUnfamiliarWordCount(), 0, wordPos->first.c_str() );
                        if (wordPos->second.first == nonProperCount || wordPos->second.first == 1)
                            { pos->GetListViewData()->SetItemValue(pos->GetUniqueUnfamiliarWordCount(), 1, wordPos->second.first); }
                        else if (nonProperCount == 0)
                            {
                            pos->GetListViewData()->SetItemText(pos->GetUniqueUnfamiliarWordCount(), 1, wxString::Format(_(L"1 (%zu total occurrences, only first occurrence unfamiliar)"), wordPos->second.first),
                                NumberFormatInfo::NumberFormatType::StandardFormatting, 1);
                            }
                        else
                            {
                            pos->GetListViewData()->SetItemText(pos->GetUniqueUnfamiliarWordCount(), 1, wxString::Format(_(L"%zu (%zu total occurrences. First proper occurrence unfamiliar, %zu non-proper and unfamiliar)"),
                                nonProperCount+1, wordPos->second.first, nonProperCount), NumberFormatInfo::NumberFormatType::StandardFormatting, nonProperCount+1);
                            }
                        }
                    pos->IncrementUnfamiliarWordCount((wordPos->second.first == nonProperCount) ? wordPos->second.first : nonProperCount+1);
                    }
                pos->IncrementUniqueUnfamiliarWordCount();
                }
            }
        }

    // important words (uncommon words removed, remaining stemmed and combined)
    size_t uniqueImportWordsCount{ 0 };
    if (HasUI())
        {
        if (GetImportantWordsBaseData() == nullptr)
            { m_importantWordsBaseData = new ListCtrlExNumericDataProvider; }
        GetImportantWordsBaseData()->DeleteAllItems();
        GetImportantWordsBaseData()->SetSize(importantWordsStemmedWithCounts.get_data().size(), 2);

        if (m_threePlusSyllableWords == nullptr)
            { m_threePlusSyllableWords = std::make_shared<Wisteria::Data::Dataset>(); }
        m_threePlusSyllableWords->Clear();
        m_threePlusSyllableWords->AddCategoricalColumn(GetWordsColumnName());
        m_threePlusSyllableWords->AddContinuousColumn(GetWordsCountsSColumnName());
        wxASSERT_MSG(m_threePlusSyllableWords->GetCategoricalColumns().size() == 1,
            L"Hard word dataset invalid!");
        wxASSERT_MSG(m_threePlusSyllableWords->GetRowCount() == 0,
            L"Hard word dataset should be empty!");
        m_threePlusSyllableWords->Resize(importantWordsStemmedWithCounts.get_data().size());
        auto hardWordsColumn =
            m_threePlusSyllableWords->GetCategoricalColumn(GetWordsColumnName());
        auto hardWordsFreqColumn =
            m_threePlusSyllableWords->GetContinuousColumn(GetWordsCountsSColumnName());

        wxString allValuesStr;
        for (const auto& [hardWordStem, hardWordFreqInfo] : importantWordsStemmedWithCounts.get_data())
            {
            // WORD CLOUD DATASET
            // which variation of the current stem occurs the most often
            auto mostFrequentWordVariation =
                std::max_element(hardWordFreqInfo.first.get_data().cbegin(),
                    hardWordFreqInfo.first.get_data().cend(),
                    [](const auto lhv, const auto rhv) noexcept
                    { return lhv.second < rhv.second; });
            wxASSERT_MSG(mostFrequentWordVariation != hardWordFreqInfo.first.get_data().cend(),
                L"Empty word list for stemmed word?!");
            const auto nextKey = hardWordsColumn->GetNextKey();
            if (mostFrequentWordVariation != hardWordFreqInfo.first.get_data().cend())
                {
                hardWordsColumn->GetStringTable().insert(
                    std::make_pair(nextKey, mostFrequentWordVariation->first.c_str()));
                }
            // could never happen, but for robustness sake use the stem word
            // if the word list for the stem is empty
            else
                {
                hardWordsColumn->GetStringTable().insert(
                    std::make_pair(nextKey, hardWordStem.c_str()));
                }
            hardWordsColumn->SetValue(uniqueImportWordsCount, nextKey);
            hardWordsFreqColumn->SetValue(uniqueImportWordsCount, hardWordFreqInfo.second);

            // IMPORTANT WORDS LISTCONTROL
            // aggregate all the variations of the current word that share a common stem
            allValuesStr.clear();
            for (const auto& subWord : hardWordFreqInfo.first.get_data())
                { allValuesStr.append(subWord.first.c_str()).append(L"; "); }
            allValuesStr.Trim();
            allValuesStr.RemoveLast();
            wxASSERT_MSG(allValuesStr.length(), L"Empty word list from stemmed word!");

            GetImportantWordsBaseData()->SetItemText(uniqueImportWordsCount, 0, allValuesStr);
            GetImportantWordsBaseData()->SetItemValue(uniqueImportWordsCount, 1, hardWordFreqInfo.second);
            ++uniqueImportWordsCount;
            }
        }

    // special handling for tests that include headers, but not lists
    m_totalWordsFromCompleteSentencesAndHeaders = 0;
    m_totalSentencesFromCompleteSentencesAndHeaders = 0;
    m_totalNumeralsFromCompleteSentencesAndHeaders = 0;
    m_totalCharactersFromCompleteSentencesAndHeaders = 0;
    double_frequency_set<word_case_insensitive_no_stem> complete_sent_and_header_word_frequency_map;
    for (std::vector<grammar::sentence_info>::const_iterator sentPos = GetWords()->get_sentences().begin();
        sentPos != GetWords()->get_sentences().end();
        ++sentPos)
        {
        if (sentPos->is_valid() || sentPos->get_type() == grammar::sentence_paragraph_type::header)
            {
            ++m_totalSentencesFromCompleteSentencesAndHeaders;
            // go through the words in the current sentence and add them to the map
            for (size_t wordIter = sentPos->get_first_word_index();
                 wordIter <= sentPos->get_last_word_index();
                 ++wordIter)
                {
                ++m_totalWordsFromCompleteSentencesAndHeaders;
                if (GetWords()->get_words()[wordIter].is_numeric())
                    { ++m_totalNumeralsFromCompleteSentencesAndHeaders; }
                m_totalCharactersFromCompleteSentencesAndHeaders += GetWords()->get_words()[wordIter].get_length_excluding_punctuation();
                complete_sent_and_header_word_frequency_map.insert(GetWords()->get_words()[wordIter],
                    GetWords()->get_words()[wordIter].is_proper_noun());
                }
            }
        }
    //resize the difficult word vectors
    if (HasUI() && (GetHarrisJacobsonTextExclusionMode() == SpecializedTestTextExclusion::ExcludeIncompleteSentencesExceptHeadings))
        {
        if (GetHarrisJacobsonHardWordDataData() == nullptr)
            { m_harrisJacobsonHardWordsData = new ListCtrlExNumericDataProvider; }
        GetHarrisJacobsonHardWordDataData()->DeleteAllItems();
        GetHarrisJacobsonHardWordDataData()->SetSize(complete_sent_and_header_word_frequency_map.get_data().size(), 3);
        }
    if (HasUI() && (GetDaleChallTextExclusionMode() == SpecializedTestTextExclusion::ExcludeIncompleteSentencesExceptHeadings) )
        {
        if (GetDaleChallHardWordData() == nullptr)
            { m_DCHardWordsData = new ListCtrlExNumericDataProvider; }
        GetDaleChallHardWordData()->DeleteAllItems();
        GetDaleChallHardWordData()->SetSize(complete_sent_and_header_word_frequency_map.get_data().size(), 3);
        }
    for (std::vector<CustomReadabilityTestInterface>::iterator pos = m_customTestsInUse.begin();
        pos != m_customTestsInUse.end();
        ++pos)
        {
        if ((pos->IsHarrisJacobsonFormula() && GetHarrisJacobsonTextExclusionMode() == SpecializedTestTextExclusion::ExcludeIncompleteSentencesExceptHeadings) ||
            (pos->IsDaleChallFormula() && GetDaleChallTextExclusionMode() == SpecializedTestTextExclusion::ExcludeIncompleteSentencesExceptHeadings))
            {
            pos->Reset();
            if (HasUI() && pos->GetIterator()->is_using_familiar_words())
                { pos->GetListViewData()->SetSize(complete_sent_and_header_word_frequency_map.get_data().size(), 2); }
            }
        }
    for (std::map<word_case_insensitive_no_stem, std::pair<size_t,size_t>>::const_iterator wordPos = complete_sent_and_header_word_frequency_map.get_data().begin();
        wordPos != complete_sent_and_header_word_frequency_map.get_data().end();
        ++wordPos)
        {
        wxASSERT_LEVEL_2(wordPos->second.first >= wordPos->second.second);
        /*subtract number of times word is proper from total count of word
        to see if at least on instance is NOT proper. If they are equal, then
        all instances are proper and therefore cannot be an UNfamiliar word.*/
        const bool allInstancesAreProper = (wordPos->second.first == wordPos->second.second);
        const size_t nonProperCount = (wordPos->second.first - wordPos->second.second);
        //SOL (same as SMOG, but needs to include headers)
        if (wordPos->first.get_syllable_count() >= 3)
            {
            m_totalHardWordsSol += wordPos->second.first;
            }
        if (!allInstancesAreProper &&
            (GetHarrisJacobsonTextExclusionMode() == SpecializedTestTextExclusion::ExcludeIncompleteSentencesExceptHeadings) &&
            !isHarrisJacobsonWord(wordPos->first))
            {
            //only load the data for standard projects
            if (HasUI())
                {
                GetHarrisJacobsonHardWordDataData()->SetItemText(GetTotalUniqueHarrisJacobsonHardWords(), 0, wordPos->first.c_str() );
                if (wordPos->second.first == nonProperCount)
                    { GetHarrisJacobsonHardWordDataData()->SetItemValue(GetTotalUniqueHarrisJacobsonHardWords(), 1, nonProperCount); }
                else
                    {
                    GetHarrisJacobsonHardWordDataData()->SetItemText(GetTotalUniqueHarrisJacobsonHardWords(), 1, wxString::Format(_(L"%zu (%zu total occurrences, %zu proper and familiar, %zu non-proper and unfamiliar)"),
                        nonProperCount, wordPos->second.first, (wordPos->second.first-nonProperCount), nonProperCount), NumberFormatInfo::NumberFormatType::StandardFormatting, nonProperCount);
                    }
                std::pair<bool,word_list_with_replacements::word_type> replacement = harris_jacobson_replacement_list.find(wordPos->first.c_str());
                if (replacement.first)
                    { GetHarrisJacobsonHardWordDataData()->SetItemText(GetTotalUniqueHarrisJacobsonHardWords(), 2, replacement.second.c_str() ); }
                }
            ++m_uniqueHarrisJacobsonHardWords;
            m_totalHardWordsHarrisJacobson += nonProperCount;
            }
        if ((GetDaleChallTextExclusionMode() == SpecializedTestTextExclusion::ExcludeIncompleteSentencesExceptHeadings) &&
            !isDCWord(wordPos->first))
            {
            //all forms of word are proper and proper words are familiar? Then it can't be a hard word at all.
            if (allInstancesAreProper && GetDaleChallProperNounCountingMethod() == readability::proper_noun_counting_method::all_proper_nouns_are_familiar)
                { /*no-op*/; }
            else if (GetDaleChallProperNounCountingMethod() == readability::proper_noun_counting_method::all_proper_nouns_are_familiar)
                {
                //only load the data for standard projects
                if (HasUI())
                    {
                    GetDaleChallHardWordData()->SetItemText(GetTotalUniqueDCHardWords(), 0, wordPos->first.c_str() );
                    if (wordPos->second.first == nonProperCount)
                        { GetDaleChallHardWordData()->SetItemValue(GetTotalUniqueDCHardWords(), 1, nonProperCount); }
                    else
                        {
                        GetDaleChallHardWordData()->SetItemText(GetTotalUniqueDCHardWords(), 1, wxString::Format(_(L"%zu (%zu total occurrences, %zu proper and familiar, %zu non-proper and unfamiliar)"),
                            nonProperCount, wordPos->second.first, (wordPos->second.first-nonProperCount), nonProperCount), NumberFormatInfo::NumberFormatType::StandardFormatting, nonProperCount);
                        }
                    std::pair<bool,word_list_with_replacements::word_type> replacement = dale_chall_replacement_list.find(wordPos->first.c_str());
                    if (replacement.first)
                        { GetDaleChallHardWordData()->SetItemText(GetTotalUniqueDCHardWords(), 2, replacement.second.c_str() ); }
                    }
                ++m_uniqueDCHardWords;
                m_totalHardWordsDaleChall += nonProperCount;
                }
            else if (GetDaleChallProperNounCountingMethod() == readability::proper_noun_counting_method::all_proper_nouns_are_unfamiliar)
                {
                //only load the data for standard projects
                if (HasUI())
                    {
                    GetDaleChallHardWordData()->SetItemText(GetTotalUniqueDCHardWords(), 0, wordPos->first.c_str() );
                    GetDaleChallHardWordData()->SetItemValue(GetTotalUniqueDCHardWords(), 1, wordPos->second.first);
                    std::pair<bool,word_list_with_replacements::word_type> replacement = dale_chall_replacement_list.find(wordPos->first.c_str());
                    if (replacement.first)
                        { GetDaleChallHardWordData()->SetItemText(GetTotalUniqueDCHardWords(), 2, replacement.second.c_str() ); }
                    }
                ++m_uniqueDCHardWords;
                m_totalHardWordsDaleChall += wordPos->second.first;
                }
            else if (GetDaleChallProperNounCountingMethod() == readability::proper_noun_counting_method::only_count_first_instance_of_proper_noun_as_unfamiliar)
                {
                //only load the data for standard projects
                if (HasUI())
                    {
                    GetDaleChallHardWordData()->SetItemText(GetTotalUniqueDCHardWords(), 0, wordPos->first.c_str() );
                    if (wordPos->second.first == nonProperCount || wordPos->second.first == 1)
                        { GetDaleChallHardWordData()->SetItemValue(GetTotalUniqueDCHardWords(), 1,wordPos->second.first); }
                    else if (nonProperCount == 0)
                        {
                        GetDaleChallHardWordData()->SetItemText(GetTotalUniqueDCHardWords(), 1, wxString::Format(_(L"1 (%zu total occurrences, only first occurrence unfamiliar)"), wordPos->second.first),
                            NumberFormatInfo::NumberFormatType::StandardFormatting, 1);
                        }
                    else
                        {
                        GetDaleChallHardWordData()->SetItemText(GetTotalUniqueDCHardWords(), 1, wxString::Format(_(L"%zu (%zu total occurrences. First proper occurrence unfamiliar, %zu non-proper and unfamiliar)"),
                            nonProperCount+1, wordPos->second.first, nonProperCount), NumberFormatInfo::NumberFormatType::StandardFormatting, nonProperCount+1);
                        }
                    std::pair<bool,word_list_with_replacements::word_type> replacement = dale_chall_replacement_list.find(wordPos->first.c_str());
                    if (replacement.first)
                        { GetDaleChallHardWordData()->SetItemText(GetTotalUniqueDCHardWords(), 2, replacement.second.c_str() ); }
                    }
                ++m_uniqueDCHardWords;
                m_totalHardWordsDaleChall += (wordPos->second.first == nonProperCount) ? wordPos->second.first : nonProperCount+1;
                }
            }
        //go through the custom tests
        for (std::vector<CustomReadabilityTestInterface>::iterator pos = m_customTestsInUse.begin();
            pos != m_customTestsInUse.end();
            ++pos)
            {
            //if not using familiar word then skip.
            if (!pos->GetIterator()->is_using_familiar_words())
                { continue; }

            // If using CustomHJ or CustomDC (if regular DC/HJ are excluding only list items) load hard words from this buffer 
            if ((pos->IsHarrisJacobsonFormula() && GetHarrisJacobsonTextExclusionMode() == SpecializedTestTextExclusion::ExcludeIncompleteSentencesExceptHeadings) ||
                (pos->IsDaleChallFormula() && GetDaleChallTextExclusionMode() == SpecializedTestTextExclusion::ExcludeIncompleteSentencesExceptHeadings))
                {
                if (!pos->GetIterator()->is_word_familiar(wordPos->first,
                        // proper noun logic is handled separately below
                        false, pos->GetIterator()->is_including_numeric_as_familiar()))
                    {
                    if (allInstancesAreProper && pos->GetIterator()->get_proper_noun_method() == readability::proper_noun_counting_method::all_proper_nouns_are_familiar)
                        { continue; }
                    else if (pos->GetIterator()->get_proper_noun_method() == readability::proper_noun_counting_method::all_proper_nouns_are_familiar)
                        {
                        // only load the data if user asked for this view
                        if (HasUI())
                            {
                            pos->GetListViewData()->SetItemText(pos->GetUniqueUnfamiliarWordCount(), 0, wordPos->first.c_str() );
                            if (wordPos->second.first == nonProperCount)
                                { pos->GetListViewData()->SetItemValue(pos->GetUniqueUnfamiliarWordCount(), 1, nonProperCount); }
                            else
                                {
                                pos->GetListViewData()->SetItemText(pos->GetUniqueUnfamiliarWordCount(), 1, wxString::Format(_(L"%zu (%zu total occurrences, %zu proper and familiar, %zu non-proper and unfamiliar)"),
                                    nonProperCount, wordPos->second.first, (wordPos->second.first-nonProperCount), nonProperCount), NumberFormatInfo::NumberFormatType::StandardFormatting, nonProperCount);
                                }
                            }
                        pos->IncrementUnfamiliarWordCount(nonProperCount);
                        }
                    else if (pos->GetIterator()->get_proper_noun_method() == readability::proper_noun_counting_method::all_proper_nouns_are_unfamiliar)
                        {
                        // only load the data if user asked for this view
                        if (HasUI())
                            {
                            pos->GetListViewData()->SetItemText(pos->GetUniqueUnfamiliarWordCount(), 0, wordPos->first.c_str() );
                            pos->GetListViewData()->SetItemValue(pos->GetUniqueUnfamiliarWordCount(), 1,wordPos->second.first);
                            }
                        pos->IncrementUnfamiliarWordCount(wordPos->second.first);
                        }
                    else if (pos->GetIterator()->get_proper_noun_method() == readability::proper_noun_counting_method::only_count_first_instance_of_proper_noun_as_unfamiliar)
                        {
                        // only load the data if user asked for this view
                        if (HasUI())
                            {
                            pos->GetListViewData()->SetItemText(pos->GetUniqueUnfamiliarWordCount(), 0, wordPos->first.c_str() );
                            if (wordPos->second.first == nonProperCount || wordPos->second.first == 1)
                                { pos->GetListViewData()->SetItemValue(pos->GetUniqueUnfamiliarWordCount(), 1, wordPos->second.first); }
                            else if (nonProperCount == 0)
                                {
                                pos->GetListViewData()->SetItemText(pos->GetUniqueUnfamiliarWordCount(), 1, wxString::Format(_(L"1 (%zu total occurrences, only first occurrence unfamiliar)"),
                                    wordPos->second.first), NumberFormatInfo::NumberFormatType::StandardFormatting, 1);
                                }
                            else
                                {
                                pos->GetListViewData()->SetItemText(pos->GetUniqueUnfamiliarWordCount(), 1, wxString::Format(_(L"%zu (%zu total occurrences. First proper occurrence unfamiliar, %zu non-proper and unfamiliar)"),
                                    nonProperCount+1, wordPos->second.first, nonProperCount), NumberFormatInfo::NumberFormatType::StandardFormatting, nonProperCount+1);
                                }
                            }
                        pos->IncrementUnfamiliarWordCount((wordPos->second.first == nonProperCount) ? wordPos->second.first : nonProperCount+1);
                        }
                    pos->IncrementUniqueUnfamiliarWordCount();
                    }
                }
            }
        }

    //finalize the size of the vectors
    if (HasUI())
        {
        if (GetProperNounsData())
            {
            GetProperNounsData()->SetSize(uniqueProperNouns);
            GetProperNounsData()->ShrinkToFit();
            }
        if (GetContractionsData())
            {
            GetContractionsData()->SetSize(uniqueContractions);
            GetContractionsData()->ShrinkToFit();
            }
        if (GetImportantWordsBaseData())
            {
            GetImportantWordsBaseData()->SetSize(uniqueImportWordsCount);
            GetImportantWordsBaseData()->ShrinkToFit();
            }
        if (Get3SyllablePlusData())
            {
            Get3SyllablePlusData()->SetSize(GetTotalUnique3PlusSyllableWords());
            Get3SyllablePlusData()->ShrinkToFit();
            }
        if (Get6CharacterPlusData())
            {
            Get6CharacterPlusData()->SetSize(GetTotalUnique6CharsPlusWords());
            Get6CharacterPlusData()->ShrinkToFit();
            }
        if (GetDaleChallHardWordData())
            {
            GetDaleChallHardWordData()->SetSize(GetTotalUniqueDCHardWords());
            GetDaleChallHardWordData()->ShrinkToFit();
            }
        if (GetSpacheHardWordData())
            {
            GetSpacheHardWordData()->SetSize(GetTotalUniqueHardWordsSpache());
            GetSpacheHardWordData()->ShrinkToFit();
            }
        if (GetHarrisJacobsonHardWordDataData())
            {
            GetHarrisJacobsonHardWordDataData()->SetSize(GetTotalUniqueHarrisJacobsonHardWords());
            GetHarrisJacobsonHardWordDataData()->ShrinkToFit();
            }
        if (GetUnusedDolchWordData())//already the right size
            { GetUnusedDolchWordData()->ShrinkToFit(); }
        if (GetDolchWordData())
            {
            GetDolchWordData()->SetSize(uniqueDolchWords);
            GetDolchWordData()->ShrinkToFit();
            }
        if (GetNonDolchWordData())
            {
            GetNonDolchWordData()->SetSize(nonUniqueDolchWords);
            GetNonDolchWordData()->ShrinkToFit();
            }
        }
    }

//-------------------------------------------------------
void BaseProject::CalculateStatisticsIgnoringInvalidSentences()
    {
    m_totalWords = GetWords()->get_valid_word_count();
    m_totalSentences = GetWords()->get_complete_sentence_count();
    m_totalParagraphs = GetWords()->get_valid_paragraph_count();

    //Number of total syllables
    m_totalSyllables = std::accumulate(GetWords()->get_words().begin(),
                    GetWords()->get_words().end(), static_cast<size_t>(0),
                    add_valid_syllable_size<word_case_insensitive_no_stem>(GetNumeralSyllabicationMethod() == NumeralSyllabize::WholeWordIsOneSyllable) );
    //Number of syllables, numerals just one syllable
    m_totalSyllablesNumeralsOneSyllable = std::accumulate(GetWords()->get_words().begin(),
                    GetWords()->get_words().end(), static_cast<size_t>(0),
                    add_valid_syllable_size<word_case_insensitive_no_stem>(true) );
    //Number of syllables, numerals fully syllabized
    m_totalSyllablesNumeralsFullySyllabized = std::accumulate(GetWords()->get_words().begin(),
                    GetWords()->get_words().end(), static_cast<size_t>(0),
                    add_valid_syllable_size<word_case_insensitive_no_stem>(false) );
    //Number of syllables, ignoring numeric strings
    m_totalSyllablesIgnoringNumerals = std::accumulate(GetWords()->get_words().begin(),
                    GetWords()->get_words().end(), static_cast<size_t>(0),
                    add_valid_syllable_size_ignore_numerals<word_case_insensitive_no_stem>() );
    //Number of syllables, ignoring numeric and proper strings
    m_totalSyllablesIgnoringNumeralsAndProperNouns = std::accumulate(GetWords()->get_words().begin(),
                    GetWords()->get_words().end(), static_cast<size_t>(0),
                    add_valid_syllable_size_ignore_numerals_and_proper_nouns<word_case_insensitive_no_stem>() );
    //Number of total characters
    m_totalCharacters = std::accumulate(GetWords()->get_words().begin(),
                    GetWords()->get_words().end(), static_cast<size_t>(0),
                    add_valid_word_size_excluding_punctuation<word_case_insensitive_no_stem>() );
    //Number of total characters and punctuation (this includes punctuation that is part of the words and between them)
    m_totalCharactersPlusPunctuation = std::accumulate(GetWords()->get_words().begin(),
                    GetWords()->get_words().end(), static_cast<size_t>(0),
                    add_valid_word_size<word_case_insensitive_no_stem>() ) + GetWords()->get_valid_punctuation_count();
    //Monosyllabic words
    m_totalMonoSyllabic = std::count_if(GetWords()->get_words().begin(),
                                    GetWords()->get_words().end(),
                                    valid_syllable_count_equals<word_case_insensitive_no_stem>(1, GetNumeralSyllabicationMethod() == NumeralSyllabize::WholeWordIsOneSyllable) );
    //Numerals
    m_totalNumerals = std::count_if(GetWords()->get_words().begin(),
                                    GetWords()->get_words().end(),
                                    is_valid_numeric<word_case_insensitive_no_stem>() );
    //proper nouns
    m_totalProperNouns = std::count_if(GetWords()->get_words().begin(),
                                    GetWords()->get_words().end(),
                                    is_valid_proper_noun<word_case_insensitive_no_stem>() );
    //hard lix/rix words
    m_totalHardWordsLixRix = std::count_if(GetWords()->get_words().begin(),
                                    GetWords()->get_words().end(),
                                    valid_word_length_excluding_punctuation_greater_equals<word_case_insensitive_no_stem>(7) );

    //load the unique words and their frequencies.
    m_word_frequency_map = new double_frequency_set<word_case_insensitive_no_stem>;
    for (const auto& word : GetWords()->get_words())
        {
        if (word.is_valid())
            {
            //go through the words and add them to the map
            m_word_frequency_map->insert(word, word.is_proper_noun());
            }
        }

    //difficult sentences
    if (m_longSentenceMethod == LongSentence::OutlierLength)
        {
        if (GetTotalSentences() > 0)
            {
            //if to be calculated by outlier range
            std::vector<double> statistic_sizes(GetTotalSentences() );

            //load a vector of the sentence lengths
            copy_member_if(GetWords()->get_sentences().begin(),
                GetWords()->get_sentences().end(),
                statistic_sizes.begin(),
                [](const grammar::sentence_info& s) noexcept { return s.is_valid(); },
                [](const grammar::sentence_info& s) noexcept { return s.get_valid_word_count(); } );
            statistics::find_outliers fos(statistic_sizes);
            m_difficultSentenceLength = static_cast<int>(fos.get_upper_outlier_boundary());
            }
        else
            {
            LogMessage(_(L"No sentences found. Unable to calculate sentence length outlier range."),
                _(L"Warning"), wxOK|wxICON_EXCLAMATION);
            m_difficultSentenceLength = 0;
            }
        }

    m_totalInterrogativeSentences = m_totalExclamatorySentences = m_totalSentenceUnits = 0;
    if (GetTotalSentences() > 0)
        {
        // Number of overly long sentences
        m_totalOverlyLongSentences = std::count_if(GetWords()->get_sentences().begin(),
            GetWords()->get_sentences().end(),
            grammar::complete_sentence_length_greater_than(m_difficultSentenceLength) );

        for (size_t i = 0; i < GetWords()->get_sentences().size(); ++i)
            {
            if (GetWords()->get_sentences()[i].is_valid())
                {
                const wchar_t endingChar = GetWords()->get_sentences()[i].get_ending_punctuation();
                if (endingChar == common_lang_constants::QUESTION_MARK ||
                    endingChar == common_lang_constants::QUESTION_MARK_FULL_WIDTH)
                    { ++m_totalInterrogativeSentences; }
                else if (endingChar == common_lang_constants::EXCLAMATION_MARK ||
                    endingChar == common_lang_constants::EXCLAMATION_MARK_FULL_WIDTH)
                    { ++m_totalExclamatorySentences; }
                else if (endingChar == common_lang_constants::INTERROBANG)
                    { ++m_totalExclamatorySentences; }
                // count the sentence units
                m_totalSentenceUnits += GetWords()->get_sentences()[i].get_unit_count();
                }
            }

        std::vector<grammar::sentence_info>::const_iterator longestSent = 
            std::max_element(GetWords()->get_sentences().begin(),
            GetWords()->get_sentences().end(),
            grammar::complete_sentence_length_less());
        m_longestSentence = longestSent->get_valid_word_count();
        // be sure to add 1 to make it one-indexed when being displayed
        m_longestSentenceIndex = (longestSent - GetWords()->get_sentences().begin());
        }
    // No valid sentences? Just reset these values then.
    else
        {
        m_totalSentenceUnits = 0;
        m_totalOverlyLongSentences = 0;
        m_longestSentence = 0;
        m_longestSentenceIndex = 0;
        }

    // we count these in ignored sentences too because they are just
    // general grammar issues to improve the overall document
    SetDuplicateWordCount(GetWords()->get_duplicate_word_indices().size());
    SetMismatchedArticleCount(GetWords()->get_incorrect_article_indices().size());
    SetPassiveVoicesCount(GetWords()->get_passive_voice_indices().size());
    SetMisspelledWordCount(GetWords()->get_misspelled_words().size());
    SetOverusedWordsBySentenceCount(GetWords()->get_overused_words_by_sentence().size());

    m_wordyPhraseCount = m_redundantPhraseCount = m_wordingErrorCount = m_clicheCount = 0;
    const auto& wordyIndices = GetWords()->get_known_phrase_indices();
    const auto& wordyPhrases =
        GetWords()->get_known_phrases().get_phrases();
    for (size_t i = 0; i < wordyIndices.size(); ++i)
        {
        switch (wordyPhrases[wordyIndices[i].second].first.get_type())
            {
        case grammar::phrase_type::phrase_wordy:
            ++m_wordyPhraseCount;
            break;
        case grammar::phrase_type::phrase_redundant:
            ++m_redundantPhraseCount;
            break;
        case grammar::phrase_type::phrase_cliche:
            ++m_clicheCount;
            break;
        case grammar::phrase_type::phrase_error:
            ++m_wordingErrorCount;
            break;
            };
        }

    m_sentenceStartingWithConjunctionsCount = GetWords()->get_conjunction_beginning_sentences().size();
    m_sentenceStartingWithLowercaseCount = GetWords()->get_lowercase_beginning_sentences().size();

    ReviewStatGoals();
    }

//-------------------------------------------------------
void BaseProject::CalculateStatistics()
    {
    m_totalWords = GetWords()->get_word_count();
    m_totalSentences = GetWords()->get_sentence_count();
    m_totalParagraphs = GetWords()->get_paragraph_count();

    //Number of total syllables
    m_totalSyllables = std::accumulate(GetWords()->get_words().begin(),
                    GetWords()->get_words().end(), static_cast<size_t>(0),
                    add_syllable_size<word_case_insensitive_no_stem>(GetNumeralSyllabicationMethod() == NumeralSyllabize::WholeWordIsOneSyllable) );
    //Number of syllables, numerals just one syllable
    m_totalSyllablesNumeralsOneSyllable = std::accumulate(GetWords()->get_words().begin(),
                    GetWords()->get_words().end(), static_cast<size_t>(0),
                    add_syllable_size<word_case_insensitive_no_stem>(true) );
    //Number of syllables, numerals fully syllabized
    m_totalSyllablesNumeralsFullySyllabized = std::accumulate(GetWords()->get_words().begin(),
                    GetWords()->get_words().end(), static_cast<size_t>(0),
                    add_syllable_size<word_case_insensitive_no_stem>(false) );
    //Number of syllables, ignoring numeric strings
    m_totalSyllablesIgnoringNumerals = std::accumulate(GetWords()->get_words().begin(),
                    GetWords()->get_words().end(), static_cast<size_t>(0),
                    add_syllable_size_ignore_numerals<word_case_insensitive_no_stem>() );
    //Number of syllables, ignoring numeric and proper strings
    m_totalSyllablesIgnoringNumeralsAndProperNouns = std::accumulate(GetWords()->get_words().begin(),
                    GetWords()->get_words().end(), static_cast<size_t>(0),
                    add_syllable_size_ignore_numerals_and_proper_nouns<word_case_insensitive_no_stem>() );
    //Number of total characters
    m_totalCharacters = std::accumulate(GetWords()->get_words().begin(),
                    GetWords()->get_words().end(), static_cast<size_t>(0),
                    add_word_size_excluding_punctuation<word_case_insensitive_no_stem>() );
    //Number of total characters and punctuation (this includes punctuation that is part of the words and between them)
    m_totalCharactersPlusPunctuation = std::accumulate(GetWords()->get_words().begin(),
                    GetWords()->get_words().end(), static_cast<size_t>(0),
                    add_word_size<word_case_insensitive_no_stem>() ) + GetWords()->get_punctuation_count();
    //Monosyllabic words
    m_totalMonoSyllabic = std::count_if(GetWords()->get_words().begin(),
                                    GetWords()->get_words().end(),
                                    syllable_count_equals<word_case_insensitive_no_stem>(1, GetNumeralSyllabicationMethod() == NumeralSyllabize::WholeWordIsOneSyllable) );
    //Numerals
    m_totalNumerals = std::count_if(GetWords()->get_words().begin(),
                                    GetWords()->get_words().end(),
                                    is_numeric<word_case_insensitive_no_stem>() );
    //proper nouns
    m_totalProperNouns = std::count_if(GetWords()->get_words().begin(),
                                    GetWords()->get_words().end(),
                                    is_proper_noun<word_case_insensitive_no_stem>() );
    //hard lix/rix words
    m_totalHardWordsLixRix = std::count_if(GetWords()->get_words().begin(),
                                    GetWords()->get_words().end(),
                                    word_length_excluding_punctuation_greater_equals<word_case_insensitive_no_stem>(7) );

    //load the unique words and their frequencies
    m_word_frequency_map = new double_frequency_set<word_case_insensitive_no_stem>;
    std::vector<word_case_insensitive_no_stem>::const_iterator currentWord;
    for (std::vector<grammar::sentence_info>::const_iterator sentPos = GetWords()->get_sentences().begin();
        sentPos != GetWords()->get_sentences().end();
        ++sentPos)
        {
        // go through the words in the current sentence and add them to the map
        for (size_t i = sentPos->get_first_word_index();
             i <= sentPos->get_last_word_index();
             ++i)
            {
            currentWord = GetWords()->get_words().begin()+i;
            m_word_frequency_map->insert(*currentWord, currentWord->is_proper_noun());
            }
        }

    //difficult sentences
    if (m_longSentenceMethod == LongSentence::OutlierLength)
        {
        if (GetTotalSentences() > 0)
            {
            //if to be calculated by outlier range
            std::vector<double> statistic_sizes(GetTotalSentences() );

            //load a vector of the sentence lengths
            copy_member(GetWords()->get_sentences().begin(),
                GetWords()->get_sentences().end(),
                statistic_sizes.begin(),
                [](const grammar::sentence_info& s) noexcept { return s.get_word_count(); } );
            statistics::find_outliers fos(statistic_sizes);
            m_difficultSentenceLength = static_cast<int>(fos.get_upper_outlier_boundary());
            }
        else
            {
            LogMessage(_(L"No sentences found. Unable to calculate sentence length outlier range."),
                _(L"Warning"), wxOK|wxICON_EXCLAMATION);
            m_difficultSentenceLength = 0;
            }
        }

    m_totalInterrogativeSentences = m_totalExclamatorySentences = m_totalSentenceUnits = 0;
    if (GetTotalSentences() > 0)
        {
        // Number of overly long sentences
        m_totalOverlyLongSentences = std::count_if(GetWords()->get_sentences().begin(),
            GetWords()->get_sentences().end(),
            grammar::sentence_length_greater_than(m_difficultSentenceLength) );

        for (size_t i = 0; i < GetWords()->get_sentences().size(); ++i)
            {
            const wchar_t endingChar = GetWords()->get_sentences()[i].get_ending_punctuation();
            if (endingChar == common_lang_constants::QUESTION_MARK || endingChar == common_lang_constants::QUESTION_MARK_FULL_WIDTH)
                { ++m_totalInterrogativeSentences; }
            else if (endingChar == common_lang_constants::EXCLAMATION_MARK || endingChar == common_lang_constants::EXCLAMATION_MARK_FULL_WIDTH)
                { ++m_totalExclamatorySentences; }
            else if (endingChar == common_lang_constants::INTERROBANG)
                { ++m_totalExclamatorySentences; }
            // count the sentence units
            m_totalSentenceUnits += GetWords()->get_sentences()[i].get_unit_count();
            }

        std::vector<grammar::sentence_info>::const_iterator longestSent =
            std::max_element(GetWords()->get_sentences().begin(),
            GetWords()->get_sentences().end() );
        m_longestSentence = longestSent->get_word_count();
        // be sure to add 1 to make it one-indexed when being displayed
        m_longestSentenceIndex = (longestSent - GetWords()->get_sentences().begin());
        }
    // No valid sentences? Just reset these values then.
    else
        {
        m_totalSentenceUnits = 0;
        m_totalOverlyLongSentences = 0;
        m_longestSentence = 0;
        m_longestSentenceIndex = 0;
        }

    SetDuplicateWordCount(GetWords()->get_duplicate_word_indices().size());
    SetMismatchedArticleCount(GetWords()->get_incorrect_article_indices().size());
    SetPassiveVoicesCount(GetWords()->get_passive_voice_indices().size());
    SetMisspelledWordCount(GetWords()->get_misspelled_words().size());
    SetOverusedWordsBySentenceCount(GetWords()->get_overused_words_by_sentence().size());

    m_wordyPhraseCount = m_redundantPhraseCount = m_wordingErrorCount = m_clicheCount = 0;
    const auto& wordyIndices = GetWords()->get_known_phrase_indices();
    const auto& wordyPhrases =
        GetWords()->get_known_phrases().get_phrases();
    for (size_t i = 0; i < wordyIndices.size(); ++i)
        {
        switch (wordyPhrases[wordyIndices[i].second].first.get_type())
            {
        case grammar::phrase_type::phrase_wordy:
            ++m_wordyPhraseCount;
            break;
        case grammar::phrase_type::phrase_redundant:
            ++m_redundantPhraseCount;
            break;
        case grammar::phrase_type::phrase_cliche:
            ++m_clicheCount;
            break;
        case grammar::phrase_type::phrase_error:
            ++m_wordingErrorCount;
            break;
            };
        }

    m_sentenceStartingWithConjunctionsCount = GetWords()->get_conjunction_beginning_sentences().size();
    m_sentenceStartingWithLowercaseCount = GetWords()->get_lowercase_beginning_sentences().size();

    ReviewStatGoals();
    }

//------------------------------------------------
std::pair<bool,wxString> BaseProject::ExtractRawTextWithEncoding(const wxString& sourceFileText,
                                             const wxString& fileExtension, const wxFileName& fileName, wxString& label)
    {
    lily_of_the_valley::html_extract_text filter_html;
    filter_html.set_log_message_separator(L", ");
    const WebPageExtension isHtmlExtension;

    try
        {
        if (isHtmlExtension(fileExtension))
            {
            const wxString filteredText(filter_html(sourceFileText, sourceFileText.length(), true, false));
            if (filter_html.get_log().length())
                { wxLogWarning(L"%s: %s", fileName.GetFullPath(), filter_html.get_log()); }
            label = coalesce<wchar_t>(
                        { filter_html.get_subject(),
                          filter_html.get_title(),
                          filter_html.get_keywords(),
                          filter_html.get_description(),
                          filter_html.get_author() }
                        );
            return std::make_pair(true, filteredText);
            }
        else
            { return std::make_pair(true, sourceFileText); }
        }
    catch (...)
        {
        wxLogWarning(L"An unknown error occurred while importing %s.", fileName.GetFullPath());
        return std::make_pair(false, wxEmptyString);
        }
    }

//------------------------------------------------
std::pair<bool,wxString> BaseProject::ExtractRawText(const char* sourceFileText,
                                 const size_t streamSize,
                                 const wxString& fileExtension)
    {
    PROFILE();
    if (sourceFileText == nullptr || streamSize == 0)
        { return std::make_pair(false, wxEmptyString); }
    wxLogMessage(wxString::Format(L"%s parser being called.", fileExtension.Upper()));

    const WebPageExtension isHtmlExtension;

    if (fileExtension.CmpNoCase(L"rtf") == 0)
        {
        lily_of_the_valley::rtf_extract_text filter_rtf;
        filter_rtf.set_log_message_separator(L", ");
        try
            {
            const wxString filteredText(filter_rtf(sourceFileText, streamSize));
            if (filter_rtf.get_log().length())
                { wxLogWarning(L"%s: %s", GetOriginalDocumentFilePath(), filter_rtf.get_log()); }
            SetOriginalDocumentDescription(
                coalesce<wchar_t>(
                        { GetOriginalDocumentDescription().wc_str(),
                          filter_rtf.get_subject(),
                          filter_rtf.get_title(),
                          filter_rtf.get_keywords(),
                          filter_rtf.get_comments(),
                          filter_rtf.get_author(),
                          wxFileName(GetOriginalDocumentFilePath()).GetName().wc_str() }
                        ));
            return std::make_pair(true,filteredText);
            }
        catch (rtf_extract_text::rtfparse_stack_underflow)
            {
            LogMessage(_(L"Unable to import RTF file. Stack underflow."),
                _(L"Import Error"), wxOK|wxICON_EXCLAMATION);
            return std::make_pair(false, wxEmptyString);
            }
        catch (rtf_extract_text::rtfparse_stack_overflow)
            {
            LogMessage(_(L"Unable to import RTF file. Stack overflow."),
                _(L"Import Error"), wxOK|wxICON_EXCLAMATION);
            return std::make_pair(false, wxEmptyString);
            }
        catch (rtf_extract_text::rtfparse_unmatched_brace)
            {
            LogMessage(_(L"Unable to import RTF file. Unmatched brace."),
                _(L"Import Error"), wxOK|wxICON_EXCLAMATION);
            return std::make_pair(false, wxEmptyString);
            }
        catch (rtf_extract_text::rtfparse_assertion)
            {
            LogMessage(_(L"Unable to import RTF file. Assertion."),
                _(L"Import Error"), wxOK|wxICON_EXCLAMATION);
            return std::make_pair(false, wxEmptyString);
            }
        catch (rtf_extract_text::rtfparse_invalid_hex)
            {
            LogMessage(_(L"Unable to import RTF file. Invalid hex value."),
                _(L"Import Error"), wxOK|wxICON_EXCLAMATION);
            return std::make_pair(false, wxEmptyString);
            }
        catch (rtf_extract_text::rtfparse_bad_table)
            {
            LogMessage(_(L"Unable to import RTF file. Bad table."),
                _(L"Import Error"), wxOK|wxICON_EXCLAMATION);
            return std::make_pair(false, wxEmptyString);
            }
        catch (...)
            {
            LogMessage(_(L"An unknown error occurred while importing. Unable to continue creating project."),
                _(L"Import Error"), wxOK|wxICON_EXCLAMATION);
            return std::make_pair(false, wxEmptyString);
            }
        }
    else if (fileExtension.CmpNoCase(L"doc") == 0 ||
             fileExtension.CmpNoCase(L"dot") == 0)
        {
        word1997_extract_text filter_word; // Word 97-2003
        filter_word.set_log_message_separator(L", ");
        try
            {
            const wxString filteredText(filter_word(sourceFileText, streamSize));
            if (filter_word.get_log().length())
                { wxLogWarning(L"%s: %s", GetOriginalDocumentFilePath(), filter_word.get_log()); }
            SetOriginalDocumentDescription(
                coalesce<wchar_t>(
                        { GetOriginalDocumentDescription().wc_str(),
                          filter_word.get_subject(),
                          filter_word.get_title(),
                          filter_word.get_keywords(),
                          filter_word.get_comments(),
                          filter_word.get_author(),
                          wxFileName(GetOriginalDocumentFilePath()).GetName().wc_str() }
                        ));
            return std::make_pair(true,filteredText);
            }
        catch (rtf_extract_text::rtfparse_exception)
            {
            LogMessage(_(L"Invalid RTF file."),
                _(L"Import Error"), wxOK|wxICON_EXCLAMATION);
            return std::make_pair(false, wxEmptyString);
            }
        catch (word1997_extract_text::cfb_bad_bat)
            {
            LogMessage(_(L"Word file is corrupted and cannot be read."),
                _(L"Import Error"), wxOK|wxICON_EXCLAMATION);
            return std::make_pair(false, wxEmptyString);
            }
        catch (word1997_extract_text::cfb_bad_bat_entry)
            {
            LogMessage(_(L"Word file is corrupted and cannot be read."),
                _(L"Import Error"), wxOK|wxICON_EXCLAMATION);
            return std::make_pair(false, wxEmptyString);
            }
        catch (word1997_extract_text::cfb_bad_xbat)
            {
            LogMessage(_(L"Word file is corrupted and cannot be read."),
                _(L"Import Error"), wxOK|wxICON_EXCLAMATION);
            return std::make_pair(false, wxEmptyString);
            }
        catch (word1997_extract_text::cfb_bad_xbat_entry)
            {
            LogMessage(_(L"Word file is corrupted and cannot be read."),
                _(L"Import Error"), wxOK|wxICON_EXCLAMATION);
            return std::make_pair(false, wxEmptyString);
            }
        catch (word1997_extract_text::msword_encrypted)
            {
            LogMessage(_(L"Word file is encrypted and cannot be read."),
                _(L"Import Error"), wxOK|wxICON_EXCLAMATION);
            return std::make_pair(false, wxEmptyString);
            }
        catch (word1997_extract_text::msword_corrupted)
            {
            LogMessage(_(L"Word file is corrupted and cannot be read."),
                _(L"Import Error"), wxOK|wxICON_EXCLAMATION);
            return std::make_pair(false, wxEmptyString);
            }
        catch (word1997_extract_text::msword_fastsaved)
            {
            LogMessage(_(L"Word file is fast-saved and could not be read.\nPlease save file with fast-saving turned off and try again."),
                _(L"Import Error"), wxOK|wxICON_EXCLAMATION);
            return std::make_pair(false, wxEmptyString);
            }
        catch (word1997_extract_text::msword_header_not_found)
            {
            LogMessage(_(L"File does not appear to be a valid Word file."),
                _(L"Import Error"), wxOK|wxICON_EXCLAMATION);
            return std::make_pair(false, wxEmptyString);
            }
        catch (word1997_extract_text::msword_root_enrty_not_found)
            {
            LogMessage(_(L"File does not appear to be a valid Word file."),
                _(L"Import Error"), wxOK|wxICON_EXCLAMATION);
            return std::make_pair(false, wxEmptyString);
            }
        catch (...)
            {
            LogMessage(_(L"An unknown error occurred while importing. Unable to continue creating project."),
                _(L"Import Error"), wxOK|wxICON_EXCLAMATION);
            return std::make_pair(false, wxEmptyString);
            }
        }
    else if (fileExtension.CmpNoCase(L"docx") == 0 ||
             fileExtension.CmpNoCase(L"docm") == 0)
        {
        word2007_extract_text filter_docx;//Word 2007
        filter_docx.set_log_message_separator(L", ");
        Wisteria::ZipCatalog archive(sourceFileText, streamSize);
        const wxString docxMetaFileText = archive.ReadTextFile(L"docProps/core.xml");
        if (docxMetaFileText.length())
            {
            filter_docx.read_meta_data(docxMetaFileText, docxMetaFileText.length());
            SetOriginalDocumentDescription(
                coalesce<wchar_t>(
                        { GetOriginalDocumentDescription().wc_str(),
                          filter_docx.get_subject(),
                          filter_docx.get_title(),
                          filter_docx.get_keywords(),
                          filter_docx.get_description(),
                          filter_docx.get_author(),
                          wxFileName(GetOriginalDocumentFilePath()).GetName().wc_str() }
                        ));
            }
        if (archive.Find(L"word/document.xml") == nullptr)
            {
            LogMessage(_(L"Unable to open Word document, file is either password-protected or corrupt."), wxGetApp().GetAppDisplayName(), wxICON_EXCLAMATION|wxOK);
            return std::make_pair(false, wxEmptyString);
            }
        const wxString docxFileText = archive.ReadTextFile(L"word/document.xml");
        try
            {
            const wxString filteredText(filter_docx(docxFileText.wc_str(), docxFileText.length()));
            if (filter_docx.get_log().length())
                { wxLogWarning(L"%s: %s", GetOriginalDocumentFilePath(), filter_docx.get_log()); }
            return std::make_pair(true,filteredText);
            }
        catch (...)
            {
            LogMessage(_(L"An unknown error occurred while importing. Unable to continue creating project."),
                _(L"Import Error"), wxOK|wxICON_EXCLAMATION);
            return std::make_pair(false, wxEmptyString);
            }
        }
    else if (fileExtension.CmpNoCase(L"hhc") == 0 ||
             fileExtension.CmpNoCase(L"hhk") == 0)
        {
        lily_of_the_valley::hhc_hhk_extract_text filter_hhc_hhk;
        filter_hhc_hhk.set_log_message_separator(L", ");
        try
            {
            const wxString hhStr = Wisteria::TextStream::CharStreamToUnicode(sourceFileText, streamSize);
            const wxString filteredText(filter_hhc_hhk(hhStr, hhStr.length()));
            if (filter_hhc_hhk.get_log().length())
                { wxLogWarning(L"%s: %s", GetOriginalDocumentFilePath(), filter_hhc_hhk.get_log()); }
            SetOriginalDocumentDescription(
                coalesce({ GetOriginalDocumentDescription(),
                           wxFileName(GetOriginalDocumentFilePath()).GetName() }
                        ));
            return std::make_pair(true,filteredText);
            }
        catch (...)
            {
            LogMessage(_(L"An unknown error occurred while importing. Unable to continue creating project."),
                _(L"Import Error"), wxOK|wxICON_EXCLAMATION);
            return std::make_pair(false, wxEmptyString);
            }
        }
    else if (isHtmlExtension(fileExtension))
        {
        std::pair<bool,wxString> extractResult(false,wxEmptyString);
        wxString label;
        //if UTF-8 or simply 7-bit ASCII, then just convert as UTF-8 and run the HTML parser on it
        if (utf8::is_valid(sourceFileText,sourceFileText+streamSize))
            { extractResult = ExtractRawTextWithEncoding(Wisteria::TextStream::CharStreamToUnicode(sourceFileText, streamSize), L"html", GetOriginalDocumentFilePath(), label); }
        //Otherwise, need to search for the encoding in the HTML itself and convert using that, then run the HTML parser on it
        else
            { extractResult = ExtractRawTextWithEncoding(Wisteria::TextStream::CharStreamToUnicode(sourceFileText, streamSize, WebHarvester::GetCharsetFromPageContent(sourceFileText, streamSize)), L"html", GetOriginalDocumentFilePath(), label); }
        if (!extractResult.first)
            {
            LogMessage(_(L"An unknown error occurred while importing. Unable to continue creating project."),
                _(L"Import Error"), wxOK|wxICON_EXCLAMATION);
            return std::make_pair(false, wxEmptyString);
            }
        // set the description to the title (unless user specified a description already, e.g., from a batch project).
        SetOriginalDocumentDescription(
            coalesce({
                     GetOriginalDocumentDescription(),
                     label,
                     wxFileName(GetOriginalDocumentFilePath()).GetName()
                     }));
        return std::make_pair(true,extractResult.second);
        }
    else if (fileExtension.CmpNoCase(L"ps") == 0)
        {
        lily_of_the_valley::postscript_extract_text filter_ps;
        filter_ps.set_log_message_separator(L", ");
        try
            {
            const wxString filteredText(filter_ps(sourceFileText, streamSize));
            if (filter_ps.get_log().length())
                { wxLogWarning(L"%s: %s", GetOriginalDocumentFilePath(), filter_ps.get_log()); }
            SetOriginalDocumentDescription(
                coalesce({ GetOriginalDocumentDescription(),
                           wxString(filter_ps.get_title()),
                           wxFileName(GetOriginalDocumentFilePath()).GetName() }
                        ));
            return std::make_pair(true,filteredText);
            }
        catch (lily_of_the_valley::postscript_extract_text::postscript_header_not_found)
            {
            LogMessage(_(L"File does not appear to be a valid Postscript file."),
                _(L"Import Error"), wxOK|wxICON_EXCLAMATION);
            return std::make_pair(false, wxEmptyString);
            }
        catch (lily_of_the_valley::postscript_extract_text::postscript_version_not_supported)
            {
            LogMessage(_(L"Only PostScript versions 1-2 are supported. Unable to import file."),
                _(L"Import Error"), wxOK|wxICON_EXCLAMATION);
            return std::make_pair(false, wxEmptyString);
            }
        catch (...)
            {
            LogMessage(_(L"An unknown error occurred while importing. Unable to continue creating project."),
                _(L"Import Error"), wxOK|wxICON_EXCLAMATION);
            return std::make_pair(false, wxEmptyString);
            }
        }
    //OpenDocument text or presentation files
    else if (fileExtension.CmpNoCase(L"odt") == 0 ||
             fileExtension.CmpNoCase(L"ott") == 0 ||
             fileExtension.CmpNoCase(L"odp") == 0 ||
             fileExtension.CmpNoCase(L"otp") == 0)
        {
        lily_of_the_valley::odt_odp_extract_text filter_odt;
        filter_odt.set_log_message_separator(L", ");
        Wisteria::ZipCatalog archive(sourceFileText, streamSize);
        const wxString odtMetaFileText = archive.ReadTextFile(L"meta.xml");
        if (odtMetaFileText.length())
            {
            filter_odt.read_meta_data(odtMetaFileText, odtMetaFileText.length());
            SetOriginalDocumentDescription(
                coalesce<wchar_t>(
                        { GetOriginalDocumentDescription().wc_str(),
                          filter_odt.get_subject(),
                          filter_odt.get_title(),
                          filter_odt.get_keywords(),
                          filter_odt.get_description(),
                          filter_odt.get_author(),
                          wxFileName(GetOriginalDocumentFilePath()).GetName().wc_str() }
                        ));
            }
        const wxString odtFileText = archive.ReadTextFile(L"content.xml");
        if (odtFileText.empty() && archive.GetMessages().size())
            {
            LogMessage(wxString::Format(_(L"Unable to open ODT/ODP document: %s"), archive.GetMessages().at(0).m_message), wxGetApp().GetAppDisplayName(), wxICON_EXCLAMATION|wxOK);
            return std::make_pair(false, wxEmptyString);
            }
        try
            {
            const wxString filteredText(filter_odt(odtFileText, odtFileText.length()));
            if (filter_odt.get_log().length())
                { wxLogWarning(L"%s: %s", GetOriginalDocumentFilePath(), filter_odt.get_log()); }
            return std::make_pair(true,filteredText);
            }
        catch (...)
            {
            LogMessage(_(L"An unknown error occurred while importing. Unable to continue creating project."),
                _(L"Import Error"), wxOK|wxICON_EXCLAMATION);
            return std::make_pair(false, wxEmptyString);
            }
        }
    else if (fileExtension.CmpNoCase(L"pptx") == 0 ||
             fileExtension.CmpNoCase(L"pptm") == 0)
        {
        lily_of_the_valley::pptx_extract_text filter_pptx;//PowerPoint 2007
        filter_pptx.set_log_message_separator(L", ");
        wxString pptParsedText;
        Wisteria::ZipCatalog archive(sourceFileText, streamSize);
        const wxString pptMetaFileText = archive.ReadTextFile(L"docProps/core.xml");
        if (pptMetaFileText.length())
            {
            filter_pptx.read_meta_data(pptMetaFileText, pptMetaFileText.length());
            SetOriginalDocumentDescription(
                coalesce<wchar_t>(
                        { GetOriginalDocumentDescription().wc_str(),
                          filter_pptx.get_subject(),
                          filter_pptx.get_title(),
                          filter_pptx.get_keywords(),
                          filter_pptx.get_description(),
                          filter_pptx.get_author(),
                          wxFileName(GetOriginalDocumentFilePath()).GetName().wc_str() }
                        ));
            }
        if (archive.Find(L"ppt/slides/slide1.xml") == nullptr)
            {
            LogMessage(_(L"Unable to open PowerPoint document, file is either password-protected or corrupt."), wxGetApp().GetAppDisplayName(), wxICON_EXCLAMATION|wxOK);
            return std::make_pair(false, wxEmptyString);
            }
        for (size_t i = 1;/*breaks when no more pages are found*/;++i)
            {
            if (archive.Find(wxString::Format(L"ppt/slides/slide%zu.xml", i)) )
                {
                const wxString pptxFileText = archive.ReadTextFile(wxString::Format(L"ppt/slides/slide%zu.xml",i));
                try
                    {
                    pptParsedText += filter_pptx(pptxFileText.wc_str(), pptxFileText.length());
                    pptParsedText += L"\n";
                    if (filter_pptx.get_log().length())
                        { wxLogWarning(L"%s: %s", GetOriginalDocumentFilePath(), filter_pptx.get_log()); }
                    }
                catch (...)
                    {
                    LogMessage(_(L"An unknown error occurred while importing. Unable to continue creating project."),
                        _(L"Import Error"), wxOK|wxICON_EXCLAMATION);
                    return std::make_pair(false, wxEmptyString);
                    }
                }
            else
                { break; }
            }
        return std::make_pair(true, pptParsedText);
        }
    else if (fileExtension.CmpNoCase(L"idl") == 0)
        {
        SpellCheckIgnoreProgrammerCode(true);//override user settings, we would want to ignore code here
        lily_of_the_valley::idl_extract_text filter_idl;
        const wxString unicodeStr = Wisteria::TextStream::CharStreamToUnicode(sourceFileText, streamSize);
        SetOriginalDocumentDescription(
            coalesce({
                     GetOriginalDocumentDescription(),
                     wxFileName(GetOriginalDocumentFilePath()).GetName()
                     }));
        return std::make_pair(true, filter_idl({ unicodeStr, unicodeStr.length() }));
        }
    else if (fileExtension.CmpNoCase(L"cpp") == 0 ||
        fileExtension.CmpNoCase(L"c") == 0 ||
        fileExtension.CmpNoCase(L"h") == 0)
        {
        SpellCheckIgnoreProgrammerCode(true);
        lily_of_the_valley::cpp_extract_text filter_cpp;
        if (wxGetApp().IsUsingAdvancedImport())
            { filter_cpp.include_all_comments(true); }
        const wxString unicodeStr = Wisteria::TextStream::CharStreamToUnicode(sourceFileText, streamSize);
        SetOriginalDocumentDescription(
            coalesce<wchar_t>({ GetOriginalDocumentDescription().wc_str(),
                                filter_cpp.get_author(),
                                wxFileName(GetOriginalDocumentFilePath()).GetName().wc_str() }
                             ));
        return std::make_pair(true, filter_cpp(unicodeStr, unicodeStr.length()));
        }
    else if (fileExtension.CmpNoCase(L"md") == 0)
        {
        lily_of_the_valley::markdown_extract_text filter_md;
        const wxString unicodeStr = Wisteria::TextStream::CharStreamToUnicode(sourceFileText, streamSize);
        SetOriginalDocumentDescription(
            coalesce({
                     GetOriginalDocumentDescription(),
                     wxFileName(GetOriginalDocumentFilePath()).GetName()
                     }));
        return std::make_pair(true, filter_md(unicodeStr, unicodeStr.length()));
        }
    else if (fileExtension.CmpNoCase(L"txt") == 0)
        {
        SetOriginalDocumentDescription(
            coalesce({ GetOriginalDocumentDescription(),
                       wxFileName(GetOriginalDocumentFilePath()).GetName() }
                    ));
        return std::make_pair(true, Wisteria::TextStream::CharStreamToUnicode(sourceFileText, streamSize));
        }
    else if (fileExtension.CmpNoCase(L"pdf") == 0)
        {
        LogMessage(_(L"PDF files are not supported."),
                   _(L"Import Error"), wxOK|wxICON_EXCLAMATION);
        return std::make_pair(false, wxEmptyString);
        }
    //Unknown (or no) extension
    else
        {
        //See if it is HTML, given that a lot of web pages may not use a known file extension
        if (string_util::stristr(sourceFileText, "<html") )
            {
            std::pair<bool,wxString> extractResult(false, wxEmptyString);
            wxString title;
            //if UTF-8 or simply 7-bit ASCII, then just convert as UTF-8 and run the HTML parser on it
            if (utf8::is_valid(sourceFileText,sourceFileText+streamSize))
                { extractResult = ExtractRawTextWithEncoding(Wisteria::TextStream::CharStreamToUnicode(sourceFileText, streamSize), L"html", GetOriginalDocumentFilePath(), title); }
            //Otherwise, need to search for the encoding in the HTML itself and convert using that, then run the HTML parser on it
            else
                {
                const wxString str = Wisteria::TextStream::CharStreamToUnicode(sourceFileText, streamSize, WebHarvester::GetCharsetFromPageContent(sourceFileText, streamSize));
                extractResult = ExtractRawTextWithEncoding(str, L"html", GetOriginalDocumentFilePath(), title);
                }
            SetOriginalDocumentDescription(
                coalesce({ GetOriginalDocumentDescription(),
                           title,
                           wxFileName(GetOriginalDocumentFilePath()).GetName() }
                        ));
            return std::make_pair(true, extractResult.second);
            }
        //otherwise, just load it as regular text
        else
            {
            LogMessage(_(L"Unknown file extension. File will be imported as plain text."),
                   _(L"Import Warning"), wxOK|wxICON_WARNING);
            return std::make_pair(true, Wisteria::TextStream::CharStreamToUnicode(sourceFileText, streamSize));
            }
        }
    }

//------------------------------------------------
bool BaseProject::LoadExternalDocument()
    {
    FilePathResolver resolvePath;
    // this will "fix" the file path in case it has "file:///". Also fixes slash problem,
    // appends "http" if it just says "www", etc.
    SetOriginalDocumentFilePath(resolvePath.ResolvePath(GetOriginalDocumentFilePath(), true,
        { GetProjectDirectory() }));
    if (resolvePath.IsHTTPFile() || resolvePath.IsHTTPSFile())
        {
        wxString content, contentType;
        long responseCode;
        wxString urlPath = GetOriginalDocumentFilePath();
        if (wxGetApp().GetWebHarvester().ReadWebPage(urlPath, content, contentType,
                                                     responseCode, false))
            {
            wxString title;
            std::pair<bool,wxString> extractResult =
                ExtractRawTextWithEncoding(content, WebHarvester::GetFileTypeFromContentType(contentType),
                                           GetOriginalDocumentFilePath(), title);
            if (!extractResult.first)
                { return false; }
            if (GetOriginalDocumentDescription().empty())
                { SetOriginalDocumentDescription(title); }
            //load the extracted text into the indexing engine
            try
                {
                SetDocumentText(extractResult.second);
                LoadDocument();
                }
            catch (...)
                {
                LogMessage(_(L"An unknown error occurred while analyzing the document. Unable to create project."),
                    _(L"Import Error"), wxOK|wxICON_EXCLAMATION);
                return false;
                }
            // set the name of the project from the title of the page
            string_util::remove_extra_spaces(title);
            title = StripIllegalFileCharacters(title);
            title.Replace(L".", wxString{}, true);
            SetDocumentTitle(title);
            }
        else
            {
            LogMessage(wxString::Format(_(L"Unable to open webpage. The following error occurred:\n%s"),
                         WebHarvester::GetResponseMessage(responseCode)),
                _(L"Error"), wxOK|wxICON_EXCLAMATION);
            return false;
            }
        }
    else if (resolvePath.IsInvalidFile())
        {
        LogMessage(_(L"Invalid file specified."), _(L"Invalid File"), wxOK|wxICON_EXCLAMATION);
        return false;
        }
    else if (resolvePath.IsLocalOrNetworkFile())
        {
        //make sure the file exists first
        if (!wxFile::Exists(GetOriginalDocumentFilePath()) )
            {
            //first try to find the document from the project file's folder structure, then ask the user to search for it manually
            wxString fileBySameNameInProjectDirectory;
            if (FindMissingFile(GetOriginalDocumentFilePath(), fileBySameNameInProjectDirectory))
                { SetOriginalDocumentFilePath(fileBySameNameInProjectDirectory); }

            if (!wxFile::Exists(GetOriginalDocumentFilePath()) )
                {
                if (wxMessageBox(
                        wxString::Format(_(L"%s:\n\nDocument could not be located. Do you wish to search for it?"), GetOriginalDocumentFilePath()),
                        _(L"Document Not Found"), wxYES_NO|wxICON_QUESTION) == wxYES)
                    {
                    wxFileName fn(StripIllegalFileCharacters(GetOriginalDocumentFilePath()));
                    wxFileDialog dialog(nullptr,
                        _(L"Select Document to Analyze"),
                        fn.GetPath(),
                        fn.GetName(),
                        fn.GetFullName(),
                        wxFD_OPEN|wxFD_FILE_MUST_EXIST|wxFD_PREVIEW);

                    if (dialog.ShowModal() != wxID_OK)
                        { return false; }

                    SetOriginalDocumentFilePath(dialog.GetPath());
                    SetModifiedFlag();
                    }
                else
                    {
                    //Give it one last try. If it is set to embedded (e.g., from a parent Batch project),
                    //then try to load the embedded text.
                    if (GetDocumentStorageMethod() == TextStorage::EmbedText)
                        {
                        //there is embedded text (that may have been passed from a batch project),
                        //so load that here.
                        if (GetDocumentText().length())
                            {
                            LoadDocument();
                            return true;
                            }
                        //otherwise, fail.
                        else
                            {
                            if (WarningManager::HasWarning(_DT(L"no-embedded-text")))
                                { LogMessage(*WarningManager::GetWarning(_DT(L"no-embedded-text"))); }
                            return false;
                            }
                        }
                    else
                        { return false; }
                    }
                }
            }

        //read in the text from the file
        try
            {
            MemoryMappedFile sourceFile(GetOriginalDocumentFilePath(), true, true);
            const std::pair<bool,wxString> extractResult = ExtractRawText(static_cast<const char*>(sourceFile.GetStream()), sourceFile.GetMapSize(), wxFileName(GetOriginalDocumentFilePath()).GetExt());
            if (extractResult.first)
                {
                SetDocumentText(extractResult.second);
                try
                    { LoadDocument(); }
                catch (...)
                    {
                    LogMessage(_(L"An unknown error occurred while analyzing the document. Unable to create project."),
                        _(L"Error"), wxOK|wxICON_EXCLAMATION);
                    return false;
                    }
                return true;
                }
            else
                { return false; }
            }
        // weird exception that auto-buffering won't help, so explain it to the user
        catch (const MemoryMappedFileCloudFileError&)
            {
            LogMessage(wxString::Format(_(L"%s:\n\nUnable to open file from Cloud service."),
                                        GetOriginalDocumentFilePath()), 
                _(L"Error"), wxOK|wxICON_EXCLAMATION);
            return false;
            }
        catch (const std::exception& exp)
            {
            LogMessage(wxString::Format(L"%s:\n\n%s", GetOriginalDocumentFilePath(), wxString(exp.what())),
                _(L"Error"), wxOK|wxICON_EXCLAMATION);
            return false;
            }
        catch (...)
            {
            LogMessage(wxString::Format(_(L"%s:\n\nAn unknown error occurred while analyzing the document. Unable to create project."), GetOriginalDocumentFilePath()),
                _(L"Error"), wxOK|wxICON_EXCLAMATION);
            return false;
            }
        }
    // if a file is in an archive then extract it and analyze it
    else if (resolvePath.IsArchivedFile())
        {
        const size_t poundInFile = GetOriginalDocumentFilePath().MakeLower().find(_DT(L".zip#"));
        wxFileName poundFn(GetOriginalDocumentFilePath().substr(0, poundInFile+4));
        if (!wxFile::Exists(poundFn.GetFullPath()) )
            {
            // first try to find the document from the project file's folder structure
            wxString fileBySameNameInProjectDirectory;
            if (FindMissingFile(poundFn.GetFullPath(), fileBySameNameInProjectDirectory))
                { poundFn.Assign(fileBySameNameInProjectDirectory); }
            else
                {
                LogMessage(wxString::Format(_(L"%s:\n\nUnable to open file."), poundFn.GetFullPath()),
                    _(L"Error"), wxOK|wxICON_EXCLAMATION);
                return false;
                }
            }
        Wisteria::ZipCatalog zc(poundFn.GetFullPath());
        wxMemoryOutputStream memstream;
        if (!zc.ReadFile(GetOriginalDocumentFilePath().substr(poundInFile+5), memstream) &&
            zc.GetMessages().size())
            { LogMessage(zc.GetMessages().back().m_message, poundFn.GetFullPath(), zc.GetMessages().back().m_icon); }
        const std::pair<bool,wxString> extractResult =
            ExtractRawText(static_cast<const char*>(memstream.GetOutputStreamBuffer()->GetBufferStart()), memstream.GetLength(), wxFileName(GetOriginalDocumentFilePath()).GetExt());
        if (extractResult.first)
            {
            SetDocumentText(extractResult.second);
            try
                { LoadDocument(); }
            catch (...)
                {
                LogMessage(_(L"An unknown error occurred while analyzing the document. Unable to create project."),
                    _(L"Error"), wxOK|wxICON_EXCLAMATION);
                return false;
                }
            return true;
            }
        else
            { return false; }
        }
    // or a cell in an Excel file
    else if (resolvePath.IsExcelCell())
        {
        const size_t excelTag = GetOriginalDocumentFilePath().MakeLower().find(_DT(L".xlsx#"));
        wxFileName poundFn(GetOriginalDocumentFilePath().substr(0, excelTag+5));
        if (!wxFile::Exists(poundFn.GetFullPath()) )
            {
            // first try to find the document from the project file's folder structure
            wxString fileBySameNameInProjectDirectory;
            if (FindMissingFile(poundFn.GetFullPath(), fileBySameNameInProjectDirectory))
                { poundFn.Assign(fileBySameNameInProjectDirectory); }
            else
                {
                LogMessage(wxString::Format(_(L"%s:\n\nUnable to open file."), poundFn.GetFullPath()),
                    _(L"Error"), wxOK|wxICON_EXCLAMATION);
                return false;
                }
            }
        wxString worksheetName = GetOriginalDocumentFilePath().substr(excelTag+6);
        const size_t slash = worksheetName.find_last_of(L'#');
        if (slash != wxString::npos)
            {
            wxString CellName = worksheetName.substr(slash+1);
            worksheetName.Truncate(slash);
            lily_of_the_valley::xlsx_extract_text filter_xlsx{ false };
            Wisteria::ZipCatalog zc(poundFn.GetFullPath());
            // read in the worksheets
            wxString workBookFileText = zc.ReadTextFile(L"xl/workbook.xml");
            filter_xlsx.read_worksheet_names(workBookFileText.wc_str(), workBookFileText.length());
            // read in the string table
            const wxString sharedStrings = zc.ReadTextFile(L"xl/sharedStrings.xml");
            if (!sharedStrings.length())
                { return false; }
            // find the sheet to get the cells from
            auto sheetPos = std::find(filter_xlsx.get_worksheet_names().begin(),
                                      filter_xlsx.get_worksheet_names().end(), worksheetName.wc_str());
            if (sheetPos != filter_xlsx.get_worksheet_names().end())
                {
                const wxString sheetFile =
                    zc.ReadTextFile(wxString::Format(L"xl/worksheets/sheet%zu.xml",
                                    (sheetPos-filter_xlsx.get_worksheet_names().begin())+1));
                wxString cellText = filter_xlsx.get_cell_text(CellName.wc_str(),
                    sharedStrings.wc_str(), sharedStrings.length(),
                    sheetFile.wc_str(), sheetFile.length());
                SetDocumentText(cellText);
                LoadDocument();
                return true;
                }
            else
                {
                LogMessage(
                    wxString::Format(_(L"Unable to find the worksheet \"%s\" in \"%s\"."),
                        worksheetName, poundFn.GetFullPath()),
                    _(L"Error"), wxOK|wxICON_EXCLAMATION);
                return false;
                }
            }
        }

    return true;
    }

//-------------------------------------------------------
bool BaseProject::LoadDocumentAsSubProject(const wxString& path, const wxString& text, const size_t minWordCount)
    {
    if (!path.empty())
        { wxLogMessage(L"Analyzing %s", path); }
    SetOriginalDocumentFilePath(path);
    SetDocumentStorageMethod(text.empty() ? TextStorage::NoEmbedText : TextStorage::EmbedText);

    DeleteUniqueWordMap();
    ClearSubProjectMessages();
    CreateWords();

    if (GetDocumentStorageMethod() == TextStorage::NoEmbedText)
        {
        if (!LoadExternalDocument() )
            {
            SetLoadingOriginalTextSucceeded(false);
            return false;
            }
        }
    else
        {
        SetDocumentText(text);
        if (text.empty())
            {
            SetLoadingOriginalTextSucceeded(false);
            return false;
            }
        try
            { LoadDocument(); }
        catch (...)
            {
            LogMessage(_(L"An unknown error occurred while analyzing the document. Unable to create project."),
                _(L"Error"), wxOK|wxICON_EXCLAMATION);
            SetLoadingOriginalTextSucceeded(false);
            return false;
            }
        }

    if (GetWords() == nullptr)
        {
        LogMessage(_(L"An unknown error occurred while loading the document."),
                   _(L"Error"), wxOK|wxICON_EXCLAMATION);
        SetLoadingOriginalTextSucceeded(false);
        return false;
        }

    /** sSee if there is an inordinate amount of titles/headers/bullets, and if they
        are asking to these then make sure they understand that a large part of the
        document will be ignored.
        Note that we don't bother with this check with webpages because they normally
        contain lists for things like menus that we would indeed want to ignore.*/
    FilePathResolver resolvePath(GetOriginalDocumentFilePath(), true);
    if (GetWords()->get_sentence_count() > 0 &&
        !resolvePath.IsWebFile())
        {
        /* if document is nothing but valid sentences then it is OK.*/
        if (GetWords()->get_sentence_count() == GetWords()->get_complete_sentence_count())
            {
            // NOOP
            }
        else
            {
            const double numberOfInvalidSentencesPercentage = safe_divide<double>((GetWords()->get_sentence_count() - GetWords()->get_complete_sentence_count()),
                GetWords()->get_sentence_count());
            if (numberOfInvalidSentencesPercentage > 0.60f &&
                (GetInvalidSentenceMethod() == InvalidSentence::ExcludeFromAnalysis || GetInvalidSentenceMethod() == InvalidSentence::ExcludeExceptForHeadings))
                {
                
                if (WarningManager::HasWarning(_DT(L"high-count-sentences-being-ignored")))
                    {
                    auto warningMsg = *WarningManager::GetWarning(_DT(L"high-count-sentences-being-ignored"));
                    warningMsg.SetMessage(_(L"This document contains a large percentage of incomplete sentences that you have requested to ignore."));
                    LogMessage(warningMsg);
                    }
                }
            }
        }

    if (GetInvalidSentenceMethod() == InvalidSentence::ExcludeFromAnalysis ||
        GetInvalidSentenceMethod() == InvalidSentence::ExcludeExceptForHeadings)
        { CalculateStatisticsIgnoringInvalidSentences(); }
    else if (GetInvalidSentenceMethod() == InvalidSentence::IncludeAsFullSentences)
        { CalculateStatistics(); }
    if (GetTotalWords() == 0)
        {
        LogMessage(_(L"No words were found in this file."),
            _(L"Import Error"), wxOK|wxICON_EXCLAMATION);
        SetLoadingOriginalTextSucceeded(false);
        return false;
        }
    else if (GetTotalWords() < minWordCount)
        {
        LogMessage(wxString::Format(_(L"The text that you are analyzing is less than %zu words. Test results will not be meaningful with such a small sample; therefore, this document will not be analyzed."),
                        minWordCount),
                   _(L"Error"), wxOK|wxICON_EXCLAMATION);
        SetLoadingOriginalTextSucceeded(false);
        return false;
        }
    else if (GetTotalWords() < 100)
        {
        if (WarningManager::HasWarning(_DT(L"document-less-than-100-words")))
            { LogMessage(*WarningManager::GetWarning(_DT(L"document-less-than-100-words"))); }
        }
    // check for sentences that got broken up by paragraph breaks and warn if there are lot of them,
    // this indicates a messed up file.
    size_t paragraphBrokenSentences = 0;
    for (std::vector<size_t>::const_iterator pos = GetWords()->get_lowercase_beginning_sentences().begin();
        pos != GetWords()->get_lowercase_beginning_sentences().end();
        ++pos)
        {
        // if there is a complete, 3-word or more sentence starting with a lowercased letter
        // following an incomplete sentence (that would not be a list item or header),
        // then this might be a sentence accidentally split by two lines (e.g., a paragraph break)
        if (*pos > 0 &&
            GetWords()->get_sentences()[*pos].get_word_count() > 3 &&
            GetWords()->get_sentences()[(*pos)].get_type() == grammar::sentence_paragraph_type::complete &&
            GetWords()->get_sentences()[(*pos)-1].get_type() == grammar::sentence_paragraph_type::incomplete)
            { ++paragraphBrokenSentences; }
        }
    if (paragraphBrokenSentences >= 5)
        {
        if (WarningManager::HasWarning(_DT(L"sentences-split-by-paragraph-breaks")))
            {
            auto warningMsg = *WarningManager::GetWarning(_DT(L"sentences-split-by-paragraph-breaks"));
            warningMsg.SetMessage(wxString::Format(_(L"This document contains at least %zu sentences that appear to be split by paragraph breaks. This may lead to incorrect results.\nPlease review your document to verify that this is intentional."), paragraphBrokenSentences));
            LogMessage(warningMsg);
            }
        }
    //Go through the sentences and see if any are not complete but considered valid because of their length.
    //If any are found, then mention it to the user.
    size_t sentencesMissingEndingPunctionsConsideredCompleteBecauseOfLength = 0;
    for (std::vector<grammar::sentence_info>::const_iterator sentPos = GetWords()->get_sentences().begin();
         sentPos != GetWords()->get_sentences().end();
         ++sentPos)
        {
        if (sentPos->is_valid() && !sentPos->ends_with_valid_punctuation() && sentPos->get_ending_punctuation() != common_lang_constants::SEMICOLON &&
            sentPos->get_word_count() > GetIncludeIncompleteSentencesIfLongerThanValue())
            { ++sentencesMissingEndingPunctionsConsideredCompleteBecauseOfLength; }
        }
    if (sentencesMissingEndingPunctionsConsideredCompleteBecauseOfLength > 0)
        {
        if (WarningManager::HasWarning(_DT(L"incomplete-sentences-valid-from-length")))
            {
            auto warningMsg = *WarningManager::GetWarning(_DT(L"incomplete-sentences-valid-from-length"));
            warningMsg.SetMessage(wxString::Format(_(L"This document contains %d incomplete sentences longer than %zu words which will be included in the analysis.\n\nTo change this, increase the \"Include incomplete sentences containing more than...\" option under Project Properties->Document Indexing."), sentencesMissingEndingPunctionsConsideredCompleteBecauseOfLength, GetIncludeIncompleteSentencesIfLongerThanValue()));
            LogMessage(warningMsg);
            }
        }

    LoadHardWords();

    SetLoadingOriginalTextSucceeded(true);
    return true;
    }

//-------------------------------------------------------
bool BaseProject::AddBormuthClozeMeanTest(const bool setFocus)
    {
    ClearReadabilityTestResult();
    if (!wxGetApp().GetLicenseAdmin().IsFeatureEnabled(wxGetApp().FeatureEnglishReadabilityTestsCode()))
        { return false; }
    const wxString CURRENT_TEST_KEY = ReadabilityMessages::BORMUTH_CLOZE_MEAN();

    if ((GetDaleChallTextExclusionMode() == SpecializedTestTextExclusion::ExcludeIncompleteSentencesExceptHeadings &&
            !GetTotalWordsFromCompleteSentencesAndHeaders()) ||
        !GetTotalWords())
        {
        LogMessage(wxString::Format(_(L"Unable to calculate %s: at least one word must be present in document."),
            GetReadabilityTests().get_test_short_name(CURRENT_TEST_KEY).c_str()),
            _(L"Error"), wxOK|wxICON_ERROR, wxEmptyString, true);
        GetReadabilityTests().include_test(CURRENT_TEST_KEY, false);
        return false;
        }
    if ((GetDaleChallTextExclusionMode() == SpecializedTestTextExclusion::ExcludeIncompleteSentencesExceptHeadings &&
            !GetTotalSentencesFromCompleteSentencesAndHeaders()) ||
        !GetTotalSentences() )
        {
        LogMessage(wxString::Format(_(L"Unable to calculate %s: at least one sentence must be present in document."),
            GetReadabilityTests().get_test_short_name(CURRENT_TEST_KEY).c_str()),
            _(L"Error"), wxOK|wxICON_ERROR, wxEmptyString, true);
        GetReadabilityTests().include_test(CURRENT_TEST_KEY, false);
        return false;
        }

    try
        {
        const double val = round_to_integer(readability::bormuth_cloze_mean(
            (GetDaleChallTextExclusionMode() == SpecializedTestTextExclusion::ExcludeIncompleteSentencesExceptHeadings) ? GetTotalWordsFromCompleteSentencesAndHeaders() : GetTotalWords(),
            (GetDaleChallTextExclusionMode() == SpecializedTestTextExclusion::ExcludeIncompleteSentencesExceptHeadings) ? (GetTotalWordsFromCompleteSentencesAndHeaders() - GetTotalHardWordsDaleChall()) : (GetTotalWords() - GetTotalHardWordsDaleChall()),
            (GetDaleChallTextExclusionMode() == SpecializedTestTextExclusion::ExcludeIncompleteSentencesExceptHeadings) ? GetTotalCharactersFromCompleteSentencesAndHeaders() : GetTotalCharacters(),
            (GetDaleChallTextExclusionMode() == SpecializedTestTextExclusion::ExcludeIncompleteSentencesExceptHeadings) ? GetTotalSentencesFromCompleteSentencesAndHeaders() : GetTotalSentences() )
            * 100/*convert to integer percentage*/);

        const auto theTest = GetReadabilityTests().find_test(CURRENT_TEST_KEY);
        if (!VerifyTestBeforeAdding(theTest))
            { return false; }

        const wxString description = HasUI() ?
            ProjectReportFormat::FormatTestResult(
                ReadabilityMessages::GetPredictedClozeDescription(round_to_integer(val)),
                theTest.first->get_test(),
                ReadabilityMessages::GetPredictedClozeNote() + L"<br /><br />" + ReadabilityMessages::GetPunctuationIgnoredNote()) :
            wxString{};

        SetReadabilityTestResult(CURRENT_TEST_KEY, theTest.first->get_test().get_long_name().c_str(),
            description,
            std::make_pair(std::numeric_limits<double>::quiet_NaN(), wxEmptyString),
            wxEmptyString,
            std::numeric_limits<double>::quiet_NaN(), val, setFocus);
        }
    catch (...)
        {
        HandleFailedTestCalculation(CURRENT_TEST_KEY);
        return false;
        }

    GetReadabilityTests().include_test(CURRENT_TEST_KEY, true);
    return true;
    }

//-------------------------------------------------------
bool BaseProject::AddBormuthGradePlacement35Test(const bool setFocus)
    {
    ClearReadabilityTestResult();
    if (!wxGetApp().GetLicenseAdmin().IsFeatureEnabled(wxGetApp().FeatureEnglishReadabilityTestsCode()))
        { return false; }
    const wxString CURRENT_TEST_KEY = ReadabilityMessages::BORMUTH_GRADE_PLACEMENT_35();

    if ((GetDaleChallTextExclusionMode() == SpecializedTestTextExclusion::ExcludeIncompleteSentencesExceptHeadings &&
            !GetTotalWordsFromCompleteSentencesAndHeaders()) ||
        !GetTotalWords())
        {
        LogMessage(wxString::Format(_(L"Unable to calculate %s: at least one word must be present in document."),
            GetReadabilityTests().get_test_short_name(CURRENT_TEST_KEY).c_str()),
            _(L"Error"), wxOK|wxICON_ERROR, wxEmptyString, true);
        GetReadabilityTests().include_test(CURRENT_TEST_KEY, false);
        return false;
        }
    if ((GetDaleChallTextExclusionMode() == SpecializedTestTextExclusion::ExcludeIncompleteSentencesExceptHeadings &&
            !GetTotalSentencesFromCompleteSentencesAndHeaders()) ||
        !GetTotalSentences() )
        {
        LogMessage(wxString::Format(_(L"Unable to calculate %s: at least one sentence must be present in document."),
            GetReadabilityTests().get_test_short_name(CURRENT_TEST_KEY).c_str()),
            _(L"Error"), wxOK|wxICON_ERROR, wxEmptyString, true);
        GetReadabilityTests().include_test(CURRENT_TEST_KEY, false);
        return false;
        }

    try
        {
        const double gradeValue = readability::bormuth_grade_placement_35(
            (GetDaleChallTextExclusionMode() == SpecializedTestTextExclusion::ExcludeIncompleteSentencesExceptHeadings) ? GetTotalWordsFromCompleteSentencesAndHeaders() : GetTotalWords(),
            (GetDaleChallTextExclusionMode() == SpecializedTestTextExclusion::ExcludeIncompleteSentencesExceptHeadings) ? (GetTotalWordsFromCompleteSentencesAndHeaders() - GetTotalHardWordsDaleChall()) : (GetTotalWords() - GetTotalHardWordsDaleChall()),
            (GetDaleChallTextExclusionMode() == SpecializedTestTextExclusion::ExcludeIncompleteSentencesExceptHeadings) ? GetTotalCharactersFromCompleteSentencesAndHeaders() : GetTotalCharacters(),
            (GetDaleChallTextExclusionMode() == SpecializedTestTextExclusion::ExcludeIncompleteSentencesExceptHeadings) ? GetTotalSentencesFromCompleteSentencesAndHeaders() : GetTotalSentences() );

        const auto theTest = GetReadabilityTests().find_test(CURRENT_TEST_KEY);
        if (!VerifyTestBeforeAdding(theTest))
            { return false; }

        const wxString description = HasUI() ?
            ProjectReportFormat::FormatTestResult(
                GetReadabilityMessageCatalog().GetGradeScaleDescription(gradeValue),
                theTest.first->get_test(),
                ReadabilityMessages::GetPunctuationIgnoredNote()) :
            wxString{};

        wxString displayableGradeLevel = wxNumberFormatter::ToString(gradeValue, 1, wxNumberFormatter::Style::Style_NoTrailingZeroes);
        if (gradeValue == 19)
            { displayableGradeLevel += L"+"; }

        SetReadabilityTestResult(CURRENT_TEST_KEY, theTest.first->get_test().get_long_name().c_str(),
            description,
            std::make_pair(gradeValue,displayableGradeLevel),
            ReadabilityMessages::GetAgeFromUSGrade(gradeValue, GetReadabilityMessageCatalog().GetReadingAgeDisplay()),
            std::numeric_limits<double>::quiet_NaN(),
            std::numeric_limits<double>::quiet_NaN(), setFocus);
        }
    catch (...)
        {
        HandleFailedTestCalculation(CURRENT_TEST_KEY);
        return false;
        }

    GetReadabilityTests().include_test(CURRENT_TEST_KEY, true);
    return true;
    }

//-------------------------------------------------------
bool BaseProject::AddPskDaleChallTest(const bool setFocus)
    {
    ClearReadabilityTestResult();
    if (!wxGetApp().GetLicenseAdmin().IsFeatureEnabled(wxGetApp().FeatureEnglishReadabilityTestsCode()))
        { return false; }
    const wxString CURRENT_TEST_KEY = ReadabilityMessages::PSK_DALE_CHALL();

    if (!GetTotalWords())
        {
        LogMessage(wxString::Format(_(L"Unable to calculate %s: at least one word must be present in document."),
            GetReadabilityTests().get_test_short_name(CURRENT_TEST_KEY).c_str()),
            _(L"Error"), wxOK|wxICON_ERROR, wxEmptyString, true);
        GetReadabilityTests().include_test(CURRENT_TEST_KEY, false);
        return false;
        }
    if (!GetTotalSentences() )
        {
        LogMessage(wxString::Format(_(L"Unable to calculate %s: at least one sentence must be present in document."),
            GetReadabilityTests().get_test_short_name(CURRENT_TEST_KEY).c_str()),
            _(L"Error"), wxOK|wxICON_ERROR, wxEmptyString, true);
        GetReadabilityTests().include_test(CURRENT_TEST_KEY, false);
        return false;
        }

    try
        {
        const double gradeValue = readability::powers_sumner_kearl_dale_chall(
            (GetDaleChallTextExclusionMode() == SpecializedTestTextExclusion::ExcludeIncompleteSentencesExceptHeadings) ? GetTotalWordsFromCompleteSentencesAndHeaders() : GetTotalWords(),
            GetTotalHardWordsDaleChall(),
            (GetDaleChallTextExclusionMode() == SpecializedTestTextExclusion::ExcludeIncompleteSentencesExceptHeadings) ? GetTotalSentencesFromCompleteSentencesAndHeaders() : GetTotalSentences() );

        const auto theTest = GetReadabilityTests().find_test(CURRENT_TEST_KEY);
        if (!VerifyTestBeforeAdding(theTest))
            { return false; }

        const wxString description = HasUI() ?
            ProjectReportFormat::FormatTestResult(
                GetReadabilityMessageCatalog().GetGradeScaleDescription(gradeValue),
                theTest.first->get_test()) :
            wxString{};

        wxString displayableGradeLevel = wxNumberFormatter::ToString(gradeValue, 1, wxNumberFormatter::Style::Style_NoTrailingZeroes);
        if (gradeValue == 19)
            { displayableGradeLevel += L"+"; }

        SetReadabilityTestResult(CURRENT_TEST_KEY, theTest.first->get_test().get_long_name().c_str(),
            description,
            std::make_pair(gradeValue, displayableGradeLevel),
            ReadabilityMessages::GetAgeFromUSGrade(gradeValue, GetReadabilityMessageCatalog().GetReadingAgeDisplay()),
            std::numeric_limits<double>::quiet_NaN(),
            std::numeric_limits<double>::quiet_NaN(), setFocus);
        }
    catch (...)
        {
        HandleFailedTestCalculation(CURRENT_TEST_KEY);
        return false;
        }

    GetReadabilityTests().include_test(CURRENT_TEST_KEY, true);
    return true;
    }

//-------------------------------------------------------
bool BaseProject::AddNewDaleChallTest(const bool setFocus)
    {
    ClearReadabilityTestResult();
    if (!wxGetApp().GetLicenseAdmin().IsFeatureEnabled(wxGetApp().FeatureEnglishReadabilityTestsCode()))
        { return false; }
    const wxString CURRENT_TEST_KEY = ReadabilityMessages::DALE_CHALL();

    if ((GetDaleChallTextExclusionMode() == SpecializedTestTextExclusion::ExcludeIncompleteSentencesExceptHeadings &&
            !GetTotalWordsFromCompleteSentencesAndHeaders()) ||
        !GetTotalWords())
        {
        LogMessage(wxString::Format(_(L"Unable to calculate %s: at least one word must be present in document."),
            GetReadabilityTests().get_test_short_name(CURRENT_TEST_KEY).c_str()),
            _(L"Error"), wxOK|wxICON_ERROR, wxEmptyString, true);
        GetReadabilityTests().include_test(CURRENT_TEST_KEY, false);
        return false;
        }
    if ((GetDaleChallTextExclusionMode() == SpecializedTestTextExclusion::ExcludeIncompleteSentencesExceptHeadings &&
            !GetTotalSentencesFromCompleteSentencesAndHeaders()) ||
        !GetTotalSentences() )
        {
        LogMessage(wxString::Format(_(L"Unable to calculate %s: at least one sentence must be present in document."),
            GetReadabilityTests().get_test_short_name(CURRENT_TEST_KEY).c_str()),
            _(L"Error"), wxOK|wxICON_ERROR, wxEmptyString, true);
        GetReadabilityTests().include_test(CURRENT_TEST_KEY, false);
        return false;
        }

    try
        {
        size_t gradeBegin{ 0 }, gradeEnd{ 0 };
        if (GetDaleChallTextExclusionMode() == SpecializedTestTextExclusion::ExcludeIncompleteSentencesExceptHeadings)
            {
            readability::new_dale_chall(gradeBegin, gradeEnd,
                    GetTotalWordsFromCompleteSentencesAndHeaders(),
                    GetTotalHardWordsDaleChall(),
                    GetTotalSentencesFromCompleteSentencesAndHeaders() );
            }
        else
            {
            readability::new_dale_chall(gradeBegin, gradeEnd,
                        GetTotalWords(),
                        GetTotalHardWordsDaleChall(),
                        GetTotalSentences() );
            }

        const auto theTest = GetReadabilityTests().find_test(CURRENT_TEST_KEY);
        if (!VerifyTestBeforeAdding(theTest))
            { return false; }

        //if a range between different grades then it needs to be displayed differently
        wxString gradeValue, explanation;
        if (gradeBegin != gradeEnd)
            {
            gradeValue = wxString::Format(L"%zu-%zu", gradeBegin, gradeEnd);
            explanation = ProjectReportFormat::FormatTestResult(
                GetReadabilityMessageCatalog().GetGradeScaleDescription(gradeBegin, gradeEnd),
                theTest.first->get_test());
            }
        else
            {
            gradeValue = std::to_wstring(gradeBegin);
            if (gradeBegin == 16)
                { gradeValue += L"+"; }
            explanation = ProjectReportFormat::FormatTestResult(
                GetReadabilityMessageCatalog().GetGradeScaleDescription(gradeBegin),
                theTest.first->get_test());
            }

        SetReadabilityTestResult(CURRENT_TEST_KEY, theTest.first->get_test().get_long_name().c_str(),
            explanation,
            std::make_pair(safe_divide<double>(gradeBegin+gradeEnd, 2), gradeValue),
            ReadabilityMessages::GetAgeFromUSGrade(gradeBegin, gradeEnd, GetReadabilityMessageCatalog().GetReadingAgeDisplay()),
            std::numeric_limits<double>::quiet_NaN(),
            std::numeric_limits<double>::quiet_NaN(), setFocus);
        }
    catch (...)
        {
        HandleFailedTestCalculation(CURRENT_TEST_KEY);
        return false;
        }

    GetReadabilityTests().include_test(CURRENT_TEST_KEY, true);
    return true;
    }

//-------------------------------------------------------
bool BaseProject::AddDegreesOfReadingPowerGeTest(const bool setFocus)
    {
    ClearReadabilityTestResult();
    if (!wxGetApp().GetLicenseAdmin().IsFeatureEnabled(wxGetApp().FeatureEnglishReadabilityTestsCode()))
        { return false; }
    const wxString CURRENT_TEST_KEY = ReadabilityMessages::DEGREES_OF_READING_POWER_GE();

    if ((GetDaleChallTextExclusionMode() == SpecializedTestTextExclusion::ExcludeIncompleteSentencesExceptHeadings &&
            !GetTotalWordsFromCompleteSentencesAndHeaders()) ||
        !GetTotalWords())
        {
        LogMessage(wxString::Format(_(L"Unable to calculate %s: at least one word must be present in document."),
            GetReadabilityTests().get_test_short_name(CURRENT_TEST_KEY).c_str()),
            _(L"Error"), wxOK|wxICON_ERROR, wxEmptyString, true);
        GetReadabilityTests().include_test(CURRENT_TEST_KEY, false);
        return false;
        }
    if ((GetDaleChallTextExclusionMode() == SpecializedTestTextExclusion::ExcludeIncompleteSentencesExceptHeadings &&
            !GetTotalSentencesFromCompleteSentencesAndHeaders()) ||
        !GetTotalSentences() )
        {
        LogMessage(wxString::Format(_(L"Unable to calculate %s: at least one sentence must be present in document."),
            GetReadabilityTests().get_test_short_name(CURRENT_TEST_KEY).c_str()),
            _(L"Error"), wxOK|wxICON_ERROR, wxEmptyString, true);
        GetReadabilityTests().include_test(CURRENT_TEST_KEY, false);
        return false;
        }

    try
        {
        const double gradeValue = readability::degrees_of_reading_power_ge(
            (GetDaleChallTextExclusionMode() == SpecializedTestTextExclusion::ExcludeIncompleteSentencesExceptHeadings) ? GetTotalWordsFromCompleteSentencesAndHeaders() : GetTotalWords(),
            (GetDaleChallTextExclusionMode() == SpecializedTestTextExclusion::ExcludeIncompleteSentencesExceptHeadings) ? (GetTotalWordsFromCompleteSentencesAndHeaders() - GetTotalHardWordsDaleChall()) : (GetTotalWords() - GetTotalHardWordsDaleChall()),
            (GetDaleChallTextExclusionMode() == SpecializedTestTextExclusion::ExcludeIncompleteSentencesExceptHeadings) ? GetTotalCharactersFromCompleteSentencesAndHeaders() : GetTotalCharacters(),
            (GetDaleChallTextExclusionMode() == SpecializedTestTextExclusion::ExcludeIncompleteSentencesExceptHeadings) ? GetTotalSentencesFromCompleteSentencesAndHeaders() : GetTotalSentences() );

        const auto theTest = GetReadabilityTests().find_test(CURRENT_TEST_KEY);
        if (!VerifyTestBeforeAdding(theTest))
            { return false; }

        const wxString description  = HasUI() ?
            ProjectReportFormat::FormatTestResult(
                GetReadabilityMessageCatalog().GetGradeScaleDescription(gradeValue),
                theTest.first->get_test(),
                ReadabilityMessages::GetPunctuationIgnoredNote()) :
            wxString{};

        wxString displayableGradeLevel = wxNumberFormatter::ToString(gradeValue, 1, wxNumberFormatter::Style::Style_NoTrailingZeroes);
        if (gradeValue == 18) // Carver table stops at 18
            { displayableGradeLevel += L"+"; }

        SetReadabilityTestResult(CURRENT_TEST_KEY, theTest.first->get_test().get_long_name().c_str(), description,
            std::make_pair(gradeValue, displayableGradeLevel),
            ReadabilityMessages::GetAgeFromUSGrade(gradeValue, GetReadabilityMessageCatalog().GetReadingAgeDisplay()),
            std::numeric_limits<double>::quiet_NaN(),
            std::numeric_limits<double>::quiet_NaN(), setFocus);
        }
    catch (...)
        {
        HandleFailedTestCalculation(CURRENT_TEST_KEY);
        return false;
        }

    GetReadabilityTests().include_test(CURRENT_TEST_KEY, true);
    return true;
    }

//-------------------------------------------------------
bool BaseProject::AddDegreesOfReadingPowerTest(const bool setFocus)
    {
    ClearReadabilityTestResult();
    if (!wxGetApp().GetLicenseAdmin().IsFeatureEnabled(wxGetApp().FeatureEnglishReadabilityTestsCode()))
        { return false; }
    const wxString CURRENT_TEST_KEY = ReadabilityMessages::DEGREES_OF_READING_POWER();

    if ((GetDaleChallTextExclusionMode() == SpecializedTestTextExclusion::ExcludeIncompleteSentencesExceptHeadings &&
            !GetTotalWordsFromCompleteSentencesAndHeaders()) ||
        !GetTotalWords())
        {
        LogMessage(wxString::Format(_(L"Unable to calculate %s: at least one word must be present in document."),
            GetReadabilityTests().get_test_short_name(CURRENT_TEST_KEY).c_str()),
            _(L"Error"), wxOK|wxICON_ERROR, wxEmptyString, true);
        GetReadabilityTests().include_test(CURRENT_TEST_KEY, false);
        return false;
        }
    if ((GetDaleChallTextExclusionMode() == SpecializedTestTextExclusion::ExcludeIncompleteSentencesExceptHeadings &&
            !GetTotalSentencesFromCompleteSentencesAndHeaders()) ||
        !GetTotalSentences() )
        {
        LogMessage(wxString::Format(_(L"Unable to calculate %s: at least one sentence must be present in document."),
            GetReadabilityTests().get_test_short_name(CURRENT_TEST_KEY).c_str()),
            _(L"Error"), wxOK|wxICON_ERROR, wxEmptyString, true);
        GetReadabilityTests().include_test(CURRENT_TEST_KEY, false);
        return false;
        }

    try
        {
        const size_t val = round_to_integer(readability::degrees_of_reading_power(
            (GetDaleChallTextExclusionMode() == SpecializedTestTextExclusion::ExcludeIncompleteSentencesExceptHeadings) ? GetTotalWordsFromCompleteSentencesAndHeaders() : GetTotalWords(),
            (GetDaleChallTextExclusionMode() == SpecializedTestTextExclusion::ExcludeIncompleteSentencesExceptHeadings) ? (GetTotalWordsFromCompleteSentencesAndHeaders() - GetTotalHardWordsDaleChall()) : (GetTotalWords() - GetTotalHardWordsDaleChall()),
            (GetDaleChallTextExclusionMode() == SpecializedTestTextExclusion::ExcludeIncompleteSentencesExceptHeadings) ? GetTotalCharactersFromCompleteSentencesAndHeaders() : GetTotalCharacters(),
            (GetDaleChallTextExclusionMode() == SpecializedTestTextExclusion::ExcludeIncompleteSentencesExceptHeadings) ? GetTotalSentencesFromCompleteSentencesAndHeaders() : GetTotalSentences() ) );

        const auto theTest = GetReadabilityTests().find_test(CURRENT_TEST_KEY);
        if (!VerifyTestBeforeAdding(theTest))
            { return false; }

        const wxString description = HasUI() ?
            ProjectReportFormat::FormatTestResult(
                ReadabilityMessages::GetDrpUnitDescription(val),
                theTest.first->get_test(),
                ReadabilityMessages::GetDrpNote() + L"<br /><br />" + ReadabilityMessages::GetPunctuationIgnoredNote()) :
            wxString{};

        SetReadabilityTestResult(CURRENT_TEST_KEY, theTest.first->get_test().get_long_name().c_str(),
            description,
            std::make_pair(std::numeric_limits<double>::quiet_NaN(), wxEmptyString),
            wxEmptyString, val,
            std::numeric_limits<double>::quiet_NaN(), setFocus);
        }
    catch (...)
        {
        HandleFailedTestCalculation(CURRENT_TEST_KEY);
        return false;
        }

    GetReadabilityTests().include_test(CURRENT_TEST_KEY, true);
    return true;
    }

//-------------------------------------------------------
bool BaseProject::AddSolSpanishTest(const bool setFocus)
    {
    ClearReadabilityTestResult();
    if (!wxGetApp().GetLicenseAdmin().IsFeatureEnabled(wxGetApp().FeatureLanguagePackCode()) &&
        !wxGetApp().GetLicenseAdmin().IsFeatureEnabled(wxGetApp().FeatureProfessionalCode()))
        { return false; }
    const wxString CURRENT_TEST_KEY = ReadabilityMessages::SOL_SPANISH();

    if (GetTotalSentencesFromCompleteSentencesAndHeaders() == 0)
        {
        LogMessage(wxString::Format(_(L"Unable to calculate %s: at least one sentence must be present in document."),
            GetReadabilityTests().get_test_short_name(CURRENT_TEST_KEY).c_str()),
            _(L"Error"), wxOK|wxICON_ERROR, wxEmptyString, true);
        GetReadabilityTests().include_test(CURRENT_TEST_KEY, false);
        return false;
        }

    try
        {
        const double gradeValue = readability::sol_spanish(GetTotalHardWordsSol(), GetTotalSentencesFromCompleteSentencesAndHeaders());

        const auto theTest = GetReadabilityTests().find_test(CURRENT_TEST_KEY);
        if (!VerifyTestBeforeAdding(theTest))
            { return false; }

        const wxString description = HasUI() ?
            ProjectReportFormat::FormatTestResult(
                GetReadabilityMessageCatalog().GetGradeScaleDescription(gradeValue),
                theTest.first->get_test(),
                _(L"Numerals are fully syllabized (i.e., sounded out) for this test. Headers and footers are also included and counted as separate sentences.")) :
                wxString{};

        wxString displayableGradeLevel = wxNumberFormatter::ToString(gradeValue, 1, wxNumberFormatter::Style::Style_NoTrailingZeroes);
        if (gradeValue == 19)
            { displayableGradeLevel += L"+"; }

        SetReadabilityTestResult(CURRENT_TEST_KEY, theTest.first->get_test().get_long_name().c_str(), description,
            std::make_pair(gradeValue, displayableGradeLevel),
            ReadabilityMessages::GetAgeFromUSGrade(gradeValue, GetReadabilityMessageCatalog().GetReadingAgeDisplay()),
            std::numeric_limits<double>::quiet_NaN(),
            std::numeric_limits<double>::quiet_NaN(), setFocus);
        }
    catch (...)
        {
        HandleFailedTestCalculation(CURRENT_TEST_KEY);
        return false;
        }

    GetReadabilityTests().include_test(CURRENT_TEST_KEY, true);
    return true;
    }

//-------------------------------------------------------
bool BaseProject::AddElfTest(const bool setFocus)
    {
    ClearReadabilityTestResult();
    if (!wxGetApp().GetLicenseAdmin().IsFeatureEnabled(wxGetApp().FeatureEnglishReadabilityTestsCode()))
        { return false; }
    const wxString CURRENT_TEST_KEY = ReadabilityMessages::ELF();

    if (GetTotalSentences() == 0)
        {
        LogMessage(wxString::Format(_(L"Unable to calculate %s: at least one sentence must be present in document."),
            GetReadabilityTests().get_test_short_name(CURRENT_TEST_KEY).c_str()),
            _(L"Error"), wxOK|wxICON_ERROR, wxEmptyString, true);
        GetReadabilityTests().include_test(CURRENT_TEST_KEY, false);
        return false;
        }

    try
        {
        const double gradeValue = readability::easy_listening_formula(GetTotalWords(), GetTotalSyllables(), GetTotalSentences());

        const auto theTest = GetReadabilityTests().find_test(CURRENT_TEST_KEY);
        if (!VerifyTestBeforeAdding(theTest))
            { return false; }

        const wxString description = HasUI() ?
            ProjectReportFormat::FormatTestResult(
                GetReadabilityMessageCatalog().GetGradeScaleDescription(gradeValue),
                theTest.first->get_test()) :
            wxString{};

        wxString displayableGradeLevel = wxNumberFormatter::ToString(gradeValue, 1, wxNumberFormatter::Style::Style_NoTrailingZeroes);
        if (gradeValue == 19)
            { displayableGradeLevel += L"+"; }

        SetReadabilityTestResult(CURRENT_TEST_KEY, theTest.first->get_test().get_long_name().c_str(), description,
            std::make_pair(gradeValue, displayableGradeLevel),
            ReadabilityMessages::GetAgeFromUSGrade(gradeValue, GetReadabilityMessageCatalog().GetReadingAgeDisplay()),
            std::numeric_limits<double>::quiet_NaN(),
            std::numeric_limits<double>::quiet_NaN(), setFocus);
        }
    catch (...)
        {
        HandleFailedTestCalculation(CURRENT_TEST_KEY);
        return false;
        }

    GetReadabilityTests().include_test(CURRENT_TEST_KEY, true);
    return true;
    }

//-------------------------------------------------------
bool BaseProject::AddSmogSimplifiedTest(const bool setFocus)
    {
    ClearReadabilityTestResult();
    if (!wxGetApp().GetLicenseAdmin().IsFeatureEnabled(wxGetApp().FeatureEnglishReadabilityTestsCode()))
        { return false; }
    const wxString CURRENT_TEST_KEY = ReadabilityMessages::SMOG_SIMPLIFIED();

    if (GetTotalSentences() == 0)
        {
        LogMessage(wxString::Format(_(L"Unable to calculate %s: at least one sentence must be present in document."),
            GetReadabilityTests().get_test_short_name(CURRENT_TEST_KEY).c_str()),
            _(L"Error"), wxOK|wxICON_ERROR, wxEmptyString, true);
        GetReadabilityTests().include_test(CURRENT_TEST_KEY, false);
        return false;
        }

    try
        {
        const double gradeValue = readability::smog_simplified(GetTotal3PlusSyllabicWordsNumeralsFullySyllabized(), GetTotalSentences());

        const auto theTest = GetReadabilityTests().find_test(CURRENT_TEST_KEY);
        if (!VerifyTestBeforeAdding(theTest))
            { return false; }

        const wxString numeralLabel = (GetTotalNumerals() > 0) ?
            _(L"Numerals are fully syllabized (i.e., sounded out) for this test.") :
            wxString{};
        const wxString description = HasUI() ?
            ProjectReportFormat::FormatTestResult(
                GetReadabilityMessageCatalog().GetGradeScaleDescription(gradeValue),
                theTest.first->get_test(),
                numeralLabel) :
            wxString{};

        wxString displayableGradeLevel = wxNumberFormatter::ToString(gradeValue, 1, wxNumberFormatter::Style::Style_NoTrailingZeroes);
        if (gradeValue == 19)
            { displayableGradeLevel += L"+"; }

        SetReadabilityTestResult(CURRENT_TEST_KEY, theTest.first->get_test().get_long_name().c_str(), description,
            std::make_pair(gradeValue, displayableGradeLevel),
            ReadabilityMessages::GetAgeFromUSGrade(gradeValue, GetReadabilityMessageCatalog().GetReadingAgeDisplay()),
            std::numeric_limits<double>::quiet_NaN(),
            std::numeric_limits<double>::quiet_NaN(), setFocus);
        }
    catch (...)
        {
        HandleFailedTestCalculation(CURRENT_TEST_KEY);
        return false;
        }

    GetReadabilityTests().include_test(CURRENT_TEST_KEY, true);
    return true;
    }

//-------------------------------------------------------
bool BaseProject::AddModifiedSmogTest(const bool setFocus)
    {
    ClearReadabilityTestResult();
    if (!wxGetApp().GetLicenseAdmin().IsFeatureEnabled(wxGetApp().FeatureEnglishReadabilityTestsCode()))
        { return false; }
    const wxString CURRENT_TEST_KEY = ReadabilityMessages::MODIFIED_SMOG();

    if (GetTotalSentences() == 0)
        {
        LogMessage(wxString::Format(_(L"Unable to calculate %s: at least one sentence must be present in document."),
            GetReadabilityTests().get_test_short_name(CURRENT_TEST_KEY).c_str()),
            _(L"Error"), wxOK|wxICON_ERROR, wxEmptyString, true);
        GetReadabilityTests().include_test(CURRENT_TEST_KEY, false);
        return false;
        }

    try
        {
        const double gradeValue = readability::modified_smog(GetUnique3PlusSyllabicWordsNumeralsFullySyllabized(), GetTotalSentences());

        const auto theTest = GetReadabilityTests().find_test(CURRENT_TEST_KEY);
        if (!VerifyTestBeforeAdding(theTest))
            { return false; }

        const wxString numeralLabel = (GetTotalNumerals() > 0) ?
            _(L"Numerals are fully syllabized (i.e., sounded out) for this test.") :
            wxString{};
        const wxString description = HasUI() ?
            ProjectReportFormat::FormatTestResult(
                GetReadabilityMessageCatalog().GetGradeScaleDescription(gradeValue),
                theTest.first->get_test(),
                numeralLabel) :
            wxString{};

        wxString displayableGradeLevel = wxNumberFormatter::ToString(gradeValue, 1, wxNumberFormatter::Style::Style_NoTrailingZeroes);
        if (gradeValue == 13)
            { displayableGradeLevel += L"+"; }

        SetReadabilityTestResult(CURRENT_TEST_KEY, theTest.first->get_test().get_long_name().c_str(), description,
            std::make_pair(gradeValue, displayableGradeLevel),
            ReadabilityMessages::GetAgeFromUSGrade(gradeValue, GetReadabilityMessageCatalog().GetReadingAgeDisplay()),
            std::numeric_limits<double>::quiet_NaN(),
            std::numeric_limits<double>::quiet_NaN(), setFocus);
        }
    catch (...)
        {
        HandleFailedTestCalculation(CURRENT_TEST_KEY);
        return false;
        }

    GetReadabilityTests().include_test(CURRENT_TEST_KEY, true);
    return true;
    }

//-------------------------------------------------------
bool BaseProject::AddQuBambergerVanecekTest(const bool setFocus)
    {
    ClearReadabilityTestResult();
    if (!wxGetApp().GetLicenseAdmin().IsFeatureEnabled(wxGetApp().FeatureLanguagePackCode()) &&
        !wxGetApp().GetLicenseAdmin().IsFeatureEnabled(wxGetApp().FeatureProfessionalCode()))
        { return false; }
    const wxString CURRENT_TEST_KEY = ReadabilityMessages::QU();

    if (GetTotalSentences() == 0)
        {
        LogMessage(wxString::Format(_(L"Unable to calculate %s: at least one sentence must be present in document."),
            GetReadabilityTests().get_test_short_name(CURRENT_TEST_KEY).c_str()),
            _(L"Error"), wxOK|wxICON_ERROR, wxEmptyString, true);
        GetReadabilityTests().include_test(CURRENT_TEST_KEY, false);
        return false;
        }

    try
        {
        const double gradeValue = readability::quadratwurzelverfahren_bamberger_vanecek(GetTotalWords(), GetTotal3PlusSyllabicWordsNumeralsFullySyllabized(), GetTotalSentences());

        const auto theTest = GetReadabilityTests().find_test(CURRENT_TEST_KEY);
        if (!VerifyTestBeforeAdding(theTest))
            { return false; }

        const wxString numeralLabel = (GetTotalNumerals() > 0) ?
            _(L"Numerals are fully syllabized (i.e., sounded out) for this test.") :
            wxString{};
        const wxString description = HasUI() ?
            ProjectReportFormat::FormatTestResult(
                GetReadabilityMessageCatalog().GetGradeScaleDescription(gradeValue),
                theTest.first->get_test(),
                numeralLabel) :
            wxString{};

        wxString displayableGradeLevel = wxNumberFormatter::ToString(gradeValue, 1, wxNumberFormatter::Style::Style_NoTrailingZeroes);
        if (gradeValue == 19)
            { displayableGradeLevel += L"+"; }

        SetReadabilityTestResult(CURRENT_TEST_KEY, theTest.first->get_test().get_long_name().c_str(), description,
            std::make_pair(gradeValue, displayableGradeLevel),
            ReadabilityMessages::GetAgeFromUSGrade(gradeValue, GetReadabilityMessageCatalog().GetReadingAgeDisplay()),
            std::numeric_limits<double>::quiet_NaN(),
            std::numeric_limits<double>::quiet_NaN(), setFocus);
        }
    catch (...)
        {
        HandleFailedTestCalculation(CURRENT_TEST_KEY);
        return false;
        }

    GetReadabilityTests().include_test(CURRENT_TEST_KEY, true);
    return true;
    }

//-------------------------------------------------------
bool BaseProject::AddSmogBambergerVanecekTest(const bool setFocus)
    {
    ClearReadabilityTestResult();
    if (!wxGetApp().GetLicenseAdmin().IsFeatureEnabled(wxGetApp().FeatureLanguagePackCode()) &&
        !wxGetApp().GetLicenseAdmin().IsFeatureEnabled(wxGetApp().FeatureProfessionalCode()))
        { return false; }
    const wxString CURRENT_TEST_KEY = ReadabilityMessages::SMOG_BAMBERGER_VANECEK();

    if (GetTotalSentences() == 0)
        {
        LogMessage(wxString::Format(_(L"Unable to calculate %s: at least one sentence must be present in document."),
            GetReadabilityTests().get_test_short_name(CURRENT_TEST_KEY).c_str()),
            _(L"Error"), wxOK|wxICON_ERROR, wxEmptyString, true);
        GetReadabilityTests().include_test(CURRENT_TEST_KEY, false);
        return false;
        }

    try
        {
        const double gradeValue = readability::smog_bamberger_vanecek(GetTotal3PlusSyllabicWordsNumeralsFullySyllabized(), GetTotalSentences());

        const auto theTest = GetReadabilityTests().find_test(CURRENT_TEST_KEY);
        if (!VerifyTestBeforeAdding(theTest))
            { return false; }

        const wxString numeralLabel = (GetTotalNumerals() > 0) ?
            _(L"Numerals are fully syllabized (i.e., sounded out) for this test.") :
            wxString{};
        const wxString description = HasUI() ?
            ProjectReportFormat::FormatTestResult(
                GetReadabilityMessageCatalog().GetGradeScaleDescription(gradeValue),
                theTest.first->get_test(),
                numeralLabel) :
            wxString{};

        wxString displayableGradeLevel = wxNumberFormatter::ToString(gradeValue, 1, wxNumberFormatter::Style::Style_NoTrailingZeroes);
        if (gradeValue == 19)
            { displayableGradeLevel += L"+"; }

        SetReadabilityTestResult(CURRENT_TEST_KEY, theTest.first->get_test().get_long_name().c_str(), description,
            std::make_pair(gradeValue, displayableGradeLevel),
            ReadabilityMessages::GetAgeFromUSGrade(gradeValue, GetReadabilityMessageCatalog().GetReadingAgeDisplay()),
            std::numeric_limits<double>::quiet_NaN(),
            std::numeric_limits<double>::quiet_NaN(), setFocus);
        }
    catch (...)
        {
        HandleFailedTestCalculation(CURRENT_TEST_KEY);
        return false;
        }

    GetReadabilityTests().include_test(CURRENT_TEST_KEY, true);
    return true;
    }

//-------------------------------------------------------
bool BaseProject::AddSmogTest(const bool setFocus)
    {
    ClearReadabilityTestResult();
    if (!wxGetApp().GetLicenseAdmin().IsFeatureEnabled(wxGetApp().FeatureEnglishReadabilityTestsCode()))
        { return false; }
    const wxString CURRENT_TEST_KEY = ReadabilityMessages::SMOG();

    if (GetTotalSentences() == 0)
        {
        LogMessage(wxString::Format(_(L"Unable to calculate %s: at least one sentence must be present in document."),
            GetReadabilityTests().get_test_short_name(CURRENT_TEST_KEY).c_str()),
            _(L"Error"), wxOK|wxICON_ERROR, wxEmptyString, true);
        GetReadabilityTests().include_test(CURRENT_TEST_KEY, false);
        return false;
        }

    try
        {
        const double gradeValue = readability::smog(GetTotal3PlusSyllabicWordsNumeralsFullySyllabized(), GetTotalSentences());

        const auto theTest = GetReadabilityTests().find_test(CURRENT_TEST_KEY);
        if (!VerifyTestBeforeAdding(theTest))
            { return false; }

        const wxString numeralLabel = (GetTotalNumerals() > 0) ?
            _(L"Numerals are fully syllabized (i.e., sounded out) for this test.") :
            wxString{};
        const wxString description = HasUI() ?
            ProjectReportFormat::FormatTestResult(
                GetReadabilityMessageCatalog().GetGradeScaleDescription(gradeValue),
                theTest.first->get_test(),
                numeralLabel) :
            wxString{};

        wxString displayableGradeLevel = wxNumberFormatter::ToString(gradeValue, 1, wxNumberFormatter::Style::Style_NoTrailingZeroes);
        if (gradeValue == 19)
            { displayableGradeLevel += L"+"; }

        SetReadabilityTestResult(CURRENT_TEST_KEY, theTest.first->get_test().get_long_name().c_str(), description,
            std::make_pair(gradeValue, displayableGradeLevel),
            ReadabilityMessages::GetAgeFromUSGrade(gradeValue, GetReadabilityMessageCatalog().GetReadingAgeDisplay()),
            std::numeric_limits<double>::quiet_NaN(),
            std::numeric_limits<double>::quiet_NaN(), setFocus);
        }
    catch (...)
        {
        HandleFailedTestCalculation(CURRENT_TEST_KEY);
        return false;
        }

    GetReadabilityTests().include_test(CURRENT_TEST_KEY, true);
    return true;
    }

//-------------------------------------------------------
bool BaseProject::AddCrawfordTest(const bool setFocus)
    {
    ClearReadabilityTestResult();
    if (!wxGetApp().GetLicenseAdmin().IsFeatureEnabled(wxGetApp().FeatureLanguagePackCode()) &&
        !wxGetApp().GetLicenseAdmin().IsFeatureEnabled(wxGetApp().FeatureProfessionalCode()))
        { return false; }
    const wxString CURRENT_TEST_KEY = ReadabilityMessages::CRAWFORD();

    if (!GetTotalWords())
        {
        LogMessage(wxString::Format(_(L"Unable to calculate %s: at least one word must be present in document."),
            GetReadabilityTests().get_test_short_name(CURRENT_TEST_KEY).c_str()),
            _(L"Error"), wxOK|wxICON_ERROR, wxEmptyString, true);
        GetReadabilityTests().include_test(CURRENT_TEST_KEY, false);
        return false;
        }

    try
        {
        const double gradeValue = readability::crawford(GetTotalWords(), GetTotalSyllables(), GetTotalSentences());

        const auto theTest = GetReadabilityTests().find_test(CURRENT_TEST_KEY);
        if (!VerifyTestBeforeAdding(theTest))
            { return false; }

        const wxString numeralLabel = (GetTotalNumerals() > 0) ?
            _(L"Numerals are fully syllabized (i.e., sounded out) for this test.") :
            wxString{};
        const wxString description = HasUI() ?
            ProjectReportFormat::FormatTestResult(
                GetReadabilityMessageCatalog().GetGradeScaleDescription(gradeValue),
                theTest.first->get_test(),
                numeralLabel) :
            wxString{};

        wxString displayableGradeLevel = wxNumberFormatter::ToString(gradeValue, 1, wxNumberFormatter::Style::Style_NoTrailingZeroes);
        if (gradeValue == 19)
            { displayableGradeLevel += L"+"; }

        SetReadabilityTestResult(CURRENT_TEST_KEY, theTest.first->get_test().get_long_name().c_str(), description,
            std::make_pair(gradeValue, displayableGradeLevel),
            ReadabilityMessages::GetAgeFromUSGrade(gradeValue, GetReadabilityMessageCatalog().GetReadingAgeDisplay()),
            std::numeric_limits<double>::quiet_NaN(),
            std::numeric_limits<double>::quiet_NaN(), setFocus);
        }
    catch (...)
        {
        HandleFailedTestCalculation(CURRENT_TEST_KEY);
        return false;
        }

    AddCrawfordGraph(setFocus);

    GetReadabilityTests().include_test(CURRENT_TEST_KEY, true);
    return true;
    }

//-------------------------------------------------------
bool BaseProject::AddNeueWienerSachtextformel1(const bool setFocus)
    {
    ClearReadabilityTestResult();
    if (!wxGetApp().GetLicenseAdmin().IsFeatureEnabled(wxGetApp().FeatureLanguagePackCode()) &&
        !wxGetApp().GetLicenseAdmin().IsFeatureEnabled(wxGetApp().FeatureProfessionalCode()))
        { return false; }
    const wxString CURRENT_TEST_KEY = ReadabilityMessages::NEUE_WIENER_SACHTEXTFORMEL1();

    if (!GetTotalWords())
        {
        LogMessage(wxString::Format(_(L"Unable to calculate %s: at least one word must be present in document."),
            GetReadabilityTests().get_test_short_name(CURRENT_TEST_KEY).c_str()),
            _(L"Error"), wxOK|wxICON_ERROR, wxEmptyString, true);
        GetReadabilityTests().include_test(CURRENT_TEST_KEY, false);
        return false;
        }
    if (!GetTotalSentences() )
        {
        LogMessage(wxString::Format(_(L"Unable to calculate %s: at least one sentence must be present in document."),
            GetReadabilityTests().get_test_short_name(CURRENT_TEST_KEY).c_str()),
            _(L"Error"), wxOK|wxICON_ERROR, wxEmptyString, true);
        GetReadabilityTests().include_test(CURRENT_TEST_KEY, false);
        return false;
        }

    try
        {
        const double gradeValue = readability::neue_wiener_sachtextformel_1(GetTotalWords(),
                        GetTotalMonoSyllabicWords(),
                        GetTotal3PlusSyllabicWords(),
                        GetTotalHardLixRixWords(),
                        GetTotalSentences());

        const auto theTest = GetReadabilityTests().find_test(CURRENT_TEST_KEY);
        if (!VerifyTestBeforeAdding(theTest))
            { return false; }

        const wxString description = HasUI() ?
            ProjectReportFormat::FormatTestResult(
                GetReadabilityMessageCatalog().GetGradeScaleDescription(gradeValue),
                theTest.first->get_test()) :
            wxString{};

        wxString displayableGradeLevel = wxNumberFormatter::ToString(gradeValue, 1, wxNumberFormatter::Style::Style_NoTrailingZeroes);
        if (gradeValue == 19)
            { displayableGradeLevel += L"+"; }

        SetReadabilityTestResult(CURRENT_TEST_KEY, theTest.first->get_test().get_long_name().c_str(),
            description,
            std::make_pair(gradeValue, displayableGradeLevel),
            ReadabilityMessages::GetAgeFromUSGrade(gradeValue, GetReadabilityMessageCatalog().GetReadingAgeDisplay()),
            std::numeric_limits<double>::quiet_NaN(),
            std::numeric_limits<double>::quiet_NaN(), setFocus);
        }
    catch (...)
        {
        HandleFailedTestCalculation(CURRENT_TEST_KEY);
        return false;
        }

    GetReadabilityTests().include_test(CURRENT_TEST_KEY, true);
    return true;
    }

//-------------------------------------------------------
bool BaseProject::AddNeueWienerSachtextformel2(const bool setFocus)
    {
    ClearReadabilityTestResult();
    if (!wxGetApp().GetLicenseAdmin().IsFeatureEnabled(wxGetApp().FeatureLanguagePackCode()) &&
        !wxGetApp().GetLicenseAdmin().IsFeatureEnabled(wxGetApp().FeatureProfessionalCode()))
        { return false; }
    const wxString CURRENT_TEST_KEY = ReadabilityMessages::NEUE_WIENER_SACHTEXTFORMEL2();

    if (!GetTotalWords())
        {
        LogMessage(wxString::Format(_(L"Unable to calculate %s: at least one word must be present in document."),
            GetReadabilityTests().get_test_short_name(CURRENT_TEST_KEY).c_str()),
            _(L"Error"), wxOK|wxICON_ERROR, wxEmptyString, true);
        GetReadabilityTests().include_test(CURRENT_TEST_KEY, false);
        return false;
        }
    if (!GetTotalSentences() )
        {
        LogMessage(wxString::Format(_(L"Unable to calculate %s: at least one sentence must be present in document."),
            GetReadabilityTests().get_test_short_name(CURRENT_TEST_KEY).c_str()),
            _(L"Error"), wxOK|wxICON_ERROR, wxEmptyString, true);
        GetReadabilityTests().include_test(CURRENT_TEST_KEY, false);
        return false;
        }

    try
        {
        const double gradeValue = readability::neue_wiener_sachtextformel_2(GetTotalWords(),
                        GetTotal3PlusSyllabicWords(),
                        GetTotalHardLixRixWords(),
                        GetTotalSentences());

        const auto theTest = GetReadabilityTests().find_test(CURRENT_TEST_KEY);
        if (!VerifyTestBeforeAdding(theTest))
            { return false; }

        const wxString description = HasUI() ?
            ProjectReportFormat::FormatTestResult(
                GetReadabilityMessageCatalog().GetGradeScaleDescription(gradeValue),
                theTest.first->get_test()) :
            wxString{};

        wxString displayableGradeLevel = wxNumberFormatter::ToString(gradeValue, 1, wxNumberFormatter::Style::Style_NoTrailingZeroes);
        if (gradeValue == 19)
            { displayableGradeLevel += L"+"; }

        SetReadabilityTestResult(CURRENT_TEST_KEY, theTest.first->get_test().get_long_name().c_str(),
            description,
            std::make_pair(gradeValue, displayableGradeLevel),
            ReadabilityMessages::GetAgeFromUSGrade(gradeValue, GetReadabilityMessageCatalog().GetReadingAgeDisplay()),
            std::numeric_limits<double>::quiet_NaN(),
            std::numeric_limits<double>::quiet_NaN(), setFocus);
        }
    catch (...)
        {
        HandleFailedTestCalculation(CURRENT_TEST_KEY);
        return false;
        }

    GetReadabilityTests().include_test(CURRENT_TEST_KEY, true);
    return true;
    }

//-------------------------------------------------------
bool BaseProject::AddNeueWienerSachtextformel3(const bool setFocus)
    {
    ClearReadabilityTestResult();
    if (!wxGetApp().GetLicenseAdmin().IsFeatureEnabled(wxGetApp().FeatureLanguagePackCode()) &&
        !wxGetApp().GetLicenseAdmin().IsFeatureEnabled(wxGetApp().FeatureProfessionalCode()))
        { return false; }
    const wxString CURRENT_TEST_KEY = ReadabilityMessages::NEUE_WIENER_SACHTEXTFORMEL3();

    if (!GetTotalWords())
        {
        LogMessage(wxString::Format(_(L"Unable to calculate %s: at least one word must be present in document."),
            GetReadabilityTests().get_test_short_name(CURRENT_TEST_KEY).c_str()),
            _(L"Error"), wxOK|wxICON_ERROR, wxEmptyString, true);
        GetReadabilityTests().include_test(CURRENT_TEST_KEY, false);
        return false;
        }
    if (!GetTotalSentences() )
        {
        LogMessage(wxString::Format(_(L"Unable to calculate %s: at least one sentence must be present in document."),
            GetReadabilityTests().get_test_short_name(CURRENT_TEST_KEY).c_str()),
            _(L"Error"), wxOK|wxICON_ERROR, wxEmptyString, true);
        GetReadabilityTests().include_test(CURRENT_TEST_KEY, false);
        return false;
        }

    try
        {
        const double gradeValue = readability::neue_wiener_sachtextformel_3(GetTotalWords(),
                        GetTotal3PlusSyllabicWords(),
                        GetTotalSentences());

        const auto theTest = GetReadabilityTests().find_test(CURRENT_TEST_KEY);
        if (!VerifyTestBeforeAdding(theTest))
            { return false; }

        const wxString description = HasUI() ?
            ProjectReportFormat::FormatTestResult(
                GetReadabilityMessageCatalog().GetGradeScaleDescription(gradeValue),
                theTest.first->get_test()) :
            wxString{};

        wxString displayableGradeLevel = wxNumberFormatter::ToString(gradeValue, 1, wxNumberFormatter::Style::Style_NoTrailingZeroes);
        if (gradeValue == 19)
            { displayableGradeLevel += L"+"; }

        SetReadabilityTestResult(CURRENT_TEST_KEY, theTest.first->get_test().get_long_name().c_str(),
            description,
            std::make_pair(gradeValue, displayableGradeLevel),
            ReadabilityMessages::GetAgeFromUSGrade(gradeValue, GetReadabilityMessageCatalog().GetReadingAgeDisplay()),
            std::numeric_limits<double>::quiet_NaN(),
            std::numeric_limits<double>::quiet_NaN(), setFocus);
        }
    catch (...)
        {
        HandleFailedTestCalculation(CURRENT_TEST_KEY);
        return false;
        }

    GetReadabilityTests().include_test(CURRENT_TEST_KEY, true);
    return true;
    }

//-------------------------------------------------------
bool BaseProject::AddWheelerSmithBambergerVanecekTest(const bool setFocus)
    {
    ClearReadabilityTestResult();
    if (!wxGetApp().GetLicenseAdmin().IsFeatureEnabled(wxGetApp().FeatureLanguagePackCode()) &&
        !wxGetApp().GetLicenseAdmin().IsFeatureEnabled(wxGetApp().FeatureProfessionalCode()))
        { return false; }
    const wxString CURRENT_TEST_KEY = ReadabilityMessages::WHEELER_SMITH_BAMBERGER_VANECEK();

    if (!GetTotalWords())
        {
        LogMessage(wxString::Format(_(L"Unable to calculate %s: at least one word must be present in document."),
            GetReadabilityTests().get_test_short_name(CURRENT_TEST_KEY).c_str()),
            _(L"Error"), wxOK|wxICON_ERROR, wxEmptyString, true);
        GetReadabilityTests().include_test(CURRENT_TEST_KEY, false);
        return false;
        }
    if (!GetTotalSentenceUnits() )
        {
        LogMessage(wxString::Format(_(L"Unable to calculate %s: at least one sentence must be present in document."),
            GetReadabilityTests().get_test_short_name(CURRENT_TEST_KEY).c_str()),
            _(L"Error"), wxOK|wxICON_ERROR, wxEmptyString, true);
        GetReadabilityTests().include_test(CURRENT_TEST_KEY, false);
        return false;
        }

    try
        {
        double index_score(0);
        const size_t gradeValue = readability::wheeler_smith_bamberger_vanecek(GetTotalWords(),
                        GetTotal3PlusSyllabicWords(),//German version uses 3+ syllable words, not 2+
                        GetTotalSentenceUnits(),
                        index_score);

        const auto theTest = GetReadabilityTests().find_test(CURRENT_TEST_KEY);
        if (!VerifyTestBeforeAdding(theTest))
            { return false; }

        const wxString description  = HasUI() ?
            ProjectReportFormat::FormatTestResult(
                GetReadabilityMessageCatalog().GetGradeScaleDescription(gradeValue),
                theTest.first->get_test(),
                ReadabilityMessages::GetUnitNote()) :
            wxString{};

        wxString displayableGradeValue = wxNumberFormatter::ToString(gradeValue, 0, wxNumberFormatter::Style::Style_NoTrailingZeroes);
        if (gradeValue == 10)
            { displayableGradeValue += L"+"; }

        SetReadabilityTestResult(CURRENT_TEST_KEY, theTest.first->get_test().get_long_name().c_str(),
            description,
            std::make_pair(gradeValue, displayableGradeValue),
            ReadabilityMessages::GetAgeFromUSGrade(gradeValue, GetReadabilityMessageCatalog().GetReadingAgeDisplay()),
            index_score,
            std::numeric_limits<double>::quiet_NaN(), setFocus);
        }
    catch (...)
        {
        HandleFailedTestCalculation(CURRENT_TEST_KEY);
        return false;
        }

    GetReadabilityTests().include_test(CURRENT_TEST_KEY, true);
    return true;
    }

//-------------------------------------------------------
bool BaseProject::AddWheelerSmithTest(const bool setFocus)
    {
    ClearReadabilityTestResult();
    if (!wxGetApp().GetLicenseAdmin().IsFeatureEnabled(wxGetApp().FeatureEnglishReadabilityTestsCode()))
        { return false; }
    const wxString CURRENT_TEST_KEY = ReadabilityMessages::WHEELER_SMITH();

    if (!GetTotalWords())
        {
        LogMessage(wxString::Format(_(L"Unable to calculate %s: at least one word must be present in document."),
            GetReadabilityTests().get_test_short_name(CURRENT_TEST_KEY).c_str()),
            _(L"Error"), wxOK|wxICON_ERROR, wxEmptyString, true);
        GetReadabilityTests().include_test(CURRENT_TEST_KEY, false);
        return false;
        }
    if (!GetTotalSentenceUnits() )
        {
        LogMessage(wxString::Format(_(L"Unable to calculate %s: at least one sentence must be present in document."),
            GetReadabilityTests().get_test_short_name(CURRENT_TEST_KEY).c_str()),
            _(L"Error"), wxOK|wxICON_ERROR, wxEmptyString, true);
        GetReadabilityTests().include_test(CURRENT_TEST_KEY, false);
        return false;
        }

    try
        {
        double index_score(0);
        const size_t gradeValue = readability::wheeler_smith(GetTotalWords(),
                        GetTotalWords()-GetTotalMonoSyllabicWords(),//Polysyllabic here means 2+ syllable words (or non-monosyllabic)
                        GetTotalSentenceUnits(),
                        index_score);

        const auto theTest = GetReadabilityTests().find_test(CURRENT_TEST_KEY);
        if (!VerifyTestBeforeAdding(theTest))
            { return false; }

        const wxString description  = HasUI() ?
            ProjectReportFormat::FormatTestResult(
                GetReadabilityMessageCatalog().GetGradeScaleDescription(gradeValue),
                theTest.first->get_test(),
                ReadabilityMessages::GetUnitNote()) :
            wxString{};

        wxString displayableGradeValue = wxNumberFormatter::ToString(gradeValue, 0, wxNumberFormatter::Style::Style_NoTrailingZeroes);
        if (gradeValue == 4)
            { displayableGradeValue += L"+"; }

        SetReadabilityTestResult(CURRENT_TEST_KEY, theTest.first->get_test().get_long_name().c_str(),
            description,
            std::make_pair(gradeValue, displayableGradeValue),
            ReadabilityMessages::GetAgeFromUSGrade(gradeValue, GetReadabilityMessageCatalog().GetReadingAgeDisplay()),
            index_score,
            std::numeric_limits<double>::quiet_NaN(), setFocus);
        }
    catch (...)
        {
        HandleFailedTestCalculation(CURRENT_TEST_KEY);
        return false;
        }

    GetReadabilityTests().include_test(CURRENT_TEST_KEY, true);
    return true;
    }

//-------------------------------------------------------
bool BaseProject::AddColemanLiauTest(const bool setFocus)
    {
    ClearReadabilityTestResult();
    if (!wxGetApp().GetLicenseAdmin().IsFeatureEnabled(wxGetApp().FeatureEnglishReadabilityTestsCode()))
        { return false; }
    const wxString CURRENT_TEST_KEY = ReadabilityMessages::COLEMAN_LIAU();

    if (!GetTotalWords())
        {
        LogMessage(wxString::Format(_(L"Unable to calculate %s: at least one word must be present in document."),
            GetReadabilityTests().get_test_short_name(CURRENT_TEST_KEY).c_str()),
            _(L"Error"), wxOK|wxICON_ERROR, wxEmptyString, true);
        GetReadabilityTests().include_test(CURRENT_TEST_KEY, false);
        return false;
        }
    if (!GetTotalSentences() )
        {
        LogMessage(wxString::Format(_(L"Unable to calculate %s: at least one sentence must be present in document."),
            GetReadabilityTests().get_test_short_name(CURRENT_TEST_KEY).c_str()),
            _(L"Error"), wxOK|wxICON_ERROR, wxEmptyString, true);
        GetReadabilityTests().include_test(CURRENT_TEST_KEY, false);
        return false;
        }

    try
        {
        double predictedClozeScore(0);
        const double gradeValue = readability::coleman_liau(GetTotalWords(),
                        GetTotalCharacters(),
                        GetTotalSentences(),
                        predictedClozeScore);
        predictedClozeScore *= 100; // convert to a percentage because the test returns a fractal value

        const auto theTest = GetReadabilityTests().find_test(CURRENT_TEST_KEY);
        if (!VerifyTestBeforeAdding(theTest))
            { return false; }

        const wxString description  = HasUI() ?
            ProjectReportFormat::FormatTestResult(
                GetReadabilityMessageCatalog().GetGradeScaleDescription(gradeValue) +
                L"<br /><br />" +
                ReadabilityMessages::GetPredictedClozeDescription(round_to_integer(predictedClozeScore)),
                theTest.first->get_test(),
                ReadabilityMessages::GetPredictedClozeNote() +
                L"<br /><br />" +
                ReadabilityMessages::GetPunctuationIgnoredNote()) :
            wxString{};

        wxString displayableGradeLevel = wxNumberFormatter::ToString(gradeValue, 1, wxNumberFormatter::Style::Style_NoTrailingZeroes);
        if (gradeValue == 19)
            { displayableGradeLevel += L"+"; }

        SetReadabilityTestResult(CURRENT_TEST_KEY, theTest.first->get_test().get_long_name().c_str(),
            description,
            std::make_pair(gradeValue, displayableGradeLevel),
            ReadabilityMessages::GetAgeFromUSGrade(gradeValue, GetReadabilityMessageCatalog().GetReadingAgeDisplay()),
            std::numeric_limits<double>::quiet_NaN(),
            predictedClozeScore, setFocus);
        }
    catch (...)
        {
        HandleFailedTestCalculation(CURRENT_TEST_KEY);
        return false;
        }

    GetReadabilityTests().include_test(CURRENT_TEST_KEY, true);
    return true;
    }

///Generic interface to add a test based on name or ID
//-------------------------------------------------------
bool BaseProject::AddStandardReadabilityTest(const wxString& id, const bool setFocus /*= true*/)
    {
    PROFILE();
    const auto theTest = GetReadabilityTests().get_test(id);
    if (!theTest.second)
        { return false; }
    if (HasUI() && theTest.first->get_test().get_id() == _DT(L"dale-chall-test") &&
        GetDaleChallTextExclusionMode() != SpecializedTestTextExclusion::UseSystemDefault)
        {
        if (wxGetApp().GetAppOptions().HasWarning(_DT(L"new-dale-chall-text-exclusion-differs-note")))
            {
            auto warningMsg = *wxGetApp().GetAppOptions().GetWarning(_DT(L"new-dale-chall-text-exclusion-differs-note"));
            warningMsg.SetMessage(wxString::Format(_(L"NOTE: %s uses a specialized method for excluding text that may differ from the system default.\nThis behavior can be changed from the \"Readability Scores\"->\"Test Options\" page of the Options and Project Properties dialogs."), GetReadabilityTests().get_test_long_name(id).c_str()));
            LogMessage(warningMsg, true);
            }
        }
    if (HasUI() && theTest.first->get_test().get_id() == _DT(L"harris-jacobson") &&
        GetHarrisJacobsonTextExclusionMode() != SpecializedTestTextExclusion::UseSystemDefault)
        {
        if (wxGetApp().GetAppOptions().HasWarning(_DT(L"harris-jacobson-text-exclusion-differs-note")))
            {
            auto warningMsg = *wxGetApp().GetAppOptions().GetWarning(_DT(L"harris-jacobson-text-exclusion-differs-note"));
            warningMsg.SetMessage(wxString::Format(_(L"NOTE: %s uses a specialized method for excluding text that may differ from the system default.\nThis behavior can be changed from the \"Readability Scores\"->\"Test Options\" page of the Options and Project Properties dialogs."), GetReadabilityTests().get_test_long_name(id).c_str()));
            LogMessage(warningMsg, true);
            }
        }
    std::vector<comparable_first_pair<int, AddTestFunction>>::const_iterator addTestFunction =
        std::lower_bound(m_standardTestFunctions.begin(), m_standardTestFunctions.end(),
            comparable_first_pair<int, AddTestFunction>(theTest.first->get_test().get_interface_id(),nullptr));
    //this vector is sorted (so lower_bound should work), but in case it fails then do a brute force search.
    if (addTestFunction == m_standardTestFunctions.end() ||
        addTestFunction->first != theTest.first->get_test().get_interface_id())
        {
        wxFAIL_MSG(L"Standard test function pointer not found via sorted search.");
        addTestFunction = std::find(m_standardTestFunctions.begin(), m_standardTestFunctions.end(),
            comparable_first_pair<int, AddTestFunction>(theTest.first->get_test().get_interface_id(),nullptr));
        }
    if (addTestFunction != m_standardTestFunctions.end() &&
        addTestFunction->first == theTest.first->get_test().get_interface_id() &&
        addTestFunction->second != nullptr)
        {
        return (*this.*addTestFunction->second)(setFocus);
        }
    else
        {
        wxFAIL_MSG(wxString::Format(L"%s test function pointer not found, unable to add test.",id ));
        return false;
        }
    }

//-------------------------------------------------------
bool BaseProject::AddSpacheTest(const bool setFocus)
    {
    ClearReadabilityTestResult();
    if (!wxGetApp().GetLicenseAdmin().IsFeatureEnabled(wxGetApp().FeatureEnglishReadabilityTestsCode()))
        { return false; }
    const wxString CURRENT_TEST_KEY = ReadabilityMessages::SPACHE();

    if (!GetTotalWords())
        {
        LogMessage(wxString::Format(_(L"Unable to calculate %s: at least one word must be present in document."),
            GetReadabilityTests().get_test_short_name(CURRENT_TEST_KEY).c_str()),
            _(L"Error"), wxOK|wxICON_ERROR, wxEmptyString, true);
        GetReadabilityTests().include_test(CURRENT_TEST_KEY, false);
        return false;
        }
    if (!GetTotalSentences() )
        {
        LogMessage(wxString::Format(_(L"Unable to calculate %s: at least one sentence must be present in document."),
            GetReadabilityTests().get_test_short_name(CURRENT_TEST_KEY).c_str()),
            _(L"Error"), wxOK|wxICON_ERROR, wxEmptyString, true);
        GetReadabilityTests().include_test(CURRENT_TEST_KEY, false);
        return false;
        }

    try
        {
        const double gradeValue = readability::spache(GetTotalWords(),
                        GetTotalUniqueHardWordsSpache(),
                        GetTotalSentences() );

        const auto theTest = GetReadabilityTests().find_test(CURRENT_TEST_KEY);
        if (!VerifyTestBeforeAdding(theTest))
            { return false; }

        const wxString description = HasUI() ?
            ProjectReportFormat::FormatTestResult(
                GetReadabilityMessageCatalog().GetGradeScaleDescription(gradeValue),
                theTest.first->get_test()) :
            wxString{};

        wxString displayableGradeLevel = wxNumberFormatter::ToString(gradeValue, 1, wxNumberFormatter::Style::Style_NoTrailingZeroes);
        if (gradeValue == 19)
            { displayableGradeLevel += L"+"; }

        SetReadabilityTestResult(CURRENT_TEST_KEY, theTest.first->get_test().get_long_name().c_str(),
            description,
            std::make_pair(gradeValue, displayableGradeLevel),
            ReadabilityMessages::GetAgeFromUSGrade(gradeValue, GetReadabilityMessageCatalog().GetReadingAgeDisplay()),
            std::numeric_limits<double>::quiet_NaN(),
            std::numeric_limits<double>::quiet_NaN(), setFocus);
        }
    catch (...)
        {
        HandleFailedTestCalculation(CURRENT_TEST_KEY);
        return false;
        }

    GetReadabilityTests().include_test(CURRENT_TEST_KEY, true);
    return true;
    }

//-------------------------------------------------------
bool BaseProject::AddNewFogCountTest(const bool setFocus)
    {
    ClearReadabilityTestResult();
    if (!wxGetApp().GetLicenseAdmin().IsFeatureEnabled(wxGetApp().FeatureEnglishReadabilityTestsCode()))
        { return false; }
    const wxString CURRENT_TEST_KEY = ReadabilityMessages::NEW_FOG();

    if (GetTotalWords() == 0)
        {
        LogMessage(wxString::Format(_(L"Unable to calculate %s: at least one word must be present in document."),
            GetReadabilityTests().get_test_short_name(CURRENT_TEST_KEY).c_str()),
            _(L"Error"), wxOK|wxICON_ERROR, wxEmptyString, true);
        GetReadabilityTests().include_test(CURRENT_TEST_KEY, false);
        return false;
        }
    if ((FogUseSentenceUnits() && GetTotalSentenceUnits() == 0) ||
        GetTotalSentences() == 0)
        {
        LogMessage(wxString::Format(_(L"Unable to calculate %s: at least one sentence must be present in document."),
            GetReadabilityTests().get_test_short_name(CURRENT_TEST_KEY).c_str()),
            _(L"Error"), wxOK|wxICON_ERROR, wxEmptyString, true);
        GetReadabilityTests().include_test(CURRENT_TEST_KEY, false);
        return false;
        }

    try
        {
        const double gradeValue = readability::new_fog_count(GetTotalWords(),
                        GetTotalHardWordsFog(),
                        FogUseSentenceUnits() ? GetTotalSentenceUnits() : GetTotalSentences());

        const auto theTest = GetReadabilityTests().find_test(CURRENT_TEST_KEY);
        if (!VerifyTestBeforeAdding(theTest))
            { return false; }

        const wxString numeralLabel = (GetTotalNumerals() > 0 || GetTotalProperNouns() > 0) ?
            _(L"All numerals and proper nouns are treated as easy words for this test.") :
            wxString{};
        const wxString description = HasUI() ?
            ProjectReportFormat::FormatTestResult(
                GetReadabilityMessageCatalog().GetGradeScaleDescription(gradeValue),
                theTest.first->get_test(),
                numeralLabel) :
            wxString{};

        wxString displayableGradeLevel = wxNumberFormatter::ToString(gradeValue, 1, wxNumberFormatter::Style::Style_NoTrailingZeroes);
        if (gradeValue == 19)
            { displayableGradeLevel += L"+"; }

        SetReadabilityTestResult(CURRENT_TEST_KEY, theTest.first->get_test().get_long_name().c_str(), description,
            std::make_pair(gradeValue, displayableGradeLevel),
            ReadabilityMessages::GetAgeFromUSGrade(gradeValue, GetReadabilityMessageCatalog().GetReadingAgeDisplay()),
            std::numeric_limits<double>::quiet_NaN(),
            std::numeric_limits<double>::quiet_NaN(), setFocus);
        }
    catch (...)
        {
        HandleFailedTestCalculation(CURRENT_TEST_KEY);
        return false;
        }

    GetReadabilityTests().include_test(CURRENT_TEST_KEY, true);
    return true;
    }

//-------------------------------------------------------
bool BaseProject::AddPskFogTest(const bool setFocus)
    {
    ClearReadabilityTestResult();
    if (!wxGetApp().GetLicenseAdmin().IsFeatureEnabled(wxGetApp().FeatureEnglishReadabilityTestsCode()))
        { return false; }
    const wxString CURRENT_TEST_KEY = ReadabilityMessages::PSK_GUNNING_FOG();

    if (GetTotalWords() == 0)
        {
        LogMessage(wxString::Format(_(L"Unable to calculate %s: at least one word must be present in document."),
            GetReadabilityTests().get_test_short_name(CURRENT_TEST_KEY).c_str()),
            _(L"Error"), wxOK|wxICON_ERROR, wxEmptyString, true);
        GetReadabilityTests().include_test(CURRENT_TEST_KEY, false);
        return false;
        }
    if ((FogUseSentenceUnits() && GetTotalSentenceUnits() == 0) ||
        GetTotalSentences() == 0)
        {
        LogMessage(wxString::Format(_(L"Unable to calculate %s: at least one sentence must be present in document."),
            GetReadabilityTests().get_test_short_name(CURRENT_TEST_KEY).c_str()),
            _(L"Error"), wxOK|wxICON_ERROR, wxEmptyString, true);
        GetReadabilityTests().include_test(CURRENT_TEST_KEY, false);
        return false;
        }

    try
        {
        const double gradeValue = readability::psk_gunning_fog(GetTotalWords(),
                        GetTotalHardWordsFog(),
                        FogUseSentenceUnits() ? GetTotalSentenceUnits() : GetTotalSentences());

        const auto theTest = GetReadabilityTests().find_test(CURRENT_TEST_KEY);
        if (!VerifyTestBeforeAdding(theTest))
            { return false; }

        const wxString numeralLabel = (GetTotalNumerals() > 0 || GetTotalProperNouns() > 0) ?
            _(L"All numerals and proper nouns are treated as easy words for this test.") :
            wxString{};
        const wxString description = HasUI() ?
            ProjectReportFormat::FormatTestResult(
                GetReadabilityMessageCatalog().GetGradeScaleDescription(gradeValue),
                theTest.first->get_test(),
                numeralLabel) :
            wxString{};

        wxString displayableGradeLevel = wxNumberFormatter::ToString(gradeValue, 1, wxNumberFormatter::Style::Style_NoTrailingZeroes);
        if (gradeValue == 19)
            { displayableGradeLevel += L"+"; }

        SetReadabilityTestResult(CURRENT_TEST_KEY, theTest.first->get_test().get_long_name().c_str(), description,
            std::make_pair(gradeValue, displayableGradeLevel),
            ReadabilityMessages::GetAgeFromUSGrade(gradeValue, GetReadabilityMessageCatalog().GetReadingAgeDisplay()),
            std::numeric_limits<double>::quiet_NaN(),
            std::numeric_limits<double>::quiet_NaN(), setFocus);
        }
    catch (...)
        {
        HandleFailedTestCalculation(CURRENT_TEST_KEY);
        return false;
        }

    GetReadabilityTests().include_test(CURRENT_TEST_KEY, true);
    return true;
    }

//-------------------------------------------------------
bool BaseProject::AddFogTest(const bool setFocus)
    {
    ClearReadabilityTestResult();
    if (!wxGetApp().GetLicenseAdmin().IsFeatureEnabled(wxGetApp().FeatureEnglishReadabilityTestsCode()))
        { return false; }
    const wxString CURRENT_TEST_KEY = ReadabilityMessages::GUNNING_FOG();

    if (GetTotalWords() == 0)
        {
        LogMessage(wxString::Format(_(L"Unable to calculate %s: at least one word must be present in document."),
            GetReadabilityTests().get_test_short_name(CURRENT_TEST_KEY).c_str()),
            _(L"Error"), wxOK|wxICON_ERROR, wxEmptyString, true);
        GetReadabilityTests().include_test(CURRENT_TEST_KEY, false);
        return false;
        }
    if ((FogUseSentenceUnits() && GetTotalSentenceUnits() == 0) ||
        GetTotalSentences() == 0)
        {
        LogMessage(wxString::Format(_(L"Unable to calculate %s: at least one sentence must be present in document."),
            GetReadabilityTests().get_test_short_name(CURRENT_TEST_KEY).c_str()),
            _(L"Error"), wxOK|wxICON_ERROR, wxEmptyString, true);
        GetReadabilityTests().include_test(CURRENT_TEST_KEY, false);
        return false;
        }

    try
        {
        const double gradeValue = readability::gunning_fog(GetTotalWords(),
                        GetTotalHardWordsFog(),
                        FogUseSentenceUnits() ? GetTotalSentenceUnits() : GetTotalSentences());

        const auto theTest = GetReadabilityTests().find_test(CURRENT_TEST_KEY);
        if (!VerifyTestBeforeAdding(theTest))
            { return false; }

        const wxString numeralLabel = (GetTotalNumerals() > 0 || GetTotalProperNouns() > 0) ?
            _(L"All numerals and proper nouns are treated as easy words for this test.") :
            wxString{};
        const wxString description = HasUI() ?
            ProjectReportFormat::FormatTestResult(
                GetReadabilityMessageCatalog().GetGradeScaleDescription(gradeValue),
                theTest.first->get_test(),
                numeralLabel) :
            wxString{};

        wxString displayableGradeLevel = wxNumberFormatter::ToString(gradeValue, 1, wxNumberFormatter::Style::Style_NoTrailingZeroes);
        if (gradeValue == 19)
            { displayableGradeLevel += L"+"; }

        SetReadabilityTestResult(CURRENT_TEST_KEY, theTest.first->get_test().get_long_name().c_str(), description,
            std::make_pair(gradeValue, displayableGradeLevel),
            ReadabilityMessages::GetAgeFromUSGrade(gradeValue, GetReadabilityMessageCatalog().GetReadingAgeDisplay()),
            std::numeric_limits<double>::quiet_NaN(),
            std::numeric_limits<double>::quiet_NaN(), setFocus);
        }
    catch (...)
        {
        HandleFailedTestCalculation(CURRENT_TEST_KEY);
        return false;
        }

    GetReadabilityTests().include_test(CURRENT_TEST_KEY, true);
    return true;
    }

//-------------------------------------------------------
bool BaseProject::AddForcastTest(const bool setFocus)
    {
    ClearReadabilityTestResult();
    if (!wxGetApp().GetLicenseAdmin().IsFeatureEnabled(wxGetApp().FeatureEnglishReadabilityTestsCode()))
        { return false; }
    const wxString CURRENT_TEST_KEY = ReadabilityMessages::FORCAST();

    if (!GetTotalWords())
        {
        LogMessage(wxString::Format(_(L"Unable to calculate %s: at least one word must be present in document."),
            GetReadabilityTests().get_test_short_name(CURRENT_TEST_KEY).c_str()),
            _(L"Error"), wxOK|wxICON_ERROR, wxEmptyString, true);
        GetReadabilityTests().include_test(CURRENT_TEST_KEY, false);
        return false;
        }

    try
        {
        const double gradeValue = readability::forcast(GetTotalWords(),
            GetTotalMonoSyllabicWords() );

        const auto theTest = GetReadabilityTests().find_test(CURRENT_TEST_KEY);
        if (!VerifyTestBeforeAdding(theTest))
            { return false; }

        const wxString description  = HasUI() ?
            ProjectReportFormat::FormatTestResult(
                GetReadabilityMessageCatalog().GetGradeScaleDescription(gradeValue),
                theTest.first->get_test(),
                ReadabilityMessages::GetForcastNote()) :
            wxString{};

        wxString displayableGradeLevel = wxNumberFormatter::ToString(gradeValue, 1, wxNumberFormatter::Style::Style_NoTrailingZeroes);
        if (gradeValue == 19)
            { displayableGradeLevel += L"+"; }

        SetReadabilityTestResult(CURRENT_TEST_KEY, theTest.first->get_test().get_long_name().c_str(),
            description,
            std::make_pair(gradeValue, displayableGradeLevel),
            ReadabilityMessages::GetAgeFromUSGrade(gradeValue, GetReadabilityMessageCatalog().GetReadingAgeDisplay()),
            std::numeric_limits<double>::quiet_NaN(),
            std::numeric_limits<double>::quiet_NaN(), setFocus);
        }
    catch (...)
        {
        HandleFailedTestCalculation(CURRENT_TEST_KEY);
        return false;
        }

    GetReadabilityTests().include_test(CURRENT_TEST_KEY, true);
    return true;
    }

//-------------------------------------------------------
bool BaseProject::AddAmstadTest(const bool setFocus)
    {
    ClearReadabilityTestResult();
    if (!wxGetApp().GetLicenseAdmin().IsFeatureEnabled(wxGetApp().FeatureLanguagePackCode()) &&
        !wxGetApp().GetLicenseAdmin().IsFeatureEnabled(wxGetApp().FeatureProfessionalCode()))
        { return false; }
    const wxString CURRENT_TEST_KEY = ReadabilityMessages::AMSTAD();

    if (!GetTotalWords())
        {
        LogMessage(wxString::Format(_(L"Unable to calculate %s: at least one word must be present in document."),
            GetReadabilityTests().get_test_short_name(CURRENT_TEST_KEY).c_str()),
            _(L"Error"), wxOK|wxICON_ERROR, wxEmptyString, true);
        GetReadabilityTests().include_test(CURRENT_TEST_KEY, false);
        return false;
        }
    if (!GetTotalSentences() )
        {
        LogMessage(wxString::Format(_(L"Unable to calculate %s: at least one sentence must be present in document."),
            GetReadabilityTests().get_test_short_name(CURRENT_TEST_KEY).c_str()),
            _(L"Error"), wxOK|wxICON_ERROR, wxEmptyString, true);
        GetReadabilityTests().include_test(CURRENT_TEST_KEY, false);
        return false;
        }

    try
        {
        readability::flesch_difficulty diffLevel;
        const size_t val = readability::amstad(GetTotalWords(),
                                            (GetFleschNumeralSyllabizeMethod() == FleschNumeralSyllabize::NumeralIsOneSyllable) ?
                                                GetTotalSyllablesNumeralsOneSyllable() : GetTotalSyllables(),
                                            GetTotalSentences(),
                                            diffLevel);

        const auto theTest = GetReadabilityTests().find_test(CURRENT_TEST_KEY);
        if (!VerifyTestBeforeAdding(theTest))
            { return false; }

        const wxString description = HasUI() ?
            ProjectReportFormat::FormatTestResult(
                ReadabilityMessages::GetFleschDescription(diffLevel),
                theTest.first->get_test()) :
            wxString{};

        SetReadabilityTestResult(CURRENT_TEST_KEY, theTest.first->get_test().get_long_name().c_str(),
            description,
            std::make_pair(std::numeric_limits<double>::quiet_NaN(), wxEmptyString),
            wxEmptyString,
            val,
            std::numeric_limits<double>::quiet_NaN(), setFocus);
        }
    catch (...)
        {
        HandleFailedTestCalculation(CURRENT_TEST_KEY);
        return false;
        }

    GetReadabilityTests().include_test(CURRENT_TEST_KEY, true);

    return true;
    }

//-------------------------------------------------------
bool BaseProject::AddDanielsonBryan1Test(const bool setFocus)
    {
    ClearReadabilityTestResult();
    if (!wxGetApp().GetLicenseAdmin().IsFeatureEnabled(wxGetApp().FeatureEnglishReadabilityTestsCode()))
        { return false; }
    const wxString CURRENT_TEST_KEY = ReadabilityMessages::DANIELSON_BRYAN_1();

    if (!GetTotalWords())
        {
        LogMessage(wxString::Format(_(L"Unable to calculate %s: at least one word must be present in document."),
            GetReadabilityTests().get_test_short_name(CURRENT_TEST_KEY).c_str()),
            _(L"Error"), wxOK|wxICON_ERROR, wxEmptyString, true);
        GetReadabilityTests().include_test(CURRENT_TEST_KEY, false);
        return false;
        }
    if (!GetTotalSentences() )
        {
        LogMessage(wxString::Format(_(L"Unable to calculate %s: at least one sentence must be present in document."),
            GetReadabilityTests().get_test_short_name(CURRENT_TEST_KEY).c_str()),
            _(L"Error"), wxOK|wxICON_ERROR, wxEmptyString, true);
        GetReadabilityTests().include_test(CURRENT_TEST_KEY, false);
        return false;
        }

    try
        {
        const double gradeValue = readability::danielson_bryan_1(GetTotalWords(),
                                            GetTotalCharactersPlusPunctuation(),
                                            GetTotalSentences());

        const auto theTest = GetReadabilityTests().find_test(CURRENT_TEST_KEY);
        if (!VerifyTestBeforeAdding(theTest))
            { return false; }

        const wxString description = HasUI() ?
            ProjectReportFormat::FormatTestResult(
                GetReadabilityMessageCatalog().GetGradeScaleDescription(gradeValue),
                theTest.first->get_test()) :
            wxString{};

        wxString displayableGradeLevel = wxNumberFormatter::ToString(gradeValue, 1, wxNumberFormatter::Style::Style_NoTrailingZeroes);
        if (gradeValue == 19)
            { displayableGradeLevel += L"+"; }

        SetReadabilityTestResult(CURRENT_TEST_KEY, theTest.first->get_test().get_long_name().c_str(),
            description,
            std::make_pair(gradeValue, displayableGradeLevel),
            ReadabilityMessages::GetAgeFromUSGrade(gradeValue, GetReadabilityMessageCatalog().GetReadingAgeDisplay()),
            std::numeric_limits<double>::quiet_NaN(),
            std::numeric_limits<double>::quiet_NaN(), setFocus);
        }
    catch (...)
        {
        HandleFailedTestCalculation(CURRENT_TEST_KEY);
        return false;
        }

    GetReadabilityTests().include_test(CURRENT_TEST_KEY, true);
    return true;
    }

//-------------------------------------------------------
bool BaseProject::AddDanielsonBryan2Test(const bool setFocus)
    {
    ClearReadabilityTestResult();
    if (!wxGetApp().GetLicenseAdmin().IsFeatureEnabled(wxGetApp().FeatureEnglishReadabilityTestsCode()))
        { return false; }
    const wxString CURRENT_TEST_KEY = ReadabilityMessages::DANIELSON_BRYAN_2();

    if (!GetTotalWords())
        {
        LogMessage(wxString::Format(_(L"Unable to calculate %s: at least one word must be present in document."),
            GetReadabilityTests().get_test_short_name(CURRENT_TEST_KEY).c_str()),
            _(L"Error"), wxOK|wxICON_ERROR, wxEmptyString, true);
        GetReadabilityTests().include_test(CURRENT_TEST_KEY, false);
        return false;
        }
    if (!GetTotalSentences() )
        {
        LogMessage(wxString::Format(_(L"Unable to calculate %s: at least one sentence must be present in document."),
            GetReadabilityTests().get_test_short_name(CURRENT_TEST_KEY).c_str()),
            _(L"Error"), wxOK|wxICON_ERROR, wxEmptyString, true);
        GetReadabilityTests().include_test(CURRENT_TEST_KEY, false);
        return false;
        }

    try
        {
        const size_t val = readability::danielson_bryan_2(GetTotalWords(),
                                            GetTotalCharactersPlusPunctuation(),
                                            GetTotalSentences());
        const readability::flesch_difficulty diffLevel = readability::flesch_score_to_difficulty_level(val);

        const auto theTest = GetReadabilityTests().find_test(CURRENT_TEST_KEY);
        if (!VerifyTestBeforeAdding(theTest))
            { return false; }

        const wxString description = HasUI() ?
            ProjectReportFormat::FormatTestResult(
                ReadabilityMessages::GetDanielsonBryan2Description(diffLevel),
                theTest.first->get_test()) :
            wxString{};

        SetReadabilityTestResult(CURRENT_TEST_KEY, theTest.first->get_test().get_long_name().c_str(),
            description,
            std::make_pair(std::numeric_limits<double>::quiet_NaN(), wxEmptyString),
            wxEmptyString,
            val,
            std::numeric_limits<double>::quiet_NaN(), setFocus);
        }
    catch (...)
        {
        HandleFailedTestCalculation(CURRENT_TEST_KEY);
        return false;
        }

    AddDB2Plot(setFocus);

    GetReadabilityTests().include_test(CURRENT_TEST_KEY, true);

    return true;
    }

//-------------------------------------------------------
bool BaseProject::AddFleschTest(const bool setFocus)
    {
    ClearReadabilityTestResult();
    if (!wxGetApp().GetLicenseAdmin().IsFeatureEnabled(wxGetApp().FeatureEnglishReadabilityTestsCode()))
        { return false; }
    const wxString CURRENT_TEST_KEY = ReadabilityMessages::FLESCH();

    if (!GetTotalWords())
        {
        LogMessage(wxString::Format(_(L"Unable to calculate %s: at least one word must be present in document."),
            GetReadabilityTests().get_test_short_name(CURRENT_TEST_KEY).c_str()),
            _(L"Error"), wxOK|wxICON_ERROR, wxEmptyString, true);
        GetReadabilityTests().include_test(CURRENT_TEST_KEY, false);
        return false;
        }
    if (!GetTotalSentences() )
        {
        LogMessage(wxString::Format(_(L"Unable to calculate %s: at least one sentence must be present in document."),
            GetReadabilityTests().get_test_short_name(CURRENT_TEST_KEY).c_str()),
            _(L"Error"), wxOK|wxICON_ERROR, wxEmptyString, true);
        GetReadabilityTests().include_test(CURRENT_TEST_KEY, false);
        return false;
        }

    try
        {
        readability::flesch_difficulty diffLevel;
        const size_t val = readability::flesch_reading_ease(GetTotalWords(),
                                            (GetFleschNumeralSyllabizeMethod() == FleschNumeralSyllabize::NumeralIsOneSyllable) ?
                                                GetTotalSyllablesNumeralsOneSyllable() : GetTotalSyllables(),
                                            GetTotalSentences(),
                                            diffLevel);

        const auto theTest = GetReadabilityTests().find_test(CURRENT_TEST_KEY);
        if (!VerifyTestBeforeAdding(theTest))
            { return false; }

        const wxString description = HasUI() ?
            ProjectReportFormat::FormatTestResult(
                ReadabilityMessages::GetFleschDescription(diffLevel),
                theTest.first->get_test()) :
            wxString{};

        SetReadabilityTestResult(CURRENT_TEST_KEY, theTest.first->get_test().get_long_name().c_str(),
            description,
            std::make_pair(std::numeric_limits<double>::quiet_NaN(), wxEmptyString),
            wxEmptyString,
            val,
            std::numeric_limits<double>::quiet_NaN(), setFocus);
        }
    catch (...)
        {
        HandleFailedTestCalculation(CURRENT_TEST_KEY);
        return false;
        }

    AddFleschChart(setFocus);

    GetReadabilityTests().include_test(CURRENT_TEST_KEY, true);

    return true;
    }

//-------------------------------------------------------
bool BaseProject::AddFarrJenkinsPatersonTest(const bool setFocus)
    {
    ClearReadabilityTestResult();
    if (!wxGetApp().GetLicenseAdmin().IsFeatureEnabled(wxGetApp().FeatureEnglishReadabilityTestsCode()))
        { return false; }
    const wxString CURRENT_TEST_KEY = ReadabilityMessages::FARR_JENKINS_PATERSON();

    if (!GetTotalWords())
        {
        LogMessage(wxString::Format(_(L"Unable to calculate %s: at least one word must be present in document."),
            GetReadabilityTests().get_test_short_name(CURRENT_TEST_KEY).c_str()),
            _(L"Error"), wxOK|wxICON_ERROR, wxEmptyString, true);
        GetReadabilityTests().include_test(CURRENT_TEST_KEY, false);
        return false;
        }
    if (!GetTotalSentences() )
        {
        LogMessage(wxString::Format(_(L"Unable to calculate %s: at least one sentence must be present in document."),
            GetReadabilityTests().get_test_short_name(CURRENT_TEST_KEY).c_str()),
            _(L"Error"), wxOK|wxICON_ERROR, wxEmptyString, true);
        GetReadabilityTests().include_test(CURRENT_TEST_KEY, false);
        return false;
        }

    try
        {
        readability::flesch_difficulty diffLevel;
        const size_t val = readability::farr_jenkins_paterson(GetTotalWords(),
                                            GetTotalMonoSyllabicWords(),
                                            GetTotalSentences(),
                                            diffLevel);

        const auto theTest = GetReadabilityTests().find_test(CURRENT_TEST_KEY);
        if (!VerifyTestBeforeAdding(theTest))
            { return false; }

        const wxString description = HasUI() ?
            ProjectReportFormat::FormatTestResult(
                ReadabilityMessages::GetFleschDescription(diffLevel),
                theTest.first->get_test()) :
            wxString{};

        SetReadabilityTestResult(CURRENT_TEST_KEY, theTest.first->get_test().get_long_name().c_str(),
            description,
            std::make_pair(std::numeric_limits<double>::quiet_NaN(), wxEmptyString),
            wxEmptyString,
            val,
            std::numeric_limits<double>::quiet_NaN(), setFocus);
        }
    catch (...)
        {
        HandleFailedTestCalculation(CURRENT_TEST_KEY);
        return false;
        }

    GetReadabilityTests().include_test(CURRENT_TEST_KEY, true);
    return true;
    }

//-------------------------------------------------------
bool BaseProject::AddNewFarrJenkinsPatersonTest(const bool setFocus)
    {
    ClearReadabilityTestResult();
    if (!wxGetApp().GetLicenseAdmin().IsFeatureEnabled(wxGetApp().FeatureEnglishReadabilityTestsCode()))
        { return false; }
    const wxString CURRENT_TEST_KEY = ReadabilityMessages::NEW_FARR_JENKINS_PATERSON();

    if (!GetTotalWords())
        {
        LogMessage(wxString::Format(_(L"Unable to calculate %s: at least one word must be present in document."),
            GetReadabilityTests().get_test_short_name(CURRENT_TEST_KEY).c_str()),
            _(L"Error"), wxOK|wxICON_ERROR, wxEmptyString, true);
        GetReadabilityTests().include_test(CURRENT_TEST_KEY, false);
        return false;
        }
    if (!GetTotalSentences() )
        {
        LogMessage(wxString::Format(_(L"Unable to calculate %s: at least one sentence must be present in document."),
            GetReadabilityTests().get_test_short_name(CURRENT_TEST_KEY).c_str()),
            _(L"Error"), wxOK|wxICON_ERROR, wxEmptyString, true);
        GetReadabilityTests().include_test(CURRENT_TEST_KEY, false);
        return false;
        }

    try
        {
        const double gradeValue = readability::new_farr_jenkins_paterson(GetTotalWords(),
                                            GetTotalMonoSyllabicWords(),
                                            GetTotalSentences());

        const auto theTest = GetReadabilityTests().find_test(CURRENT_TEST_KEY);
        if (!VerifyTestBeforeAdding(theTest))
            { return false; }

        const wxString description = HasUI() ?
            ProjectReportFormat::FormatTestResult(
                GetReadabilityMessageCatalog().GetGradeScaleDescription(gradeValue),
                theTest.first->get_test()) :
            wxString{};

        wxString displayableGradeLevel = wxNumberFormatter::ToString(gradeValue, 1, wxNumberFormatter::Style::Style_NoTrailingZeroes);
        if (gradeValue == 19)
            { displayableGradeLevel += L"+"; }

        SetReadabilityTestResult(CURRENT_TEST_KEY, theTest.first->get_test().get_long_name().c_str(),
            description,
            std::make_pair(gradeValue, displayableGradeLevel),
            ReadabilityMessages::GetAgeFromUSGrade(gradeValue, GetReadabilityMessageCatalog().GetReadingAgeDisplay()),
            std::numeric_limits<double>::quiet_NaN(),
            std::numeric_limits<double>::quiet_NaN(), setFocus);
        }
    catch (...)
        {
        HandleFailedTestCalculation(CURRENT_TEST_KEY);
        return false;
        }

    GetReadabilityTests().include_test(CURRENT_TEST_KEY, true);
    return true;
    }

//-------------------------------------------------------
bool BaseProject::AddPskFarrJenkinsPatersonTest(const bool setFocus)
    {
    ClearReadabilityTestResult();
    if (!wxGetApp().GetLicenseAdmin().IsFeatureEnabled(wxGetApp().FeatureEnglishReadabilityTestsCode()))
        { return false; }
    const wxString CURRENT_TEST_KEY = ReadabilityMessages::PSK_FARR_JENKINS_PATERSON();

    if (!GetTotalWords())
        {
        LogMessage(wxString::Format(_(L"Unable to calculate %s: at least one word must be present in document."),
            GetReadabilityTests().get_test_short_name(CURRENT_TEST_KEY).c_str()),
            _(L"Error"), wxOK|wxICON_ERROR, wxEmptyString, true);
        GetReadabilityTests().include_test(CURRENT_TEST_KEY, false);
        return false;
        }
    if (!GetTotalSentences() )
        {
        LogMessage(wxString::Format(_(L"Unable to calculate %s: at least one sentence must be present in document."),
            GetReadabilityTests().get_test_short_name(CURRENT_TEST_KEY).c_str()),
            _(L"Error"), wxOK|wxICON_ERROR, wxEmptyString, true);
        GetReadabilityTests().include_test(CURRENT_TEST_KEY, false);
        return false;
        }

    try
        {
        const double gradeValue = readability::powers_sumner_kearl_farr_jenkins_paterson(GetTotalWords(),
                                                GetTotalMonoSyllabicWords(),
                                                GetTotalSentences() );

        const auto theTest = GetReadabilityTests().find_test(CURRENT_TEST_KEY);
        if (!VerifyTestBeforeAdding(theTest))
            { return false; }

        const wxString description = HasUI() ?
            ProjectReportFormat::FormatTestResult(
                GetReadabilityMessageCatalog().GetGradeScaleDescription(gradeValue),
                theTest.first->get_test()) :
            wxString{};

        wxString displayableGradeLevel = wxNumberFormatter::ToString(gradeValue, 1, wxNumberFormatter::Style::Style_NoTrailingZeroes);
        if (gradeValue == 19)
            { displayableGradeLevel += L"+"; }

        SetReadabilityTestResult(CURRENT_TEST_KEY, theTest.first->get_test().get_long_name().c_str(),
            description,
            std::make_pair(gradeValue, displayableGradeLevel),
            ReadabilityMessages::GetAgeFromUSGrade(gradeValue, GetReadabilityMessageCatalog().GetReadingAgeDisplay()),
            std::numeric_limits<double>::quiet_NaN(),
            std::numeric_limits<double>::quiet_NaN(), setFocus);
        }
    catch (...)
        {
        HandleFailedTestCalculation(CURRENT_TEST_KEY);
        return false;
        }

    GetReadabilityTests().include_test(CURRENT_TEST_KEY, true);
    return true;
    }

//-------------------------------------------------------
bool BaseProject::AddFleschKincaidSimplifiedTest(const bool setFocus)
    {
    ClearReadabilityTestResult();
    if (!wxGetApp().GetLicenseAdmin().IsFeatureEnabled(wxGetApp().FeatureEnglishReadabilityTestsCode()))
        { return false; }
    const wxString CURRENT_TEST_KEY = ReadabilityMessages::FLESCH_KINCAID_SIMPLIFIED();

    if (!GetTotalWords())
        {
        LogMessage(wxString::Format(_(L"Unable to calculate %s: at least one word must be present in document."),
            GetReadabilityTests().get_test_short_name(CURRENT_TEST_KEY).c_str()),
            _(L"Error"), wxOK|wxICON_ERROR, wxEmptyString, true);
        GetReadabilityTests().include_test(CURRENT_TEST_KEY, false);
        return false;
        }
    if (!GetTotalSentences() )
        {
        LogMessage(wxString::Format(_(L"Unable to calculate %s: at least one sentence must be present in document."),
            GetReadabilityTests().get_test_short_name(CURRENT_TEST_KEY).c_str()),
            _(L"Error"), wxOK|wxICON_ERROR, wxEmptyString, true);
        GetReadabilityTests().include_test(CURRENT_TEST_KEY, false);
        return false;
        }

    try
        {
        const double gradeValue = readability::flesch_kincaid_simplified(GetTotalWords(),
                                                (GetFleschKincaidNumeralSyllabizeMethod() == FleschKincaidNumeralSyllabize::FleschKincaidNumeralSoundOutEachDigit) ?
                                                    GetTotalSyllablesNumeralsFullySyllabized() : GetTotalSyllables(),
                                                GetTotalSentences() );

        const auto theTest = GetReadabilityTests().find_test(CURRENT_TEST_KEY);
        if (!VerifyTestBeforeAdding(theTest))
            { return false; }

        const wxString description = HasUI() ?
            ProjectReportFormat::FormatTestResult(
                GetReadabilityMessageCatalog().GetGradeScaleDescription(gradeValue),
                theTest.first->get_test()) :
            wxString{};

        wxString displayableGradeLevel = wxNumberFormatter::ToString(gradeValue, 1, wxNumberFormatter::Style::Style_NoTrailingZeroes);
        if (gradeValue == 19)
            { displayableGradeLevel += L"+"; }

        SetReadabilityTestResult(CURRENT_TEST_KEY, theTest.first->get_test().get_long_name().c_str(),
            description,
            std::make_pair(gradeValue, displayableGradeLevel),
            ReadabilityMessages::GetAgeFromUSGrade(gradeValue, GetReadabilityMessageCatalog().GetReadingAgeDisplay()),
            std::numeric_limits<double>::quiet_NaN(),
            std::numeric_limits<double>::quiet_NaN(), setFocus);
        }
    catch (...)
        {
        HandleFailedTestCalculation(CURRENT_TEST_KEY);
        return false;
        }

    GetReadabilityTests().include_test(CURRENT_TEST_KEY, true);
    return true;
    }

//-------------------------------------------------------
bool BaseProject::AddFleschKincaidTest(const bool setFocus)
    {
    ClearReadabilityTestResult();
    if (!wxGetApp().GetLicenseAdmin().IsFeatureEnabled(wxGetApp().FeatureEnglishReadabilityTestsCode()))
        { return false; }
    const wxString CURRENT_TEST_KEY = ReadabilityMessages::FLESCH_KINCAID();

    if (!GetTotalWords())
        {
        LogMessage(wxString::Format(_(L"Unable to calculate %s: at least one word must be present in document."),
            GetReadabilityTests().get_test_short_name(CURRENT_TEST_KEY).c_str()),
            _(L"Error"), wxOK|wxICON_ERROR, wxEmptyString, true);
        GetReadabilityTests().include_test(CURRENT_TEST_KEY, false);
        return false;
        }
    if (!GetTotalSentences() )
        {
        LogMessage(wxString::Format(_(L"Unable to calculate %s: at least one sentence must be present in document."),
            GetReadabilityTests().get_test_short_name(CURRENT_TEST_KEY).c_str()),
            _(L"Error"), wxOK|wxICON_ERROR, wxEmptyString, true);
        GetReadabilityTests().include_test(CURRENT_TEST_KEY, false);
        return false;
        }

    try
        {
        const double gradeValue = readability::flesch_kincaid(GetTotalWords(),
                                                (GetFleschKincaidNumeralSyllabizeMethod() == FleschKincaidNumeralSyllabize::FleschKincaidNumeralSoundOutEachDigit) ?
                                                    GetTotalSyllablesNumeralsFullySyllabized() : GetTotalSyllables(),
                                                GetTotalSentences() );

        const auto theTest = GetReadabilityTests().find_test(CURRENT_TEST_KEY);
        if (!VerifyTestBeforeAdding(theTest))
            { return false; }

        const wxString description = HasUI() ?
            ProjectReportFormat::FormatTestResult(
                GetReadabilityMessageCatalog().GetGradeScaleDescription(gradeValue),
                theTest.first->get_test()) :
            wxString{};

        wxString displayableGradeLevel = wxNumberFormatter::ToString(gradeValue, 1, wxNumberFormatter::Style::Style_NoTrailingZeroes);
        if (gradeValue == 19)
            { displayableGradeLevel += L"+"; }

        SetReadabilityTestResult(CURRENT_TEST_KEY, theTest.first->get_test().get_long_name().c_str(),
            description,
            std::make_pair(gradeValue, displayableGradeLevel),
            ReadabilityMessages::GetAgeFromUSGrade(gradeValue, GetReadabilityMessageCatalog().GetReadingAgeDisplay()),
            std::numeric_limits<double>::quiet_NaN(),
            std::numeric_limits<double>::quiet_NaN(), setFocus);
        }
    catch (...)
        {
        HandleFailedTestCalculation(CURRENT_TEST_KEY);
        return false;
        }

    GetReadabilityTests().include_test(CURRENT_TEST_KEY, true);
    return true;
    }

//-------------------------------------------------------
bool BaseProject::AddPskFleschTest(const bool setFocus)
    {
    ClearReadabilityTestResult();
    if (!wxGetApp().GetLicenseAdmin().IsFeatureEnabled(wxGetApp().FeatureEnglishReadabilityTestsCode()))
        { return false; }
    const wxString CURRENT_TEST_KEY = ReadabilityMessages::PSK_FLESCH();

    if (!GetTotalWords())
        {
        LogMessage(wxString::Format(_(L"Unable to calculate %s: at least one word must be present in document."),
            GetReadabilityTests().get_test_short_name(CURRENT_TEST_KEY).c_str()),
            _(L"Error"), wxOK|wxICON_ERROR, wxEmptyString, true);
        GetReadabilityTests().include_test(CURRENT_TEST_KEY, false);
        return false;
        }
    if (!GetTotalSentences() )
        {
        LogMessage(wxString::Format(_(L"Unable to calculate %s: at least one sentence must be present in document."),
            GetReadabilityTests().get_test_short_name(CURRENT_TEST_KEY).c_str()),
            _(L"Error"), wxOK|wxICON_ERROR, wxEmptyString, true);
        GetReadabilityTests().include_test(CURRENT_TEST_KEY, false);
        return false;
        }

    try
        {
        const double gradeValue = readability::powers_sumner_kearl_flesch(GetTotalWords(),
                                (GetFleschNumeralSyllabizeMethod() == FleschNumeralSyllabize::NumeralIsOneSyllable) ?
                                    GetTotalSyllablesNumeralsOneSyllable() : GetTotalSyllables(),
                                GetTotalSentences() );

        const auto theTest = GetReadabilityTests().find_test(CURRENT_TEST_KEY);
        if (!VerifyTestBeforeAdding(theTest))
            { return false; }

        const wxString description = HasUI() ?
            ProjectReportFormat::FormatTestResult(
                GetReadabilityMessageCatalog().GetGradeScaleDescription(gradeValue),
                theTest.first->get_test()) :
            wxString{};

        wxString displayableGradeLevel = wxNumberFormatter::ToString(gradeValue, 1, wxNumberFormatter::Style::Style_NoTrailingZeroes);
        if (gradeValue == 19)
            { displayableGradeLevel += L"+"; }

        SetReadabilityTestResult(CURRENT_TEST_KEY,
            wxString(theTest.first->get_test().get_long_name().c_str()),
            description,
            std::make_pair(gradeValue, displayableGradeLevel),
            ReadabilityMessages::GetAgeFromUSGrade(gradeValue, GetReadabilityMessageCatalog().GetReadingAgeDisplay()),
            std::numeric_limits<double>::quiet_NaN(),
            std::numeric_limits<double>::quiet_NaN(), setFocus);
        }
    catch (...)
        {
        HandleFailedTestCalculation(CURRENT_TEST_KEY);
        return false;
        }

    GetReadabilityTests().include_test(CURRENT_TEST_KEY, true);
    return true;
    }

//-------------------------------------------------------
bool BaseProject::AddAriTest(const bool setFocus)
    {
    ClearReadabilityTestResult();
    if (!wxGetApp().GetLicenseAdmin().IsFeatureEnabled(wxGetApp().FeatureEnglishReadabilityTestsCode()))
        { return false; }
    const wxString CURRENT_TEST_KEY = ReadabilityMessages::ARI();

    if (!GetTotalWords())
        {
        LogMessage(wxString::Format(_(L"Unable to calculate %s: at least one word must be present in document."),
            GetReadabilityTests().get_test_short_name(CURRENT_TEST_KEY).c_str()),
            _(L"Error"), wxOK|wxICON_ERROR, wxEmptyString, true);
        GetReadabilityTests().include_test(CURRENT_TEST_KEY, false);
        return false;
        }
    if (!GetTotalSentences())
        {
        LogMessage(wxString::Format(_(L"Unable to calculate %s: at least one sentence must be present in document."),
            GetReadabilityTests().get_test_short_name(CURRENT_TEST_KEY).c_str()),
            _(L"Error"), wxOK|wxICON_ERROR, wxEmptyString, true);
        GetReadabilityTests().include_test(CURRENT_TEST_KEY, false);
        return false;
        }

    try
        {
        const double gradeValue = readability::automated_readability_index(GetTotalWords(),
                    GetTotalCharactersPlusPunctuation(),
                    GetTotalSentences() );

        const auto theTest = GetReadabilityTests().find_test(CURRENT_TEST_KEY);
        if (!VerifyTestBeforeAdding(theTest))
            { return false; }

        wxString displayableGradeLevel = wxNumberFormatter::ToString(gradeValue, 1, wxNumberFormatter::Style::Style_NoTrailingZeroes);
        if (gradeValue == 19)
            { displayableGradeLevel += L"+"; }

        const wxString description = HasUI() ?
            ProjectReportFormat::FormatTestResult(
                GetReadabilityMessageCatalog().GetGradeScaleDescription(gradeValue),
                theTest.first->get_test()) :
            wxString{};

        SetReadabilityTestResult(CURRENT_TEST_KEY, theTest.first->get_test().get_long_name().c_str(),
            description,
            std::make_pair(gradeValue, displayableGradeLevel),
            ReadabilityMessages::GetAgeFromUSGrade(gradeValue, GetReadabilityMessageCatalog().GetReadingAgeDisplay()),
            std::numeric_limits<double>::quiet_NaN(),
            std::numeric_limits<double>::quiet_NaN(), setFocus);
        }
    catch (...)
        {
        HandleFailedTestCalculation(CURRENT_TEST_KEY);
        return false;
        }

    GetReadabilityTests().include_test(CURRENT_TEST_KEY, true);
    return true;
    }

//-------------------------------------------------------
bool BaseProject::AddNewAriTest(const bool setFocus)
    {
    ClearReadabilityTestResult();
    if (!wxGetApp().GetLicenseAdmin().IsFeatureEnabled(wxGetApp().FeatureEnglishReadabilityTestsCode()))
        { return false; }
    const wxString CURRENT_TEST_KEY = ReadabilityMessages::NEW_ARI();

    if (!GetTotalWords())
        {
        LogMessage(wxString::Format(_(L"Unable to calculate %s: at least one word must be present in document."),
            GetReadabilityTests().get_test_short_name(CURRENT_TEST_KEY).c_str()),
            _(L"Error"), wxOK|wxICON_ERROR, wxEmptyString, true);
        GetReadabilityTests().include_test(CURRENT_TEST_KEY, false);
        return false;
        }
    if (!GetTotalSentences())
        {
        LogMessage(wxString::Format(_(L"Unable to calculate %s: at least one sentence must be present in document."),
            GetReadabilityTests().get_test_short_name(CURRENT_TEST_KEY).c_str()),
            _(L"Error"), wxOK|wxICON_ERROR, wxEmptyString, true);
        GetReadabilityTests().include_test(CURRENT_TEST_KEY, false);
        return false;
        }

    try
        {
        const double gradeValue = readability::new_automated_readability_index(GetTotalWords(),
                    GetTotalCharactersPlusPunctuation(),
                    GetTotalSentences() );

        const auto theTest = GetReadabilityTests().find_test(CURRENT_TEST_KEY);
        if (!VerifyTestBeforeAdding(theTest))
            { return false; }

        const wxString description = HasUI() ?
            ProjectReportFormat::FormatTestResult(
                GetReadabilityMessageCatalog().GetGradeScaleDescription(gradeValue),
                theTest.first->get_test()) :
            wxString{};

        wxString displayableGradeLevel = wxNumberFormatter::ToString(gradeValue, 1, wxNumberFormatter::Style::Style_NoTrailingZeroes);
        if (gradeValue == 19)
            { displayableGradeLevel += L"+"; }

        SetReadabilityTestResult(CURRENT_TEST_KEY, theTest.first->get_test().get_long_name().c_str(),
            description,
            std::make_pair(gradeValue, displayableGradeLevel),
            ReadabilityMessages::GetAgeFromUSGrade(gradeValue, GetReadabilityMessageCatalog().GetReadingAgeDisplay()),
            std::numeric_limits<double>::quiet_NaN(),
            std::numeric_limits<double>::quiet_NaN(), setFocus);
        }
    catch (...)
        {
        HandleFailedTestCalculation(CURRENT_TEST_KEY);
        return false;
        }

    GetReadabilityTests().include_test(CURRENT_TEST_KEY, true);
    return true;
    }

//-------------------------------------------------------
bool BaseProject::AddSimplifiedAriTest(const bool setFocus)
    {
    ClearReadabilityTestResult();
    if (!wxGetApp().GetLicenseAdmin().IsFeatureEnabled(wxGetApp().FeatureEnglishReadabilityTestsCode()))
        { return false; }
    const wxString CURRENT_TEST_KEY = ReadabilityMessages::SIMPLE_ARI();

    if (!GetTotalWords())
        {
        LogMessage(wxString::Format(_(L"Unable to calculate %s: at least one word must be present in document."),
            GetReadabilityTests().get_test_short_name(CURRENT_TEST_KEY).c_str()),
            _(L"Error"), wxOK|wxICON_ERROR, wxEmptyString, true);
        GetReadabilityTests().include_test(CURRENT_TEST_KEY, false);
        return false;
        }
    if (!GetTotalSentences())
        {
        LogMessage(wxString::Format(_(L"Unable to calculate %s: at least one sentence must be present in document."),
            GetReadabilityTests().get_test_short_name(CURRENT_TEST_KEY).c_str()),
            _(L"Error"), wxOK|wxICON_ERROR, wxEmptyString, true);
        GetReadabilityTests().include_test(CURRENT_TEST_KEY, false);
        return false;
        }

    try
        {
        const double gradeValue = readability::simplified_automated_readability_index(GetTotalWords(),
                    GetTotalCharactersPlusPunctuation(),
                    GetTotalSentences() );

        const auto theTest = GetReadabilityTests().find_test(CURRENT_TEST_KEY);
        if (!VerifyTestBeforeAdding(theTest))
            { return false; }

        const wxString description = HasUI() ?
            ProjectReportFormat::FormatTestResult(
                GetReadabilityMessageCatalog().GetGradeScaleDescription(gradeValue),
                theTest.first->get_test()) :
            wxString{};

        wxString displayableGradeLevel = wxNumberFormatter::ToString(gradeValue, 1, wxNumberFormatter::Style::Style_NoTrailingZeroes);
        if (gradeValue == 19)
            { displayableGradeLevel += L"+"; }

        SetReadabilityTestResult(CURRENT_TEST_KEY, theTest.first->get_test().get_long_name().c_str(),
            description,
            std::make_pair(gradeValue, displayableGradeLevel),
            ReadabilityMessages::GetAgeFromUSGrade(gradeValue, GetReadabilityMessageCatalog().GetReadingAgeDisplay()),
            std::numeric_limits<double>::quiet_NaN(),
            std::numeric_limits<double>::quiet_NaN(), setFocus);
        }
    catch (...)
        {
        HandleFailedTestCalculation(CURRENT_TEST_KEY);
        return false;
        }

    GetReadabilityTests().include_test(CURRENT_TEST_KEY, true);
    return true;
    }

//-------------------------------------------------------
bool BaseProject::AddEflawTest(const bool setFocus)
    {
    ClearReadabilityTestResult();
    if (!wxGetApp().GetLicenseAdmin().IsFeatureEnabled(wxGetApp().FeatureEnglishReadabilityTestsCode()))
        { return false; }
    const wxString CURRENT_TEST_KEY = ReadabilityMessages::EFLAW();

    if (!GetTotalWords())
        {
        LogMessage(wxString::Format(_(L"Unable to calculate %s: at least one word must be present in document."),
            GetReadabilityTests().get_test_short_name(CURRENT_TEST_KEY).c_str()),
            _(L"Error"), wxOK|wxICON_ERROR, wxEmptyString, true);
        GetReadabilityTests().include_test(CURRENT_TEST_KEY, false);
        return false;
        }
    if (!GetTotalSentences() )
        {
        LogMessage(wxString::Format(_(L"Unable to calculate %s: at least one sentence must be present in document."),
            GetReadabilityTests().get_test_short_name(CURRENT_TEST_KEY).c_str()),
            _(L"Error"), wxOK|wxICON_ERROR, wxEmptyString, true);
        GetReadabilityTests().include_test(CURRENT_TEST_KEY, false);
        return false;
        }

    readability::eflaw_difficulty diffLevel;

    try
        {
        const size_t val = readability::eflaw(diffLevel, GetTotalWords(),
                        GetTotalMiniWords(),
                        GetTotalSentences());

        const auto theTest = GetReadabilityTests().find_test(CURRENT_TEST_KEY);
        if (!VerifyTestBeforeAdding(theTest))
            { return false; }

        const wxString description = HasUI() ?
            ProjectReportFormat::FormatTestResult(
                ReadabilityMessages::GetEflawDescription(diffLevel),
                theTest.first->get_test(),
                ReadabilityMessages::GetPunctuationIgnoredNote()) :
            wxString{};

        SetReadabilityTestResult(CURRENT_TEST_KEY, theTest.first->get_test().get_long_name().c_str(),
            description,
            std::make_pair(std::numeric_limits<double>::quiet_NaN(), wxEmptyString),
            wxEmptyString,
            val,
            std::numeric_limits<double>::quiet_NaN(), setFocus);
        }
    catch (...)
        {
        HandleFailedTestCalculation(CURRENT_TEST_KEY);
        return false;
        }

    GetReadabilityTests().include_test(CURRENT_TEST_KEY, true);
    return true;
    }

//-------------------------------------------------------
bool BaseProject::AddHarrisJacobsonTest(const bool setFocus)
    {
    ClearReadabilityTestResult();
    if (!wxGetApp().GetLicenseAdmin().IsFeatureEnabled(wxGetApp().FeatureEnglishReadabilityTestsCode()))
        { return false; }
    const wxString CURRENT_TEST_KEY = ReadabilityMessages::HARRIS_JACOBSON();

    if ((GetHarrisJacobsonTextExclusionMode() == SpecializedTestTextExclusion::ExcludeIncompleteSentencesExceptHeadings &&
            !GetTotalWordsFromCompleteSentencesAndHeaders()) ||
        !GetTotalWords())
        {
        LogMessage(wxString::Format(_(L"Unable to calculate %s: at least one word must be present in document."),
            GetReadabilityTests().get_test_short_name(CURRENT_TEST_KEY).c_str()),
            _(L"Error"), wxOK|wxICON_ERROR, wxEmptyString, true);
        GetReadabilityTests().include_test(CURRENT_TEST_KEY, false);
        return false;
        }
    if ((GetHarrisJacobsonTextExclusionMode() == SpecializedTestTextExclusion::ExcludeIncompleteSentencesExceptHeadings &&
            !GetTotalSentencesFromCompleteSentencesAndHeaders() ) ||
        !GetTotalSentences())
        {
        LogMessage(wxString::Format(_(L"Unable to calculate %s: at least one sentence must be present in document."),
            GetReadabilityTests().get_test_short_name(CURRENT_TEST_KEY).c_str()),
            _(L"Error"), wxOK|wxICON_ERROR, wxEmptyString, true);
        GetReadabilityTests().include_test(CURRENT_TEST_KEY, false);
        return false;
        }

    try
        {
        double gradeValue = 1;
        if (GetHarrisJacobsonTextExclusionMode() == SpecializedTestTextExclusion::ExcludeIncompleteSentencesExceptHeadings)
            {
            gradeValue = readability::harris_jacobson(
                GetTotalWordsFromCompleteSentencesAndHeaders()-GetTotalNumeralsFromCompleteSentencesAndHeaders(),
                GetTotalUniqueHarrisJacobsonHardWords(),
                GetTotalSentencesFromCompleteSentencesAndHeaders());
            }
        else
            {
            gradeValue = readability::harris_jacobson(
                GetTotalWords()-GetTotalNumerals(),
                GetTotalUniqueHarrisJacobsonHardWords(),
                GetTotalSentences());
            }

        const auto theTest = GetReadabilityTests().find_test(CURRENT_TEST_KEY);
        if (!VerifyTestBeforeAdding(theTest))
            { return false; }

        const wxString description = HasUI() ?
            ProjectReportFormat::FormatTestResult(
                GetReadabilityMessageCatalog().GetGradeScaleDescription(gradeValue),
                theTest.first->get_test(),
                ReadabilityMessages::GetHarrisJacobsonNote()) :
            wxString{};

        wxString displayableGradeLevel = wxNumberFormatter::ToString(gradeValue, 1, wxNumberFormatter::Style::Style_NoTrailingZeroes);
        if (gradeValue == 11.3)
            { displayableGradeLevel += L"+"; }

        SetReadabilityTestResult(CURRENT_TEST_KEY, theTest.first->get_test().get_long_name().c_str(),
            description,
            std::make_pair(gradeValue, displayableGradeLevel),
            ReadabilityMessages::GetAgeFromUSGrade(gradeValue, GetReadabilityMessageCatalog().GetReadingAgeDisplay()),
            std::numeric_limits<double>::quiet_NaN(),
            std::numeric_limits<double>::quiet_NaN(), setFocus);
        }
    catch (...)
        {
        HandleFailedTestCalculation(CURRENT_TEST_KEY);
        return false;
        }

    GetReadabilityTests().include_test(CURRENT_TEST_KEY, true);
    return true;
    }

//-------------------------------------------------------
bool BaseProject::AddRixTest(const bool setFocus)
    {
    ClearReadabilityTestResult();
    if (!wxGetApp().GetLicenseAdmin().IsFeatureEnabled(wxGetApp().FeatureEnglishReadabilityTestsCode()))
        { return false; }
    const wxString CURRENT_TEST_KEY = ReadabilityMessages::RIX();

    if (!GetTotalWords())
        {
        LogMessage(wxString::Format(_(L"Unable to calculate %s: at least one word must be present in document."),
            GetReadabilityTests().get_test_short_name(CURRENT_TEST_KEY).c_str()),
            _(L"Error"), wxOK|wxICON_ERROR, wxEmptyString, true);
        GetReadabilityTests().include_test(CURRENT_TEST_KEY, false);
        return false;
        }
    if (!GetTotalSentenceUnits() )
        {
        LogMessage(wxString::Format(_(L"Unable to calculate %s: at least one sentence must be present in document."),
            GetReadabilityTests().get_test_short_name(CURRENT_TEST_KEY).c_str()),
            _(L"Error"), wxOK|wxICON_ERROR, wxEmptyString, true);
        GetReadabilityTests().include_test(CURRENT_TEST_KEY, false);
        return false;
        }

    try
        {
        size_t gradeLevel{ 1 };
        const double val = readability::rix(gradeLevel,
                        GetTotalHardLixRixWords(), GetTotalSentenceUnits() );

        const auto theTest = GetReadabilityTests().find_test(CURRENT_TEST_KEY);
        if (!VerifyTestBeforeAdding(theTest))
            { return false; }

        const wxString description = HasUI() ?
            ProjectReportFormat::FormatTestResult(
                GetReadabilityMessageCatalog().GetGradeScaleDescription(gradeLevel),
                theTest.first->get_test(),
                ReadabilityMessages::GetPunctuationIgnoredNote()) :
            wxString{};

        wxString displayableScore = wxNumberFormatter::ToString(gradeLevel, 0, wxNumberFormatter::Style::Style_NoTrailingZeroes);
        if (gradeLevel == 13)
            { displayableScore += L"+"; }

        SetReadabilityTestResult(CURRENT_TEST_KEY, theTest.first->get_test().get_long_name().c_str(),
            description,
            std::make_pair(gradeLevel, displayableScore),
            ReadabilityMessages::GetAgeFromUSGrade(gradeLevel, GetReadabilityMessageCatalog().GetReadingAgeDisplay()),
            val, std::numeric_limits<double>::quiet_NaN(), setFocus);
        }
    catch (...)
        {
        HandleFailedTestCalculation(CURRENT_TEST_KEY);
        return false;
        }

    GetReadabilityTests().include_test(CURRENT_TEST_KEY, true);
    return true;
    }

//-------------------------------------------------------
bool BaseProject::AddRixGermanFiction(const bool setFocus)
    {
    ClearReadabilityTestResult();
    if (!wxGetApp().GetLicenseAdmin().IsFeatureEnabled(wxGetApp().FeatureLanguagePackCode()) &&
        !wxGetApp().GetLicenseAdmin().IsFeatureEnabled(wxGetApp().FeatureProfessionalCode()))
        { return false; }
    const wxString CURRENT_TEST_KEY = ReadabilityMessages::RIX_GERMAN_FICTION();

    if (!GetTotalWords())
        {
        LogMessage(wxString::Format(_(L"Unable to calculate %s: at least one word must be present in document."),
            GetReadabilityTests().get_test_short_name(CURRENT_TEST_KEY).c_str()),
            _(L"Error"), wxOK|wxICON_ERROR, wxEmptyString, true);
        GetReadabilityTests().include_test(CURRENT_TEST_KEY, false);
        return false;
        }
    if (!GetTotalSentenceUnits() )
        {
        LogMessage(wxString::Format(_(L"Unable to calculate %s: at least one sentence must be present in document."),
            GetReadabilityTests().get_test_short_name(CURRENT_TEST_KEY).c_str()),
            _(L"Error"), wxOK|wxICON_ERROR, wxEmptyString, true);
        GetReadabilityTests().include_test(CURRENT_TEST_KEY, false);
        return false;
        }

    try
        {
        const double val = readability::rix_bamberger_vanecek(GetTotalWords(),
                        GetTotalHardLixRixWords(), GetTotalSentenceUnits() );
        const size_t gradeLevel = readability::rix_index_to_german_fiction_grade_level(val);

        const auto theTest = GetReadabilityTests().find_test(CURRENT_TEST_KEY);
        if (!VerifyTestBeforeAdding(theTest))
            { return false; }

        const wxString description = HasUI() ?
            ProjectReportFormat::FormatTestResult(
                GetReadabilityMessageCatalog().GetGradeScaleDescription(gradeLevel),
                theTest.first->get_test(),
                ReadabilityMessages::GetPunctuationIgnoredNote()) :
            wxString{};

        wxString displayableScore = wxNumberFormatter::ToString(gradeLevel, 0, wxNumberFormatter::Style::Style_NoTrailingZeroes);
        if (gradeLevel == 11)
            { displayableScore += L"+"; }

        SetReadabilityTestResult(CURRENT_TEST_KEY, theTest.first->get_test().get_long_name().c_str(),
            description,
            std::make_pair(gradeLevel, displayableScore),
            ReadabilityMessages::GetAgeFromUSGrade(gradeLevel, GetReadabilityMessageCatalog().GetReadingAgeDisplay()),
            val,
            std::numeric_limits<double>::quiet_NaN(), setFocus);
        }
    catch (...)
        {
        HandleFailedTestCalculation(CURRENT_TEST_KEY);
        return false;
        }

    GetReadabilityTests().include_test(CURRENT_TEST_KEY, true);
    return true;
    }

//-------------------------------------------------------
bool BaseProject::AddRixGermanNonFiction(const bool setFocus)
    {
    ClearReadabilityTestResult();
    if (!wxGetApp().GetLicenseAdmin().IsFeatureEnabled(wxGetApp().FeatureLanguagePackCode()) &&
        !wxGetApp().GetLicenseAdmin().IsFeatureEnabled(wxGetApp().FeatureProfessionalCode()))
        { return false; }
    const wxString CURRENT_TEST_KEY = ReadabilityMessages::RIX_GERMAN_NONFICTION();

    if (!GetTotalWords())
        {
        LogMessage(wxString::Format(_(L"Unable to calculate %s: at least one word must be present in document."),
            GetReadabilityTests().get_test_short_name(CURRENT_TEST_KEY).c_str()),
            _(L"Error"), wxOK|wxICON_ERROR, wxEmptyString, true);
        GetReadabilityTests().include_test(CURRENT_TEST_KEY, false);
        return false;
        }
    if (!GetTotalSentenceUnits() )
        {
        LogMessage(wxString::Format(_(L"Unable to calculate %s: at least one sentence must be present in document."),
            GetReadabilityTests().get_test_short_name(CURRENT_TEST_KEY).c_str()),
            _(L"Error"), wxOK|wxICON_ERROR, wxEmptyString, true);
        GetReadabilityTests().include_test(CURRENT_TEST_KEY, false);
        return false;
        }

    try
        {
        const double val = readability::rix_bamberger_vanecek(GetTotalWords(),
                        GetTotalHardLixRixWords(), GetTotalSentenceUnits() );
        const size_t gradeLevel = readability::rix_index_to_german_nonfiction_grade_level(val);

        const auto theTest = GetReadabilityTests().find_test(CURRENT_TEST_KEY);
        if (!VerifyTestBeforeAdding(theTest))
            { return false; }

        const wxString description = HasUI() ?
            ProjectReportFormat::FormatTestResult(
                GetReadabilityMessageCatalog().GetGradeScaleDescription(gradeLevel),
                theTest.first->get_test(),
                ReadabilityMessages::GetPunctuationIgnoredNote()) :
            wxString{};

        wxString displayableScore = wxNumberFormatter::ToString(gradeLevel, 0, wxNumberFormatter::Style::Style_NoTrailingZeroes);
        if (gradeLevel == 14)
            { displayableScore += L"+"; }

        SetReadabilityTestResult(CURRENT_TEST_KEY, theTest.first->get_test().get_long_name().c_str(),
            description,
            std::make_pair(gradeLevel, displayableScore),
            ReadabilityMessages::GetAgeFromUSGrade(gradeLevel, GetReadabilityMessageCatalog().GetReadingAgeDisplay()),
            val,
            std::numeric_limits<double>::quiet_NaN(), setFocus);
        }
    catch (...)
        {
        HandleFailedTestCalculation(CURRENT_TEST_KEY);
        return false;
        }

    GetReadabilityTests().include_test(CURRENT_TEST_KEY, true);
    return true;
    }

//-------------------------------------------------------
bool BaseProject::AddLixGermanChildrensLiterature(const bool setFocus)
    {
    ClearReadabilityTestResult();
    if (!wxGetApp().GetLicenseAdmin().IsFeatureEnabled(wxGetApp().FeatureLanguagePackCode()) &&
        !wxGetApp().GetLicenseAdmin().IsFeatureEnabled(wxGetApp().FeatureProfessionalCode()))
        { return false; }
    const wxString CURRENT_TEST_KEY = ReadabilityMessages::LIX_GERMAN_CHILDRENS_LITERATURE();

    if (!GetTotalWords())
        {
        LogMessage(wxString::Format(_(L"Unable to calculate %s: at least one word must be present in document."),
            GetReadabilityTests().get_test_short_name(CURRENT_TEST_KEY).c_str()),
            _(L"Error"), wxOK|wxICON_ERROR, wxEmptyString, true);
        GetReadabilityTests().include_test(CURRENT_TEST_KEY, false);
        return false;
        }
    if (!GetTotalSentences() )
        {
        LogMessage(wxString::Format(_(L"Unable to calculate %s: at least one sentence must be present in document."),
            GetReadabilityTests().get_test_short_name(CURRENT_TEST_KEY).c_str()),
            _(L"Error"), wxOK|wxICON_ERROR, wxEmptyString, true);
        GetReadabilityTests().include_test(CURRENT_TEST_KEY, false);
        return false;
        }

    try
        {
        readability::german_lix_difficulty diffLevel;
        const size_t val = readability::german_lix(diffLevel, GetTotalWords(),
                        GetTotalHardLixRixWords(), GetTotalSentences() );
        size_t gradeLevel = readability::lix_index_to_german_childrens_literature_grade_level(val);

        const auto theTest = GetReadabilityTests().find_test(CURRENT_TEST_KEY);
        if (!VerifyTestBeforeAdding(theTest))
            { return false; }

        const wxString description = HasUI() ?
            ProjectReportFormat::FormatTestResult(
                ReadabilityMessages::GetLixDescription(diffLevel),
                theTest.first->get_test(),
                ReadabilityMessages::GetPunctuationIgnoredNote()) :
            wxString{};

        wxString displayableScore = wxNumberFormatter::ToString(gradeLevel, 0, wxNumberFormatter::Style::Style_NoTrailingZeroes);
        if (gradeLevel == 8)
            { displayableScore += L"+"; }

        SetReadabilityTestResult(CURRENT_TEST_KEY, theTest.first->get_test().get_long_name().c_str(),
            description,
            std::make_pair(gradeLevel, displayableScore),
            ReadabilityMessages::GetAgeFromUSGrade(gradeLevel, GetReadabilityMessageCatalog().GetReadingAgeDisplay()),
            val,
            std::numeric_limits<double>::quiet_NaN(), setFocus);
        }
    catch (...)
        {
        HandleFailedTestCalculation(CURRENT_TEST_KEY);
        return false;
        }

    AddLixGermanGuage(setFocus);

    GetReadabilityTests().include_test(CURRENT_TEST_KEY, true);
    return true;
    }

//-------------------------------------------------------
bool BaseProject::AddLixGermanTechnical(const bool setFocus)
    {
    ClearReadabilityTestResult();
    if (!wxGetApp().GetLicenseAdmin().IsFeatureEnabled(wxGetApp().FeatureLanguagePackCode()) &&
        !wxGetApp().GetLicenseAdmin().IsFeatureEnabled(wxGetApp().FeatureProfessionalCode()))
        { return false; }
    const wxString CURRENT_TEST_KEY = ReadabilityMessages::LIX_GERMAN_TECHNICAL();

    if (!GetTotalWords())
        {
        LogMessage(wxString::Format(_(L"Unable to calculate %s: at least one word must be present in document."),
            GetReadabilityTests().get_test_short_name(CURRENT_TEST_KEY).c_str()),
            _(L"Error"), wxOK|wxICON_ERROR, wxEmptyString, true);
        GetReadabilityTests().include_test(CURRENT_TEST_KEY, false);
        return false;
        }
    if (!GetTotalSentences() )
        {
        LogMessage(wxString::Format(_(L"Unable to calculate %s: at least one sentence must be present in document."),
            GetReadabilityTests().get_test_short_name(CURRENT_TEST_KEY).c_str()),
            _(L"Error"), wxOK|wxICON_ERROR, wxEmptyString, true);
        GetReadabilityTests().include_test(CURRENT_TEST_KEY, false);
        return false;
        }

    try
        {
        readability::german_lix_difficulty diffLevel;
        const size_t val = readability::german_lix(diffLevel, GetTotalWords(),
                        GetTotalHardLixRixWords(), GetTotalSentences() );
        size_t gradeLevel = readability::lix_index_to_german_technical_literature_grade_level(val);

        const auto theTest = GetReadabilityTests().find_test(CURRENT_TEST_KEY);
        if (!VerifyTestBeforeAdding(theTest))
            { return false; }

        const wxString description = HasUI() ?
            ProjectReportFormat::FormatTestResult(
                ReadabilityMessages::GetLixDescription(diffLevel),
                theTest.first->get_test(),
                ReadabilityMessages::GetPunctuationIgnoredNote()) :
            wxString{};

        wxString displayableScore = wxNumberFormatter::ToString(gradeLevel, 0, wxNumberFormatter::Style::Style_NoTrailingZeroes);
        if (gradeLevel == 15)
            { displayableScore += L"+"; }

        SetReadabilityTestResult(CURRENT_TEST_KEY, theTest.first->get_test().get_long_name().c_str(),
            description,
            std::make_pair(gradeLevel, displayableScore),
            ReadabilityMessages::GetAgeFromUSGrade(gradeLevel, GetReadabilityMessageCatalog().GetReadingAgeDisplay()),
            val,
            std::numeric_limits<double>::quiet_NaN(), setFocus);
        }
    catch (...)
        {
        HandleFailedTestCalculation(CURRENT_TEST_KEY);
        return false;
        }

    AddLixGermanGuage(setFocus);

    GetReadabilityTests().include_test(CURRENT_TEST_KEY, true);
    return true;
    }

//-------------------------------------------------------
bool BaseProject::AddLixTest(const bool setFocus)
    {
    ClearReadabilityTestResult();
    if (!wxGetApp().GetLicenseAdmin().IsFeatureEnabled(wxGetApp().FeatureEnglishReadabilityTestsCode()))
        { return false; }
    const wxString CURRENT_TEST_KEY = ReadabilityMessages::LIX();

    if (!GetTotalWords())
        {
        LogMessage(wxString::Format(_(L"Unable to calculate %s: at least one word must be present in document."),
            GetReadabilityTests().get_test_short_name(CURRENT_TEST_KEY).c_str()),
            _(L"Error"), wxOK|wxICON_ERROR, wxEmptyString, true);
        GetReadabilityTests().include_test(CURRENT_TEST_KEY, false);
        return false;
        }
    if (!GetTotalSentences() )
        {
        LogMessage(wxString::Format(_(L"Unable to calculate %s: at least one sentence must be present in document."),
            GetReadabilityTests().get_test_short_name(CURRENT_TEST_KEY).c_str()),
            _(L"Error"), wxOK|wxICON_ERROR, wxEmptyString, true);
        GetReadabilityTests().include_test(CURRENT_TEST_KEY, false);
        return false;
        }

    try
        {
        readability::lix_difficulty diffLevel;
        size_t gradeLevel{ 1 };
        const size_t val = readability::lix(diffLevel, gradeLevel, GetTotalWords(),
                        GetTotalHardLixRixWords(), GetTotalSentences() );

        const auto theTest = GetReadabilityTests().find_test(CURRENT_TEST_KEY);
        if (!VerifyTestBeforeAdding(theTest))
            { return false; }

        const wxString description = HasUI() ?
            ProjectReportFormat::FormatTestResult(
                ReadabilityMessages::GetLixDescription(diffLevel),
                theTest.first->get_test(),
                ReadabilityMessages::GetPunctuationIgnoredNote()) :
            wxString{};

        wxString displayableScore = wxNumberFormatter::ToString(gradeLevel, 0, wxNumberFormatter::Style::Style_NoTrailingZeroes);
        if (gradeLevel == 13)
            { displayableScore += L"+"; }

        SetReadabilityTestResult(CURRENT_TEST_KEY, theTest.first->get_test().get_long_name().c_str(),
            description,
            std::make_pair(gradeLevel, displayableScore),
            ReadabilityMessages::GetAgeFromUSGrade(gradeLevel, GetReadabilityMessageCatalog().GetReadingAgeDisplay()),
            val,
            std::numeric_limits<double>::quiet_NaN(), setFocus);
        }
    catch (...)
        {
        HandleFailedTestCalculation(CURRENT_TEST_KEY);
        return false;
        }

    AddLixGauge(setFocus);

    GetReadabilityTests().include_test(CURRENT_TEST_KEY, true);
    return true;
    }

//-------------------------------------------------------
void BaseProject::SyncCustomTests()
    {
    for(std::vector<CustomReadabilityTestInterface>::iterator pos = m_customTestsInUse.begin();
        pos != m_customTestsInUse.end();
        /*in loop*/)
        {
        CustomReadabilityTestCollection::iterator testIter = 
            std::find(m_custom_word_tests.begin(), m_custom_word_tests.end(), pos->GetTestName());
        //shouldn't happen, but remove test from project if not found in the global system
        if (testIter == m_custom_word_tests.end())
            {
            pos = m_customTestsInUse.erase(pos);
            continue;
            }
        pos->SetIterator(testIter);
        pos->SetIsDaleChallFormula(pos->GetIterator()->get_formula().find(ReadabilityFormulaParser::GetCustomNewDaleChallSignature()) != -1);
        pos->SetIsHarrisJacobsonFormula(pos->GetIterator()->get_formula().find(ReadabilityFormulaParser::GetCustomHarrisJacobsonSignature()) != -1);
        ++pos;
        }
    }

//-------------------------------------------------------
bool BaseProject::AddCustomReadabilityTest(const wxString& name, const bool calculate /*= false*/)
    {
    ClearReadabilityTestResult();

    //first, see if this is a legit custom test that is loaded in the global system
    const CustomReadabilityTestCollection::const_iterator testIter =
            std::find(m_custom_word_tests.cbegin(), m_custom_word_tests.cend(), name);
    if (testIter == m_custom_word_tests.cend())
        { return false; }

    std::vector<CustomReadabilityTestInterface>::iterator pos = std::find(m_customTestsInUse.begin(), m_customTestsInUse.end(), name);
    //see if test needs to be included
    if (pos == m_customTestsInUse.end())
        {
        m_customTestsInUse.push_back(CustomReadabilityTestInterface(name));
        SyncCustomTests(); //reset the internal iterators that point to the global tests
        pos = std::find(m_customTestsInUse.begin(), m_customTestsInUse.end(), name);
        if (pos == m_customTestsInUse.end())
            { return false; }
        }

    if (calculate)
        {
        if (!GetTotalWords())
            {
            LogMessage(_(L"Unable to calculate custom readability test: at least one word must be present in document."),
                _(L"Error"), wxOK|wxICON_ERROR, wxEmptyString, true);
            return false;
            }
        try
            {
            //put together a description detailing the formula and stemming type used by this test
            wxString customDescription = wxString::Format(L"<p>%s</p>\r\n<p>&nbsp;&nbsp;&nbsp;&nbsp;%s</p>",
                _(L"This is a custom test using the following formula:"),
                ProjectReportFormat::FormatFormulaToHtml(pos->GetIterator()->get_formula().c_str()) );
            if (pos->GetIterator()->is_using_familiar_words())
                {
                customDescription += wxString::Format(L"<p>%s</p>\r\n<ul>\r\n", 
                    _(L"This test uses the following criteria to determine word familiarity:") );
                if (pos->GetIterator()->is_including_dale_chall_list())
                    { customDescription += wxString(L"<li>") + wxString::Format(_(L"%s familiar word list"), _DT(L"New Dale Chall")) + wxString(L"</li>"); }
                if (pos->GetIterator()->is_including_spache_list())
                    { customDescription += wxString(L"<li>") + wxString::Format(_(L"%s familiar word list"), _DT(L"Spache Revised")) + wxString(L"</li>"); }
                if (pos->GetIterator()->is_including_harris_jacobson_list())
                    { customDescription += wxString(L"<li>") + wxString::Format(_(L"%s familiar word list"), _DT(L"Harris-Jacobson")) + wxString(L"</li>"); }
                if (pos->GetIterator()->is_including_stocker_list())
                    { customDescription += wxString(L"<li>") + _(L"Stocker's Catholic supplementary word list") + wxString(L"</li>"); }
                if (pos->GetIterator()->is_including_custom_familiar_word_list())
                    {
                    customDescription += wxString(L"<li>");
                    if (pos->GetIterator()->get_stemming_type() == stemming::stemming_type::no_stemming)
                        {
                        customDescription += _(L"A custom word list:") +
                            wxString::Format(L"<br />&nbsp;&nbsp;&nbsp;&nbsp;&ldquo;<span style=\"font-style:italic;\">%s</span>&rdquo;",
                                pos->GetIterator()->get_familiar_word_list_file_path().c_str());
                        }
                    else
                        {
                        customDescription += wxString::Format(_(L"A custom word list (that is using %s stemming):"),
                            ProjectReportFormat::GetStemmingDisplayName(pos->GetIterator()->get_stemming_type())) +
                            wxString::Format(
                                L"<br />&nbsp;&nbsp;&nbsp;&nbsp;&ldquo;<span style=\"font-style:italic;\">%s</span>&rdquo;",
                                pos->GetIterator()->get_familiar_word_list_file_path().c_str());
                        }
                    customDescription += wxString(L"</li>");
                    }
                if (pos->GetIterator()->get_proper_noun_method() == readability::proper_noun_counting_method::all_proper_nouns_are_familiar)
                    { customDescription += wxString(L"<li>") + _(L"Proper nouns") + wxString(L"</li>"); }
                else if (pos->GetIterator()->get_proper_noun_method() == readability::proper_noun_counting_method::only_count_first_instance_of_proper_noun_as_unfamiliar)
                    { customDescription += wxString(L"<li>") + _(L"Proper nouns (except first occurrence)") + wxString(L"</li>"); }
                if (pos->GetIterator()->is_including_numeric_as_familiar())
                    { customDescription += wxString(L"<li>") + _(L"Numerals") + wxString(L"</li>"); }
                customDescription += wxString(L"\r\n</ul>");
                }
            customDescription = L"<tr><td>" + customDescription + L"</td></tr>";
            SetCurrentCustomTest(pos->GetIterator()->get_name().c_str());

            try
                {
                GetFormulaParser().UpdateVariables();
                [[maybe_unused]] auto notUsedRes = GetFormulaParser().evaluate(wxString(pos->GetIterator()->get_formula().c_str()));
                if (!GetFormulaParser().success())
                    {
                    SetReadabilityTestResult(wxString(pos->GetIterator()->get_name().c_str()),
                        wxString(pos->GetIterator()->get_name().c_str()),
                        customDescription,
                        std::make_pair(std::numeric_limits<double>::quiet_NaN(),
                            wxString::Format(_(L"Syntax error in formula at position %s."), std::to_wstring(GetFormulaParser().get_last_error_position()))),
                        wxEmptyString, std::numeric_limits<double>::quiet_NaN(),
                        std::numeric_limits<double>::quiet_NaN(), false);
                    }
                else if (pos->GetIterator()->get_test_type() == readability::readability_test_type::grade_level)
                    {
                    // custom DC test may will be using range values instead of a score
                    if (pos->IsDaleChallFormula())
                        {
                        uint32_t gradeBegin, gradeEnd;
                        split_int64(static_cast<uint64_t>(GetFormulaParser().get_result()), gradeBegin, gradeEnd);
                        if (gradeBegin == gradeEnd)
                            {
                            SetReadabilityTestResult(wxString(pos->GetIterator()->get_name().c_str()),
                                wxString(pos->GetIterator()->get_name().c_str()),
                                L"<tr><td>" + GetReadabilityMessageCatalog().GetGradeScaleDescription(static_cast<size_t>(gradeBegin)) + L"</td></tr>" +
                                    customDescription,
                                std::make_pair(gradeBegin, wxNumberFormatter::ToString(gradeBegin, 0, wxNumberFormatter::Style::Style_NoTrailingZeroes)),
                                ReadabilityMessages::GetAgeFromUSGrade(gradeBegin, gradeEnd, GetReadabilityMessageCatalog().GetReadingAgeDisplay()),
                                std::numeric_limits<double>::quiet_NaN(),
                                std::numeric_limits<double>::quiet_NaN(), false);
                            }
                        else
                            {
                            SetReadabilityTestResult(wxString(pos->GetIterator()->get_name().c_str()),
                                wxString(pos->GetIterator()->get_name().c_str()),
                                L"<tr><td>" + GetReadabilityMessageCatalog().GetGradeScaleDescription(gradeBegin, gradeEnd) + L"</td></tr>\n" +
                                    customDescription,
                                std::make_pair(safe_divide<double>(gradeBegin + gradeEnd, 2), wxNumberFormatter::ToString(gradeBegin, 0, wxNumberFormatter::Style::Style_NoTrailingZeroes) + wxString(L"-") + wxNumberFormatter::ToString(gradeEnd, 0, wxNumberFormatter::Style::Style_NoTrailingZeroes)),
                                ReadabilityMessages::GetAgeFromUSGrade(gradeBegin, gradeEnd, GetReadabilityMessageCatalog().GetReadingAgeDisplay()),
                                std::numeric_limits<double>::quiet_NaN(),
                                std::numeric_limits<double>::quiet_NaN(), false);
                            }
                        }
                    else
                        {
                        const double score = readability::truncate_k12_plus_grade(GetFormulaParser().get_result());
                        SetReadabilityTestResult(wxString(pos->GetIterator()->get_name().c_str()),
                            wxString(pos->GetIterator()->get_name().c_str()),
                            L"<tr><td>" + GetReadabilityMessageCatalog().GetGradeScaleDescription(score) + L"</td></tr>\n" +
                                customDescription,
                            std::make_pair(score, wxNumberFormatter::ToString(score, 1, wxNumberFormatter::Style::Style_NoTrailingZeroes)),
                            ReadabilityMessages::GetAgeFromUSGrade(score, GetReadabilityMessageCatalog().GetReadingAgeDisplay()),
                            std::numeric_limits<double>::quiet_NaN(),
                            std::numeric_limits<double>::quiet_NaN(), false);
                        }
                    }
                else if (pos->GetIterator()->get_test_type() == readability::readability_test_type::index_value)
                    {
                    const double score = GetFormulaParser().get_result();
                    SetReadabilityTestResult(wxString(pos->GetIterator()->get_name().c_str()),
                        wxString(pos->GetIterator()->get_name().c_str()),
                        L"<tr><td>" + _(L"Score: ") + wxNumberFormatter::ToString(score, 1, wxNumberFormatter::Style::Style_NoTrailingZeroes) + L"</td></tr>\n" +
                        customDescription,
                        std::make_pair(std::numeric_limits<double>::quiet_NaN(), wxEmptyString),
                        wxEmptyString, score,
                        std::numeric_limits<double>::quiet_NaN(), false);
                    }
                else if (pos->GetIterator()->get_test_type() == readability::readability_test_type::predicted_cloze_score)
                    {
                    const double score = GetFormulaParser().get_result();
                    SetReadabilityTestResult(wxString(pos->GetIterator()->get_name().c_str()),
                        wxString(pos->GetIterator()->get_name().c_str()),
                        L"<tr><td>" + _(L"Predicted cloze score: ") + wxNumberFormatter::ToString(score, 1, wxNumberFormatter::Style::Style_NoTrailingZeroes) + L"</td></tr>\n" +
                        customDescription,
                        std::make_pair(std::numeric_limits<double>::quiet_NaN(), wxEmptyString),
                        wxEmptyString, std::numeric_limits<double>::quiet_NaN(), score, false);
                    }
                }
            catch (const std::exception& ex)
                {
                SetReadabilityTestResult(wxString(pos->GetIterator()->get_name().c_str()),
                    wxString(pos->GetIterator()->get_name().c_str()),
                    customDescription,
                    std::make_pair(std::numeric_limits<double>::quiet_NaN(), ex.what()),
                    wxEmptyString, std::numeric_limits<double>::quiet_NaN(),
                    std::numeric_limits<double>::quiet_NaN(), false);
                }
            catch (...)
                {
                SetReadabilityTestResult(wxString(pos->GetIterator()->get_name().c_str()),
                        wxString(pos->GetIterator()->get_name().c_str()),
                        wxEmptyString,
                        std::make_pair(std::numeric_limits<double>::quiet_NaN(), wxEmptyString),
                        wxEmptyString, std::numeric_limits<double>::quiet_NaN(),
                        std::numeric_limits<double>::quiet_NaN(), false);
                }
            }
        catch (...)
            {
            LogMessage(wxString::Format(_(L"Unable to calculate %s."), pos->GetIterator()->get_name().c_str()),
                _(L"Error"), wxOK|wxICON_ERROR, wxEmptyString, true);
            return false;
            }
        }
    return true;
    }

//-------------------------------------------------------
std::vector<CustomReadabilityTestInterface>::iterator BaseProject::RemoveCustomReadabilityTest(const wxString& testName, [[maybe_unused]] const int Id)
    {
    std::vector<CustomReadabilityTestInterface>::iterator testPos =
        std::find(m_customTestsInUse.begin(), m_customTestsInUse.end(), testName);
    if (testPos == m_customTestsInUse.end())
        { return testPos; }
    return m_customTestsInUse.erase(testPos);
    }

//-------------------------------------------------------
bool BaseProject::RemoveTest(const wxString& name)
    {
    if (name == ReadabilityMessages::GetDolchLabel())
        {
        m_includeDolchSightWords = false;
        return true;
        }
    else if (GetReadabilityTests().include_test(name, false))
        {
        // remove from the test's goals too
        const auto [sTest, found] = GetReadabilityTests().find_test(name.wc_str());
        if (found)
            {
            auto foundTest = GetTestGoals().find(TestGoal(sTest->get_test().get_id()));
            if (foundTest != GetTestGoals().end())
                { GetTestGoals().erase(foundTest); }
            }
        return true;
        }
    else if (HasCustomTest(name))
        {
        RemoveCustomReadabilityTest(name, GetCustomTest(name)->GetIterator()->get_interface_id());
        // remove from the test's goals too
        auto foundTest = GetTestGoals().find(TestGoal(name.wc_str()));
        if (foundTest != GetTestGoals().end())
            { GetTestGoals().erase(foundTest); }
        return true;
        }
    else
        { return false; }
    }

bool BaseProject::FindMissingFile(const wxString& filePath, wxString& fileBySameNameInProjectDirectory)
    {
    //if file not found, then try to search for it in the subdirectories from where the project is
    fileBySameNameInProjectDirectory = FindFileInMatchingDirStructure(GetProjectDirectory(), filePath);
    if (wxFile::Exists(fileBySameNameInProjectDirectory) )
        {
        auto warningIter = wxGetApp().GetAppOptions().GetWarning(_DT(L"file-autosearch-from-project-directory"));
        // if they want to be prompted for this...
        if (warningIter != wxGetApp().GetAppOptions().GetWarnings().end() &&
            warningIter->ShouldBeShown())
            {
            wxRichMessageDialog msg(wxGetApp().GetMainFrame(), wxString::Format(_(L"%s:\n\nFile could not be located. However, a file by the same name was found at:\n\n%s\n\nDo you wish to use this file instead?"), filePath, fileBySameNameInProjectDirectory),
                warningIter->GetTitle(), warningIter->GetFlags());
            msg.ShowCheckBox(_(L"Remember my answer"));
            const int dlgResponse = msg.ShowModal();
            // save the checkbox status
            if (msg.IsCheckBoxChecked())
                {
                warningIter->Show(false);
                warningIter->SetPreviousResponse(dlgResponse);
                }
            // now see if they said "Yes" or "No"
            if (dlgResponse == wxID_YES)
                {
                SetModifiedFlag();
                return true;
                }
            else
                {
                fileBySameNameInProjectDirectory.Clear();
                return false;
                }
            }
        // or if they said "yes" before, then use the found path
        else if (warningIter != wxGetApp().GetAppOptions().GetWarnings().end() &&
            warningIter->GetPreviousResponse() == wxID_YES)
            {
            SetModifiedFlag();
            return true;
            }
        // if they said "no" before, then don't bother using the found path
        else
            {
            fileBySameNameInProjectDirectory.Clear();
            return false;
            }
        }
    //alternate file path not found
    else
        {
        fileBySameNameInProjectDirectory.Clear();
        return false;
        }
    }

//------------------------------------------------------
void BaseProject::CopySettings(const BaseProject& that)
    {
    m_hasUI = that.m_hasUI;

    m_language = that.m_language;
    m_reviewer = that.GetReviewer();
    m_status = that.GetStatus();
    m_appendedDocumentFilePath = that.GetAppendedDocumentFilePath();

    SetProjectDirectory(that.GetProjectDirectory());//this is only needed if we need to go searching for a missing document file

    //phrases to be excluded
    m_excludedPhrasesPath = that.GetExcludedPhrasesPath();

    m_exclusionBlockTags = that.GetExclusionBlockTags();

    //indexing and project options
    m_difficultSentenceLength = that.GetDifficultSentenceLength();
    m_longSentenceMethod = that.GetLongSentenceMethod();
    m_numeralSyllabicationMethod = that.GetNumeralSyllabicationMethod();
    m_ignoreBlankLinesForParagraphsParser = that.GetIgnoreBlankLinesForParagraphsParser();
    m_ignoreIndentingForParagraphsParser = that.GetIgnoreIndentingForParagraphsParser();
    m_sentenceStartMustBeUppercased = that.GetSentenceStartMustBeUppercased();
    m_aggressiveExclusion = that.IsExcludingAggressively();
    m_ignoreTrailingCopyrightNoticeParagraphs = that.IsIgnoringTrailingCopyrightNoticeParagraphs();
    m_ignoreTrailingCitations = that.IsIgnoringTrailingCitations();
    m_ignoreFileAddresses = that.IsIgnoringFileAddresses();
    m_ignoreNumerals = that.IsIgnoringNumerals();
    m_ignoreProperNouns = that.IsIgnoringProperNouns();
    m_includeExcludedPhraseFirstOccurrence = that.IsIncludingExcludedPhraseFirstOccurrence();
    m_paragraphsParsingMethod = that.GetParagraphsParsingMethod();
    m_invalidSentenceMethod = that.GetInvalidSentenceMethod();
    m_includeIncompleteSentencesIfLongerThan = that.GetIncludeIncompleteSentencesIfLongerThanValue();
    m_textSource = that.GetTextSource();
    m_documentStorageMethod = that.GetDocumentStorageMethod();
    if (that.GetDocumentStorageMethod() == TextStorage::EmbedText)
        { SetDocumentText(that.GetDocumentText()); }
    m_varianceMethod = that.GetVarianceMethod();
    m_minDocWordCountForBatch = that.m_minDocWordCountForBatch;

    //grammar
    m_spellcheck_ignore_proper_nouns = that.m_spellcheck_ignore_proper_nouns;
    m_spellcheck_ignore_uppercased = that.m_spellcheck_ignore_uppercased;
    m_spellcheck_ignore_numerals = that.m_spellcheck_ignore_numerals;
    m_spellcheck_ignore_file_addresses = that.m_spellcheck_ignore_file_addresses;
    m_spellcheck_ignore_programmer_code = that.m_spellcheck_ignore_programmer_code;
    m_allow_colloquialisms = that.m_allow_colloquialisms;
    m_spellcheck_ignore_social_media_tags = that.m_spellcheck_ignore_social_media_tags;

    //tests inclusion
    m_readabilityTests.clear();
    m_readabilityTests = that.m_readabilityTests;

    m_testGoals = that.m_testGoals;
    m_statGoals = that.m_statGoals;

    m_includeDolchSightWords = that.m_includeDolchSightWords;

    m_readMessages = that.m_readMessages;

    m_includeScoreSummaryReport = that.IsIncludingScoreSummaryReport();

    //test-specific options
    m_hjTextExclusion = that.m_hjTextExclusion;
    m_dcTextExclusion = that.m_dcTextExclusion;
    m_dcProperNounCountingMethod = that.m_dcProperNounCountingMethod;
    m_includeStockerCatholicDCSupplement = that.m_includeStockerCatholicDCSupplement;
    m_fogUseSentenceUnits = that.m_fogUseSentenceUnits;
    m_fleschNumeralSyllabizeMethod = that.m_fleschNumeralSyllabizeMethod;
    m_fleschKincaidNumeralSyllabizeMethod = that.m_fleschKincaidNumeralSyllabizeMethod;

    m_statsReportInfo = that.GetStatisticsReportInfo();
    m_statsInfo = that.GetStatisticsInfo();
    m_grammarInfo = that.GetGrammarInfo();
    m_wordsBreakdownInfo = that.GetWordsBreakdownInfo();
    m_sentencesBreakdownInfo = that.GetSentencesBreakdownInfo();

    //remove any custom tests that this project has that the other one doesn't.
    //We don't want to arbitrarily clear this out, because we want to retain the statistics for any custom tests that will remain in here.
    for (std::vector<CustomReadabilityTestInterface>::const_iterator pos = GetCustTestsInUse().begin();
        pos != GetCustTestsInUse().end();
        /*handled in the loop*/)
        {
        if (!that.HasCustomTest(pos->GetTestName()))
            { pos = RemoveCustomReadabilityTest(pos->GetTestName(), pos->GetIterator()->get_interface_id()); }
        else
            { ++pos; }
        }
    /*add any custom tests that other project has.  If this project already has the test then leave it alone
    and have its statistics kept intact.*/
    for (std::vector<CustomReadabilityTestInterface>::const_iterator pos = that.GetCustTestsInUse().begin();
        pos != that.GetCustTestsInUse().end();
        ++pos)
        {
        std::vector<CustomReadabilityTestInterface>::const_iterator customTestPos = std::find(GetCustTestsInUse().begin(), GetCustTestsInUse().end(), pos->GetTestName());
        //see if test needs to be added
        if (customTestPos == GetCustTestsInUse().end())
            { m_customTestsInUse.push_back(CustomReadabilityTestInterface(pos->GetTestName())); }
        }
    SyncCustomTests();//reset the internal iterators that point to the global tests
    }

//------------------------------------------------
bool BaseProject::VerifyTestBeforeAdding(const std::pair<std::vector<ProjectTestType>::const_iterator, bool>& theTest)
    {
    // see if the test was found in the list of known tests (this shouldn't be an issue)
    wxASSERT_LEVEL_2(theTest.second);
    if (!theTest.second)
        { throw std::exception(); }
    // see if test relates to the language for the project
    if (!theTest.first->get_test().has_language(GetProjectLanguage()) )
        {
        LogMessage(wxString::Format(_(L"\"%s\" is not compatible with this project's language. This test will be removed."),
            theTest.first->get_test().get_short_name().c_str()),
            _(L"Warning"), wxOK|wxICON_EXCLAMATION);
        GetReadabilityTests().include_test(theTest.first->get_test().get_id().c_str(), false);
        return false;
        }
    return true;
    }

//------------------------------------------------
void BaseProject::HandleFailedTestCalculation(const wxString& testName)
    {
    LogMessage(wxString::Format(_(L"Unable to calculate %s."),
        GetReadabilityTests().get_test_short_name(testName).c_str()),
        _(L"Error"), wxOK|wxICON_ERROR, wxEmptyString, true);
    GetReadabilityTests().include_test(testName, false);
    }

//------------------------------------------------
bool BaseProject::ReviewTestGoal(const wxString& testName, const double score)
    {
    wxASSERT_LEVEL_2_MSG(!std::isnan(score) ||
        // doesn't have an actual score
        testName == ReadabilityMessages::GetDolchLabel() ||
        // these tests can fail
        testName == ReadabilityMessages::FRASE() ||
        testName == ReadabilityMessages::FRY() ||
        testName == ReadabilityMessages::GPM_FRY() ||
        testName == ReadabilityMessages::RAYGOR() ||
        testName == ReadabilityMessages::SCHWARTZ(), L"Score should not be NaN!");
    if (GetTestGoals().size() == 0)
        { return false; }

    auto foundTest = GetTestGoals().find(TestGoal(testName.wc_str()));
    if (foundTest != GetTestGoals().end())
        {
        auto node = GetTestGoals().extract(foundTest);
        node.value().GetPassFailFlags().set(); // set to passing
        // set to failure if there is a goal and score doesn't meet it (or is NaN, like when Fry fails to plot)
        if (!std::isnan(node.value().GetMinGoal()) &&
            (std::isnan(score) || score < node.value().GetMinGoal()) )
            { node.value().GetPassFailFlags().set(0, false); }
        if (!std::isnan(node.value().GetMaxGoal()) &&
            (std::isnan(score) || score > node.value().GetMaxGoal()) )
            { node.value().GetPassFailFlags().set(1, false); }
        GetTestGoals().insert(std::move(node));
        return true;
        }
    return false;
    }

//------------------------------------------------
void BaseProject::ReviewStatGoals()
    {
    for (const auto& statGoal : GetStatGoals())
        {
        const auto statPos = GetStatGoalLabels().find(comparable_first_pair(statGoal.GetName(), statGoal.GetName()));
        if (statPos != GetStatGoalLabels().cend())
            { ReviewStatGoal(statGoal.GetName().c_str(), statPos->second(this)); }
        }
    }

//------------------------------------------------
bool BaseProject::ReviewStatGoal(const wxString& statName, const double value)
    {
    wxASSERT_LEVEL_2_MSG(!std::isnan(value), L"Stat value should not be NaN!");
    if (GetStatGoals().size() == 0)
        { return false; }

    auto foundStat = GetStatGoals().find(StatGoal(statName.wc_str()));
    if (foundStat != GetStatGoals().end())
        {
        auto node = GetStatGoals().extract(foundStat);
        node.value().GetPassFailFlags().set(); // set to passing
        // set to failure if there is a goal and value doesn't meet it
        if (!std::isnan(node.value().GetMinGoal()) &&
            (std::isnan(value) || value < node.value().GetMinGoal()) )
            { node.value().GetPassFailFlags().set(0, false); }
        if (!std::isnan(node.value().GetMaxGoal()) &&
            (std::isnan(value) || value > node.value().GetMaxGoal()) )
            { node.value().GetPassFailFlags().set(1, false); }
        GetStatGoals().insert(std::move(node));
        return true;
        }
    else
        { wxLogError(L"%s: unable to find statistic for goal review.", statName); }
    return false;
    }
