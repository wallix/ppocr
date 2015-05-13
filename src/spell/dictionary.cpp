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

#include "dictionary.hpp"

#include <fstream>
#include <algorithm>

#include <cassert>


namespace spell {

Dictionary::Dictionary(container::trie<char> const & trie)
: trie_(trie)
{}

Dictionary::Dictionary(container::flat_trie<char> trie)
: trie_(std::move(trie))
{}

Dictionary::Dictionary(std::vector<std::string> words)
: trie_((
    std::sort(words.begin(), words.end()),
    words.erase(std::unique(words.begin(), words.end()), words.end()),
    container::trie<char>(words.begin(), words.end())
))
{}

namespace {
    struct IODictionary : Dictionary::Manipulator
    { using Dictionary::Manipulator::trie; };
}

std::ostream& operator<<(std::ostream& os, Dictionary const & dict)
{
    for (auto node : IODictionary().trie(dict).all()) {
        os
          << unsigned(node.get()) << " "
          << node.relative_pos() << " "
          << node.size() << " "
          << node.is_terminal()
          << "\n";
    }
    return os;
}

std::istream& operator>>(std::istream& is, Dictionary & dict)
{
    using trie_type = container::flat_trie<char>;
    std::vector<trie_type::node_type> nodes;
    unsigned c;
    unsigned pos;
    unsigned sz;
    bool terminal;
    char e1, e2, e3, e4;
    while (is >> c >> e1 >> pos >> e2 >> sz >> e3 >> terminal >> e4) {
        if (e1 != ' ' || e2 != ' ' || e3 != ' ' || e4 != '\n' || c < 256u) {
            throw std::runtime_error("bad format");
        }
        nodes.emplace_back(c, pos, sz, terminal);
    }
    dict = Dictionary(std::move(nodes));
    return is;
}

}
