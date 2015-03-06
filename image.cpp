#include "image.hpp"
#include <ostream>

#include <cassert>

using cP = Pixel const *;
using P = Pixel *;

Image::Image(const Bounds& bounds, PtrImageData data)
: data_(std::move(data))
, bounds_(bounds)
{}

Image::Image(Bounds const & bounds, Pixel const * data, const Index & section_idx, const Bounds & section_bnd)
: data_(new Pixel[section_bnd.air()])
, bounds_(section_bnd)
{
    assert(data);
    cP d = data + to_size_t(section_idx);
    P out = data_.get();
    for (size_t y = 0; y != height(); ++y) {
        out = std::copy(d, d+width(), out);
        d += bounds.h();
    }
}

Image Image::section(const Index& section_idx, const Bounds& section_bnd) const
{
    assert(bounds_.contains(section_idx));
    assert(section_bnd.contains(section_idx));
    assert(section_bnd.w() <= width() && section_bnd.h() <= height());
    return Image(bounds_, data_.get(), section_idx, section_bnd);
}

Image Image::rotate90() const
{
    Bounds bnd(height(), width());
    PtrImageData data(new Pixel[bnd.air()]);
    P out = data.get();
    for (size_t x = 0; x != width(); ++x) {
        for (cP d = data_.get() + x, e = d + bounds_.air(); d != e; d += width()) {
            *out++ = *d;
        }
    }
    return {bnd, std::move(data)};
}


std::ostream & operator<<(std::ostream & os, Image const & image)
{
    os.fill(':');
    os.width(image.width()+3);
    os << ":\n";
    cP p = image.data_.get();
    for (size_t h = 0; h != image.height(); ++h) {
        os << ':';
        os.write(p, image.width());
        os << ":\n";
        p += image.width();
    }
    os.width(image.width()+3);
    os << ":\n";
    return os;
}

