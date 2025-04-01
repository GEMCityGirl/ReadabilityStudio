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
#include "../src/indexing/contraction.h"

// clang-format off
// NOLINTBEGIN

using namespace grammar;
using namespace characters;

TEST_CASE("Contractions", "[contractions]")
    {
    SECTION("Test Nulls")
        {
        grammar::is_contraction isContraction;
        CHECK_FALSE(isContraction(L""));
        }

    SECTION("Possessive")
        {
        grammar::is_contraction isContraction;
        CHECK_FALSE(isContraction(L"bob's"));
        CHECK_FALSE(isContraction(L"frank's"));
        CHECK_FALSE(isContraction(L"BOB'S"));
        CHECK_FALSE(isContraction(L"FRANK'S"));
        }

    SECTION("S Correct")
        {
        grammar::is_contraction isContraction;
        CHECK(isContraction(L"let's"));
        CHECK(isContraction(L"he's"));
        CHECK(isContraction(L"it's"));
        CHECK(isContraction(L"that’s"));
        CHECK(isContraction(L"there’s"));
        CHECK(isContraction(L"where’s"));
        CHECK(isContraction(L"LET'S"));
        CHECK(isContraction(L"HE'S"));
        CHECK(isContraction(L"IT'S"));
        CHECK(isContraction(L"THAT’S"));
        }

    SECTION("Regular")
        {
        grammar::is_contraction isContraction;
        CHECK(isContraction(L"would've"));
        CHECK(isContraction(L"we'd"));
        CHECK(isContraction(L"it'll"));
        CHECK(isContraction(L"t'was"));
        CHECK(isContraction(L"WOULD'VE"));
        CHECK(isContraction(L"WE'D"));
        CHECK(isContraction(L"IT'LL"));
        CHECK(isContraction(L"T'WAS"));
        }

    SECTION("Whole word")
        {
        grammar::is_contraction isContraction;
        CHECK(isContraction(L"gonna"));
        CHECK(isContraction(L"wanna"));
        }

    SECTION("No Apos")
        {
        grammar::is_contraction isContraction;
        CHECK_FALSE(isContraction(L"wed"));
        CHECK_FALSE(isContraction(L"WED"));
        CHECK_FALSE(isContraction(L"a"));
        }

    SECTION("Next Word")
        {
        grammar::is_contraction isContraction;
        CHECK(isContraction(L"Jane's", L"got"));
        CHECK_FALSE(isContraction(L"Jane's", L"car"));
        }
    }

// NOLINTEND
// clang-format on
