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

#define BOOST_TEST_MODULE TestDisctionary
#include <boost/test/auto_unit_test.hpp>

#include "ppocr/spell/dictionary.hpp"

#include <algorithm>
#include <vector>
#include <iomanip>


#ifndef CXX_ENABLE_ASAN
#include <sys/time.h>
#include <sys/resource.h>
#endif

using namespace ppocr;

BOOST_AUTO_TEST_CASE(TestDisctionary)
{
#ifndef CXX_ENABLE_ASAN
    {
        rlimit rlim{10000000, 10000000};
        setrlimit(RLIMIT_AS, &rlim);
    }
#endif

    std::vector<std::string> strings{"abcd", "abce", "abcehn", "abcehne", "abcehnu", "abcej", "azerty", "abc", "bob", "coco", "paco", "parano"};
    ppocr::spell::Dictionary dict1(strings);

    std::stringstream io;

    io << dict1;

    ppocr::spell::Dictionary dict2;

    io >> dict2;

    struct DictionaryCheck : ppocr::spell::Dictionary::Manipulator {
        DictionaryCheck() {}

        bool operator()(spell::Dictionary & dict1, spell::Dictionary & dict2) const {
            auto & c1 = this->trie(dict1).all();
            auto & c2 = this->trie(dict2).all();
            using T = decltype(*c1.begin());
            return c1.size() == c2.size() && std::equal(c1.begin(), c1.end(), c2.begin(), [](T n1, T n2) {
                return n1.get() == n2.get()
                    && n1.relative_pos() == n2.relative_pos()
                    && n1.size() == n2.size()
                    && n1.is_terminal() == n2.is_terminal()
                ;
            });
        }
    };

    struct DictionarySize : ppocr::spell::Dictionary::Manipulator {
        DictionarySize() {}

        std::size_t operator()(ppocr::spell::Dictionary && dict) const {
            return this->trie(dict).all().size();
        }
    };

    BOOST_CHECK(DictionaryCheck()(dict1, dict2));

    BOOST_CHECK_EQUAL(DictionarySize()(ppocr::spell::Dictionary({"à"})), 1);
}
