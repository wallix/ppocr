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

#ifndef REDEMPTION_SRC_STRATEGIES_UTILS_ALGO_BASE_HPP
#define REDEMPTION_SRC_STRATEGIES_UTILS_ALGO_BASE_HPP

class Image;

namespace strategies { namespace utils {

class linear_comparable;
class circular_comparable;
class no_comparable;

template<class Algo>
struct to_comparable;

template<class Algo>
struct algo_base
{
    using value_type = typename Algo::value_type;
    using comparable = typename to_comparable<value_type>::type;;

    algo_base() = default;
    algo_base(Image const & img);
    algo_base(zone3 z) : z(z) {}

    bool operator < (algo_base const & other) const noexcept { return x < other.x; }

    value_type get() const noexcept { return x; }

private:
    value_type x;
};

} }

#endif
