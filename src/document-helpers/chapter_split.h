/********************************************************************************
 * Copyright (c) 2005-2025 Blake Madden
 *
 * This program and the accompanying materials are made available under the
 * terms of the Eclipse Public License 2.0 which is available at
 * https://www.eclipse.org/legal/epl-2.0.
 *
 * SPDX-License-Identifier: EPL-2.0
 *
 * Contributors:
 *   Blake Madden - initial implementation
 ********************************************************************************/

#ifndef __CHAPTER_SPLIT_H__
#define __CHAPTER_SPLIT_H__

#include "../Wisteria-Dataviz/src/import/html_encode.h"
#include "../Wisteria-Dataviz/src/import/html_extract_text.h"
#include "../Wisteria-Dataviz/src/utfcpp/source/utf8.h"
#include "../Wisteria-Dataviz/src/util/donttranslate.h"
#include "../Wisteria-Dataviz/src/util/fileutil.h"
#include "../Wisteria-Dataviz/src/util/string_util.h"
#include "../Wisteria-Dataviz/src/util/textstream.h"
#include "../indexing/sentence.h"
#include "../webharvester/filepathresolver.h"
#include <wx/file.h>
#include <wx/wx.h>

class ChapterSplit
    {
  public:
    void SetOutputFolder(const wxString& outFolder) { m_outputFolder = outFolder; }

    void SplitIntoChapters(const wchar_t* fileText);
    static bool SplitHtmlByBookmarks(wxString& sourceFile, const bool convertToXhtml);

  private:
    void WriteChapterFile(const wchar_t* textStart, const wchar_t* textEnd);
    wxString m_outputFolder;
    };

#endif //__CHAPTER_SPLIT_H__
