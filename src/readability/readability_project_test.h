/** @addtogroup Readability
    @brief Classes for readability tests.
    @date 2005-2023
    @copyright Oleander Software, Ltd.
    @author Blake Madden
    @details This program is free software; you can redistribute it and/or modify
     it under the terms of the 3-Clause BSD License.

     SPDX-License-Identifier: BSD-3-Clause
* @{*/

#ifndef __READABILITY_PROJECT_TEST_H__
#define __READABILITY_PROJECT_TEST_H__

#include <vector>
#include <bitset>
#include <utility>
#include "readability_test.h"
#include "../Wisteria-Dataviz/src/data/dataset.h"

namespace readability
    {
    /// @brief Wrapper around a readability_test, which adds features needed for projects,
    ///     such as score data and an inclusion flag.
    /// @details This also provides a thin wrapper around readability_test objects,
    ///     so that numerous copies of a test can point to a constant readability_test;
    ///     this way the same strings inside of readability_test objects don't get
    ///     copied thousand of times.
    class readability_project_test
        {
    public:
        /// @private
        readability_project_test(const readability_project_test& that) noexcept :
            m_test(that.m_test), m_included(that.m_included)
            {}
        /// @brief CTOR from regular test definition.
        explicit readability_project_test(const readability_test& test) noexcept :
            m_test(&test)
            {}
        /// @private
        readability_project_test() = delete;
        /// @private
        virtual ~readability_project_test() {}
        /// @private
        void operator=(const readability_project_test& that) noexcept
            {
            m_test = that.m_test;
            m_included = that.m_included;
            }
        /// @private
        [[nodiscard]]
        bool operator<(const readability_test& that) const noexcept
            { return (get_test() < that); }
        /// @private
        [[nodiscard]]
        bool operator<(const readability_project_test& that) const noexcept
            { return (get_test() < that.get_test()); }
        /// @brief Compares (case insensitively) a string value to the test's ID,
        ///     short name, and long name.
        [[nodiscard]]
        bool operator==(const readability_test& that) const noexcept
            { return (get_test() == that); }
        /// @private
        [[nodiscard]]
        bool operator==(const readability_project_test& that) const noexcept
            { return (get_test() == that.get_test()); }
        /// @returns The underlying test description.
        [[nodiscard]]
        const readability_test& get_test() const noexcept
            {
            assert(m_test && "test reference not initialized in project test.");
            return *m_test;
            }
        /// @returns grade score points (for multi-document tests).
        [[nodiscard]]
        std::shared_ptr<Wisteria::Data::Dataset> get_grade_point_collection()
            { return m_grade_values; }
        /// @returns index score points (for multi-document tests).
        [[nodiscard]]
        std::shared_ptr<Wisteria::Data::Dataset> get_index_point_collection()
            { return m_index_values; }
        /// @returns cloze score points (for multi-document tests).
        [[nodiscard]]
        std::shared_ptr<Wisteria::Data::Dataset> get_cloze_point_collection()
            { return m_cloze_scores; }
        /// @returns Whether the test is included in the project.
        [[nodiscard]]
        bool is_included() const noexcept
            { return m_included; }
        /// Whether to include the test in the project.
        /// @param inc True to include the test, false to remove.
        void include(const bool inc) noexcept
            { m_included = inc; }
        /// @brief Gives direct access to the inclusion flag.
        /// @details Used when an external source needs to edit the
        ///     inclusion status, but can't call include().
        [[nodiscard]]
        bool& get_include_flag() noexcept
            { return m_included; }
    private:
        const readability_test* m_test{ nullptr };
        // whether the test is included in the project
        bool m_included{ false };
        // batch projects use these for its multi-doc graphs
        std::shared_ptr<Wisteria::Data::Dataset> m_grade_values{ std::make_shared<Wisteria::Data::Dataset>() };
        std::shared_ptr<Wisteria::Data::Dataset> m_index_values{ std::make_shared<Wisteria::Data::Dataset>() };
        std::shared_ptr<Wisteria::Data::Dataset> m_cloze_scores{ std::make_shared<Wisteria::Data::Dataset>() };
        };
    }

/** @} */

#endif // __READABILITY_PROJECT_TEST_H__
