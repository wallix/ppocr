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
#define BOOST_TEST_MODULE TestAuthentifierNew
#include <boost/test/auto_unit_test.hpp>

#include "container/trie.hpp"
#include "utils/disambiguous.hpp"

#include <algorithm>
#include <vector>

using char_trie = container::trie<char>;
using flat_char_trie = container::flat_trie<char_trie::value_type>;

BOOST_AUTO_TEST_CASE(TestDisambigus)
{
    std::vector<std::string> strings{"abcd", "abce", "abcehn", "abcehne", "abcehnu", "abcej", "azerty", "abc", "bob", "coco", "paco", "parano"};
    std::sort(begin(strings), end(strings));
    char_trie trie(begin(strings), end(strings));
    flat_char_trie flat_trie(trie);

    using letters_t = std::vector<char>;
    std::vector<letters_t> word{
        {'a'},
        {'b', 'z'},
        {'e'},
        {'r'},
        {'u', 't'},
        {'y'},
    };

    std::string s;
    BOOST_CHECK(utils::disambiguous(flat_trie.nodes(), word.begin(), word.end(), s));
    BOOST_CHECK_EQUAL(s, "azerty");

    word[1].emplace_back('i');
    s.clear();
    BOOST_CHECK(utils::disambiguous(flat_trie.nodes(), word.begin(), word.end(), s));
    BOOST_CHECK_EQUAL(s, "azerty");

    word.emplace_back(1, 'y');
    s.clear();
    BOOST_CHECK(!utils::disambiguous(flat_trie.nodes(), word.begin(), word.end(), s));

    word.pop_back();
    word.pop_back();
    s.clear();
    BOOST_CHECK(!utils::disambiguous(flat_trie.nodes(), word.begin(), word.end(), s));
}
