///////////////////////////////////////////////////////////////////////////////
// Name:        readability_test.cpp
// Author:      Blake Madden
// Copyright:   (c) 2005-2023 Blake Madden
// Licence:     3-Clause BSD licence
// SPDX-License-Identifier: BSD-3-Clause
///////////////////////////////////////////////////////////////////////////////

#include "readability_test.h"

namespace readability
    {
    bool readability_test::operator<(const readability_project_test& that) const noexcept
        { return m_id < that.get_test().get_id(); }
    }
