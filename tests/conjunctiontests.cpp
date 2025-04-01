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
#include "../src/indexing/conjunction.h"

// clang-format off
// NOLINTBEGIN

using namespace grammar;
using namespace characters;

TEST_CASE("English conjuctions", "[conjuctions]")
    {
    SECTION("Nulls")
        {
        grammar::is_english_coordinating_conjunction iecc;
        CHECK_FALSE(iecc(L""));
        grammar::is_german_coordinating_conjunction igcc;
        CHECK_FALSE(igcc(L""));
        grammar::is_spanish_coordinating_conjunction iscc;
        CHECK_FALSE(iscc(L""));
        }
    SECTION("English")
        {
        grammar::is_english_coordinating_conjunction iecc;
        CHECK(iecc(L"anD"));
        CHECK(iecc(L"bUt"));
        CHECK(iecc(L"Nor"));
        CHECK(iecc(L"yeT"));
        CHECK(iecc(L"Or"));
        CHECK(iecc(L"sO"));
        CHECK_FALSE(iecc(L"ye"));
        CHECK_FALSE(iecc(L"yes"));
        CHECK_FALSE(iecc(L"yets"));
        // using CTOR to init data, so make sure second object has the same results
        grammar::is_english_coordinating_conjunction iecc2;
        CHECK(iecc2(L"anD"));
        CHECK(iecc2(L"bUt"));
        CHECK(iecc2(L"Nor"));
        CHECK(iecc2(L"yeT"));
        CHECK(iecc2(L"Or"));
        CHECK(iecc2(L"sO"));
        CHECK_FALSE(iecc2(L"ye"));
        CHECK_FALSE(iecc2(L"yes"));
        CHECK_FALSE(iecc2(L"yets"));
        }
    }

TEST_CASE("German conjuctions", "[conjuctions]")
    {
    SECTION("German")
        {
        grammar::is_german_coordinating_conjunction igcc;
        CHECK(igcc(L"Und"));
        CHECK(igcc(L"oDEr"));
        CHECK(igcc(L"dEnn"));
        CHECK(igcc(L"abER"));
        CHECK(igcc(L"sONDErn"));
        CHECK_FALSE(igcc(L"undi"));
        CHECK_FALSE(igcc(L"ode"));
        // using CTOR to init data, so make sure second object has the same results
        grammar::is_german_coordinating_conjunction igcc2;
        CHECK(igcc2(L"Und"));
        CHECK(igcc2(L"oDEr"));
        CHECK(igcc2(L"dEnn"));
        CHECK(igcc2(L"abER"));
        CHECK(igcc2(L"sONDErn"));
        CHECK_FALSE(igcc2(L"undi"));
        CHECK_FALSE(igcc2(L"ode"));
        }
    }

TEST_CASE("Spanish conjuctions", "[conjuctions]")
    {
    SECTION("Spanish")
        {
        grammar::is_spanish_coordinating_conjunction iscc;
        CHECK(iscc(L"Y"));
        CHECK(iscc(L"O"));
        CHECK(iscc(L"E"));
        CHECK(iscc(L"U"));
        CHECK(iscc(L"nI"));
        CHECK(iscc(L"pEro"));
        CHECK(iscc(L"siNo"));
        CHECK_FALSE(iscc(L"per"));
        CHECK_FALSE(iscc(L"sinos"));
        // using CTOR to init data, so make sure second object has the same results
        grammar::is_spanish_coordinating_conjunction iscc2;
        CHECK(iscc2(L"Y"));
        CHECK(iscc2(L"O"));
        CHECK(iscc2(L"E"));
        CHECK(iscc2(L"U"));
        CHECK(iscc2(L"nI"));
        CHECK(iscc2(L"pEro"));
        CHECK(iscc2(L"siNo"));
        CHECK_FALSE(iscc2(L"per"));
        CHECK_FALSE(iscc2(L"sinos"));
        }
    }

TEST_CASE("Russian conjuctions", "[conjuctions]")
    {
    SECTION("Null")
        {
        is_russian_coordinating_conjunction isConj;
        CHECK_FALSE(isConj(L""));
        }
    SECTION("All")
        {
        is_russian_coordinating_conjunction isConj;
        CHECK(isConj(L"и"));
        CHECK(isConj(L"ни"));
        CHECK(isConj(L"да"));
        CHECK(isConj(L"или"));
        CHECK(isConj(L"то"));
        CHECK(isConj(L"кто"));
        CHECK(isConj(L"но"));
        CHECK(isConj(L"а"));
        CHECK(isConj(L"И"));
        CHECK(isConj(L"НИ"));
        CHECK(isConj(L"ДА"));
        CHECK(isConj(L"ИЛИ"));
        CHECK(isConj(L"ТО"));
        CHECK(isConj(L"КТО"));
        CHECK(isConj(L"НО"));
        CHECK(isConj(L"А"));
        }
    SECTION("Non conjunctions")
        {
        is_russian_coordinating_conjunction isConj;
        CHECK_FALSE(isConj(L"илиа"));
        CHECK_FALSE(isConj(L"кт"));
        CHECK_FALSE(isConj(L"даи"));
        }
    }

// NOLINTEND
// clang-format on
