///////////////////////////////////////////////////////////////////////////////
// Name:        schwartzgraph.cpp
// Author:      Blake Madden
// Copyright:   (c) 2005-2023 Blake Madden
// Licence:     3-Clause BSD licence
// SPDX-License-Identifier: BSD-3-Clause
///////////////////////////////////////////////////////////////////////////////

#include "schwartzgraph.h"

using namespace Wisteria;
using namespace Wisteria::Graphs;
using namespace Wisteria::GraphItems;

namespace Wisteria::Graphs
    {
    //----------------------------------------------------------------
    SchwartzGraph::SchwartzGraph(Wisteria::Canvas* canvas,
            std::shared_ptr<Wisteria::Colors::Schemes::ColorScheme> colors /*= nullptr*/,
            std::shared_ptr<Wisteria::Icons::Schemes::IconScheme> shapes /*= nullptr*/) :
        PolygonReadabilityGraph(canvas)
        {
        SetColorScheme(colors != nullptr ? colors :
            Settings::GetDefaultColorScheme());
        SetShapeScheme(shapes != nullptr ? shapes :
            std::make_shared<Wisteria::Icons::Schemes::IconScheme>(
                Wisteria::Icons::Schemes::StandardShapes()));

        GetCanvas()->SetLabel(_(L"Schwartz Graph"));
        GetCanvas()->SetName(_(L"Schwartz Graph"));

        // number of sentences axis goes down, unlike the Fry Graph that goes upwards
        GetLeftYAxis().ReverseScale(true);
        GetLeftYAxis().SetRange(2.4, 20, 1, 0.4f, 1);
        GetLeftYAxis().ShowOuterLabels(true);
        GetLeftYAxis().EnableAutoStacking(true);
        if (GetLeftYAxis().GetGridlinePen().IsOk())
            { GetLeftYAxis().GetGridlinePen().SetColour(wxColour(211, 211, 211, 75)); }

        GetBottomXAxis().SetRange(125, 189, 0, 1, 1);
        // The axis points are displayed with the 100 chopped off of it
        // (so 125 is displayed as 25).
        // This is an "optimization" to fit more labels on the axis.
        // (Not my design, that's how the published graph is designed).
        for (const auto& label : GetBottomXAxis().GetAxisPoints())
            {
            GetBottomXAxis().SetCustomLabel(label.GetValue(),
                GraphItems::Label(wxString::FromDouble(label.GetValue()-100, 0)));
            }
        GetBottomXAxis().ShowOuterLabels(true);
        GetBottomXAxis().GetFooter().SetText(_DT(L"100+", DTExplanation::Constant));
        GetBottomXAxis().GetFooter().SetTextAlignment(TextAlignment::FlushRight);
        GetBottomXAxis().GetFooter().SetRelativeAlignment(Wisteria::RelativeAlignment::FlushBottom);
        GetBottomXAxis().GetFooter().GetPen() = wxNullPen;
        GetBottomXAxis().GetFooter().SetLabelStyle(LabelStyle::RightArrowLinedPaperWithMargins);
        GetBottomXAxis().GetFooter().SetPadding(0, 25, 5, 5);
        if (GetBottomXAxis().GetGridlinePen().IsOk())
            { GetBottomXAxis().GetGridlinePen().SetColour(wxColour(211, 211, 211, 75)); }

        // add the labels for the grades
        AddLevelLabel(LevelLabel(129, 11, _DT(L"1"), 1, 1));
        AddLevelLabel(LevelLabel(135, 5.6, L"2", 2, 2));
        AddLevelLabel(LevelLabel(143, 3.2, L"3/4", 3, 4));
        AddLevelLabel(LevelLabel(156, 2.5, L"5/6", 5, 6));
        AddLevelLabel(LevelLabel(169, 2.5, L"7/8", 7, 7.9)); // really like 7th to lower 8th grade
        AddLevelLabel(LevelLabel(181, 2.5, L"8+", 8, 8));    // advanced 8th grade and beyond
        }

    //----------------------------------------------------------------
    void SchwartzGraph::SetData(std::shared_ptr<const Wisteria::Data::Dataset> data,
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
    void SchwartzGraph::BuildBackscreen()
        {
        if (m_backscreen == nullptr)
            {
            // Shape and color schemes are irrelevant, as this is used
            // only for the base polygon calculations. 
            m_backscreen = std::make_unique<SchwartzGraph>(GetCanvas());
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
    void SchwartzGraph::RecalcSizes(wxDC& dc)
        {
        // axes footers maintain their own font, so adjust this one
        GetBottomXAxis().GetFooter().GetFont() = GetBottomXAxis().GetFont();
        GetBottomXAxis().GetFooter().SetFontColor(GetBottomXAxis().GetFontColor());

        Graph2D::RecalcSizes(dc);

        wxASSERT_MSG((!m_backscreen ||
            m_backscreen->GetBoundingBox(dc).GetWidth() ==
            Canvas::GetDefaultCanvasWidthDIPs()), L"Invalid backscreen size!");
        wxASSERT_MSG((!m_backscreen ||
            m_backscreen->GetBoundingBox(dc).GetHeight() ==
            Canvas::GetDefaultCanvasHeightDIPs()), L"Invalid backscreen size!");

        // long sentence region
        GetPhyscialCoordinates(125, 10.8, m_longSentencesPoints[0]);
        GetPhyscialCoordinates(126.2, 9.6, m_longSentencesPoints[1]);
        GetPhyscialCoordinates(127.2, 8.6, m_longSentencesPoints[2]);
        GetPhyscialCoordinates(128.2, 7.6, m_longSentencesPoints[3]);
        GetPhyscialCoordinates(129.25, 6.8, m_longSentencesPoints[4]);
        GetPhyscialCoordinates(130.3, 6.15, m_longSentencesPoints[5]);
        GetPhyscialCoordinates(131, 5.75, m_longSentencesPoints[6]);
        GetPhyscialCoordinates(132.1, 5.2, m_longSentencesPoints[7]);
        GetPhyscialCoordinates(132.95, 4.8, m_longSentencesPoints[8]);
        GetPhyscialCoordinates(133.9, 4.4, m_longSentencesPoints[9]);
        GetPhyscialCoordinates(135, 4, m_longSentencesPoints[10]);
        GetPhyscialCoordinates(136.85, 3.45, m_longSentencesPoints[11]);
        GetPhyscialCoordinates(137.85, 3.2, m_longSentencesPoints[12]);
        GetPhyscialCoordinates(139.8, 2.7, m_longSentencesPoints[13]);
        GetPhyscialCoordinates(141, 2.4, m_longSentencesPoints[14]);
        GetPhyscialCoordinates(125, 2.4, m_longSentencesPoints[15]);

        auto invalidSentenceArea = std::make_shared<GraphItems::Polygon>(
            GraphItemInfo().Pen(wxNullPen).
            Brush(wxBrush(GetInvalidAreaColor(), wxBRUSHSTYLE_FDIAGONAL_HATCH)).
            Text(_(L"Invalid region: sentences are too long")),
            m_longSentencesPoints);
        invalidSentenceArea->SetBackgroundFill(Colors::GradientFill(*wxWHITE));
        AddObject(invalidSentenceArea);
        
        // long word region
        GetPhyscialCoordinates(160.5, 20.0, m_longWordPoints[0]);
        GetPhyscialCoordinates(161, 19.2, m_longWordPoints[1]);
        GetPhyscialCoordinates(162, 17.6, m_longWordPoints[2]);
        GetPhyscialCoordinates(163.1, 16.45, m_longWordPoints[3]);
        GetPhyscialCoordinates(164, 15.6, m_longWordPoints[4]);
        GetPhyscialCoordinates(165.1, 14.8, m_longWordPoints[5]);
        GetPhyscialCoordinates(166.1, 14.2, m_longWordPoints[6]);
        GetPhyscialCoordinates(167, 13.7, m_longWordPoints[7]);
        GetPhyscialCoordinates(168, 13.3, m_longWordPoints[8]);
        GetPhyscialCoordinates(169, 12.9, m_longWordPoints[9]);
        GetPhyscialCoordinates(170.1, 12.5, m_longWordPoints[10]);
        GetPhyscialCoordinates(171, 12.2, m_longWordPoints[11]);
        GetPhyscialCoordinates(172, 11.9, m_longWordPoints[12]);
        GetPhyscialCoordinates(173, 11.64, m_longWordPoints[13]);
        GetPhyscialCoordinates(174, 11.4, m_longWordPoints[14]);
        GetPhyscialCoordinates(174.9, 11.22, m_longWordPoints[15]);
        GetPhyscialCoordinates(176, 11.0, m_longWordPoints[16]);
        GetPhyscialCoordinates(177, 10.825, m_longWordPoints[17]);
        GetPhyscialCoordinates(178, 10.7, m_longWordPoints[18]);
        GetPhyscialCoordinates(178.8, 10.6, m_longWordPoints[19]);
        GetPhyscialCoordinates(180, 10.45, m_longWordPoints[20]);
        GetPhyscialCoordinates(181, 10.4, m_longWordPoints[21]);
        GetPhyscialCoordinates(182, 10.35, m_longWordPoints[22]);
        GetPhyscialCoordinates(183, 10.32, m_longWordPoints[23]);
        GetPhyscialCoordinates(184, 10.3, m_longWordPoints[24]);
        GetPhyscialCoordinates(185, 10.28, m_longWordPoints[25]);
        GetPhyscialCoordinates(186, 10.26, m_longWordPoints[26]);
        GetPhyscialCoordinates(187, 10.26, m_longWordPoints[27]);
        GetPhyscialCoordinates(188, 10.25, m_longWordPoints[28]);
        GetPhyscialCoordinates(189, 10.24, m_longWordPoints[29]);
        GetPhyscialCoordinates(189, 20.0, m_longWordPoints[30]);

        auto invalidWordArea = std::make_shared<GraphItems::Polygon>(
            GraphItemInfo().Pen(wxNullPen).
            Brush(wxBrush(GetInvalidAreaColor(), wxBRUSHSTYLE_FDIAGONAL_HATCH)).
            Text(_(L"Invalid region: words are too long")),
            m_longWordPoints);
        invalidWordArea->SetBackgroundFill(Colors::GradientFill(*wxWHITE));
        AddObject(invalidWordArea);

        // divider line
        GetPhyscialCoordinates(139, 20, m_dividerLinePoints[0]); // not drawn, used for point region
        GetPhyscialCoordinates(144, 15.6, m_dividerLinePoints[1]);
        GetPhyscialCoordinates(152.5, 10.4, m_dividerLinePoints[2]);
        GetPhyscialCoordinates(158, 7.8, m_dividerLinePoints[3]);
        GetPhyscialCoordinates(161, 6.7, m_dividerLinePoints[4]);
        GetPhyscialCoordinates(166, 5.6, m_dividerLinePoints[5]);
        GetPhyscialCoordinates(173, 5.2, m_dividerLinePoints[6]);
        // the following are not drawn, just used for calculating information
        GetPhyscialCoordinates(189, 5.2, m_dividerLinePoints[7]);
        GetPhyscialCoordinates(189, 20, m_dividerLinePoints[8]);

        // light grey on selection
        const wxBrush selectionBrush = wxBrush(wxColour(192, 192, 192, 100));

        // grade lines
        // 1st grade
        GetPhyscialCoordinates(125, 10.8, m_gradeLinePoints[0]);
        GetPhyscialCoordinates(125, 20.0, m_gradeLinePoints[1]);
        GetPhyscialCoordinates(160.5, 20.0, m_gradeLinePoints[2]);
        GetPhyscialCoordinates(162.45, 17.1, m_gradeLinePoints[3]);
        GetPhyscialCoordinates(128.2, 7.6, m_gradeLinePoints[4]);
        AddObject(std::make_shared<GraphItems::Polygon>(
            GraphItemInfo().Pen(wxPen(wxColour(0, 0, 255, 200))).
            Brush(*wxBLACK_BRUSH).Scaling(GetScaling()),
            &m_gradeLinePoints[3], 2));
        // 2th grade
        GetPhyscialCoordinates(168.5, 13.1, m_gradeLinePoints[5]);
        GetPhyscialCoordinates(135, 4, m_gradeLinePoints[6]);
        AddObject(std::make_shared<GraphItems::Polygon>(
            GraphItemInfo().Pen(wxPen(wxColour(0, 0, 255, 200))).
            Brush(*wxBLACK_BRUSH).Scaling(GetScaling()),
            &m_gradeLinePoints[5], 2));
        // 3rd-4th grade
        GetPhyscialCoordinates(148, 2.4, m_gradeLinePoints[7]);
        GetPhyscialCoordinates(170.1, 12.5, m_gradeLinePoints[8]);
        AddObject(std::make_shared<GraphItems::Polygon>(
            GraphItemInfo().Pen(wxPen(wxColour(0, 0, 255, 200))).
            Brush(*wxBLACK_BRUSH).Scaling(GetScaling()),
            &m_gradeLinePoints[7], 2));
        // 5th-6th grade
        GetPhyscialCoordinates(172, 11.9, m_gradeLinePoints[9]);
        GetPhyscialCoordinates(165, 2.4, m_gradeLinePoints[10]);
        AddObject(std::make_shared<GraphItems::Polygon>(
            GraphItemInfo().Pen(wxPen(wxColour(0, 0, 255, 200))).
            Brush(*wxBLACK_BRUSH).Scaling(GetScaling()),
            &m_gradeLinePoints[9], 2));
        // 7th-8th grade
        GetPhyscialCoordinates(177.5, 2.4, m_gradeLinePoints[11]);
        GetPhyscialCoordinates(180, 10.45, m_gradeLinePoints[12]);
        AddObject(std::make_shared<GraphItems::Polygon>(
            GraphItemInfo().Pen(wxPen(wxColour(0, 0, 255, 200))).
            Brush(*wxBLACK_BRUSH).Scaling(GetScaling()),
            &m_gradeLinePoints[11], 2));
        // 8th grade+
        GetPhyscialCoordinates(189, 10.24, m_gradeLinePoints[13]);
        GetPhyscialCoordinates(189, 2.4, m_gradeLinePoints[14]);
        AddObject(std::make_shared<GraphItems::Polygon>(
            GraphItemInfo().Pen(wxPen(wxColour(0, 0, 255, 200))).
            Brush(*wxBLACK_BRUSH).Scaling(GetScaling()),
            &m_gradeLinePoints[13], 2));

        wxASSERT_MSG(GetMessageCatalog(), L"Label manager not set in Schwartz Graph!");

        // 1st grade polygon
        GetPhyscialCoordinates(125, 20.0, m_grade1Polygon[0]);
        GetPhyscialCoordinates(160.5, 20.0, m_grade1Polygon[1]);
        GetPhyscialCoordinates(161, 19.2, m_grade1Polygon[2]);
        GetPhyscialCoordinates(162, 17.6, m_grade1Polygon[3]);
        GetPhyscialCoordinates(162.45, 17.1, m_grade1Polygon[4]);
        GetPhyscialCoordinates(128.2, 7.6, m_grade1Polygon[5]);
        GetPhyscialCoordinates(126.2, 9.6, m_grade1Polygon[6]);
        GetPhyscialCoordinates(127.2, 8.6, m_grade1Polygon[7]);
        GetPhyscialCoordinates(125, 10.8, m_grade1Polygon[8]);
        AddObject(std::make_shared<GraphItems::Polygon>(
            GraphItemInfo(GetMessageCatalog()->GetGradeScaleLongLabel(1)).
            Pen(wxNullPen).Brush(wxNullBrush).SelectionBrush(selectionBrush),
            m_grade1Polygon));
        // 2nd grade polygon
        GetPhyscialCoordinates(162.45, 17.1, m_grade2Polygon[0]);
        GetPhyscialCoordinates(163.1, 16.45, m_grade2Polygon[1]);
        GetPhyscialCoordinates(164, 15.6, m_grade2Polygon[2]);
        GetPhyscialCoordinates(165.1, 14.8, m_grade2Polygon[3]);
        GetPhyscialCoordinates(166.1, 14.2, m_grade2Polygon[4]);
        GetPhyscialCoordinates(167, 13.7, m_grade2Polygon[5]);
        GetPhyscialCoordinates(168, 13.3, m_grade2Polygon[6]);
        GetPhyscialCoordinates(168.5, 13.1, m_grade2Polygon[7]);
        GetPhyscialCoordinates(135, 4, m_grade2Polygon[8]);
        GetPhyscialCoordinates(133.9, 4.4, m_grade2Polygon[9]);
        GetPhyscialCoordinates(132.95, 4.8, m_grade2Polygon[10]);
        GetPhyscialCoordinates(132.1, 5.2, m_grade2Polygon[11]);
        GetPhyscialCoordinates(131, 5.75, m_grade2Polygon[12]);
        GetPhyscialCoordinates(130.3, 6.15, m_grade2Polygon[13]);
        GetPhyscialCoordinates(129.25, 6.8, m_grade2Polygon[14]);
        GetPhyscialCoordinates(128.2, 7.6, m_grade2Polygon[15]);
        AddObject(std::make_shared<GraphItems::Polygon>(
            GraphItemInfo(GetMessageCatalog()->GetGradeScaleLongLabel(2)).
            Pen(wxNullPen).Brush(wxNullBrush).SelectionBrush(selectionBrush),
            m_grade2Polygon));
        // 3rd-4th grade polygon
        GetPhyscialCoordinates(168.5, 13.1, m_grade3to4Polygon[0]);
        GetPhyscialCoordinates(169, 12.9, m_grade3to4Polygon[1]);
        GetPhyscialCoordinates(170.1, 12.5, m_grade3to4Polygon[2]);
        GetPhyscialCoordinates(148, 2.4, m_grade3to4Polygon[3]);
        GetPhyscialCoordinates(141, 2.4, m_grade3to4Polygon[4]);
        GetPhyscialCoordinates(139.8, 2.7, m_grade3to4Polygon[5]);
        GetPhyscialCoordinates(137.85, 3.2, m_grade3to4Polygon[6]);
        GetPhyscialCoordinates(136.85, 3.45, m_grade3to4Polygon[7]);
        GetPhyscialCoordinates(135, 4, m_grade3to4Polygon[8]);
        AddObject(std::make_shared<GraphItems::Polygon>(
            GraphItemInfo(wxString::Format(L"%s-%s",
                GetMessageCatalog()->GetGradeScaleLongLabel(3),
                GetMessageCatalog()->GetGradeScaleLongLabel(4))).
            Pen(wxNullPen).Brush(wxNullBrush).SelectionBrush(selectionBrush),
            m_grade3to4Polygon));
        // 5th-6th grade polygon
        GetPhyscialCoordinates(170.1, 12.5, m_grade5to6Polygon[0]);
        GetPhyscialCoordinates(171, 12.2, m_grade5to6Polygon[1]);
        GetPhyscialCoordinates(172, 11.9, m_grade5to6Polygon[2]);
        GetPhyscialCoordinates(165, 2.4, m_grade5to6Polygon[3]);
        GetPhyscialCoordinates(148, 2.4, m_grade5to6Polygon[4]);
        AddObject(std::make_shared<GraphItems::Polygon>(
            GraphItemInfo(wxString::Format(L"%s-%s",
                GetMessageCatalog()->GetGradeScaleLongLabel(5),
                GetMessageCatalog()->GetGradeScaleLongLabel(6))).
            Pen(wxNullPen).Brush(wxNullBrush).SelectionBrush(selectionBrush),
            m_grade5to6Polygon));
        // 7th-8th grade polygon
        GetPhyscialCoordinates(172, 11.9, m_grade7to8Polygon[0]);
        GetPhyscialCoordinates(173, 11.64, m_grade7to8Polygon[1]);
        GetPhyscialCoordinates(174, 11.4, m_grade7to8Polygon[2]);
        GetPhyscialCoordinates(174.9, 11.22, m_grade7to8Polygon[3]);
        GetPhyscialCoordinates(176, 11.0, m_grade7to8Polygon[4]);
        GetPhyscialCoordinates(177, 10.825, m_grade7to8Polygon[5]);
        GetPhyscialCoordinates(178, 10.7, m_grade7to8Polygon[6]);
        GetPhyscialCoordinates(178.8, 10.6, m_grade7to8Polygon[7]);
        GetPhyscialCoordinates(180, 10.5, m_grade7to8Polygon[8]);
        GetPhyscialCoordinates(177.5, 2.4, m_grade7to8Polygon[9]);
        GetPhyscialCoordinates(165, 2.4, m_grade7to8Polygon[10]);
        AddObject(std::make_shared<GraphItems::Polygon>(
            GraphItemInfo(wxString::Format(L"%s-%s",
                GetMessageCatalog()->GetGradeScaleLongLabel(7),
                GetMessageCatalog()->GetGradeScaleLongLabel(8))).
            Pen(wxNullPen).Brush(wxNullBrush).SelectionBrush(selectionBrush),
            m_grade7to8Polygon));
        // 8+ polygon
        GetPhyscialCoordinates(180, 10.5, m_gradeOver8Polygon[0]);
        GetPhyscialCoordinates(181, 10.4, m_gradeOver8Polygon[1]);
        GetPhyscialCoordinates(182, 10.35, m_gradeOver8Polygon[2]);
        GetPhyscialCoordinates(183, 10.32, m_gradeOver8Polygon[3]);
        GetPhyscialCoordinates(184, 10.3, m_gradeOver8Polygon[4]);
        GetPhyscialCoordinates(185, 10.28, m_gradeOver8Polygon[5]);
        GetPhyscialCoordinates(186, 10.26, m_gradeOver8Polygon[6]);
        GetPhyscialCoordinates(187, 10.26, m_gradeOver8Polygon[7]);
        GetPhyscialCoordinates(188, 10.25, m_gradeOver8Polygon[8]);
        GetPhyscialCoordinates(189, 10.24, m_gradeOver8Polygon[9]);
        GetPhyscialCoordinates(189, 2.4, m_gradeOver8Polygon[10]);
        GetPhyscialCoordinates(177.5, 2.4, m_gradeOver8Polygon[11]);
        AddObject(std::make_shared<GraphItems::Polygon>(
            GraphItemInfo(GetMessageCatalog()->GetGradeScaleLongLabel(8) + _DT(L"+")).
            Pen(wxNullPen).Brush(wxNullBrush).SelectionBrush(selectionBrush),
            m_gradeOver8Polygon));

        // middle points on the separator line
        const wxColour lightGray{ wxColour(0, 0, 0, 200) };
        auto linePoints = std::make_shared<GraphItems::Points2D>(wxNullPen);
        linePoints->SetScaling(GetScaling());
        linePoints->SetDPIScaleFactor(GetDPIScaleFactor());
        linePoints->AddPoint(Point2D(
            GraphItemInfo().AnchorPoint(m_dividerLinePoints[1]).
            Brush(lightGray),
            Settings::GetPointRadius(), Icons::IconShape::Circle), dc);
        linePoints->AddPoint(Point2D(
            GraphItemInfo().AnchorPoint(m_dividerLinePoints[2]).
            Brush(lightGray),
            Settings::GetPointRadius(), Icons::IconShape::Circle), dc);
        linePoints->AddPoint(Point2D(
            GraphItemInfo().AnchorPoint(m_dividerLinePoints[3]).
            Brush(lightGray),
            Settings::GetPointRadius(), Icons::IconShape::Circle), dc);
        linePoints->AddPoint(Point2D(
            GraphItemInfo().AnchorPoint(m_dividerLinePoints[4]).
            Brush(lightGray),
            Settings::GetPointRadius(), Icons::IconShape::Circle), dc);
        // index [5] is just added to the spline to curve it better,
        // not used for a middle point
        linePoints->AddPoint(Point2D(
            GraphItemInfo().AnchorPoint(m_dividerLinePoints[6]).
            Brush(lightGray), Settings::GetPointRadius(), Icons::IconShape::Circle), dc);
        AddObject(linePoints);

        // separator line
        auto levelsSpline = std::make_shared<GraphItems::Polygon>(
            GraphItemInfo().Pen(wxPen(wxColour(0, 0, 0, 200))).
            Brush(wxBrush(wxColour(0, 0, 0, 200))).Scaling(GetScaling()),
            &m_dividerLinePoints[1], m_dividerLinePoints.size() - 3);
        levelsSpline->SetShape(GraphItems::Polygon::PolygonShape::Spline);
        AddObject(levelsSpline);

        // inner lines of polygons
        // (can't draw outline around full polygon because it overlaps the axis)
        auto polysepLineWords =
            std::make_shared<GraphItems::Lines>(wxPen(L"#ECECEC", 1), GetScaling());
        for (size_t i = 0; i < m_longWordPoints.size() - 2; ++i)
            { polysepLineWords->AddLine(m_longWordPoints[i], m_longWordPoints[i+1]); }
        AddObject(polysepLineWords);

        auto polysepLineWordsAxis = std::make_shared<GraphItems::Lines>(
            GetLeftYAxis().GetPen(), GetScaling());
        polysepLineWordsAxis->AddLine(m_longWordPoints[m_longWordPoints.size() - 2],
            m_longWordPoints[m_longWordPoints.size() - 1]);
        polysepLineWordsAxis->AddLine(m_longWordPoints[m_longWordPoints.size() - 1],
            m_longWordPoints[0]);
        AddObject(polysepLineWordsAxis);

        auto polysepLineSentences =
            std::make_shared<GraphItems::Lines>(wxPen(L"#ECECEC"), GetScaling());
        for (size_t i = 0; i < m_longSentencesPoints.size() - 2; ++i)
            {
            polysepLineSentences->AddLine(m_longSentencesPoints[i],
                                          m_longSentencesPoints[i + 1]);
            }
        AddObject(polysepLineSentences);

        auto polysepLineSentencesAxis = std::make_shared<GraphItems::Lines>(
            GetTopXAxis().GetPen(), GetScaling());
        polysepLineSentencesAxis->AddLine(m_longSentencesPoints[m_longSentencesPoints.size() - 2],
                                          m_longSentencesPoints[m_longSentencesPoints.size() - 1]);
        polysepLineSentencesAxis->AddLine(m_longSentencesPoints[m_longSentencesPoints.size() - 1],
                                          m_longSentencesPoints[0]);
        AddObject(polysepLineSentencesAxis);

        // the explanatory legends (embedded on the graph)
        const wxFont legendFont(
            wxSystemSettings::GetFont(wxSYS_DEFAULT_GUI_FONT).GetPointSize(), 
            wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD,
            false, GetFancyFontFaceName());
        // main label on graph
        wxPoint textCoordinate;
        GetPhyscialCoordinates(126, 19.6, textCoordinate);
        auto mainLabel = std::make_shared<GraphItems::Label>(
            GraphItemInfo(_(L"German\nReadability Graph")).
            Pen(*wxBLACK_PEN).FontBackgroundColor(*wxWHITE).
            Scaling(GetScaling()).
            Font(wxFont(wxSystemSettings::GetFont(
                wxSYS_DEFAULT_GUI_FONT).GetPointSize()*1.5,
                wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD,
                false, GetFancyFontFaceName())).
            LabelAlignment(TextAlignment::Centered).
            Anchoring(Anchoring::BottomLeftCorner).Padding(4, 4, 4, 4).
            AnchorPoint(textCoordinate));
        mainLabel->SetBoxCorners(BoxCorners::Straight);
        AddObject(mainLabel);

            {
            wxGCDC measureDC;
            auto legend = std::make_shared<GraphItems::Label>(
                GraphItemInfo(_(L"Average line\nAverage, each zone")).
                Scaling(GetScaling()).Pen(*wxBLACK_PEN).FontBackgroundColor(*wxWHITE).
                Font(legendFont).LabelAlignment(TextAlignment::FlushLeft).
                Anchoring(Anchoring::BottomLeftCorner).
                Padding(4,4,4,GraphItems::Label::GetMinLegendWidthDIPs()).
                AnchorPoint(
                    mainLabel->GetBoundingBox(measureDC).GetTopLeft() -
                        wxPoint(0,ScaleToScreenAndCanvas(2))));
            legend->GetLegendIcons().push_back(
                Icons::LegendIcon(Icons::IconShape::HorizontalLine,
                                  wxColour(0, 0, 0, 200), wxColour(0, 0, 0, 200)));
            legend->GetLegendIcons().push_back(
                Icons::LegendIcon(Icons::IconShape::Circle,
                                  wxColour(0, 0, 0, 200), wxColour(0, 0, 0, 200)));
            legend->SetBoxCorners(BoxCorners::Straight);
            AddObject(legend);
            }

        CalculateScorePositions(dc);

        // Add the grade labels to the regions
        // (and highlight in red the one where the score lies)
        const wxFont numberFont(wxSystemSettings::GetFont(
            wxSYS_DEFAULT_GUI_FONT).GetPointSize() * 3,
            wxFONTFAMILY_DEFAULT, wxFONTSTYLE_ITALIC, wxFONTWEIGHT_BOLD,
            false, GetFancyFontFaceName());
        for (const auto& level : GetLevelLabels())
            {
            wxPoint pt;
            GetPhyscialCoordinates(level.GetX(), level.GetY(), pt);

            auto gradeLabel = std::make_shared<GraphItems::Label>(
                GraphItemInfo(level.GetLabel()).Scaling(GetScaling()).
                Pen(wxNullPen).Font(numberFont).AnchorPoint(pt));
            gradeLabel->SetAnchoring(Anchoring::TopLeftCorner);
            gradeLabel->SetFontColor(
                (GetScores().size() == 1 &&
                 level == std::floor(GetScores().front().GetScoreAverage()) ) ?
                    *wxRED : *wxBLACK);
            AddObject(gradeLabel);
            }
        }

    //----------------------------------------------------------------
    void SchwartzGraph::CalculateScorePositions(wxDC& dc)
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
                        125, 189),
                    std::clamp<double>(
                        normalizationFactor * m_numberOfSentencesColumn->GetValue(i),
                        2.4, 20));

            m_results[i].ResetStatus();

            const auto calcScoreFromPolygons = [](
                const std::unique_ptr<SchwartzGraph>& graph,
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

                if (IsScoreInsideRegion(scorePoint.m_scorePoint, graph->m_gradeOver8Polygon, 1, -1) )
                    { scorePoint.SetScore(8); }
                else if (IsScoreInsideRegion(scorePoint.m_scorePoint, graph->m_grade7to8Polygon, 1, -1) )
                    { scorePoint.SetScoreRange(7,8); }
                else if (IsScoreInsideRegion(scorePoint.m_scorePoint, graph->m_grade5to6Polygon, 1, -1) )
                    { scorePoint.SetScoreRange(5,6); }
                else if (IsScoreInsideRegion(scorePoint.m_scorePoint, graph->m_grade3to4Polygon, 1, -1) )
                    { scorePoint.SetScoreRange(3,4); }
                else if (IsScoreInsideRegion(scorePoint.m_scorePoint, graph->m_grade2Polygon, 1, -1) )
                    { scorePoint.SetScore(2); }
                else if (IsScoreInsideRegion(scorePoint.m_scorePoint, graph->m_grade1Polygon, 1, -1) )
                    { scorePoint.SetScore(1); }
                else
                    { scorePoint.SetScoreOutOfGradeRange(true); }

                // if in a valid grade area see if it leans towards having harder sentences or words
                if (!scorePoint.IsScoreInvalid())
                    {
                    if (IsScoreInsideRegion(scorePoint.m_scorePoint,
                        graph->m_dividerLinePoints, 1, -1))
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
