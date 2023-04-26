#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>
#include "../src/indexing/word.h"
#include "../src/indexing/punctuation.h"

using namespace Catch::Matchers;
using namespace grammar;

TEST_CASE("Word", "[word]")
    {
    SECTION("Proper Noun Flag")
        {
        // version with stemmer
        word<traits::case_insensitive_ex, stemming::english_stem<std::basic_string<wchar_t,traits::case_insensitive_ex >  > > myWord(L"hello", 5, 0, 0, 0, false, true, false, false, 0, 0);
        CHECK(myWord.is_valid() == true);
        CHECK(myWord.is_numeric() == false);
        CHECK(myWord.is_proper_noun() == false);
        myWord.set_proper_noun(false);
        CHECK(myWord.is_valid() == true);
        CHECK(myWord.is_numeric() == false);
        CHECK(myWord.is_proper_noun() == false);
        myWord.set_proper_noun(true);
        CHECK(myWord.is_valid() == true);
        CHECK(myWord.is_numeric() == false);
        CHECK(myWord.is_proper_noun() == true);

        word<traits::case_insensitive_ex, stemming::english_stem<std::basic_string<wchar_t,traits::case_insensitive_ex >  > > myWord2(L"1974", 4, 0, 0, 0, true, true, false, false, 0, 0);
        CHECK(myWord2.is_valid() == true);
        CHECK(myWord2.is_numeric() == true);
        CHECK(myWord2.is_proper_noun() == false);

        myWord = myWord2;
        CHECK(myWord.is_valid() == true);
        CHECK(myWord.is_numeric() == true);
        CHECK(myWord.is_proper_noun() == false);

        // version without stemmer
        word<traits::case_insensitive_ex, stemming::no_op_stem<std::basic_string<wchar_t,traits::case_insensitive_ex > > > myWord6(L"hello", 5, 0, 0, 0, false, true, false, false, 0, 0);
        CHECK(myWord6.is_valid() == true);
        CHECK(myWord6.is_numeric() == false);
        CHECK(myWord6.is_proper_noun() == false);
        myWord6.set_proper_noun(false);
        CHECK(myWord6.is_valid() == true);
        CHECK(myWord6.is_numeric() == false);
        CHECK(myWord6.is_proper_noun() == false);
        myWord6.set_proper_noun(true);
        CHECK(myWord6.is_valid() == true);
        CHECK(myWord6.is_numeric() == false);
        CHECK(myWord6.is_proper_noun() == true);

        word<traits::case_insensitive_ex, stemming::no_op_stem<std::basic_string<wchar_t,traits::case_insensitive_ex > > > myWord9;
        CHECK(myWord9.is_valid() == false);
        CHECK(myWord9.is_numeric() == false);
        CHECK(myWord9.is_proper_noun() == false);

        myWord6 = myWord9;
        CHECK(myWord6.is_valid() == false);
        CHECK(myWord6.is_numeric() == false);
        CHECK(myWord6.is_proper_noun() == false);
        }
    SECTION("Numeric Flag")
        {
        // version with stemmer
        word<traits::case_insensitive_ex, stemming::english_stem<std::basic_string<wchar_t,traits::case_insensitive_ex >  > > myWord(L"hello", 5, 0, 0, 0, false, true, false, false, 0, 0);
        CHECK(myWord.is_numeric() == false);
        CHECK(myWord.is_proper_noun() == false);

        word<traits::case_insensitive_ex, stemming::english_stem<std::basic_string<wchar_t,traits::case_insensitive_ex >  > > myWord2(L"1974", 4, 0, 0, 0, true, true, false, false, 0, 0);
        CHECK(myWord2.is_numeric() == true);
        CHECK(myWord2.is_proper_noun() == false);

        word<traits::case_insensitive_ex, stemming::english_stem<std::basic_string<wchar_t,traits::case_insensitive_ex >  > > myWord3(L"hello", 5, 0, 0, 0, false, false, false, false, 0, 0);
        CHECK(myWord3.is_numeric() == false);
        CHECK(myWord3.is_proper_noun() == false);

        word<traits::case_insensitive_ex, stemming::english_stem<std::basic_string<wchar_t,traits::case_insensitive_ex >  > > myWord4;
        CHECK(myWord4.is_numeric() == false);
        CHECK(myWord4.is_proper_noun() == false);

        myWord4 = myWord2;
        CHECK(myWord4.is_numeric() == true);
        CHECK(myWord4.is_proper_noun() == false);

        word<traits::case_insensitive_ex, stemming::english_stem<std::basic_string<wchar_t,traits::case_insensitive_ex >  > > myWord5(myWord2);
        CHECK(myWord5.is_numeric() == true);
        CHECK(myWord5.is_proper_noun() == false);

        // version without stemmer
        word<traits::case_insensitive_ex, stemming::no_op_stem<std::basic_string<wchar_t,traits::case_insensitive_ex  > > > myWord6(L"hello", 5, 0, 0, 0, false, true, false, false, 0, 0);
        CHECK(myWord6.is_numeric() == false);
        CHECK(myWord6.is_proper_noun() == false);

        word<traits::case_insensitive_ex, stemming::no_op_stem<std::basic_string<wchar_t,traits::case_insensitive_ex > > > myWord7(L"1974", 4, 0, 0, 0, true, true, false, false, 0, 0);
        CHECK(myWord7.is_numeric() == true);
        CHECK(myWord7.is_proper_noun() == false);

        word<traits::case_insensitive_ex, stemming::no_op_stem<std::basic_string<wchar_t,traits::case_insensitive_ex > > > myWord8(L"hello", 5, 0, 0, 0, false, false, false, false, 0, 0);
        CHECK(myWord8.is_numeric() == false);
        CHECK(myWord8.is_proper_noun() == false);

        word<traits::case_insensitive_ex, stemming::no_op_stem<std::basic_string<wchar_t,traits::case_insensitive_ex > > > myWord9;
        CHECK(myWord9.is_numeric() == false);
        CHECK(myWord9.is_proper_noun() == false);

        myWord9 = myWord7;
        CHECK(myWord9.is_numeric() == true);
        CHECK(myWord9.is_proper_noun() == false);

        word<traits::case_insensitive_ex, stemming::no_op_stem<std::basic_string<wchar_t,traits::case_insensitive_ex > > > myWord10(myWord7);
        CHECK(myWord10.is_numeric() == true);
        CHECK(myWord10.is_proper_noun() == false);
        }
    SECTION("Invalid Sentence Flag")
        {
        // version with stemmer
        word<traits::case_insensitive_ex, stemming::english_stem<std::basic_string<wchar_t,traits::case_insensitive_ex >  > > myWord(L"hello", 5, 0, 0, 0, false, true, false, false, 0, 0);
        CHECK(myWord.is_numeric() == false);
        CHECK(myWord.is_proper_noun() == false);
        myWord.set_valid(true);
        CHECK(myWord.is_valid() == true);
        CHECK(myWord.is_numeric() == false);
        CHECK(myWord.is_proper_noun() == false);
        myWord.set_valid(false);
        CHECK(myWord.is_valid() == false);
        CHECK(myWord.is_numeric() == false);
        CHECK(myWord.is_proper_noun() == false);
        myWord.set_numeric(true);
        CHECK(myWord.is_valid() == false);
        CHECK(myWord.is_numeric() == true);
        CHECK(myWord.is_proper_noun() == false);

        word<traits::case_insensitive_ex, stemming::english_stem<std::basic_string<wchar_t,traits::case_insensitive_ex >  > > myWord2(L"1974", 4, 0, 0, 0, true, true, false, false, 0, 0);
        CHECK(myWord2.is_numeric() == true);
        CHECK(myWord2.is_numeric() == true);
        CHECK(myWord2.is_proper_noun() == false);
        myWord2.set_valid(true);
        CHECK(myWord2.is_valid() == true);
        CHECK(myWord2.is_numeric() == true);
        CHECK(myWord2.is_proper_noun() == false);
        myWord2.set_valid(false);
        CHECK(myWord2.is_valid() == false);
        CHECK(myWord2.is_numeric() == true);
        CHECK(myWord2.is_proper_noun() == false);
        myWord2.set_numeric(false);
        CHECK(myWord2.is_valid() == false);
        CHECK(myWord2.is_numeric() == false);
        CHECK(myWord2.is_proper_noun() == false);

        // version without stemmer
        word<traits::case_insensitive_ex, stemming::no_op_stem<std::basic_string<wchar_t,traits::case_insensitive_ex > > > myWord6(L"hello", 5, 0, 0, 0, false, true, false, false, 0, 0);
        CHECK(myWord6.is_numeric() == false);
        CHECK(myWord6.is_proper_noun() == false);
        myWord6.set_valid(true);
        CHECK(myWord6.is_valid() == true);
        CHECK(myWord6.is_numeric() == false);
        CHECK(myWord6.is_proper_noun() == false);
        myWord6.set_valid(false);
        CHECK(myWord6.is_valid() == false);
        CHECK(myWord6.is_numeric() == false);
        CHECK(myWord6.is_proper_noun() == false);
        myWord6.set_numeric(true);
        CHECK(myWord6.is_valid() == false);
        CHECK(myWord6.is_numeric() == true);
        CHECK(myWord6.is_proper_noun() == false);

        word<traits::case_insensitive_ex, stemming::no_op_stem<std::basic_string<wchar_t,traits::case_insensitive_ex > > > myWord7(L"1974", 4, 0, 0, 0, true, true, false, false, 0, 0);
        CHECK(myWord7.is_numeric() == true);
        CHECK(myWord7.is_proper_noun() == false);
        myWord7.set_valid(true);
        CHECK(myWord7.is_valid() == true);
        CHECK(myWord7.is_numeric() == true);
        CHECK(myWord7.is_proper_noun() == false);
        myWord7.set_valid(false);
        CHECK(myWord7.is_valid() == false);
        CHECK(myWord7.is_numeric() == true);
        CHECK(myWord7.is_proper_noun() == false);
        myWord7.set_numeric(false);
        CHECK(myWord7.is_valid() == false);
        CHECK(myWord7.is_numeric() == false);
        CHECK(myWord7.is_proper_noun() == false);
        }
    SECTION("Punctuation Count")
        {
        english_syllabize mySllablizer;
        punctuation::punctuation_count count_punctuation;
        // version with stemmer
        word<traits::case_insensitive_ex, stemming::english_stem<std::basic_string<wchar_t,traits::case_insensitive_ex >  > > myWord(L"hello!", 6, 0, 0, 0, false, true, false, false, mySllablizer(L"hello!", 6), count_punctuation(L"hello!"));
        CHECK(myWord.get_punctuation_count() == 1);
        CHECK(myWord.length() == 6);
        CHECK(myWord.get_length_excluding_punctuation() == 5);
        CHECK(myWord.get_syllable_count() == 2);

        word<traits::case_insensitive_ex, stemming::english_stem<std::basic_string<wchar_t,traits::case_insensitive_ex >  > > myWord2(L"$5.00###", 8, 0, 0, 0, true, true, false, false, mySllablizer(L"$5.00###", 8), count_punctuation(L"$5.00###"));
        CHECK(myWord2.get_punctuation_count() == 5);
        CHECK(myWord2.length() == 8);
        CHECK(myWord2.get_length_excluding_punctuation() == 3);
        CHECK(myWord2.get_syllable_count() == 11);

        myWord = myWord2;
        CHECK(myWord.get_punctuation_count() == 5);
        CHECK(myWord.length() == 8);
        CHECK(myWord.get_length_excluding_punctuation() == 3);
        CHECK(myWord.get_syllable_count() == 11);

        word<traits::case_insensitive_ex, stemming::english_stem<std::basic_string<wchar_t,traits::case_insensitive_ex >  > > myWord2b(myWord2);
        CHECK(myWord2b.get_punctuation_count() == 5);
        CHECK(myWord2b.length() == 8);
        CHECK(myWord2b.get_length_excluding_punctuation() == 3);
        CHECK(myWord2b.get_syllable_count() == 11);

        // version without stemmer
        word<traits::case_insensitive_ex, stemming::no_op_stem<std::basic_string<wchar_t,traits::case_insensitive_ex > > > myWord3(L"hello!", 6, 0, 0, 0, false, true, false, false, mySllablizer(L"hello!", 6), count_punctuation(L"hello!"));
        CHECK(myWord3.get_punctuation_count() == 1);
        CHECK(myWord3.length() == 6);
        CHECK(myWord3.get_length_excluding_punctuation() == 5);
        CHECK(myWord3.get_syllable_count() == 2);

        word<traits::case_insensitive_ex, stemming::no_op_stem<std::basic_string<wchar_t,traits::case_insensitive_ex > > > myWord4(L"$5.00###", 8, 0, 0, 0, true, true, false, false, mySllablizer(L"$5.00###", 8), count_punctuation(L"$5.00###"));
        CHECK(myWord4.get_punctuation_count() == 5);
        CHECK(myWord4.length() == 8);
        CHECK(myWord4.get_length_excluding_punctuation() == 3);
        CHECK(myWord4.get_syllable_count() == 11);

        myWord3 = myWord4;
        CHECK(myWord3.get_punctuation_count() == 5);
        CHECK(myWord3.length() == 8);
        CHECK(myWord3.get_length_excluding_punctuation() == 3);
        CHECK(myWord3.get_syllable_count() == 11);

        word<traits::case_insensitive_ex, stemming::no_op_stem<std::basic_string<wchar_t,traits::case_insensitive_ex > > > myWord5(myWord4);
        CHECK(myWord5.get_punctuation_count() == 5);
        CHECK(myWord5.length() == 8);
        CHECK(myWord5.get_length_excluding_punctuation() == 3);
        CHECK(myWord5.get_syllable_count() == 11);
        }
    SECTION("Capitalized")
        {
        // version with stemmer
        word<traits::case_insensitive_ex, stemming::english_stem<std::basic_string<wchar_t,traits::case_insensitive_ex >  > > myWord(L"", 0, 0, 0, 0, false, true, false, false, 0, 0);
        CHECK(myWord.is_capitalized() == false);
        word<traits::case_insensitive_ex, stemming::english_stem<std::basic_string<wchar_t,traits::case_insensitive_ex >  > > myWord2(L"hello!", 6, 0, 0, 0, false, true, false, false, 0, 0);
        CHECK(myWord2.is_capitalized() == false);
        word<traits::case_insensitive_ex, stemming::english_stem<std::basic_string<wchar_t,traits::case_insensitive_ex >  > > myWord3(L"Hello!", 6, 0, 0, 0, false, true, false, false, 0, 0);
        CHECK(myWord3.is_capitalized());
        word<traits::case_insensitive_ex, stemming::english_stem<std::basic_string<wchar_t,traits::case_insensitive_ex >  > > myWord4(L"A", 1, 0, 0, 0, false, true, false, false, 0, 0);
        CHECK(myWord4.is_capitalized());
        word<traits::case_insensitive_ex, stemming::english_stem<std::basic_string<wchar_t,traits::case_insensitive_ex >  > > myWord5(L"A.", 2, 0, 0, 0, false, true, false, false, 0, 0);
        CHECK(myWord5.is_capitalized());

        // without stemmer
        word<traits::case_insensitive_ex, stemming::no_op_stem<std::basic_string<wchar_t,traits::case_insensitive_ex > > > myWord6(L"", 0, 0, 0, 0, false, true, false, false, 0, 0);
        CHECK(myWord6.is_capitalized() == false);
        word<traits::case_insensitive_ex, stemming::no_op_stem<std::basic_string<wchar_t,traits::case_insensitive_ex > > > myWord7(L"hello!", 6, 0, 0, 0, false, true, false, false, 0, 0);
        CHECK(myWord7.is_capitalized() == false);
        word<traits::case_insensitive_ex, stemming::no_op_stem<std::basic_string<wchar_t,traits::case_insensitive_ex > > > myWord8(L"Hello", 5, 0, 0, 0, false, true, false, false, 0, 0);
        CHECK(myWord8.is_capitalized() );
        word<traits::case_insensitive_ex, stemming::no_op_stem<std::basic_string<wchar_t,traits::case_insensitive_ex > > > myWord9(L"A", 1, 0, 0, 0, false, true, false, false, 0, 0);
        CHECK(myWord9.is_capitalized() );
        word<traits::case_insensitive_ex, stemming::no_op_stem<std::basic_string<wchar_t,traits::case_insensitive_ex > > > myWord10(L"A.", 2, 0, 0, 0, false, true, false, false, 0, 0);
        CHECK(myWord10.is_capitalized() );
        }
    SECTION("Capitalized Not In Caps")
        {
        // version with stemmer
        word<traits::case_insensitive_ex, stemming::english_stem<std::basic_string<wchar_t,traits::case_insensitive_ex >  > > myWord(L"", 0, 0, 0, 0, false, true, false, false, 0, 0);
        CHECK(myWord.is_capitalized_not_in_caps() == false);
        word<traits::case_insensitive_ex, stemming::english_stem<std::basic_string<wchar_t,traits::case_insensitive_ex >  > > myWord2(L"HELLO!", 6, 0, 0, 0, false, true, false, false, 0, 0);
        CHECK(myWord2.is_capitalized_not_in_caps() == false);
        word<traits::case_insensitive_ex, stemming::english_stem<std::basic_string<wchar_t,traits::case_insensitive_ex >  > > myWord3(L"Hello!", 6, 0, 0, 0, false, true, false, false, 0, 0);
        CHECK(myWord3.is_capitalized_not_in_caps());
        word<traits::case_insensitive_ex, stemming::english_stem<std::basic_string<wchar_t,traits::case_insensitive_ex >  > > myWord4(L"A", 1, 0, 0, 0, false, true, false, false, 0, 0);
        CHECK(myWord4.is_capitalized_not_in_caps());
        word<traits::case_insensitive_ex, stemming::english_stem<std::basic_string<wchar_t,traits::case_insensitive_ex >  > > myWord5(L"A.", 2, 0, 0, 0, false, true, false, false, 0, 0);
        CHECK(myWord5.is_capitalized_not_in_caps());

        // without stemmer
        word<traits::case_insensitive_ex, stemming::no_op_stem<std::basic_string<wchar_t,traits::case_insensitive_ex > > > myWord6(L"", 0, 0, 0, 0, false, true, false, false, 0, 0);
        CHECK(myWord6.is_capitalized_not_in_caps() == false);
        word<traits::case_insensitive_ex, stemming::no_op_stem<std::basic_string<wchar_t,traits::case_insensitive_ex > > > myWord7(L"HELLO!", 6, 0, 0, 0, false, true, false, false, 0, 0);
        CHECK(myWord7.is_capitalized_not_in_caps() == false);
        word<traits::case_insensitive_ex, stemming::no_op_stem<std::basic_string<wchar_t,traits::case_insensitive_ex > > > myWord8(L"Hello", 5, 0, 0, 0, false, true, false, false, 0, 0);
        CHECK(myWord8.is_capitalized_not_in_caps());
        word<traits::case_insensitive_ex, stemming::no_op_stem<std::basic_string<wchar_t,traits::case_insensitive_ex > > > myWord9(L"A", 1, 0, 0, 0, false, true, false, false, 0, 0);
        CHECK(myWord9.is_capitalized_not_in_caps());
        word<traits::case_insensitive_ex, stemming::no_op_stem<std::basic_string<wchar_t,traits::case_insensitive_ex > > > myWord10(L"A.", 2, 0, 0, 0, false, true, false, false, 0, 0);
        CHECK(myWord10.is_capitalized_not_in_caps());
        }
    SECTION("Comparisons")
        {
        // version without stemmer
        word<std::char_traits<wchar_t>, stemming::no_op_stem<std::basic_string<wchar_t,std::char_traits<wchar_t> > > > myWord1(L"hello", 5, 1, 1, 1, false, false, true, true, 1, 0);
        word<std::char_traits<wchar_t>, stemming::no_op_stem<std::basic_string<wchar_t,std::char_traits<wchar_t> > > > myWord2(L"hello", 5, 0, 0, 0, true, true, false, false, 0, 0);
        CHECK((myWord1 == myWord2) == true);
        CHECK((myWord1 != myWord2) == false);
        CHECK((myWord1 < myWord2) == false);
        word<std::char_traits<wchar_t>, stemming::no_op_stem<std::basic_string<wchar_t,std::char_traits<wchar_t> > > > myWordOther(L"HelLo!!!", 8, 2, 2, 2, false, true, true, false, 3, 0);
        CHECK((myWordOther == myWord2) == false);
        CHECK((myWordOther != myWord2) == true);
        CHECK((myWordOther < myWord2) == true);

        // with stemmer
        word<traits::case_insensitive_ex > myWord3(L"hello", 5, 0, 0, 0, true, true, false, false, 0, 0);
        word<traits::case_insensitive_ex > myWord4(L"HelLo", 5, 0, 0, 0, true, true, false, false, 0, 0);
        CHECK((myWord3 == myWord4) == true);
        CHECK((myWord3 != myWord4) == false);
        CHECK((myWord3 < myWord4) == false);
        }
    SECTION("Comparisons Case Insensitive")
        {
        // version without stemmer
        word<traits::case_insensitive_ex, stemming::no_op_stem<std::basic_string<wchar_t,traits::case_insensitive_ex > > > myWord1(L"hello", 5, 1, 1, 1, false, false, true, true, 1, 0);
        word<traits::case_insensitive_ex, stemming::no_op_stem<std::basic_string<wchar_t,traits::case_insensitive_ex > > > myWord2(L"hello", 5, 0, 0, 0, true, true, false, false, 0, 0);
        CHECK((myWord1 == myWord2) == true);
        CHECK((myWord1 != myWord2) == false);
        CHECK((myWord1 < myWord2) == false);
        word<traits::case_insensitive_ex, stemming::no_op_stem<std::basic_string<wchar_t,traits::case_insensitive_ex > > > myWordOther(L"HelLo", 5, 2, 2, 2, false, true, true, false, 3, 0);
        CHECK((myWordOther == myWord2) == true);
        CHECK((myWordOther != myWord2) == false);
        CHECK((myWordOther < myWord2) == false);

        word<traits::case_insensitive_ex, stemming::no_op_stem<std::basic_string<wchar_t,traits::case_insensitive_ex > > > myWordOtherLonger(L"HelLo!!!", 8, 2, 2, 2, false, true, true, false, 3, 0);
        CHECK((myWordOtherLonger == myWord2) == false);
        CHECK((myWordOtherLonger != myWord2) == true);
        CHECK((myWordOtherLonger < myWord2) == false);

        // with stemmer
        word<traits::case_insensitive_ex > myWord3(L"hello", 5, 0, 0, 0, true, true, false, false, 0, 0);
        word<traits::case_insensitive_ex > myWord4(L"HelLo", 5, 0, 0, 0, true, true, false, false, 0, 0);
        CHECK((myWord3 == myWord4) == true);
        CHECK((myWord3 != myWord4) == false);
        CHECK((myWord3 < myWord4) == false);
        }
    SECTION("Comparisons By Stem")
        {
        word<traits::case_insensitive_ex > myWord1(L"document", 8, 0, 0, 0, true, true, false, false, 0, 0);
        word<traits::case_insensitive_ex > myWord2(L"documenting", 11, 0, 0, 0, true, true, false, false, 0, 0);
        word<traits::case_insensitive_ex > wrongWord(L"dokumenting", 11, 0, 0, 0, true, true, false, false, 0, 0);
        CHECK((myWord1 == myWord2));
        CHECK((myWord1 != myWord2) == false);
        CHECK((myWord1 < myWord2) == false);
        CHECK((myWord2 < myWord1) == false);
        CHECK(0 == myWord1.compare(myWord2));
        // mismatch
        CHECK((myWord1 == wrongWord) == false);
        CHECK((myWord1 != wrongWord));
        CHECK((myWord1 < wrongWord));
        CHECK((wrongWord < myWord1) == false);
        CHECK(-1 == myWord1.compare(wrongWord));
        // inline stemming
        CHECK((myWord1 == L"documented"));
        CHECK(0 == myWord1.compare(L"documented"));
        // inline stemming mismatch
        CHECK((myWord1 == L"dokumented") == false);
        CHECK(-1 == myWord1.compare(L"dokumented"));
        }

    }
