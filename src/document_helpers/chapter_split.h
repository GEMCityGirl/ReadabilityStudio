/** @addtogroup Helpers
    @brief Helper classes for tests and documents.
    @date 2005-2023
    @copyright Oleander Software, Ltd.
    @author Blake Madden
    @details This program is free software; you can redistribute it and/or modify
     it under the terms of the 3-Clause BSD License.

     SPDX-License-Identifier: BSD-3-Clause
* @{*/

#ifndef __CHAPTER_SPLIT_H__
#define __CHAPTER_SPLIT_H__

#include "../Wisteria-Dataviz/src/i18n-check/src/donttranslate.h"
#include "../Wisteria-Dataviz/src/i18n-check/src/string_util.h"
#include "../Wisteria-Dataviz/src/import/html_encode.h"
#include "../Wisteria-Dataviz/src/import/html_extract_text.h"
#include "../Wisteria-Dataviz/src/utfcpp/source/utf8.h"
#include "../Wisteria-Dataviz/src/util/fileutil.h"
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

    /** @}*/

#endif //__CHAPTER_SPLIT_H__
