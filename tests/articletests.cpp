#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>
#include "../src/indexing/article.h"
#include "../src/indexing/word_collection.h"
#include "../src/indexing/spanish_syllabize.h"
#include "../src/indexing/german_syllabize.h"
#include "../src/indexing/word.h"

// clang-format off
// NOLINTBEGIN

using namespace grammar;

using MYWORD = word<traits::case_insensitive_ex,
    stemming::english_stem<std::basic_string<wchar_t, traits::case_insensitive_ex> > >;

extern word_list Stop_list;

TEST_CASE("English Article", "[articles]")
    {
    grammar::english_syllabize ENsyllabizer;
    grammar::spanish_syllabize ESsyllabizer;
    grammar::german_syllabize DEsyllabizer;
    stemming::english_stem<std::wstring> ENStemmer;
    grammar::is_english_coordinating_conjunction is_conjunction;
    grammar::is_incorrect_english_article is_english_mismatched_article;
    grammar::phrase_collection pmap;
    grammar::phrase_collection copyrightPMap;
    grammar::phrase_collection citationPMap;
    word_list Known_proper_nouns;
    word_list Known_personal_nouns;
    word_list Known_spellings;
    word_list Secondary_known_spellings;
    word_list Programming_known_spellings;

    SECTION("NULL")
        {
        grammar::is_incorrect_english_article artc;
        CHECK_FALSE(artc(L"and", L"Irish"));
        CHECK_FALSE(artc(L"", L"Orange"));
        CHECK_FALSE(artc(L"a", L""));
        }
    // this one requires higher level testing
    SECTION("Punctuation Between Words")
        {
        document<MYWORD> doc(std::wstring{}, &ENsyllabizer, &ENStemmer, &is_conjunction,
            &pmap, &copyrightPMap, &citationPMap, &Known_proper_nouns, &Known_personal_nouns,
            &Known_spellings, &Secondary_known_spellings, &Programming_known_spellings,
            &Stop_list);
        // comma indicates that the 'a' is not connected to 'auto'
        const wchar_t* text = L"a auto";
        doc.load_document(text, wcslen(text), false, false, false, false);
        CHECK(doc.get_incorrect_article_indices().size() == 0);
        }
    SECTION("Acronyms")
        {
        grammar::is_incorrect_english_article artc;

        CHECK(artc(L"a", L"UND"));
        CHECK(artc(L"a", L"AMP"));
        CHECK(artc(L"a", L"EMP"));
        CHECK(artc(L"a", L"IMP"));
        CHECK(artc(L"a", L"OMP"));
        CHECK(artc(L"a", L"SAT"));
        CHECK(artc(L"a", L"STT"));
        CHECK(artc(L"a", L"STT-AT"));
        CHECK(!artc(L"an", L"SAT"));
        }
    SECTION("Punctuation")
        {
        grammar::is_incorrect_english_article artc;

        CHECK_FALSE(artc(L"a", L"-verse"));
        CHECK_FALSE(artc(L"an", L"'verse"));
        }
    SECTION("Acronyms Hyphenated")
        {
        grammar::is_incorrect_english_article artc;

        CHECK(artc(L"a", L"u-verse") == false);

        CHECK(artc(L"a", L"XML"));
        CHECK(artc(L"a", L"XML-compatible"));

        CHECK(artc(L"an", L"XAML"));
        CHECK(artc(L"an", L"XAML-compatible"));

        CHECK(artc(L"a", L"RTF"));
        CHECK(artc(L"a", L"RTF-license"));
        }
    SECTION("Odd Combos")
        {
        grammar::is_incorrect_english_article artc;

        CHECK(artc(L"a", L"LDA")); // wrong with A
        CHECK(artc(L"an", L"LDA") == false); // correct with AN

        CHECK(artc(L"a", L"FPG")); // wrong with A
        CHECK(artc(L"an", L"FPG") == false); // correct with AN

        CHECK(artc(L"a", L"FSA")); // wrong with A
        CHECK(artc(L"an", L"FSA") == false); // correct with AN

        CHECK(artc(L"a", L"FLY") == false); // correct with A
        CHECK(artc(L"an", L"FLY")); // wrong with AN

        CHECK(artc(L"a", L"FRY") == false); // correct with A
        CHECK(artc(L"an", L"FRY")); // wrong with AN

        CHECK(artc(L"a", L"NDA")); // wrong with A
        CHECK(artc(L"an", L"NDA") == false); // correct with AN

        CHECK(artc(L"a", L"SBA")); // wrong with A
        CHECK(artc(L"an", L"SBA") == false); // correct with AN
        }
    SECTION("2-Letter Word")
        {
        grammar::is_incorrect_english_article artc;

        CHECK(artc(L"a", L"UD") == false); // correct with A
        CHECK(artc(L"an", L"UD")); // wrong with AN

        // will actually separate this and look at just "re" as a separate word
        CHECK(artc(L"a", L"re-implementation") == false); // correct with A
        CHECK(artc(L"an", L"re-implementation")); // wrong with AN

        // behaviour is different with an acronym "RE"
        CHECK(artc(L"a", L"RE-implementation")); // wrong with A
        CHECK(artc(L"an", L"RE-implementation") == false); // correct with AN

        // behavior will be different with more than one letter
        CHECK(artc(L"a", L"UDS")); // wrong with A
        CHECK(artc(L"an", L"UDS") == false); // correct with AN
        }
    SECTION("Acronyms Mixed")
        {
        grammar::is_incorrect_english_article artc;

        CHECK(artc(L"a", L"STReport") == false);
        CHECK(artc(L"an", L"STReport"));

        CHECK(artc(L"a", L"SPBReport"));
        CHECK(artc(L"an", L"SPBReport") == false);

        CHECK(artc(L"a", L"NPV"));
        CHECK(artc(L"an", L"NPV") == false);
        }
    SECTION("No Vowel Word")
        {
        grammar::is_incorrect_english_article artc;

        CHECK(artc(L"an", L"Np")); // wrong with an
        CHECK(artc(L"a", L"ZPP") == false); // correct
        CHECK(artc(L"a", L"NZPD")); // should be "an NYPD"
        CHECK(artc(L"an", L"ZPP")); // should be "a ZPP"
        CHECK(artc(L"an", L"NZPD") == false); // correct
        }
    SECTION("Known A Acronymns")
        {
        grammar::is_incorrect_english_article art;
        CHECK(!art(L"a", L"UTF"));
        CHECK(art(L"an", L"UTF"));
        CHECK(!art(L"a", L"UNC"));
        CHECK(art(L"an", L"UNC"));

        CHECK(art(L"a", L"UNCLE"));
        CHECK(!art(L"an", L"UNCLE"));
        }
    SECTION("Schwartz")
        {
        grammar::is_incorrect_english_article art;
        CHECK(!art(L"a", L"Schwartz"));
        }
    SECTION("Digit 2 Number")
        {
        grammar::is_incorrect_english_article artc;
    
        CHECK(artc(L"a", L"10-hole") == false);
        CHECK(artc(L"a", L"11-hole"));
        CHECK(artc(L"a", L"12-hole") == false);
        CHECK(artc(L"a", L"13-hole") == false);
        CHECK(artc(L"a", L"14-hole") == false);
        CHECK(artc(L"a", L"15-hole") == false);
        CHECK(artc(L"a", L"16-hole") == false);
        CHECK(artc(L"a", L"17-hole") == false);
        CHECK(artc(L"a", L"18-hole"));
        CHECK(artc(L"a", L"19-hole") == false);
        CHECK(artc(L"a", L"20-hole") == false);
        CHECK(artc(L"a", L"21-hole") == false);
        CHECK(artc(L"a", L"31-hole") == false);
        CHECK(artc(L"a", L"41-hole") == false);
        CHECK(artc(L"a", L"51-hole") == false);
        CHECK(artc(L"a", L"61-hole") == false);
        CHECK(artc(L"a", L"71-hole") == false);
        CHECK(artc(L"a", L"81-hole"));
        CHECK(artc(L"a", L"91-hole") == false);

        CHECK(artc(L"an", L"10-hole"));
        CHECK(artc(L"an", L"11-hole") == false);
        CHECK(artc(L"an", L"12-hole"));
        CHECK(artc(L"an", L"13-hole"));
        CHECK(artc(L"an", L"14-hole"));
        CHECK(artc(L"an", L"15-hole"));
        CHECK(artc(L"an", L"16-hole"));
        CHECK(artc(L"an", L"17-hole"));
        CHECK(artc(L"an", L"18-hole") == false);
        CHECK(artc(L"an", L"19-hole"));
        CHECK(artc(L"an", L"20-hole"));
        CHECK(artc(L"an", L"21-hole"));
        CHECK(artc(L"an", L"31-hole"));
        CHECK(artc(L"an", L"41-hole"));
        CHECK(artc(L"an", L"51-hole"));
        CHECK(artc(L"an", L"61-hole"));
        CHECK(artc(L"an", L"71-hole"));
        CHECK(artc(L"an", L"81-hole") == false);
        CHECK(artc(L"an", L"91-hole"));
        }
    SECTION("A Simple")
        {
        grammar::is_incorrect_english_article artc;

        CHECK(artc(L"a", L"plane") == false);
        CHECK(artc(L"a", L"zoo") == false);
        CHECK(artc(L"a", L"car") == false);
        CHECK(artc(L"a", L"dog") == false);
        CHECK(artc(L"a", L"net") == false);

        CHECK(artc(L"a", L"Plane") == false);
        CHECK(artc(L"a", L"Zoo") == false);
        CHECK(artc(L"a", L"Car") == false);
        CHECK(artc(L"a", L"Dog") == false);
        CHECK(artc(L"a", L"Net") == false);
        }
    SECTION("A Vowels")
        {
        grammar::is_incorrect_english_article artc;

        CHECK(artc(L"a", L"airplane"));
        CHECK(artc(L"a", L"english"));
        CHECK(artc(L"a", L"irish"));
        CHECK(artc(L"a", L"orange"));
        CHECK(artc(L"a", L"undertow"));
        CHECK(artc(L"a", L"onerous"));
        CHECK(artc(L"a", L"year") == false);

        CHECK(artc(L"a", L"Airplane"));
        CHECK(artc(L"a", L"English"));
        CHECK(artc(L"a", L"Irish"));
        CHECK(artc(L"a", L"Orange"));
        CHECK(artc(L"a", L"Undertow"));
        CHECK(artc(L"a", L"Onerous"));
        CHECK(artc(L"a", L"Year") == false);

        CHECK(artc(L"a", L"on-campus"));
        CHECK(!artc(L"an", L"on-campus"));
        }
    SECTION("A Exceptions")
        {
        grammar::is_incorrect_english_article artc;
        artc.get_a_exceptions().add_word(L"and");
        artc.get_a_exceptions().add_word(L"are");
        artc.get_a_exceptions().add_word(L"is");
        artc.get_a_exceptions().add_word(L"nor");
        artc.get_a_exceptions().add_word(L"of");
        artc.get_a_exceptions().add_word(L"once");
        artc.get_a_exceptions().add_word(L"one");
        artc.get_a_exceptions().add_word(L"onetime");
        artc.get_a_exceptions().add_word(L"UAW");
        artc.get_a_exceptions().add_word(L"UCSF");
        artc.get_a_exceptions().add_word(L"USB");
        artc.get_a_exceptions().add_word(L"USPTO");

        CHECK(artc(L"a", L"are") == false);
        CHECK(artc(L"a", L"area"));

        CHECK(artc(L"a", L"is") == false);
        CHECK(artc(L"a", L"istanbul"));

        CHECK(artc(L"a", L"and") == false);
        CHECK(artc(L"a", L"anderson"));

        CHECK(artc(L"a", L"ore"));
        CHECK(artc(L"a", L"or") == false);

        CHECK(artc(L"a", L"Euro") == false);
        CHECK(artc(L"a", L"one") == false);
        // these 2 should be wrong
        CHECK(artc(L"a", L"onerous"));
        CHECK(artc(L"a", L"on-campus"));
        CHECK(artc(L"a", L"one-sided") == false);
        CHECK(artc(L"a", L"unicycle") == false);
        CHECK(artc(L"a", L"universal") == false);
        CHECK(artc(L"a", L"united") == false);
        CHECK(artc(L"a", L"unix") == false);
        CHECK(artc(L"a", L"useful") == false);
        CHECK(artc(L"a", L"Utopia") == false);
        CHECK(artc(L"a", L"Ukranian") == false);
        CHECK(artc(L"a", L"uterus") == false);
        CHECK(artc(L"a", L"urethra") == false);
        CHECK(artc(L"a", L"u-turn") == false);
        CHECK(artc(L"a", L"utility") == false);
        CHECK(artc(L"a", L"using") == false);
        CHECK(artc(L"a", L"uninteresting"));
        CHECK(artc(L"a", L"unimpressed"));
        CHECK(artc(L"a", L"unidentified"));
        CHECK(artc(L"a", L"unilateral") == false);
        // a Sat (like Saturday) is OK
        CHECK(artc(L"a", L"Sat") == false);
        // a SAT (like the text) is wrong
        CHECK(artc(L"a", L"SAT"));

        CHECK(artc(L"an", L"utility"));
        CHECK(artc(L"an", L"u-turn"));
        CHECK(artc(L"an", L"Euro"));
        CHECK(artc(L"an", L"one"));
        CHECK(artc(L"an", L"one-sided"));
        CHECK(artc(L"an", L"unicycle"));
        CHECK(artc(L"an", L"useful"));
        CHECK(artc(L"an", L"Utopia"));
        CHECK(artc(L"an", L"Ukranian"));
        CHECK(artc(L"an", L"uterus"));
        CHECK(artc(L"an", L"urethra"));
        CHECK(artc(L"an", L"using"));
        CHECK(artc(L"an", L"uninteresting") == false);
        CHECK(artc(L"an", L"unimpressed") == false);
        CHECK(artc(L"an", L"unidentified") == false);
        CHECK(artc(L"an", L"unilateral"));
        // a Sat (like Saturday) is OK
        CHECK(artc(L"an", L"Sat"));
        // a SAT (like the text) is wrong
        CHECK(artc(L"an", L"SAT") == false);
        }
    SECTION("A Letters")
        {
        grammar::is_incorrect_english_article artc;

        CHECK(artc(L"a", L"x-ray"));
        CHECK(artc(L"a", L"x'x"));
        CHECK(artc(L"a", L"u") == false);
        CHECK(artc(L"a", L"u.s.") == false);
        CHECK(artc(L"a", L"h1n1"));

        CHECK(artc(L"an", L"x-ray") == false);
        CHECK(artc(L"an", L"x'x") == false);
        CHECK(artc(L"an", L"u"));
        CHECK(artc(L"an", L"u.s."));
        CHECK(artc(L"an", L"h1n1") == false);
        }
    SECTION("AN Simple")
        {
        grammar::is_incorrect_english_article artc;

        CHECK(artc(L"an", L"plane"));
        CHECK(artc(L"an", L"zoo"));
        CHECK(artc(L"an", L"car"));
        CHECK(artc(L"an", L"dog"));
        CHECK(artc(L"an", L"net"));

        CHECK(artc(L"an", L"Plane"));
        CHECK(artc(L"an", L"Zoo"));
        CHECK(artc(L"an", L"Car"));
        CHECK(artc(L"an", L"Dog"));
        CHECK(artc(L"an", L"Net"));
        }
    SECTION("AN Vowels")
        {
        grammar::is_incorrect_english_article artc;

        CHECK(artc(L"an", L"airplane") == false);
        CHECK(artc(L"an", L"english") == false);
        CHECK(artc(L"an", L"irish") == false);
        CHECK(artc(L"an", L"orange") == false);
        CHECK(artc(L"an", L"undertow") == false);
        CHECK(artc(L"an", L"year") == false);

        CHECK(artc(L"an", L"Airplane") == false);
        CHECK(artc(L"an", L"English") == false);
        CHECK(artc(L"an", L"Irish") == false);
        CHECK(artc(L"an", L"Orange") == false);
        CHECK(artc(L"an", L"Undertow") == false);
        CHECK(artc(L"an", L"Year") == false);
        }
    SECTION("AN Exceptions")
        {
        grammar::is_incorrect_english_article artc;

        CHECK(artc(L"a", L"hysterical") == false);
        CHECK(artc(L"an", L"honour") == false);
        CHECK(artc(L"an", L"honey"));
        CHECK(artc(L"an", L"hour") == false);
        CHECK(artc(L"an", L"honest") == false);
        CHECK(artc(L"an", L"house"));
        CHECK(artc(L"an", L"historical"));
        CHECK(artc(L"an", L"history"));
        CHECK(artc(L"an", L"heir") == false);

        CHECK(artc(L"a", L"Hysterical") == false);
        CHECK(artc(L"an", L"Honour") == false);
        CHECK(artc(L"an", L"Honey"));
        CHECK(artc(L"an", L"Hour") == false);
        CHECK(artc(L"an", L"Honest") == false);
        CHECK(artc(L"an", L"House"));
        CHECK(artc(L"an", L"Historical"));
        CHECK(artc(L"an", L"History"));
        CHECK(artc(L"an", L"Heir") == false);

        CHECK(artc(L"an", L"hysterical"));
        CHECK(artc(L"a", L"honour"));
        CHECK(artc(L"a", L"honey") == false);
        CHECK(artc(L"a", L"hour"));
        CHECK(artc(L"a", L"honest"));
        CHECK(artc(L"a", L"house") == false);
        CHECK(artc(L"a", L"historical") == false);
        CHECK(artc(L"a", L"history") == false);
        CHECK(artc(L"a", L"heir"));
        }
    SECTION("AN Acronyms And Letters")
        {
        grammar::is_incorrect_english_article artc;

        CHECK(artc(L"an", L"HRESULT") == false);
        CHECK(artc(L"an", L"HMS") == false);
        CHECK(artc(L"an", L"LLC") == false);
        CHECK(artc(L"an", L"MST") == false);
        CHECK(artc(L"an", L"NWP") == false);
        CHECK(artc(L"an", L"RR") == false);
        CHECK(artc(L"an", L"SS") == false);
        CHECK(artc(L"an", L"XBT") == false);
        // the vowel makes it be seen as a word
        CHECK(artc(L"an", L"HAM"));

        CHECK(artc(L"an", L"H1N1") == false);

        CHECK(artc(L"an", L"f") == false);
        CHECK(artc(L"an", L"h") == false);
        CHECK(artc(L"an", L"l") == false);
        CHECK(artc(L"an", L"m") == false);
        CHECK(artc(L"an", L"n") == false);
        CHECK(artc(L"an", L"r") == false);
        CHECK(artc(L"an", L"s") == false);
        CHECK(artc(L"an", L"x") == false);
        CHECK(artc(L"an", L"u"));

        CHECK(artc(L"an", L"F") == false);
        CHECK(artc(L"an", L"H") == false);
        CHECK(artc(L"an", L"L") == false);
        CHECK(artc(L"an", L"M") == false);
        CHECK(artc(L"an", L"N") == false);
        CHECK(artc(L"an", L"R") == false);
        CHECK(artc(L"an", L"S") == false);
        CHECK(artc(L"an", L"X") == false);
        CHECK(artc(L"an", L"U"));
        }
    SECTION("Numbers")
        {
        grammar::is_incorrect_english_article artc;

        CHECK(artc(L"an", L"0-char"));
        CHECK(artc(L"a", L"0-char") == false);

        CHECK(artc(L"an", L"1-char"));
        CHECK(artc(L"a", L"1-char") == false);

        CHECK(artc(L"an", L"2-char"));
        CHECK(artc(L"a", L"2-char") == false);

        CHECK(artc(L"an", L"3-char"));
        CHECK(artc(L"a", L"3-char") == false);

        CHECK(artc(L"an", L"4-char"));
        CHECK(artc(L"a", L"4-char") == false);

        CHECK(artc(L"an", L"5-char"));
        CHECK(artc(L"a", L"5-char") == false);

        CHECK(artc(L"an", L"6-char"));
        CHECK(artc(L"a", L"6-char") == false);

        CHECK(artc(L"an", L"7-char"));
        CHECK(artc(L"a", L"7-char") == false);

        CHECK(artc(L"an", L"8-char") == false);
        CHECK(artc(L"a", L"8-char"));

        CHECK(artc(L"an", L"9-char"));
        CHECK(artc(L"a", L"9-char") == false);

        CHECK(artc(L"an", L"0char"));
        CHECK(artc(L"a", L"0char") == false);

        CHECK(artc(L"an", L"1char"));
        CHECK(artc(L"a", L"1char") == false);

        CHECK(artc(L"an", L"2char"));
        CHECK(artc(L"a", L"2char") == false);

        CHECK(artc(L"an", L"3char"));
        CHECK(artc(L"a", L"3char") == false);

        CHECK(artc(L"an", L"4char"));
        CHECK(artc(L"a", L"4char") == false);

        CHECK(artc(L"an", L"5char"));
        CHECK(artc(L"a", L"5char") == false);

        CHECK(artc(L"an", L"6char"));
        CHECK(artc(L"a", L"6char") == false);

        CHECK(artc(L"an", L"7char"));
        CHECK(artc(L"a", L"7char") == false);

        CHECK(artc(L"an", L"8char") == false);
        CHECK(artc(L"a", L"8char"));

        CHECK(artc(L"an", L"9char"));
        CHECK(artc(L"a", L"9char") == false);

        // don't bother with more complicated numbers
        CHECK(artc(L"an", L"1,800") == false);
        CHECK(artc(L"a", L"1,800") == false);

        CHECK(artc(L"an", L"1st"));
        CHECK(artc(L"a", L"1st") == false);

        CHECK(artc(L"an", L"21st"));
        CHECK(artc(L"a", L"21st") == false);

        CHECK(artc(L"an", L"2nd"));
        CHECK(artc(L"a", L"2nd") == false);

        CHECK(artc(L"an", L"3rd"));
        CHECK(artc(L"a", L"3rd") == false);

        CHECK(artc(L"an", L"11th") == false);
        CHECK(artc(L"a", L"11th"));

        CHECK(artc(L"an", L"8th") == false);
        CHECK(artc(L"a", L"8th"));

        CHECK(artc(L"an", L"4th"));
        CHECK(artc(L"a", L"4th") == false);
        }
    SECTION("Year")
        {
        grammar::is_incorrect_english_article artc;

        CHECK(artc(L"a", L"1087") == false);
        CHECK(artc(L"a", L"1187"));
        CHECK(artc(L"a", L"1287") == false);
        CHECK(artc(L"a", L"1387") == false);
        CHECK(artc(L"a", L"1487") == false);
        CHECK(artc(L"a", L"1587") == false);
        CHECK(artc(L"a", L"1687") == false);
        CHECK(artc(L"a", L"1787") == false);
        CHECK(artc(L"a", L"1887"));
        CHECK(artc(L"a", L"1987") == false);
        CHECK(artc(L"a", L"2087") == false);

        CHECK(artc(L"an", L"1087"));
        CHECK(artc(L"an", L"1187") == false);
        CHECK(artc(L"an", L"1287"));
        CHECK(artc(L"an", L"1387"));
        CHECK(artc(L"an", L"1487"));
        CHECK(artc(L"an", L"1587"));
        CHECK(artc(L"an", L"1687"));
        CHECK(artc(L"an", L"1787"));
        CHECK(artc(L"an", L"1887") == false);
        CHECK(artc(L"an", L"1987"));
        CHECK(artc(L"an", L"2087"));
        }

    SECTION("Time")
        {
        grammar::is_incorrect_english_article artc;

        CHECK(artc(L"a", L"10:87") == false);
        CHECK(artc(L"a", L"11:87"));
        CHECK(artc(L"a", L"12:87") == false);
        CHECK(artc(L"a", L"13:87") == false);
        CHECK(artc(L"a", L"14:87") == false);
        CHECK(artc(L"a", L"15:87") == false);
        CHECK(artc(L"a", L"16:87") == false);
        CHECK(artc(L"a", L"17:87") == false);
        CHECK(artc(L"a", L"18:87"));
        CHECK(artc(L"a", L"19:87") == false);
        CHECK(artc(L"a", L"20:87") == false);

        CHECK(artc(L"an", L"10:87"));
        CHECK(artc(L"an", L"11:87") == false);
        CHECK(artc(L"an", L"12:87"));
        CHECK(artc(L"an", L"13:87"));
        CHECK(artc(L"an", L"14:87"));
        CHECK(artc(L"an", L"15:87"));
        CHECK(artc(L"an", L"16:87"));
        CHECK(artc(L"an", L"17:87"));
        CHECK(artc(L"an", L"18:87") == false);
        CHECK(artc(L"an", L"19:87"));
        CHECK(artc(L"an", L"20:87"));
        }
    }

// NOLINTEND
// clang-format on
