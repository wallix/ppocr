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

#ifndef PPOCR_STRATEGIES_UTILS_HORIZONTAL_ZONE_HPP
#define PPOCR_STRATEGIES_UTILS_HORIZONTAL_ZONE_HPP

#include "image/image.hpp"
#include "top_bottom.hpp"

namespace ppocr { namespace strategies { namespace utils {

inline unsigned horizontal_box_zone_0_1_2(const Image& img, unsigned d)
{
    if (d > img.height() / 4) {
        if (d > img.height()) {
            return 2;
        }
        return 1;
    }
    return 0;
}

inline unsigned horizontal_zone_1_2_3_4_5(const Image& img, TopBottom const & top_bottom)
{
    return (top_bottom.top > top_bottom.bottom)
        ? 3 + utils::horizontal_box_zone_0_1_2(img, top_bottom.top - top_bottom.bottom)
        : (top_bottom.top < top_bottom.bottom)
        ? 3 - utils::horizontal_box_zone_0_1_2(img, top_bottom.bottom - top_bottom.top)
        : 3;
}

} } }

#endif
