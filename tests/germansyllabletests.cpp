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
#include "../src/indexing/german_syllabize.h"

// clang-format off
// NOLINTBEGIN

using namespace grammar;

TEST_CASE("German Syllabizer", "[syllable]")
    {
    SECTION("Regular Words")
        {
        german_syllabize syllabize;
        CHECK(syllabize(L"fisole", 6) == 3);
        CHECK(syllabize(L"fingerabdruck", 13) == 4);
        CHECK(syllabize(L"flugzeug", 8) == 2);
        CHECK(syllabize(L"flusslauf", 9) == 2);
        CHECK(syllabize(L"bleiben", 7) == 2);
        CHECK(syllabize(L"formular", 8) == 3);
        CHECK(syllabize(L"freirbier", 9) == 2);
        CHECK(syllabize(L"freund", 6) == 1);
        // capitalized
        CHECK(syllabize(L"FISOLE", 6) == 3);
        CHECK(syllabize(L"FINGERABDRUCK", 13) == 4);
        CHECK(syllabize(L"FLUGZEUG", 8) == 2);
        CHECK(syllabize(L"FLUSSLAUF", 9) == 2);
        CHECK(syllabize(L"BLEIBEN", 7) == 2);
        CHECK(syllabize(L"FORMULAR", 8) == 3);
        CHECK(syllabize(L"FREIRBIER", 9) == 2);
        CHECK(syllabize(L"FREUND", 6) == 1);
        }
    SECTION("Seperable Prefixes")
        {
        german_syllabize syllabize;
        CHECK(syllabize(L"abbrechen", 9) == 3);
        CHECK(syllabize(L"abo", 3) == 2);
        CHECK(syllabize(L"abs", 3) == 1);
        CHECK(syllabize(L"absolut", 7) == 3);
        CHECK(syllabize(L"zuerst", 6) == 2);
        // capitalized
        CHECK(syllabize(L"ABBRECHEN", 9) == 3);
        CHECK(syllabize(L"ABO", 3) == 2);
        CHECK(syllabize(L"ABS", 3) == 1);
        CHECK(syllabize(L"ABSOLUT", 7) == 3);
        CHECK(syllabize(L"ZUERST", 6) == 2);
        }
    SECTION("Past Tense")
        {
        german_syllabize syllabize;
        CHECK(syllabize(L"geiffnen", 8) == 3);
        CHECK(syllabize(L"angeeignet", 10) == 4);
        CHECK(syllabize(L"dageiffnen", 10) == 4);
        // capitalized
        CHECK(syllabize(L"GEIFFNEN", 8) == 3);
        CHECK(syllabize(L"ANGEEIGNET", 10) == 4);
        CHECK(syllabize(L"DAGEIFFNEN", 10) == 4);
        }
    SECTION("Special Chars")
        {
        german_syllabize syllabize;
        CHECK(syllabize(L"WÄSCHT", 6) == 1);
        CHECK(syllabize(L"wäscht", 6) == 1);
        CHECK(syllabize(L"VORÜBER", 7) == 3);
        CHECK(syllabize(L"vorüber", 7) == 3);
        CHECK(syllabize(L"versöhnung", 10) == 3);
        CHECK(syllabize(L"VERSÖHNUNG", 10) == 3);
        CHECK(syllabize(L"paßen", 5) == 2);
        }
    SECTION("Numbers")
        {
        german_syllabize syllabize;
        CHECK(syllabize(L"2D", 2) == 2);
        CHECK(syllabize(L"0", 1) == 1);
        CHECK(syllabize(L"1", 1) == 1);
        CHECK(syllabize(L"2", 1) == 1);
        CHECK(syllabize(L"3", 1) == 1);
        CHECK(syllabize(L"4", 1) == 1);
        CHECK(syllabize(L"5", 1) == 1);
        CHECK(syllabize(L"6", 1) == 1);
        CHECK(syllabize(L"7", 1) == 2);
        CHECK(syllabize(L"8", 1) == 1);
        CHECK(syllabize(L"9", 1) == 1);
        CHECK(syllabize(L"10", 1) == 1);
        CHECK(syllabize(L"85", 2) == 2);
        CHECK(syllabize(L"18.7", 4) == 5);
        CHECK(syllabize(L"$18.7", 5) == 7);
        CHECK(syllabize(L"\x20B1", 1) == 2);
        CHECK(syllabize(L"\x20AC", 1) == 2);
        }
    SECTION("Vowel Blocks")
        {
        german_syllabize syllabize;
        CHECK(syllabize(L"koeff", 5) == 2);
        CHECK(syllabize(L"koefficient", 11) == 4);
        CHECK(syllabize(L"quartiere", 9) == 3);
        CHECK(syllabize(L"hauptquartier", 13) == 3);
        CHECK(syllabize(L"januar", 6) == 3);
        CHECK(syllabize(L"februar", 7) == 3);
        CHECK(syllabize(L"february", 8) == 4);
        CHECK(syllabize(L"auszuarbeiten", 13) == 5);
        CHECK(syllabize(L"hilfsquellen", 12) == 3);
        CHECK(syllabize(L"manuellen", 9) == 4);
        CHECK(syllabize(L"aktuell", 7) == 3);
        CHECK(syllabize(L"aktuelle", 8) == 4);
        CHECK(syllabize(L"kategoriale", 11) == 6);
        CHECK(syllabize(L"Kategorien", 10) == 5);
        CHECK(syllabize(L"formaliens", 10) == 4);
        CHECK(syllabize(L"ferienjob", 9) == 3);
        CHECK(syllabize(L"beet", 4) == 1);
        CHECK(syllabize(L"beere", 5) == 2);
        CHECK(syllabize(L"beerdigung", 10) == 4);
        CHECK(syllabize(L"beerben", 7) == 3);
        CHECK(syllabize(L"beeilen", 7) == 3);
        CHECK(syllabize(L"beehren", 7) == 3);
        CHECK(syllabize(L"beenden", 7) == 3);
        CHECK(syllabize(L"ausgeschrieen", 13) == 4);
        CHECK(syllabize(L"geartet", 7) == 3);
        CHECK(syllabize(L"daune", 5) == 2);
        CHECK(syllabize(L"armee", 5) == 2);
        CHECK(syllabize(L"armeeabschaff", 13) == 4);
        CHECK(syllabize(L"geist", 5) == 1);
        CHECK(syllabize(L"geißel", 6) == 2);
        CHECK(syllabize(L"poltergeist", 11) == 3);
        CHECK(syllabize(L"geeignet", 8) == 3);
        CHECK(syllabize(L"staat", 5) == 1);
        CHECK(syllabize(L"fotoalbum", 9) == 4);
        CHECK(syllabize(L"koalition", 9) == 5);
        CHECK(syllabize(L"toast", 5) == 1);
        CHECK(syllabize(L"frauen", 6) == 2);
        CHECK(syllabize(L"fernsteuern", 11) == 3);
        CHECK(syllabize(L"videoüberwachung", 16) == 7);
        CHECK(syllabize(L"videoaufzeichnung", 17) == 6);
        CHECK(syllabize(L"misstrauisch", 12) == 3);
        CHECK(syllabize(L"freund", 5) == 1);
        CHECK(syllabize(L"voraus", 5) == 2);
        CHECK(syllabize(L"vorschau", 8) == 2);
        CHECK(syllabize(L"vorschauen", 10) == 3);
        CHECK(syllabize(L"vorschauin", 10) == 3);
        CHECK(syllabize(L"voyeur", 6) == 2);
        CHECK(syllabize(L"vertrauen", 9) == 3);
        CHECK(syllabize(L"vertreuern", 10) == 3);
        CHECK(syllabize(L"eurer", 5) == 2);
        CHECK(syllabize(L"Bombe", 5) == 2);
        CHECK(syllabize(L"Genie", 5) == 2);
        CHECK(syllabize(L"Papier", 6) == 2);
        CHECK(syllabize(L"Pizza", 5) == 2);
        CHECK(syllabize(L"Nation", 6) == 3);
        CHECK(syllabize(L"priorität", 9) == 4);
        CHECK(syllabize(L"million", 7) == 3);
        CHECK(syllabize(L"biologie", 8) == 4);
        CHECK(syllabize(L"radio", 5) == 3);
        CHECK(syllabize(L"bordeaux", 8) == 2);
        CHECK(syllabize(L"theater", 7) == 3);
        CHECK(syllabize(L"beobachten", 10) == 4);
        CHECK(syllabize(L"dauer", 5) == 2);
        CHECK(syllabize(L"feuern", 6) == 2);
        CHECK(syllabize(L"freien", 6) == 2);
        CHECK(syllabize(L"wenngleich", 10) == 2);
        CHECK(syllabize(L"fluzeug", 7) == 2);
        CHECK(syllabize(L"brief", 5) == 1);
        CHECK(syllabize(L"tausend", 7) == 2);
        CHECK(syllabize(L"vielle", 6) == 2);
        CHECK(syllabize(L"toilette", 8) == 3);
        CHECK(syllabize(L"vielleicht", 10) == 2);
        CHECK(syllabize(L"spiellesen", 10) == 3);
        CHECK(syllabize(L"zeremoniell", 11) == 5);
        CHECK(syllabize(L"zeremonielle", 12) == 6);
        CHECK(syllabize(L"zeremonieller", 13) == 6);
        CHECK(syllabize(L"zeremonielles", 13) == 6);
        CHECK(syllabize(L"zeremoniellen", 13) == 6);
        CHECK(syllabize(L"zeremoniellin", 13) == 6);
        CHECK(syllabize(L"zeremoniellers", 14) == 6);
        CHECK(syllabize(L"zeremoniellerin", 15) == 7);
        CHECK(syllabize(L"zeremoniellerins", 16) == 7);
        //capitalized
        CHECK(syllabize(L"KOEFFICIENT", 11) == 4);
        CHECK(syllabize(L"QUARTIERE", 9) == 3);
        CHECK(syllabize(L"HAUPTQUARTIER", 13) == 3);
        CHECK(syllabize(L"JANUAR", 6) == 3);
        CHECK(syllabize(L"FEBRUAR", 7) == 3);
        CHECK(syllabize(L"FEBRUARY", 8) == 4);
        CHECK(syllabize(L"AUSZUARBEITEN", 13) == 5);
        CHECK(syllabize(L"HILFSQUELLEN", 12) == 3);
        CHECK(syllabize(L"MANUELLEN", 9) == 4);
        CHECK(syllabize(L"AKTUELL", 7) == 3);
        CHECK(syllabize(L"AKTUELLE", 8) == 4);
        CHECK(syllabize(L"KATEGORIALE", 11) == 6);
        CHECK(syllabize(L"KATEGORIEN", 10) == 5);
        CHECK(syllabize(L"FORMALIENS", 10) == 4);
        CHECK(syllabize(L"FERIENJOB", 9) == 3);
        CHECK(syllabize(L"BEET", 4) == 1);
        CHECK(syllabize(L"BEERE", 5) == 2);
        CHECK(syllabize(L"BEERDIGUNG", 10) == 4);
        CHECK(syllabize(L"BEERBEN", 7) == 3);
        CHECK(syllabize(L"BEEILEN", 7) == 3);
        CHECK(syllabize(L"BEEHREN", 7) == 3);
        CHECK(syllabize(L"BEENDEN", 7) == 3);
        CHECK(syllabize(L"AUSGESCHRIEEN", 13) == 4);
        CHECK(syllabize(L"GEARTET", 7) == 3);
        CHECK(syllabize(L"DAUNE", 5) == 2);
        CHECK(syllabize(L"ARMEE", 5) == 2);
        CHECK(syllabize(L"ARMEEABSCHAFF", 13) == 4);
        CHECK(syllabize(L"GEIST", 5) == 1);
        CHECK(syllabize(L"GEIßEL", 6) == 2);
        CHECK(syllabize(L"POLTERGEIST", 11) == 3);
        CHECK(syllabize(L"GEEIGNET", 8) == 3);
        CHECK(syllabize(L"STAAT", 5) == 1);
        CHECK(syllabize(L"FOTOALBUM", 9) == 4);
        CHECK(syllabize(L"KOALITION", 9) == 5);
        CHECK(syllabize(L"TOAST", 5) == 1);
        CHECK(syllabize(L"FRAUEN", 6) == 2);
        CHECK(syllabize(L"FERNSTEUERN", 11) == 3);
        CHECK(syllabize(L"VIDEOÜBERWACHUNG", 16) == 7);
        CHECK(syllabize(L"VIDEOAUFZEICHNUNG", 17) == 6);
        CHECK(syllabize(L"MISSTRAUISCH", 12) == 3);
        CHECK(syllabize(L"FREUND", 5) == 1);
        CHECK(syllabize(L"VORAUS", 5) == 2);
        CHECK(syllabize(L"VORSCHAU", 8) == 2);
        CHECK(syllabize(L"VORSCHAUEN", 10) == 3);
        CHECK(syllabize(L"VORSCHAUIN", 10) == 3);
        CHECK(syllabize(L"VOYEUR", 6) == 2);
        CHECK(syllabize(L"VERTRAUEN", 9) == 3);
        CHECK(syllabize(L"VERTREUERN", 10) == 3);
        CHECK(syllabize(L"EURER", 5) == 2);
        CHECK(syllabize(L"BOMBE", 5) == 2);
        CHECK(syllabize(L"GENIE", 5) == 2);
        CHECK(syllabize(L"PAPIER", 6) == 2);
        CHECK(syllabize(L"PIZZA", 5) == 2);
        CHECK(syllabize(L"NATION", 6) == 3);
        CHECK(syllabize(L"PRIORITÄT", 9) == 4);
        CHECK(syllabize(L"MILLION", 7) == 3);
        CHECK(syllabize(L"BIOLOGIE", 8) == 4);
        CHECK(syllabize(L"RADIO", 5) == 3);
        CHECK(syllabize(L"BORDEAUX", 8) == 2);
        CHECK(syllabize(L"THEATER", 7) == 3);
        CHECK(syllabize(L"BEOBACHTEN", 10) == 4);
        CHECK(syllabize(L"DAUER", 5) == 2);
        CHECK(syllabize(L"FEUERN", 6) == 2);
        CHECK(syllabize(L"FREIEN", 6) == 2);
        CHECK(syllabize(L"WENNGLEICH", 10) == 2);
        CHECK(syllabize(L"FLUZEUG", 7) == 2);
        CHECK(syllabize(L"BRIEF", 5) == 1);
        CHECK(syllabize(L"TAUSEND", 7) == 2);
        CHECK(syllabize(L"VIELLE", 6) == 2);
        CHECK(syllabize(L"TOILETTE", 8) == 3);
        CHECK(syllabize(L"VIELLEICHT", 10) == 2);
        CHECK(syllabize(L"SPIELLESEN", 10) == 3);
        CHECK(syllabize(L"ZEREMONIELL", 11) == 5);
        CHECK(syllabize(L"ZEREMONIELLE", 12) == 6);
        CHECK(syllabize(L"ZEREMONIELLER", 13) == 6);
        CHECK(syllabize(L"ZEREMONIELLES", 13) == 6);
        CHECK(syllabize(L"ZEREMONIELLEN", 13) == 6);
        CHECK(syllabize(L"ZEREMONIELLIN", 13) == 6);
        CHECK(syllabize(L"ZEREMONIELLERS", 14) == 6);
        CHECK(syllabize(L"ZEREMONIELLERIN", 15) == 7);
        CHECK(syllabize(L"ZEREMONIELLERINS", 16) == 7);
        }
    SECTION("Special Cases")
        {
        german_syllabize syllabize;
        CHECK(syllabize(L"McCoy", 5) == 2);
        }

    SECTION("Numbers")
        {
        syllabize_german_number syllabize;
        CHECK(syllabize(L'0') == 1);
        CHECK(syllabize(L'1') == 1);
        CHECK(syllabize(L'2') == 1);
        CHECK(syllabize(L'3') == 1);
        CHECK(syllabize(L'4') == 1);
        CHECK(syllabize(L'5') == 1);
        CHECK(syllabize(L'6') == 1);
        CHECK(syllabize(L'7') == 2);
        CHECK(syllabize(L'8') == 1);
        CHECK(syllabize(L'9') == 1);
        }
    }

// NOLINTEND
// clang-format on
