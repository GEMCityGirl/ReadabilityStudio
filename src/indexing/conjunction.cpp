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
