#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>
#include "../src/readability/german_readability.h"
#include "../src/readability/spanish_readability.h"
#include "../src/readability/english_readability.h"

using namespace Catch::Matchers;
using namespace readability;

TEST_CASE("Lix tests", "[lix][readability-tests]")
    {
    SECTION("LIX Indices")
        {
        CHECK(readability::lix_index_to_difficulty_level(5) == lix_difficulty::lix_very_easy);
        CHECK(readability::lix_index_to_difficulty_level(21) == lix_difficulty::lix_very_easy);
        CHECK(readability::lix_index_to_difficulty_level(25) == lix_difficulty::lix_very_easy);
        CHECK(readability::lix_index_to_difficulty_level(29) == lix_difficulty::lix_very_easy);
        CHECK(readability::lix_index_to_difficulty_level(30) == lix_difficulty::lix_easy);
        CHECK(readability::lix_index_to_difficulty_level(34) == lix_difficulty::lix_easy);
        CHECK(readability::lix_index_to_difficulty_level(35) == lix_difficulty::lix_easy);
        CHECK(readability::lix_index_to_difficulty_level(39) == lix_difficulty::lix_easy);
        CHECK(readability::lix_index_to_difficulty_level(40) == lix_difficulty::lix_average);
        CHECK(readability::lix_index_to_difficulty_level(44) == lix_difficulty::lix_average);
        CHECK(readability::lix_index_to_difficulty_level(45) == lix_difficulty::lix_average);
        CHECK(readability::lix_index_to_difficulty_level(49) == lix_difficulty::lix_average);
        CHECK(readability::lix_index_to_difficulty_level(50) == lix_difficulty::lix_difficult);
        CHECK(readability::lix_index_to_difficulty_level(54) == lix_difficulty::lix_difficult);
        CHECK(readability::lix_index_to_difficulty_level(55) == lix_difficulty::lix_difficult);
        CHECK(readability::lix_index_to_difficulty_level(59) == lix_difficulty::lix_difficult);
        CHECK(readability::lix_index_to_difficulty_level(60) == lix_difficulty::lix_very_difficult);
        CHECK(readability::lix_index_to_difficulty_level(99) == lix_difficulty::lix_very_difficult);
        }
    SECTION("LIX")
        {
        size_t grade_level;
        readability::lix_difficulty diffLevel;
        CHECK(24 == readability::lix(diffLevel, grade_level, 490582, 70206, 48544));
        CHECK(lix_difficulty::lix_very_easy == diffLevel);
        CHECK(38 == readability::lix(diffLevel, grade_level, 128409, 33373, 10514));
        CHECK(lix_difficulty::lix_easy == diffLevel);

        CHECK(readability::lix(diffLevel, grade_level, 945, 221, 78) == 36);
        CHECK(diffLevel == lix_difficulty::lix_easy);
        }
    SECTION("LIXGradeLevels")
        {
        size_t grade_level;
        readability::lix_difficulty diffLevel;

        [[maybe_unused]] auto res = readability::lix(diffLevel, grade_level, 945, 10, 150);
        CHECK(grade_level == 1);

        res = readability::lix(diffLevel, grade_level, 945, 10, 100);
        CHECK(grade_level == 2);

        res = readability::lix(diffLevel, grade_level, 945, 70, 100);
        CHECK(grade_level == 3);

        res = readability::lix(diffLevel, grade_level, 945, 80, 78);
        CHECK(grade_level == 4);

        res = readability::lix(diffLevel, grade_level, 490582, 70206, 48544);
        CHECK(grade_level == 5);

        res = readability::lix(diffLevel, grade_level, 945, 170, 78);
        CHECK(grade_level == 6);

        res = readability::lix(diffLevel, grade_level, 955, 221, 78);
        CHECK(grade_level == 7);

        res = readability::lix(diffLevel, grade_level, 128409, 33373, 10514);
        CHECK(grade_level == 8);

        res = readability::lix(diffLevel, grade_level, 965, 300, 78);
        CHECK(grade_level == 9);

        res = readability::lix(diffLevel, grade_level, 945, 320, 85);
        CHECK(grade_level == 10);

        res = readability::lix(diffLevel, grade_level, 945, 350, 78);
        CHECK(grade_level == 11);

        res = readability::lix(diffLevel, grade_level, 945, 400, 78);
        CHECK(grade_level == 12);

        res = readability::lix(diffLevel, grade_level, 945, 500, 78);
        CHECK(grade_level == 13);
        }
    SECTION("LIX Exceptions 1")
        {
        size_t grade_level;
        readability::lix_difficulty diffLevel;
        CHECK_THROWS(readability::lix(diffLevel, grade_level, 0, 221, 78));
        }
    SECTION("LIX Exceptions 2")
        {
        size_t grade_level;
        readability::lix_difficulty diffLevel;
        CHECK_THROWS(readability::lix(diffLevel, grade_level, 945, 224, 0));
        }
    }

