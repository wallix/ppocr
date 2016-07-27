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
*   Copyright (C) Wallix 2010-2016
*   Author(s): Jonathan Poelen
*/

#pragma once

namespace ppocr
{
    template<class T>
    struct integer_iterator
    {
        using iterator_category = std::random_access_iterator_tag;
        using value_type = T;
        using difference_type = std::ptrdiff_t;
        using pointer = void;
        using const_reference = value_type const &;
        using reference = const_reference;

        value_type n;
        integer_iterator & operator++() { ++n; return *this; }
        reference operator*() const { return n; }
        difference_type operator-(integer_iterator const & other) const { return this->n - other.n; }
        bool operator==(integer_iterator const & other) const { return this->n == other.n; }
        bool operator!=(integer_iterator const & other) const { return !(*this == other); }
    };
}
