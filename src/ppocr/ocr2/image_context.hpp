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

#include "../image/image.hpp"

namespace ppocr { namespace ocr2 {

struct ImageContext
{
    template<class Maker>
    ppocr::Image const & img(Bounds const & bnd, Maker maker) {
        this->reserve(bnd);
        maker(this->data_.get());
        return this->img_;
    }

    ppocr::Image const & img90() {
        this->img90_.release().release();
        this->img90_ = Image(
            {this->img_.height(), this->img_.width()},
            PtrImageData(this->data_.get() + this->img_.area())
        );
        ppocr::rotate90(this->img_, this->data_.get() + this->img_.area());
        return this->img90_;
    }

    ~ImageContext() {
        this->img_.release().release();
        this->img90_.release().release();
    }

private:
    PtrImageData data_;
    std::size_t capacity_sz_ = 0;

    ppocr::Image img_;
    ppocr::Image img90_;


    void reserve(Bounds const & bnd) {
        auto const new_sz = bnd.area() * 2;
        this->img_.release().release();
        if (new_sz > this->capacity_sz_) {
            this->data_.reset(new ppocr::Pixel[new_sz]);
            this->capacity_sz_ = new_sz;
        }
        this->img_ = Image(bnd, PtrImageData(this->data_.get()));
    }
};

} }

#endif
