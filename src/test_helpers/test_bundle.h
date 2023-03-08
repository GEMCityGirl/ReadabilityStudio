#ifndef __TEST_BUNDLE_H__
#define __TEST_BUNDLE_H__

#include <wx/wx.h>
#include <set>
#include <string>
#include <limits>
#include <bitset>
#include "../readability/readability_test.h"

/// A test (and its optional goals) included in a bundle.
class Goal
    {
public:
    using string_type = traits::case_insensitive_wstring_ex;

    /// Constructor.
    /// @param name The name of the goal (test name or statistic name).
    explicit Goal(const string_type& name) : m_name(name)
        { m_passFails.set(); }
    /// Constructor.
    /// @param name The name of the goal (test name or statistic name).
    /// @param minGoal The minimum value of the goal.
    /// @param maxGoal The maximum value of the goal.
    /// @note Set either the min or max goal to NaN to not use them
    Goal(const string_type& name, const double minGoal, const double maxGoal) :
        m_name(name), m_minGoal(minGoal), m_maxGoal(maxGoal)
        { m_passFails.set(); }
    /// Compares goals by name.
    /// @param that The goal to compare against.
    /// @returns True if this goal's name is less than the other one.
    [[nodiscard]] bool operator<(const Goal& that) const noexcept
        { return GetName() < that.GetName(); }
    /// @returns The test/statistic's name.
    [[nodiscard]] const string_type& GetName() const noexcept
        { return m_name; }
    /// @returns The minimum score that a test/statistic should have.
    /// @note Normally, you would be more interested in setting a max score, rather than this.
    [[nodiscard]] double GetMinGoal() const noexcept
        { return m_minGoal; }
    /// @returns The maximum score that a test/statistic should have.
    [[nodiscard]] double GetMaxGoal() const noexcept
        { return m_maxGoal; }
    /// @returns The flags used to indicate if a test/statistic is within its constraints.
    [[nodiscard]] const std::bitset<32>& GetPassFailFlags() const noexcept
        { return m_passFails; }
    /// @returns The flags used to indicate if a test/statistic is within its constraints.
    [[nodiscard]] std::bitset<32>& GetPassFailFlags() noexcept
        { return m_passFails; }
    /// @returns True if there is at least one goal enabled for this test/statistic.
    [[nodiscard]] bool HasGoals() const noexcept
        { return (!std::isnan(GetMinGoal()) || !std::isnan(GetMaxGoal())); }
private:
    double m_minGoal{ std::numeric_limits<double>::quiet_NaN() };
    double m_maxGoal{ std::numeric_limits<double>::quiet_NaN() };
    std::bitset<32> m_passFails;
    string_type m_name;
    };

using TestGoal = Goal;
using StatGoal = Goal;

/// A collection of tests (and optionally, their recommended goals).
class TestBundle
    {
public:
    using string_type = traits::case_insensitive_wstring_ex;

    explicit TestBundle(const wxString& name) : m_name(name.wc_str()), m_locked(false), m_language(readability::test_language::unknown_language) {}
    explicit TestBundle(const string_type& name) : m_name(name), m_locked(false), m_language(readability::test_language::unknown_language) {}
    TestBundle() : m_locked(false), m_language(readability::test_language::unknown_language) {}

    /// Compares bundles by name.
    /// @param that The bundle to compare against.
    /// @returns True if this bundle's name is less than the other one.
    [[nodiscard]] bool operator<(const TestBundle& that) const noexcept
        { return GetName() < that.GetName(); }
    /// @returns The bundle's name.
    [[nodiscard]] const string_type& GetName() const noexcept
        { return m_name; }
    void SetName(const string_type& name)
        { m_name = name; }
    /// @returns The bundle's description.
    [[nodiscard]] const wxString& GetDescription() const noexcept
        { return m_description; }
    /// @returns The bundle's description.
    [[nodiscard]] wxString& GetDescription() noexcept
        { return m_description; }
    void SetDescription(const wxString& desc)
        { m_description = desc; }
    [[nodiscard]] readability::test_language GetLanguage() const noexcept
        { return m_language; }
    void SetLanguage(const readability::test_language lang) noexcept
        { m_language = lang; }

    [[nodiscard]] std::set<TestGoal>& GetTestGoals() noexcept
        { return m_includedTests; }
    [[nodiscard]] const std::set<TestGoal>& GetTestGoals() const noexcept
        { return m_includedTests; }

    [[nodiscard]] std::set<StatGoal>& GetStatGoals() noexcept
        { return m_includedStatGoals; }
    [[nodiscard]] const std::set<StatGoal>& GetStatGoals() const noexcept
        { return m_includedStatGoals; }

    /// Locks or unlocks the bundle. A locked bundle cannot be edited by the user through the interface.
    /// This is useful for creating system bundles based on published articles that should not be changed.
    void Lock(const bool lock = true) noexcept
        { m_locked = lock; }
    /// @returns Whether the bundle can be edited by the user.
    [[nodiscard]] bool IsLocked() const noexcept
        { return m_locked; }
private:
    string_type m_name;
    wxString m_description;
    std::set<TestGoal> m_includedTests;
    std::set<StatGoal> m_includedStatGoals;
    bool m_locked{ false };
    readability::test_language m_language{ readability::test_language::unknown_language };
    };

#endif // __TEST_BUNDLE_H__
