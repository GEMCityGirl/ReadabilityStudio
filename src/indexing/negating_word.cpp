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

#include "negating_word.h"

const std::set<grammar::is_negating::string_type> grammar::is_negating::m_words = {
    L"ain't", L"aren't", L"can't", L"cannot", L"couldn't", L"didn't", L"doesn't", L"don't",
    L"hadn't", L"hasn't", L"haven't", L"isn't", L"mustn't", L"never", L"no", L"non", L"not",
    L"nowhere", L"shan't", L"wasn't", L"weren't", L"won't", L"wouldn't",
    // for writers with an aversion to proper punctuation *eye-roll*
    L"aint", L"arent", L"cant", L"couldnt", L"didnt", L"doesnt", L"dont", L"hadnt", L"hasnt",
    L"havent", L"isnt", L"mustnt", L"shant", L"wasnt", L"werent", L"wont", L"wouldnt"
};
