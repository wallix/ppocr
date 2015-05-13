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

#ifndef REDEMPTION_SRC_SPELL_DICTIONARY_HPP
#define REDEMPTION_SRC_SPELL_DICTIONARY_HPP

#include "container/trie.hpp"

#include <iosfwd>

namespace spell
{
    struct Dictionary
    {
        class Manipulator {
        protected:
            using trie_type = container::flat_trie<char>;

            trie_type const & trie(Dictionary const & dict)
            { return dict.trie_; }
        };

        Dictionary(std::vector<std::string> words);
        Dictionary(container::flat_trie<char> trie);
        Dictionary(container::trie<char> const & trie);
        Dictionary() = default;

    private:
        container::flat_trie<char> trie_;
    };

    std::istream & operator >> (std::istream & is, Dictionary & dict);
    std::ostream & operator << (std::ostream & os, Dictionary const & dict);
}

#endif
