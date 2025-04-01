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
#include "../src/projects/project_refresh.h"

// clang-format off
// NOLINTBEGIN

TEST_CASE("Project refresh", "[project]")
    {
    SECTION("Flags")
        {
        ProjectRefresh bp;

        CHECK(bp.IsRefreshRequired() == false);
        CHECK(bp.IsTextSectionRefreshRequired() == false);
        CHECK(bp.IsDocumentReindexingRequired() == false);
        bp.RefreshRequired(ProjectRefresh::Minimal);
        CHECK(bp.IsRefreshRequired());
        CHECK(bp.IsTextSectionRefreshRequired() == false);
        CHECK(bp.IsDocumentReindexingRequired() == false);
        bp.RefreshRequired(ProjectRefresh::FullReindexing);
        CHECK(bp.IsRefreshRequired());
        CHECK(bp.IsTextSectionRefreshRequired() == false);
        CHECK(bp.IsDocumentReindexingRequired());
        bp.RefreshRequired(ProjectRefresh::NoRefresh);
        CHECK(bp.IsRefreshRequired() == false);
        CHECK(bp.IsTextSectionRefreshRequired() == false);
        CHECK(bp.IsDocumentReindexingRequired() == false);
        bp.RefreshRequired(ProjectRefresh::FullReindexing);
        CHECK(bp.IsRefreshRequired());
        CHECK(bp.IsTextSectionRefreshRequired() == false);
        CHECK(bp.IsDocumentReindexingRequired());
        bp.ResetRefreshRequired();
        CHECK(bp.IsRefreshRequired() == false);
        CHECK(bp.IsTextSectionRefreshRequired() == false);
        CHECK(bp.IsDocumentReindexingRequired() == false);
        bp.RefreshRequired(ProjectRefresh::TextSection);
        CHECK(bp.IsRefreshRequired());
        CHECK(bp.IsTextSectionRefreshRequired());
        CHECK(bp.IsDocumentReindexingRequired() == false);
        }
    }
// NOLINTEND
// clang-format on
