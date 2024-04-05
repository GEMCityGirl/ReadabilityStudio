///////////////////////////////////////////////////////////////////////////////
// Name:        pronoun.cpp
// Author:      Blake Madden
// Copyright:   (c) 2005-2023 Blake Madden
// Licence:     3-Clause BSD licence
// SPDX-License-Identifier: BSD-3-Clause
///////////////////////////////////////////////////////////////////////////////

#include "pronoun.h"

const std::set<grammar::is_personal_pronoun::string_type> grammar::is_personal_pronoun::m_words = {
    // English
    L"he", L"her", L"him", L"I", L"it", L"me", L"she", L"them", L"they", L"us", L"we", L"you"
};
