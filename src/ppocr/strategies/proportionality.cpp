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

#include "ppocr/proportionality.hpp"
#include "ppocr/image/image.hpp"


namespace ppocr { namespace strategies {

proportionality::value_type proportionality::load(Image const & img, Image const & /*img90*/) const
{ return img.width() * 100 / (img.width() + img.height()); }

proportionality::relationship_type proportionality::relationship() const
{ return {100}; }

unsigned proportionality::best_difference() const
{ return 5; }

} }
