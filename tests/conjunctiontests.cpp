#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>
#include "../src/indexing/conjunction.h"

using namespace grammar;
using namespace characters;

TEST_CASE("English conjuctions", "[conjuctions]")
    {
    SECTION("Nulls")
        {
        grammar::is_english_coordinating_conjunction iecc;
        CHECK_FALSE(iecc(nullptr, 1));
        CHECK_FALSE(iecc(L"that", 0));
        CHECK_FALSE(iecc(L"", 0));
        grammar::is_german_coordinating_conjunction igcc;
        CHECK_FALSE(igcc(nullptr, 1));
        CHECK_FALSE(igcc(L"that", 0));
        CHECK_FALSE(igcc(L"", 0));
        grammar::is_spanish_coordinating_conjunction iscc;
        CHECK_FALSE(iscc(nullptr, 1));
        CHECK_FALSE(iscc(L"that", 0));
        CHECK_FALSE(iscc(L"", 0));
        }
    SECTION("English")
        {
        grammar::is_english_coordinating_conjunction iecc;
        CHECK(iecc(L"anD", 3));
        CHECK(iecc(L"bUt", 3));
        CHECK(iecc(L"Nor", 3));
        CHECK(iecc(L"yeT", 3));
        CHECK(iecc(L"Or", 2));
        CHECK(iecc(L"sO", 2));
        CHECK_FALSE(iecc(L"ye", 2));
        CHECK_FALSE(iecc(L"yes", 3));
        CHECK_FALSE(iecc(L"yets", 4));
        // using CTOR to init data, so make sure second object has the same results
        grammar::is_english_coordinating_conjunction iecc2;
        CHECK(iecc2(L"anD", 3));
        CHECK(iecc2(L"bUt", 3));
        CHECK(iecc2(L"Nor", 3));
        CHECK(iecc2(L"yeT", 3));
        CHECK(iecc2(L"Or", 2));
        CHECK(iecc2(L"sO", 2));
        CHECK_FALSE(iecc2(L"ye", 2));
        CHECK_FALSE(iecc2(L"yes", 3));
        CHECK_FALSE(iecc2(L"yets", 4));
        }
    }

TEST_CASE("German conjuctions", "[conjuctions]")
    {
    SECTION("German")
        {
        grammar::is_german_coordinating_conjunction igcc;
        CHECK(igcc(L"Und", 3));
        CHECK(igcc(L"oDEr", 4));
        CHECK(igcc(L"dEnn", 4));
        CHECK(igcc(L"abER", 4));
        CHECK(igcc(L"sONDErn", 7));
        CHECK_FALSE(igcc(L"undi", 4));
        CHECK_FALSE(igcc(L"ode", 3));
        // using CTOR to init data, so make sure second object has the same results
        grammar::is_german_coordinating_conjunction igcc2;
        CHECK(igcc2(L"Und", 3));
        CHECK(igcc2(L"oDEr", 4));
        CHECK(igcc2(L"dEnn", 4));
        CHECK(igcc2(L"abER", 4));
        CHECK(igcc2(L"sONDErn", 7));
        CHECK_FALSE(igcc2(L"undi", 4));
        CHECK_FALSE(igcc2(L"ode", 3));
        }
    }

TEST_CASE("Spanish conjuctions", "[conjuctions]")
    {
    SECTION("Spanish")
        {
        grammar::is_spanish_coordinating_conjunction iscc;
        CHECK(iscc(L"Y", 1));
        CHECK(iscc(L"O", 1));
        CHECK(iscc(L"E", 1));
        CHECK(iscc(L"U", 1));
        CHECK(iscc(L"nI", 2));
        CHECK(iscc(L"pEro", 4));
        CHECK(iscc(L"siNo", 4));
        CHECK_FALSE(iscc(L"per", 3));
        CHECK_FALSE(iscc(L"sinos", 5));
        // using CTOR to init data, so make sure second object has the same results
        grammar::is_spanish_coordinating_conjunction iscc2;
        CHECK(iscc2(L"Y", 1));
        CHECK(iscc2(L"O", 1));
        CHECK(iscc2(L"E", 1));
        CHECK(iscc2(L"U", 1));
        CHECK(iscc2(L"nI", 2));
        CHECK(iscc2(L"pEro", 4));
        CHECK(iscc2(L"siNo", 4));
        CHECK_FALSE(iscc2(L"per", 3));
        CHECK_FALSE(iscc2(L"sinos", 5));
        }
    }

TEST_CASE("Russian conjuctions", "[conjuctions]")
    {
    SECTION("Null")
        {
        is_russian_coordinating_conjunction isConj;
        CHECK_FALSE(isConj(nullptr, 5));
        CHECK_FALSE(isConj(L"", 3));
        CHECK_FALSE(isConj(L"и", 0));
        }
    SECTION("All")
        {
        is_russian_coordinating_conjunction isConj;
        CHECK(isConj(L"и", 1));
        CHECK(isConj(L"ни", 2));
        CHECK(isConj(L"да", 2));
        CHECK(isConj(L"или", 3));
        CHECK(isConj(L"то", 2));
        CHECK(isConj(L"кто", 3));
        CHECK(isConj(L"но", 2));
        CHECK(isConj(L"а", 1));
        CHECK(isConj(L"И", 1));
        CHECK(isConj(L"НИ", 2));
        CHECK(isConj(L"ДА", 2));
        CHECK(isConj(L"ИЛИ", 3));
        CHECK(isConj(L"ТО", 2));
        CHECK(isConj(L"КТО", 3));
        CHECK(isConj(L"НО", 2));
        CHECK(isConj(L"А", 1));
        }
    SECTION("Non conjunctions")
        {
        is_russian_coordinating_conjunction isConj;
        CHECK_FALSE(isConj(L"илиа", 4));
        CHECK_FALSE(isConj(L"кт", 2));
        CHECK_FALSE(isConj(L"даи", 3));
        }
    }
