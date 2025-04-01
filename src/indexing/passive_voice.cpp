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

#include "passive_voice.h"

using namespace grammar;

std::set<is_english_passive_voice::string_type> is_english_passive_voice::m_to_be_verbs = {
    L"am", L"are",   L"aren't", L"be",     L"been", L"being",
    L"is", L"isn't", L"was",    L"wasn't", L"were", L"weren't"
};

word_list is_english_passive_voice::m_past_participle_exeptions;
