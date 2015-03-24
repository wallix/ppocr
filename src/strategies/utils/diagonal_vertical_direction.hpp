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
*   Product name: redemption, a FLOSS RDP proxy
*   Copyright (C) Wallix 2010-2015
*   Author(s): Jonathan Poelen
*/

#ifndef REDEMPTION_SRC_STRATEGIES_UTILS_DIAGONAL_VERTICAL_DIRECTION_HPP
#define REDEMPTION_SRC_STRATEGIES_UTILS_DIAGONAL_VERTICAL_DIRECTION_HPP

#include "image.hpp"
#include "top_bottom.hpp"

#include "sassert.hpp"

namespace strategies { namespace utils {

namespace details_ {
    inline unsigned count_diagonal_vertical_direction(
        Bounds const & bnd, Pixel const * p, Pixel const * ep, bool is_top
    ) {
        unsigned d = 0;
        size_t ih = 0;
        size_t const wdiv2 = bnd.w()/2;
        for (; p != ep; p += bnd.w(), ++ih) {
            size_t x = wdiv2 - bnd.w() / (!is_top ? bnd.h() - ih : 1 + ih) / 2;
            auto leftp = p + x;
            auto rightp = p + bnd.w() - x;
            for (; leftp != rightp; ++leftp) {
                if (is_pix_letter(*leftp)) {
                    ++d;
                }
            }
        }
        return d;
    }

    inline unsigned count_diagonal_vertical_area(
        Bounds const & bnd, Pixel const * p, Pixel const * ep, bool is_top
    ) {
        unsigned d = 0;
        size_t ih = 0;
        size_t const wdiv2 = bnd.w()/2;
        for (; p != ep; p += bnd.w(), ++ih) {
            size_t x = wdiv2 - bnd.w() / (!is_top ? bnd.h() - ih : 1 + ih) / 2;
            d += bnd.w() - x*2;
        }
        return d;
    }
}

inline unsigned diagonal_vertical_direction_area(const Image& img)
{
    Bounds const bnd(img.width(), img.height() / 2);
    auto p = img.data();
    auto ep = img.data({0, bnd.h()});

    unsigned area = 0;
    size_t ih = 0;
    size_t const wdiv2 = bnd.w()/2;
    for (; p != ep; p += bnd.w(), ++ih) {
        size_t x = wdiv2 - bnd.w() / (1 + ih) / 2;
        area += bnd.w() - x*2;
    }
    return area * 2;
}

inline
TopBottom diagonal_vertical_direction(const Image& img)
{
    Bounds const bnd(img.width(), img.height() / 2);
    auto p = img.data();
    auto ep = img.data({0, bnd.h()});
    auto const top = details_::count_diagonal_vertical_direction(bnd, p, ep, true);

    p = ep;
    if (img.height() & 1) {
        p += img.width();
    }
    auto const bottom = details_::count_diagonal_vertical_direction(bnd, p, img.data_end(), false);

    return {top, bottom};
}

} }

#endif
