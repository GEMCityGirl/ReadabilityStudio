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
#include "../src/indexing/character_traits.h"

// clang-format off
// NOLINTBEGIN

TEST_CASE("Wide & Narrow", "[string]")
    {
    SECTION("Narrow letters")
        {
        traits::case_insensitive_wstring_ex wide(L"ＨＥＬＬＯ");
        traits::case_insensitive_wstring_ex narrow(L"Hello");
        CHECK(wide == narrow);
        }
    }
// NOLINTEND
// clang-format on
