#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>
#include "../src/indexing/article.h"
#include "../src/indexing/word_collection.h"
#include "../src/indexing/spanish_syllabize.h"
#include "../src/indexing/german_syllabize.h"
#include "../src/indexing/word.h"

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
    grammar::phrase_collection excludedPMap;
    word_list Known_proper_nouns;
    word_list Known_personal_nouns;
    word_list Known_spellings;
    word_list Secondary_known_spellings;
    word_list Programming_known_spellings;

    SECTION("NULL")
        {
        grammar::is_incorrect_english_article artc;

        CHECK(artc(nullptr, 1, L"airplane", 8) == false);
        CHECK(artc(L"a", 1, nullptr, 7) == false);
        CHECK(artc(L"and", 3, L"Irish", 5) == false);
        CHECK(artc(L"", 0, L"Orange", 6) == false);
        CHECK(artc(L"a", 1, L"", 0) == false);
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

        CHECK(artc(L"a", 1, L"UND", 3));
        CHECK(artc(L"a", 1, L"AMP", 3));
        CHECK(artc(L"a", 1, L"EMP", 3));
        CHECK(artc(L"a", 1, L"IMP", 3));
        CHECK(artc(L"a", 1, L"OMP", 3));
        CHECK(artc(L"a", 1, L"SAT", 3));
        CHECK(!artc(L"an", 2, L"SAT", 3));
        }
    SECTION("Punctuation")
        {
        grammar::is_incorrect_english_article artc;

        CHECK(artc(L"a", 1, L"-verse", 6) == false);
        CHECK(artc(L"an", 2, L"'verse", 6) == false);
        }
    SECTION("Acronyms Hyphenated")
        {
        grammar::is_incorrect_english_article artc;

        CHECK(artc(L"a", 1, L"u-verse", 7) == false);

        CHECK(artc(L"a", 1, L"XML", 3));
        CHECK(artc(L"a", 1, L"XML-compatible", 14));

        CHECK(artc(L"an", 2, L"XAML", 4));
        CHECK(artc(L"an", 2, L"XAML-compatible", 15));

        CHECK(artc(L"a", 1, L"RTF", 3));
        CHECK(artc(L"a", 1, L"RTF-license", 11));
        }
    SECTION("Odd Combos")
        {
        grammar::is_incorrect_english_article artc;

        CHECK(artc(L"a", 1, L"LDA", 3)); // wrong with A
        CHECK(artc(L"an", 2, L"LDA", 3) == false); // correct with AN

        CHECK(artc(L"a", 1, L"FPG", 3)); // wrong with A
        CHECK(artc(L"an", 2, L"FPG", 3) == false); // correct with AN

        CHECK(artc(L"a", 1, L"FSA", 3)); // wrong with A
        CHECK(artc(L"an", 2, L"FSA", 3) == false); // correct with AN

        CHECK(artc(L"a", 1, L"FLY", 3) == false); // correct with A
        CHECK(artc(L"an", 2, L"FLY", 3)); // wrong with AN

        CHECK(artc(L"a", 1, L"FRY", 3) == false); // correct with A
        CHECK(artc(L"an", 2, L"FRY", 3)); // wrong with AN

        CHECK(artc(L"a", 1, L"NDA", 3)); // wrong with A
        CHECK(artc(L"an", 2, L"NDA", 3) == false); // correct with AN

        CHECK(artc(L"a", 1, L"SBA", 3)); // wrong with A
        CHECK(artc(L"an", 2, L"SBA", 3) == false); // correct with AN
        }
    SECTION("2-Letter Word")
        {
        grammar::is_incorrect_english_article artc;

        CHECK(artc(L"a", 1, L"UD", 2) == false); // correct with A
        CHECK(artc(L"an", 2, L"UD", 2)); // wrong with AN

        // will actually separate this and look at just "re" as a separate word
        CHECK(artc(L"a", 1, L"re-implementation", 17) == false); // correct with A
        CHECK(artc(L"an", 2, L"re-implementation", 17)); // wrong with AN

        // behaviour is different with an acronym "RE"
        CHECK(artc(L"a", 1, L"RE-implementation", 17)); // wrong with A
        CHECK(artc(L"an", 2, L"RE-implementation", 17) == false); // correct with AN

        // behavior will be different with more than one letter
        CHECK(artc(L"a", 1, L"UDS", 3)); // wrong with A
        CHECK(artc(L"an", 2, L"UDS", 3) == false); // correct with AN
        }
    SECTION("Acronyms Mixed")
        {
        grammar::is_incorrect_english_article artc;

        CHECK(artc(L"a", 1, L"STReport", 8) == false);
        CHECK(artc(L"an", 2, L"STReport", 8));

        CHECK(artc(L"a", 1, L"SPBReport", 9));
        CHECK(artc(L"an", 2, L"SPBReport", 9) == false);

        CHECK(artc(L"a", 1, L"NPV", 3));
        CHECK(artc(L"an", 2, L"NPV", 3) == false);
        }
    SECTION("No Vowel Word")
        {
        grammar::is_incorrect_english_article artc;

        CHECK(artc(L"an", 2, L"Np", 2)); // wrong with an
        CHECK(artc(L"a", 1, L"ZPP", 3) == false); // correct
        CHECK(artc(L"a", 1, L"NZPD", 4)); // should be "an NYPD"
        CHECK(artc(L"an", 2, L"ZPP", 3)); // should be "a ZPP"
        CHECK(artc(L"an", 2, L"NZPD", 4) == false); // correct
        }
    SECTION("Known A Acronymns")
        {
        grammar::is_incorrect_english_article art;
        CHECK(!art(L"a", 1, L"UTF", 3));
        CHECK(art(L"an", 2, L"UTF", 3));
        CHECK(!art(L"a", 1, L"UNC", 3));
        CHECK(art(L"an", 2, L"UNC", 3));

        CHECK(art(L"a", 1, L"UNCLE", 5));
        CHECK(!art(L"an", 2, L"UNCLE", 5));
        }
    SECTION("Schwartz")
        {
        grammar::is_incorrect_english_article art;
        CHECK(!art(L"a", 1, L"Schwartz", 8));
        }
    SECTION("Digit 2 Number")
        {
        grammar::is_incorrect_english_article artc;
    
        CHECK(artc(L"a", 1, L"10-hole", 7) == false);
        CHECK(artc(L"a", 1, L"11-hole", 7));
        CHECK(artc(L"a", 1, L"12-hole", 7) == false);
        CHECK(artc(L"a", 1, L"13-hole", 7) == false);
        CHECK(artc(L"a", 1, L"14-hole", 7) == false);
        CHECK(artc(L"a", 1, L"15-hole", 7) == false);
        CHECK(artc(L"a", 1, L"16-hole", 7) == false);
        CHECK(artc(L"a", 1, L"17-hole", 7) == false);
        CHECK(artc(L"a", 1, L"18-hole", 7));
        CHECK(artc(L"a", 1, L"19-hole", 7) == false);
        CHECK(artc(L"a", 1, L"20-hole", 7) == false);
        CHECK(artc(L"a", 1, L"21-hole", 7) == false);
        CHECK(artc(L"a", 1, L"31-hole", 7) == false);
        CHECK(artc(L"a", 1, L"41-hole", 7) == false);
        CHECK(artc(L"a", 1, L"51-hole", 7) == false);
        CHECK(artc(L"a", 1, L"61-hole", 7) == false);
        CHECK(artc(L"a", 1, L"71-hole", 7) == false);
        CHECK(artc(L"a", 1, L"81-hole", 7));
        CHECK(artc(L"a", 1, L"91-hole", 7) == false);

        CHECK(artc(L"an", 2, L"10-hole", 7));
        CHECK(artc(L"an", 2, L"11-hole", 7) == false);
        CHECK(artc(L"an", 2, L"12-hole", 7));
        CHECK(artc(L"an", 2, L"13-hole", 7));
        CHECK(artc(L"an", 2, L"14-hole", 7));
        CHECK(artc(L"an", 2, L"15-hole", 7));
        CHECK(artc(L"an", 2, L"16-hole", 7));
        CHECK(artc(L"an", 2, L"17-hole", 7));
        CHECK(artc(L"an", 2, L"18-hole", 7) == false);
        CHECK(artc(L"an", 2, L"19-hole", 7));
        CHECK(artc(L"an", 2, L"20-hole", 7));
        CHECK(artc(L"an", 2, L"21-hole", 7));
        CHECK(artc(L"an", 2, L"31-hole", 7));
        CHECK(artc(L"an", 2, L"41-hole", 7));
        CHECK(artc(L"an", 2, L"51-hole", 7));
        CHECK(artc(L"an", 2, L"61-hole", 7));
        CHECK(artc(L"an", 2, L"71-hole", 7));
        CHECK(artc(L"an", 2, L"81-hole", 7) == false);
        CHECK(artc(L"an", 2, L"91-hole", 7));
        }
    SECTION("A Simple")
        {
        grammar::is_incorrect_english_article artc;

        CHECK(artc(L"a", 1, L"plane", 5) == false);
        CHECK(artc(L"a", 1, L"zoo", 3) == false);
        CHECK(artc(L"a", 1, L"car", 3) == false);
        CHECK(artc(L"a", 1, L"dog", 3) == false);
        CHECK(artc(L"a", 1, L"net", 3) == false);

        CHECK(artc(L"a", 1, L"Plane", 5) == false);
        CHECK(artc(L"a", 1, L"Zoo", 3) == false);
        CHECK(artc(L"a", 1, L"Car", 3) == false);
        CHECK(artc(L"a", 1, L"Dog", 3) == false);
        CHECK(artc(L"a", 1, L"Net", 3) == false);
        }
    SECTION("A Vowels")
        {
        grammar::is_incorrect_english_article artc;

        CHECK(artc(L"a", 1, L"airplane", 8));
        CHECK(artc(L"a", 1, L"english", 7));
        CHECK(artc(L"a", 1, L"irish", 5));
        CHECK(artc(L"a", 1, L"orange", 6));
        CHECK(artc(L"a", 1, L"undertow", 8));
        CHECK(artc(L"a", 1, L"onerous", 7));
        CHECK(artc(L"a", 1, L"year", 4) == false);

        CHECK(artc(L"a", 1, L"Airplane", 8));
        CHECK(artc(L"a", 1, L"English", 7));
        CHECK(artc(L"a", 1, L"Irish", 5));
        CHECK(artc(L"a", 1, L"Orange", 6));
        CHECK(artc(L"a", 1, L"Undertow", 8));
        CHECK(artc(L"a", 1, L"Onerous", 7));
        CHECK(artc(L"a", 1, L"Year", 4) == false);

        CHECK(artc(L"a", 1, L"on-campus", 9));
        CHECK(!artc(L"an", 2, L"on-campus", 9));
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

        CHECK(artc(L"a", 1, L"are", 3) == false);
        CHECK(artc(L"a", 1, L"area", 4));

        CHECK(artc(L"a", 1, L"is", 2) == false);
        CHECK(artc(L"a", 1, L"istanbul", 8));

        CHECK(artc(L"a", 1, L"and", 3) == false);
        CHECK(artc(L"a", 1, L"anderson", 8));

        CHECK(artc(L"a", 1, L"ore", 3));
        CHECK(artc(L"a", 1, L"or", 2) == false);

        CHECK(artc(L"a", 1, L"Euro", 4) == false);
        CHECK(artc(L"a", 1, L"one", 3) == false);
        // these 2 should be wrong
        CHECK(artc(L"a", 1, L"onerous", 7));
        CHECK(artc(L"a", 1, L"on-campus", 9));
        CHECK(artc(L"a", 1, L"one-sided", 9) == false);
        CHECK(artc(L"a", 1, L"unicycle", 8) == false);
        CHECK(artc(L"a", 1, L"universal", 9) == false);
        CHECK(artc(L"a", 1, L"united", 6) == false);
        CHECK(artc(L"a", 1, L"unix", 4) == false);
        CHECK(artc(L"a", 1, L"useful", 6) == false);
        CHECK(artc(L"a", 1, L"Utopia", 6) == false);
        CHECK(artc(L"a", 1, L"Ukranian", 8) == false);
        CHECK(artc(L"a", 1, L"uterus", 6) == false);
        CHECK(artc(L"a", 1, L"urethra", 7) == false);
        CHECK(artc(L"a", 1, L"u-turn", 6) == false);
        CHECK(artc(L"a", 1, L"utility", 7) == false);
        CHECK(artc(L"a", 1, L"using", 5) == false);
        CHECK(artc(L"a", 1, L"uninteresting", 13));
        CHECK(artc(L"a", 1, L"unimpressed", 11));
        CHECK(artc(L"a", 1, L"unidentified", 12));
        CHECK(artc(L"a", 1, L"unilateral", 10) == false);
        // a Sat (like Saturday) is OK
        CHECK(artc(L"a", 1, L"Sat", 3) == false);
        // a SAT (like the text) is wrong
        CHECK(artc(L"a", 1, L"SAT", 3));

        CHECK(artc(L"an", 2, L"utility", 7));
        CHECK(artc(L"an", 2, L"u-turn", 6));
        CHECK(artc(L"an", 2, L"Euro", 4));
        CHECK(artc(L"an", 2, L"one", 3));
        CHECK(artc(L"an", 2, L"one-sided", 9));
        CHECK(artc(L"an", 2, L"unicycle", 8));
        CHECK(artc(L"an", 2, L"useful", 6));
        CHECK(artc(L"an", 2, L"Utopia", 6));
        CHECK(artc(L"an", 2, L"Ukranian", 8));
        CHECK(artc(L"an", 2, L"uterus", 6));
        CHECK(artc(L"an", 2, L"urethra", 7));
        CHECK(artc(L"an", 2, L"using", 5));
        CHECK(artc(L"an", 2, L"uninteresting", 13) == false);
        CHECK(artc(L"an", 2, L"unimpressed", 11) == false);
        CHECK(artc(L"an", 2, L"unidentified", 12) == false);
        CHECK(artc(L"an", 2, L"unilateral", 10));
        // a Sat (like Saturday) is OK
        CHECK(artc(L"an", 2, L"Sat", 3));
        // a SAT (like the text) is wrong
        CHECK(artc(L"an", 2, L"SAT", 3) == false);
        }
    SECTION("A Letters")
        {
        grammar::is_incorrect_english_article artc;

        CHECK(artc(L"a", 1, L"x-ray", 5));
        CHECK(artc(L"a", 1, L"x'x", 3));
        CHECK(artc(L"a", 1, L"u", 1) == false);
        CHECK(artc(L"a", 1, L"u.s.", 4) == false);
        CHECK(artc(L"a", 1, L"h1n1", 4));

        CHECK(artc(L"an", 2, L"x-ray", 5) == false);
        CHECK(artc(L"an", 2, L"x'x", 3) == false);
        CHECK(artc(L"an", 2, L"u", 1));
        CHECK(artc(L"an", 2, L"u.s.", 4));
        CHECK(artc(L"an", 2, L"h1n1", 4) == false);
        }
    SECTION("AN Simple")
        {
        grammar::is_incorrect_english_article artc;

        CHECK(artc(L"an", 2, L"plane", 5));
        CHECK(artc(L"an", 2, L"zoo", 3));
        CHECK(artc(L"an", 2, L"car", 3));
        CHECK(artc(L"an", 2, L"dog", 3));
        CHECK(artc(L"an", 2, L"net", 3));

        CHECK(artc(L"an", 2, L"Plane", 5));
        CHECK(artc(L"an", 2, L"Zoo", 3));
        CHECK(artc(L"an", 2, L"Car", 3));
        CHECK(artc(L"an", 2, L"Dog", 3));
        CHECK(artc(L"an", 2, L"Net", 3));
        }
    SECTION("AN Vowels")
        {
        grammar::is_incorrect_english_article artc;

        CHECK(artc(L"an", 2, L"airplane", 8) == false);
        CHECK(artc(L"an", 2, L"english", 7) == false);
        CHECK(artc(L"an", 2, L"irish", 5) == false);
        CHECK(artc(L"an", 2, L"orange", 6) == false);
        CHECK(artc(L"an", 2, L"undertow", 8) == false);
        CHECK(artc(L"an", 2, L"year", 4) == false);

        CHECK(artc(L"an", 2, L"Airplane", 8) == false);
        CHECK(artc(L"an", 2, L"English", 7) == false);
        CHECK(artc(L"an", 2, L"Irish", 5) == false);
        CHECK(artc(L"an", 2, L"Orange", 6) == false);
        CHECK(artc(L"an", 2, L"Undertow", 8) == false);
        CHECK(artc(L"an", 2, L"Year", 4) == false);
        }
    SECTION("AN Exceptions")
        {
        grammar::is_incorrect_english_article artc;

        CHECK(artc(L"a", 1, L"hysterical", 10) == false);
        CHECK(artc(L"an", 2, L"honour", 6) == false);
        CHECK(artc(L"an", 2, L"honey", 5));
        CHECK(artc(L"an", 2, L"hour", 4) == false);
        CHECK(artc(L"an", 2, L"honest", 6) == false);
        CHECK(artc(L"an", 2, L"house", 5));
        CHECK(artc(L"an", 2, L"historical", 10));
        CHECK(artc(L"an", 2, L"history", 7));
        CHECK(artc(L"an", 2, L"heir", 4) == false);

        CHECK(artc(L"a", 1, L"Hysterical", 10) == false);
        CHECK(artc(L"an", 2, L"Honour", 6) == false);
        CHECK(artc(L"an", 2, L"Honey", 5));
        CHECK(artc(L"an", 2, L"Hour", 4) == false);
        CHECK(artc(L"an", 2, L"Honest", 6) == false);
        CHECK(artc(L"an", 2, L"House", 5));
        CHECK(artc(L"an", 2, L"Historical", 10));
        CHECK(artc(L"an", 2, L"History", 7));
        CHECK(artc(L"an", 2, L"Heir", 4) == false);

        CHECK(artc(L"an", 2, L"hysterical", 10));
        CHECK(artc(L"a", 1, L"honour", 6));
        CHECK(artc(L"a", 1, L"honey", 5) == false);
        CHECK(artc(L"a", 1, L"hour", 4));
        CHECK(artc(L"a", 1, L"honest", 6));
        CHECK(artc(L"a", 1, L"house", 5) == false);
        CHECK(artc(L"a", 1, L"historical", 10) == false);
        CHECK(artc(L"a", 1, L"history", 7) == false);
        CHECK(artc(L"a", 1, L"heir", 4));
        }
    SECTION("AN Acronyms And Letters")
        {
        grammar::is_incorrect_english_article artc;

        CHECK(artc(L"an", 2, L"HMS", 3) == false);
        CHECK(artc(L"an", 2, L"LLC", 3) == false);
        CHECK(artc(L"an", 2, L"MST", 3) == false);
        CHECK(artc(L"an", 2, L"NWP", 3) == false);
        CHECK(artc(L"an", 2, L"RR", 2) == false);
        CHECK(artc(L"an", 2, L"SS", 2) == false);
        CHECK(artc(L"an", 2, L"XBT", 3) == false);
        // the vowel makes it be seen as a word
        CHECK(artc(L"an", 2, L"HAM", 3));

        CHECK(artc(L"an", 2, L"H1N1", 3) == false);

        CHECK(artc(L"an", 2, L"f", 1) == false);
        CHECK(artc(L"an", 2, L"h", 1) == false);
        CHECK(artc(L"an", 2, L"l", 1) == false);
        CHECK(artc(L"an", 2, L"m", 1) == false);
        CHECK(artc(L"an", 2, L"n", 1) == false);
        CHECK(artc(L"an", 2, L"r", 1) == false);
        CHECK(artc(L"an", 2, L"s", 1) == false);
        CHECK(artc(L"an", 2, L"x", 1) == false);
        CHECK(artc(L"an", 2, L"u", 1));

        CHECK(artc(L"an", 2, L"F", 1) == false);
        CHECK(artc(L"an", 2, L"H", 1) == false);
        CHECK(artc(L"an", 2, L"L", 1) == false);
        CHECK(artc(L"an", 2, L"M", 1) == false);
        CHECK(artc(L"an", 2, L"N", 1) == false);
        CHECK(artc(L"an", 2, L"R", 1) == false);
        CHECK(artc(L"an", 2, L"S", 1) == false);
        CHECK(artc(L"an", 2, L"X", 1) == false);
        CHECK(artc(L"an", 2, L"U", 1));
        }
    SECTION("Numbers")
        {
        grammar::is_incorrect_english_article artc;

        CHECK(artc(L"an", 2, L"0-char", 6));
        CHECK(artc(L"a", 1, L"0-char", 6) == false);

        CHECK(artc(L"an", 2, L"1-char", 6));
        CHECK(artc(L"a", 1, L"1-char", 6) == false);

        CHECK(artc(L"an", 2, L"2-char", 6));
        CHECK(artc(L"a", 1, L"2-char", 6) == false);

        CHECK(artc(L"an", 2, L"3-char", 6));
        CHECK(artc(L"a", 1, L"3-char", 6) == false);

        CHECK(artc(L"an", 2, L"4-char", 6));
        CHECK(artc(L"a", 1, L"4-char", 6) == false);

        CHECK(artc(L"an", 2, L"5-char", 6));
        CHECK(artc(L"a", 1, L"5-char", 6) == false);

        CHECK(artc(L"an", 2, L"6-char", 6));
        CHECK(artc(L"a", 1, L"6-char", 6) == false);

        CHECK(artc(L"an", 2, L"7-char", 6));
        CHECK(artc(L"a", 1, L"7-char", 6) == false);

        CHECK(artc(L"an", 2, L"8-char", 6) == false);
        CHECK(artc(L"a", 1, L"8-char", 6));

        CHECK(artc(L"an", 2, L"9-char", 6));
        CHECK(artc(L"a", 1, L"9-char", 6) == false);

        CHECK(artc(L"an", 2, L"0char", 5));
        CHECK(artc(L"a", 1, L"0char", 5) == false);

        CHECK(artc(L"an", 2, L"1char", 5));
        CHECK(artc(L"a", 1, L"1char", 5) == false);

        CHECK(artc(L"an", 2, L"2char", 5));
        CHECK(artc(L"a", 1, L"2char", 5) == false);

        CHECK(artc(L"an", 2, L"3char", 5));
        CHECK(artc(L"a", 1, L"3char", 5) == false);

        CHECK(artc(L"an", 2, L"4char", 5));
        CHECK(artc(L"a", 1, L"4char", 5) == false);

        CHECK(artc(L"an", 2, L"5char", 5));
        CHECK(artc(L"a", 1, L"5char", 5) == false);

        CHECK(artc(L"an", 2, L"6char", 5));
        CHECK(artc(L"a", 1, L"6char", 5) == false);

        CHECK(artc(L"an", 2, L"7char", 5));
        CHECK(artc(L"a", 1, L"7char", 5) == false);

        CHECK(artc(L"an", 2, L"8char", 5) == false);
        CHECK(artc(L"a", 1, L"8char", 5));

        CHECK(artc(L"an", 2, L"9char", 5));
        CHECK(artc(L"a", 1, L"9char", 5) == false);

        // don't bother with more complicated numbers
        CHECK(artc(L"an", 2, L"1,800", 5) == false);
        CHECK(artc(L"a", 1, L"1,800", 5) == false);

        CHECK(artc(L"an", 2, L"1st", 3));
        CHECK(artc(L"a", 1, L"1st", 3) == false);

        CHECK(artc(L"an", 2, L"21st", 4));
        CHECK(artc(L"a", 1, L"21st", 4) == false);

        CHECK(artc(L"an", 2, L"2nd", 3));
        CHECK(artc(L"a", 1, L"2nd", 3) == false);

        CHECK(artc(L"an", 2, L"3rd", 3));
        CHECK(artc(L"a", 1, L"3rd", 3) == false);

        CHECK(artc(L"an", 2, L"11th", 4) == false);
        CHECK(artc(L"a", 1, L"11th", 4));

        CHECK(artc(L"an", 2, L"8th", 3) == false);
        CHECK(artc(L"a", 1, L"8th", 3));

        CHECK(artc(L"an", 2, L"4th", 3));
        CHECK(artc(L"a", 1, L"4th", 3) == false);
        }
    SECTION("Year")
        {
        grammar::is_incorrect_english_article artc;

        CHECK(artc(L"a", 1, L"1087", 4) == false);
        CHECK(artc(L"a", 1, L"1187", 4));
        CHECK(artc(L"a", 1, L"1287", 4) == false);
        CHECK(artc(L"a", 1, L"1387", 4) == false);
        CHECK(artc(L"a", 1, L"1487", 4) == false);
        CHECK(artc(L"a", 1, L"1587", 4) == false);
        CHECK(artc(L"a", 1, L"1687", 4) == false);
        CHECK(artc(L"a", 1, L"1787", 4) == false);
        CHECK(artc(L"a", 1, L"1887", 4));
        CHECK(artc(L"a", 1, L"1987", 4) == false);
        CHECK(artc(L"a", 1, L"2087", 4) == false);

        CHECK(artc(L"an", 2, L"1087", 4));
        CHECK(artc(L"an", 2, L"1187", 4) == false);
        CHECK(artc(L"an", 2, L"1287", 4));
        CHECK(artc(L"an", 2, L"1387", 4));
        CHECK(artc(L"an", 2, L"1487", 4));
        CHECK(artc(L"an", 2, L"1587", 4));
        CHECK(artc(L"an", 2, L"1687", 4));
        CHECK(artc(L"an", 2, L"1787", 4));
        CHECK(artc(L"an", 2, L"1887", 4) == false);
        CHECK(artc(L"an", 2, L"1987", 4));
        CHECK(artc(L"an", 2, L"2087", 4));
        }

    SECTION("Time")
        {
        grammar::is_incorrect_english_article artc;

        CHECK(artc(L"a", 1, L"10:87", 5) == false);
        CHECK(artc(L"a", 1, L"11:87", 5));
        CHECK(artc(L"a", 1, L"12:87", 5) == false);
        CHECK(artc(L"a", 1, L"13:87", 5) == false);
        CHECK(artc(L"a", 1, L"14:87", 5) == false);
        CHECK(artc(L"a", 1, L"15:87", 5) == false);
        CHECK(artc(L"a", 1, L"16:87", 5) == false);
        CHECK(artc(L"a", 1, L"17:87", 5) == false);
        CHECK(artc(L"a", 1, L"18:87", 5));
        CHECK(artc(L"a", 1, L"19:87", 5) == false);
        CHECK(artc(L"a", 1, L"20:87", 5) == false);

        CHECK(artc(L"an", 2, L"10:87", 5));
        CHECK(artc(L"an", 2, L"11:87", 5) == false);
        CHECK(artc(L"an", 2, L"12:87", 5));
        CHECK(artc(L"an", 2, L"13:87", 5));
        CHECK(artc(L"an", 2, L"14:87", 5));
        CHECK(artc(L"an", 2, L"15:87", 5));
        CHECK(artc(L"an", 2, L"16:87", 5));
        CHECK(artc(L"an", 2, L"17:87", 5));
        CHECK(artc(L"an", 2, L"18:87", 5) == false);
        CHECK(artc(L"an", 2, L"19:87", 5));
        CHECK(artc(L"an", 2, L"20:87", 5));
        }
    }
