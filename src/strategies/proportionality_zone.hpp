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

#ifndef REDEMPTION_SRC_STRATEGIES_PROPORTIONALITY_ZONE_HPP
#define REDEMPTION_SRC_STRATEGIES_PROPORTIONALITY_ZONE_HPP

#include <vector>
#include <iosfwd>

class Image;


namespace strategies {

    struct proportionality_zone {
        using value_type = std::vector<unsigned>;

        proportionality_zone() = default;
        proportionality_zone(value_type const & datas) : datas_(datas) {}

        proportionality_zone(const Image & img, const Image & img90);

        bool operator<(proportionality_zone const & other) const
        { return datas_ < other.datas_; }

        bool operator==(proportionality_zone const & other) const
        { return this->datas_ == other.datas_; }

        value_type const & datas() const noexcept { return datas_; }

        unsigned relationship(proportionality_zone const & other) const;

        friend std::istream & operator>>(std::istream &, proportionality_zone &);

    private:
        value_type datas_;
    };

    std::ostream & operator<<(std::ostream &, proportionality_zone const &);

}

#endif
