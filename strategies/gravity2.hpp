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

#ifndef REDEMPTION_STRATEGIES_GRAVITY2_HPP
#define REDEMPTION_STRATEGIES_GRAVITY2_HPP

#include <iosfwd>

class Image;

namespace strategies
{
    struct gravity2
    {
        gravity2() = default;

        gravity2(const Image & img, const Image & img90);

        bool operator<(gravity2 const & other) const
        { return d < other.d; }

        bool operator==(gravity2 const & other) const
        { return d == other.d; }

        unsigned relationship(const gravity2& other) const;

        int id() const noexcept { return d; }

        friend std::istream & operator>>(std::istream &, gravity2 &);

    private:
        int d = 0;
    };

    std::ostream & operator<<(std::ostream &, gravity2 const &);
}

#endif
