#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>
#include "../src/indexing/abbreviation.h"
#include "../src/indexing/passive_voice.h"
#include "../src/indexing/tokenize.h"

TEST_CASE("Bullets", "[bullets]")
    {
    SECTION("Null")
        {
        grammar::is_bulleted_text is_indented;
        CHECK(is_indented(nullptr).first == false);
        CHECK(is_indented(nullptr).second == 0);
        }
    SECTION("Tab")
        {
        const wchar_t text[] = L"\titem 1";
        grammar::is_bulleted_text is_indented;
        CHECK(is_indented(text).first == false);
        CHECK(is_indented(text).second == 0);
        }
    SECTION("One Space")
        {
        const wchar_t text[] = L" item 1";
        grammar::is_bulleted_text is_indented;
        CHECK(is_indented(text).first == false);
        CHECK(is_indented(text).second == 0);
        }
    SECTION("Two Space")
        {
        const wchar_t text[] = L"  item 1";
        grammar::is_bulleted_text is_indented;
        CHECK(is_indented(text).first == false);
        CHECK(is_indented(text).second == 0);
        }
    SECTION("Spaces")
        {
        const wchar_t text[] = L"   item 1";
        grammar::is_bulleted_text is_indented;
        CHECK(is_indented(text).first == false);
        CHECK(is_indented(text).second == 0);
        }
    SECTION("All Spaces")
        {
        const wchar_t text[] = L"     ";
        grammar::is_bulleted_text is_indented;
        CHECK(is_indented(text).first == false);
        CHECK(is_indented(text).second == 0);
        }
    SECTION("Bullet")
        {
        const wchar_t* text = L"· item 1";
        grammar::is_bulleted_text is_indented;
        CHECK(is_indented(text).first == true);
        CHECK(is_indented(text).second == 1);
        text = L"• item 1";
        CHECK(is_indented(text).first == true);
        CHECK(is_indented(text).second == 1);
        }
    SECTION("Bullet With Indenting")
        {
        const wchar_t* text = L"   · item 1";
        grammar::is_bulleted_text is_indented;
        CHECK(is_indented(text).first == true);
        CHECK(is_indented(text).second == 1);
        text = L"\t• item 1";
        CHECK(is_indented(text).first == true);
        CHECK(is_indented(text).second == 1);
        }
    SECTION("Number Bullet Followed By Colon")
        {
        const wchar_t text[] = L"12: item 1";
        grammar::is_bulleted_text is_indented;
        CHECK(is_indented(text).first == true);
        CHECK(is_indented(text).second == 3);
        CHECK(is_indented(L"12:").first == false);//boundary check
        }
    SECTION("Number Bullet Time")
        {
        const wchar_t text[] = L"12:55 item 1";
        grammar::is_bulleted_text is_indented;
        CHECK(is_indented(text).first == false);
        CHECK(is_indented(text).second == 0);
        }
    SECTION("Number Bullet")
        {
        const wchar_t text[] = L"12. item 1";
        grammar::is_bulleted_text is_indented;
        CHECK(is_indented(text).first == true);
        CHECK(is_indented(text).second == 3);
        }
    SECTION("Number Bullet With Indenting")
        {
        const wchar_t text[] = L"  \t12. item 1";
        grammar::is_bulleted_text is_indented;
        CHECK(is_indented(text).first == true);
        CHECK(is_indented(text).second == 3);
        }
    SECTION("Number Bullet 2")
        {
        const wchar_t text[] = L"1, item 1";
        grammar::is_bulleted_text is_indented;
        CHECK(is_indented(text).first == false);
        CHECK(is_indented(text).second == 0);
        }
    SECTION("Number Bullet 3")
        {
        const wchar_t text[] = L"1";
        grammar::is_bulleted_text is_indented;
        CHECK(is_indented(text).first == false);
        CHECK(is_indented(text).second == 0);
        }
    SECTION("Number List")
        {
        const wchar_t text[] = L" 1 tablespoon\n 2 cups\n  1 1/2 cups\n\n";
        grammar::is_bulleted_text is_indented;
        CHECK(is_indented(text).first == true);
        CHECK(is_indented(text).second == 1);
        CHECK(is_indented(text+14).first == true);
        CHECK(is_indented(text+14).second == 1);
        CHECK(is_indented(text+22).first == true);
        CHECK(is_indented(text+22).second == 1);
        }
    SECTION("Number List Last Item Not List Item")
        {
        const wchar_t text[] = L"1 tablespoon\n 2 cups or\n1 1/2 cups\r\nSome text";
        grammar::is_bulleted_text is_indented;
        CHECK(is_indented(text).first == true);
        CHECK(is_indented(text).second == 1);
        CHECK(is_indented(text+14).first == true);
        CHECK(is_indented(text+14).second == 1);
        CHECK(is_indented(text+24).first == false);//\r\n will be seen as only one line
        CHECK(is_indented(text+24).second == 0);
        }
    SECTION("Number Number Paranthesis")
        {
        const wchar_t text[] = L"3.) item 1";
        grammar::is_bulleted_text is_indented;
        CHECK(is_indented(text).first == true);
        CHECK(is_indented(text).second == 3);
        }
    SECTION("Number Number Paranthesis 2")
        {
        const wchar_t text[] = L"3). item 1";
        grammar::is_bulleted_text is_indented;
        CHECK(is_indented(text).first == true);
        CHECK(is_indented(text).second == 3);
        }
    SECTION("Letter")
        {
        const wchar_t text[] = L"a. item 1";
        grammar::is_bulleted_text is_indented;
        CHECK(is_indented(text).first == true);
        CHECK(is_indented(text).second == 2);
        }
    SECTION("Letter Paranthesis")
        {
        const wchar_t text[] = L"a.) item 1";
        grammar::is_bulleted_text is_indented;
        CHECK(is_indented(text).first == true);
        CHECK(is_indented(text).second == 3);
        }
    SECTION("Letter Not Bullet")
        {
        const wchar_t text[] = L"B. Madden.";
        grammar::is_bulleted_text is_indented;
        CHECK(is_indented(text).first == false);
        CHECK(is_indented(text).second == 0);
        }
    SECTION("Plain Text")
        {
        const wchar_t text[] = L"This is some text";
        grammar::is_bulleted_text is_indented;
        CHECK(is_indented(text).first == false);
        CHECK(is_indented(text).second == 0);
        }
    SECTION("Dash")
        {
        const wchar_t text[] = L"- This is some text";
        grammar::is_bulleted_text is_indented;
        CHECK(is_indented(text).first == true);
        CHECK(is_indented(text).second == 1);
        }
    }

