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

#ifndef REDEMPTION_SRC_STRATEGIES_UTILS_BASIC_PROPORTIONALITY_HPP
#define REDEMPTION_SRC_STRATEGIES_UTILS_BASIC_PROPORTIONALITY_HPP

#include <iosfwd>

class Image;

namespace strategies
{
    struct proportionality_base
    {
        proportionality_base() = default;
        proportionality_base(unsigned proportion) : proportion_(proportion) {}

        bool operator<(proportionality_base const & other) const
        { return proportion_ < other.proportion_; }

        bool operator==(proportionality_base const & other) const
        { return this->proportion_ == other.proportion_; }

        unsigned proportion() const noexcept { return proportion_; }

        friend std::istream & operator>>(std::istream &, proportionality_base &);

    protected:
        unsigned relationship(proportionality_base const & other, unsigned interval) const;

    private:
        unsigned proportion_ = 0;
    };

    std::ostream & operator<<(std::ostream &, proportionality_base const &);

    namespace details_ {
        void check_interval(unsigned x, unsigned interval);
    }

    template<class Traits>
    struct basic_proportionality : proportionality_base
    {
        basic_proportionality(unsigned proportion)
        : proportionality_base(proportion)
        { details_::check_interval(proportion, Traits::get_interval()); }

        basic_proportionality() = default;

        basic_proportionality(const Image & img, const Image & img90)
        : proportionality_base(Traits::compute(img, img90))
        {}

        unsigned relationship(basic_proportionality const & other) const
        { return proportionality_base::relationship(other, Traits::get_interval()); }
    };
}

#endif
