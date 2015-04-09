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

#ifndef REDEMPTION_SRC_STRATEGIES_UTILS_DIAGONAL_ZONE_DENSITY_HPP
#define REDEMPTION_SRC_STRATEGIES_UTILS_DIAGONAL_ZONE_DENSITY_HPP

#include "image/image.hpp"

#include <algorithm>


namespace strategies { namespace utils {

namespace details_ {
    template<class Fn>
    void diagonal_zone_density_impl(const Image& img, Fn fn)
    {
        int const area = int(img.area());
        int const h3 = int(img.height() * 3);
        int const w3 = int(img.width() * 3);
        int yp = 0;
        for (size_t y = 0; y < img.height(); ++y, yp += w3) {
            int const xs = (w3 + yp - area + h3-1) / h3;
            int const xe = (     yp + area       ) / h3;
            fn(std::max(0, xs), std::min(int(img.width()), xe));
        }
    }
}

inline unsigned diagonal_zone_density_area(const Image& img)
{
    if (img.width() < 3 && img.height() < 3) {
        return is_pix_letter(*img.data()) ? 1 : 0;
    }
    int res = 0;
    details_::diagonal_zone_density_impl(img, [&](int xs, int xe) { res += xe-xs; });
    return unsigned(res);
}

inline
unsigned diagonal_zone_density(const Image& img)
{
    if (img.width() < 3 && img.height() < 3) {
        return is_pix_letter(*img.data()) ? 1 : 0;
    }
    int res = 0;
    auto data = img.data();
    details_::diagonal_zone_density_impl(img, [&](int xs, int xe) {
        res += std::count_if(data+xs, data+xe, is_pix_letter_fn());
        data += img.width();
    });
    return unsigned(res);
}

} }

#endif
