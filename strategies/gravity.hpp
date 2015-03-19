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

#ifndef REDEMPTION_STRATEGIES_GRAVITY_HPP
#define REDEMPTION_STRATEGIES_GRAVITY_HPP

#include <iosfwd>

class Image;

namespace strategies
{
    struct gravity
    {
        enum cardinal_direction {
            NONE   = 3 + (3 << 3),
            NORTH2 = 5 + (3 << 3),
            NORTH  = 4 + (3 << 3),
            SOUTH  = 2 + (3 << 3),
            SOUTH2 = 1 + (3 << 3),
            EAST2  = 3 + (5 << 3),
            EAST   = 3 + (4 << 3),
            WEST   = 3 + (2 << 3),
            WEST2  = 3 + (1 << 3),
            NORTH_EAST   = (NORTH  & 0b111) | (EAST  & 0b111000),
            NORTH2_EAST  = (NORTH2 & 0b111) | (EAST  & 0b111000),
            NORTH_EAST2  = (NORTH  & 0b111) | (EAST2 & 0b111000),
            NORTH2_EAST2 = (NORTH2 & 0b111) | (EAST2 & 0b111000),
            NORTH_WEST   = (NORTH  & 0b111) | (WEST  & 0b111000),
            NORTH2_WEST  = (NORTH2 & 0b111) | (WEST  & 0b111000),
            NORTH_WEST2  = (NORTH  & 0b111) | (WEST2 & 0b111000),
            NORTH2_WEST2 = (NORTH2 & 0b111) | (WEST2 & 0b111000),
            SOUTH_EAST   = (SOUTH  & 0b111) | (EAST  & 0b111000),
            SOUTH2_EAST  = (SOUTH2 & 0b111) | (EAST  & 0b111000),
            SOUTH_EAST2  = (SOUTH  & 0b111) | (EAST2 & 0b111000),
            SOUTH2_EAST2 = (SOUTH2 & 0b111) | (EAST2 & 0b111000),
            SOUTH_WEST   = (SOUTH  & 0b111) | (WEST  & 0b111000),
            SOUTH2_WEST  = (SOUTH2 & 0b111) | (WEST  & 0b111000),
            SOUTH_WEST2  = (SOUTH  & 0b111) | (WEST2 & 0b111000),
            SOUTH2_WEST2 = (SOUTH2 & 0b111) | (WEST2 & 0b111000),
        };

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
        cardinal_direction d = NONE;
    };

    std::ostream & operator<<(std::ostream &, gravity const &);
}

#endif
