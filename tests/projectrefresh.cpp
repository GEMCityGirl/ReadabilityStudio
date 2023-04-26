#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>
#include "../src/projects/project_refresh.h"

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
