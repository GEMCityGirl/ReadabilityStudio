///////////////////////////////////////////////////////////////////////////////
// Name:        frasegraph.cpp
// Author:      Blake Madden
// Copyright:   (c) 2005-2023 Blake Madden
// Licence:     3-Clause BSD licence
// SPDX-License-Identifier: BSD-3-Clause
///////////////////////////////////////////////////////////////////////////////

#include "frasegraph.h"

using namespace Wisteria;
using namespace Wisteria::Colors;
using namespace Wisteria::GraphItems;

namespace Wisteria::Graphs
    {
    //----------------------------------------------------------------
    FraseGraph::FraseGraph(Wisteria::Canvas* canvas,
            std::shared_ptr<Wisteria::Colors::Schemes::ColorScheme> colors /*= nullptr*/,
            std::shared_ptr<Wisteria::Icons::Schemes::IconScheme> shapes /*= nullptr*/) :
        PolygonReadabilityGraph(canvas)
        {
        SetColorScheme(colors != nullptr ? colors :
            Settings::GetDefaultColorScheme());
        SetShapeScheme(shapes != nullptr ? shapes :
            std::make_shared<Wisteria::Icons::Schemes::IconScheme>(
                Wisteria::Icons::Schemes::StandardShapes()));

        GetCanvas()->SetLabel(_("FRASE Graph"));
        GetCanvas()->SetName(_("FRASE Graph"));
        GetLeftYAxis().GetTitle().SetText(
            _("Average number of sentences per 100 words"));
        GetBottomXAxis().GetTitle().SetText(
            _("Average number of syllables per 100 words"));
        GetTitle() = GraphItems::Label(GraphItemInfo(
            _("FRASE (Fry Readability Adaptation for Spanish Evaluation) Graph")).
            Pen(wxNullPen));

        // set up the X axis
        GetBottomXAxis().SetRange(182, 234, 0, 1, 1);
        GetBottomXAxis().SetDisplayInterval(4, 2);
        GetBottomXAxis().GetGridlinePen() = wxNullPen;
        GetBottomXAxis().SetTickMarkDisplay(Axis::TickMark::DisplayType::Inner);
        GetBottomXAxis().SetTickMarkInterval(2);
        GetBottomXAxis().SetMajorTickMarkLength(10);
        GetBottomXAxis().SetMinorTickMarkLength(10);

        // set up the Y axis
        GetLeftYAxis().SetRange(0, 15, 0, 1, 1);
        GetLeftYAxis().ShowOuterLabels(false);
        GetLeftYAxis().GetGridlinePen() = wxNullPen;
        GetLeftYAxis().SetTickMarkDisplay(Axis::TickMark::DisplayType::Inner);
        GetLeftYAxis().SetTickMarkInterval(1);
        GetLeftYAxis().SetMajorTickMarkLength(10);
        GetLeftYAxis().SetMinorTickMarkLength(10);

        // add the labels for the grades
        AddLevelLabel(LevelLabel(204, 13, _("BEGINNING\nLEVEL\nI"), 1, 1));
        AddLevelLabel(LevelLabel(215.5, 10, _("INTERMEDIATE\nLEVEL\nII"), 2, 2));
        AddLevelLabel(LevelLabel(223.5, 8, _("ADVANCED\nINTERMEDIATE\nLEVEL\nIII"), 3, 3));
        AddLevelLabel(LevelLabel(230, 6, _("ADVANCED\nLEVEL\nIV"), 4, 4));
        }

    //----------------------------------------------------------------
    void FraseGraph::SetData(std::shared_ptr<const Wisteria::Data::Dataset> data,
        const wxString& numberOfWordsColumnName,
        const wxString& numberOfSyllablesColumnName,
        const wxString& numberOfSentencesColumnName,
        std::optional<const wxString> groupColumnName /*= std::nullopt*/)
        {
        SetDataset(data);
        ResetGrouping();
        m_results.clear();
        m_numberOfWordsColumn = m_numberOfSyllablesColumn =
            m_numberOfSentencesColumn = nullptr;
        GetSelectedIds().clear();

        if (GetDataset() == nullptr)
            { return; }

        SetGroupColumn(groupColumnName);

        // if grouping, build the list of group IDs, sorted by their respective labels
        if (IsUsingGrouping())
            { BuildGroupIdMap(); }

        m_numberOfWordsColumn =
            GetContinuousColumnRequired(numberOfWordsColumnName);
        m_numberOfSyllablesColumn =
            GetContinuousColumnRequired(numberOfSyllablesColumnName);
        m_numberOfSentencesColumn =
            GetContinuousColumnRequired(numberOfSentencesColumnName);

        BuildBackscreen();
        }

    //----------------------------------------------------------------
    void FraseGraph::BuildBackscreen()
        {
        if (m_backscreen == nullptr)
            {
            // Shape and color schemes are irrelevant, as this is used
            // only for the base polygon calculations. 
            m_backscreen = std::make_unique<FraseGraph>(GetCanvas());
            m_backscreen->SetMessageCatalog(GetMessageCatalog());
            // Also, leave the data as null, as that isn't used either for
            // simply creating polygons. All scaling should just be 1.0.
            m_backscreen->SetDataset(nullptr);
            m_backscreen->SetScaling(1.0);
            m_backscreen->SetDPIScaleFactor(1.0);
            wxGCDC dc(GetCanvas());
            // Just use this size as-is, don't adjust it from DIP -> DPI under HiDPI.
            // We are just interested in having a 700x500 size window to to do polygon
            // collision within; this isn't actually being presented. This is also why
            // this canvas's DPI scale factor is hard coded to 1.9 (above).
            m_backscreen->SetBoundingBox(
                wxSize(Canvas::GetDefaultCanvasWidthDIPs(),
                       Canvas::GetDefaultCanvasHeightDIPs()),
                dc, 1.0);
            m_backscreen->RecalcSizes(dc);
            }
        }

    //----------------------------------------------------------------
    void FraseGraph::RecalcSizes(wxDC& dc)
        {
        Graph2D::RecalcSizes(dc);

        wxASSERT_MSG((!m_backscreen ||
            m_backscreen->GetBoundingBox(dc).GetWidth() ==
            Canvas::GetDefaultCanvasWidthDIPs()), L"Invalid backscreen size!");
        wxASSERT_MSG((!m_backscreen ||
            m_backscreen->GetBoundingBox(dc).GetHeight() ==
            Canvas::GetDefaultCanvasHeightDIPs()), L"Invalid backscreen size!");

        // divider line
        GetPhyscialCoordinates(188, 15, m_dividerLinePoints[0]);
        GetPhyscialCoordinates(196, 12, m_dividerLinePoints[1]);
        GetPhyscialCoordinates(204, 9, m_dividerLinePoints[2]);
        GetPhyscialCoordinates(210, 7, m_dividerLinePoints[3]);
        GetPhyscialCoordinates(217, 5, m_dividerLinePoints[4]);
        GetPhyscialCoordinates(224, 3.8, m_dividerLinePoints[5]);
        GetPhyscialCoordinates(230, 3.5, m_dividerLinePoints[6]);
        GetPhyscialCoordinates(234, 3.75, m_dividerLinePoints[7]);

        // I
        GetPhyscialCoordinates(182, 3.1, m_levelLinePoints[0]);
        GetPhyscialCoordinates(182, 15, m_levelLinePoints[1]);
        GetPhyscialCoordinates(224, 15, m_levelLinePoints[2]);
        // II
        GetPhyscialCoordinates(224, 15, m_levelLinePoints[3]);
        GetPhyscialCoordinates(182, 3.1, m_levelLinePoints[4]);
        GetPhyscialCoordinates(182, 0, m_levelLinePoints[5]);
        // III
        GetPhyscialCoordinates(196, 0, m_levelLinePoints[6]);
        GetPhyscialCoordinates(234, 15, m_levelLinePoints[7]);
        // IV
        GetPhyscialCoordinates(234, 10.75, m_levelLinePoints[8]);
        GetPhyscialCoordinates(210.5, 0, m_levelLinePoints[9]);
        GetPhyscialCoordinates(234, 0, m_levelLinePoints[10]);

        const wxBrush selectionBrush =
            wxBrush(ColorContrast::ChangeOpacity(ColorBrewer::GetColor(Color::LightGray), 100));
        const wxColour lightGray{ wxColour(0, 0, 0, 200) };

        // I
        AddObject(std::make_shared<GraphItems::Polygon>(GraphItemInfo().Pen(wxNullPen).
            Text(_("Beginner Level")).Brush(wxNullBrush).SelectionBrush(selectionBrush),
            &m_levelLinePoints[0], 3));
        // II
        AddObject(std::make_shared<GraphItems::Polygon>(GraphItemInfo().Pen(wxNullPen).
            Text(_("Intermediate Level")).Brush(wxNullBrush).SelectionBrush(selectionBrush),
            &m_levelLinePoints[3], 5));
        // III
        AddObject(std::make_shared<GraphItems::Polygon>(GraphItemInfo().Pen(wxNullPen).
            Text(_("Advanced Intermediate Level")).Brush(wxNullBrush).SelectionBrush(selectionBrush),
            &m_levelLinePoints[6], 4));
        // IV
        AddObject(std::make_shared<GraphItems::Polygon>(GraphItemInfo().Pen(wxNullPen).
            Text(_("Advanced Level")).Brush(wxNullBrush).SelectionBrush(selectionBrush),
            &m_levelLinePoints[8], 3));

        // separator line
        auto levelsSpline = std::make_shared<GraphItems::Polygon>(
            GraphItemInfo().Pen(wxPen(lightGray)).
            Brush(lightGray).SelectionBrush(selectionBrush).
            Scaling(GetScaling()),
            m_dividerLinePoints);
        levelsSpline->SetShape(Polygon::PolygonShape::Spline);
        AddObject(levelsSpline);

        // draw the region lines
        AddObject(std::make_shared<GraphItems::Polygon>(GraphItemInfo().Pen(wxPen(lightGray)).
            Brush(lightGray).SelectionBrush(selectionBrush).
            Scaling(GetScaling()),
            &m_levelLinePoints[3], 2));
        AddObject(std::make_shared<GraphItems::Polygon>(GraphItemInfo().Pen(wxPen(lightGray)).
            Brush(lightGray).SelectionBrush(selectionBrush).
            Scaling(GetScaling()),
            &m_levelLinePoints[6], 2) );
        AddObject(std::make_shared<GraphItems::Polygon>(GraphItemInfo().Pen(wxPen(lightGray)).
            Brush(lightGray).SelectionBrush(selectionBrush).
            Scaling(GetScaling()),
            &m_levelLinePoints[8], 2) );

        CalculateScorePositions(dc);

        // draw the quadrant labels
        wxPoint pt1;
        const wxFont quadLabelFont(
            wxFont(wxSystemSettings::GetFont(wxSYS_DEFAULT_GUI_FONT).GetPointSize()*1.25f,
                wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD,
                false, L"Times New Roman"));
        for (const auto& level : GetLevelLabels())
            {
            GetPhyscialCoordinates(level.GetX(), level.GetY(), pt1);
            auto levelLabel = std::make_shared<GraphItems::Label>(
                GraphItemInfo(level.GetLabel()).
                Scaling(GetScaling()).
                Pen(wxNullPen).Font(quadLabelFont).AnchorPoint(pt1));
            levelLabel->SetTextAlignment(TextAlignment::Centered);
            levelLabel->SetFontColor(
                (GetScores().size() == 1 &&
                 level == GetScores().front().GetScore()) ?
                *wxRED : *wxBLACK);
            AddObject(levelLabel);
            }
        }

    //----------------------------------------------------------------
    void FraseGraph::CalculateScorePositions(wxDC& dc)
        {
        if (GetDataset() == nullptr)
            { return; }

        wxASSERT_MSG(m_backscreen, L"Backscreen not set!");
        wxASSERT_MSG(m_backscreen->GetBoundingBox(dc).GetWidth() ==
            Canvas::GetDefaultCanvasWidthDIPs(), L"Invalid backscreen size!");
        wxASSERT_MSG(m_backscreen->GetBoundingBox(dc).GetHeight() ==
            Canvas::GetDefaultCanvasHeightDIPs(), L"Invalid backscreen size!");

        auto points = std::make_shared<GraphItems::Points2D>(wxNullPen);
        points->SetScaling(GetScaling());
        points->SetDPIScaleFactor(GetDPIScaleFactor());
        points->Reserve(GetDataset()->GetRowCount());
        // these will all be filled with something, even if NaN
        m_results.resize(GetDataset()->GetRowCount());
        for (size_t i = 0; i < GetDataset()->GetRowCount(); ++i)
            {
            if (std::isnan(m_numberOfWordsColumn->GetValue(i)))
                {
                m_results[i].SetScoreInvalid(true);
                continue;
                }

            const double normalizationFactor =
                safe_divide<double>(100, m_numberOfWordsColumn->GetValue(i));

            // add the score to the grouped data
            m_results[i] =
                Wisteria::ScorePoint(
                    std::clamp<double>(
                        normalizationFactor * m_numberOfSyllablesColumn->GetValue(i),
                        182, 234),
                    std::clamp<double>(
                        normalizationFactor * m_numberOfSentencesColumn->GetValue(i),
                        0, 15));

            m_results[i].ResetStatus();

            const auto calcScoreFromPolygons = [](
                    const std::unique_ptr<FraseGraph>& graph,
                    ScorePoint& scorePoint)
                {
                // see where the point is
                if (!graph->GetPhyscialCoordinates(scorePoint.m_wordStatistic,
                                            scorePoint.m_sentenceStatistic,
                                            scorePoint.m_scorePoint))
                    {
                    scorePoint.SetScoreInvalid(true);
                    return false;
                    }

                if (IsScoreInsideRegion(scorePoint.m_scorePoint, &graph->m_levelLinePoints[8], 3, 1, 1) )
                    { scorePoint.SetScore(4); }
                else if (IsScoreInsideRegion(scorePoint.m_scorePoint, &graph->m_levelLinePoints[6], 4, 1, 1) )
                    { scorePoint.SetScore(3); }
                else if (IsScoreInsideRegion(scorePoint.m_scorePoint, &graph->m_levelLinePoints[3], 5, 1, 1) )
                    { scorePoint.SetScore(2); }
                else if (IsScoreInsideRegion(scorePoint.m_scorePoint, &graph->m_levelLinePoints[0], 3, 1, 1) )
                    { scorePoint.SetScore(1); }
                else
                    {
                    scorePoint.SetScoreOutOfGradeRange(true);
                    scorePoint.SetScoreInvalid(true);
                    }

                // if in a valid grade area see if it leans towards having harder sentences or words
                if (!scorePoint.IsScoreInvalid())
                    {
                    if (IsScoreInsideRegion(scorePoint.m_scorePoint,
                        graph->m_dividerLinePoints, 1, 1))
                        {
                        scorePoint.SetSentencesHard(false);
                        scorePoint.SetWordsHard(true);
                        }
                    else
                        {
                        scorePoint.SetWordsHard(false);
                        scorePoint.SetSentencesHard(true);
                        }
                    }

                return true;
                };

            calcScoreFromPolygons(m_backscreen, m_results[i]);

            // Convert group ID into color scheme index
            // (index is ordered by labels alphabetically).
            // Note that this will be zero if grouping is not in use.
            const size_t colorIndex = IsUsingGrouping() ?
                GetSchemeIndexFromGroupId(GetGroupColumn()->GetValue(i)) :
                0;
            
            // see where the point is on this graph (not the backscreen) and
            // add it to be physically plotted
            if (GetPhyscialCoordinates(m_results[i].m_wordStatistic,
                                       m_results[i].m_sentenceStatistic,
                                       m_results[i].m_scorePoint))
                {
                points->AddPoint(Point2D(
                        GraphItemInfo(GetDataset()->GetIdColumn().GetValue(i)).
                        AnchorPoint(m_results[i].m_scorePoint).
                        Brush(GetColorScheme()->GetColor(colorIndex)),
                        Settings::GetPointRadius(),
                        GetShapeScheme()->GetShape(colorIndex)), dc);
                }
            else
                {
                wxFAIL_MSG(
                    wxString::Format(
                        L"Point plotted on backscreen, but failed on main graph!\n"
                         "%.2f, %.2f", m_results[i].m_wordStatistic,
                                       m_results[i].m_sentenceStatistic));
                }
            }
        AddObject(points);

        if (m_results.size() == 1 &&
            m_results.front().IsScoreInvalid())
            {
            wxFont LabelFont = wxSystemSettings::GetFont(wxSYS_DEFAULT_GUI_FONT);
            LabelFont.SetPointSize(12);
            const wxPoint textCoordinate(
                GetPlotAreaBoundingBox().GetX() +
                (GetPlotAreaBoundingBox().GetWidth()/2), GetPlotAreaBoundingBox().GetY() +
                (GetPlotAreaBoundingBox().GetHeight()/2));
            AddObject(std::make_shared<Wisteria::GraphItems::Label>(
                GraphItemInfo(_(L"Invalid score: text is too difficult to be plotted")).
                Scaling(GetScaling()).Pen(*wxBLACK_PEN).
                Font(LabelFont).
                AnchorPoint(textCoordinate)) );
            }
        }
    }
