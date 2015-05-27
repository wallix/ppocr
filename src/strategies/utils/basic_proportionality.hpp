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

#ifndef PPOCR_SRC_STRATEGIES_UTILS_BASIC_PROPORTIONALITY_HPP
#define PPOCR_SRC_STRATEGIES_UTILS_BASIC_PROPORTIONALITY_HPP

#include <iosfwd>


namespace ppocr {

class Image;

namespace strategies {

struct proportionality_base
{
    proportionality_base() = default;
    proportionality_base(unsigned proportion) : proportion_(proportion) {}

    bool operator<(proportionality_base const & other) const
    { return proportion_ < other.proportion_; }

    bool operator==(proportionality_base const & other) const
    { return this->proportion_ == other.proportion_; }

    unsigned value() const noexcept { return proportion_; }

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
    using traits = Traits;

    basic_proportionality(unsigned proportion)
    : proportionality_base(proportion)
    { details_::check_interval(proportion, traits::get_interval()); }

    basic_proportionality() = default;

    basic_proportionality(const Image & img, const Image & img90)
    : proportionality_base(traits::compute(img, img90))
    {}

    unsigned relationship(basic_proportionality const & other) const
    { return proportionality_base::relationship(other, traits::get_interval()); }

    unsigned best_difference() const { return Traits::best_difference(); }
};

}

}

#endif