TEST_CASE("End of line", "[end-of-line]")
    {
    SECTION("NULLs")
        {
        grammar::is_end_of_line isEol;
        const wchar_t* text = L"\n\n\n";
        isEol(nullptr, text+std::wcslen(text));
        CHECK(isEol.get_characters_skipped_count() == 0);
        CHECK(isEol.get_eol_count() == 0);
        isEol(text, nullptr);
        CHECK(isEol.get_characters_skipped_count() == 0);
        CHECK(isEol.get_eol_count() == 0);
        }
    SECTION("Single Char")
        {
        grammar::is_end_of_line isEol;
        CHECK(isEol(L'\n'));
        CHECK(isEol(L'\r'));
        CHECK(isEol(L'\f'));//formfeed
        CHECK(isEol(L' ') == false);
        CHECK(isEol(L'n') == false);
        CHECK(isEol(L'\t') == false);
        CHECK(isEol(L'r') == false);
        }
    SECTION("Just New Lines")
        {
        grammar::is_end_of_line isEol;
        const wchar_t* text = L"\n\n\n";
        isEol(text, text+std::wcslen(text));
        CHECK(isEol.get_characters_skipped_count() == 3);
        CHECK(isEol.get_eol_count() == 3);
        }
    SECTION("One New Line")
        {
        grammar::is_end_of_line isEol;
        const wchar_t* text = L"\nsome text";
        isEol(text, text+std::wcslen(text));
        CHECK(isEol.get_characters_skipped_count() == 1);
        CHECK(isEol.get_eol_count() == 1);
        }
    SECTION("One Carriage Return")
        {
        grammar::is_end_of_line isEol;
        const wchar_t* text = L"\rsome text";
        isEol(text, text+std::wcslen(text));
        CHECK(isEol.get_characters_skipped_count() == 1);
        CHECK(isEol.get_eol_count() == 1);
        }
    SECTION("One CRLF Pair")
        {
        grammar::is_end_of_line isEol;
        const wchar_t* text = L"\n\rsome text";
        isEol(text, text+std::wcslen(text));
        CHECK(isEol.get_characters_skipped_count() == 2);
        CHECK(isEol.get_eol_count() == 1);
        }
    SECTION("Two New Lines")
        {
        grammar::is_end_of_line isEol;
        const wchar_t* text = L"\n\nsome text";
        isEol(text, text+std::wcslen(text));
        CHECK(isEol.get_characters_skipped_count() == 2);
        CHECK(isEol.get_eol_count() == 2);
        }
    SECTION("Two New Lines With Spaces")
        {
        grammar::is_end_of_line isEol;
        const wchar_t* text = L"\n \nsome text";
        isEol(text, text+std::wcslen(text));
        CHECK(isEol.get_characters_skipped_count() == 3);
        CHECK(isEol.get_eol_count() == 2);
        }
    SECTION("Four New Lines With Spaces")
        {
        grammar::is_end_of_line isEol;
        const wchar_t* text = L"\n \n\t \t\n  \t\nsome text";
        isEol(text, text+std::wcslen(text));
        CHECK(isEol.get_characters_skipped_count() == 11);
        CHECK(isEol.get_eol_count() == 4);
        }
    SECTION("Form feeds")
        {
        grammar::is_end_of_line isEol;
        const wchar_t* text = L"\n \n\r\f\n\r\t \t\n  \t\nsome text";
        isEol(text, text+std::wcslen(text));
        CHECK(isEol.get_characters_skipped_count() == 15);
        CHECK(isEol.get_eol_count() == 6);
        }
    SECTION("One New Lines With Spaces")
        {
        grammar::is_end_of_line isEol;
        const wchar_t* text = L"\n  some text";
        isEol(text, text+std::wcslen(text));
        CHECK(isEol.get_characters_skipped_count() == 1);
        CHECK(isEol.get_eol_count() == 1);
        }
    SECTION("One New Lines With Spaces No Following Text")
        {
        grammar::is_end_of_line isEol;
        const wchar_t* text = L"\n  ";
        isEol(text, text+std::wcslen(text));
        CHECK(isEol.get_characters_skipped_count() == 1);
        CHECK(isEol.get_eol_count() == 1);
        }
    SECTION("Two New Lines With Spaces No Following Text")
        {
        grammar::is_end_of_line isEol;
        const wchar_t* text = L"\n  \n";
        isEol(text, text+std::wcslen(text));
        CHECK(isEol.get_characters_skipped_count() == 4);
        CHECK(isEol.get_eol_count() == 2);
        }
    SECTION("Reset")
        {
        grammar::is_end_of_line isEol;
        const wchar_t* text = L"\n\nsome text";
        isEol(text, text+std::wcslen(text));
        CHECK(isEol.get_characters_skipped_count() == 2);
        CHECK(isEol.get_eol_count() == 2);
        text = L"\n \n\t \t\n  \t\nsome text";
        isEol(text, text+std::wcslen(text));
        CHECK(isEol.get_characters_skipped_count() == 11);
        CHECK(isEol.get_eol_count() == 4);
        text = L"\n  ";
        isEol(text, text+std::wcslen(text));
        CHECK(isEol.get_characters_skipped_count() == 1);
        CHECK(isEol.get_eol_count() == 1);
        text = L"  ";
        isEol(text, text+std::wcslen(text));
        CHECK(isEol.get_characters_skipped_count() == 0);
        CHECK(isEol.get_eol_count() == 0);
        }
    }

