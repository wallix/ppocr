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

#include "dzdensity.hpp"

#include "utils/diagonal_zone_density.hpp"

#include "image/image.hpp"

#include <iostream>

namespace strategies {

unsigned dzdensity_traits::compute(const Image& img, const Image&)
{ return utils::diagonal_zone_density(img); }

unsigned dzdensity90_traits::compute(const Image&, const Image& img90)
{ return utils::diagonal_zone_density(img90); }

unsigned int dzdensity_traits::get_interval() { return 100; }
unsigned int dzdensity90_traits::get_interval() { return 100; }

}
