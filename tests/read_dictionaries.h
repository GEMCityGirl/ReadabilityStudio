#ifndef __READ_DICTIONARIES_H__
#define __READ_DICTIONARIES_H__

#include <fstream>
#include <iostream>
#include <filesystem>
#include <numeric>
#include "../src/indexing/article.h"
#include "../src/indexing/tokenize.h"
#include "../src/indexing/stop_lists.h"
#include "../src/indexing/word_collection.h"
#include "../src/indexing/spanish_syllabize.h"
#include "../src/indexing/german_syllabize.h"
#include "../src/Wisteria-Dataviz/src/utfcpp/source/utf8.h"

//-------------------------------------------------
inline bool valid_utf8_file(const std::string& file_name)
    {
    std::ifstream ifs(file_name);
    if (!ifs)
        { return false; }

    std::istreambuf_iterator<char> it(ifs.rdbuf());
    std::istreambuf_iterator<char> eos;

    if (utf8::starts_with_bom(it, eos))
        {
        std::advance(it, std::size(utf8::bom));
        }

    return utf8::is_valid(it, eos);
    }

//-------------------------------------------------
inline std::pair<bool, std::wstring> read_utf8_file(const std::string& file_name)
    {
    if (!valid_utf8_file(file_name))
        { return std::make_pair(false, std::wstring{}); }

    std::ifstream fs8(file_name);
    if (!fs8.is_open())
        {
        std::cout << "Could not open " << file_name << "\n";
        return std::make_pair(false, std::wstring{});
        }

    unsigned line_count = 1;
    std::string line;
    std::wstring buffer;
    buffer.reserve(static_cast<size_t>(std::filesystem::file_size(file_name)));

    while (std::getline(fs8, line))
        {
        if (auto end_it = utf8::find_invalid(line.begin(), line.end());
            end_it != line.cend())
            {
            std::cout << "Invalid UTF-8 encoding detected at line " << line_count << "\n";
            std::cout << "This part is fine: " << std::string(line.begin(), end_it) << "\n";
            }

        // Convert it to utf-16
        std::u16string utf16line = utf8::utf8to16(line);
        for (const auto& ch : utf16line)
            { buffer += static_cast<wchar_t>(ch); }
        buffer += L"\n";

        ++line_count;
        }

    return std::make_pair(true, buffer);
    }

#endif __READ_DICTIONARIES_H__
