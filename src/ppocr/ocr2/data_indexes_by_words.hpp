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

#ifndef REDEMPTION_PPOCR_SRC_OCR2_DATA_INDEXES_BY_WORDS_HPP
#define REDEMPTION_PPOCR_SRC_OCR2_DATA_INDEXES_BY_WORDS_HPP

#include "ppocr/ocr2/glyphs.hpp"

#include <vector>


namespace ppocr { namespace ocr2 {

struct DataIndexesByWords
{
    DataIndexesByWords(DataIndexesByWords &&) = default;
    DataIndexesByWords(DataIndexesByWords const &) = delete;

    DataIndexesByWords(Glyphs const & glyphs);

    std::vector<unsigned> const & operator[](std::size_t i) const noexcept {
        return this->indexes_by_words[i];
    }

private:
    std::vector<std::vector<unsigned>> indexes_by_words;
};


} }

#endif
