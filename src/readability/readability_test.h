/** @addtogroup Readability
    @brief Classes for readability tests.
    @date 2004-2020
    @copyright Oleander Software, Ltd.
    @author Blake Madden
    @details This program is free software; you can redistribute it and/or modify
     it under the terms of the 3-Clause BSD License.

     SPDX-License-Identifier: BSD-3-Clause
* @{*/

#ifndef __READABILITY_TEST_H__
#define __READABILITY_TEST_H__

#include <vector>
#include <bitset>
#include <utility>
#include "../indexing/character_traits.h"
#include "../../../SRC/Wisteria-Dataviz/src/util/stringutil.h"
#include "../../../SRC/Wisteria-Dataviz/src/data/dataset.h"

namespace readability
    {
    /// Test result type.
    enum class readability_test_type
        {
        grade_level,
        index_value,
        index_value_and_grade_level,
        predicted_cloze_score,
        grade_level_and_predicted_cloze_score,
        TEST_TYPE_COUNT
        };

    /// Test language.
    enum class test_language
        {
        english_test,
        spanish_test,
        german_test,
        french_test,
        russian_test,
        unknown_language,
        TEST_LANGUAGE_COUNT
        };

    /// Factors used in a test's regression equation.
    enum class test_factor
        {
        word_complexity,
        word_complexity_2_plus_syllables,
        word_complexity_3_plus_syllables,
        word_complexity_density,
        word_length,
        word_length_3_less,
        word_length_6_plus,
        word_length_7_plus,
        word_familiarity_spache,
        word_familiarity_dale_chall,
        word_familiarity_harris_jacobson,
        sentence_length,
        TEST_FACTOR_COUNT
        };

    /// The type of student materials that a readability test is meant for.
    enum class test_teaching_level
        {
        primary_grade,
        secondary_grade,
        adult_level,
        second_language,
        TEST_TEACHING_LEVEL_COUNT
        };

    /// The type of documents that a readability test is meant for.
    enum class document_classification
        {
        general_document,
        technical_document,
        nonnarrative_document,
        adult_literature_document,
        childrens_literature_document,
        DOCUMENT_CLASSIFICATION_COUNT
        };

    /// The type of industry that a readability test is meant for.
    enum class industry_classification
        {
        childrens_publishing_industry,
        adult_publishing_industry,
        childrens_healthcare_industry,
        adult_healthcare_industry,
        military_government_industry,
        sedondary_language_industry,
        broadcasting_industry,
        INDUSTRY_CLASSIFICATION_COUNT
        };

    class base_test
        {
    public:
        base_test() noexcept : m_interfaceId(0) {}
        [[nodiscard]] int get_interface_id() const noexcept
            { return m_interfaceId; }
        void set_interface_id(const int id) noexcept
            { m_interfaceId = id; }
        [[nodiscard]] bool operator==(const int value) const noexcept
            { return (m_interfaceId == value); }
    protected:
        int m_interfaceId{ 0 }; // ID used for menus or other interfaces
        };

    class test_with_classification
        {
    public:
        test_with_classification() noexcept :
            m_industry_classification(0),
            m_document_classificaiton(0),
            m_language(0),
            m_teaching_level(0)
            {}
        test_with_classification(//industry
                                  const bool industryChildrensPublishingSelected,
                                  const bool industryAdultPublishingSelected,
                                  const bool industrySecondaryLanguageSelected,
                                  const bool industryChildrensHealthCareSelected,
                                  const bool industryAdultHealthCareSelected,
                                  const bool industryMilitaryGovernmentSelected,
                                  const bool industryBroadcastingSelected,
                                  //document
                                  const bool documentGeneralSelected,
                                  const bool documentTechSelected,
                                  const bool documentNonNarrativeFormSelected,
                                  const bool documentYoungAdultSelected,
                                  const bool documentChildrenSelected) :
            m_industry_classification(0),
            m_document_classificaiton(0),
            m_language(0),
            m_teaching_level(0)
            {
            add_industry_classification(industry_classification::childrens_publishing_industry, industryChildrensPublishingSelected);
            add_industry_classification(industry_classification::adult_publishing_industry, industryAdultPublishingSelected);
            add_industry_classification(industry_classification::childrens_healthcare_industry, industryChildrensHealthCareSelected);
            add_industry_classification(industry_classification::adult_healthcare_industry, industryAdultHealthCareSelected);
            add_industry_classification(industry_classification::military_government_industry, industryMilitaryGovernmentSelected);
            add_industry_classification(industry_classification::sedondary_language_industry, industrySecondaryLanguageSelected);
            add_industry_classification(industry_classification::broadcasting_industry, industryBroadcastingSelected);

            add_document_classification(document_classification::general_document, documentGeneralSelected);
            add_document_classification(document_classification::technical_document, documentTechSelected);
            add_document_classification(document_classification::nonnarrative_document, documentNonNarrativeFormSelected);
            add_document_classification(document_classification::adult_literature_document, documentYoungAdultSelected);
            add_document_classification(document_classification::childrens_literature_document, documentChildrenSelected);
            }
        test_with_classification(const test_with_classification& that) noexcept
            { copy_classifications(that); }
        void operator=(const test_with_classification& that) noexcept
            { copy_classifications(that); }
        /// Manual assignment. Derived classed are advised to directly call this in its own assignment operations.
        void copy_classifications(const test_with_classification& that) noexcept
            {
            //industry
            m_industry_classification = that.m_industry_classification;
            //document
            m_document_classificaiton = that.m_document_classificaiton;
            //language
            m_language = that.m_language;
            //factors
            m_test_factors = that.m_test_factors;
            //teaching level
            m_teaching_level = that.m_teaching_level;
            }
        //industry association
        void add_industry_classification(const industry_classification industry, const bool set_flag)
            { m_industry_classification.set(static_cast<size_t>(industry), set_flag); }
        [[nodiscard]] bool has_industry_classification(const industry_classification industry) const
            { return m_industry_classification.test(static_cast<size_t>(industry)); }
        //document type association
        void add_document_classification(const document_classification doc, const bool set_flag)
            { m_document_classificaiton.set(static_cast<size_t>(doc), set_flag); }
        [[nodiscard]] bool has_document_classification(const document_classification doc) const
            { return m_document_classificaiton.test(static_cast<size_t>(doc)); }

        /// Adds a language that this test should be used on.
        void add_language(const test_language lang)
            { m_language.set(static_cast<size_t>(lang), true); }
        [[nodiscard]] bool has_language(const test_language lang) const
            { return m_language.test(static_cast<size_t>(lang)); }
        void reset_languages() noexcept
            { m_language.reset(); }

        /// Adds a factor used in the test's equation.
        void add_factor(const test_factor factor)
            { m_test_factors.set(static_cast<size_t>(factor), true); }
        [[nodiscard]] bool has_factor(const test_factor factor) const
            { return m_test_factors.test(static_cast<size_t>(factor)); }
        void reset_factors() noexcept
            { m_test_factors.reset(); }

        ///Teaching level for test
        void add_teaching_level(const test_teaching_level level)
            { m_teaching_level.set(static_cast<size_t>(level), true); }
        [[nodiscard]] bool has_teaching_level(const test_teaching_level level) const
            { return m_teaching_level.test(static_cast<size_t>(level)); }        
        void reset_teaching_levels() noexcept
            { m_teaching_level.reset(); }
    protected:
        //industry
        std::bitset<static_cast<size_t>(industry_classification::INDUSTRY_CLASSIFICATION_COUNT)> m_industry_classification;
        //document
        std::bitset<static_cast<size_t>(document_classification::DOCUMENT_CLASSIFICATION_COUNT)> m_document_classificaiton;
        //language
        std::bitset<static_cast<size_t>(test_language::TEST_LANGUAGE_COUNT)> m_language;
        //factors
        std::bitset<static_cast<size_t>(test_factor::TEST_FACTOR_COUNT)> m_test_factors;
        //teaching level
        std::bitset<static_cast<size_t>(test_teaching_level::TEST_TEACHING_LEVEL_COUNT)> m_teaching_level;
        };

    class readability_project_test;

    /// Readability test description, which includes its various names, IDs, and description.
    class readability_test : public test_with_classification, public base_test
        {
    public:
        using string_type = traits::case_insensitive_wstring_ex;
        readability_test() : m_is_integral(false), m_readability_test_type(readability_test_type::grade_level)
            {}  
        /// CTORs simply used for quick binary searches.
        explicit readability_test(const wchar_t* id) : m_id(id), m_short_name(id), m_long_name(id), m_description(id),
            m_is_integral(false), m_readability_test_type(readability_test_type::grade_level)
            {}
        explicit readability_test(const int id) : m_is_integral(false), m_readability_test_type(readability_test_type::grade_level)
            { m_interfaceId = id; }
        /** Constructor meant for normal use.
            @param id The (string-based) ID of the test. This is used for internal identification of the test across the
                      program, resources, and documentation.
            @param interface_id The ID used for the menus.
            @param short_name A user-facing short name for the test.
            @param long_name A user-facing long name for the test.
            @param description A user-facing description for the test.
            @param test_type The type of test that this is.
            @param is_integral Whether the test's results is truncated to an integer value (i.e., no floating-point precision).
            @param formula If a linear-regression-based test, the formula of the test.
                           This can be an expression used as the basis for a custom test; refer to the readability test formula parser's
                           commands to build a usable expression. (Note that this expression should be in US format.)
                           Also note that if this test uses something like a table or graph, then this does not apply and should be an empty string.*/
        readability_test(const wchar_t* id,
                         const int interface_id,
                         const wchar_t* short_name,
                         const wchar_t* long_name,
                         const wchar_t* description,
                         const readability_test_type test_type,
                         const bool is_integral,
                         const wchar_t* formula) :
            m_id(id),
            m_short_name(short_name), m_long_name(long_name),
            m_description(description),
            m_formula(formula),
            m_readability_test_type(test_type),
            m_is_integral(is_integral)
            { m_interfaceId = interface_id; }
        /// Copy CTOR.
        readability_test(const readability_test& that) :
            m_id(that.m_id),
            m_short_name(that.m_short_name),
            m_long_name(that.m_long_name),
            m_description(that.m_description),
            m_formula(that.m_formula),
            m_readability_test_type(that.m_readability_test_type),
            m_is_integral(that.m_is_integral)
            {
            m_interfaceId = that.m_interfaceId;
            copy_classifications(that);
            }
        void operator=(const readability_test& that)
            {
            copy_classifications(that);
            m_id = that.m_id;
            m_interfaceId = that.m_interfaceId;
            m_short_name = that.m_short_name;
            m_long_name = that.m_long_name;
            m_description = that.m_description;
            m_formula = that.m_formula;
            m_readability_test_type = that.m_readability_test_type;
            m_is_integral = that.m_is_integral;
            }
        [[nodiscard]] bool operator<(const readability_test& that) const noexcept
            { return m_id < that.m_id; }
        [[nodiscard]] bool operator<(const readability_project_test& that) const noexcept;
        /// Compares (case insensitive) a string value to the test's ID, short name, or long name.
        /// If any of those match, then returns true.
        [[nodiscard]] bool operator==(const readability_test& that) const noexcept
            {
            return ((m_id == that.m_id) ||
                (m_interfaceId == that.m_interfaceId) ||
                (m_short_name == that.m_short_name) ||
                (m_long_name == that.m_long_name) );
            }
        /// @returns Whether test results are truncated to integers.
        [[nodiscard]] bool is_integral() const noexcept
            { return m_is_integral; }
        ///value functions
        [[nodiscard]] const string_type& get_id() const noexcept
            { return m_id; }
        [[nodiscard]] const string_type& get_short_name() const noexcept
            { return m_short_name; }
        [[nodiscard]] const string_type& get_long_name() const noexcept
            { return m_long_name; }
        [[nodiscard]] const string_type& get_description() const noexcept
            { return m_description; }
        [[nodiscard]] const string_type& get_formula() const noexcept
            { return m_formula; }
        [[nodiscard]] readability_test_type get_test_type() const noexcept
            { return m_readability_test_type; }
    private:
        string_type m_id; // ID used in the project file
        string_type m_short_name;
        string_type m_long_name;
        string_type m_description;
        string_type m_formula;
        bool m_is_integral{ false }; // whether test scores do NOT use floating-point precision (e.g., index tests)
        readability_test_type m_readability_test_type{ readability_test_type::grade_level };
        };

    /// Wrapper around a readability_test, which adds features needed for projects, such as score data and an inclusion flag.
    /// This also provides a thin wrapper around readability_test objects, so that numerous copies of a test
    /// can point to a constant readability_test--this way the same strings inside of readability_test objects don't get
    /// copied thousand of times.
    class readability_project_test
        {
        readability_project_test() = delete;
    public:
        /// Copy CTOR
        readability_project_test(const readability_project_test& that) noexcept : m_test(that.m_test), m_included(that.m_included)
            {}
        /// CTOR from regular test definition.
        explicit readability_project_test(const readability_test& test) noexcept : m_test(&test)
            {}
        /// DTOR
        virtual ~readability_project_test() {}
        /// Assignment from another project test.
        void operator=(const readability_project_test& that) noexcept
            {
            m_test = that.m_test;
            m_included = that.m_included;
            }
        [[nodiscard]] bool operator<(const readability_test& that) const noexcept
            { return (get_test() < that); }
        [[nodiscard]] bool operator<(const readability_project_test& that) const noexcept
            { return (get_test() < that.get_test()); }
        /// Compares (case insensitively) a string value to the test's ID, short name, and long name.
        [[nodiscard]] bool operator==(const readability_test& that) const noexcept
            { return (get_test() == that); }
        [[nodiscard]] bool operator==(const readability_project_test& that) const noexcept
            { return (get_test() == that.get_test()); }
        /// @returns The underlying test description.
        [[nodiscard]] const readability_test& get_test() const noexcept
            {
            assert(m_test && "test reference not initialized in project test.");
            return *m_test;
            }
        /// @returns grade score points (for multi-document tests).
        [[nodiscard]] std::shared_ptr<Wisteria::Data::Dataset> get_grade_point_collection()
            { return m_grade_values; }
        /// @returns index score points (for multi-document tests).
        [[nodiscard]] std::shared_ptr<Wisteria::Data::Dataset> get_index_point_collection()
            { return m_index_values; }
        /// @returns cloze score points (for multi-document tests).
        [[nodiscard]] std::shared_ptr<Wisteria::Data::Dataset> get_cloze_point_collection()
            { return m_cloze_scores; }
        /// @returns Whether the test is included in the project.
        [[nodiscard]] bool is_included() const noexcept
            { return m_included; }
        /// Whether to include the test in the project.
        /// @param inc True to include the test, false to remove.
        void include(const bool inc) noexcept
            { m_included = inc; }
        /// Gives direct access to the inclusion flag.
        /// Used when an external source needs to edit the inclusion status, but can't call include().
        [[nodiscard]] bool& get_include_flag() noexcept
            { return m_included; }
    private:
        const readability_test* m_test{ nullptr };
        bool m_included{ false }; //whether the test is included in the project
        // batch projects use these for its multi-doc graphs
        std::shared_ptr<Wisteria::Data::Dataset> m_grade_values{ std::make_shared<Wisteria::Data::Dataset>() };
        std::shared_ptr<Wisteria::Data::Dataset> m_index_values{ std::make_shared<Wisteria::Data::Dataset>() };
        std::shared_ptr<Wisteria::Data::Dataset> m_cloze_scores{ std::make_shared<Wisteria::Data::Dataset>() };
        };

    /// Class to hold all of the tests for a project.
    template<typename test_typeT = readability_project_test>
    class readability_test_collection
        {
    public:
        using string_type = traits::case_insensitive_wstring_ex;
        /// Adds a test to the collection. Will only add it if not already in there.
        /// @param test The test to add.
        template<typename T>
        void add_test(const T& test)
            {
            if (has_test(test))
                { return; }
            auto pos = std::lower_bound(m_tests.begin(), m_tests.end(), test);
            m_tests.insert(pos, test_typeT(test));
            }
        /// Adds a collection of readability_test objects all at once. Duplicates will be removed in here, and
        /// the collection passed in here does not need to be sorted.
        /// @param tests The collection of tests to add.
        template<typename T>
        void add_tests(const T& tests)
            {
            for (typename T::const_iterator testPos = tests.begin();
                testPos != tests.end();
                ++testPos)
                { m_tests.push_back(test_typeT(*testPos)); }
            // sort and remove any duplicates
            std::sort(m_tests.begin(), m_tests.end());
            typename std::vector<test_typeT>::iterator endOfUniquePos = 
                std::unique(m_tests.begin(), m_tests.end());
            if (endOfUniquePos != m_tests.end())
                { m_tests.erase(endOfUniquePos, m_tests.end()); }
            }
        /// @returns The number of tests in the collection.
        [[nodiscard]] size_t get_test_count() const noexcept
            { return m_tests.size(); }
        /// Removes all tests.
        void clear() noexcept
            { m_tests.clear(); }
        /// Reserves space before a bunch of calls to add_test.
        /// @param size The amount of space requested to reserve for x number of tests.
        void reserve(const size_t size)
            { m_tests.reserve(size); }
        /// @returns a test, based on specified criterion. The test will be an iterator to the test and a bool flag indicating that it was found.
        template<typename T>
        [[nodiscard]] std::pair<typename std::vector<test_typeT>::const_iterator, bool> get_test(const T& test) const
            { return find_test(test); }
        /// @returns Whether a test exists. Test may or may not actually be enabled (included) in the project.
        template<typename T>
        [[nodiscard]] bool has_test(const T& test) const
            { return find_test(test).second; }
        /// @returns Whether a test is included in the project.
        template<typename T>
        [[nodiscard]] bool is_test_included(const T& test) const
            {
            auto [iterator, found] = find_test(test);
            if (!found)
                { return false; }
            return iterator->is_included();
            }
        /// Includes or excludes a @c test from the project.
        /// @returns False if the requested test wasn't found.
        template<typename T>
        bool include_test(const T& test, const bool include)
            {
            auto [iterator, found] = find_test(test);
            if (!found)
                { return false; }
            iterator->include(include);
            return true;
            }
        /// @returns The ID for a given @c test.
        template<typename T>
        [[nodiscard]] const string_type get_test_id(const T& test) const
            {
            auto [iterator, found] = find_test(test);
            if (!found)
                { return string_type(L""); }
            else
                { return iterator->get_test().get_id(); }
            }
        /// @returns The short name for a given test.
        template<typename T>
        [[nodiscard]] const string_type get_test_short_name(const T& test) const
            {
            auto [iterator, found] = find_test(test);
            if (!found)
                { return string_type(L""); }
            else
                { return iterator->get_test().get_short_name(); }
            }
        /// @returns The long name for a given test.
        template<typename T>
        [[nodiscard]] const string_type get_test_long_name(const T& test) const
            {
            auto [iterator, found] = find_test(test);
            if (!found)
                { return string_type(L""); }
            else
                { return iterator->get_test().get_long_name(); }
            }
        /// @returns The description for a given test.
        template<typename T>
        [[nodiscard]] const string_type get_test_description(const T& test) const
            {
            auto [iterator, found] = find_test(test);
            if (!found)
                { return string_type(L""); }
            else
                { return iterator->get_test().get_description(); }
            }
        /// Searches for a test.
        /// @param test The test to find (this key can be an ID (the integer or string one), short name, or long name).
        /// @returns A pair: the iterator to the test and a boolean indicating whether the test was found or not.
        template<typename T>
        [[nodiscard]] std::pair<typename std::vector<test_typeT>::iterator, bool> find_test(const T& test)
            {
            // first, do a binary search (by ID) as this is a fast and most common way to find a test
            typename std::vector<test_typeT>::iterator pos = std::lower_bound(m_tests.begin(), m_tests.end(), readability_test(test));
            if (pos != m_tests.end() && *pos == readability_test(test))
                { return std::pair<typename std::vector<test_typeT>::iterator, bool>(pos, true); }
            // if that fails, then search items one-by-one (in case caller is searching by long name or short name)
            pos = std::find(m_tests.begin(), m_tests.end(), readability_test(test));
            return std::pair<typename std::vector<test_typeT>::iterator, bool>(pos, (pos != m_tests.end())/*whether it was found or not*/);
            }
        /// Searches for a test.
        /// @param test The test to find (this key can be an ID (the integer or string one), short name, or long name).
        /// @returns A pair: the iterator to the test and a boolean indicating whether the test was found or not.
        template<typename T>
        [[nodiscard]] std::pair<typename std::vector<test_typeT>::const_iterator, bool> find_test(const T& test) const
            {
            // first, do a binary search (by ID) as this is a fast and most common way to find a test
            typename std::vector<test_typeT>::const_iterator pos = std::lower_bound(m_tests.cbegin(), m_tests.cend(), readability_test(test));
            if (pos != m_tests.cend() && *pos == readability_test(test))
                { return std::pair<typename std::vector<test_typeT>::const_iterator, bool>(pos, true); }
            // if that fails, then search items one-by-one (in case caller is searching by long name or short name)
            pos = std::find(m_tests.cbegin(), m_tests.cend(), readability_test(test));
            return std::pair<typename std::vector<test_typeT>::const_iterator, bool>(pos, (pos != m_tests.cend())/*whether it was found or not*/);
            }
        [[nodiscard]] std::vector<test_typeT>& get_tests() noexcept
            { return m_tests; }
        [[nodiscard]] const std::vector<test_typeT>& get_tests() const noexcept
            { return m_tests; }
        /// @returns The number of grade-level tests (or grade-level with index value tests).
        [[nodiscard]] size_t get_grade_level_test_count() const
            {
            return std::count_if(m_tests.cbegin(), m_tests.cend(),
                [](const auto& test)
                { return (test.get_test().get_test_type() == readability_test_type::grade_level ||
                          test.get_test().get_test_type() == readability_test_type::index_value_and_grade_level); });
            }
    private:
        // Although this is a vector, the container keeps this sorted via the add_test* functions.
        // This is a vector so that can use the more liberal == operators for the tests, rather than the < operator.
        std::vector<test_typeT> m_tests;
        };
    }

#endif //__READABILITY_TEST_H__
