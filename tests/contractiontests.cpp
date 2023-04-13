#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>
#include "../src/indexing/contraction.h"

using namespace grammar;
using namespace characters;

TEST_CASE("Contractions", "[contractions]")
    {
    SECTION("Test Nulls")
        {
        grammar::is_contraction isContraction;
        CHECK_FALSE(isContraction(nullptr, 1));
        CHECK_FALSE(isContraction(L"std.", 0));
        CHECK_FALSE(isContraction(L"", 0));
        }

    SECTION("Possessive")
        {
        grammar::is_contraction isContraction;
        CHECK_FALSE(isContraction(L"bob's", 5));
        CHECK_FALSE(isContraction(L"frank's", 7));
        CHECK_FALSE(isContraction(L"BOB'S", 5));
        CHECK_FALSE(isContraction(L"FRANK'S", 7));
        }

    SECTION("S Correct")
        {
        grammar::is_contraction isContraction;
        CHECK(isContraction(L"let's", 5));
        CHECK(isContraction(L"he's", 4));
        CHECK(isContraction(L"it's", 4));
        CHECK(isContraction(L"that’s", 6));
        CHECK(isContraction(L"there’s", 7));
        CHECK(isContraction(L"where’s", 7));
        CHECK(isContraction(L"LET'S", 5));
        CHECK(isContraction(L"HE'S", 4));
        CHECK(isContraction(L"IT'S", 4));
        CHECK(isContraction(L"THAT’S", 6));
        }

    SECTION("Regular")
        {
        grammar::is_contraction isContraction;
        CHECK(isContraction(L"would've", 8));
        CHECK(isContraction(L"we'd", 4));
        CHECK(isContraction(L"it'll", 5));
        CHECK(isContraction(L"t'was", 5));
        CHECK(isContraction(L"WOULD'VE", 8));
        CHECK(isContraction(L"WE'D", 4));
        CHECK(isContraction(L"IT'LL", 5));
        CHECK(isContraction(L"T'WAS", 5));
        }

    SECTION("No Apos")
        {
        grammar::is_contraction isContraction;
        CHECK_FALSE(isContraction(L"wed", 3));
        CHECK_FALSE(isContraction(L"WED", 3));
        CHECK_FALSE(isContraction(L"a", 1));
        }

    SECTION("Next Word")
        {
        grammar::is_contraction isContraction;
        CHECK(isContraction(L"Jane's", 6, L"got", 3));
        CHECK_FALSE(isContraction(L"Jane's", 6, L"got", 1/*bogus length should be handled gracefully*/));
        CHECK_FALSE(isContraction(L"Jane's", 6, L"car", 3));
        }
    }
