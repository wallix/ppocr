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

#ifndef REDEMPTION_COORDINATE_HPP
#define REDEMPTION_COORDINATE_HPP

#include <cstddef>
#include <iosfwd>


using std::size_t;

struct Index {
    Index(size_t x, size_t y)
    : x_(x)
    , y_(y)
    {}

    Index() = default;

    size_t x() const noexcept { return x_; }
    size_t y() const noexcept { return y_; }

private:
    size_t x_ = 0;
    size_t y_ = 0;
};

struct Bounds {
    Bounds(size_t w, size_t h)
    : w_(w)
    , h_(h)
    {}

    Bounds() = default;

    size_t area() const noexcept { return w_ * h_; }
    size_t w() const noexcept { return w_; }
    size_t h() const noexcept { return h_; }

    size_t contains(Index const & idx) const
    { return idx.x() < w() && idx.y() < h(); }

private:
    size_t w_ = 0;
    size_t h_ = 0;
};


std::ostream & operator<<(std::ostream & os, Index const & idx);
std::istream & operator>>(std::istream & is, Index & idx);
std::ostream & operator<<(std::ostream & os, Bounds const & bnd);
std::istream & operator>>(std::istream & is, Bounds & bnd);

#endif
