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

#ifndef REDEMPTION_IMAGE_HPP
#define REDEMPTION_IMAGE_HPP

#include "ppocr/image/coordinate.hpp"
#include "pixel.hpp"

#include <memory>


namespace ppocr {

template<class PixelGetter>
struct HorizontalRange;

using PtrImageData = std::unique_ptr<Pixel[]>;

struct Image
{
    Image() = default;

    Image(Bounds const & bounds, PtrImageData data);

    size_t width() const noexcept { return bounds_.w(); }
    size_t height() const noexcept { return bounds_.h(); }

    Bounds const & bounds() const noexcept { return bounds_; }
    size_t area() const noexcept { return bounds_.area(); }

    Image section(Index const & section_idx, Bounds const & section_bnd) const;

    Image rotate90() const;

    Image clone() const;

    Pixel operator[](Index const & idx) const noexcept
    { return data()[to_size_t(idx)]; }

    Pixel const * data() const noexcept { return this->data_.get(); }

    Pixel const * data(Index const & idx) const noexcept
    { return data() + to_size_t(idx); }

    Pixel const * data_end() const noexcept
    { return data() + width() * height(); }

    size_t to_size_t(Index const & idx) const noexcept
    { return idx.y() * this->width() + idx.x(); }

    explicit operator bool () const noexcept { return bool(this->data_); }
    PtrImageData release() { return std::move(data_); }

    friend std::ostream & operator<<(std::ostream &, Image const &);

private:
    PtrImageData data_;
    Bounds bounds_;

    template<class PixelGetter>
    friend class HorizontalRange;
};


void rotate90(Image const & from, Pixel * data);
void section(Image const & from, Pixel * data, Index const & idx, Bounds const & bnd);

bool operator == (Image const &, Image const &);
inline bool operator != (Image const & a, Image const & b) { return !(a == b); }


struct NormalPixelGet {
    constexpr NormalPixelGet() noexcept {}

    bool operator()(Pixel const * p) const
    { return is_pix_letter(*p); }
};

template<class PixelGetter = NormalPixelGet>
struct HorizontalRange
{
    struct iterator
    {
        using iterator_category = std::forward_iterator_tag;
        using difference_type = std::ptrdiff_t;
        using value_type = bool;
        using reference = bool;
        using pointer = void;

        bool operator<(iterator const & other) const { return data_ < other.data_; }
        bool operator==(iterator const & other) const { return data_ == other.data_; }
        bool operator!=(iterator const & other) const { return !(*this == other); }

        bool operator*() const { return r_.pixel_get_(data_); }

        bool operator-(iterator const & other) const { return data_ - other.data_; }

        iterator & operator++() { ++data_; return *this; }

        Pixel const * base() const noexcept { return data_; }

    private:
        HorizontalRange const & r_;
        Pixel const * data_;

        friend class HorizontalRange;

        iterator(HorizontalRange const & r, Pixel const * data)
        : r_(r)
        , data_(data)
        {}
    };

    HorizontalRange(Image const & img, Index idx, size_t w, PixelGetter pixel_get)
    : pixel_get_(pixel_get)
    , w_(w)
    , data_(img.data(idx))
    {}

    iterator begin() const { return {*this, data_}; }
    iterator end() const { return {*this, data_ + w_}; }
    size_t size() const { return w_; }
    bool operator[](size_t x) const { return pixel_get_(data_ + w_); }

private:
    PixelGetter pixel_get_;
    size_t w_;
    Pixel const * data_;
};


template<class PixelGetter>
HorizontalRange<PixelGetter> hrange(Image const & img, Index pos, size_t w, PixelGetter pixel_get)
{ return {img, pos, w, pixel_get}; }

inline HorizontalRange<NormalPixelGet> hrange(Image const & img, Index pos, size_t w)
{ return {img, pos, w, NormalPixelGet()}; }


struct AnyPixelGet
{
    constexpr AnyPixelGet(size_t h, size_t step) noexcept
    : h_(h)
    , step_(step)
    {}

    bool operator()(Pixel const * p) const
    {
        for (Pixel const * e = p + h_*step_; p != e; p += step_) {
            if (is_pix_letter(*p)) {
                return true;
            }
        }
        return false;
    }

private:
    size_t h_;
    size_t step_;
};

inline HorizontalRange<AnyPixelGet> hrange(Image const & img, Index pos, Bounds bounds)
{ return {img, pos, bounds.w(), AnyPixelGet(bounds.h(), img.width())}; }


namespace rng
{
    template<class PixelGetter>
    bool any(HorizontalRange<PixelGetter> const & range)
    {
        for (bool is : range) {
            if (is) {
                return true;
            }
        }
        return false;
    }

    template<class PixelGetter>
    bool all(HorizontalRange<PixelGetter> const & range)
    {
        for (bool is : range) {
            if (!is) {
                return false;
            }
        }
        return true;
    }

    template<class PixelGetter>
    bool none(HorizontalRange<PixelGetter> const & range)
    {
        for (bool is : range) {
            if (is) {
                return false;
            }
        }
        return true;
    }

    template<class Iterator>
    bool next_alternation(Iterator & it, Iterator last)
    {
        bool x = *it;
        while (it != last && *it == x) {
            ++it;
        }
        return it != last;
    }
}

}

#endif
