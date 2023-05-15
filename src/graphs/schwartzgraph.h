/** @addtogroup Graphics
    @brief Graphing classes.
    @date 2005-2023
    @copyright Blake Madden
    @author Blake Madden
    @details This program is free software; you can redistribute it and/or modify
     it under the terms of the 3-Clause BSD License.

     SPDX-License-Identifier: BSD-3-Clause
@{*/

#ifndef __SCHWARTZ_GRAPH_H__
#define __SCHWARTZ_GRAPH_H__

#include "polygon_readability_graph.h"

namespace Wisteria::Graphs
    {
    /** @brief German readability graph, similar to Fry.

        @par %Data:
         This plot accepts a Data::Dataset where there are continuous columns for
         the word, syllable, and sentences counts for a sample.
         The ID column's labels will be associated with each point,
         so it is recommended to fill this column with the documents' (or samples') names.

         A categorical column can also optionally be used as a grouping variable.

        @par Missing Data:
         - Scores that are missing data will not be plotted.
         - Blank IDs will apply blank selection labels to their respective points.
         - Blank group labels will be lumped into a "[NO GROUP]" category.

        @par Citation:
            Schwartz, Roswitha E. W. “An Exploratory Effort to Design a Readability Graph for German Material.” 1975.
            State University of New York at Albany, Reading Department. Unpublished study.*/
    class SchwartzGraph final : public PolygonReadabilityGraph
        {
    public:
        /** @brief Constructor.
            @param canvas The parent canvas to render on.
            @param colors The color scheme to apply to the points.
               Leave as null to use the default theme.
            @param shapes The shape scheme to use for the points.
               Leave as null to use the standard shapes.*/
        explicit SchwartzGraph(Wisteria::Canvas* canvas,
            std::shared_ptr<Wisteria::Colors::Schemes::ColorScheme> colors = nullptr,
            std::shared_ptr<Wisteria::Icons::Schemes::IconScheme> shapes = nullptr);

        /** @brief Sets the data.
            @param data The data to use.
            @param numberOfWordsColumnName The column containing the number of words
                in the sample (a continuous column).
            @param numberOfSyllablesColumnName The column containing the number of syllables
                in the sample (a continuous column).
            @param numberOfSentencesColumnName The column containing the number of sentences
                in the sample (a continuous column).
            @param groupColumnName The (optional) categorical column to use for grouping.
            @note Pass in the raw numbers of the sample, this function will normalize it
                to a 100-word sample for you.
            @throws std::runtime_error If any columns can't be found, throws an exception.\n
                The exception's @c what() message is UTF-8 encoded, so pass it to
                @c wxString::FromUTF8() when formatting it for an error message.*/
        void SetData(std::shared_ptr<const Wisteria::Data::Dataset> data,
                     const wxString& numberOfWordsColumnName,
                     const wxString& numberOfSyllablesColumnName,
                     const wxString& numberOfSentencesColumnName,
                     std::optional<const wxString> groupColumnName = std::nullopt);

        /// @returns The results calculated from the last call to SetData().
        /// @note These scores will be aligned with the order of the data from
        ///     when it was initially passed to SetData().
        [[nodiscard]]
        const std::vector<Wisteria::ScorePoint>& GetScores() const noexcept
            { return m_results; }
    private:
        void RecalcSizes(wxDC& dc) final;
        void CalculateScorePositions(wxDC& dc) final;

        void BuildBackscreen();

        std::array<wxPoint, 15> m_gradeLinePoints;
        std::array<wxPoint, 16>  m_longSentencesPoints;
        std::array<wxPoint, 31> m_longWordPoints;
        std::array<wxPoint, 9> m_dividerLinePoints;
        std::array<wxPoint, 9> m_grade1Polygon;
        std::array<wxPoint, 16> m_grade2Polygon;
        std::array<wxPoint, 9> m_grade3to4Polygon;
        std::array<wxPoint, 5> m_grade5to6Polygon;
        std::array<wxPoint, 11> m_grade7to8Polygon;
        std::array<wxPoint, 12> m_gradeOver8Polygon;

        const Wisteria::Data::Column<double>* m_numberOfWordsColumn{ nullptr };
        const Wisteria::Data::Column<double>* m_numberOfSyllablesColumn{ nullptr };
        const Wisteria::Data::Column<double>* m_numberOfSentencesColumn{ nullptr };
        std::vector<Wisteria::ScorePoint> m_results;

        // used for consistent polygon calculations
        std::unique_ptr<SchwartzGraph> m_backscreen{ nullptr };
        };
    }

/** @}*/

#endif //__SCHWARTZ_GRAPH_H__
