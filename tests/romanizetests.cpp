#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>
#include "../src/indexing/romanize.h"

using namespace Catch::Matchers;
using namespace lily_of_the_valley;

TEST_CASE("Romanize", "[romanize]")
    {
    SECTION("Null")
        {
        text_transform::romanize encode;
        CHECK(encode(NULL, 1, true, false, false, true) == L"");
        CHECK(encode(L"text", 0, true, false, false, true) == L"");
        }
    SECTION("Punctuation")
        {
        text_transform::romanize encode;
        CHECK(encode(L"\x201DSören\x201E\x2013…", 9, true, false, false, true) == L"\"Soeren\"--...");
        CHECK(encode(L"±5¼½¾©®™", 8, true, false, false, true) == L"+-51/41/23/4(c)(r)(tm)");
        }
    SECTION("Special Chars")
        {
        text_transform::romanize encode;
        CHECK(encode(L"ŒÀÁÂÃÄÅÆÇÈÉÊËÌÍÎÏÑÒÓÔÕÖØÙÚÛÜÝß", 30, true, false, false, true) == L"OeAAAAAeAaAeCEEEEIIIINnOOOOOeOUUUUeYss");
        CHECK(encode(L"œàáâãäåæçèéêëìíîïñòóôõöøùúûüýý", 30, true, false, false, true) == L"oeaaaaaeaaaeceeeeiiiinnoooooeouuuueyy");
        }
    SECTION("Preserve Special Chars")
        {
        text_transform::romanize encode;
        CHECK(encode(L"ŒÀÁÂÃÄÅÆÇÈÉÊËÌÍÎÏÑÒÓÔÕÖØÙÚÛÜÝß", 30, false, false, false, true) == L"ŒÀÁÂÃÄÅÆÇÈÉÊËÌÍÎÏÑÒÓÔÕÖØÙÚÛÜÝß");
        CHECK(encode(L"œàáâãäåæçèéêëìíîïñòóôõöøùúûüýý", 30, false, false, false, true) == L"œàáâãäåæçèéêëìíîïñòóôõöøùúûüýý");
        CHECK(encode(L"±5¼½¾©®™", 8, false, false, false, true) == L"±5¼½¾©®™");
        CHECK(encode(L"\x201DSören\x201E\x2013…", 9, false, false, false, true) == L"\x201DSören\x201E\x2013…");
        }
    SECTION("Ful lWidth Characters")
        {
        text_transform::romanize encode;
        CHECK(encode(L"Ｈｅｌｌｏ！", 6, true, false, false, true) == L"Hello!");
        }
    SECTION("Ellipsis removal")
        {
        text_transform::romanize encode;
        CHECK(encode(L"Hi…", 3, true, true, false, true) == L"Hi.");
        CHECK(encode(L"Hi… ", 4, true, true, false, true) == L"Hi. ");
        CHECK(encode(L"Hi… ", 4, true, true, false, true) == L"Hi. ");
        CHECK(encode(L"Hi… there", 9, true, true, false, true) == L"Hi  there");
        CHECK(encode(L"Hi…there", 8, true, true, false, true) == L"Hi there");
        CHECK(encode(L"Hi… There", 9, true, true, false, true) == L"Hi. There");
        CHECK(encode(L"Hi…There", 8, true, true, false, true) == L"Hi.There");

        CHECK(encode(L"Hi... there", 11, true, true, false, true) == L"Hi  there");
        CHECK(encode(L"Hi．．． there", 11, true, true, false, true) == L"Hi  there"); // full width
        CHECK(encode(L"Hi...there", 10, true, true, false, true) == L"Hi there");
        CHECK(encode(L"Hi... There", 11, true, true, false, true) == L"Hi. There");
        CHECK(encode(L"Hi...There", 10, true, true, false, true) == L"Hi.There");
        CHECK(encode(L"Hi...", 5, true, true, false, true) == L"Hi.");

        CHECK(encode(L"Hi...\n\rThere", 12, true, true, false, true) == L"Hi.\n\rThere");

        CHECK(encode(L"Hi... there", 6, true, true, false, true) == L"Hi. ");//don't read all the way to the 'T'
        }
    SECTION("Bullet removal")
        {
        text_transform::romanize encode;
        CHECK(encode(L"1. one\n2. two\n\r13. three\nend of list", 36, true, true, false, true) == L"1. one\n2. two\n\r13. three\nend of list");//should NOT remove bullets here
        // now test bullet removal
        CHECK(encode(L" 1. one\n2. two\n\r13. three\nend of list", 37, true, true, true, true) == L" \t one\n\t two\n\r\t three\nend of list");
        CHECK(encode(L"1. one\n2. two\n\r13. three\nend of list", 36, true, true, true, true) == L"\t one\n\t two\n\r\t three\nend of list");
        CHECK(encode(L"Hi one\n2. two\n\r13. three\nend of list", 36, true, true, true, true) == L"Hi one\n\t two\n\r\t three\nend of list");
        CHECK(encode(L"· one\n2. two\n\r13. three\nend of list", 35, true, true, true, true) == L"\t one\n\t two\n\r\t three\nend of list");
        CHECK(encode(L"\t one\n2. two\n\r13. three\nend of list", 35, true, true, true, true) == L"\t one\n\t two\n\r\t three\nend of list");
        }
    }
