///////////////////////////////////////////////////////////////////////////////
// Name:        negating_word.cpp
// Author:      Blake Madden
// Copyright:   (c) 2005-2023 Blake Madden
// Licence:     3-Clause BSD licence
// SPDX-License-Identifier: BSD-3-Clause
///////////////////////////////////////////////////////////////////////////////

#include "negating_word.h"

using namespace grammar;

std::set<is_negating::string_type> is_negating::m_words = {
    L"ain't", L"aren't", L"can't", L"cannot", L"couldn't", L"didn't", L"doesn't", L"don't",
    L"hadn't", L"hasn't", L"haven't", L"isn't", L"mustn't", L"never", L"no", L"non", L"not",
    L"nowhere", L"shan't", L"wasn't", L"weren't", L"won't", L"wouldn't",
    // for writers with an aversion to proper punctuation *eye-roll*
    L"aint", L"arent", L"cant", L"couldnt", L"didnt", L"doesnt", L"dont", L"hadnt", L"hasnt",
    L"havent", L"isnt", L"mustnt", L"shant", L"wasnt", L"werent", L"wont", L"wouldnt"
};
