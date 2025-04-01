/********************************************************************************
 * Copyright (c) 2005-2025 Blake Madden
 *
 * This program and the accompanying materials are made available under the
 * terms of the Eclipse Public License 2.0 which is available at
 * https://www.eclipse.org/legal/epl-2.0.
 *
 * SPDX-License-Identifier: EPL-2.0
 *
 * Contributors:
 *   Blake Madden - initial implementation
 ********************************************************************************/

#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>
#include "../src/indexing/article.h"
#include "../src/indexing/word_collection.h"
#include "../src/indexing/word.h"

// clang-format off
// NOLINTBEGIN

using namespace grammar;

using MYWORD = word<traits::case_insensitive_ex,
    stemming::english_stem<std::basic_string<wchar_t, traits::case_insensitive_ex> > >;

extern word_list Stop_list;

TEST_CASE("Ending with dash ending sentence", "[sentence]")
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

TEST_CASE("Ending with plus", "[sentence]")
    {
    grammar::is_end_of_sentence sent(false);

    // should break into two sentences
    const wchar_t* text = L"I have 5+. That is my score.";
    CHECK(sent(text, wcslen(text), 9, 7, 2));

    text = L"I use C++. That is my language.";
    CHECK(sent(text, wcslen(text), 9, 6, 2));
    }

TEST_CASE("Ending with trademark", "[sentence]")
    {
    grammar::is_end_of_sentence sent(false);

    // should break into two sentences
    const wchar_t* text = L"This is my slogan.™ Some more text.";
    CHECK(sent(text, wcslen(text), 17, 10, 3));

    // won't split, needs a space
    text = L"This is my slogan.™Some more text.";
    CHECK_FALSE(sent(text, wcslen(text), 17, 10, 3));
    }

