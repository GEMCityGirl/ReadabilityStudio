///////////////////////////////////////////////////////////////////////////////
// Name:        passive_voice.cpp
// Author:      Blake Madden
// Copyright:   (c) 2005-2023 Blake Madden
// Licence:     3-Clause BSD licence
// SPDX-License-Identifier: BSD-3-Clause
///////////////////////////////////////////////////////////////////////////////

#include "passive_voice.h"

using namespace grammar;

std::set<is_english_passive_voice::string_type> is_english_passive_voice::m_to_be_verbs =
    { L"am", L"are", L"aren't",
      L"be", L"been", L"being",
      L"is", L"isn't", L"was",
      L"wasn't", L"were", L"weren't" };

word_list is_english_passive_voice::m_past_participle_exeptions;
