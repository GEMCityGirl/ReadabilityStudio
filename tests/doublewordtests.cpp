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
#include "../src/indexing/double_words.h"

// clang-format off
// NOLINTBEGIN

using namespace grammar;
using namespace characters;

TEST_CASE("Double Words", "[double words]")
    {
    SECTION("Nulls")
        {
        grammar::is_double_word_exception dwe;
        CHECK(dwe(L""));
        }
    SECTION("English")
        {
        grammar::is_double_word_exception dwe;
        CHECK_FALSE(dwe(L"thats"));
        CHECK_FALSE(dwe(L"H"));
        CHECK(dwe(L"tHat"));
        CHECK(dwe(L"Had"));
        CHECK(dwe(L"HA"));
        // using CTOR to init data, so make sure second object has the same results
        grammar::is_double_word_exception dwe2;
        CHECK_FALSE(dwe2(L"thats"));
        CHECK_FALSE(dwe2(L"H"));
        CHECK(dwe2(L"tHat"));
        CHECK(dwe2(L"Had"));
        CHECK(dwe2(L"HA"));
        }
    SECTION("German")
        {
        grammar::is_double_word_exception dwe;
        CHECK_FALSE(dwe(L"si"));
        CHECK(dwe(L"sie"));
        CHECK(dwe(L"das"));
        CHECK(dwe(L"der"));
        CHECK(dwe(L"die"));
        CHECK_FALSE(dwe(L"died"));
        }
    }
// NOLINTEND
// clang-format on
