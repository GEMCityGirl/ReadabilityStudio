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

#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>
#include "../src/indexing/abbreviation.h"

// clang-format off
// NOLINTBEGIN
TEST_CASE("Abbreviation casing", "[abbreviations]")
    {
    grammar::is_abbreviation isAbbrev;

    CHECK(isAbbrev(L"BRO."));
    CHECK(isAbbrev(L"bro."));
    }

TEST_CASE("Abbreviation time", "[abbreviations]")
    {
    grammar::is_abbreviation isAbbrev;

    CHECK(isAbbrev(L"5:07P.M."));
    CHECK(isAbbrev(L"02:10a.m."));
    CHECK(isAbbrev(L"2a.m."));
    CHECK(isAbbrev(L"12P.m."));
    CHECK_FALSE(isAbbrev(L"0P:00a.m."));
    CHECK_FALSE(isAbbrev(L"P:00a.m."));
    CHECK_FALSE(isAbbrev(L"5:00d.m."));
    CHECK_FALSE(isAbbrev(L"500p.m."));
    }
// NOLINTEND
// clang-format on
