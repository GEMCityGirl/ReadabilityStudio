#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>
#include "../src/indexing/word_list.h"
#include "../src/indexing/word.h"
#include "../src/readability/readability.h"

#define UNUSED_WORD_ARGS 0, 0, 0, false, false, false, false, 0, 0

using namespace Catch::Matchers;
using namespace lily_of_the_valley;

using MYWORD = word<traits::case_insensitive_ex,
    stemming::english_stem<std::basic_string<wchar_t, traits::case_insensitive_ex> > >;

TEST_CASE("Familiar words", "[word-list]")
    {
    SECTION("Numeric")
        {
        word_list dcLite;
        dcLite.load_words(L"chocolate cake sugar one thirty trash-man", true, false);
        readability::is_familiar_word<MYWORD, word_list, stemming::no_op_stem<MYWORD> > is_fam(&dcLite, readability::proper_noun_counting_method::all_proper_nouns_are_familiar, false);
        MYWORD numberWord(L"7.2", 3, UNUSED_WORD_ARGS);
        numberWord.set_numeric(true);

        is_fam.include_numeric_as_familiar(false);
        CHECK(is_fam(numberWord) == false);
        is_fam.include_numeric_as_familiar(true);
        CHECK(is_fam(numberWord));
        }
    SECTION("Proper Noun")
        {
        word_list dcLite;
        dcLite.load_words(L"chocolate cake sugar one thirty trash-man", true, false);
        readability::is_familiar_word<MYWORD, word_list, stemming::no_op_stem<MYWORD> > is_fam(&dcLite, readability::proper_noun_counting_method::all_proper_nouns_are_familiar, false);
        MYWORD numberWord(L"Jimbo", 5, UNUSED_WORD_ARGS);
        numberWord.set_proper_noun(true);

        is_fam.set_proper_noun_method(readability::proper_noun_counting_method::all_proper_nouns_are_unfamiliar);
        CHECK(is_fam(numberWord) == false);

        is_fam.set_proper_noun_method(readability::proper_noun_counting_method::only_count_first_instance_of_proper_noun_as_unfamiliar);
        CHECK(is_fam(numberWord) == false);//first instance unfamiliar
        CHECK(is_fam(numberWord));
        CHECK(is_fam(numberWord));
        is_fam.clear_encountered_proper_nouns();
        CHECK(is_fam(numberWord) == false);//first instance unfamiliar
        CHECK(is_fam(numberWord));

        is_fam.set_proper_noun_method(readability::proper_noun_counting_method::all_proper_nouns_are_familiar);
        CHECK(is_fam(numberWord));
        }
    SECTION("Hyphen Word")
        {
        word_list dcLite;
        dcLite.load_words(L"chocolate cake sugar one thirty trash-man", true, false);
        readability::is_familiar_word<MYWORD, word_list, stemming::no_op_stem<MYWORD> > is_fam(&dcLite, readability::proper_noun_counting_method::all_proper_nouns_are_familiar, false);
        CHECK(is_fam(MYWORD(L"one-thirty", 10, UNUSED_WORD_ARGS)));
        CHECK(is_fam(MYWORD(L"one-thirteen", 12, UNUSED_WORD_ARGS)) == false);
        CHECK(is_fam(MYWORD(L"uno-thirty", 10, UNUSED_WORD_ARGS)) == false);
        CHECK(is_fam(MYWORD(L"-", 1, UNUSED_WORD_ARGS)) == false);
        CHECK(is_fam(MYWORD(L"--", 2, UNUSED_WORD_ARGS)) == false);
        CHECK(is_fam(MYWORD(L"one-", 4, UNUSED_WORD_ARGS)));
        CHECK(is_fam(MYWORD(L"-one", 4, UNUSED_WORD_ARGS)));
        CHECK(is_fam(MYWORD(L"chocolate-cake-sugar", 20, UNUSED_WORD_ARGS)));
        CHECK(is_fam(MYWORD(L"trash-man", 9, UNUSED_WORD_ARGS)));
        CHECK(is_fam(MYWORD(L"trash", 5, UNUSED_WORD_ARGS)) == false);
        }
    SECTION("Hyphen Word With Stemming")
        {
        word_list dcLite;
        dcLite.load_words(L"document depart", true, false);
        readability::is_familiar_word<MYWORD, word_list,
            stemming::english_stem<MYWORD>>
            is_fam(&dcLite, readability::proper_noun_counting_method::all_proper_nouns_are_familiar, false);
        CHECK(is_fam(MYWORD(L"document", 8, UNUSED_WORD_ARGS)));
        CHECK(is_fam(MYWORD(L"documents", 9, UNUSED_WORD_ARGS)));
        CHECK(is_fam(MYWORD(L"documentation-department", 24, UNUSED_WORD_ARGS)));
        CHECK(is_fam(MYWORD(L"documentation-departments", 25, UNUSED_WORD_ARGS)));
        CHECK(is_fam(MYWORD(L"documentation-depo", 18, UNUSED_WORD_ARGS)) == false);
        }
    }

