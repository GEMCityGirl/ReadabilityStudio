#ifndef __READABILITY_FORMULA_PARSER_H__
#define __READABILITY_FORMULA_PARSER_H__

#include <wx/wx.h>
#include <vector>
#include <limits>
#include "../../../../SRC/tinyexpr-plusplus/tinyexpr.h"

class BaseProject;

class FormulaProject : public te_expr
    {
public:
    explicit FormulaProject(const BaseProject* project) noexcept : m_project(project)
        {}
    [[nodiscard]] const BaseProject* GetProject() const noexcept
        { return m_project; }
private:
    const BaseProject* m_project{ nullptr };
    };

/// Readability formula parser.
class ReadabilityFormulaParser : public te_parser
    {
    wxDECLARE_NO_COPY_CLASS(ReadabilityFormulaParser);
public:
    ReadabilityFormulaParser(const BaseProject* project, const wchar_t decimalSeparator, const wchar_t listSeparator);
    ReadabilityFormulaParser() = delete;
    void UpdateVariables();
    static const wxString SignatureToFunctionName(const wxString& signature)
        {
        const int paren = signature.Find(wxT('('));
        if (paren == wxNOT_FOUND)
            { return signature; }
        else
            { return signature.Mid(0, paren); }
        }
    static wxString GetCustomNewDaleChallSignature()
        { return L"CustomNewDaleChall()"; }
    static wxString GetCustomSpacheSignature()
        { return L"CustomSpache()"; }
    static wxString GetCustomHarrisJacobsonSignature()
        { return L"CustomHarrisJacobson()"; }
private:
    FormulaProject m_formualProject;
    };

#endif // __READABILITY_FORMULA_PARSER_H__
