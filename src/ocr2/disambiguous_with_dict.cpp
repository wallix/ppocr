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

#include "disambiguous_with_dict.hpp"
#include "glyphs.hpp"

#include "../spell/word_disambiguouser.hpp"
#include "../spell/dictionary.hpp"

#include <cassert>


namespace {
    using namespace ppocr::ocr2;

    inline void append(std::string & result, Glyphs const & glyphs, view_ref const & v) {
        auto & s = glyphs.get_word(v);
        result.append(s.begin(), s.size());
    }

    inline void append(std::string & result, Glyphs const & glyphs, view_ref_list const & l) {
        if (!l.empty()) {
            append(result, glyphs, l.front());
        }
    }
}

unsigned ppocr::ocr2::disambiguous_with_dict(
    ambiguous_t & ambiguous,
    Glyphs const & glyphs,
    spell::Dictionary const & dict,
    std::vector<unsigned>::const_iterator it_space,
    std::string & result
) {
    unsigned unrecognized_count = 0;

    result.clear();
    auto search_fn = [&](ppocr::ocr2::view_ref_list const & vlist) -> bool {
        if (vlist.empty()) {
            return true;
        }
        auto & s = glyphs.get_word(vlist.front());
        return (s.size() == 1
            // TODO punct
            && (s.front() == '!'

            || s.front() == '#'
            || s.front() == '$'
            || s.front() == '%'
            || s.front() == '&'
            || s.front() == '\''
            || s.front() == '('
            || s.front() == ')'
            || s.front() == '*'
            || s.front() == '+'
            || s.front() == ','
            || s.front() == '-'
            || s.front() == '.'

            || s.front() == ':'
            || s.front() == ';'
            || s.front() == '<'
            || s.front() == '='
            || s.front() == '>'
            || s.front() == '?'

            || s.front() == '['
            || s.front() == ']'

            || s.front() == '{'
            || s.front() == '}'
            )
        );
    };
    ppocr::spell::WordDisambiguouser word_disambiguouser;

    using ambiguous_iterator_base = decltype(ambiguous.begin());
    struct ambiguous_view_ref_list_iterator : ambiguous_iterator_base
    {
        using Glyphs = ppocr::ocr2::Glyphs;

        Glyphs const & glyphs;

        ambiguous_view_ref_list_iterator(ambiguous_iterator_base it, Glyphs const & glyphs)
        : ambiguous_iterator_base(it)
        , glyphs(glyphs)
        {}

        struct value_type {
            std::reference_wrapper<ppocr::ocr2::view_ref_list> ref_list;
            Glyphs const & glyphs;

            using range_string_iterator_base = ppocr::ocr2::view_ref_list::const_iterator;
            struct range_string_iterator : range_string_iterator_base
            {
                Glyphs const & glyphs;

                range_string_iterator(range_string_iterator_base it, Glyphs const & glyphs)
                : range_string_iterator_base(it)
                , glyphs(glyphs)
                {}

                using value_type = Glyphs::string;
                value_type const & operator*() const {
                    return this->glyphs.get_word(range_string_iterator_base::operator*());
                }
            };

            range_string_iterator begin() const {
                return {ref_list.get().begin(), this->glyphs};
            }
            range_string_iterator end() const {
                return {ref_list.get().end(), this->glyphs};
            }
        };

        value_type operator*() const {
            return {ambiguous_iterator_base::operator*(), this->glyphs};
        }
        ambiguous_view_ref_list_iterator operator+(std::ptrdiff_t i) const {
            return {ambiguous_iterator_base::operator+(i), this->glyphs};
        }
    };

    auto first = ambiguous.begin();
    auto last = ambiguous.end();
    auto middle = std::find_if_not(first, first + *it_space, search_fn);
    for (; first != middle; ++first) {
        append(result, glyphs, *first);
    }
    if (first == ambiguous.begin() + *it_space) {
        ++it_space;
    }
    while (first != last) {
        auto e = ambiguous.begin() + *it_space;
        assert(e <= ambiguous.end());
        assert(first < e);
        auto middle = std::find_if(first, e, search_fn);
        assert(middle <= e);
        using It = ambiguous_view_ref_list_iterator;
        if (!word_disambiguouser(dict, It(first, glyphs), It(middle, glyphs), result)) {
            for (; first != middle; ++first) {
                assert(first <= ambiguous.end());
                append(result, glyphs, *first);
            }
        }

        for (; middle != e; ++middle) {
            if (middle->get().empty()) {
                result += '?';
                ++unrecognized_count;
            }
            else if (search_fn(*middle)) {
                append(result, glyphs, middle->get().front());
            }
            else {
                break;
            }
        }
        if (middle == e && e != last) {
            result += ' ';
            ++it_space;
        }

        first = middle;
    }

    return unrecognized_count;
}
