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

#ifndef REDEMPTION_SRC_UTILS_DISAMBIGUOUS_HPP
#define REDEMPTION_SRC_UTILS_DISAMBIGUOUS_HPP

#include <algorithm>


namespace utils {

namespace details_ {
    template<class Trie, class FwIt, class Cont>
    bool disambiguous_impl(Trie const & trie, FwIt first, FwIt last, Cont & result) {
        for (auto & c : *first) {
            auto pos = trie.find(c);
            if (pos != trie.end()) {
                if (last - first == 1) {
                    if (pos->is_terminal()) {
                        result += c;
                        return true;
                    }
                }
                else if (!pos->empty() && disambiguous_impl(pos->nodes(), first+1, last, result)) {
                    result += c;
                    return true;
                }
            }
        }
        return false;
    }
}

template<class Trie, class FwIt, class Cont>
bool disambiguous(Trie const & trie, FwIt first, FwIt last, Cont & result) {
    if (first != last && details_::disambiguous_impl(trie, first, last, result)) {
        auto first = result.begin();
        auto last = result.end();
        for (; first < --last; ++first) {
            std::iter_swap(first, last);
        }
        return true;
    }
    return false;
}

}

#endif
