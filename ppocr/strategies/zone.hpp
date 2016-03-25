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

#ifndef PPOCR_SRC_STRATEGIES_ZONE_HPP
#define PPOCR_SRC_STRATEGIES_ZONE_HPP

#include <array>

#include "relationship/array_compare.hpp"

namespace ppocr {

class Image;

namespace strategies {

struct zone {
    enum {
        top_left_is_letter,
        bottom_right_is_letter,
        number_top_alternations,
        number_right_alternations,
        number_bottom_alternations,
        number_left_alternations,
        number_internal_alternations,
        number_index
    };

    using relationship_type = array_compare_relationship<unsigned, number_index>;
    using value_type = relationship_type::value_type;

    static constexpr bool one_axis = false;

    value_type load(const Image & img, const Image & /*img90*/) const;

    constexpr relationship_type relationship() const { return {}; }
    constexpr unsigned best_difference() const { return 20; }
};

} }

#endif
