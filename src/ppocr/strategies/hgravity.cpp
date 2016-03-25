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

#include "ppocr//strategies/hgravity.hpp"

#include "ppocr/strategies/utils/horizontal_gravity.hpp"

#include "ppocr/image/image.hpp"

namespace ppocr { namespace strategies {

hgravity::value_type hgravity::load(Image const & img, Image const & /*img90*/) const
{
    auto const top_bottom = utils::horizontal_gravity(img);
    auto const sum = top_bottom.top + top_bottom.bottom;
    return sum ? (sum + top_bottom.top - top_bottom.bottom) * 100 / sum : 100;
}

hgravity::relationship_type hgravity::relationship() const
{ return {200}; }

unsigned hgravity::best_difference() const
{ return 10; }

} }