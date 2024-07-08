/** @addtogroup Graphics
    @brief Graphing classes.
    @date 2005-2020
    @copyright Oleander Software, Ltd.
    @author Blake Madden
    @details This program is free software; you can redistribute it and/or modify
    it under the terms of the BSD License.
* @{*/

#ifndef __READABILITY_GRAPH_H__
#define __READABILITY_GRAPH_H__

#include "../Wisteria-Dataviz/src/base/colorbrewer.h"
#include "../Wisteria-Dataviz/src/graphs/groupgraph2d.h"
#include "../results-format/readability_messages.h"
#include "scorepoint.h"
#include <array>

namespace Wisteria::Graphs
    {
    /** @brief Readability graph base class which uses polygons to determine a test score.*/
    class PolygonReadabilityGraph : public GroupGraph2D
        {
        wxDECLARE_DYNAMIC_CLASS(PolygonReadabilityGraph);
        PolygonReadabilityGraph() = default;

      public:
        /** @brief Constructor.
            @param canvas The parent canvas to render on.*/
        explicit PolygonReadabilityGraph(Wisteria::Canvas* canvas)
            : GroupGraph2D(canvas), m_graphInvalidAreaColor(Wisteria::Colors::ColorBrewer::GetColor(
                                        Wisteria::Colors::Color::Red)),
              m_fancyFontFaceName(Wisteria::GraphItems::Label::GetFirstAvailableFont(
                  { DONTTRANSLATE(L"Georgia", DTExplanation::FontName),
                    DONTTRANSLATE(L"Century Gothic"), DONTTRANSLATE(L"Century"),
                    DONTTRANSLATE(L"Garamond") }))
            {
            }

        /// @brief A label display on a level (e.g., grade level).
        struct LevelLabel
            {
          public:
            /** @brief Constructor.
                @param x The x axis position.
                @param y The y axis position.
                @param label The label's display string.
                @param startValue The start value of the level's range of values.
                @param endValue The end value of the level's range of values.*/
            LevelLabel(const double x, const double y, const wxString& label,
                       const double startValue, const double endValue)
                : m_x(x), m_y(y), m_value1(startValue), m_value2(endValue), m_label(label)
                {
                }

            /// @returns @c true if a @c value falls within the range of the level.
            [[nodiscard]]
            bool
            operator==(const double value) const noexcept
                {
                return value >= m_value1 && value <= m_value2;
                }

            /// @returns The x axis position of the label.
            [[nodiscard]]
            double GetX() const noexcept
                {
                return m_x;
                }

            /// @returns The y axis position of the label.
            [[nodiscard]]
            double GetY() const noexcept
                {
                return m_y;
                }

            /// @returns The label's display string.
            [[nodiscard]]
            const wxString& GetLabel() const noexcept
                {
                return m_label;
                }

          private:
            double m_x{ 0 };
            double m_y{ 0 };
            double m_value1{ 0 };
            double m_value2{ 0 };
            wxString m_label;
            };

        /// @returns The face name of a "fancy" fontface that can be used on the graphs.
        const wxString& GetFancyFontFaceName() const noexcept { return m_fancyFontFaceName; }

        /// @returns The color used for invalid areas (e.g., on a Fry graph).
        [[nodiscard]]
        const wxColour& GetInvalidAreaColor() const noexcept
            {
            return m_graphInvalidAreaColor;
            }

        /** @brief Sets the color used for invalid areas.
            @param color The color to use.*/
        void SetInvalidAreaColor(const wxColour& color)
            {
            if (color.IsOk())
                {
                m_graphInvalidAreaColor = color;
                }
            }

        /// @brief Sets the extended label information usually shown when a region is selected.
        /// @param cat The message catalog to use.
        void SetMessageCatalog(const ReadabilityMessages* cat) noexcept { m_readMessages = cat; }

        /// @returns The extended label information usually shown when a region is selected.
        [[nodiscard]]
        const ReadabilityMessages* GetMessageCatalog() const noexcept
            {
            return m_readMessages;
            }

        /// @brief Adds a label to be shown on a level.
        /// @param label The level label to add.
        void AddLevelLabel(const LevelLabel& label) { m_levelLabels.push_back(label); }

        /// @returns The labels displayed on various levels (e.g., grade levels).
        [[nodiscard]]
        std::vector<LevelLabel>& GetLevelLabels() noexcept
            {
            return m_levelLabels;
            }

        /// @private
        [[nodiscard]]
        const std::vector<LevelLabel>& GetLevelLabels() const noexcept
            {
            return m_levelLabels;
            }

        /// @returns Whether the score is being showcases.
        /// @sa ShowcaseScore().
        [[nodiscard]]
        bool IsShowcasingScore() const noexcept
            {
            return m_showcaseScore;
            }

        /// @brief Makes most areas of the graph translucent except for where the score is,
        ///     drawing attention to it.
        /// @param showcase @c true to showcase where the score is.
        /// @note This will have no effect if multiple scores are shown.
        void ShowcaseScore(const bool showcase) noexcept { m_showcaseScore = showcase; }

      protected:
        /// @brief Can be derived for custom score position calculations when laying out the plot.
        /// @param dc The measuring DC.
        virtual void CalculateScorePositions([[maybe_unused]] wxDC& dc) {}

        /** This is a more liberal than calling `Polygon::IsInsidePolygon()` because it
                will see if the point next to or below the point are also in the polygon.
                This helps prevent scores that are right on a line to switch between regions
                when different screen resolutions. This way, it will error on the side of going
                into the more difficult region.
            @note The backscreened plot that this is used on is not DPI scaled, so the offset
                values should be treated like DIPs (although technically, they aren't).
            @param pt The point to review.
            @param polygon The points that make up the polygon (region) to test inside of.
            @param N The number of points in the @c polygon.
            @param xOffset How much "wiggle room" (in pixels) to test around to
                x axis of the polygon's vertices.
            @param yOffset How much "wiggle room" (in pixels) to test around to
                y axis of the polygon's vertices.*/
        [[nodiscard]]
        static bool IsScoreInsideRegion(const wxPoint pt, const wxPoint* polygon, const int N,
                                        const int xOffset, const int yOffset)
            {
            // see if the point is even in the polygon's bounding box, then see if it's
            // actually in the polygon
            return Wisteria::GraphItems::Polygon::GetPolygonBoundingBox(polygon, N).Contains(pt) ?
                       (Wisteria::GraphItems::Polygon::IsInsidePolygon(pt, polygon, N) ||
                        Wisteria::GraphItems::Polygon::IsInsidePolygon(
                            wxPoint(pt.x + xOffset, pt.y), polygon, N) ||
                        Wisteria::GraphItems::Polygon::IsInsidePolygon(
                            wxPoint(pt.x, pt.y + yOffset), polygon, N)) :
                       false;
            }

        /** This is a more liberal than calling `Polygon::IsInsidePolygon()` because it
                will see if the point next to or below the point are also in the polygon.
                This helps prevent scores that are right on a line to switch between regions
                when different screen resolutions. This way, it will error on the side of going
                into the more difficult region.
            @note The backscreened plot that this is used on is not DPI scaled, so the offset
                values should be treated like DIPs (although technically, they aren't).
            @param pt The point to review.
            @param polygon The array of points that make up the polygon (region) to test inside of.
            @param xOffset How much "wiggle room" (in pixels) to test around to
                x axis of the polygon's vertices.
            @param yOffset How much "wiggle room" (in pixels) to test around to
                y axis of the polygon's vertices.*/
        template<typename polygonT>
        [[nodiscard]]
        static bool IsScoreInsideRegion(const wxPoint pt, const polygonT& polygon,
                                        const int xOffset, const int yOffset)
            {
            // see if the point is even in the polygon's bounding box, then see if it's
            // actually in the polygon
            return Wisteria::GraphItems::Polygon::GetPolygonBoundingBox(&polygon[0], polygon.size())
                           .Contains(pt) ?
                       (Wisteria::GraphItems::Polygon::IsInsidePolygon(pt, &polygon[0],
                                                                       polygon.size()) ||
                        Wisteria::GraphItems::Polygon::IsInsidePolygon(
                            wxPoint(pt.x + xOffset, pt.y), &polygon[0], polygon.size()) ||
                        Wisteria::GraphItems::Polygon::IsInsidePolygon(
                            wxPoint(pt.x, pt.y + yOffset), &polygon[0], polygon.size())) :
                       false;
            }

      private:
        bool m_showcaseScore{ false };
        std::vector<LevelLabel> m_levelLabels;
        wxColour m_graphInvalidAreaColor;
        wxString m_fancyFontFaceName;
        const ReadabilityMessages* m_readMessages{ nullptr };
        };
    } // namespace Wisteria::Graphs

/** @}*/

#endif //__READABILITY_GRAPH_H__
