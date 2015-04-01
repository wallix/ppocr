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

#ifndef REDEMPTION_SRC_STRATEGIES_ZONE_HPP
#define REDEMPTION_SRC_STRATEGIES_ZONE_HPP

#include <array>
#include <iosfwd>

class Image;


namespace strategies {

    struct zone {
        enum {
            top_left_is_letter,
            bottom_right_is_letter,
            number_top_alternations,
            number_right_alternations,
            number_bottom_alternations,
            number_left_alternations,
            number_internal_alternations,
            number_index
        };
        using value_type = std::array<unsigned, number_index>;

        zone() = default;
        zone(value_type const & datas) : datas_(datas) {}

        zone(const Image & img, const Image & img90);

        bool operator<(zone const & other) const
        { return datas_ < other.datas_; }

        bool operator==(zone const & other) const
        { return this->datas_ == other.datas_; }

        value_type const & datas() const noexcept { return datas_; }

        unsigned relationship(zone const & other) const;

        friend std::istream & operator>>(std::istream &, zone &);

    private:
        value_type datas_ = /*value_type*/{{}};
    };

    std::ostream & operator<<(std::ostream &, zone const &);

}

#endif