TEST_CASE("Sentences", "[sentence]")
    {
    grammar::english_syllabize ENsyllabizer;
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

    SECTION("Sentence Word Count Less")
        {
        document<MYWORD> doc(L"", &ENsyllabizer, &ENStemmer, &is_conjunction, &pmap, &copyrightPMap, &citationPMap, &Known_proper_nouns, &Known_personal_nouns, &Known_spellings, &Secondary_known_spellings, &Programming_known_spellings, &Stop_list);
        const wchar_t* text = L"Chapter 1--The Beginning of the Book\n\nIt was a night. It was a dark night.\n\nThe End of this great book";
        doc.load_document(text, wcslen(text), false, false, false, false);

        grammar::sentence_info longestSent = *(std::max_element(doc.get_sentences().begin(),
            doc.get_sentences().end() ));
        CHECK(longestSent.get_word_count() == 7);

        longestSent = *(std::max_element(doc.get_sentences().begin(),
            doc.get_sentences().end(), grammar::complete_sentence_length_less()));
        CHECK(longestSent.get_word_count() == 5);
        }
    SECTION("Sentence Word Count Greater Than Value")
        {
        document<MYWORD> doc(L"", &ENsyllabizer, &ENStemmer, &is_conjunction, &pmap, &copyrightPMap, &citationPMap, &Known_proper_nouns, &Known_personal_nouns, &Known_spellings, &Secondary_known_spellings, &Programming_known_spellings, &Stop_list);
        const wchar_t* text = L"Chapter 1--The Beginning of the Book\n\nIt was a night. It was a dark night.\n\nThe End of this great book";
        doc.load_document(text, wcslen(text), false, false, false, false);

        size_t total = std::count_if(doc.get_sentences().begin(),
                    doc.get_sentences().end(),
                    grammar::sentence_length_greater_than(3) );
        CHECK(total == 4);

        total = std::count_if(doc.get_sentences().begin(),
                    doc.get_sentences().end(),
                    grammar::complete_sentence_length_greater_than(3) );
        CHECK(total == 2);
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

TEST_CASE("Sentences Incomplete", "[sentence]")
    {
    grammar::english_syllabize ENsyllabizer;
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

    SECTION("Colon Ending Sentence")
        {
        document<MYWORD> doc(L"", &ENsyllabizer, &ENStemmer, &is_conjunction, &pmap, &copyrightPMap,
            &citationPMap, &Known_proper_nouns, &Known_personal_nouns, &Known_spellings,
            &Secondary_known_spellings, &Programming_known_spellings, &Stop_list);
        const wchar_t* text = L"Chapter, 1:\n\nIt's a dark, stormy night (at Bob's house):\n\nThe end";
        doc.load_document(text, wcslen(text), false, false, false, false);

        CHECK(doc.get_complete_sentence_count() == 2);
        CHECK(doc.get_valid_paragraph_count() == 2);
        CHECK(doc.get_valid_word_count() == 10);
        }
    SECTION("Semi Colon Ending Sentence")
        {
        document<MYWORD> doc(L"", &ENsyllabizer, &ENStemmer, &is_conjunction, &pmap,
            &copyrightPMap, &citationPMap, &Known_proper_nouns, &Known_personal_nouns,
            &Known_spellings, &Secondary_known_spellings, &Programming_known_spellings, &Stop_list);
        const wchar_t* text = L"Chapter, 1;\n\nIt's a dark, stormy night (at Bob's house);\n\nThe end";
        doc.load_document(text, wcslen(text), false, false, false, false);

        CHECK(doc.get_complete_sentence_count() == 2);
        CHECK(doc.get_valid_paragraph_count() == 2);
        CHECK(doc.get_valid_word_count() == 10);
        }
    SECTION("Semi Colon Ending All Sentence")
        {
        document<MYWORD> doc(L"", &ENsyllabizer, &ENStemmer, &is_conjunction, &pmap,
            &copyrightPMap, &citationPMap, &Known_proper_nouns, &Known_personal_nouns,
            &Known_spellings, &Secondary_known_spellings, &Programming_known_spellings, &Stop_list);
        const wchar_t* text = L"Chapter, 1;\n\nIt's a dark, stormy night (at Bob's house);\n\nThe end;";
        doc.load_document(text, wcslen(text), false, false, false, false);

        CHECK(doc.get_complete_sentence_count() == 3);
        CHECK(doc.get_valid_paragraph_count() == 3);
        CHECK(doc.get_valid_word_count() == 12);
        }
    SECTION("Semi Colon Not Ending Sentence")
        {
        document<MYWORD> doc(L"", &ENsyllabizer, &ENStemmer, &is_conjunction, &pmap,
            &copyrightPMap, &citationPMap, &Known_proper_nouns, &Known_personal_nouns,
            &Known_spellings, &Secondary_known_spellings, &Programming_known_spellings, &Stop_list);
        const wchar_t* text = L"It was good;\ntherefore, they were happy.";
        doc.load_document(text, wcslen(text), false, false, false, false);

        CHECK(doc.get_complete_sentence_count() == 1);
        CHECK(doc.get_valid_paragraph_count() == 1);
        CHECK(doc.get_valid_word_count() == 7);
        }
    SECTION("Abbreviation Header")
        {
        document<MYWORD> doc(L"", &ENsyllabizer, &ENStemmer, &is_conjunction, &pmap,
            &copyrightPMap, &citationPMap, &Known_proper_nouns, &Known_personal_nouns,
            &Known_spellings, &Secondary_known_spellings, &Programming_known_spellings, &Stop_list);
        const wchar_t* text = L"Tyson\n\nvs.\n\nDouglas";
        doc.load_document(text, wcslen(text), false, false, false, false);

        CHECK(doc.get_sentences()[0].get_type() == sentence_paragraph_type::header);
        CHECK(doc.get_sentences()[1].get_type() == sentence_paragraph_type::header);
        CHECK(doc.get_sentences()[2].get_type() == sentence_paragraph_type::header);

        CHECK(doc.get_sentences()[0].is_valid() == false);
        CHECK(doc.get_sentences()[1].is_valid() == false);
        CHECK(doc.get_sentences()[2].is_valid() == false);
        CHECK(doc.get_valid_word_count() == 0);
        CHECK(doc.get_valid_paragraph_count() == 0);
        CHECK(doc.get_complete_sentence_count() == 0);
        }
    SECTION("Abbreviation Header Valid")
        {
        document<MYWORD> doc(L"", &ENsyllabizer, &ENStemmer, &is_conjunction,
            &pmap, &copyrightPMap, &citationPMap, &Known_proper_nouns,
            &Known_personal_nouns, &Known_spellings, &Secondary_known_spellings,
            &Programming_known_spellings, &Stop_list);
        const wchar_t* text = L"Tyson\n\nvs.\n\nDouglas";
        doc.treat_header_words_as_valid(true);
        doc.load_document(text, wcslen(text), false, false, false, false);

        CHECK(doc.get_sentences()[0].get_type() == sentence_paragraph_type::header);
        CHECK(doc.get_sentences()[1].get_type() == sentence_paragraph_type::header);
        CHECK(doc.get_sentences()[2].get_type() == sentence_paragraph_type::header);

        CHECK(doc.get_sentences()[0].is_valid());
        CHECK(doc.get_sentences()[1].is_valid());
        CHECK(doc.get_sentences()[2].is_valid());
        CHECK(doc.get_valid_word_count() == 3);
        CHECK(doc.get_valid_paragraph_count() == 3);
        CHECK(doc.get_complete_sentence_count() == 3);
        }
    SECTION("Header")
        {
        document<MYWORD> doc(L"", &ENsyllabizer, &ENStemmer, &is_conjunction, &pmap,
            &copyrightPMap, &citationPMap, &Known_proper_nouns, &Known_personal_nouns,
            &Known_spellings, &Secondary_known_spellings, &Programming_known_spellings, &Stop_list);
        const wchar_t* text = L"Dark Night\n\nBy Blake Madden\n\nIt was a night. It was a dark night.\n\nThe End of this great book";
        doc.load_document(text, wcslen(text), false, false, false, false);

        CHECK(doc.get_sentences()[0].get_type() == sentence_paragraph_type::header);
        CHECK(doc.get_sentences()[1].get_type() == sentence_paragraph_type::header);
        CHECK(doc.get_sentences()[2].get_type() == sentence_paragraph_type::complete);
        CHECK(doc.get_sentences()[3].get_type() == sentence_paragraph_type::complete);
        CHECK(doc.get_sentences()[4].get_type() == sentence_paragraph_type::header);
        }
    SECTION("Header Is Valid")
        {
        document<MYWORD> doc(L"", &ENsyllabizer, &ENStemmer, &is_conjunction, &pmap,
            &copyrightPMap, &citationPMap, &Known_proper_nouns, &Known_personal_nouns,
            &Known_spellings, &Secondary_known_spellings, &Programming_known_spellings, &Stop_list);
        const wchar_t* text = L"Dark Night\n\nBy Blake Madden\n\nIt was a night. It was a dark night.\n\nThe End of this great book";
        doc.load_document(text, wcslen(text), false, false, false, false);

        CHECK(doc.get_sentences()[0].get_type() == sentence_paragraph_type::header);
        CHECK(doc.get_sentences()[1].get_type() == sentence_paragraph_type::header);
        CHECK(doc.get_sentences()[2].get_type() == sentence_paragraph_type::complete);
        CHECK(doc.get_sentences()[3].get_type() == sentence_paragraph_type::complete);
        CHECK(doc.get_sentences()[4].get_type() == sentence_paragraph_type::header);
        CHECK(doc.get_sentences()[0].is_valid() == false);
        CHECK(doc.get_sentences()[1].is_valid() == false);
        CHECK(doc.get_sentences()[2].is_valid() == true);
        CHECK(doc.get_sentences()[3].is_valid() == true);
        CHECK(doc.get_sentences()[4].is_valid() == false);

        document<MYWORD> doc2(L"", &ENsyllabizer, &ENStemmer, &is_conjunction, &pmap, &copyrightPMap, &citationPMap, &Known_proper_nouns, &Known_personal_nouns, &Known_spellings, &Secondary_known_spellings, &Programming_known_spellings, &Stop_list);
        doc2.treat_header_words_as_valid(true);
        doc2.load_document(text, wcslen(text), false, false, false, false);
        CHECK(doc2.get_sentences()[0].get_type() == sentence_paragraph_type::header);
        CHECK(doc2.get_sentences()[1].get_type() == sentence_paragraph_type::header);
        CHECK(doc2.get_sentences()[2].get_type() == sentence_paragraph_type::complete);
        CHECK(doc2.get_sentences()[3].get_type() == sentence_paragraph_type::complete);
        CHECK(doc2.get_sentences()[4].get_type() == sentence_paragraph_type::header);
        CHECK(doc2.get_sentences()[0].is_valid() == true);
        CHECK(doc2.get_sentences()[1].is_valid() == true);
        CHECK(doc2.get_sentences()[2].is_valid() == true);
        CHECK(doc2.get_sentences()[3].is_valid() == true);
        CHECK(doc2.get_sentences()[4].is_valid() == true);
        }
    SECTION("Header2")
        {
        document<MYWORD> doc(L"", &ENsyllabizer, &ENStemmer, &is_conjunction, &pmap,
            &copyrightPMap, &citationPMap, &Known_proper_nouns, &Known_personal_nouns,
            &Known_spellings, &Secondary_known_spellings, &Programming_known_spellings, &Stop_list);
        const wchar_t* text = L"Dark Night\n\nIt was a night. It was a dark night.\n\nThe End of this great book";
        doc.load_document(text, wcslen(text), false, false, false, false);

        CHECK(doc.get_sentences()[0].get_type() == sentence_paragraph_type::header);
        CHECK(doc.get_sentences()[1].get_type() == sentence_paragraph_type::complete);
        CHECK(doc.get_sentences()[2].get_type() == sentence_paragraph_type::complete);
        CHECK(doc.get_sentences()[3].get_type() == sentence_paragraph_type::header);
        }
    SECTION("List")
        {
        document<MYWORD> doc(L"", &ENsyllabizer, &ENStemmer, &is_conjunction, &pmap,
            &copyrightPMap, &citationPMap, &Known_proper_nouns, &Known_personal_nouns,
            &Known_spellings, &Secondary_known_spellings, &Programming_known_spellings, &Stop_list);
        const wchar_t* text = L"Item 1\n\nItem 2\n\nItem 3\n\nIt was a night.\n\nThe End of this great book";
        doc.load_document(text, wcslen(text), false, false, false, false);

        CHECK(doc.get_sentences()[0].get_type() == sentence_paragraph_type::list_item);
        CHECK(doc.get_sentences()[1].get_type() == sentence_paragraph_type::list_item);
        CHECK(doc.get_sentences()[2].get_type() == sentence_paragraph_type::list_item);
        CHECK(doc.get_sentences()[3].get_type() == sentence_paragraph_type::complete);
        CHECK(doc.get_sentences()[4].get_type() == sentence_paragraph_type::header);
        }
    SECTION("List Header Is Valid")
        {
        document<MYWORD> doc(L"", &ENsyllabizer, &ENStemmer, &is_conjunction, &pmap,
            &copyrightPMap, &citationPMap, &Known_proper_nouns, &Known_personal_nouns,
            &Known_spellings, &Secondary_known_spellings, &Programming_known_spellings, &Stop_list);
        const wchar_t* text = L"Item 1\n\nItem 2\n\nItem 3\n\nIt was a night.\n\nThe End of this great book";
        doc.treat_header_words_as_valid(true);
        doc.load_document(text, wcslen(text), false, false, false, false);

        CHECK(doc.get_sentences()[0].get_type() == sentence_paragraph_type::list_item);
        CHECK(doc.get_sentences()[1].get_type() == sentence_paragraph_type::list_item);
        CHECK(doc.get_sentences()[2].get_type() == sentence_paragraph_type::list_item);
        CHECK(doc.get_sentences()[3].get_type() == sentence_paragraph_type::complete);
        CHECK(doc.get_sentences()[4].get_type() == sentence_paragraph_type::header);
        }
    SECTION("All Complete")
        {
        document<MYWORD> doc(L"", &ENsyllabizer, &ENStemmer, &is_conjunction, &pmap,
            &copyrightPMap, &citationPMap, &Known_proper_nouns, &Known_personal_nouns,
            &Known_spellings, &Secondary_known_spellings, &Programming_known_spellings, &Stop_list);
        const wchar_t* text = L"Dark Night.\n\nBy Blake Madden.\n\nIt was a night.\n\nThe End of this great book.";
        doc.load_document(text, wcslen(text), false, false, false, false);

        CHECK(doc.get_sentences()[0].get_type() == sentence_paragraph_type::complete);
        CHECK(doc.get_sentences()[1].get_type() == sentence_paragraph_type::complete);
        CHECK(doc.get_sentences()[2].get_type() == sentence_paragraph_type::complete);
        CHECK(doc.get_sentences()[3].get_type() == sentence_paragraph_type::complete);
        }
    SECTION("All List")
        {
        document<MYWORD> doc(L"", &ENsyllabizer, &ENStemmer, &is_conjunction,
            &pmap, &copyrightPMap, &citationPMap, &Known_proper_nouns, &Known_personal_nouns,
            &Known_spellings, &Secondary_known_spellings, &Programming_known_spellings, &Stop_list);
        const wchar_t* text = L"Dark Night\n\nBy Blake Madden\n\nThe End of this great book";
        doc.load_document(text, wcslen(text), false, false, false, false);

        CHECK(doc.get_sentences()[0].get_type() == sentence_paragraph_type::list_item);
        CHECK(doc.get_sentences()[1].get_type() == sentence_paragraph_type::list_item);
        CHECK(doc.get_sentences()[2].get_type() == sentence_paragraph_type::list_item);
        }
    SECTION("All Header")
        {
        document<MYWORD> doc(L"", &ENsyllabizer, &ENStemmer, &is_conjunction, &pmap,
            &copyrightPMap, &citationPMap, &Known_proper_nouns, &Known_personal_nouns,
            &Known_spellings, &Secondary_known_spellings, &Programming_known_spellings, &Stop_list);
        const wchar_t* text = L"Dark Night\n\nBy Blake Madden";
        doc.load_document(text, wcslen(text), false, false, false, false);

        CHECK(doc.get_sentences()[0].get_type() == sentence_paragraph_type::header);
        CHECK(doc.get_sentences()[1].get_type() == sentence_paragraph_type::header);
        }
    SECTION("All")
        {
        document<MYWORD> doc(L"", &ENsyllabizer, &ENStemmer, &is_conjunction, &pmap,
            &copyrightPMap, &citationPMap, &Known_proper_nouns, &Known_personal_nouns,
            &Known_spellings, &Secondary_known_spellings, &Programming_known_spellings, &Stop_list);
        const wchar_t* text = L"Dark Night\n\nBy Blake Madden\n\nIt was a night. It was a dark night.\n\nItem 1\n\nItem 2\n\nItem 3\n\nSome sentence.\n\nThe End of this great book";
        doc.load_document(text, wcslen(text), false, false, false, false);

        CHECK(doc.get_sentences()[0].get_type() == sentence_paragraph_type::header);
        CHECK(doc.get_sentences()[1].get_type() == sentence_paragraph_type::header);
        CHECK(doc.get_sentences()[2].get_type() == sentence_paragraph_type::complete);
        CHECK(doc.get_sentences()[3].get_type() == sentence_paragraph_type::complete);
        CHECK(doc.get_sentences()[4].get_type() == sentence_paragraph_type::list_item);
        CHECK(doc.get_sentences()[5].get_type() == sentence_paragraph_type::list_item);
        CHECK(doc.get_sentences()[6].get_type() == sentence_paragraph_type::list_item);
        CHECK(doc.get_sentences()[7].get_type() == sentence_paragraph_type::complete);
        CHECK(doc.get_sentences()[8].get_type() == sentence_paragraph_type::header);
        }
    SECTION("All Header Is Valid")
        {
        document<MYWORD> doc(L"", &ENsyllabizer, &ENStemmer, &is_conjunction, &pmap,
            &copyrightPMap, &citationPMap, &Known_proper_nouns, &Known_personal_nouns,
            &Known_spellings, &Secondary_known_spellings, &Programming_known_spellings, &Stop_list);
        const wchar_t* text = L"Dark Night\n\nBy Blake Madden\n\nIt was a night. It was a dark night.\n\nItem 1\n\nItem 2\n\nItem 3\n\nSome sentence.\n\nThe End of this great book";
        doc.load_document(text, wcslen(text), false, false, false, false);

        CHECK(doc.get_sentences()[0].get_type() == sentence_paragraph_type::header);
        CHECK(doc.get_sentences()[1].get_type() == sentence_paragraph_type::header);
        CHECK(doc.get_sentences()[2].get_type() == sentence_paragraph_type::complete);
        CHECK(doc.get_sentences()[3].get_type() == sentence_paragraph_type::complete);
        CHECK(doc.get_sentences()[4].get_type() == sentence_paragraph_type::list_item);
        CHECK(doc.get_sentences()[5].get_type() == sentence_paragraph_type::list_item);
        CHECK(doc.get_sentences()[6].get_type() == sentence_paragraph_type::list_item);
        CHECK(doc.get_sentences()[7].get_type() == sentence_paragraph_type::complete);
        CHECK(doc.get_sentences()[8].get_type() == sentence_paragraph_type::header);

        CHECK(doc.get_sentences()[0].is_valid() == false);
        CHECK(doc.get_sentences()[1].is_valid() == false);
        CHECK(doc.get_sentences()[2].is_valid() == true);
        CHECK(doc.get_sentences()[3].is_valid() == true);
        CHECK(doc.get_sentences()[4].is_valid() == false);
        CHECK(doc.get_sentences()[5].is_valid() == false);
        CHECK(doc.get_sentences()[6].is_valid() == false);
        CHECK(doc.get_sentences()[7].is_valid() == true);
        CHECK(doc.get_sentences()[8].is_valid() == false);

        CHECK(doc.get_words()[0].is_valid() == false);
        CHECK(doc.get_words()[1].is_valid() == false);
        CHECK(doc.get_words()[2].is_valid() == false);
        CHECK(doc.get_words()[3].is_valid() == false);
        CHECK(doc.get_words()[4].is_valid() == false);
        CHECK(doc.get_words()[5].is_valid() == true);
        CHECK(doc.get_words()[6].is_valid() == true);
        CHECK(doc.get_words()[7].is_valid() == true);
        CHECK(doc.get_words()[8].is_valid() == true);
        CHECK(doc.get_words()[9].is_valid() == true);
        CHECK(doc.get_words()[10].is_valid() == true);
        CHECK(doc.get_words()[11].is_valid() == true);
        CHECK(doc.get_words()[12].is_valid() == true);
        CHECK(doc.get_words()[13].is_valid() == true);
        CHECK(doc.get_words()[14].is_valid() == false);
        CHECK(doc.get_words()[15].is_valid() == false);
        CHECK(doc.get_words()[16].is_valid() == false);
        CHECK(doc.get_words()[17].is_valid() == false);
        CHECK(doc.get_words()[18].is_valid() == false);
        CHECK(doc.get_words()[19].is_valid() == false);
        CHECK(doc.get_words()[20].is_valid() == true);
        CHECK(doc.get_words()[21].is_valid() == true);
        CHECK(doc.get_words()[22].is_valid() == false);
        CHECK(doc.get_words()[23].is_valid() == false);
        CHECK(doc.get_words()[24].is_valid() == false);
        CHECK(doc.get_words()[25].is_valid() == false);
        CHECK(doc.get_words()[26].is_valid() == false);
        CHECK(doc.get_words()[27].is_valid() == false);

        document<MYWORD> doc2(L"", &ENsyllabizer, &ENStemmer, &is_conjunction, &pmap, &copyrightPMap, &citationPMap, &Known_proper_nouns, &Known_personal_nouns, &Known_spellings, &Secondary_known_spellings, &Programming_known_spellings, &Stop_list);
        doc2.treat_header_words_as_valid(true);
        doc2.load_document(text, wcslen(text), false, false, false, false);

        CHECK(doc2.get_sentences()[0].get_type() == sentence_paragraph_type::header);
        CHECK(doc2.get_sentences()[1].get_type() == sentence_paragraph_type::header);
        CHECK(doc2.get_sentences()[2].get_type() == sentence_paragraph_type::complete);
        CHECK(doc2.get_sentences()[3].get_type() == sentence_paragraph_type::complete);
        CHECK(doc2.get_sentences()[4].get_type() == sentence_paragraph_type::list_item);
        CHECK(doc2.get_sentences()[5].get_type() == sentence_paragraph_type::list_item);
        CHECK(doc2.get_sentences()[6].get_type() == sentence_paragraph_type::list_item);
        CHECK(doc2.get_sentences()[7].get_type() == sentence_paragraph_type::complete);
        CHECK(doc2.get_sentences()[8].get_type() == sentence_paragraph_type::header);

        CHECK(doc2.get_sentences()[0].is_valid() == true);
        CHECK(doc2.get_sentences()[1].is_valid() == true);
        CHECK(doc2.get_sentences()[2].is_valid() == true);
        CHECK(doc2.get_sentences()[3].is_valid() == true);
        CHECK(doc2.get_sentences()[4].is_valid() == false);
        CHECK(doc2.get_sentences()[5].is_valid() == false);
        CHECK(doc2.get_sentences()[6].is_valid() == false);
        CHECK(doc2.get_sentences()[7].is_valid() == true);
        CHECK(doc2.get_sentences()[8].is_valid() == true);

        CHECK(doc2.get_words()[0].is_valid() == true);
        CHECK(doc2.get_words()[1].is_valid() == true);
        CHECK(doc2.get_words()[2].is_valid() == true);
        CHECK(doc2.get_words()[3].is_valid() == true);
        CHECK(doc2.get_words()[4].is_valid() == true);
        CHECK(doc2.get_words()[5].is_valid() == true);
        CHECK(doc2.get_words()[6].is_valid() == true);
        CHECK(doc2.get_words()[7].is_valid() == true);
        CHECK(doc2.get_words()[8].is_valid() == true);
        CHECK(doc2.get_words()[9].is_valid() == true);
        CHECK(doc2.get_words()[10].is_valid() == true);
        CHECK(doc2.get_words()[11].is_valid() == true);
        CHECK(doc2.get_words()[12].is_valid() == true);
        CHECK(doc2.get_words()[13].is_valid() == true);
        CHECK(doc2.get_words()[14].is_valid() == false);
        CHECK(doc2.get_words()[15].is_valid() == false);
        CHECK(doc2.get_words()[16].is_valid() == false);
        CHECK(doc2.get_words()[17].is_valid() == false);
        CHECK(doc2.get_words()[18].is_valid() == false);
        CHECK(doc2.get_words()[19].is_valid() == false);
        CHECK(doc2.get_words()[20].is_valid() == true);
        CHECK(doc2.get_words()[21].is_valid() == true);
        CHECK(doc2.get_words()[22].is_valid() == true);
        CHECK(doc2.get_words()[23].is_valid() == true);
        CHECK(doc2.get_words()[24].is_valid() == true);
        CHECK(doc2.get_words()[25].is_valid() == true);
        CHECK(doc2.get_words()[26].is_valid() == true);
        CHECK(doc2.get_words()[27].is_valid() == true);
        }
    SECTION("All Paragraphs Header Is Valid")
        {
        document<MYWORD> doc(L"", &ENsyllabizer, &ENStemmer, &is_conjunction, &pmap,
            &copyrightPMap, &citationPMap, &Known_proper_nouns, &Known_personal_nouns,
            &Known_spellings, &Secondary_known_spellings, &Programming_known_spellings, &Stop_list);
        const wchar_t* text = L"Dark Night\n\nBy Blake Madden\n\nIt was a night. It was a dark night.\n\nItem 1\n\nItem 2\n\nItem 3\n\nSome sentence.\n\nThe End of this great book";
        doc.load_document(text, wcslen(text), false, false, false, false);

        CHECK(doc.get_sentences()[0].get_type() == sentence_paragraph_type::header);

        CHECK(doc.get_paragraphs()[0].get_type() == sentence_paragraph_type::header);
        CHECK(doc.get_paragraphs()[1].get_type() == sentence_paragraph_type::header);
        CHECK(doc.get_paragraphs()[2].get_type() == sentence_paragraph_type::complete);
        CHECK(doc.get_paragraphs()[3].get_type() == sentence_paragraph_type::list_item);
        CHECK(doc.get_paragraphs()[4].get_type() == sentence_paragraph_type::list_item);
        CHECK(doc.get_paragraphs()[5].get_type() == sentence_paragraph_type::list_item);
        CHECK(doc.get_paragraphs()[6].get_type() == sentence_paragraph_type::complete);
        CHECK(doc.get_paragraphs()[7].get_type() == sentence_paragraph_type::header);

        CHECK(doc.get_paragraphs()[0].is_valid() == false);
        CHECK(doc.get_paragraphs()[1].is_valid() == false);
        CHECK(doc.get_paragraphs()[2].is_valid() == true);
        CHECK(doc.get_paragraphs()[3].is_valid() == false);
        CHECK(doc.get_paragraphs()[4].is_valid() == false);
        CHECK(doc.get_paragraphs()[5].is_valid() == false);
        CHECK(doc.get_paragraphs()[6].is_valid() == true);
        CHECK(doc.get_paragraphs()[7].is_valid() == false);
        CHECK(doc.get_valid_paragraph_count() == 2);

        document<MYWORD> doc2(L"", &ENsyllabizer, &ENStemmer, &is_conjunction, &pmap,
            &copyrightPMap, &citationPMap, &Known_proper_nouns, &Known_personal_nouns,
            &Known_spellings, &Secondary_known_spellings, &Programming_known_spellings, &Stop_list);
        doc2.treat_header_words_as_valid(true);
        doc2.load_document(text, wcslen(text), false, false, false, false);

        CHECK(doc2.get_paragraphs()[0].get_type() == sentence_paragraph_type::header);
        CHECK(doc2.get_paragraphs()[1].get_type() == sentence_paragraph_type::header);
        CHECK(doc2.get_paragraphs()[2].get_type() == sentence_paragraph_type::complete);
        CHECK(doc2.get_paragraphs()[3].get_type() == sentence_paragraph_type::list_item);
        CHECK(doc2.get_paragraphs()[4].get_type() == sentence_paragraph_type::list_item);
        CHECK(doc2.get_paragraphs()[5].get_type() == sentence_paragraph_type::list_item);
        CHECK(doc2.get_paragraphs()[6].get_type() == sentence_paragraph_type::complete);
        CHECK(doc2.get_paragraphs()[7].get_type() == sentence_paragraph_type::header);

        CHECK(doc2.get_paragraphs()[0].is_valid() == true);
        CHECK(doc2.get_paragraphs()[1].is_valid() == true);
        CHECK(doc2.get_paragraphs()[2].is_valid() == true);
        CHECK(doc2.get_paragraphs()[3].is_valid() == false);
        CHECK(doc2.get_paragraphs()[4].is_valid() == false);
        CHECK(doc2.get_paragraphs()[5].is_valid() == false);
        CHECK(doc2.get_paragraphs()[6].is_valid() == true);
        CHECK(doc2.get_paragraphs()[7].is_valid() == true);
        CHECK(doc2.get_valid_paragraph_count() == 5);
        }
    SECTION("Sparse Sentences")
        {
        document<MYWORD> doc(L"", &ENsyllabizer, &ENStemmer, &is_conjunction, &pmap,
            &copyrightPMap, &citationPMap, &Known_proper_nouns, &Known_personal_nouns,
            &Known_spellings, &Secondary_known_spellings, &Programming_known_spellings, &Stop_list);
        const wchar_t* text = L"Dark Night\n\nBy Blake Madden\n\n2010\n\nChapter 1.\n\nItem 1\n\nItem 2\n\nItem 3\n\nEpilogue.\n\nThe End of this great book";
        doc.load_document(text, wcslen(text), false, false, false, false);

        CHECK(doc.get_sentences()[0].get_type() == sentence_paragraph_type::list_item);
        CHECK(doc.get_sentences()[1].get_type() == sentence_paragraph_type::list_item);
        CHECK(doc.get_sentences()[2].get_type() == sentence_paragraph_type::list_item);
        CHECK(doc.get_sentences()[3].get_type() == sentence_paragraph_type::list_item);
        CHECK(doc.get_sentences()[4].get_type() == sentence_paragraph_type::list_item);
        CHECK(doc.get_sentences()[5].get_type() == sentence_paragraph_type::list_item);
        CHECK(doc.get_sentences()[6].get_type() == sentence_paragraph_type::list_item);
        CHECK(doc.get_sentences()[7].get_type() == sentence_paragraph_type::complete);
        CHECK(doc.get_sentences()[8].get_type() == sentence_paragraph_type::header);

        CHECK(doc.get_paragraphs()[0].get_type() == sentence_paragraph_type::list_item);
        CHECK(doc.get_paragraphs()[1].get_type() == sentence_paragraph_type::list_item);
        CHECK(doc.get_paragraphs()[2].get_type() == sentence_paragraph_type::list_item);
        CHECK(doc.get_paragraphs()[3].get_type() == sentence_paragraph_type::list_item);
        CHECK(doc.get_paragraphs()[4].get_type() == sentence_paragraph_type::list_item);
        CHECK(doc.get_paragraphs()[5].get_type() == sentence_paragraph_type::list_item);
        CHECK(doc.get_paragraphs()[6].get_type() == sentence_paragraph_type::list_item);
        CHECK(doc.get_paragraphs()[7].get_type() == sentence_paragraph_type::complete);
        CHECK(doc.get_paragraphs()[8].get_type() == sentence_paragraph_type::header);
        CHECK(doc.get_complete_sentence_count() == 1);

        CHECK(doc.get_paragraphs()[0].is_valid() == false);
        CHECK(doc.get_paragraphs()[1].is_valid() == false);
        CHECK(doc.get_paragraphs()[2].is_valid() == false);
        CHECK(doc.get_paragraphs()[3].is_valid() == false);
        CHECK(doc.get_paragraphs()[4].is_valid() == false);
        CHECK(doc.get_paragraphs()[5].is_valid() == false);
        CHECK(doc.get_paragraphs()[6].is_valid() == false);
        CHECK(doc.get_paragraphs()[7].is_valid());
        CHECK(doc.get_paragraphs()[8].is_valid() == false);
        CHECK(doc.get_valid_paragraph_count() == 1);

        // now run with headers being valid
        document<MYWORD> doc2(L"", &ENsyllabizer, &ENStemmer, &is_conjunction, &pmap,
            &copyrightPMap, &citationPMap, &Known_proper_nouns, &Known_personal_nouns,
            &Known_spellings, &Secondary_known_spellings, &Programming_known_spellings, &Stop_list);
        doc2.treat_header_words_as_valid(true);
        doc2.load_document(text, wcslen(text), false, false, false, false);

        CHECK(doc2.get_sentences()[0].get_type() == sentence_paragraph_type::list_item);
        CHECK(doc2.get_sentences()[1].get_type() == sentence_paragraph_type::list_item);
        CHECK(doc2.get_sentences()[2].get_type() == sentence_paragraph_type::list_item);
        CHECK(doc2.get_sentences()[3].get_type() == sentence_paragraph_type::list_item);
        CHECK(doc2.get_sentences()[4].get_type() == sentence_paragraph_type::list_item);
        CHECK(doc2.get_sentences()[5].get_type() == sentence_paragraph_type::list_item);
        CHECK(doc2.get_sentences()[6].get_type() == sentence_paragraph_type::list_item);
        CHECK(doc2.get_sentences()[7].get_type() == sentence_paragraph_type::complete);
        CHECK(doc2.get_sentences()[8].get_type() == sentence_paragraph_type::header);

        CHECK(doc2.get_paragraphs()[0].get_type() == sentence_paragraph_type::list_item);
        CHECK(doc2.get_paragraphs()[1].get_type() == sentence_paragraph_type::list_item);
        CHECK(doc2.get_paragraphs()[2].get_type() == sentence_paragraph_type::list_item);
        CHECK(doc2.get_paragraphs()[3].get_type() == sentence_paragraph_type::list_item);
        CHECK(doc2.get_paragraphs()[4].get_type() == sentence_paragraph_type::list_item);
        CHECK(doc2.get_paragraphs()[5].get_type() == sentence_paragraph_type::list_item);
        CHECK(doc2.get_paragraphs()[6].get_type() == sentence_paragraph_type::list_item);
        CHECK(doc2.get_paragraphs()[7].get_type() == sentence_paragraph_type::complete);
        CHECK(doc2.get_paragraphs()[8].get_type() == sentence_paragraph_type::header);
        CHECK(doc2.get_complete_sentence_count() == 2);

        CHECK(doc2.get_sentences()[0].is_valid() == false);
        CHECK(doc2.get_sentences()[1].is_valid() == false);
        CHECK(doc2.get_sentences()[2].is_valid() == false);
        CHECK(doc2.get_sentences()[3].is_valid() == false);
        CHECK(doc2.get_sentences()[4].is_valid() == false);
        CHECK(doc2.get_sentences()[5].is_valid() == false);
        CHECK(doc2.get_sentences()[6].is_valid() == false);
        CHECK(doc2.get_sentences()[7].is_valid());

        CHECK(doc2.get_paragraphs()[0].is_valid() == false);
        CHECK(doc2.get_paragraphs()[1].is_valid() == false);
        CHECK(doc2.get_paragraphs()[2].is_valid() == false);
        CHECK(doc2.get_paragraphs()[3].is_valid() == false);
        CHECK(doc2.get_paragraphs()[4].is_valid() == false);
        CHECK(doc2.get_paragraphs()[5].is_valid() == false);
        CHECK(doc2.get_paragraphs()[6].is_valid() == false);
        CHECK(doc2.get_paragraphs()[7].is_valid());
        // this is what will be different
        CHECK(doc2.get_paragraphs()[8].is_valid());
        CHECK(doc2.get_valid_paragraph_count() == 2);
        }
    SECTION("Sparse Sentences Aggressive")
        {
        document<MYWORD> doc(L"", &ENsyllabizer, &ENStemmer, &is_conjunction,
            &pmap, &copyrightPMap, &citationPMap, &Known_proper_nouns,
            &Known_personal_nouns, &Known_spellings, &Secondary_known_spellings, &Programming_known_spellings, &Stop_list);
        const wchar_t* text = L"Item one\n\nThis is a list item that looks like sentence.\n\nItem two";
        doc.load_document(text, wcslen(text), false, false, false, false);

        CHECK(doc.get_sentences()[0].get_type() == sentence_paragraph_type::header);
        CHECK(doc.get_sentences()[1].get_type() == sentence_paragraph_type::complete);
        CHECK(doc.get_sentences()[2].get_type() == sentence_paragraph_type::header);

        doc.set_aggressive_exclusion(true);
        doc.load_document(text, wcslen(text), false, false, false, false);

        CHECK(doc.get_sentences()[0].get_type() == sentence_paragraph_type::list_item);
        CHECK(doc.get_sentences()[1].get_type() == sentence_paragraph_type::list_item);
        CHECK(doc.get_sentences()[2].get_type() == sentence_paragraph_type::list_item);
        }
    SECTION("Sparse Sentences Aggressive With Regular Sentences")
        {
        document<MYWORD> doc(L"", &ENsyllabizer, &ENStemmer, &is_conjunction, &pmap,
            &copyrightPMap, &citationPMap, &Known_proper_nouns, &Known_personal_nouns,
            &Known_spellings, &Secondary_known_spellings, &Programming_known_spellings, &Stop_list);
        const wchar_t* text = L"Some text.\n\nItem one\n\nThis is a list item that looks like sentence.\n\nItem two\n\nEnding text.";
        doc.load_document(text, wcslen(text), false, false, false, false);

        CHECK(doc.get_sentences()[0].get_type() == sentence_paragraph_type::complete);
        CHECK(doc.get_sentences()[1].get_type() == sentence_paragraph_type::header);
        CHECK(doc.get_sentences()[2].get_type() == sentence_paragraph_type::complete);
        CHECK(doc.get_sentences()[3].get_type() == sentence_paragraph_type::header);
        CHECK(doc.get_sentences()[4].get_type() == sentence_paragraph_type::complete);

        doc.set_aggressive_exclusion(true);
        doc.load_document(text, wcslen(text), false, false, false, false);

        CHECK(doc.get_sentences()[0].get_type() == sentence_paragraph_type::complete);
        CHECK(doc.get_sentences()[1].get_type() == sentence_paragraph_type::list_item);
        CHECK(doc.get_sentences()[2].get_type() == sentence_paragraph_type::list_item);
        CHECK(doc.get_sentences()[3].get_type() == sentence_paragraph_type::list_item);
        CHECK(doc.get_sentences()[4].get_type() == sentence_paragraph_type::complete);
        }
    SECTION("Sparse Sentences Aggressive Double Headers")
        {
        document<MYWORD> doc(L"", &ENsyllabizer, &ENStemmer, &is_conjunction, &pmap,
            &copyrightPMap, &citationPMap, &Known_proper_nouns, &Known_personal_nouns,
            &Known_spellings, &Secondary_known_spellings, &Programming_known_spellings, &Stop_list);
        const wchar_t* text = L"Item one\n\nItem two\n\nThis is a list item that looks like senence.\n\nItem three\n\nItem four";
        doc.load_document(text, wcslen(text), false, false, false, false);

        CHECK(doc.get_sentences()[0].get_type() == sentence_paragraph_type::header);
        CHECK(doc.get_sentences()[1].get_type() == sentence_paragraph_type::header);
        CHECK(doc.get_sentences()[2].get_type() == sentence_paragraph_type::complete);
        CHECK(doc.get_sentences()[3].get_type() == sentence_paragraph_type::header);
        CHECK(doc.get_sentences()[4].get_type() == sentence_paragraph_type::header);

        doc.set_aggressive_exclusion(true);
        doc.load_document(text, wcslen(text), false, false, false, false);

        CHECK(doc.get_sentences()[0].get_type() == sentence_paragraph_type::list_item);
        CHECK(doc.get_sentences()[1].get_type() == sentence_paragraph_type::list_item);
        CHECK(doc.get_sentences()[2].get_type() == sentence_paragraph_type::list_item);
        CHECK(doc.get_sentences()[3].get_type() == sentence_paragraph_type::list_item);
        CHECK(doc.get_sentences()[4].get_type() == sentence_paragraph_type::list_item);
        }
    SECTION("Sparse Sentences With Lists")
        {
        document<MYWORD> doc(L"", &ENsyllabizer, &ENStemmer, &is_conjunction, &pmap,
            &copyrightPMap, &citationPMap, &Known_proper_nouns, &Known_personal_nouns,
            &Known_spellings, &Secondary_known_spellings, &Programming_known_spellings, &Stop_list);
        const wchar_t* text = L"Ingredients:\n\nItem 1\n\nItem 2\n\nItem 3\n\nMore Ingredients:\n\nItem 1\n\nItem 2\n\nItem 3";
        doc.load_document(text, wcslen(text), false, false, false, false);

        CHECK(doc.get_sentences()[0].get_type() == sentence_paragraph_type::complete);
        CHECK(doc.get_sentences()[1].get_type() == sentence_paragraph_type::list_item);
        CHECK(doc.get_sentences()[2].get_type() == sentence_paragraph_type::list_item);
        CHECK(doc.get_sentences()[3].get_type() == sentence_paragraph_type::list_item);
        // "More Ingredients:" is surrounded by 3 list items, so
        // it will be a list item too, even with aggressive list deduction turned off.
        CHECK(doc.get_sentences()[4].get_type() == sentence_paragraph_type::list_item);
        CHECK(doc.get_sentences()[5].get_type() == sentence_paragraph_type::list_item);
        CHECK(doc.get_sentences()[6].get_type() == sentence_paragraph_type::list_item);
        CHECK(doc.get_sentences()[7].get_type() == sentence_paragraph_type::list_item);
        CHECK(doc.get_complete_sentence_count() == 1);

        CHECK(doc.get_paragraphs()[0].get_type() == sentence_paragraph_type::complete);
        CHECK(doc.get_paragraphs()[1].get_type() == sentence_paragraph_type::list_item);
        CHECK(doc.get_paragraphs()[2].get_type() == sentence_paragraph_type::list_item);
        CHECK(doc.get_paragraphs()[3].get_type() == sentence_paragraph_type::list_item);
        CHECK(doc.get_paragraphs()[4].get_type() == sentence_paragraph_type::list_item);
        CHECK(doc.get_paragraphs()[5].get_type() == sentence_paragraph_type::list_item);
        CHECK(doc.get_paragraphs()[6].get_type() == sentence_paragraph_type::list_item);
        CHECK(doc.get_paragraphs()[7].get_type() == sentence_paragraph_type::list_item);
        CHECK(doc.get_valid_paragraph_count() == 1);

        CHECK(doc.get_sentences()[0].is_valid());
        CHECK(doc.get_sentences()[1].is_valid() == false);
        CHECK(doc.get_sentences()[2].is_valid() == false);
        CHECK(doc.get_sentences()[3].is_valid() == false);
        CHECK(doc.get_sentences()[4].is_valid()== false);
        CHECK(doc.get_sentences()[5].is_valid() == false);
        CHECK(doc.get_sentences()[6].is_valid() == false);
        CHECK(doc.get_sentences()[7].is_valid() == false);

        CHECK(doc.get_paragraphs()[0].is_valid());
        CHECK(doc.get_paragraphs()[1].is_valid() == false);
        CHECK(doc.get_paragraphs()[2].is_valid() == false);
        CHECK(doc.get_paragraphs()[3].is_valid() == false);
        CHECK(doc.get_paragraphs()[4].is_valid()== false);
        CHECK(doc.get_paragraphs()[5].is_valid() == false);
        CHECK(doc.get_paragraphs()[6].is_valid() == false);
        CHECK(doc.get_paragraphs()[7].is_valid() == false);
        }
    SECTION("Sparse Sentences With Long Sentence")
        {
        document<MYWORD> doc(L"", &ENsyllabizer, &ENStemmer, &is_conjunction, &pmap,
            &copyrightPMap, &citationPMap, &Known_proper_nouns, &Known_personal_nouns,
            &Known_spellings, &Secondary_known_spellings, &Programming_known_spellings, &Stop_list);
        const wchar_t* text = L"Ingredients:\n\nItem 1\n\nItem 2\n\nItem 3\n\nHere are some other ingredientes?\n\nItem 1\n\nItem 2\n\nItem 3";
        doc.load_document(text, wcslen(text), false, false, false, false);

        CHECK(doc.get_sentences()[0].get_type() == sentence_paragraph_type::complete);
        CHECK(doc.get_sentences()[1].get_type() == sentence_paragraph_type::list_item);
        CHECK(doc.get_sentences()[2].get_type() == sentence_paragraph_type::list_item);
        CHECK(doc.get_sentences()[3].get_type() == sentence_paragraph_type::list_item);
        CHECK(doc.get_sentences()[4].get_type() == sentence_paragraph_type::complete); // it's incomplete, but 5 words
        CHECK(doc.get_sentences()[5].get_type() == sentence_paragraph_type::list_item);
        CHECK(doc.get_sentences()[6].get_type() == sentence_paragraph_type::list_item);
        CHECK(doc.get_sentences()[7].get_type() == sentence_paragraph_type::list_item);
        CHECK(doc.get_complete_sentence_count() == 2);

        CHECK(doc.get_paragraphs()[0].get_type() == sentence_paragraph_type::complete);
        CHECK(doc.get_paragraphs()[1].get_type() == sentence_paragraph_type::list_item);
        CHECK(doc.get_paragraphs()[2].get_type() == sentence_paragraph_type::list_item);
        CHECK(doc.get_paragraphs()[3].get_type() == sentence_paragraph_type::list_item);
        CHECK(doc.get_paragraphs()[4].get_type() == sentence_paragraph_type::complete);
        CHECK(doc.get_paragraphs()[5].get_type() == sentence_paragraph_type::list_item);
        CHECK(doc.get_paragraphs()[6].get_type() == sentence_paragraph_type::list_item);
        CHECK(doc.get_paragraphs()[7].get_type() == sentence_paragraph_type::list_item);
        CHECK(doc.get_valid_paragraph_count() == 2);
        }
    }
// NOLINTEND
// clang-format on
