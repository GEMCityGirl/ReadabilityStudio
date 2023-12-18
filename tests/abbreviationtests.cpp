#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>
#include "../src/indexing/abbreviation.h"

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
