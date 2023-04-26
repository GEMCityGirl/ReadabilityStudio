#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>
#include "../src/indexing/character_traits.h"
#include "../src/OleanderStemmingLibrary/src/common_lang_constants.h"

using namespace Catch::Matchers;

TEST_CASE("Western character traits", "[chartraits]")
    {
    SECTION("Length")
        {
        CHECK(traits::case_insensitive_ex::length(L"") == 0);
        CHECK(traits::case_insensitive_ex::length(L"Some'String5") == 12);
        CHECK(traits::case_insensitive_ex::length(L"Some String5") == 12);
        }
    SECTION("Assign")
        {
        wchar_t a = L'b', a2 = L'a';
        traits::case_insensitive_ex::assign(a, a2);
        CHECK(a == L'a');
        }
    SECTION("Assign 2")
        {
        wchar_t myArray[5] = { 0,0,0,0,0 };
        wchar_t* result = traits::case_insensitive_ex::assign(myArray, 5, L'z');
        for (size_t i = 0; i < 5; ++i)
            { CHECK(myArray[i] == L'z'); }
        CHECK(result == myArray);
        for (size_t i = 0; i < 5; ++i)
            { CHECK(result[i] == L'z'); }
        }
    SECTION("Move")
        {
        wchar_t myArray[5] = { 0,0,0,0,0 };
        wchar_t myZArray[5] = { L'z',L'z',L'z',L'z',L'z' };
        traits::case_insensitive_ex::move(myArray, myZArray, 5);
        for (size_t i = 0; i < 5; ++i)
            { CHECK(myArray[i] == L'z'); }
        for (size_t i = 0; i < 5; ++i)
            { CHECK(myZArray[i] == L'z'); }
        }
    SECTION("Copy")
        {
        wchar_t myArray[5] = { 0,0,0,0,0 };
        wchar_t myZArray[5] = { L'z',L'z',L'z',L'z',L'z' };
        traits::case_insensitive_ex::copy(myArray, myZArray, 5);
        for (size_t i = 0; i < 5; ++i)
            { CHECK(myArray[i] == L'z'); }
        for (size_t i = 0; i < 5; ++i)
            { CHECK(myZArray[i] == L'z'); }
        }
    SECTION("Compare")
        {
        CHECK(traits::case_insensitive_ex::compare(L"HELLO", L"hello", 5) == 0);
        CHECK(traits::case_insensitive_ex::compare(L"hello", L"hello", 5) == 0);
        CHECK(traits::case_insensitive_ex::compare(L"helÜo", L"helüo", 5) == 0);
        }
    SECTION("Find")
        {
        const wchar_t* myString = L"HELLO";
        CHECK(traits::case_insensitive_ex::find(myString, 1, L'o') == nullptr);
        CHECK(traits::case_insensitive_ex::find(myString, 2, L'o') == nullptr);
        CHECK(traits::case_insensitive_ex::find(myString, 3, L'o') == nullptr);
        CHECK(traits::case_insensitive_ex::find(myString, 4, L'o') == nullptr);
        CHECK(traits::case_insensitive_ex::find(myString, 5, L'o') == myString+4);
        }
    SECTION("Find 2")
        {
        const wchar_t* myString = L"HELLO";
        CHECK(traits::case_insensitive_ex::find(myString, 1, L"lo", 2) == nullptr);
        CHECK(traits::case_insensitive_ex::find(myString, 2, L"lo", 2) == nullptr);
        CHECK(traits::case_insensitive_ex::find(myString, 3, L"lo", 2) == nullptr);
        CHECK(traits::case_insensitive_ex::find(myString, 4, L"lo", 2) == myString+3);
        }
    SECTION("Apostrophes")
        {
        CHECK(traits::case_insensitive_ex::eq(L'\'', 8217));
        CHECK(traits::case_insensitive_ex::lt(L'\'', 8217) == false);
        CHECK(traits::case_insensitive_ex::lt(L'a', 8217) == false);
        CHECK(traits::case_insensitive_ex::lt(8217, L'a'));
        CHECK(traits::case_insensitive_ex::compare(L"it's", L"it´s", 4) == 0);
        CHECK(traits::case_insensitive_ex::compare(L"itself", L"it´s", 4) == 1);
        CHECK(traits::case_insensitive_ex::compare(L"it´s", L"itself", 4) == -1);

        typedef std::basic_string<wchar_t, traits::case_insensitive_ex > myString;
        myString s1 = L"it's";
        myString s2 = L"it´s";
        CHECK(s1 == s2);
        CHECK((s1 < s2) == false);
        CHECK((s1 > s2) == false);
        CHECK((s2 < s1) == false);
        CHECK((s2 > s1) == false);
        }
    SECTION("String Functor")
        {
        typedef std::basic_string<wchar_t, traits::case_insensitive_ex > myString;
        wchar_t buffer[6];
        myString s1 = L"NothingString";
        myString s2 = L"IT´s";
        // test partial assignment
        s1.assign(L"IT´s", 2, 1);
        CHECK(s1 == L"´");
        s1.assign(L"Nothin");
        // test copy
        s1.copy(buffer, 6);
        CHECK(std::wcsncmp(buffer, L"Nothin", 6) == 0);
        // test full assignment
        s1.assign(L"IT´s");
        CHECK(s1 == s2);
        CHECK((s1 < s2) == false);
        CHECK((s1 > s2) == false);
        CHECK((s2 < s1) == false);
        CHECK((s2 > s1) == false);
        }
    }

