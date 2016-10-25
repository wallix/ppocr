/*
* Copyright (C) 2016 Wallix
* 
* This library is free software; you can redistribute it and/or modify it under
* the terms of the GNU Lesser General Public License as published by the Free
* Software Foundation; either version 2.1 of the License, or (at your option)
* any later version.
* 
* This library is distributed in the hope that it will be useful, but WITHOUT
* ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
* FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more
* details.
* 
* You should have received a copy of the GNU Lesser General Public License along
* with this library; if not, write to the Free Software Foundation, Inc., 59
* Temple Place, Suite 330, Boston, MA 02111-1307 USA
*/

#define BOOST_AUTO_TEST_MAIN
#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE TestWordDisambiguouser
#include "boost_unit_tests.hpp"

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
