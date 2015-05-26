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
        auto sz = output.size();
        if (disambiguous_impl(this->trie(dict).childrens(), first, last, output)) {
            std::swap_ranges(output.begin() + sz, output.begin() + sz + (output.size() - sz) / 2, output.rbegin());
            return true;
        }
        return false;
    }

private:
    template<class FwRngStrIt>
    bool disambiguous_impl(trie_type::range rng, FwRngStrIt first, FwRngStrIt last, std::string & output) {
        for (auto & s : *first) {
            if (disambiguous_utf_char(rng, first, last, utf::UTF8Iterator(s.data()), output)) {
                return true;
            }
        }
        return false;
    }

    template<class FwRngStrIt>
    bool disambiguous_utf_char(trie_type::range rng, FwRngStrIt first, FwRngStrIt last, utf::UTF8Iterator it, std::string & output) {
        auto const c = *it;
        auto pos = rng.lower_bound(c);
        if (pos != rng.end() && pos->get() == c) {
            if (first + 1 == last) {
                if (pos->is_terminal()) {
                    set_c(output, c);
                    return true;
                }
            }
            else if (!pos->empty()) {
                if (*++it) {
                    if (disambiguous_utf_char(pos->childrens(), first, last, it, output)) {
                        set_c(output, c);
                        return true;
                    }
                }
                else if (disambiguous_impl(pos->childrens(), first+1, last, output)) {
                    set_c(output, c);
                    return true;
                }
            }
        }
        return false;
    }

    void set_c(std::string & s, uint32_t code) {
        char c[] = {
            char((code & 0x000000FF)),
            char((code & 0x0000FF00) >> 8),
            char((code & 0x00FF0000) >> 16),
            char((code & 0xFF000000) >> 24),
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
            s += c[3];
        }
    }
};

}

#endif
