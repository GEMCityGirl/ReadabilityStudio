#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>
#include "../src/indexing/double_words.h"

using namespace grammar;
using namespace characters;

TEST_CASE("Double Words", "[double words]")
    {
    SECTION("Nulls")
        {
        grammar::is_double_word_exception dwe;
        CHECK(dwe(nullptr, 1));
        CHECK(dwe(L"that", 0));
        }
    SECTION("English")
        {
        grammar::is_double_word_exception dwe;
        CHECK_FALSE(dwe(L"thats", 5));
        CHECK_FALSE(dwe(L"H", 1));
        CHECK(dwe(L"tHat", 4));
        CHECK(dwe(L"Had", 3));
        CHECK(dwe(L"HA", 2));
        // using CTOR to init data, so make sure second object has the same results
        grammar::is_double_word_exception dwe2;
        CHECK_FALSE(dwe2(L"thats", 5));
        CHECK_FALSE(dwe2(L"H", 1));
        CHECK(dwe2(L"tHat", 4));
        CHECK(dwe2(L"Had", 3));
        CHECK(dwe2(L"HA", 2));
        }
    SECTION("German")
        {
        grammar::is_double_word_exception dwe;
        CHECK_FALSE(dwe(L"si", 2));
        CHECK(dwe(L"sie", 3));
        CHECK(dwe(L"das", 3));
        CHECK(dwe(L"der", 3));
        CHECK(dwe(L"die", 3));
        CHECK_FALSE(dwe(L"died", 4));
        }
    }
