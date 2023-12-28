///////////////////////////////////////////////////////////////////////////////
// Name:        chapter_split.cpp
// Author:      Blake Madden
// Copyright:   (c) 2005-2023 Blake Madden
// Licence:     3-Clause BSD licence
// SPDX-License-Identifier: BSD-3-Clause
///////////////////////////////////////////////////////////////////////////////

#include "chapter_split.h"

using namespace lily_of_the_valley;

//------------------------------------------------
bool ChapterSplit::SplitHtmlByBookmarks(wxString& sourceFile, const bool convertToXhtml)
    {
    wxString outputFolder =
        wxFileName(sourceFile).GetPath(wxPATH_GET_VOLUME | wxPATH_GET_SEPARATOR) +
        wxFileName(sourceFile).GetName();
    if (!wxFileName::DirExists(outputFolder))
        {
        if (outputFolder.empty() ||
            !wxFileName::Mkdir(outputFolder, wxS_DIR_DEFAULT, wxPATH_MKDIR_FULL))
            {
            return false;
            }
        }

    wxString FileText;
    Wisteria::TextStream::ReadFile(sourceFile, FileText);

    const wchar_t* htmlText = FileText.wc_str();
    const wchar_t* const htmlTextEnd = htmlText + FileText.length();

    // read the Head section
    wxString headSection{
        L"<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"no\"?>\n"
        "<!DOCTYPE html PUBLIC \"-//W3C//DTD XHTML 1.1//EN\" "
        "\"http://www.w3.org/TR/xhtml11/DTD/xhtml11.dtd\">\n"
        "<html xmlns=\"http://www.w3.org/1999/xhtml\" xml:lang=\"en\">\n<head>\n"
        "<meta content=\"text/html; charset=UTF-8\"/>\n<title></title>\n"
        "<link href=\"stylesheet.css\" type=\"text/css\" rel=\"stylesheet\" />\n"
        "<link rel=\"stylesheet\" type=\"application/vnd.adobe-page-template+xml\" "
        "href=\"page-template.xpgt\"/>\n</head>"
    };
    const wchar_t* headStart =
        html_extract_text::find_element(htmlText, htmlTextEnd, _DT(L"head"), 4);
    if (!convertToXhtml && headStart)
        {
        const wchar_t* headEnd =
            html_extract_text::find_closing_element(headStart, htmlTextEnd, _DT(L"head"), 4);
        if (headEnd)
            {
            headEnd = html_extract_text::find_close_tag(headEnd);
            if (headEnd)
                {
                headSection = wxString(headStart, headEnd + 1);
                }
            }
        headSection.insert(0, L"<html>\n");
        }
    // read the Body declaration
    wxString bodySection{ L"<body>" };
    const wchar_t* bodyStart =
        html_extract_text::find_element(htmlText, htmlTextEnd, _DT(L"body"), 4);
    if (!convertToXhtml && bodyStart)
        {
        const wchar_t* bodyEnd = html_extract_text::find_close_tag(bodyStart);
        if (bodyEnd)
            {
            bodyEnd = html_extract_text::find_close_tag(bodyEnd);
            if (bodyEnd)
                {
                bodySection = wxString(bodyStart, bodyEnd + 1);
                }
            }
        }
    // begin splitting up the file
    auto bookmark = html_extract_text::find_bookmark(htmlText, htmlTextEnd);
    while (bookmark.first)
        {
        auto nextBookMark = html_extract_text::find_bookmark(bookmark.first + 1, htmlTextEnd);
        const wchar_t* endOfFirstBookmark = html_extract_text::find_close_tag(bookmark.first);
        // shouldn't happen
        if (endOfFirstBookmark == nullptr)
            {
            break;
            }
        if (nextBookMark.first)
            {
            std::wstring outputText(++endOfFirstBookmark, nextBookMark.first);
            if (outputText.length())
                {
                outputText.insert(0, wxString::Format(L"%s\n%s\n<div id=\"%s\">", headSection,
                                                      bodySection, bookmark.second.c_str()));
                outputText.append(L"\n</div>\n</body>\n</html>");
                string_util::replace_all(outputText, std::wstring(L"<mbp:pagebreak />"),
                                         std::wstring{});
                lily_of_the_valley::html_format::set_encoding(outputText, L"UTF-8");
                wxFile fout(
                    wxString::Format(L"%s\\%s.xhtml", outputFolder, bookmark.second.c_str()),
                    wxFile::write);
                fout.Write(outputText);
                }
            }
        else
            {
            std::wstring outputText(++endOfFirstBookmark);
            if (outputText.length())
                {
                outputText.insert(0, wxString::Format(L"%s\n%s\n<div id=\"%s\">", headSection,
                                                      bodySection, bookmark.second.c_str()));
                string_util::replace_all(outputText, std::wstring(L"<mbp:pagebreak />"),
                                         std::wstring{});
                const size_t endBodyTag = outputText.find(L"</body>");
                if (endBodyTag != std::wstring::npos)
                    {
                    outputText.insert(endBodyTag, L"\n</div>");
                    }
                lily_of_the_valley::html_format::set_encoding(outputText, L"UTF-8");
                wxFile fout(
                    wxString::Format(L"%s\\%s.xhtml", outputFolder, bookmark.second.c_str()),
                    wxFile::write);
                fout.Write(outputText);
                }
            }
        bookmark = nextBookMark;
        }
    return true;
    }

//------------------------------------------------
void ChapterSplit::WriteChapterFile(const wchar_t* textStart, const wchar_t* textEnd)
    {
    if (!textStart || !textEnd || (textEnd - textStart) == 0)
        {
        return;
        }
    const wxString chapterContent(textStart, (textEnd - textStart));
    const wchar_t *firstLine(textStart), *endOfFirstLine(nullptr);
    while (*firstLine && (firstLine < textEnd) && std::iswspace(firstLine[0]))
        {
        ++firstLine;
        }
    endOfFirstLine = firstLine;
    const grammar::is_end_of_line isEol;
    while (*endOfFirstLine && (endOfFirstLine < textEnd) && !isEol(endOfFirstLine[0]))
        {
        ++endOfFirstLine;
        }
    const wxString chapterName =
        StripIllegalFileCharacters(wxString(firstLine, (endOfFirstLine - firstLine)));
    wxFile ff(m_outputFolder + wxFileName::GetPathSeparator() + chapterName + L".txt",
              wxFile::write);
    if (ff.IsOpened())
        {
#ifdef __WXMSW__
        ff.Write(utf8::bom, sizeof(utf8::bom));
#endif
        ff.Write(chapterContent);
        }
    }

//------------------------------------------------
void ChapterSplit::SplitIntoChapters(const wchar_t* fileText)
    {
    if (!fileText)
        {
        return;
        }
    const wchar_t* nextChapterStart = std::wcschr(fileText, 0x0C);
    while (nextChapterStart)
        {
        WriteChapterFile(fileText, nextChapterStart);
        fileText = nextChapterStart + 1;
        nextChapterStart = std::wcschr(fileText, 0x0C);
        }
    WriteChapterFile(fileText, fileText + std::wcslen(fileText));
    }
