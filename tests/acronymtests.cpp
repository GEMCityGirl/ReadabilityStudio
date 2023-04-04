#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>
#include "../src/indexing/word_collection.h"
#include "../src/indexing/word.h"

using namespace grammar;

using MYWORD = word<traits::case_insensitive_ex,
    stemming::english_stem<std::basic_string<wchar_t, traits::case_insensitive_ex> > >;

extern word_list Stop_list;

TEST_CASE("Acronyms", "[acronyms]")
    {
    grammar::english_syllabize ENsyllabizer;
    stemming::english_stem<std::wstring> ENStemmer;
    grammar::is_english_coordinating_conjunction is_conjunction;
    grammar::is_incorrect_english_article is_english_mismatched_article;
    grammar::phrase_collection pmap;
    grammar::phrase_collection copyrightPMap;
    grammar::phrase_collection citationPMap;
    grammar::phrase_collection excludedPMap;
    word_list Known_proper_nouns;
    word_list Known_personal_nouns;
    word_list Known_spellings;
    word_list Secondary_known_spellings;
    word_list Programming_known_spellings;

    SECTION("Acronym")
        {
        const wchar_t text[] = L"I live U.S. of A. right now.";
        grammar::is_end_of_sentence sent(false);
        CHECK_FALSE(sent(text, wcslen(text), 10, 7, 1));
        }
    SECTION("Acronym2")
        {
        is_acronym isAcronym;
        CHECK_FALSE(isAcronym(L"A", 1));
        CHECK_FALSE(isAcronym(L"A.", 2));
        CHECK_FALSE(isAcronym(L"86L", 3));
        CHECK_FALSE(isAcronym(L"Hello", 5));
        CHECK_FALSE(isAcronym(L"HeLlo", 5));
        CHECK(isAcronym(L"ID's", 4));
        CHECK(isAcronym(L"KAOS", 4));
        CHECK(isAcronym(L"UN", 2));
        CHECK(isAcronym(L"U.N.", 4));
        CHECK(isAcronym(L"UN", 3));
        CHECK_FALSE(isAcronym(L"Is", 2));
        CHECK_FALSE(isAcronym(L"iOS", 3));
        CHECK(isAcronym(L"IOs", 3));
        CHECK_FALSE(isAcronym(nullptr, 5));
        CHECK_FALSE(isAcronym(L"", 1));
        CHECK_FALSE(isAcronym(L"", 0));
        }
    SECTION("Acronym Dotted")
        {
        CHECK_FALSE(is_acronym::is_dotted_acronym(nullptr, 0));
        CHECK(is_acronym::is_dotted_acronym(L"N.A.R.C.", 8));
        CHECK_FALSE(is_acronym::is_dotted_acronym(L"image.bmp.", 10));
        CHECK_FALSE(is_acronym::is_dotted_acronym(L"MR.", 3));
        CHECK_FALSE(is_acronym::is_dotted_acronym(L"N.A", 3));
        CHECK_FALSE(is_acronym::is_dotted_acronym(L"NARC", 4));
        CHECK_FALSE(is_acronym::is_dotted_acronym(L"ETC...", 6));
        CHECK(is_acronym::is_dotted_acronym(L"U.N.", 4));
        CHECK_FALSE(is_acronym::is_dotted_acronym(L"n.d.).", 6));
        CHECK(is_acronym::is_dotted_acronym(L"F.A.K.K.2", 9));
        CHECK(is_acronym::is_dotted_acronym(L"F.A.K.K.²", 9));
        CHECK(is_acronym::is_dotted_acronym(L"F.A.K.K.21", 10));
        }
    SECTION("Acronym Dot Count")
        {
        is_acronym isAcronym;
        CHECK_FALSE(isAcronym(nullptr, 0));
        CHECK(isAcronym.get_dot_count() == 0);
        CHECK(isAcronym(L"N.A.R.C.", 8));
        CHECK(isAcronym.get_dot_count() == 4);
        CHECK_FALSE(isAcronym(L"image.bmp.", 10));
        // not an acronym, so dots don't get counted
        CHECK(isAcronym.get_dot_count() == 0);
        CHECK(isAcronym(L"MR.", 3));
        CHECK(isAcronym.get_dot_count() == 1);
        CHECK(isAcronym(L"N.A", 3));
        CHECK(isAcronym.get_dot_count() == 1);
        CHECK(isAcronym(L"NARC", 4));
        CHECK(isAcronym.get_dot_count() == 0);
        CHECK(isAcronym(L"ETC...", 6));
        CHECK(isAcronym.get_dot_count() == 3);
        CHECK(isAcronym(L"U.N.", 4));
        CHECK(isAcronym.get_dot_count() == 2);
        }
    SECTION("Acronym Ends With S")
        {
        is_acronym isAcronym;
        CHECK(isAcronym(L"N.A.R.C.s", 9));
        CHECK(isAcronym.ends_with_lower_s());
        CHECK(isAcronym(L"N.A.R.C.", 8));
        CHECK_FALSE(isAcronym.ends_with_lower_s());
        CHECK_FALSE(isAcronym(L"is", 2));
        CHECK_FALSE(isAcronym.ends_with_lower_s());
        }
    }
