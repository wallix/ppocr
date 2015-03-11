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

#ifndef REDEMPTION_UTILS_IMAGE_COMPARE_HPP
#define REDEMPTION_UTILS_IMAGE_COMPARE_HPP

#include <functional>

class Image;

int image_compare(Image const & a, Image const & b);

namespace std {
    template<>
    struct less<Image>
    {
        constexpr less() noexcept {}

        bool operator()(Image const & a, Image const & b) const
        { return image_compare(a, b) < 0; }
    };
}

using image_less = std::less<Image>;

#endif
