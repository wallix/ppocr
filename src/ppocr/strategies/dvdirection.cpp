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

#include "ppocr/strategies/dvdirection.hpp"

#include "ppocr/strategies/utils/diagonal_vertical_direction.hpp"

#include "ppocr/image/image.hpp"

namespace ppocr { namespace strategies {

dvdirection::value_type dvdirection::load(Image const & img, Image const & /*img90*/) const
{
    auto const top_bottom = utils::diagonal_vertical_direction(img);
    auto const sum = top_bottom.top + top_bottom.bottom;
    return sum ? (sum + top_bottom.top - top_bottom.bottom) * 100 / sum : 100;
}

dvdirection::relationship_type dvdirection::relationship() const
{ return {200}; }

unsigned dvdirection::best_difference() const
{ return 10; }

} }