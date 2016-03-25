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

#ifndef PPOCR_STRATEGIES_UTILS_CARDINAL_DIRECTION_IO_HPP
#define PPOCR_STRATEGIES_UTILS_CARDINAL_DIRECTION_IO_HPP

#include <ostream>
#include <istream>
#include <type_traits>

#include "ppocr/strategies/utils/cardinal_direction.hpp"


namespace ppocr { namespace strategies { namespace utils {

inline std::ostream & operator<<(std::ostream & os, CardinalDirection d)
{ return os << static_cast<std::underlying_type<decltype(d)>::type>(d); }

inline std::istream & operator>>(std::istream & is, CardinalDirection & d)
{
    std::underlying_type<CardinalDirection>::type i;
    is >> i;
    d = static_cast<CardinalDirection>(i);
    return is;
}

inline std::ostream & operator<<(std::ostream & os, CardinalDirection2 d)
{ return os << static_cast<std::underlying_type<decltype(d)>::type>(d); }

inline std::istream & operator>>(std::istream & is, CardinalDirection2 & d)
{
    std::underlying_type<CardinalDirection2>::type i;
    is >> i;
    d = static_cast<CardinalDirection2>(i);
    return is;
}

} } }


#endif