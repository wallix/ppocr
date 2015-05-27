#include "image.hpp"
#include <ostream>

#include <cassert>

namespace ppocr {

namespace {
    using cP = Pixel const *;
    using P = Pixel *;
}

Image::Image(const Bounds& bounds, PtrImageData data)
: data_(std::move(data))
, bounds_(bounds)
{}

Image::Image(Image const & img, const Index & section_idx, const Bounds & section_bnd)
: data_(new Pixel[section_bnd.area()])
, bounds_(section_bnd)
{
    cP d = img.data(section_idx);
    P out = data_.get();
    for (size_t y = 0; y != height(); ++y) {
        out = std::copy(d, d+width(), out);
        d += img.width();
    }
}

Image Image::section(const Index& section_idx, const Bounds& section_bnd) const
{
    assert(bounds_.contains(section_idx));
    assert(section_bnd.w() + section_idx.x() <= width() && section_bnd.h() + section_idx.y() <= height());
    return Image(*this, section_idx, section_bnd);
}

Image Image::rotate90() const
{
    Bounds bnd(height(), width());
    PtrImageData data(new Pixel[bnd.area()]);
    P out = data.get();
    for (size_t x = width(); x; ) {
        --x;
        for (cP d = data_.get() + x, e = d + bounds_.area(); d != e; d += width()) {
            *out++ = *d;
        }
    }
    return {bnd, std::move(data)};
}

bool operator==(const Image& a, const Image& b)
{
    return a.width() == b.width()
        && a.height() == b.height()
        && std::equal(a.data(), a.data_end(), b.data());
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

}
