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

#ifndef REDEMPTION_MAKE_BOX_CHARACTER_CPP
#define REDEMPTION_MAKE_BOX_CHARACTER_CPP

#include "box.hpp"
#include "image.hpp"

namespace utils {
    bool horizontal_empty(Pixel const * d, size_t w) {
        for (auto e = d+w; d != e; ++d) {
            if (is_pix_letter(*d)) {
                return false;
            }
        }
        return true;
    }

    bool vertical_empty(Pixel const * d, Bounds const & bnd) {
        for (auto e = d + bnd.area(); d != e; d += bnd.w()) {
            if (is_pix_letter(*d)) {
                return false;
            }
        }
        return true;
    }
}


Box make_box_character(Image const & image, Index const & idx, Bounds const & bnd)
{
    size_t x = idx.x();

    auto d = image.data({x, idx.y()});
    for (; x < bnd.w(); ++x) {
        if (!utils::vertical_empty(d, bnd)) {
            break;
        }
        ++d;
    }

    size_t w = x;

    while (w + 1 < bnd.w()) {
        ++w;
        if ([](Pixel const * d, size_t w, size_t h) -> bool {
            for (auto e = d+w*h; d != e; d += w) {
                if (is_pix_letter(*d) && (
                    (d+1 != e && is_pix_letter(*(d+1)))
                 || (d-w+1 < e && is_pix_letter(*(d-w+1)))
                 || (d+w+1 < e && is_pix_letter(*(d+w+1)))
                )) {
                    return false;
                }
            }
            return true;
        }(d, bnd.w(), bnd.h())) {
            break;
        }
        ++d;
    }
    w -= x;

    size_t y = idx.y();

    d = image.data({x, y});
    for (; y < bnd.h(); ++y) {
        if (!utils::horizontal_empty(d, w)) {
            break;
        }
        d += bnd.w();
    }

    size_t h = bnd.h();

    d = image.data({x, h});
    while (--h > y) {
        d -= bnd.w();
        if (!utils::horizontal_empty(d, w)) {
            break;
        }
    }
    h -= y;

    ++h;
    return {{x, y}, {w, h}};
}

#endif
