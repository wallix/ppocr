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

#ifndef REDEMPTION_SRC_STRATEGIES_LOOP_HPP
#define REDEMPTION_SRC_STRATEGIES_LOOP_HPP

#include <array>
#include <iosfwd>

class Image;


namespace strategies {

    struct loop {
        using value_type = std::array<unsigned, 5>;

        loop() = default;
        loop(value_type const & datas) : datas_(datas) {}

        loop(const Image & img, const Image & img90);

        bool operator<(loop const & other) const
        { return datas_ < other.datas_; }

        bool operator==(loop const & other) const
        { return this->datas_ == other.datas_; }

        value_type const & datas() const noexcept { return datas_; }

        unsigned relationship(loop const & other) const;

        friend std::istream & operator>>(std::istream &, loop &);

    private:
        value_type datas_ = /*value_type*/{{}};
    };

    std::ostream & operator<<(std::ostream &, loop const &);

}

#endif
