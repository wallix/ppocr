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

#ifndef PPOCR_SRC_STRATEGIES_HBAR_HPP
#define PPOCR_SRC_STRATEGIES_HBAR_HPP

#include <iosfwd>

namespace ppocr {

class Image;


namespace strategies {

namespace details_ {
    struct basic_hbar {
        basic_hbar() = default;
        basic_hbar(unsigned n) : n_(n) {}

        basic_hbar(const Image & img);

        bool operator<(basic_hbar const & other) const
        { return n_ < other.n_; }

        bool operator==(basic_hbar const & other) const
        { return this->n_ == other.n_; }

        unsigned count() const noexcept { return n_; }

        unsigned relationship(basic_hbar const & other) const
        { return other.n_ == n_ ? 100 : 0; }

        unsigned best_difference() const { return 1; }

        friend std::istream & operator>>(std::istream &, basic_hbar &);

    private:
        unsigned n_ = 0;
    };

    std::ostream & operator<<(std::ostream &, basic_hbar const &);
}

struct hbar : details_::basic_hbar {
    hbar() = default;
    hbar(unsigned n) : details_::basic_hbar(n) {}
    hbar(const Image & img, const Image &) : details_::basic_hbar(img) {}
};

struct hbar90 : details_::basic_hbar {
    hbar90() = default;
    hbar90(unsigned n) : details_::basic_hbar(n) {}
    hbar90(const Image &, const Image & img90) : details_::basic_hbar(img90) {}
};

}

}

#endif
