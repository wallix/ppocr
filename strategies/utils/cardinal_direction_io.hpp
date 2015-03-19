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

#ifndef REDEMPTION_STRATEGIES_UTILS_CARDINAL_DIRECTION_IO_HPP
#define REDEMPTION_STRATEGIES_UTILS_CARDINAL_DIRECTION_IO_HPP

#include <ostream>
#include <istream>
#include <type_traits>

#include "cardinal_direction.hpp"


namespace strategies { namespace utils {

inline std::ostream & operator<<(std::ostream & os, CardinalDirection d)
{ return os << static_cast<std::underlying_type_t<decltype(d)>>(d); }

inline std::istream & operator>>(std::istream & is, CardinalDirection d)
{ return is >> reinterpret_cast<std::underlying_type_t<decltype(d)>&>(d); }

inline std::ostream & operator<<(std::ostream & os, CardinalDirection2 d)
{ return os << static_cast<std::underlying_type_t<decltype(d)>>(d); }

inline std::istream & operator>>(std::istream & is, CardinalDirection2 d)
{ return is >> reinterpret_cast<std::underlying_type_t<decltype(d)>&>(d); }

} }


#endif
