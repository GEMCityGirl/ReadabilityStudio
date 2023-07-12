#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers.hpp>
#include <catch2/catch_session.hpp>
#include <fstream>
#include <iostream>
#include <filesystem>
#include <numeric>
#include "read_dictionaries.h"
#include "../src/indexing/article.h"
#include "../src/indexing/tokenize.h"
#include "../src/indexing/stop_lists.h"
#include "../src/indexing/word_collection.h"
#include "../src/indexing/spanish_syllabize.h"
#include "../src/indexing/german_syllabize.h"
#include "../src/Wisteria-Dataviz/src/utfcpp/source/utf8.h"

using namespace grammar;
namespace fs = std::filesystem;

word_list Stop_list;
phrase_collection pcmap;

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

// Main entry point for test harness
//--------------------------------------------
int main( int argc, char* argv[] )
    {
    const auto loadResourceFile = [](const std::string& filePath)
        {
        std::wifstream inputFile;
        inputFile.open(filePath.c_str(), std::ifstream::in);
        if (!inputFile.is_open())
            { std::cout << filePath << " file failed to load."; }
        std::wstring line;
        std::wstring buffer;
        while (std::getline(inputFile, line))
            { buffer.append(L"\n").append(line); }
        return buffer;
        };

    // initialize everything
    is_abbreviation::get_abbreviations().load_words(
        read_utf8_file("WesternAbbr.txt").second.c_str(), true, false);
    grammar::is_non_proper_word::get_word_list().load_words(
        read_utf8_file("StopListForProper.txt").second.c_str(), true, false);
    Stop_list.load_words(
        read_utf8_file("StopListWestern.txt").second.c_str(), true, false);
    grammar::is_english_passive_voice::get_past_participle_exeptions().load_words(
        L"excited\nseven\nheaven\nhaven\nalien\ninfrared\nsacred", true, true);

    Catch::Session().run(argc, argv);

    return EXIT_SUCCESS;
    }