TEST_CASE("Wheeler Smith German tests", "[wheeler-smith][readability-tests]")
    {
    SECTION("WheelerSmithRangeHelperTest")
        {
        double score;
        CHECK(readability::wheeler_smith_get_score_from_range(4.8, 2.5, 6, 1, score));
        CHECK_THAT(1.6, WithinRel(score, 1e-4));
        }
    SECTION("WheelerSmithRange")
        {
        double indexScore;
        CHECK_THAT(1, WithinRel(readability::wheeler_smith_bamberger_vanecek(100, 0, 50, indexScore), 1e-4));
        CHECK_THAT(10.9, WithinRel(readability::wheeler_smith_bamberger_vanecek(100, 100, 2, indexScore), 1e-4));
        }
    SECTION("WheelerSmithTest")
        {
        double indexScore;
        CHECK_THAT(1, WithinRel(readability::wheeler_smith_bamberger_vanecek(1000, 28, 108, indexScore), 1e-4));
        CHECK_THAT(2.5, WithinRel(indexScore, 1e-4));

        CHECK_THAT(1.6, WithinRel(readability::wheeler_smith_bamberger_vanecek(1000, 58, 119, indexScore), 1e-4));
        CHECK_THAT(4.8, WithinRel(indexScore, 1e-4));

        //book says 4.3, but really 4.4
        CHECK_THAT(1.5, WithinRel(readability::wheeler_smith_bamberger_vanecek(1000, 53, 120, indexScore), 1e-4));
        CHECK_THAT(4.4, WithinRel(indexScore, 1e-4));

        CHECK_THAT(2.6, WithinRel(readability::wheeler_smith_bamberger_vanecek(1000, 82, 105, indexScore), 1e-4));
        CHECK_THAT(7.8, WithinRel(indexScore, 1e-4));

        //book says 6.5 and 2.2, but should be 6.4 and 2.1. You would only get the numbers
        //they report if the index score was rounded, but they truncate everywhere else.
        //it must be a typo in the book.
        CHECK_THAT(2.1, WithinRel(readability::wheeler_smith_bamberger_vanecek(1000, 72, 111, indexScore), 1e-4));
        CHECK_THAT(6.4, WithinRel(indexScore, 1e-4));

        CHECK_THAT(3.4, WithinRel(readability::wheeler_smith_bamberger_vanecek(1000, 110, 105, indexScore), 1e-4));
        CHECK_THAT(10.4, WithinRel(indexScore, 1e-4));

        //book says 9.2, but really 9.3
        CHECK_THAT(3.1, WithinRel(readability::wheeler_smith_bamberger_vanecek(1000, 85, 91, indexScore), 1e-4));
        CHECK_THAT(9.3, WithinRel(indexScore, 1e-4));

        //book says 4.8 score and index score of 15.5, but a simple (12*13)/10
        //of course yields 15.6.  Obviously a math error in the book, we will test against
        //the correct numbers
        CHECK_THAT(4.9, WithinRel(readability::wheeler_smith_bamberger_vanecek(1000, 130, 83, indexScore), 1e-4));
        CHECK_THAT(15.6, WithinRel(indexScore, 1e-4));

        CHECK_THAT(4.1, WithinRel(readability::wheeler_smith_bamberger_vanecek(1000, 120, 95, indexScore), 1e-4));
        CHECK_THAT(12.6, WithinRel(indexScore, 1e-4));

        CHECK_THAT(5.7, WithinRel(readability::wheeler_smith_bamberger_vanecek(1000, 150, 80, indexScore), 1e-4));
        CHECK_THAT(18.7, WithinRel(indexScore, 1e-4));

        //The book reports a score of 5.2 and 16.5, but that's not even close.
        //(11.8*13)/10 is 15.3, not 16.5.  The book is just egregiously wrong on this example,
        //so we will just use the correct numbers here.
        CHECK_THAT(4.8, WithinRel(readability::wheeler_smith_bamberger_vanecek(10000, 1300, 847, indexScore), 1e-4));
        CHECK_THAT(15.3, WithinRel(indexScore, 1e-4));

        //book says 6.3, but should be 6.4: (21.7-20.1 = 1.6; 1.6/4(the range) = .4; 6+.4=6.4)
        CHECK_THAT(6.4, WithinRel(readability::wheeler_smith_bamberger_vanecek(1000, 150, 69, indexScore), 1e-4));
        CHECK_THAT(21.7, WithinRel(indexScore, 1e-4));

        //book says 6.6, but should be 6.7: (23-20.1=2.9; 2.9/4 = .725; 6+7.25 (round or truncated) = 6.7
        CHECK_THAT(6.7, WithinRel(readability::wheeler_smith_bamberger_vanecek(1000000, 130000, 56497, indexScore), 1e-4));
        CHECK_THAT(23, WithinRel(indexScore, 1e-4));
        }
    SECTION("Wheeler Smith Test Exceptions 1")
        {
        double index;
        CHECK_THROWS(readability::wheeler_smith_bamberger_vanecek(0, 20, 12, index));
        }
    SECTION("Wheeler Smith Test Exceptions 2")
        {
        double index;
        CHECK_THROWS(readability::wheeler_smith_bamberger_vanecek(10, 10, 0, index));
        }
    }

