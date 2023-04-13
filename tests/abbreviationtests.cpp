#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>
#include "../src/indexing/abbreviation.h"

TEST_CASE("Abbreviations", "[abbreviations]")
    {
    grammar::is_abbreviation abb;
    abb.get_abbreviations().add_word(L"std.");
        abb.get_abbreviations().add_word(L"dept.");
        abb.get_abbreviations().add_word(L"mar.");
        abb.get_abbreviations().add_word(L"jan.");

    SECTION("TestNulls")
        {
        CHECK_FALSE(abb(nullptr, 1));
        CHECK_FALSE(abb(L"std.", 0));
        CHECK_FALSE(abb(L"", 0));
        }
    SECTION("Known")
        {
        CHECK(abb(L"std.", 4));
        CHECK(abb(L"dept.", 5));
        CHECK_FALSE(abb(L"stand.", 6));
        }
    SECTION("Unknown")
        {
        CHECK(abb(L"stdr.", 5)); // all consonants
        }
    SECTION("Slash")
        {
        CHECK(abb(L"class/dept.", 11)); // dept. is an abbreviation
        CHECK_FALSE(abb(L"class/depart.", 13)); // depart. is not an abbreviation
        CHECK_FALSE(abb(L"class/", 6)); // nothing there
        }
    }
