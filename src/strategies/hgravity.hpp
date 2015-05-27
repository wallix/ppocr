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

#ifndef PPOCR_SRC_STRATEGIES_HGRAVITY_HPP
#define PPOCR_SRC_STRATEGIES_HGRAVITY_HPP

#include "utils/basic_proportionality.hpp"

namespace ppocr { namespace strategies {

struct hgravity_traits {
    static unsigned get_interval();
    static unsigned compute(Image const & img, Image const & img90);
    static unsigned best_difference();
};

struct hgravity90_traits {
    static unsigned get_interval();
    static unsigned compute(Image const & img, Image const & img90);
    static unsigned best_difference();
};

using hgravity = basic_proportionality<hgravity_traits>;
using hgravity90 = basic_proportionality<hgravity90_traits>;

} }

#endif
