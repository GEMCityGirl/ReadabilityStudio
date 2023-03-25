#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers.hpp>
#include <catch2/catch_session.hpp>
#include <fstream>
#include <iostream>
#include "../src/indexing/article.h"
#include "../src/indexing/stop_lists.h"
#include "../src/indexing/word_collection.h"

using namespace grammar;

word_list Stop_list;
phrase_collection pcmap;

// Main entry point for test harness
//--------------------------------------------
int main( int argc, char* argv[] )
    {
    const auto loadResourceFile = [](const auto& filePath)
        {
        std::wifstream inputFile(filePath);
        if (!inputFile.is_open())
            { std::wcout << filePath << L" file failed to load."; }
        std::wstring line;
        std::wstring buffer;
        while (std::getline(inputFile, line))
            { buffer.append(L"\n").append(line); }
        return buffer;
        };

    // initialize everything
    is_abbreviation::get_abbreviations().load_words(
        loadResourceFile(L"WesternAbbr.txt").c_str(), true, false);
    grammar::is_non_proper_word::get_word_list().load_words(
        loadResourceFile(L"StopListForProper.txt").c_str(), true, false);
    Stop_list.load_words(
        loadResourceFile(L"StopListWestern.txt").c_str(), true, false);
    grammar::is_english_passive_voice::get_past_participle_exeptions().load_words(
        L"excited\nseven\nheaven\nhaven\nalien\ninfrared\nsacred", true, true);

    return Catch::Session().run( argc, argv );
    }
