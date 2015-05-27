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

#ifndef PPOCR_UTILS_RELATIONSHIP_HPP
#define PPOCR_UTILS_RELATIONSHIP_HPP

#include <type_traits>

#include "cardinal_direction.hpp"
#include "sassert.hpp"


namespace ppocr { namespace strategies { namespace utils {

template<class T, class U>
unsigned compute_relationship(T const & a, T const & b, U const & interval)
{
    using integer = typename std::conditional<std::is_signed<T>::value, int, unsigned>::type;
    auto d = integer{100} - ((a < b) ? b-a : a-b) * integer{100} / interval;
    assert(0 <= d && d <= 100);
    return d;
}

inline
unsigned mask_relationship(unsigned a, unsigned b, unsigned mask, unsigned rbit, unsigned interval)
{
    unsigned const a1 = a & mask;
    unsigned const a2 = b & mask;
    unsigned const b1 = (a & (mask << rbit)) >> rbit;
    unsigned const b2 = (b & (mask << rbit)) >> rbit;

    unsigned dist = 100 - (
        (a1 < a2 ? a2 - a1 : a1 - a2)
      + (b1 < b2 ? b2 - b1 : b1 - b2)
    ) * 50 / interval;
    assert(dist <= 100);
    return dist;
}


inline
unsigned cardinal_relationship(CardinalDirection a, CardinalDirection b)
{ return mask_relationship(static_cast<unsigned>(a), static_cast<unsigned>(b), 3, 2, 2); }

inline
unsigned cardinal_relationship(CardinalDirection2 a, CardinalDirection2 b)
{ return mask_relationship(static_cast<unsigned>(a), static_cast<unsigned>(b), 7, 3, 4); }


} } }

#endif
