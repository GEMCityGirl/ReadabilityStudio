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
#include <catch2/matchers/catch_matchers_floating_point.hpp>
#include "../src/indexing/spanish_syllabize.h"

// clang-format off
// NOLINTBEGIN

using namespace Catch::Matchers;
using namespace grammar;

TEST_CASE("Spanish syllabizer", "[syllable]")
    {
    SECTION("Numbers")
        {
        spanish_syllabize syllabize;
        CHECK(syllabize(L"0", 1) == 2);
        CHECK(syllabize(L"1", 1) == 2);
        CHECK(syllabize(L"2", 1) == 1);
        CHECK(syllabize(L"3", 1) == 1);
        CHECK(syllabize(L"4", 1) == 2);
        CHECK(syllabize(L"5", 1) == 2);
        CHECK(syllabize(L"6", 1) == 1);
        CHECK(syllabize(L"7", 1) == 3);
        CHECK(syllabize(L"8", 1) == 2);
        CHECK(syllabize(L"9", 1) == 3);
        CHECK(syllabize(L"10", 1) == 2);
        CHECK(syllabize(L"85", 2) == 4);
        CHECK(syllabize(L"18.7", 4) == 8);
        CHECK(syllabize(L"$18.7", 5) == 10);
        CHECK(syllabize(L"\x20B1", 1) == 2);
        CHECK(syllabize(L"\x20AC", 1) == 2);
        }
    SECTION("Vowel Blocks")
        {
        spanish_syllabize syllabize;
        CHECK(syllabize(L"pingüino", 8) == 3);
        CHECK(syllabize(L"antigüedad", 10) == 4);
        }
    SECTION("EO")
        {
        spanish_syllabize syllabize;
        CHECK(syllabize(L"apogeo", 6) == 4);
        CHECK(syllabize(L"apogEOS", 7) == 4);
        CHECK(syllabize(L"ajetreo", 7) == 4);
        CHECK(syllabize(L"apogeo-ajetreo", 14) == 8);// test dash
        }
    SECTION("EA")
        {
        spanish_syllabize syllabize;
        CHECK(syllabize(L"azotea", 6) == 4);
        CHECK(syllabize(L"azotEAS", 7) == 4);
        }
    SECTION("ACAE")
        {
        spanish_syllabize syllabize;
        CHECK(syllabize(L"acaecer", 6) == 4);
        CHECK(syllabize(L"acaecimiento", 12) == 6);
        }
    SECTION("AERO")
        {
        spanish_syllabize syllabize;
        CHECK(syllabize(L"aerÓbic", 7) == 3);
        CHECK(syllabize(L"aerolÍnea", 9) == 6);
        CHECK(syllabize(L"aeronauta", 9) == 5);
        CHECK(syllabize(L"aeroplano", 9) == 5);
        }
    SECTION("TUOSOA")
        {
        spanish_syllabize syllabize;
        CHECK(syllabize(L"afectuoso", 9) == 5);
        CHECK(syllabize(L"afectuosa", 9) == 5);
        CHECK(syllabize(L"afectuosamente", 14) == 7);
        CHECK(syllabize(L"monstruosos", 11) == 4);
        }
    SECTION("TUAL")
        {
        spanish_syllabize syllabize;
        CHECK(syllabize(L"actual", 6) == 2);
        CHECK(syllabize(L"conventual", 10) == 3);
        CHECK(syllabize(L"ritual", 6) == 2);
        CHECK(syllabize(L"textual", 7) == 2);
        CHECK(syllabize(L"eventual", 8) == 3);
        }
    SECTION("ANTI")
        {
        spanish_syllabize syllabize;
        CHECK(syllabize(L"antier", 6) == 3);
        CHECK(syllabize(L"antiinfamatorio", 15) == 7);
        }
    SECTION("IADOA")
        {
        spanish_syllabize syllabize;
        CHECK(syllabize(L"vaciado", 7) == 4);
        CHECK(syllabize(L"vaciada", 7) == 4);
        CHECK(syllabize(L"VACIADO", 7) == 4);
        CHECK(syllabize(L"VACIADA", 7) == 4);
        CHECK(syllabize(L"vaciados", 8) == 4);
        CHECK(syllabize(L"vaciadas", 8) == 4);
        CHECK(syllabize(L"guiados", 7) == 3);
        CHECK(syllabize(L"historiador", 11) == 5);
        CHECK(syllabize(L"historiadores", 13) == 6);
        CHECK(syllabize(L"criado", 6) == 2);
        }
    SECTION("FIANZ")
        {
        spanish_syllabize syllabize;
        CHECK(syllabize(L"confianza", 9) == 4);
        CHECK(syllabize(L"confianzudo", 11) == 5);
        CHECK(syllabize(L"CONFIANZA", 9) == 4);
        CHECK(syllabize(L"CONFIANZUDO", 11) == 5);
        CHECK(syllabize(L"confianzas", 10) == 4);
        CHECK(syllabize(L"confianzudos", 12) == 5);
        CHECK(syllabize(L"CONFIANZAS", 10) == 4);
        CHECK(syllabize(L"confianzudos", 12) == 5);
        }
    SECTION("Journal Samples")
        {
        spanish_syllabize syllabize;
        CHECK(syllabize(L"Cierto", 6) == 2);
        CHECK(syllabize(L"hombre", 6) == 2);
        CHECK(syllabize(L"que", 3) == 1);
        CHECK(syllabize(L"habia", 5) == 2);
        CHECK(syllabize(L"comprado", 8) ==3 );
        CHECK(syllabize(L"una", 3) == 2);
        CHECK(syllabize(L"vaca", 4) == 2);
        CHECK(syllabize(L"magnifica", 9) == 4);
        CHECK(syllabize(L"vuelan", 6) == 2);
        CHECK(syllabize(L"generalmente", 12) == 5);
        CHECK(syllabize(L"cuadrupedos", 11) == 4);
        CHECK(syllabize(L"Los", 3) == 1);
        CHECK(syllabize(L"camino", 6) == 3);
        CHECK(syllabize(L"del", 3) == 1);
        CHECK(syllabize(L"lado", 4) == 2);
        CHECK(syllabize(L"sono", 4) == 2);
        CHECK(syllabize(L"noche ", 5) == 2);
        CHECK(syllabize(L"Considerando", 12) == 5);
        CHECK(syllabize(L"presagio ", 8) == 3);
        CHECK(syllabize(L"nuevamente", 10) == 4);
        CHECK(syllabize(L"mercado", 7) == 3);
        CHECK(syllabize(L"infortunio", 10) == 4);
        CHECK(syllabize(L"Envolviendo", 11) == 4);
        CHECK(syllabize(L"comiendo", 8) == 3);
        CHECK(syllabize(L"Fue", 3) == 1);
        CHECK(syllabize(L"escapar", 7) == 3);
        CHECK(syllabize(L"primero", 7) == 3);
        CHECK(syllabize(L"historia", 8) == 3);
        }

    SECTION("Numbers")
        {
        syllabize_spanish_number syllabize;
        CHECK(syllabize(L'0') == 2);
        CHECK(syllabize(L'1') == 2);
        CHECK(syllabize(L'2') == 1);
        CHECK(syllabize(L'3') == 1);
        CHECK(syllabize(L'4') == 2);
        CHECK(syllabize(L'5') == 2);
        CHECK(syllabize(L'6') == 1);
        CHECK(syllabize(L'7') == 3);
        CHECK(syllabize(L'8') == 2);
        CHECK(syllabize(L'9') == 3);
        }
    }
// NOLINTEND
// clang-format on
