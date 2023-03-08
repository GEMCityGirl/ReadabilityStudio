#ifndef __CHAPTER_SPLIT_H__
#define __CHAPTER_SPLIT_H__

#include <wx/wx.h>
#include <wx/file.h>
#include "../../../../SRC/Wisteria-Dataviz/src/util/stringutil.h"
#include "../../../../SRC/Wisteria-Dataviz/src/util/fileutil.h"
#include "../../../../SRC/Wisteria-Dataviz/src/util/donttranslate.h"
#include "../../../../SRC/Wisteria-Dataviz/src/util/textstream.h"
#include "../../../../SRC/Wisteria-Dataviz/src/import/html_extract_text.h"
#include "../../../../SRC/Wisteria-Dataviz/src/import/html_encode.h"
#include "../../../../SRC/Wisteria-Dataviz/src/utfcpp/source/utf8.h"
#include "../../../../SRC/wxTools/wxFileFolderOperations.h"
#include "../indexing/sentence.h"

class ChapterSplit
    {
public:
    void SetOutputFolder(const wxString& outFolder)
        { m_outputFolder = outFolder; }
    void SplitIntoChapters(const wchar_t* fileText);
    static bool SplitHtmlByBookmarks(wxString& sourceFile, const bool convertToXhtml);
private:
    void WriteChapterFile(const wchar_t* textStart, const wchar_t* textEnd);
    wxString m_outputFolder;
    };

#endif //__CHAPTER_SPLIT_H__

