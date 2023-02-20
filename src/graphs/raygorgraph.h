/** @addtogroup Graphics
    @brief Graphing classes.
    @date 2005-2023
    @copyright Blake Madden
    @author Blake Madden
    @details This program is free software; you can redistribute it and/or modify
     it under the terms of the 3-Clause BSD License.

     SPDX-License-Identifier: BSD-3-Clause
@{*/

#ifndef __RAYGOR_GRAPH_H__
#define __RAYGOR_GRAPH_H__

#include "polygon_readability_graph.h"

namespace Wisteria::Graphs
    {
    /// @brief Raygor readability graph.
    class RaygorGraph final : public PolygonReadabilityGraph
        {
    public:
        /** @brief Constructor.
            @param canvas The parent canvas to render on.
            @param colors The color scheme to apply to the points.
               Leave as null to use the default theme.
            @param shapes The shape scheme to use for the points.
               Leave as null to use the standard shapes.*/
        explicit RaygorGraph(Wisteria::Canvas* canvas,
            std::shared_ptr<Wisteria::Colors::Schemes::ColorScheme> colors = nullptr,
            std::shared_ptr<Wisteria::Icons::Schemes::IconScheme> shapes = nullptr);

        /** @brief Sets the data.
            @param data The data to use.
            @param numberOfWordsColumnName The column containing the number of words
                (excluding numerals) in the sample (a continuous column).
            @param numberOf6PlusCharWordsColumnName The column containing the number of 6+
                character words (excluding numerals) in the sample (a continuous column).
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
                     const wxString& numberOf6PlusCharWordsColumnName,
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

        std::array<wxPoint, 27> m_gradeLinePoints;
        std::array<wxPoint, 8>  m_longSentencesPoints;
        std::array<wxPoint, 10> m_longWordPoints;
        std::array<wxPoint, 10> m_dividerLinePoints;

        const Wisteria::Data::Column<double>* m_numberOfWordsColumn{ nullptr };
        const Wisteria::Data::Column<double>* m_numberOf6PlusCharWordsColumn{ nullptr };
        const Wisteria::Data::Column<double>* m_numberOfSentencesColumn{ nullptr };
        std::vector<Wisteria::ScorePoint> m_results;

        // used for consistent polygon calculations
        std::unique_ptr<RaygorGraph> m_backscreen{ nullptr };
        };
    }

/** @}*/

#endif //__RAYGOR_GRAPH_H__
