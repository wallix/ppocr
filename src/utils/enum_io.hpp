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

#ifndef PPOCR_SRC_UTILS_ENUM_IO_HPP
#define PPOCR_SRC_UTILS_ENUM_IO_HPP

#include <ostream>
#include <istream>
#include <type_traits>


namespace ppocr { namespace utils {

template<class EnumT>
inline std::ostream & write_enum(std::ostream & os, EnumT e)
{ return os << static_cast<typename std::underlying_type<EnumT>::type>(e); }

template<class EnumT>
inline std::istream & read_enum(std::istream & is, EnumT & e)
{
    typename std::underlying_type<EnumT>::type i;
    is >> i;
    e = static_cast<EnumT>(i);
    return is;
}

namespace details {

    template<class E> struct ReadEnum { E & e; };
    template<class E> struct WriteEnum { E e; };

    template<class E>
    inline std::istream & operator>>(std::istream & is, ReadEnum<E> e)
    { return read_enum(is, e.e); }

    template<class E>
    inline std::ostream & operator<<(std::ostream & os, WriteEnum<E> e)
    { return write_enum(os, e.e); }

}

template<class E> inline details::WriteEnum<E> write_enum(E e) { return {e}; }
template<class E> inline details::ReadEnum<E> read_enum(E & e) { return {e}; }

} }

#endif
