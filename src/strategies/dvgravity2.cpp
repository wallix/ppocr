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

#include "dvgravity2.hpp"

#include "utils/diagonal_vertical_gravity.hpp"

#include "image/image.hpp"

namespace ppocr { namespace strategies {

static unsigned compute_dvgravity2(const Image& img)
{
    auto const area = utils::diagonal_vertical_gravity_area(img);
    if (!area) {
        return 50;
    }
    auto const top_bottom = utils::diagonal_vertical_gravity(img);
    auto const sum = top_bottom.top + top_bottom.bottom;
    auto const ret = (sum + top_bottom.top - top_bottom.bottom) * 100 / area;
    return ret;
}

unsigned dvgravity2_traits::compute(const Image& img, const Image&)
{ return compute_dvgravity2(img); }

unsigned dvgravity290_traits::compute(const Image&, const Image& img90)
{ return compute_dvgravity2(img90); }

unsigned int dvgravity2_traits::get_interval() { return 100; }
unsigned int dvgravity290_traits::get_interval() { return 100; }

unsigned int dvgravity2_traits::best_difference() { return 5; }
unsigned int dvgravity290_traits::best_difference() { return 5; }

} }
