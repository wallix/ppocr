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

#ifndef PPOCR_UTILS_IMAGE_COMPARE_HPP
#define PPOCR_UTILS_IMAGE_COMPARE_HPP

#include <functional>

namespace ppocr {

class Image;

int image_compare(Image const & a, Image const & b);

}

namespace std {
    template<>
    struct less< ::ppocr::Image>
    {
        constexpr less() noexcept {}

        bool operator()(::ppocr::Image const & a, ::ppocr::Image const & b) const
        { return ::ppocr::image_compare(a, b) < 0; }
    };
}

namespace ppocr {

using image_less = std::less<Image>;

}

#endif
