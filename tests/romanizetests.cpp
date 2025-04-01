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
#include <catch2/matchers/catch_matchers_floating_point.hpp>
#include "../src/indexing/romanize.h"

// clang-format off
// NOLINTBEGIN

using namespace Catch::Matchers;
using namespace lily_of_the_valley;

TEST_CASE("Romanize", "[romanize]")
    {
    SECTION("Null")
        {
        text_transform::romanize encode;
        CHECK(encode({ L"text", 0 }, true, false, false, true) == L"");
        }
    SECTION("Punctuation")
        {
        text_transform::romanize encode;
        CHECK(encode(L"\x201DSören\x201E\x2013…", true, false, false, true) == L"\"Soeren\"--...");
        CHECK(encode(L"±5¼½¾©®™", true, false, false, true) == L"+-51/41/23/4(c)(r)(tm)");
        }
    SECTION("Special Chars")
        {
        text_transform::romanize encode;
        CHECK(encode(L"ŒÀÁÂÃÄÅÆÇÈÉÊËÌÍÎÏÑÒÓÔÕÖØÙÚÛÜÝß", true, false, false, true) == L"OeAAAAAeAaAeCEEEEIIIINnOOOOOeOUUUUeYss");
        CHECK(encode(L"œàáâãäåæçèéêëìíîïñòóôõöøùúûüýý", true, false, false, true) == L"oeaaaaaeaaaeceeeeiiiinnoooooeouuuueyy");
        }
    SECTION("Preserve Special Chars")
        {
        text_transform::romanize encode;
        CHECK(encode(L"ŒÀÁÂÃÄÅÆÇÈÉÊËÌÍÎÏÑÒÓÔÕÖØÙÚÛÜÝß", false, false, false, true) == L"ŒÀÁÂÃÄÅÆÇÈÉÊËÌÍÎÏÑÒÓÔÕÖØÙÚÛÜÝß");
        CHECK(encode(L"œàáâãäåæçèéêëìíîïñòóôõöøùúûüýý", false, false, false, true) == L"œàáâãäåæçèéêëìíîïñòóôõöøùúûüýý");
        CHECK(encode(L"±5¼½¾©®™", false, false, false, true) == L"±5¼½¾©®™");
        CHECK(encode(L"\x201DSören\x201E\x2013…", false, false, false, true) == L"\x201DSören\x201E\x2013…");
        }
    SECTION("Ful lWidth Characters")
        {
        text_transform::romanize encode;
        CHECK(encode(L"Ｈｅｌｌｏ！", true, false, false, true) == L"Hello!");
        }
    SECTION("Ellipsis removal")
        {
        text_transform::romanize encode;
        CHECK(encode(L"Hi…", true, true, false, true) == L"Hi.");
        CHECK(encode(L"Hi… ", true, true, false, true) == L"Hi. ");
        CHECK(encode(L"Hi… ", true, true, false, true) == L"Hi. ");
        CHECK(encode(L"Hi… there", true, true, false, true) == L"Hi  there");
        CHECK(encode(L"Hi…there", true, true, false, true) == L"Hi there");
        CHECK(encode(L"Hi… There", true, true, false, true) == L"Hi. There");
        CHECK(encode(L"Hi…There", true, true, false, true) == L"Hi.There");

        CHECK(encode(L"Hi... there", true, true, false, true) == L"Hi  there");
        CHECK(encode(L"Hi．．． there", true, true, false, true) == L"Hi  there"); // full width
        CHECK(encode(L"Hi...there", true, true, false, true) == L"Hi there");
        CHECK(encode(L"Hi... There", true, true, false, true) == L"Hi. There");
        CHECK(encode(L"Hi...There", true, true, false, true) == L"Hi.There");
        CHECK(encode(L"Hi...", true, true, false, true) == L"Hi.");

        CHECK(encode(L"Hi...\n\rThere", true, true, false, true) == L"Hi.\n\rThere");

        CHECK(encode({ L"Hi... there", 6 }, true, true, false, true) ==
              L"Hi. "); // don't read all the way to the 'T'
        }
    SECTION("Bullet removal")
        {
        text_transform::romanize encode;
        CHECK(encode(L"1. one\n2. two\n\r13. three\nend of list", true, true, false, true) == L"1. one\n2. two\n\r13. three\nend of list");//should NOT remove bullets here
        // now test bullet removal
        CHECK(encode(L" 1. one\n2. two\n\r13. three\nend of list", true, true, true, true) == L" \t one\n\t two\n\r\t three\nend of list");
        CHECK(encode(L"1. one\n2. two\n\r13. three\nend of list", true, true, true, true) == L"\t one\n\t two\n\r\t three\nend of list");
        CHECK(encode(L"Hi one\n2. two\n\r13. three\nend of list", true, true, true, true) == L"Hi one\n\t two\n\r\t three\nend of list");
        CHECK(encode(L"· one\n2. two\n\r13. three\nend of list", true, true, true, true) == L"\t one\n\t two\n\r\t three\nend of list");
        CHECK(encode(L"\t one\n2. two\n\r13. three\nend of list", true, true, true, true) == L"\t one\n\t two\n\r\t three\nend of list");
        }
    }
// NOLINTEND
// clang-format on
