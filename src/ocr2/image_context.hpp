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

#ifndef REDEMPTION_PPOCR_SRC_OCR2_IMAGE_CONTEXT_HPP
#define REDEMPTION_PPOCR_SRC_OCR2_IMAGE_CONTEXT_HPP

#include "ppocr/src/image/image.hpp"

namespace ppocr { namespace ocr2 {

struct ImageContext
{
    template<class Maker>
    ppocr::Image const & img(unsigned w, unsigned h, Maker maker) {
        auto data = ptr(w*h, this->img_, this->img_sz_);
        maker(data.get());
        this->img_ = ppocr::Image({w, h}, std::move(data));
        return this->img_;
    }

    ppocr::Image const & img90() {
        this->img90_ = ppocr::rotate90(
            this->img_,
            ptr(this->img_.area(), this->img90_, this->img90_sz_)
        );
        return this->img90_;
    }

private:
    ppocr::Image img_;
    ppocr::Image img90_;
    std::size_t img_sz_ = 0;
    std::size_t img90_sz_ = 0;

    static ppocr::PtrImageData ptr(std::size_t area, ppocr::Image & img, std::size_t & sz) {
        auto data = img.release();
        if (area > sz) {
            if (auto p = new(std::nothrow) ppocr::Pixel[area]) {
                data.reset(p);
                sz = area;
            }
            else {
                img = ppocr::Image(img.bounds(), std::move(data));
                throw std::bad_alloc();
            }
        }
        return data;
    }
};

} }

#endif
