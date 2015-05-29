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

#include "dvdirection2.hpp"

#include "utils/diagonal_vertical_direction.hpp"

#include "image/image.hpp"

namespace ppocr { namespace strategies {

dvdirection2::value_type dvdirection2::load(const Image& img, const Image& img90) const
{
    auto const area = utils::diagonal_vertical_direction_area(img);
    if (!area) {
        return 50;
    }
    auto const top_bottom = utils::diagonal_vertical_direction(img);
    auto const sum = top_bottom.top + top_bottom.bottom;
    auto const ret = (sum + top_bottom.top - top_bottom.bottom) * 100 / area;
    return ret;
}

dvdirection2::relationship_type dvdirection2::relationship() const
{ return {100}; }

unsigned dvdirection2::best_difference() const
{ return 5; }

} }
