#ifndef __BATCH_PROJECT_DOC_H__
#define __BATCH_PROJECT_DOC_H__

#include <wx/wx.h>
#include <wx/docview.h>
#include <vector>
#include "../Wisteria-Dataviz/src/graphs/boxplot.h"
#include "../Wisteria-Dataviz/src/graphs/histogram.h"
#include "../Wisteria-Dataviz/src/data/dataset.h"
#include "base_project_view.h"
#include "base_project_doc.h"

class BatchProjectDoc final : public BaseProjectDoc
    {
public:
    BatchProjectDoc()
        {
        // batches don't use manually entered text, so just set this to reflect that
        SetTextSource(TextSource::FromFile);
        }
    BatchProjectDoc(const BatchProjectDoc&) = delete;
    BatchProjectDoc& operator=(const BatchProjectDoc&) = delete;
    ~BatchProjectDoc()
        {
        wxDELETE(m_scoreRawData);
        wxDELETE(m_goalsData);
        wxDELETE(m_scoreStatsData);
        wxDELETE(m_aggregatedGradeScoresData);
        wxDELETE(m_aggregatedClozeScoresData);
        wxDELETE(m_warnings);
        wxDELETE(m_dupWordData);
        wxDELETE(m_incorrectArticleData);
        wxDELETE(m_overusedWordBySentenceData);
        wxDELETE(m_passiveVoiceData);
        wxDELETE(m_misspelledWordData);
        wxDELETE(m_wordyPhraseData);
        wxDELETE(m_clichePhraseData);
        wxDELETE(m_allWordsBatchData);
        wxDELETE(m_importantWordsBatchData);
        wxDELETE(m_redundantPhraseData);
        wxDELETE(m_wordingErrorData);
        wxDELETE(m_hardWordsData);
        wxDELETE(m_overlyLongSentenceData);
        wxDELETE(m_sentenceStartingWithConjunctionsData);
        wxDELETE(m_sentenceStartingWithLowercaseData);
        wxDELETE(m_dolchCompletionData);
        wxDELETE(m_dolchWordsBatchData);
        wxDELETE(m_NonDolchWordsData);
        for (std::vector<BaseProject*>::iterator pos = m_docs.begin();
            pos != m_docs.end();
            ++pos)
            { wxDELETE(*pos); }
        DeleteExcludedPhrases();
        }
    bool OnNewDocument() final;
    bool OnCreate(const wxString& path, long flags) final;
    bool OnSaveDocument(const wxString& filename) final;
    bool OnOpenDocument(const wxString& filename) final;

    void RefreshProject() final;
    /// Only refresh the graphs, this assumes that no windows are being added or removed from the project.
    void RefreshGraphs() final;

    /// @returns A const reference to the batch's documents.
    [[nodiscard]] const std::vector<BaseProject*>& GetDocuments() const noexcept
        { return m_docs; }
    /// @returns A reference to the batch's documents.
    [[nodiscard]] std::vector<BaseProject*>& GetDocuments() noexcept
        { return m_docs; }

    /// @returns The unique list of labels connected to the documents.
    [[nodiscard]] const std::map<traits::case_insensitive_wstring_ex, Wisteria::Data::GroupIdType>& GetDocumentLabels() const noexcept
        { return m_docLabels; }
    /// @returns The maximum number of groups that the data can be broken into for graphs.
    [[nodiscard]] size_t GetMaxGroupCount() const noexcept
        { return m_maxGroupCount; }
    /// Sets the maximum number of groups allows to bin the documents into (based on their description/group label).
    void SetMaxGroupCount(const size_t maxCount) noexcept
        { m_maxGroupCount = maxCount; }
    /** @returns The document from the batch by name. If document name isn't found in the batch, then null is returned.
        @param docName The full name (including filepath) of the document.*/
    [[nodiscard]] const BaseProject* GetDocument(const wxString& docName) const
        {
        for (auto doc : GetDocuments())
            {
            if (doc && CompareFilePaths(doc->GetOriginalDocumentFilePath(), docName) == 0)
                { return doc; }
            }
        return nullptr;
        }
    void RemoveDocument(const wxString& docName);
    void RemoveMisspellings(const wxArrayString& misspellingsToRemove) final;
    [[nodiscard]] const ListCtrlExNumericDataProvider* GetMisspelledWordData() const noexcept
        { return m_misspelledWordData; }
    [[nodiscard]] const ListCtrlExNumericDataProvider* GetRepeatedWordData() const noexcept
        { return m_dupWordData; }
    [[nodiscard]] const ListCtrlExNumericDataProvider* GetIncorrectArticleData() const noexcept
        { return m_incorrectArticleData; }
    [[nodiscard]] const ListCtrlExNumericDataProvider* GetOverusedWordBySentenceData() const noexcept
        { return m_overusedWordBySentenceData; }
    [[nodiscard]] const ListCtrlExNumericDataProvider* GetPassiveVoiceData() const noexcept
        { return m_passiveVoiceData; }
    [[nodiscard]] const ListCtrlExNumericDataProvider* GetWordyItemsData() const noexcept
        { return m_wordyPhraseData; }
    [[nodiscard]] const ListCtrlExNumericDataProvider* GetRedundantPhrasesData() const noexcept
        { return m_redundantPhraseData; }
    [[nodiscard]] const ListCtrlExNumericDataProvider* GetWordingErrorsData() const noexcept
        { return m_wordingErrorData; }
    [[nodiscard]] const ListCtrlExNumericDataProvider* GetClicheData() const noexcept
        { return m_clichePhraseData; }
    [[nodiscard]] const ListCtrlExNumericDataProvider* GetAllWordsBatchData() const noexcept
        { return m_allWordsBatchData; }
    [[nodiscard]] const ListCtrlExNumericDataProvider* GetImportantWordsBatchData() const noexcept
        { return m_importantWordsBatchData; }
    [[nodiscard]] const ListCtrlExNumericDataProvider* GetLongSentencesData() const noexcept
        { return m_overlyLongSentenceData; }
    [[nodiscard]] const ListCtrlExNumericDataProvider* GetConjunctionStartingSentencesData() const noexcept
        { return m_sentenceStartingWithConjunctionsData; }
    [[nodiscard]] const ListCtrlExNumericDataProvider* GetLowerCasedSentencesData() const noexcept
        { return m_sentenceStartingWithLowercaseData; }
private:
    [[nodiscard]] ListCtrlExNumericDataProvider* GetMisspelledWordData() noexcept
        { return m_misspelledWordData; }
    [[nodiscard]] ListCtrlExNumericDataProvider* GetRepeatedWordData() noexcept
        { return m_dupWordData; }
    [[nodiscard]] ListCtrlExNumericDataProvider* GetAllWordsBatchData() noexcept
        { return m_allWordsBatchData; }
    [[nodiscard]] ListCtrlExNumericDataProvider* GetImportantWordsBatchData() noexcept
        { return m_importantWordsBatchData; }
    void DisplayFleschChart();
    void DisplayDB2Plot();
    void DisplayCrawfordGraph();
    void DisplayLixGauge();
    void DisplayGermanLixGauge();
    static constexpr int CUMULATIVE_STATS_COUNT = 13;
    void LoadProjectFile(const char* projectFileText, const size_t textLength);
    bool RunProjectWizard(const wxString& path);
    bool LoadDocuments(wxProgressDialog& progressDlg);
    void LoadScoresSection();
    void LoadWarningsSection();
    void LoadHardWordsSection();
    void LoadDolchSection();
    void DisplayReadabilityGraphs();
    void DisplayHistograms();
    void DisplayHistogram(const wxString& name, const wxWindowID Id, const wxString& topLabel, const wxString& bottomLabel,
                          std::shared_ptr<const Wisteria::Data::Dataset> data, const bool includeTest, const bool isTestGradeLevel,
                          const bool startAtOne);
    void DisplayBoxPlots();
    void DisplayScores();
    void DisplayWarnings();
    void DisplayGrammar();
    void DisplaySentencesBreakdown();
    void DisplayHardWords();
    void DisplaySightWords();

    void InitializeDocuments();
    /** Build a list of unique labels from the documents. If there is a low number of
        labels, then these should be treated like grouping labels (and used as grouping for the graphs).
        Otherwise, they are probably just document descriptions (pulled from their metadata).*/
    void LoadGroupingLabelsFromDocumentsInfo();
    void SyncFilePathsWithDocuments();

    void ShowQueuedMessages() final;

    /** Removes documents that failed to be loaded (usually because they couldn't be found
        or if they didn't contain enough text).
        Along with removing the loaded document, it will remove the file from the project settings.
        @returns @c true if the user requested to removed failed documents from the project.*/
    bool CheckForFailedDocuments();
    void RemoveFailedDocuments();

    /** @brief Fills a row in a data grid with statistics.
        @param dataGrid The data grid to write to.
        @param rowName The label for the first column. Usually a test name or a document name.
        @param optionalDescription An optional description for the second column.
         If empty, then won't be used for the second column.
         This parameter only makes sense for adding a description next to a document.
        @param rowNum The row index to write to inside of @c dataGrid.
        @param data The data to analyze.
        @param decimalSize The amount of floating-point precision to use.
        @param varianceMethod The variance method to calculate with (population or sample).
        @param allowCustomFormatting Whether custom formatting should be used for number formatting
         in the @c dataGrid.*/
    static void SetScoreStatsRow(ListCtrlExNumericDataProvider* dataGrid,
                                 const wxString& rowName,
                                 const wxString& optionalDescription,
                                 const long rowNum,
                                 const std::vector<double>& data, const int decimalSize,
                                 const VarianceMethod varianceMethod,
                                 const bool allowCustomFormatting);
    /** @brief Fills a list control with data.
        @param The name of the list control.
        @param windowId The window ID of the list control.
        @param data The data grid used for the list control (will be written to).
        @param firstColumnName The label for the first column in @c data.
        @param optionalSecondColumnName An optional label for the second column.
         If empty, will not be used for the second column.
         This parameter only makes sense for adding a description next to a document.
        @param multiSelectable True to make the list control multi-item selectable.*/
    void DisplaySummaryStatisticsWindow(const wxString& windowName, const int windowId,
                                        ListCtrlExNumericDataProvider* data,
                                        const wxString& firstColumnName,
                                        const wxString& optionalSecondColumnName,
                                        const bool multiSelectable);

    /// @returns `true` if there is more than one document label, but not more than
    ///  the max number of groups for the legends.
    [[nodiscard]]
    bool IsShowingGroupLegends() const noexcept
        {
        return (GetDocumentLabels().size() <= GetMaxGroupCount() &&
                GetDocumentLabels().size() > 1);
        }

    [[nodiscard]]
    wxString GetScoreColumnName() const
        { return _DT(L"SCORE"); }
    [[nodiscard]]
    wxString GetGroupColumnName() const
        { return _DT(L"GROUP"); }

    bool m_adjustParagraphParserForDocFiles{ false };
    std::vector<std::shared_ptr<Wisteria::Data::Dataset>> m_customTestScores;

    size_t m_maxGroupCount{ 10 };

    Wisteria::Colors::Schemes::EarthTones m_legendScheme;
    Wisteria::Icons::Schemes::StandardShapes m_iconScheme;

    std::vector<BaseProject*> m_docs;
    std::map<traits::case_insensitive_wstring_ex, Wisteria::Data::GroupIdType> m_docLabels;
    Wisteria::Data::ColumnWithStringTable::StringTableType m_groupStringTable;
    // score list data
    ListCtrlExNumericDataProvider* m_scoreRawData{ new ListCtrlExNumericDataProvider };
    ListCtrlExNumericDataProvider* m_goalsData{ new ListCtrlExNumericDataProvider };
    ListCtrlExNumericDataProvider* m_scoreStatsData{ new ListCtrlExNumericDataProvider };
    ListCtrlExNumericDataProvider* m_aggregatedGradeScoresData{ new ListCtrlExNumericDataProvider };
    ListCtrlExNumericDataProvider* m_aggregatedClozeScoresData{ new ListCtrlExNumericDataProvider };
    // grammar list data
    ListCtrlExNumericDataProvider* m_dupWordData{ new ListCtrlExNumericDataProvider };
    ListCtrlExNumericDataProvider* m_incorrectArticleData{ new ListCtrlExNumericDataProvider };
    ListCtrlExNumericDataProvider* m_passiveVoiceData{ new ListCtrlExNumericDataProvider };
    ListCtrlExNumericDataProvider* m_misspelledWordData{ new ListCtrlExNumericDataProvider };
    ListCtrlExNumericDataProvider* m_wordyPhraseData{ new ListCtrlExNumericDataProvider };
    ListCtrlExNumericDataProvider* m_redundantPhraseData{ new ListCtrlExNumericDataProvider };
    ListCtrlExNumericDataProvider* m_wordingErrorData{ new ListCtrlExNumericDataProvider };
    ListCtrlExNumericDataProvider* m_overusedWordBySentenceData{ new ListCtrlExNumericDataProvider };
    ListCtrlExNumericDataProvider* m_clichePhraseData{ new ListCtrlExNumericDataProvider };
    ListCtrlExNumericDataProvider* m_overlyLongSentenceData{ new ListCtrlExNumericDataProvider };
    ListCtrlExNumericDataProvider* m_sentenceStartingWithConjunctionsData{ new ListCtrlExNumericDataProvider };
    ListCtrlExNumericDataProvider* m_sentenceStartingWithLowercaseData{ new ListCtrlExNumericDataProvider };
    ListCtrlExNumericDataProvider* m_allWordsBatchData{ new ListCtrlExNumericDataProvider };
    ListCtrlExNumericDataProvider* m_importantWordsBatchData{ new ListCtrlExNumericDataProvider };
    // difficult words list data
    ListCtrlExNumericDataProvider* m_hardWordsData{ new ListCtrlExNumericDataProvider };
    // warnings list data
    ListCtrlExDataProvider* m_warnings{ new ListCtrlExDataProvider };
    // dolch list data
    ListCtrlExNumericDataProvider* m_dolchCompletionData{ new ListCtrlExNumericDataProvider };
    ListCtrlExNumericDataProvider* m_dolchWordsBatchData{ new ListCtrlExNumericDataProvider };
    ListCtrlExNumericDataProvider* m_NonDolchWordsData{ new ListCtrlExNumericDataProvider };

    wxDECLARE_DYNAMIC_CLASS(BatchProjectDoc);
    };

#endif //__BATCH_PROJECT_DOC_H__
