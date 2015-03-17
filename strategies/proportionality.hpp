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

#ifndef REDEMPTION_STRATEGIES_PROPORTIONALITY_HPP
#define REDEMPTION_STRATEGIES_PROPORTIONALITY_HPP

#include <iosfwd>

class Image;

namespace strategies
{
    struct proportionality
    {
        static constexpr double null_angle = 100000;

        proportionality() = default;
        proportionality(int proportion) : proportion_(proportion) {}

        proportionality(const Image & img, const Image & img90);

        bool operator<(proportionality const & other) const
        { return proportion_ < other.proportion_; }

        bool operator==(proportionality const & other) const
        { return this->proportion_ == other.proportion_; }

        unsigned relationship(proportionality const & other) const;

        int proportion() const noexcept { return proportion_; }

        friend std::istream & operator>>(std::istream &, proportionality &);

    private:
        int proportion_ = 0;
    };

    std::ostream & operator<<(std::ostream &, proportionality const &);
}

#endif
