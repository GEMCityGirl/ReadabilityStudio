///////////////////////////////////////////////////////////////////////////////
// Name:        conjunction.cpp
// Author:      Blake Madden
// Copyright:   (c) 2005-2023 Blake Madden
// Licence:     3-Clause BSD licence
// SPDX-License-Identifier: BSD-3-Clause
///////////////////////////////////////////////////////////////////////////////

#include "conjunction.h"

const std::set<grammar::is_coordinating_conjunction::string_type>
    grammar::is_english_coordinating_conjunction::m_conjunctions = { L"&",  L"and", L"but", L"nor",
                                                                     L"or", L"so",  L"yet" };

const std::set<grammar::is_coordinating_conjunction::string_type>
    grammar::is_spanish_coordinating_conjunction::m_conjunctions = {
        L"&",
        L"e",    // and
        L"ni",   // nor
        L"o",    // or
        L"pero", // but
        L"sino", // but
        L"u",    // or
        L"y"     // and
    };

const std::set<grammar::is_coordinating_conjunction::string_type>
    grammar::is_german_coordinating_conjunction::m_conjunctions = {
        L"&",
        L"und",     // and
        L"oder",    // or
        L"denn",    // for, because
        L"aber",    // but
        L"sondern", // but (instead)
    };