TEST_CASE("PSK tests", "[psk][readability-tests]")
    {
    SECTION("PSKTest")
        {
        double grade = readability::powers_sumner_kearl_flesch(4770, 7020, 550);
        CHECK_THAT(5.2, WithinRel(grade, 1e-1));
        }
    SECTION("PSKFleschRangeTest")
        {
        CHECK_THAT(0, WithinRel(readability::powers_sumner_kearl_flesch(100,0,100), 1e-1));
        CHECK_THAT(19, WithinRel(readability::powers_sumner_kearl_flesch(100,1000,2), 1e-1));
        }
    SECTION("PSKDCRangeTest")
        {
        CHECK_THAT(3.3, WithinRel(readability::powers_sumner_kearl_dale_chall(1000,0,1000), 1e-1));//equation's bare min result is 3.26
        CHECK_THAT(19, WithinRel(readability::powers_sumner_kearl_dale_chall(100,1000,2), 1e-1));
        }
    SECTION("PSK Test Exceptions 1")
        {
        CHECK_THROWS(readability::powers_sumner_kearl_flesch(0, 550, 7020));
        }
    SECTION("PSK Test Exceptions 2")
        {
        CHECK_THROWS(readability::powers_sumner_kearl_flesch(4770, 550, 0));
        }
    SECTION("PSK DC Test Exceptions 1")
        {
        CHECK_THROWS(readability::powers_sumner_kearl_dale_chall(0, 550, 7020));
        }
    SECTION("PSKTDCestExceptions2")
        {
        CHECK_THROWS(readability::powers_sumner_kearl_dale_chall(4770, 5, 0));
        }
    }

