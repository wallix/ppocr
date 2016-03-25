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

#ifndef PPOCR_MAKE_BOX_CHARACTER_HPP
#define PPOCR_MAKE_BOX_CHARACTER_HPP

#include "ppocr/box_char/box.hpp"
#include "ppocr/image/pixel.hpp"

namespace ppocr {

class Image;
class Bounds;

namespace utils {
    bool horizontal_empty(Pixel const * d, std::size_t w);
    bool vertical_empty(Pixel const * d, Bounds const & bnd);
}

Box make_box_character(Image const & image, Index const & idx, Bounds const & bnd);

}

#endif
