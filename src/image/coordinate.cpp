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

#ifndef PPOCR_IMAGE_HPP
#define PPOCR_IMAGE_HPP

#include "coordinate.hpp"

#include <ostream>
#include <istream>


namespace ppocr {

std::ostream & operator<<(std::ostream & os, Index const & idx) {
    return os << idx.x() << ' ' << idx.y();
}

std::ostream & operator<<(std::ostream & os, Bounds const & bnd) {
    return os << bnd.w() << ' ' << bnd.h();
}

std::istream & operator>>(std::istream & is, Index & idx) {
    size_t x, y;
    if (is >> x >> y) {
        idx = Index(x, y);
    }
    return is;
}

std::istream & operator>>(std::istream & is, Bounds & bnd) {
    size_t w, h;
    if (is >> w >> h) {
        bnd = Bounds(w, h);
    }
    return is;
}

}

#endif
