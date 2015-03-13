#include "direction.hpp"
#include "image.hpp"

#include <ostream>
#include <istream>

namespace strategies
{

static inline int normalize_positif_direction(const Image& img, unsigned d, int plus)
{
    if (d > img.height() / 4) {
        if (d > img.height()) {
            return plus+2;
        }
        return plus+1;
    }
    return 0;
}

static int horizontal_direction(const Image& img)
{
    unsigned top = 0;
    unsigned bottom = 0;
    auto p = img.data();
    for (auto ep = img.data({0, img.height() / 2}); p != ep; ++p) {
        if (is_pix_letter(*p)) {
            ++top;
        }
    }
    if (img.height() & 1) {
        p += img.width();
    }
    for (auto ep = img.data_end(); p != ep; ++p) {
        if (is_pix_letter(*p)) {
            ++bottom;
        }
    }

    return (top > bottom) ? normalize_positif_direction(img, top - bottom, 0)
        : (top < bottom) ? normalize_positif_direction(img, bottom - top, 2)
        : 0;
}

direction::direction(const Image& img, const Image& img90)
: d(horizontal_direction(img) + horizontal_direction(img90) * 5)
{}


std::istream& operator>>(std::istream& is, direction& d)
{ return is >> d.d; }

std::ostream& operator<<(std::ostream& os, const direction& d)
{ return os << d.id(); }

}
