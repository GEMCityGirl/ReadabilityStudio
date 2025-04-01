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

#ifndef __READABILITY_FORMULA_PARSER_H__
#define __READABILITY_FORMULA_PARSER_H__

#include "../tinyexpr-plusplus/tinyexpr.h"
#include <limits>
#include <vector>
#include <wx/wx.h>

/// @private
class BaseProject;

/// @brief Connects a formula parser and a project.
class FormulaProject : public te_expr
    {
  public:
    /// @brief Constructor.
    /// @param project The project to connect the formula parser to.
    explicit FormulaProject(const BaseProject* project) noexcept : m_project(project) {}

    /// @returns The target project.
    [[nodiscard]]
    const BaseProject* GetProject() const noexcept
        {
        return m_project;
        }

  private:
    const BaseProject* m_project{ nullptr };
    };

/// @brief Readability formula parser.
class ReadabilityFormulaParser : public te_parser
    {
  public:
    ReadabilityFormulaParser(const BaseProject* project, const wchar_t decimalSeparator,
                             const wchar_t listSeparator);
    /// @private
    ReadabilityFormulaParser() = delete;
    /// @private
    ReadabilityFormulaParser(const ReadabilityFormulaParser& that) = delete;
    /// @private
    ReadabilityFormulaParser& operator=(const ReadabilityFormulaParser& that) = delete;
    /// @brief Refreshes the project's statistics that are mapped to variables
    ///     in the formula parser.
    void UpdateVariables();

    /// @brief Parses a signature to find the function name in it.
    /// @param signature The signature to parse.
    /// @returns The name of the function from the signature.
    [[nodiscard]]
    static const wxString SignatureToFunctionName(const wxString& signature)
        {
        const auto paren = signature.find(L'(');
        if (paren == wxString::npos)
            {
            return signature;
            }
        else
            {
            return signature.substr(0, paren);
            }
        }

    /// @returns The signature of the custom DC test.
    [[nodiscard]]
    static wxString GetCustomNewDaleChallSignature()
        {
        return L"CustomNewDaleChall()";
        }

    /// @returns The signature of the custom Spache test.
    [[nodiscard]]
    static wxString GetCustomSpacheSignature()
        {
        return L"CustomSpache()";
        }

    /// @returns The signature of the custom HJ test.
    [[nodiscard]]
    static wxString GetCustomHarrisJacobsonSignature()
        {
        return L"CustomHarrisJacobson()";
        }

  private:
    FormulaProject m_formualProject;
    };

#endif // __READABILITY_FORMULA_PARSER_H__
