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

#include "density.hpp"
#include <image/image.hpp>

unsigned int strategies::density_traits::get_interval()
{ return 100; }

unsigned int strategies::density_traits::compute(const Image& img, const Image& /*img90*/)
{
    size_t count = 0;
    auto first = img.data();
    auto last = img.data_end();
    for (; first != last; ++first) {
        if (is_pix_letter(*first)) {
            ++count;
        }
    }
    return count * 100 / img.area();
}
