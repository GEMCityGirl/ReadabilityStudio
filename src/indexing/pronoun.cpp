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
#include "pronoun.h"

const std::set<grammar::is_personal_pronoun::string_type> grammar::is_personal_pronoun::m_words = {
    // English
    L"he", L"her", L"him", L"I", L"it", L"me", L"she", L"them", L"they", L"us", L"we", L"you"
};
