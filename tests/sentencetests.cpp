#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>
#include <fstream>
#include <iostream>
#include "../src/indexing/article.h"
#include "../src/indexing/word_collection.h"
#include "../src/indexing/spanish_syllabize.h"
#include "../src/indexing/german_syllabize.h"
#include "../src/indexing/word.h"

using namespace grammar;

using MYWORD = word<traits::case_insensitive_ex,
    stemming::english_stem<std::basic_string<wchar_t, traits::case_insensitive_ex> > >;

TEST_CASE("Sentences", "[sentence]")
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
    word_list Stop_list;

    std::wifstream inputFile(L"WesternAbbr.txt");
    if (!inputFile.is_open())
        { FAIL("Abbreviation file failed to load."); }
    std::wstring line;
    std::wstring buffer;
    while (std::getline(inputFile, line))
        { buffer.append(L"\n").append(line); }

    is_abbreviation::get_abbreviations().load_words(buffer.c_str(), true, false);

    SECTION("EndingWithDashEndingSentence")
        {
        grammar::is_end_of_sentence sent(false);

        // should break into two sentences
        const wchar_t* text = L"how about–\"\nThe man interrupted him.";
        CHECK(sent(text, wcslen(text), 9, 1, 0));

        text = L"how about—\"\nThe man interrupted him";
        CHECK(sent(text, wcslen(text), 9, 1, 0));

        text = L"how about–”\nThe man interrupted him";
        CHECK(sent(text, wcslen(text), 9, 1, 0));

        text = L"how about—”\nThe man interrupted him";
        CHECK(sent(text, wcslen(text), 9, 1, 0));

        // regular dash shouldn't be seen as end thought
        text = L"how about-”\nThe man interrupted him";
        CHECK_FALSE(sent(text, wcslen(text), 9, 1, 0));

        // quote after dash is required to force a new sentence
        text = L"how about—\nThe man interrupted him";
        CHECK_FALSE(sent(text, wcslen(text), 9, 1, 0));
        }

    SECTION("EndingWithDashPartOfSentence")
        {
        grammar::is_end_of_sentence sent(false);

        // should all be one sentence, dash is part of it
        const wchar_t* text = L"how about–\" James trailed off.";
        CHECK_FALSE(sent(text, wcslen(text), 9, 1, 0));

        text = L"how about—\" James trailed off.";
        CHECK_FALSE(sent(text, wcslen(text), 9, 1, 0));

        text = L"how about–” James trailed off.";
        CHECK_FALSE(sent(text, wcslen(text), 9, 1, 0));

        text = L"how about—” James trailed off.";
        CHECK_FALSE(sent(text, wcslen(text), 9, 1, 0));

        // same when quote after dash
        text = L"how about–\"James's quote here\"—";
        CHECK_FALSE(sent(text, wcslen(text), 9, 1, 0));

        text = L"how about—\"James's quote here\"—";
        CHECK_FALSE(sent(text, wcslen(text), 9, 1, 0));

        text = L"(how about–”James's quote here\"—";
        CHECK_FALSE(sent(text, wcslen(text), 9, 1, 0));

        text = L"(how about—”James's quote here\"—";
        CHECK_FALSE(sent(text, wcslen(text), 9, 1, 0));

        text = L"how about–“James's quote here\"—)";
        CHECK_FALSE(sent(text, wcslen(text), 9, 1, 0));

        text = L"how about—“James's quote here\"—";
        CHECK_FALSE(sent(text, wcslen(text), 9, 1, 0));
        }

    SECTION("ProceededByComma")
        {
        grammar::is_end_of_sentence sent(false);

        const wchar_t* text = L"who’s who\",? who knows whom,? etc.";
        CHECK_FALSE(sent(text, wcslen(text), 11, 2, 2));
        }

    SECTION("QuestionMarkInsideParen")
        {
        grammar::is_end_of_sentence sent(false);

        const wchar_t* text = L"verantwortlich?\") tabelliert werden.";
        CHECK_FALSE(sent(text, wcslen(text), 14, 0, 0));

        const wchar_t* text2 = L"verantwortlich?\") Tabelliert werden.";
        CHECK(sent(text2, wcslen(text2), 14, 0, 0));

        const wchar_t* text3 = L"verantwortlich?\") -tabelliert werden.";
        CHECK(sent(text3, wcslen(text3), 14, 0, 0));

        const wchar_t* text4 = L"verantwortlich?\")";
        CHECK(sent(text4, wcslen(text4), 14, 0, 0));
        }
    SECTION("AmbiguousAbbreviation")
        {
        grammar::is_end_of_sentence sent(false);
        const wchar_t* text = L"Del Mar. It is ready on Mar. 1st. Give it to Jan. It is ready Jan. 1st.";
        CHECK(sent(text, wcslen(text), 7, 4, 0));
        CHECK_FALSE(sent(text, wcslen(text), 26, 23, 0));
        CHECK(sent(text, wcslen(text), 48, 45, 0));
        CHECK_FALSE(sent(text, wcslen(text), 65, 62, 0));
        }

    SECTION("Etc")
        {
        const wchar_t* text = L"On a scale of 1-10, I would give it etc.";
        grammar::is_end_of_sentence sent(false);
        CHECK(sent(text, wcslen(text), 39, 36, 10));
        text = L"On a scale of 1-10, I would give it ETC.";
        CHECK(sent(text, wcslen(text), 39, 36, 10));
        }

    SECTION("Etc2")
        {
        const wchar_t* text = L"On a scale of 1-10, I would give it etc.) end of sentence.";
        grammar::is_end_of_sentence sent(false);
        CHECK_FALSE(sent(text, wcslen(text), 39, 36, 10));
        text = L"On a scale of 1-10, I would give it ETC.) end of sentence.";
        CHECK_FALSE(sent(text, wcslen(text), 39, 36, 10));
        }
    SECTION("Etc3")
        {
        const wchar_t text[] = L"On a scale of 1-10, I would give it etc. end of sentence.";
        grammar::is_end_of_sentence sent(false);
        CHECK_FALSE(sent(text, wcslen(text), 39, 36, 10));
        }
    SECTION("Usw")
        {
        const wchar_t* text = L"On a scale of 1-10, I would give it usw. end of sentence.";
        grammar::is_end_of_sentence sent(false);
        CHECK_FALSE(sent(text, wcslen(text), 39, 36, 10));
        text = L"On a scale of 1-10, I would give it USW. end of sentence.";
        CHECK_FALSE(sent(text, wcslen(text), 39, 36, 10));
        }
    SECTION("EtcEndingSentence")
        {
        const wchar_t text[] = L"On a scale of 1-10, I would give it etc. New sentence.";
        grammar::is_end_of_sentence sent(false);
        CHECK(sent(text, wcslen(text), 39, 36, 10));
        }
    SECTION("UswEndingSentence")
        {
        const wchar_t text[] = L"On a scale of 1-10, I would give it usw. New sentence.";
        grammar::is_end_of_sentence sent(false);
        CHECK(sent(text, wcslen(text), 39, 36, 10));
        }
    SECTION("EtcEndingSentenceDoubleSpaced")
        {
        const wchar_t text[] = L"On a scale of 1-10, I would give it etc.  New sentence.";
        grammar::is_end_of_sentence sent(false);
        CHECK(sent(text, wcslen(text), 39, 36, 10));
        }
    SECTION("UswEndingSentenceDoubleSpaced")
        {
        const wchar_t text[] = L"On a scale of 1-10, I would give it usw.  New sentence.";
        grammar::is_end_of_sentence sent(false);
        CHECK(sent(text, wcslen(text), 39, 36, 10));
        }
    SECTION("BracesEndingSentence")
        {
        const wchar_t text[] = L"A FunctionCall()\". Another sentence.";
        grammar::is_end_of_sentence sent(false);
        CHECK(sent(text, wcslen(text), 17, 2, 1));
        }
    //bad grammar, but a lot of people do this and we have to accept that.
    SECTION("LowercaseAtStartOfNextSentence")
        {
        const wchar_t text[] = L"On a scale of 1-10, I would give it a 4. the end!";
        grammar::is_end_of_sentence sent(false);
        CHECK(sent(text, wcslen(text), 39, 38, 10));
        }
    SECTION("GarbageCharacter")
        {
        const wchar_t text[] = L"On a scale of 1-10, I would give it a 4. / The end!";
        grammar::is_end_of_sentence sent(false);
        CHECK(sent(text, wcslen(text), 39, 38, 10));
        }
    SECTION("LegitCharacter")
        {
        const wchar_t text[] = L"On a scale of 1-10, I would give it a 4. $5 you owe me!";
        grammar::is_end_of_sentence sent(false);
        CHECK(sent(text, wcslen(text), 39, 38, 10));
        }
    SECTION("Quotes")
        {
        const wchar_t text[] = L"On a scale of 1-10, I would give it a 4.’ The End!";
        grammar::is_end_of_sentence sent(false);
        CHECK(sent(text, wcslen(text), 39, 38, 10));
        }
    SECTION("Quotes2")
        {
        const wchar_t text[] = L"On a scale of 1-10, I would give it a 4.” The End!";
        grammar::is_end_of_sentence sent(false);
        CHECK(sent(text, wcslen(text), 39, 38, 10));
        }
    SECTION("Quotes3")
        {
        const wchar_t text[] = L"On a scale of 1-10, I would give it a 4”. The End!";
        grammar::is_end_of_sentence sent(false);
        CHECK(sent(text, wcslen(text), 40, 38, 10));
        }
    SECTION("Quotes4")
        {
        const wchar_t text[] = L"On a scale of 1-10, I would give it a 4.’ he said.";
        grammar::is_end_of_sentence sent(false);
        CHECK_FALSE(sent(text, wcslen(text), 39, 38, 10));
        }
    SECTION("Quotes5")
        {
        const wchar_t text[] = L"On a scale of 1-10, I would give it a 4.\" he said.";
        grammar::is_end_of_sentence sent(false);
        CHECK_FALSE(sent(text, wcslen(text), 39, 38, 10));
        }
    SECTION("Number")
        {
        const wchar_t text[] = L"I give 110%.  Seriously, I do.";
        grammar::is_end_of_sentence sent(false);
        CHECK(sent(text, wcslen(text), 11, 7, 2));
        }
    SECTION("Abbreviation")
        {
        const wchar_t text[] = L"I am Mr. Smithe.  My name is C. Blake Smithe.";
        grammar::is_end_of_sentence sent(false);
        CHECK_FALSE(sent(text, wcslen(text), 7, 5, 2));
        }
    SECTION("AbbreviationNumberOfException")
        {
        const wchar_t text[] = L"Is the No.  of people 5? I said no. Of course I mean it!";
        grammar::is_end_of_sentence sent(false);
        CHECK_FALSE(sent(text, wcslen(text), 9, 7, 2));
        CHECK(sent(text, wcslen(text), 34, 32, 2));
        }
    SECTION("AbbreviationNumberWithNumber")
        {
        const wchar_t text[] = L"Is the No. 5?";
        grammar::is_end_of_sentence sent(false);
        CHECK_FALSE(sent(text, wcslen(text), 9, 7, 2));
        }
    SECTION("AbbreviationNumberWithPoundedNumber")
        {
        const wchar_t text[] = L"Is the No. #5?";
        grammar::is_end_of_sentence sent(false);
        CHECK_FALSE(sent(text, wcslen(text), 9, 7, 2));
        }
    SECTION("AbbreviationVersusException")
        {
        const wchar_t text[] = L"t-f. vs. them.";
        grammar::is_end_of_sentence sent(false);
        CHECK_FALSE(sent(text, wcslen(text), 3, 0, 0));
        CHECK_FALSE(sent(text, wcslen(text), 7, 5, 1));
        }
    SECTION("Abbreviation2")
        {
        const wchar_t text[] = L"I am mr. Smithe.  My name is C. Blake Smithe.";
        grammar::is_end_of_sentence sent(false);
        CHECK_FALSE(sent(text, wcslen(text), 7, 5, 2));
        }
    SECTION("Abbreviation3")
        {
        const wchar_t text[] = L"I am Sra. Smithe.  My name is C. Blake Smithe.";
        grammar::is_end_of_sentence sent(false);
        CHECK_FALSE(sent(text, wcslen(text), 8, 5, 2));
        }
    // abbreviation at the end of paragraph can end a sentence.
    SECTION("Abbreviation4")
        {
        const wchar_t text[] = L"I am Inc.\nSmithe.  My name is C. Blake Smithe.";
        grammar::is_end_of_sentence sent(false);
        CHECK(sent(text, wcslen(text), 8, 5, 2));
        }
    SECTION("Abbreviation5")
        {
        const wchar_t text[] = L"I am Inc. \nSmithe.  My name is C. Blake Smithe.";
        grammar::is_end_of_sentence sent(false);
        CHECK(sent(text, wcslen(text), 8, 5, 2));
        }
    // test unknown abbreviations that are nothing but consonants
    SECTION("Abbreviation6")
        {
        const wchar_t text[] = L"The mfgs. cfgs. IS very complx.";
        grammar::is_end_of_sentence sent(false);
        CHECK_FALSE(sent(text, wcslen(text), 8, 4, 1));
        CHECK_FALSE(sent(text, wcslen(text), 14, 10, 2));
        CHECK(sent(text, wcslen(text), 30, 24, 5));
        }
    SECTION("Abbreviation7")
        {
        const wchar_t text[] = L"The Mfg. CDs. It's very complx.";
        grammar::is_end_of_sentence sent(false);
        CHECK_FALSE(sent(text, wcslen(text), 7, 4, 1));
        CHECK(sent(text, wcslen(text), 12, 9, 2));
        }
    SECTION("AbbreviationPossesive")
        {
        document<word_case_insensitive_no_stem> doc(L"", &ENsyllabizer, &ENStemmer, &is_conjunction, &pmap, &copyrightPMap, &citationPMap, &Known_proper_nouns, &Known_personal_nouns, &Known_spellings, &Secondary_known_spellings, &Programming_known_spellings, &Stop_list);
        const wchar_t* text = L"The Co.'s books are Co.'s";
        doc.load_document(text, wcslen(text), false, false, false, false);
        CHECK(doc.get_words().at(1) == L"Co.'s");
        CHECK(doc.get_words().at(4) == L"Co.'s");
        }
    SECTION("AbbreviationQuestion")
        {
        document<word_case_insensitive_no_stem> doc(L"", &ENsyllabizer, &ENStemmer, &is_conjunction, &pmap, &copyrightPMap, &citationPMap, &Known_proper_nouns, &Known_personal_nouns, &Known_spellings, &Secondary_known_spellings, &Programming_known_spellings, &Stop_list);
        const wchar_t* text = L"He's a dr.?  Really?";
        doc.load_document(text, wcslen(text), false, false, false, false);
        CHECK(doc.get_sentence_count() == 2);
        }
    SECTION("AbbreviationInParen")
        {
        document<word_case_insensitive_no_stem> doc(L"", &ENsyllabizer, &ENStemmer, &is_conjunction, &pmap, &copyrightPMap, &citationPMap, &Known_proper_nouns, &Known_personal_nouns, &Known_spellings, &Secondary_known_spellings, &Programming_known_spellings, &Stop_list);
        const wchar_t* text = L"He's a doctor (dr.) and a good one.";
        doc.load_document(text, wcslen(text), false, false, false, false);
        CHECK(doc.get_sentence_count() == 1);
        }
    SECTION("AbbreviationInTwoParens")
        {
        document<word_case_insensitive_no_stem> doc(L"", &ENsyllabizer, &ENStemmer, &is_conjunction, &pmap, &copyrightPMap, &citationPMap, &Known_proper_nouns, &Known_personal_nouns, &Known_spellings, &Secondary_known_spellings, &Programming_known_spellings, &Stop_list);
        const wchar_t* text = L"He's a [doctor (dr.)] and a good one.";
        doc.load_document(text, wcslen(text), false, false, false, false);
        CHECK(doc.get_sentence_count() == 1);
        }
    SECTION("AbbreviationQuoteAtEnd")
        {
        document<word_case_insensitive_no_stem> doc(L"", &ENsyllabizer, &ENStemmer, &is_conjunction, &pmap, &copyrightPMap, &citationPMap, &Known_proper_nouns, &Known_personal_nouns, &Known_spellings, &Secondary_known_spellings, &Programming_known_spellings, &Stop_list);
        const wchar_t* text = L"The Co.'s books are Co.'";
        doc.load_document(text, wcslen(text), false, false, false, false);
        CHECK(doc.get_words().at(1) == L"Co.'s");
        CHECK(doc.get_words().at(4) == L"Co.");
        }
    SECTION("AbbreviationQuoteAtEnd2")
        {
        document<word_case_insensitive_no_stem> doc(L"", &ENsyllabizer, &ENStemmer, &is_conjunction, &pmap, &copyrightPMap, &citationPMap, &Known_proper_nouns, &Known_personal_nouns, &Known_spellings, &Secondary_known_spellings, &Programming_known_spellings, &Stop_list);
        const wchar_t* text = L"The Co.'s books are good.'";
        doc.load_document(text, wcslen(text), false, false, false, false);
        CHECK(doc.get_words().at(1) == L"Co.'s");
        CHECK(doc.get_words().at(4) == L"good");
        }
    SECTION("SentenceEndRegular")
        {
        const wchar_t text[] = L"I am Mr. Smithe.  My name is C. Blake Smithe.";
        grammar::is_end_of_sentence sent(false);
        CHECK(sent(text, wcslen(text), 15, 9, 3) );
        }
    SECTION("Initial")
        {
        const wchar_t text[] = L"I am mr. Smithe.  My name is C. Blake Smithe.";
        grammar::is_end_of_sentence sent(false);
        CHECK_FALSE(sent(text, wcslen(text), 30, 29, 3));
        }
    SECTION("Initial2")
        {
        const wchar_t text[] = L"C. Blake Smithe.";
        grammar::is_end_of_sentence sent(false);
        CHECK_FALSE(sent(text, wcslen(text), 1, 0, 0));
        }
    SECTION("Initial3")
        {
        const wchar_t text[] = L"I love U.S. of A. forever!";
        grammar::is_end_of_sentence sent(false);
        CHECK_FALSE(sent(text, wcslen(text), 16, 15, 4));
        }
    SECTION("Initial4")
        {
        const wchar_t text[] = L"I love U.S. of A.";
        grammar::is_end_of_sentence sent(false);
        CHECK(sent(text, wcslen(text), 16, 15, 4));
        }
    SECTION("Initial5")
        {
        const wchar_t text[] = L"Steve I. Smith";
        grammar::is_end_of_sentence sent(false);
        CHECK_FALSE(sent(text, wcslen(text), 7, 6, 2));
        }
    SECTION("Initial6")
        {
        const wchar_t text[] = L"It was I! Oh!";
        grammar::is_end_of_sentence sent(false);
        CHECK(sent(text, wcslen(text), 8, 7, 2));
        }
    SECTION("EndOfText")
        {
        const wchar_t text[] = L"I am Mr. Smithe.  My name is C. Blake Smithe.";
        grammar::is_end_of_sentence sent(false);
        CHECK(sent(text, wcslen(text), 44, 30, 5) );
        }
    SECTION("SentenceEndQuoted")
        {
        const wchar_t text[] = L"I am Mr. Smithe.\" Hello";
        grammar::is_end_of_sentence sent(false);
        CHECK(sent(text, wcslen(text), 15, 9, 3) );
        }
    SECTION("SentenceEndQuoted2")
        {
        const wchar_t text[] = L"I am Mr. Smithe.” —anonymous.";
        grammar::is_end_of_sentence sent(false);
        CHECK(sent(text, wcslen(text), 15, 9, 3) );
        }
    SECTION("SentenceStartQuoted")
        {
        const wchar_t text[] = L"I am Mr. Smithe. \"Hello";
        grammar::is_end_of_sentence sent(false);
        CHECK(sent(text, wcslen(text), 15, 9, 3) );
        }
    SECTION("SentenceEndParenthesis")
        {
        const wchar_t text[] = L"(I am Mr. Smithe.) Hello";
        grammar::is_end_of_sentence sent(false);
        CHECK(sent(text, wcslen(text), 16, 10, 3) );
        }
    SECTION("SentenceEndParenthesis2")
        {
        const wchar_t text[] = L"(I am Mr. Smithe). Hello";
        grammar::is_end_of_sentence sent(false);
        CHECK(sent(text, wcslen(text), 17, 10, 3) );
        }
    SECTION("SentenceEndParenthesisWithSpaces")
        {
        const wchar_t text[] = L"(I am Mr. Smithe) \t. Hello";
        grammar::is_end_of_sentence sent(false);
        CHECK(sent(text, wcslen(text), 19, 10, 3) );
        }
    SECTION("SentenceEndBraces")
        {
        const wchar_t text[] = L"[I am Mr. Smithe.] Hello";
        grammar::is_end_of_sentence sent(false);
        CHECK(sent(text, wcslen(text), 16, 10, 3) );
        }
    SECTION("SentenceEndBraces2")
        {
        const wchar_t text[] = L"[I am Mr. Smithe]. Hello";
        grammar::is_end_of_sentence sent(false);
        CHECK(sent(text, wcslen(text), 17, 10, 3) );
        }
    SECTION("SentenceEndBracesWithSpaces")
        {
        const wchar_t text[] = L"[I am Mr. Smithe] \t. Hello";
        grammar::is_end_of_sentence sent(false);
        CHECK(sent(text, wcslen(text), 19, 10, 3) );
        }
    SECTION("TripleBangs")
        {
        const wchar_t text[] = L"Uh!!! Oh!";
        grammar::is_end_of_sentence sent(false);
        CHECK(sent(text, wcslen(text), 2, 0, 0) );
        }
    SECTION("Interrobang")
        {
        const wchar_t text[] = L"Uh?! Oh!";
        grammar::is_end_of_sentence sent(false);
        CHECK(sent(text, wcslen(text), 2, 0, 0) );
        }
    SECTION("Ellipses")
        {
        const wchar_t text[] = L"Uh... oh!";
        grammar::is_end_of_sentence sent(false);
        CHECK_FALSE(sent(text, wcslen(text), 2, 0, 0));
        }
    SECTION("EllipsesSpaces")
        {
        const wchar_t text[] = L"Uh . . . oh!";
        grammar::is_end_of_sentence sent(false);
        CHECK_FALSE(sent(text, wcslen(text), 3, 0, 0));
        }
    SECTION("Ellipses2")
        {
        const wchar_t text[] = L"simple, but...\" he said, \"hard";
        grammar::is_end_of_sentence sent(false);
        CHECK_FALSE(sent(text, wcslen(text), 11, 8, 0));
        }
    SECTION("Ellipses3")
        {
        const wchar_t text[] = L"simple, but. . . . Here is a new sentence";
        grammar::is_end_of_sentence sent(false);
        CHECK(sent(text, wcslen(text), 11, 8, 0) );
        }
    SECTION("Ellipses4")
        {
        const wchar_t text[] = L"simple, but....\" Here is a new sentence";
        grammar::is_end_of_sentence sent(false);
        CHECK(sent(text, wcslen(text), 11, 8, 0));
        }
    SECTION("Ellipses5")
        {
        const wchar_t text[] = L"Uh...oh!";
        grammar::is_end_of_sentence sent(false);
        CHECK_FALSE(sent(text, wcslen(text), 2, 0, 0));
        }
    SECTION("Ellipses6")
        {
        const wchar_t text[] = L"simple, but...\nHere is a new sentence";
        grammar::is_end_of_sentence sent(false);
        CHECK(sent(text, wcslen(text), 11, 8, 0) );
        }
    SECTION("Ellipses7")
        {
        const wchar_t text[] = L"Uh… Oh!";
        grammar::is_end_of_sentence sent(false);
        CHECK(sent(text, wcslen(text), 2, 0, 0));
        }
    SECTION("Ellipses8")
        {
        const wchar_t text[] = L"Uh…Oh!";
        grammar::is_end_of_sentence sent(false);
        CHECK_FALSE(sent(text, wcslen(text), 2, 0, 0));
        }
    }