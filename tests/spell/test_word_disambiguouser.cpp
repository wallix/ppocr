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
#define BOOST_TEST_MODULE TestWordDisambiguouser
#include <boost/test/auto_unit_test.hpp>

#include "ppocr/spell/word_disambiguouser.hpp"

#include <vector>

using namespace ppocr;

BOOST_AUTO_TEST_CASE(TestWordDisambiguouser)
{
    spell::Dictionary dict(std::vector<std::string>{
        "abcd",
        "abce",
        "abcehn",
        "abcehne",
        "abcehnu",
        "abcej",
        "azerty",
        "abc",
        "bob",
        "coco",
        "paco",
        "parano",
        "à"
    });

    using characters_t = std::vector<std::string>;
    std::vector<characters_t> word{
        {"a"},
        {"b", "z"},
        {"e"},
        {"r"},
        {"u", "t"},
        {"y"},
    };

    spell::WordDisambiguouser disambiguouser;

    std::string s;
    BOOST_CHECK(disambiguouser(dict, word.begin(), word.end(), s));
    BOOST_CHECK_EQUAL(s, "azerty");

    word[1].emplace_back("i");
    s.clear();
    BOOST_CHECK(disambiguouser(dict, word.begin(), word.end(), s));
    BOOST_CHECK_EQUAL(s, "azerty");

    word.emplace_back(1, "y");
    s.clear();
    BOOST_CHECK(!disambiguouser(dict, word.begin(), word.end(), s));

    word.pop_back();
    word.pop_back();
    s.clear();
    BOOST_CHECK(!disambiguouser(dict, word.begin(), word.end(), s));

    word = {{"à"}};
    s.clear();
    BOOST_CHECK(disambiguouser(dict, word.begin(), word.end(), s));
    BOOST_CHECK_EQUAL(s, "à");
}