TEST_CASE("Word lists", "[word-list]")
    {
    SECTION("Null")
        {
        word_list_with_replacements WL;
        WL.load_words(nullptr, false);
        CHECK(WL.get_list_size() == 0);
        }
    SECTION("Empty(")
        {
        word_list_with_replacements WL;
        WL.load_words(L"", false);
        CHECK(WL.get_list_size() == 0);
        }
    SECTION("Loading")
        {
        word_list_with_replacements WL;
        WL.load_words(L"simple\teasy\ndifficult\thard", false);
        CHECK(WL.get_list_size() == 2);
        CHECK(WL.get_words().begin()->first == L"difficult");
        CHECK(WL.get_words().begin()->second == L"hard");
        CHECK((++WL.get_words().begin())->first == L"simple");
        CHECK((++WL.get_words().begin())->second == L"easy");
        }
    SECTION("Preserve")
        {
        word_list_with_replacements WL;
        WL.load_words(L"simple\teasy\ndifficult\thard", false);
        CHECK(WL.get_list_size() == 2);
        CHECK(WL.get_words().begin()->first == L"difficult");
        CHECK(WL.get_words().begin()->second == L"hard");
        CHECK((++WL.get_words().begin())->first == L"simple");
        CHECK((++WL.get_words().begin())->second == L"easy");
        WL.load_words(L"ask\ttell", true);
        CHECK(WL.get_list_size() == 3);
        CHECK(WL.get_words().begin()->first == L"ask");
        CHECK(WL.get_words().begin()->second == L"tell");
        CHECK((++WL.get_words().begin())->first == L"difficult");
        CHECK((++WL.get_words().begin())->second == L"hard");
        CHECK((++(++WL.get_words().begin()))->first == L"simple");
        CHECK((++(++WL.get_words().begin()))->second == L"easy");
        WL.load_words(L"ask\ttell", false);
        CHECK(WL.get_list_size() == 1);
        CHECK(WL.get_words().begin()->first == L"ask");
        CHECK(WL.get_words().begin()->second == L"tell");
        }
    SECTION("Find")
        {
        word_list_with_replacements WL;
        WL.load_words(L"simple\teasy\ndifficult\thard", false);
        CHECK(WL.find(L"difficult").first == true);
        CHECK(WL.find(L"difficult").second == L"hard");
        CHECK(WL.find(L"simple").first == true);
        CHECK(WL.find(L"simple").second == L"easy");
        CHECK(WL.find(L"easy").first == false);
        CHECK(WL.find(L"easy").second == L"");
        }

    SECTION("WL Null")
        {
        word_list WL;
        WL.load_words(nullptr, false, false);
        CHECK(WL.get_list_size() == 0);
        }
    SECTION("WL Empty")
        {
        word_list WL;
        WL.load_words(L"", false, false);
        CHECK(WL.get_list_size() == 0);
        // just delimiters, but should pick up empty tokens after delims
        WL.load_words(L" ", false, false);
        CHECK(WL.get_list_size() == 1);
        WL.load_words(L" \t", false, false);
        // skips empty tokens between delims
        CHECK(WL.get_list_size() == 1);
        }
    SECTION("WL NoSort")
        {
        word_list WL;
        WL.load_words(L"the\na\nby\ndo", false, false);
        CHECK(WL.get_list_size() == 4);
        CHECK(WL.get_words().at(0) == L"the");
        CHECK(WL.get_words().at(1) == L"a");
        CHECK(WL.get_words().at(2) == L"by");
        CHECK(WL.get_words().at(3) == L"do");
        CHECK(WL.is_sorted() == false);
        }
    SECTION("WL Sort")
        {
        word_list WL;
        WL.load_words(L"the\na\nby\ndo", true, false);
        CHECK(WL.get_list_size() == 4);
        CHECK(WL.get_words().at(0) == L"a");
        CHECK(WL.get_words().at(1) == L"by");
        CHECK(WL.get_words().at(2) == L"do");
        CHECK(WL.get_words().at(3) == L"the");
        CHECK(WL.is_sorted());
        }
    SECTION("WL NoPreserve")
        {
        word_list WL;
        WL.load_words(L"the\na\nby\ndo", false, false);
        CHECK(WL.get_list_size() == 4);
        CHECK(WL.get_words().at(0) == L"the");
        CHECK(WL.get_words().at(1) == L"a");
        CHECK(WL.get_words().at(2) == L"by");
        CHECK(WL.get_words().at(3) == L"do");
        WL.load_words(L"words\nmore", false, false);
        CHECK(WL.get_list_size() == 2);
        CHECK(WL.get_words().at(0) == L"words");
        CHECK(WL.get_words().at(1) == L"more");
        CHECK(WL.is_sorted() == false);
        }
    SECTION("WL Preserve")
        {
        word_list WL;
        WL.load_words(L"the\na\nby\ndo", false, false);
        CHECK(WL.get_list_size() == 4);
        CHECK(WL.get_words().at(0) == L"the");
        CHECK(WL.get_words().at(1) == L"a");
        CHECK(WL.get_words().at(2) == L"by");
        CHECK(WL.get_words().at(3) == L"do");
        WL.load_words(L"words\nmore", false, true);
        CHECK(WL.get_list_size() == 6);
        CHECK(WL.get_words().at(0) == L"the");
        CHECK(WL.get_words().at(1) == L"a");
        CHECK(WL.get_words().at(2) == L"by");
        CHECK(WL.get_words().at(3) == L"do");
        CHECK(WL.get_words().at(4) == L"words");
        CHECK(WL.get_words().at(5) == L"more");
        CHECK(WL.is_sorted() == false);
        }
    SECTION("WL Preserve And Sort")
        {
        word_list WL;
        WL.load_words(L"the\na\nby\ndo", false, false);
        CHECK(WL.get_list_size() == 4);
        CHECK(WL.get_words().at(0) == L"the");
        CHECK(WL.get_words().at(1) == L"a");
        CHECK(WL.get_words().at(2) == L"by");
        CHECK(WL.get_words().at(3) == L"do");
        WL.load_words(L"words\nmore", true, true);
        CHECK(WL.get_list_size() == 6);
        CHECK(WL.get_words().at(0) == L"a");
        CHECK(WL.get_words().at(1) == L"by");
        CHECK(WL.get_words().at(2) == L"do");
        CHECK(WL.get_words().at(3) == L"more");
        CHECK(WL.get_words().at(4) == L"the");
        CHECK(WL.get_words().at(5) == L"words");
        CHECK(WL.is_sorted());
        WL.clear();
        CHECK(WL.get_list_size() == 0);
        }
    SECTION("WL Add Word")
        {
        word_list WL;
        WL.load_words(L"the\na\nby\ndo", true, false);
        CHECK(WL.get_list_size() == 4);
        CHECK(WL.get_words().at(0) == L"a");
        CHECK(WL.get_words().at(1) == L"by");
        CHECK(WL.get_words().at(2) == L"do");
        CHECK(WL.get_words().at(3) == L"the");
        WL.add_word(L"words");
        CHECK(WL.get_list_size() == 5);
        WL.add_word(L"more");
        CHECK(WL.get_list_size() == 6);
        CHECK(WL.get_words().at(0) == L"a");
        CHECK(WL.get_words().at(1) == L"by");
        CHECK(WL.get_words().at(2) == L"do");
        CHECK(WL.get_words().at(3) == L"more");
        CHECK(WL.get_words().at(4) == L"the");
        CHECK(WL.get_words().at(5) == L"words");
        CHECK(WL.is_sorted());
        }
    SECTION("WL Add Words")
        {
        word_list WL;
        WL.load_words(L"the\na\nby\ndo", true, false);
        CHECK(WL.get_list_size() == 4);
        CHECK(WL.get_words().at(0) == L"a");
        CHECK(WL.get_words().at(1) == L"by");
        CHECK(WL.get_words().at(2) == L"do");
        CHECK(WL.get_words().at(3) == L"the");
        std::vector<word_list::word_type> newWords;
        WL.add_words(newWords);
        // should have no effect
        CHECK(WL.get_list_size() == 4);
        newWords.push_back(L"words");
        newWords.push_back(L"more");
        WL.add_words(newWords);
        CHECK(WL.get_list_size() == 6);
        CHECK(WL.get_words().at(0) == L"a");
        CHECK(WL.get_words().at(1) == L"by");
        CHECK(WL.get_words().at(2) == L"do");
        CHECK(WL.get_words().at(3) == L"more");
        CHECK(WL.get_words().at(4) == L"the");
        CHECK(WL.get_words().at(5) == L"words");
        CHECK(WL.is_sorted());
        }
    SECTION("WL Find")
        {
        word_list WL;
        WL.load_words(L"the\na\nby\ndo", true, false);
        CHECK(WL.find(L"a"));
        CHECK(WL.find(L"By"));
        CHECK(WL.find(L"DO"));
        CHECK(WL.find(L"tHe"));
        CHECK(WL.find(L"d") == false);
        CHECK(WL.find(L"then") == false);
        }
    SECTION("WL Remove Duplicates")
        {
        word_list WL;
        WL.load_words(L"the\na\nby\ndo\nthe\na", true, false);
        CHECK(WL.get_list_size() == 6);
        CHECK(WL.get_words().at(0) == L"a");
        CHECK(WL.get_words().at(1) == L"a");
        CHECK(WL.get_words().at(2) == L"by");
        CHECK(WL.get_words().at(3) == L"do");
        CHECK(WL.get_words().at(4) == L"the");
        CHECK(WL.get_words().at(5) == L"the");
        WL.remove_duplicates();
        CHECK(WL.get_words().at(0) == L"a");
        CHECK(WL.get_words().at(1) == L"by");
        CHECK(WL.get_words().at(2) == L"do");
        CHECK(WL.get_words().at(3) == L"the");
        }
    }
