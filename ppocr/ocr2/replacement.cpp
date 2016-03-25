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

#include "replacement.hpp"

#include <istream>

std::istream& ppocr::ocr2::operator>>(std::istream& is, Replacements& replacements) {
    using char_traits = std::char_traits<char>;
    Replacement rep;
    while (is) {
        is >> rep.pattern;
        auto c = is.rdbuf()->sgetc();
        if (!char_traits::eq_int_type(c, char_traits::eof())) {
            if (!char_traits::eq(char_traits::to_char_type(c), '\n')) {
                is >> rep.replace;
            }
            else {
                rep.replace.clear();
            }
        }
        if (is) {
            replacements.push_back(rep);
        }
    }
    return is;
}
