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

#ifndef PPOCR_STRATEGIES_GRAVITY_HPP
#define PPOCR_STRATEGIES_GRAVITY_HPP

#include <iosfwd>

#include "utils/cardinal_direction.hpp"

namespace ppocr {

class Image;

namespace strategies
{
    struct gravity
    {
        using cardinal_direction = utils::CardinalDirection2;

        gravity() = default;

        gravity(const Image & img, const Image & img90);

        gravity(cardinal_direction g) : d(g) {}

        bool operator<(gravity const & other) const
        { return d < other.d; }

        bool operator==(gravity const & other) const
        { return d == other.d; }

        unsigned relationship(const gravity& other) const;

        cardinal_direction id() const noexcept { return d; }

        friend std::istream & operator>>(std::istream &, gravity &);

    private:
        cardinal_direction d = cardinal_direction::NONE;
    };

    std::ostream & operator<<(std::ostream &, gravity const &);
}

}

#endif