TEST_CASE("Passive Voice", "[passive-voice]")
    {
    std::vector<std::wstring> m_strings;

    SECTION("Ingore Proper Nouns")
        {
        grammar::is_english_passive_voice pasV;
        size_t wordCount;

        m_strings.clear();
        m_strings.push_back(L"am");
        m_strings.push_back(L"Holden");//proper name that looks like a verb
        CHECK(pasV(m_strings.begin(), m_strings.size(), wordCount) == false);
        CHECK(wordCount == 0);

        m_strings.clear();
        m_strings.push_back(L"am");
        m_strings.push_back(L"holden");//verb
        CHECK(pasV(m_strings.begin(), m_strings.size(), wordCount));
        CHECK(wordCount == 2);

        m_strings.clear();
        m_strings.push_back(L"AM");
        m_strings.push_back(L"BOLDEN");//exclamatory? Assume this is a verb then
        CHECK(pasV(m_strings.begin(), m_strings.size(), wordCount));
        CHECK(wordCount == 2);
        }
    SECTION("ED Exceptions")
        {
        grammar::is_english_passive_voice pasV;
        size_t wordCount;
        m_strings.clear();
        m_strings.push_back(L"am");
        m_strings.push_back(L"excited");
        CHECK(pasV(m_strings.begin(), m_strings.size(), wordCount) == false);

        m_strings.push_back(L"from");
        CHECK(pasV(m_strings.begin(), m_strings.size(), wordCount) == false);

        m_strings[2] = L"by";
        CHECK(pasV(m_strings.begin(), m_strings.size(), wordCount));
        CHECK(wordCount == 3);

        m_strings.clear();
        m_strings.push_back(L"am");
        m_strings.push_back(L"not");
        m_strings.push_back(L"excited");
        m_strings.push_back(L"by");
        CHECK(pasV(m_strings.begin(), m_strings.size(), wordCount));
        CHECK(wordCount == 4);

        m_strings[3] = L"bye";
        CHECK(pasV(m_strings.begin(), m_strings.size(), wordCount) == false);
        CHECK(wordCount == 0);

        m_strings.clear();
        m_strings.push_back(L"am");
        m_strings.push_back(L"not");
        m_strings.push_back(L"excited");
        CHECK(pasV(m_strings.begin(), m_strings.size(), wordCount) == false);
        CHECK(wordCount == 0);
        }
    SECTION("Simple")
        {
        grammar::is_english_passive_voice pasV;
        size_t wordCount;

        m_strings.clear();
        m_strings.push_back(L"be");
        m_strings.push_back(L"noted");
        CHECK(pasV(m_strings.begin(), m_strings.size(), wordCount));
        CHECK(wordCount == 2);

        m_strings.clear();
        m_strings.push_back(L"am");
        m_strings.push_back(L"given");
        CHECK(pasV(m_strings.begin(), m_strings.size(), wordCount));
        CHECK(wordCount == 2);

        m_strings.clear();
        m_strings.push_back(L"was");
        m_strings.push_back(L"given");
        CHECK(pasV(m_strings.begin(), m_strings.size(), wordCount));
        CHECK(wordCount == 2);

        m_strings.clear();
        m_strings.push_back(L"wasn’t");
        m_strings.push_back(L"given");
        CHECK(pasV(m_strings.begin(), m_strings.size(), wordCount));
        CHECK(wordCount == 2);

        m_strings.clear();
        m_strings.push_back(L"are");
        m_strings.push_back(L"portrayed");
        CHECK(pasV(m_strings.begin(), m_strings.size(), wordCount));
        CHECK(wordCount == 2);

        m_strings.clear();
        m_strings.push_back(L"aren’t");
        m_strings.push_back(L"portrayed");
        CHECK(pasV(m_strings.begin(), m_strings.size(), wordCount));
        CHECK(wordCount == 2);

        m_strings.clear();
        m_strings.push_back(L"been");
        m_strings.push_back(L"saved");
        CHECK(pasV(m_strings.begin(), m_strings.size(), wordCount));
        CHECK(wordCount == 2);

        m_strings.clear();
        m_strings.push_back(L"being");
        m_strings.push_back(L"saved");
        CHECK(pasV(m_strings.begin(), m_strings.size(), wordCount));
        CHECK(wordCount == 2);

        m_strings.clear();
        m_strings.push_back(L"is");
        m_strings.push_back(L"paved");
        CHECK(pasV(m_strings.begin(), m_strings.size(), wordCount));
        CHECK(wordCount == 2);

        m_strings.clear();
        m_strings.push_back(L"isn't");
        m_strings.push_back(L"paved");
        CHECK(pasV(m_strings.begin(), m_strings.size(), wordCount));
        CHECK(wordCount == 2);

        m_strings.clear();
        m_strings.push_back(L"were");
        m_strings.push_back(L"taken");
        CHECK(pasV(m_strings.begin(), m_strings.size(), wordCount));
        CHECK(wordCount == 2);

        m_strings.clear();
        m_strings.push_back(L"weren't");
        m_strings.push_back(L"taken");
        CHECK(pasV(m_strings.begin(), m_strings.size(), wordCount));
        CHECK(wordCount == 2);

        m_strings.clear();
        m_strings.push_back(L"AM");
        m_strings.push_back(L"GIVEN");
        CHECK(pasV(m_strings.begin(), m_strings.size(), wordCount));
        CHECK(wordCount == 2);

        m_strings.clear();
        m_strings.push_back(L"WAS");
        m_strings.push_back(L"GIVEN");
        CHECK(pasV(m_strings.begin(), m_strings.size(), wordCount));
        CHECK(wordCount == 2);

        m_strings.clear();
        m_strings.push_back(L"WASN’T");
        m_strings.push_back(L"GIVEN");
        CHECK(pasV(m_strings.begin(), m_strings.size(), wordCount));
        CHECK(wordCount == 2);

        m_strings.clear();
        m_strings.push_back(L"ARE");
        m_strings.push_back(L"PORTRAYED");
        CHECK(pasV(m_strings.begin(), m_strings.size(), wordCount));
        CHECK(wordCount == 2);

        m_strings.clear();
        m_strings.push_back(L"AREN’T");
        m_strings.push_back(L"PORTRAYED");
        CHECK(pasV(m_strings.begin(), m_strings.size(), wordCount));
        CHECK(wordCount == 2);

        m_strings.clear();
        m_strings.push_back(L"BEEN");
        m_strings.push_back(L"SAVED");
        CHECK(pasV(m_strings.begin(), m_strings.size(), wordCount));
        CHECK(wordCount == 2);

        m_strings.clear();
        m_strings.push_back(L"BEING");
        m_strings.push_back(L"SAVED");
        CHECK(pasV(m_strings.begin(), m_strings.size(), wordCount));
        CHECK(wordCount == 2);

        m_strings.clear();
        m_strings.push_back(L"IS");
        m_strings.push_back(L"PAVED");
        CHECK(pasV(m_strings.begin(), m_strings.size(), wordCount));
        CHECK(wordCount == 2);

        m_strings.clear();
        m_strings.push_back(L"ISN'T");
        m_strings.push_back(L"PAVED");
        CHECK(pasV(m_strings.begin(), m_strings.size(), wordCount));
        CHECK(wordCount == 2);

        m_strings.clear();
        m_strings.push_back(L"WERE");
        m_strings.push_back(L"TAKEN");
        CHECK(pasV(m_strings.begin(), m_strings.size(), wordCount));
        CHECK(wordCount == 2);

        m_strings.clear();
        m_strings.push_back(L"WEREN'T");
        m_strings.push_back(L"TAKEN");
        CHECK(pasV(m_strings.begin(), m_strings.size(), wordCount));
        CHECK(wordCount == 2);
        }
    SECTION("Negative Simple")
        {
        grammar::is_english_passive_voice pasV;
        size_t wordCount;

        m_strings.clear();
        m_strings.push_back(L"am");
        m_strings.push_back(L"not");
        m_strings.push_back(L"given");
        CHECK(pasV(m_strings.begin(), m_strings.size(), wordCount));
        CHECK(wordCount == 3);

        m_strings.clear();
        m_strings.push_back(L"was");
        m_strings.push_back(L"not");
        m_strings.push_back(L"given");
        CHECK(pasV(m_strings.begin(), m_strings.size(), wordCount));
        CHECK(wordCount == 3);

        m_strings.clear();
        m_strings.push_back(L"are");
        m_strings.push_back(L"not");
        m_strings.push_back(L"portrayed");
        CHECK(pasV(m_strings.begin(), m_strings.size(), wordCount));
        CHECK(wordCount == 3);

        m_strings.clear();
        m_strings.push_back(L"been");
        m_strings.push_back(L"not");
        m_strings.push_back(L"saved");
        CHECK(pasV(m_strings.begin(), m_strings.size(), wordCount));
        CHECK(wordCount == 3);

        m_strings.clear();
        m_strings.push_back(L"being");
        m_strings.push_back(L"not");
        m_strings.push_back(L"saved");
        CHECK(pasV(m_strings.begin(), m_strings.size(), wordCount));
        CHECK(wordCount == 3);

        m_strings.clear();
        m_strings.push_back(L"is");
        m_strings.push_back(L"not");
        m_strings.push_back(L"paved");
        CHECK(pasV(m_strings.begin(), m_strings.size(), wordCount));
        CHECK(wordCount == 3);

        m_strings.clear();
        m_strings.push_back(L"were");
        m_strings.push_back(L"not");
        m_strings.push_back(L"taken");
        CHECK(pasV(m_strings.begin(), m_strings.size(), wordCount));
        CHECK(wordCount == 3);

        m_strings.clear();
        m_strings.push_back(L"AM");
        m_strings.push_back(L"NOT");
        m_strings.push_back(L"GIVEN");
        CHECK(pasV(m_strings.begin(), m_strings.size(), wordCount));
        CHECK(wordCount == 3);

        m_strings.clear();
        m_strings.push_back(L"WAS");
        m_strings.push_back(L"NOT");
        m_strings.push_back(L"GIVEN");
        CHECK(pasV(m_strings.begin(), m_strings.size(), wordCount));
        CHECK(wordCount == 3);

        m_strings.clear();
        m_strings.push_back(L"ARE");
        m_strings.push_back(L"NOT");
        m_strings.push_back(L"PORTRAYED");
        CHECK(pasV(m_strings.begin(), m_strings.size(), wordCount));
        CHECK(wordCount == 3);

        m_strings.clear();
        m_strings.push_back(L"BEEN");
        m_strings.push_back(L"NOT");
        m_strings.push_back(L"SAVED");
        CHECK(pasV(m_strings.begin(), m_strings.size(), wordCount));
        CHECK(wordCount == 3);

        m_strings.clear();
        m_strings.push_back(L"BEING");
        m_strings.push_back(L"NOT");
        m_strings.push_back(L"SAVED");
        CHECK(pasV(m_strings.begin(), m_strings.size(), wordCount));
        CHECK(wordCount == 3);

        m_strings.clear();
        m_strings.push_back(L"IS");
        m_strings.push_back(L"NOT");
        m_strings.push_back(L"PAVED");
        CHECK(pasV(m_strings.begin(), m_strings.size(), wordCount));
        CHECK(wordCount == 3);

        m_strings.clear();
        m_strings.push_back(L"WERE");
        m_strings.push_back(L"NOT");
        m_strings.push_back(L"TAKEN");
        CHECK(pasV(m_strings.begin(), m_strings.size(), wordCount));
        CHECK(wordCount == 3);
        }
    SECTION("Not To Be Verbs")
        {
        grammar::is_english_passive_voice pasV;
        size_t wordCount;

        m_strings.clear();
        m_strings.push_back(L"sam");
        m_strings.push_back(L"given");
        CHECK(pasV(m_strings.begin(), m_strings.size(), wordCount) == false);

        m_strings.clear();
        m_strings.push_back(L"waste");
        m_strings.push_back(L"given");
        CHECK(pasV(m_strings.begin(), m_strings.size(), wordCount) == false);

        m_strings.clear();
        m_strings.push_back(L"art");
        m_strings.push_back(L"portrayed");
        CHECK(pasV(m_strings.begin(), m_strings.size(), wordCount) == false);

        m_strings.clear();
        m_strings.push_back(L"beat");
        m_strings.push_back(L"saved");
        CHECK(pasV(m_strings.begin(), m_strings.size(), wordCount) == false);

        m_strings.clear();
        m_strings.push_back(L"beats");
        m_strings.push_back(L"saved");
        CHECK(pasV(m_strings.begin(), m_strings.size(), wordCount) == false);

        m_strings.clear();
        m_strings.push_back(L"ist");
        m_strings.push_back(L"paved");
        CHECK(pasV(m_strings.begin(), m_strings.size(), wordCount) == false);

        m_strings.clear();
        m_strings.push_back(L"worm");
        m_strings.push_back(L"taken");
        CHECK(pasV(m_strings.begin(), m_strings.size(), wordCount) == false);
        CHECK(wordCount == 0);
        }
    SECTION("Not Past Participles")
        {
        grammar::is_english_passive_voice pasV;
        size_t wordCount;

        m_strings.clear();
        m_strings.push_back(L"am");
        m_strings.push_back(L"gists");
        CHECK(pasV(m_strings.begin(), m_strings.size(), wordCount) == false);

        m_strings.clear();
        m_strings.push_back(L"was");
        m_strings.push_back(L"gists");
        CHECK(pasV(m_strings.begin(), m_strings.size(), wordCount) == false);

        m_strings.clear();
        m_strings.push_back(L"are");
        m_strings.push_back(L"portrayer");
        CHECK(pasV(m_strings.begin(), m_strings.size(), wordCount) == false);

        m_strings.clear();
        m_strings.push_back(L"been");
        m_strings.push_back(L"saver");
        CHECK(pasV(m_strings.begin(), m_strings.size(), wordCount) == false);

        m_strings.clear();
        m_strings.push_back(L"being");
        m_strings.push_back(L"saver");
        CHECK(pasV(m_strings.begin(), m_strings.size(), wordCount) == false);

        m_strings.clear();
        m_strings.push_back(L"is");
        m_strings.push_back(L"pavor");
        CHECK(pasV(m_strings.begin(), m_strings.size(), wordCount) == false);

        m_strings.clear();
        m_strings.push_back(L"were");
        m_strings.push_back(L"taker");
        CHECK(pasV(m_strings.begin(), m_strings.size(), wordCount) == false);

        m_strings.clear();
        m_strings.push_back(L"were");
        m_strings.push_back(L"eighteen");
        CHECK(pasV(m_strings.begin(), m_strings.size(), wordCount) == false);

        m_strings.clear();
        m_strings.push_back(L"were");
        m_strings.push_back(L"seven");
        m_strings.push_back(L"by");
        CHECK(pasV(m_strings.begin(), m_strings.size(), wordCount) == false);

        m_strings.clear();
        m_strings.push_back(L"were");
        m_strings.push_back(L"sponsored");
        m_strings.push_back(L"by");
        CHECK(pasV(m_strings.begin(), m_strings.size(), wordCount));

        m_strings.clear();
        m_strings.push_back(L"were");
        m_strings.push_back(L"Heaven");
        CHECK(pasV(m_strings.begin(), m_strings.size(), wordCount) == false);

        m_strings.clear();
        m_strings.push_back(L"were");
        m_strings.push_back(L"alien");
        CHECK(pasV(m_strings.begin(), m_strings.size(), wordCount) == false);

        m_strings.clear();
        m_strings.push_back(L"were");
        m_strings.push_back(L"haven");
        CHECK(pasV(m_strings.begin(), m_strings.size(), wordCount) == false);

        m_strings.clear();
        m_strings.push_back(L"were");
        m_strings.push_back(L"infrared");
        CHECK(pasV(m_strings.begin(), m_strings.size(), wordCount) == false);

        m_strings.clear();
        m_strings.push_back(L"were");
        m_strings.push_back(L"sacred");
        CHECK(pasV(m_strings.begin(), m_strings.size(), wordCount) == false);

        m_strings.clear();
        m_strings.push_back(L"were");
        m_strings.push_back(L"not");
        m_strings.push_back(L"eighteen");
        CHECK(pasV(m_strings.begin(), m_strings.size(), wordCount) == false);
        CHECK(wordCount == 0);
        }
    }

TEST_CASE("Abbreviations", "[abbreviations]")
    {
    grammar::is_abbreviation abb;
    abb.get_abbreviations().add_word(L"std.");
        abb.get_abbreviations().add_word(L"dept.");
        abb.get_abbreviations().add_word(L"mar.");
        abb.get_abbreviations().add_word(L"jan.");

    SECTION("TestNulls")
        {
        CHECK_FALSE(abb(nullptr, 1));
        CHECK_FALSE(abb(L"std.", 0));
        CHECK_FALSE(abb(L"", 0));
        }
    SECTION("Known")
        {
        CHECK(abb(L"std.", 4));
        CHECK(abb(L"dept.", 5));
        CHECK_FALSE(abb(L"stand.", 6));
        }
    SECTION("Unknown")
        {
        CHECK(abb(L"stdr.", 5)); // all consonants
        }
    SECTION("Slash")
        {
        CHECK(abb(L"class/dept.", 11)); // dept. is an abbreviation
        CHECK_FALSE(abb(L"class/depart.", 13)); // depart. is not an abbreviation
        CHECK_FALSE(abb(L"class/", 6)); // nothing there
        }
    }
