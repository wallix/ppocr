/*
*   This program is free software; you can redistribute it and/or modify
*   it under the terms of the GNU General Public License as published by
*   the Free Software Foundation; either version 2 of the License, or
*   (at your option) any later version.
*
*   This program is distributed in the hope that it will be useful,
*   but WITHOUT ANY WARRANTY; without even the implied warranty of
*   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*   GNU General Public License for more details.
*
*   You should have received a copy of the GNU General Public License
*   along with this program; if not, write to the Free Software
*   Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
*
*   Copyright (C) Wallix 2010-2015
*   Author(s): Jonathan Poelen
*/

#ifndef PPOCR_SRC_STRATEGIES_RELATIONSHIP_ARRAY_COMPARE_RELATIONSHIP_HPP
#define PPOCR_SRC_STRATEGIES_RELATIONSHIP_ARRAY_COMPARE_RELATIONSHIP_HPP

#include <array>

namespace ppocr { namespace strategies {

template<class T, std::size_t N, class R = unsigned>
struct array_compare_relationship
{
    using value_type = std::array<T, N>;
    using result_type = R;

    constexpr array_compare_relationship() noexcept {}

    result_type operator()(value_type const & a, value_type const & b) const
    {
        R ret{};
        auto it = std::begin(a);
        for (auto && i : b) {
            if (*it == i) {
                ++ret;
            }
        }
        return R(ret * R{100} / a.size());
    }
};

} }

#endif
