/*
  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program; if not, write to the Free Software
  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

  Author(s): Jonathan Poelen
*/

#define BOOST_AUTO_TEST_MAIN
#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE TestTrie
#include <boost/test/auto_unit_test.hpp>

#include "container/trie.hpp"

#include <algorithm>
#include <vector>
#include <iomanip>

using char_trie = container::trie<char>;
using flat_char_trie = container::flat_trie<char_trie::value_type>;

void str_tree(char_trie const & trie, std::string & s, unsigned depth = 0u) {
    for (auto & node : trie) {
        s.append(depth, ' ');
        s += node.get();
        if (node.is_terminal()) {
            s += " @";
        }
        s += '\n';
        str_tree(node.childrens(), s, depth+1);
    }
}

#ifndef CXX_ENABLE_ASAN
#include <sys/time.h>
#include <sys/resource.h>
#endif

BOOST_AUTO_TEST_CASE(TestTrie)
{
#ifndef CXX_ENABLE_ASAN
    {
        rlimit rlim{10000000, 10000000};
        setrlimit(RLIMIT_AS, &rlim);
    }
#endif

    std::vector<std::string> strings{"abcd", "abce", "abcehn", "abcehne", "abcehnu", "abcej", "azerty", "abc", "bob", "coco", "paco", "parano"};
    std::sort(begin(strings), end(strings));
    char_trie trie(begin(strings), end(strings));
    std::string s;
    str_tree(trie, s);
    BOOST_CHECK_EQUAL(
        s,
        "a\n"
        " b\n"
        "  c @\n"
        "   d @\n"
        "   e @\n"
        "    h\n"
        "     n @\n"
        "      e @\n"
        "      u @\n"
        "    j @\n"
        " z\n"
        "  e\n"
        "   r\n"
        "    t\n"
        "     y @\n"
        "b\n"
        " o\n"
        "  b @\n"
        "c\n"
        " o\n"
        "  c\n"
        "   o @\n"
        "p\n"
        " a\n"
        "  c\n"
        "   o @\n"
        "  r\n"
        "   a\n"
        "    n\n"
        "     o @\n"
    );

    flat_char_trie flat_trie(trie);

    BOOST_CHECK_EQUAL(flat_trie.childrens().size(), 4);

    {
        std::stringstream oss;
        auto write = [](std::stringstream & oss, flat_char_trie const & flat_trie) {
            for (auto & node : flat_trie.all()) {
                oss << node.get();
                if (node.empty()) {
                    oss << "  0 ";
                }
                else {
                    oss << std::setw(3) << node.relative_pos() << " ";
                }
                oss << node.size() << (node.is_terminal() ? " @\n" : "\n");
            }
        };

        write(oss, flat_trie);

        auto const str1 = oss.str();
        BOOST_CHECK_EQUAL(
            str1,
            "a  4 2\n"
            "b  5 1\n"
            "c  5 1\n"
            "p  5 1\n"
            "b  5 1\n"
            "z  5 1\n"
            "o  5 1\n"
            "o  5 1\n"
            "a  5 2\n"
            "c  6 2 @\n"
            "e  7 1\n"
            "b  0 0 @\n"
            "c  6 1\n"
            "c  6 1\n"
            "r  6 1\n"
            "d  0 0 @\n"
            "e  5 2 @\n"
            "r  6 1\n"
            "o  0 0 @\n"
            "o  0 0 @\n"
            "a  4 1\n"
            "h  4 1\n"
            "j  0 0 @\n"
            "t  3 1\n"
            "n  3 1\n"
            "n  3 2 @\n"
            "y  0 0 @\n"
            "o  0 0 @\n"
            "e  0 0 @\n"
            "u  0 0 @\n"
        );

        using node_type = flat_char_trie::node_type;
        std::vector<node_type> nodes;
        std::string l;
        while (std::getline(oss, l)) {
            std::istringstream iss(l);
            node_type::value_type c;
            unsigned sz;
            unsigned pos;
            char is_terminal = 0;
            iss >> c >> pos >> sz >> is_terminal;
            nodes.emplace_back(c, pos, sz, !!is_terminal);
        }
        flat_char_trie flat_trie2(std::move(nodes));

        write(oss, flat_trie2);

        BOOST_CHECK_EQUAL(str1, oss.str());
    }
}
