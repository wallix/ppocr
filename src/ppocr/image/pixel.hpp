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

#ifndef PPOCR_SRC_IMAGE_PIXEL_HPP
#define REDEMPTION_PPOCR_SRC_IMAGE_PIXEL_HPP

namespace ppocr {

using Pixel = char;

inline bool is_pix_letter(Pixel pix) noexcept
{ return pix == 'x'; }

struct is_pix_letter_fn {
    constexpr is_pix_letter_fn() noexcept {}

    bool operator()(Pixel pix) const noexcept
    { return is_pix_letter(pix); }
};

}

#endif
