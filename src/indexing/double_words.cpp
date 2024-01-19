///////////////////////////////////////////////////////////////////////////////
// Name:        double_words.cpp
// Author:      Blake Madden
// Copyright:   (c) 2005-2023 Blake Madden
// Licence:     3-Clause BSD licence
// SPDX-License-Identifier: BSD-3-Clause
///////////////////////////////////////////////////////////////////////////////

#include "double_words.h"

using namespace grammar;

std::set<is_double_word_exception::string_type>
    is_double_word_exception::m_double_word_exceptions = {
        L"bison",                    // Bison bison is American bison
        L"Boola",                    // "Boola Boola" is the Yale football song
        L"choo",                     // The train goes "choo choo"
        L"bye",                      // "Say bye bye to grandma."
        L"das",      L"der", L"die", // German articles, which can appear repeated
        L"ha",                       // "ha ha ha," she laughed at Blake's hilarious joke
        L"had",
        L"ho",       // "Ho ho ho," said Santa
        L"Sapiens",  // Homo Sapiens Sapiens is modern humans (subspecies of Homo Sapiens)
        L"Hool",     // from a Chief Joseph speech
        L"Leng",     // common name to appear doubled
        L"log",      // log-log function in math
        L"long",     // long long integers
        L"mephitis", // Mephitis mephitis is the white skunk
        L"no",       // "Using C macros in new code is a no no!"
        L"now",      // "now now, take it easy"
        L"Pago",     // capital of American Samoa
        L"plus",     // C plus plus (it sometimes gets spelled out like this)
        L"sie",      // German word that can be repeated
        L"sigma",    // Sigma Sigma is a fraternity at U. of Cincinnati
        L"Sirhan",   // "Sirhan Sirhan" was RFJ's assassin
        L"ta",       // "ta ta" means "goodbye"
        L"that",
        L"uh",   // variation of "no"
        L"Walla" // Walla Walla, Washington
    };
