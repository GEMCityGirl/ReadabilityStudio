#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>
#include "../src/readability/german_readability.h"
#include "../src/readability/spanish_readability.h"
#include "../src/readability/english_readability.h"

using namespace Catch::Matchers;

TEST_CASE("Fog tests", "[fog][readability-tests]")
    {
    SECTION("FOG")
        {
        CHECK_THAT(readability::gunning_fog(129, 26, 10), WithinRel(13.2, 1e-1));
        CHECK_THAT(readability::gunning_fog(102, 9, 6), WithinRel(10.3, 1e-1));
        CHECK_THAT(readability::gunning_fog(102, 13, 4), WithinRel(15.3, 1e-1));
        CHECK_THAT(readability::gunning_fog(104, 1, 14), WithinRel(3.4, 1e-1));
        CHECK_THAT(readability::gunning_fog(101, 6, 6), WithinRel(9.1, 1e-1));
        CHECK_THAT(readability::gunning_fog(94, 2, 6), WithinRel(7.1, 1e-1));
        }
    SECTION("FOG Range")
        {
        CHECK_THAT(std::floor(readability::gunning_fog(100, 0, 100)), WithinRel(0, 1e-1));
        CHECK_THAT(readability::gunning_fog(100, 100, 2), WithinRel(19, 1e-1));
        }
    SECTION("FOG Exceptions1")
        {
        CHECK_THROWS(readability::gunning_fog(0, 6, 9));
        }
    SECTION("FOG Exceptions2")
        {
        CHECK_THROWS(readability::gunning_fog(102, 6, 0));
        }
    SECTION("Psk FOG Exceptions")
        {
        CHECK_THROWS(readability::psk_gunning_fog(0, 6, 5));
        CHECK_THROWS(readability::psk_gunning_fog(5, 6, 0));
        }
    SECTION("New FOG Exceptions")
        {
        CHECK_THROWS(readability::new_fog_count(0, 6, 5));
        CHECK_THROWS(readability::new_fog_count(5, 6, 0));
        }
    SECTION("Psk FOG Range")
        {
        // as low as it can go
        CHECK_THAT(readability::psk_gunning_fog(100, 0, 100), WithinRel(3.1, 1e-1));
        CHECK_THAT(readability::psk_gunning_fog(100, 100, 1), WithinRel(19, 1e-1));
        }
    SECTION("New FOG Range")
        {
        CHECK_THAT(readability::new_fog_count(100, 0, 100), WithinRel(0, 1e-1));
        CHECK_THAT(readability::new_fog_count(100, 100, 2), WithinRel(19, 1e-1));
        }
    SECTION("HardWords")
        {
        CHECK(readability::is_easy_gunning_fog_word(L"reallybigword", 13, 4) == false);
        }
    SECTION("Easy Words")
        {
        CHECK(readability::is_easy_gunning_fog_word(L"simple", 6, 2));
        CHECK(readability::is_easy_gunning_fog_word(L"easy", 4, 2));
        // from the book
        CHECK(readability::is_easy_gunning_fog_word(L"created", 7, 3));
        CHECK(readability::is_easy_gunning_fog_word(L"trespasses", 10, 3));
        // es rules
        CHECK(readability::is_easy_gunning_fog_word(L"ashashes", 8, 3));
        CHECK(readability::is_easy_gunning_fog_word(L"araces", 6, 3));
        CHECK(readability::is_easy_gunning_fog_word(L"enrages", 7, 3));
        CHECK(readability::is_easy_gunning_fog_word(L"ahexes", 6, 3));
        CHECK(readability::is_easy_gunning_fog_word(L"amazes", 6, 3));
        CHECK(readability::is_easy_gunning_fog_word(L"azases", 6, 3));
        CHECK(readability::is_easy_gunning_fog_word(L"amazases", 8, 4) == false);//4 syllables fail
                                                                                 //ed rule
        CHECK(readability::is_easy_gunning_fog_word(L"ahated", 6, 3));
        CHECK(readability::is_easy_gunning_fog_word(L"ahaded", 6, 3));
        // 4 syllables fail
        CHECK(readability::is_easy_gunning_fog_word(L"adahaded", 8, 4) == false);
        }
    SECTION("Hypenated Words")
        {
        // 1 and 2 syllables, should be easy
        CHECK(readability::is_easy_gunning_fog_word(L"high-roller", 11, 3));
        // 1, 1, and 2 syllables, should be easy
        CHECK(readability::is_easy_gunning_fog_word(L"rock-of-ages", 12, 4));
        // 3 and 2 syllables, should be hard
        CHECK(readability::is_easy_gunning_fog_word(L"highroller-man", 14, 4) == false);
        // 2 and 3 syllables, should be hard
        CHECK(readability::is_easy_gunning_fog_word(L"man-highroller", 14, 4) == false);
        }
    SECTION("Hypenated Words Stray Hyphens")
        {
        // 1 and 2 syllables, should be easy
        CHECK(readability::is_easy_gunning_fog_word(L"high-roller-", 12, 3));
        // 1 and 2 syllables, should be easy
        CHECK(readability::is_easy_gunning_fog_word(L"-high-roller", 12, 3));
        }
    SECTION("Slashed Words")
        {
        // 1 and 2 syllables, should be easy
        CHECK(readability::is_easy_gunning_fog_word(L"high/roller", 11, 3));
        // 1, 1, and 2 syllables, should be easy
        CHECK(readability::is_easy_gunning_fog_word(L"rock/of-ages", 12, 4));
        // 3 and 2 syllables, should be hard
        CHECK(readability::is_easy_gunning_fog_word(L"highroller/man", 14, 4) == false);
        // 2 and 3 syllables, should be hard
        CHECK(readability::is_easy_gunning_fog_word(L"man/highroller", 14, 4) == false);
        }
    SECTION("Slashed Words Stray Hyphens")
        {
        // 1 and 2 syllables, should be easy
        CHECK(readability::is_easy_gunning_fog_word(L"high/roller/", 12, 3));
        // 1 and 2 syllables, should be easy
        CHECK(readability::is_easy_gunning_fog_word(L"/high/roller", 12, 3));
        }
    }
