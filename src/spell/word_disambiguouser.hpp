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

#ifndef REDEMPTION_SRC_SPELL_WORD_DISAMBIGUOUSER_HPP
#define REDEMPTION_SRC_SPELL_WORD_DISAMBIGUOUSER_HPP

#include "dictionary.hpp"
#include "utils/utf.hpp"

namespace spell {

struct WordDisambiguouser : Dictionary::Manipulator
{
    template<class FwRngStrIt>
    bool operator()(Dictionary const & dict, FwRngStrIt first, FwRngStrIt last, std::string & output)
    {
        if (first == last || this->trie(dict).all().empty()) {
            return false;
        }
        auto const sz = output.size();
        if (disambiguous_impl(this->trie(dict).childrens(), first, last, output)) {
            auto first = output.begin() + sz;
            auto last = output.end();
            for (; first < --last; ++first) {
                std::iter_swap(first, last);
            }
            return true;
        }
        return false;
    }

private:
    // <trie_type::node_type&, uchar[sizeof(FwRngStrIt)]> stack;

    template<class FwRngStrIt>
    bool disambiguous_impl(trie_type::range rng, FwRngStrIt first, FwRngStrIt last, std::string & output) {
        for (auto & s : *first) {
            utf::UTF8toUnicodeIterator it(s.data());
            for (uint32_t c; (c = *it); ++it) {
                auto pos = rng.find(c);
                if (pos != rng.end()) {
                    if (first + 1 == last) {
                        if (pos->is_terminal()) {
                            set_c(output, c);
                            return true;
                        }
                    }
                    else if (!pos->empty() && disambiguous_impl(pos->childrens(), first+1, last, output)) {
                        set_c(output, c);
                        return true;
                    }
                }
            }
        }
        return false;
    }

    void set_c(std::string & s, uint64_t code) {
        char c[] = {
            char((code & 0xFF000000) >> 24),
            char((code & 0x00FF0000) >> 16),
            char((code & 0x0000FF00) >> 8),
            char((code & 0x000000FF)),
        };
        if (c[0]) {
            s.append(&c[0], &c[4]);
        }
        else if (c[1]) {
            s.append(&c[1], &c[4]);
        }
        else if (c[2]) {
            s.append(&c[2], &c[4]);
        }
        else {
            s += c;
        }
    }
};

}

#endif
