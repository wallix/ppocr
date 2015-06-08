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
*   Copyright (C) Wallix 2010-2015
*   Author(s): Jonathan Poelen
*/

#include "glyphs_loader.hpp"
#include "image_io.hpp"

#include <ostream>
#include <istream>

namespace ppocr { namespace loader2 {

std::istream & operator>>(std::istream& is, View & view) {
    return is >> view.word >> view.font >> view.info_line;
}

std::istream & operator>>(std::istream& is, Views & views) {
    unsigned n;
    if (!(is >> n)) {
        return is;
    }
    unsigned i = 0;
    views.resize(n);
    while (i < n && (is >> views[i])) {
        ++i;
    }
    return is;
}

std::istream & operator>>(std::istream& is, Glyph & glyph) {
    if (read_img(is, glyph.img)) {
        is >> glyph.views;
    }
    return is;
}

std::istream & operator>>(std::istream& is, Glyphs & glyphs) {
    Glyph glyph;
    while (is >> glyph) {
        glyphs.push_back({std::move(glyph.img), glyph.views});
    }
    return is;
}


std::ostream & operator<<(std::ostream& os, View const & view) {
    return os << view.word << ' ' << view.font << ' ' << view.info_line;
}

std::ostream & operator<<(std::ostream& os, Views const & views) {
    os << views.size() << "\n";
    for (auto & view : views) {
        os << view << '\n';
    }
    return os;
}

std::ostream & operator<<(std::ostream& os, Glyph const & glyph) {
    return write_img(os, glyph.img) << "\n" << glyph.views;
}

std::ostream & operator<<(std::ostream& os, Glyphs const & glyphs) {
    for (auto & glyph : glyphs) {
        if (!(os << glyph)) {
            break;
        }
    }
    return os;
}

} }