TEST_CASE("DRP tests", "[drp][readability-tests]")
    {
    SECTION("Drp Ge Range")
        {
        CHECK_THAT(1.5, WithinRel(readability::degrees_of_reading_power_ge(100, 100, 100, 100), 1e-1));//as low as equation can go
        CHECK_THAT(18, WithinRel(readability::degrees_of_reading_power_ge(100,0,10000,1), 1e-1));
        }
    SECTION("Drp Ge Exceptions")
        {
        CHECK_THROWS(readability::degrees_of_reading_power_ge(0,1,1,1));
        CHECK_THROWS(readability::degrees_of_reading_power_ge(1,1,1,0));
        }
    SECTION("Drp Range")
        {
        CHECK_THAT(5.5, WithinRel(readability::degrees_of_reading_power(100, 100, 100, 100), 1e-1));//5.5 is as low as you can locially go
        CHECK_THAT(100, WithinRel(readability::degrees_of_reading_power(100,0,2000,1), 1e-1));
        }
    SECTION("Drp Exceptions")
        {
        CHECK_THROWS(readability::degrees_of_reading_power(0,1,1,1));
        CHECK_THROWS(readability::degrees_of_reading_power(1,1,1,0));
        }
    SECTION("Conversions")
        {
        CHECK_THAT(readability::degrees_of_reading_power_to_ge(40), WithinRel(1.6, 1e-2));
        CHECK_THAT(readability::degrees_of_reading_power_to_ge(44), WithinRel(2.0, 1e-2));
        CHECK_THAT(readability::degrees_of_reading_power_to_ge(49), WithinRel(3.0, 1e-2));
        CHECK_THAT(readability::degrees_of_reading_power_to_ge(51), WithinRel(3.6, 1e-2));
        CHECK_THAT(readability::degrees_of_reading_power_to_ge(53), WithinRel(4.3, 1e-2));
        CHECK_THAT(readability::degrees_of_reading_power_to_ge(56), WithinRel(5.5, 1e-2));
        CHECK_THAT(readability::degrees_of_reading_power_to_ge(57), WithinRel(5.9, 1e-2));
        CHECK_THAT(readability::degrees_of_reading_power_to_ge(58), WithinRel(6.3, 1e-2));
        CHECK_THAT(readability::degrees_of_reading_power_to_ge(59), WithinRel(6.8, 1e-2));
        CHECK_THAT(readability::degrees_of_reading_power_to_ge(60), WithinRel(7.3, 1e-2));
        CHECK_THAT(readability::degrees_of_reading_power_to_ge(61), WithinRel(7.8, 1e-2));
        CHECK_THAT(readability::degrees_of_reading_power_to_ge(64), WithinRel(9.4, 1e-2));
        }
    }

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

TEST_CASE("nWS tests", "[nWS][readability-tests]")
    {
   SECTION("nWS1 Range")
        {
        CHECK_THAT(1, WithinRel(readability::neue_wiener_sachtextformel_1(100,100,0,0,50), 1e-1));
        CHECK_THAT(19, WithinRel(readability::neue_wiener_sachtextformel_1(100,0,100,50,2), 1e-1));
        }
   SECTION("nWS1 Exceptions 1")
        {
        CHECK_THROWS(readability::neue_wiener_sachtextformel_1(0, 51, 23, 28, 8));
        }
   SECTION("nWS1 Exceptions 2")
        {
        CHECK_THROWS(readability::neue_wiener_sachtextformel_1(100, 51, 23, 28, 0));
        }
   SECTION("nWS2 Range")
        {
        CHECK_THAT(1, WithinRel(readability::neue_wiener_sachtextformel_2(100,0,0,50), 1e-1));
        CHECK_THAT(19, WithinRel(readability::neue_wiener_sachtextformel_2(100,100,100,2), 1e-1));
        }
   SECTION("nWS2 Exceptions 1")
        {
        CHECK_THROWS(readability::neue_wiener_sachtextformel_2(0, 51, 23, 8));
        }
   SECTION("nWS2 Exceptions 2")
        {   
        CHECK_THROWS(readability::neue_wiener_sachtextformel_2(100, 51, 23, 0));
        }
   SECTION("nWS3 Range")
        {
        CHECK_THAT(1, WithinRel(readability::neue_wiener_sachtextformel_3(100,0,50), 1e-1));
        CHECK_THAT(19, WithinRel(readability::neue_wiener_sachtextformel_3(100,100,2), 1e-1));
        }
   SECTION("nWS3 Exceptions 1")
        {
        CHECK_THROWS(readability::neue_wiener_sachtextformel_3(0, 51, 8));
        }
   SECTION("nWS3 Exceptions 2")
        {
        CHECK_THROWS(readability::neue_wiener_sachtextformel_3(100, 51, 0));
        }
    }
