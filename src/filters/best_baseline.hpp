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

#ifndef REDEMPTION_SRC_FILTERS_BEST_BASELINE_HPP
#define REDEMPTION_SRC_FILTERS_BEST_BASELINE_HPP

#include <algorithm>
#include <map>


namespace filters {

using std::size_t;

template<class FwIt>
typename std::iterator_traits<FwIt>::value_type
best_baseline(FwIt first, FwIt last)
{
    using value_type = typename std::iterator_traits<FwIt>::value_type;
    if (first == last) {
        return ~value_type{};
    }
    std::map<value_type, size_t> ys;
    for (; first != last; ++first) {
        ++ys[*first];
    }
    using cP = std::pair<value_type, size_t> const;
    return std::max_element(ys.begin(), ys.end(), [](cP & a, cP & b) {
        return a.second < b.second;
    })->first;
}

}

#endif
