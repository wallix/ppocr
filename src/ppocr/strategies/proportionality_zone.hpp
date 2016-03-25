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
*   Copyright (C) Wallix 2015
*   Author(s): Jonathan Poelen
*/

#ifndef PPOCR_SRC_STRATEGIES_PROPORTIONALITY_ZONE_HPP
#define PPOCR_SRC_STRATEGIES_PROPORTIONALITY_ZONE_HPP

#include <vector>

namespace ppocr {

class Image;

namespace strategies {

struct proportionality_zone {
    struct relationship_type {
        using value_type = std::vector<unsigned>;
        using result_type = unsigned;

        constexpr relationship_type() noexcept {}

        result_type operator()(value_type const & a, value_type const & b) const;

        /// \return [0, 1]
        double dist(value_type const & a, value_type const & b) const;

        bool in_dist(value_type const & a, value_type const & b, unsigned d) const;

        std::size_t count() const;
    };
    using value_type = relationship_type::value_type;

    value_type load(Image const & img, Image const & /*img90*/) const;

    static constexpr bool one_axis() { return false; }
    constexpr relationship_type relationship() const { return {}; }
    constexpr unsigned best_difference() const { return 20u; }
};

} }

#endif
