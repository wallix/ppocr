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

#ifndef REDEMPTION_SRC_FILTERS_LINE_HPP
#define REDEMPTION_SRC_FILTERS_LINE_HPP

#include "box_char/box.hpp"
#include "factory/definition.hpp"

#include <vector>
#include <iosfwd>


namespace filters {

using std::size_t;

enum class line_position
{
    unspecified = 0,
    upper = 1,
    below = 2,
    above = 4
};

std::ostream & operator<<(std::ostream & os, line_position);
std::istream & operator>>(std::istream & is, line_position &);

using ptr_def_list = std::vector<Definition const *>;

struct CharInfo {
    ptr_def_list ref_definitions;
    Box box;
    bool ok;
};

struct line
{
    struct data_line {
        size_t ascentline;
        size_t capline;
        size_t meanline;
        size_t baseline;

        bool is_valid() const
        { return ascentline != ~size_t{} && meanline != ~size_t{} && baseline != ~size_t{}; }
    };

    data_line search(std::vector<CharInfo> const & v) const;

    ptr_def_list operator()(CharInfo const & info, data_line const & data) const;

    friend std::ostream & operator<<(std::ostream & os, line const &);
    friend std::istream & operator>>(std::istream & is, line &);

private:
    struct Data {
        std::string c;
        line_position capline;
        line_position ascentline;
        line_position meanline;
        line_position baseline;
    };
    std::vector<Data> datas_;

    friend class line_impl;
};

}

#endif
