#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>
#include "../src/indexing/character_traits.h"

TEST_CASE("Wide & Narrow", "[string]")
    {
    SECTION("Narrow letters")
        {
        traits::case_insensitive_wstring_ex wide(L"ＨＥＬＬＯ");
        traits::case_insensitive_wstring_ex narrow(L"Hello");
        CHECK(wide == narrow);
        }
    }
