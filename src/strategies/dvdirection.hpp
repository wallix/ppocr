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

#ifndef REDEMPTION_SRC_STRATEGIES_DVDIRECTION_HPP
#define REDEMPTION_SRC_STRATEGIES_DVDIRECTION_HPP

#include "utils/basic_proportionality.hpp"

namespace strategies {

struct dvdirection_traits {
    static unsigned get_interval();
    static unsigned compute(Image const & img, Image const & img90);
};

struct dvdirection90_traits {
    static unsigned get_interval();
    static unsigned compute(Image const & img, Image const & img90);
};

using dvdirection = basic_proportionality<dvdirection_traits>;
using dvdirection90 = basic_proportionality<dvdirection90_traits>;

}

#endif