TEST_CASE("Character traits", "[chartraits]")
    {
    SECTION("Is Lower")
        {
        CHECK(characters::is_character::is_lower(L'a'));
        CHECK(characters::is_character::is_lower(L'z'));
        CHECK(characters::is_character::is_lower(L'A') == false);
        CHECK(characters::is_character::is_lower(L'×') == false);
        CHECK(characters::is_character::is_lower(L'÷') == false);
        CHECK(characters::is_character::is_lower(L'Å') == false);
        CHECK(characters::is_character::is_lower(L'Û') == false);
        CHECK(characters::is_character::is_lower(L'å'));
        CHECK(characters::is_character::is_lower(L'ý'));
        }
    SECTION("Is Upper")
        {
        CHECK(characters::is_character::is_upper(L'a') == false);
        CHECK(characters::is_character::is_upper(L'z') == false);
        CHECK(characters::is_character::is_upper(L'A'));
        CHECK(characters::is_character::is_upper(L'×') == false);
        CHECK(characters::is_character::is_upper(L'÷') == false);
        CHECK(characters::is_character::is_upper(L'Å'));
        CHECK(characters::is_character::is_upper(L'Û'));
        CHECK(characters::is_character::is_upper(L'å') == false);
        CHECK(characters::is_character::is_upper(L'ý') == false);
        }

    SECTION("Numeric String")
        {
        CHECK(characters::is_character::is_numeric(L"100", 3));
        CHECK(characters::is_character::is_numeric(L"$1", 2));
        CHECK(characters::is_character::is_numeric(L"0", 1));
        CHECK(characters::is_character::is_numeric(L"100g", 4));
        CHECK(characters::is_character::is_numeric(L"10gg", 4));
        CHECK(characters::is_character::is_numeric(L"1ggg", 4) == false);
        CHECK(characters::is_character::is_numeric(nullptr, 3) == false);
        CHECK(characters::is_character::is_numeric(L"100", 0) == false);
        CHECK(characters::is_character::is_numeric(L"", 0) == false);
        }
    SECTION("Numeric String With Dashes")
        {
        CHECK(characters::is_character::is_numeric(L"-4", 2));
        CHECK(characters::is_character::is_numeric(L"-42", 3));
        CHECK(characters::is_character::is_numeric(L"42￠", 3));
        CHECK(characters::is_character::is_numeric(L"￡42", 3));
        CHECK(characters::is_character::is_numeric(L"－42", 3));
        CHECK(characters::is_character::is_numeric(L"＋42", 3));
        CHECK(characters::is_character::is_numeric(L"-a", 2) == false);
        CHECK(characters::is_character::is_numeric(L"-year", 5) == false);
        CHECK(characters::is_character::is_numeric(L"110-a", 5));//an equation
        CHECK(characters::is_character::is_numeric(L"10000-year", 10) == false);//this should be a regular word
        CHECK(characters::is_character::is_numeric(L"10000－yr", 8) == false);//this should be a regular word
        CHECK(characters::is_character::is_numeric(L"10000-year", 10) == false);//this should be a regular word
        }
    SECTION("Numeric")
        {
        CHECK(characters::is_character::is_numeric_simple(L'0'));
        CHECK(characters::is_character::is_numeric_simple(L'1'));
        CHECK(characters::is_character::is_numeric_simple(L'2'));
        CHECK(characters::is_character::is_numeric_simple(L'3'));
        CHECK(characters::is_character::is_numeric_simple(L'4'));
        CHECK(characters::is_character::is_numeric_simple(L'5'));
        CHECK(characters::is_character::is_numeric_simple(L'6'));
        CHECK(characters::is_character::is_numeric_simple(L'7'));
        CHECK(characters::is_character::is_numeric_simple(L'8'));
        CHECK(characters::is_character::is_numeric_simple(L'9'));
        // these shouldn't be recognized by simple interface
        CHECK(characters::is_character::is_numeric_simple(common_lang_constants::NUMBER_0_FULL_WIDTH));
        CHECK(characters::is_character::is_numeric_simple(common_lang_constants::NUMBER_9_FULL_WIDTH));
        CHECK(characters::is_character::is_numeric(L'0'));
        CHECK(characters::is_character::is_numeric(L'1'));
        CHECK(characters::is_character::is_numeric(L'2'));
        CHECK(characters::is_character::is_numeric(L'3'));
        CHECK(characters::is_character::is_numeric(L'4'));
        CHECK(characters::is_character::is_numeric(L'5'));
        CHECK(characters::is_character::is_numeric(L'6'));
        CHECK(characters::is_character::is_numeric(L'7'));
        CHECK(characters::is_character::is_numeric(L'8'));
        CHECK(characters::is_character::is_numeric(L'9'));
        CHECK(characters::is_character::is_numeric(common_lang_constants::NUMBER_0_FULL_WIDTH));
        CHECK(characters::is_character::is_numeric(common_lang_constants::NUMBER_9_FULL_WIDTH));
        CHECK(characters::is_character::is_numeric(L'²'));
        CHECK(characters::is_character::is_numeric(L'³'));
        CHECK(characters::is_character::is_numeric(L'¹'));
        CHECK(characters::is_character::is_numeric(L'¼'));
        CHECK(characters::is_character::is_numeric(L'½'));
        CHECK(characters::is_character::is_numeric(L'¾'));
        CHECK(characters::is_character::is_numeric(L'a') == false);
        CHECK(characters::is_character::can_character_end_word(L'￠'));
        CHECK(characters::is_character::can_character_begin_word(L'＄'));
        CHECK(characters::is_character::can_character_begin_word(L'￡'));
        CHECK(characters::is_character::can_character_begin_word(L'＃'));
        // can't start word, but prefixes numbers
        CHECK(characters::is_character::can_character_prefix_numeral(L'＋'));
        CHECK(characters::is_character::can_character_prefix_numeral(L'－'));
        }
    SECTION("Consanents")
        {
        CHECK(characters::is_character::is_consonant(L'a') == false);
        CHECK(characters::is_character::is_consonant(L'b'));
        CHECK(characters::is_character::is_consonant(L'c'));
        CHECK(characters::is_character::is_consonant(L'd'));
        CHECK(characters::is_character::is_consonant(L'e') == false);
        CHECK(characters::is_character::is_consonant(L'r'));
        CHECK(characters::is_character::is_consonant(L'g'));
        CHECK(characters::is_character::is_consonant(L'h'));
        CHECK(characters::is_character::is_consonant(L'i') == false);
        CHECK(characters::is_character::is_consonant(L'j'));
        CHECK(characters::is_character::is_consonant(L'k'));
        CHECK(characters::is_character::is_consonant(L'l'));
        CHECK(characters::is_character::is_consonant(L'm'));
        CHECK(characters::is_character::is_consonant(L'n'));
        CHECK(characters::is_character::is_consonant(L'o') == false);
        CHECK(characters::is_character::is_consonant(L'p'));
        CHECK(characters::is_character::is_consonant(L'q'));
        CHECK(characters::is_character::is_consonant(L'r'));
        CHECK(characters::is_character::is_consonant(L's'));
        CHECK(characters::is_character::is_consonant(L't'));
        CHECK(characters::is_character::is_consonant(L'u') == false);
        CHECK(characters::is_character::is_consonant(L'v'));
        CHECK(characters::is_character::is_consonant(L'w'));
        CHECK(characters::is_character::is_consonant(L'x'));
        CHECK(characters::is_character::is_consonant(L'z'));
        CHECK(characters::is_character::is_consonant(L'A') == false);
        CHECK(characters::is_character::is_consonant(L'B'));
        CHECK(characters::is_character::is_consonant(L'C'));
        CHECK(characters::is_character::is_consonant(L'D'));
        CHECK(characters::is_character::is_consonant(L'E') == false);
        CHECK(characters::is_character::is_consonant(L'F'));
        CHECK(characters::is_character::is_consonant(L'G'));
        CHECK(characters::is_character::is_consonant(L'H'));
        CHECK(characters::is_character::is_consonant(L'I') == false);
        CHECK(characters::is_character::is_consonant(L'J'));
        CHECK(characters::is_character::is_consonant(L'K'));
        CHECK(characters::is_character::is_consonant(L'L'));
        CHECK(characters::is_character::is_consonant(L'M'));
        CHECK(characters::is_character::is_consonant(L'N'));
        CHECK(characters::is_character::is_consonant(L'O') == false);
        CHECK(characters::is_character::is_consonant(L'P'));
        CHECK(characters::is_character::is_consonant(L'Q'));
        CHECK(characters::is_character::is_consonant(L'R'));
        CHECK(characters::is_character::is_consonant(L'S'));
        CHECK(characters::is_character::is_consonant(L'T'));
        CHECK(characters::is_character::is_consonant(L'U') == false);
        CHECK(characters::is_character::is_consonant(L'V'));
        CHECK(characters::is_character::is_consonant(L'W'));
        CHECK(characters::is_character::is_consonant(L'X'));
        CHECK(characters::is_character::is_consonant(L'Z'));
        CHECK(characters::is_character::is_consonant(L'1') == false);
        CHECK(characters::is_character::is_consonant(L'\"') == false);
        CHECK(characters::is_character::is_consonant(L'Ç'));
        CHECK(characters::is_character::is_consonant(L'ß'));
        CHECK(characters::is_character::is_consonant(L'Ä') == false);
        }
    SECTION("Lower Consonants")
        {
        CHECK(characters::is_character::is_lower_consonant(L'a') == false);
        CHECK(characters::is_character::is_lower_consonant(L'b'));
        CHECK(characters::is_character::is_lower_consonant(L'c'));
        CHECK(characters::is_character::is_lower_consonant(L'd'));
        CHECK(characters::is_character::is_lower_consonant(L'e') == false);
        CHECK(characters::is_character::is_lower_consonant(L'r'));
        CHECK(characters::is_character::is_lower_consonant(L'g'));
        CHECK(characters::is_character::is_lower_consonant(L'h'));
        CHECK(characters::is_character::is_lower_consonant(L'i') == false);
        CHECK(characters::is_character::is_lower_consonant(L'j'));
        CHECK(characters::is_character::is_lower_consonant(L'k'));
        CHECK(characters::is_character::is_lower_consonant(L'l'));
        CHECK(characters::is_character::is_lower_consonant(L'm'));
        CHECK(characters::is_character::is_lower_consonant(L'n'));
        CHECK(characters::is_character::is_lower_consonant(L'o') == false);
        CHECK(characters::is_character::is_lower_consonant(L'p'));
        CHECK(characters::is_character::is_lower_consonant(L'q'));
        CHECK(characters::is_character::is_lower_consonant(L'r'));
        CHECK(characters::is_character::is_lower_consonant(L's'));
        CHECK(characters::is_character::is_lower_consonant(L't'));
        CHECK(characters::is_character::is_lower_consonant(L'u') == false);
        CHECK(characters::is_character::is_lower_consonant(L'v'));
        CHECK(characters::is_character::is_lower_consonant(L'w'));
        CHECK(characters::is_character::is_lower_consonant(L'x'));
        CHECK(characters::is_character::is_lower_consonant(L'z'));
        CHECK(characters::is_character::is_lower_consonant(L'ß'));
        CHECK(characters::is_character::is_lower_consonant(L'A') == false);
        CHECK(characters::is_character::is_lower_consonant(L'B') == false);
        CHECK(characters::is_character::is_lower_consonant(L'C') == false);
        CHECK(characters::is_character::is_lower_consonant(L'D') == false);
        CHECK(characters::is_character::is_lower_consonant(L'E') == false);
        CHECK(characters::is_character::is_lower_consonant(L'F') == false);
        CHECK(characters::is_character::is_lower_consonant(L'G') == false);
        CHECK(characters::is_character::is_lower_consonant(L'H') == false);
        CHECK(characters::is_character::is_lower_consonant(L'I') == false);
        CHECK(characters::is_character::is_lower_consonant(L'J') == false);
        CHECK(characters::is_character::is_lower_consonant(L'K') == false);
        CHECK(characters::is_character::is_lower_consonant(L'L') == false);
        CHECK(characters::is_character::is_lower_consonant(L'M') == false);
        CHECK(characters::is_character::is_lower_consonant(L'N') == false);
        CHECK(characters::is_character::is_lower_consonant(L'O') == false);
        CHECK(characters::is_character::is_lower_consonant(L'P') == false);
        CHECK(characters::is_character::is_lower_consonant(L'Q') == false);
        CHECK(characters::is_character::is_lower_consonant(L'R') == false);
        CHECK(characters::is_character::is_lower_consonant(L'S') == false);
        CHECK(characters::is_character::is_lower_consonant(L'T') == false);
        CHECK(characters::is_character::is_lower_consonant(L'U') == false);
        CHECK(characters::is_character::is_lower_consonant(L'V') == false);
        CHECK(characters::is_character::is_lower_consonant(L'W') == false);
        CHECK(characters::is_character::is_lower_consonant(L'X') == false);
        CHECK(characters::is_character::is_lower_consonant(L'Z') == false);
        CHECK(characters::is_character::is_lower_consonant(L'1') == false);
        CHECK(characters::is_character::is_lower_consonant(L'\"') == false);
        CHECK(characters::is_character::is_lower_consonant(L'Ç') == false);

        CHECK(characters::is_character::is_lower_consonant(L'Ä') == false);
        }
    SECTION("Is Quote")
        {
        CHECK(characters::is_character::is_quote(L'a') == false);
        CHECK(characters::is_character::is_quote(L'1') == false);
        CHECK(characters::is_character::is_quote(L',') == false);
        CHECK(characters::is_character::is_quote(L'.') == false);
        CHECK(characters::is_character::is_quote(L'\"'));
        CHECK(characters::is_character::is_quote(L'\''));
        CHECK(characters::is_character::is_quote(L'‚'));
        CHECK(characters::is_character::is_quote(L'„'));
        CHECK(characters::is_character::is_quote(L'‹'));
        CHECK(characters::is_character::is_quote(L'›'));
        CHECK(characters::is_character::is_quote(L'«'));
        CHECK(characters::is_character::is_quote(L'»'));
        CHECK(characters::is_character::is_quote(0x201A));
        CHECK(characters::is_character::is_quote(0x201E));
        CHECK(characters::is_character::is_quote(0x2039));
        CHECK(characters::is_character::is_quote(0x203A));
        CHECK(characters::is_character::is_quote(130));
        CHECK(characters::is_character::is_quote(132));
        CHECK(characters::is_character::is_quote(139));
        CHECK(characters::is_character::is_quote(155));
        CHECK(characters::is_character::is_quote(171));
        CHECK(characters::is_character::is_quote(187));
        CHECK(characters::is_character::is_quote(L'‟'));
        CHECK(characters::is_character::is_quote(0x201F));
        }

    SECTION("Is Single Quote")
        {
        CHECK(characters::is_character::is_single_quote(L'a') == false);
        CHECK(characters::is_character::is_single_quote(L'1') == false);
        CHECK(characters::is_character::is_single_quote(L',') == false);
        CHECK(characters::is_character::is_single_quote(L'.') == false);
        CHECK(characters::is_character::is_single_quote(L'\"') == false);
        CHECK(characters::is_character::is_single_quote(L'\''));
        CHECK(characters::is_character::is_single_quote(L'‚'));
        CHECK(characters::is_character::is_single_quote(L'„') == false);
        CHECK(characters::is_character::is_single_quote(L'‹'));
        CHECK(characters::is_character::is_single_quote(L'›'));
        CHECK(characters::is_character::is_single_quote(L'«') == false);
        CHECK(characters::is_character::is_single_quote(L'»') == false);
        CHECK(characters::is_character::is_single_quote(0x201A));
        CHECK(characters::is_character::is_single_quote(0x201C) == false);
        CHECK(characters::is_character::is_single_quote(0x201E) == false);
        CHECK(characters::is_character::is_single_quote(0x2039));
        CHECK(characters::is_character::is_single_quote(0x203A));
        CHECK(characters::is_character::is_single_quote(130));
        CHECK(characters::is_character::is_single_quote(132) == false);
        CHECK(characters::is_character::is_single_quote(139));
        CHECK(characters::is_character::is_single_quote(155));
        CHECK(characters::is_character::is_single_quote(171) == false);
        CHECK(characters::is_character::is_single_quote(187) == false);
        CHECK(characters::is_character::is_single_quote(L'‟') == false);
        CHECK(characters::is_character::is_single_quote(0x201F) == false);
        CHECK(characters::is_character::is_single_quote(0x300D));
        CHECK(characters::is_character::is_single_quote(0x300E) == false);
        }

    SECTION("Is Double Quote")
        {
        CHECK(characters::is_character::is_double_quote(L'a') == false);
        CHECK(characters::is_character::is_double_quote(L'1') == false);
        CHECK(characters::is_character::is_double_quote(L',') == false);
        CHECK(characters::is_character::is_double_quote(L'.') == false);
        CHECK(characters::is_character::is_double_quote(L'\"'));
        CHECK(characters::is_character::is_double_quote(L'\'') == false);
        CHECK(characters::is_character::is_double_quote(L'‚') == false);
        CHECK(characters::is_character::is_double_quote(L'„'));
        CHECK(characters::is_character::is_double_quote(L'‹') == false);
        CHECK(characters::is_character::is_double_quote(L'›') == false);
        CHECK(characters::is_character::is_double_quote(L'«'));
        CHECK(characters::is_character::is_double_quote(L'»'));
        CHECK(characters::is_character::is_double_quote(0x201A) == false);
        CHECK(characters::is_character::is_double_quote(0x201C));
        CHECK(characters::is_character::is_double_quote(0x201E));
        CHECK(characters::is_character::is_double_quote(0x2039) == false);
        CHECK(characters::is_character::is_double_quote(0x203A) == false);
        CHECK(characters::is_character::is_double_quote(130) == false);
        CHECK(characters::is_character::is_double_quote(132));
        CHECK(characters::is_character::is_double_quote(139) == false);
        CHECK(characters::is_character::is_double_quote(155) == false);
        CHECK(characters::is_character::is_double_quote(171));
        CHECK(characters::is_character::is_double_quote(187));
        CHECK(characters::is_character::is_double_quote(L'‟'));
        CHECK(characters::is_character::is_double_quote(0x201F));
        CHECK(characters::is_character::is_double_quote(0x300D) == false);
        CHECK(characters::is_character::is_double_quote(0x300E));
        }
    }
