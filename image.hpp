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

#include <cstddef>
#include <iosfwd>
#include <memory>

using std::size_t;

struct Index {
    Index(size_t x, size_t y)
    : x_(x)
    , y_(y)
    {}

    size_t x() const noexcept { return x_; }
    size_t y() const noexcept { return y_; }

private:
    size_t x_;
    size_t y_;
};

struct Bounds {
    Bounds(size_t w, size_t h)
    : w_(w)
    , h_(h)
    {}

    size_t air() const noexcept { return w_ * h_; }
    size_t w() const noexcept { return w_; }
    size_t h() const noexcept { return h_; }

    size_t contains(Index const & idx) const
    { return idx.x() < w() && idx.y() < h(); }

private:
    size_t w_;
    size_t h_;
};

using Pixel = char;

inline bool is_letter(Pixel pix)
{ return pix == ' '; }

template<class PixelGetter>
struct HorizontalRange;

using PtrImageData = std::unique_ptr<Pixel[]>;

struct Image
{
    Image(Bounds const & bounds, PtrImageData data);
    Image(Bounds const & bounds, Pixel const * data, const Index & section_idx, const Bounds & section_bnd);

    size_t width() const noexcept { return bounds_.w(); }
    size_t height() const noexcept { return bounds_.h(); }

    Image section(const Index& section_idx, const Bounds& section_bnd) const;

    Image rotate90() const;

    friend std::ostream & operator<<(std::ostream &, Image const &);

private:
    PtrImageData data_;
    Bounds bounds_;

    template<class PixelGetter>
    friend class HorizontalRange;

    size_t to_size_t(Index const & idx) const noexcept
    { return idx.y() * this->width() + idx.x(); }

    Pixel const * data_at(Index const & idx) const noexcept
    { return this->data_.get() + to_size_t(idx); }
};


Image image_from_file(const char * filename);


template<class PixelGetter>
struct HorizontalRange
{
    struct iterator
    {
        bool operator<(iterator const & other) const { return data < other.data; }
        bool operator==(iterator const & other) const { return data == other.data; }
        bool operator!=(iterator const & other) const { return !(*this == other); }

        bool operator*() const { return r.pixel_get_(data); }

        iterator & operator++(iterator const & other) { ++data; return *this; }

    private:
        HorizontalRange & r;
        Pixel const * data;

        friend class HorizontalRange<PixelGetter>;
    };

    HorizontalRange(Image const & img, Index idx, size_t w, PixelGetter pixel_get)
    : pixel_get_(pixel_get)
    , w_(w)
    , data_(img.data_at(idx))
    {}

    iterator begin() { return {*this, data_}; }
    iterator end() { return {*this, data_ + w_}; }

private:
    PixelGetter pixel_get_;
    size_t w_;
    Pixel const * data_;
};


template<class PixelGetter>
HorizontalRange<PixelGetter> hrange(Image const & img, Index pos, size_t w, PixelGetter pixel_get)
{ return {img, pos, w, pixel_get}; }


struct NormalPixelGet {
    constexpr NormalPixelGet() noexcept {}

    bool operator()(Pixel const * p) const
    { return is_letter(*p); }
};

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
            if (is_letter(*p)) {
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

#endif
