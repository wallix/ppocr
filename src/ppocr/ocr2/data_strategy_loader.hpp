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

#ifndef REDEMPTION_PPOCR_SRC_OCR2_DATA_STRATEGY_LOADER_HPP
#define REDEMPTION_PPOCR_SRC_OCR2_DATA_STRATEGY_LOADER_HPP

#include "ppocr/image/image.hpp"


namespace ppocr { namespace ocr2 {

template<class LoaderStrategy>
struct data_strategy_loader {
    using strategy_type = typename LoaderStrategy::strategy_type;
    using value_type = typename strategy_type::value_type;

    value_type x;

    value_type load(ppocr::Image const & img, ppocr::Image const & img90) {
        x = ppocr::loader2::load(strategy_type(), LoaderStrategy::policy, img, img90);
        return x;
    }
};

} }

#endif
