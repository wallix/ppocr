#include "compass.hpp"
#include "image.hpp"

#include "utils/relationship.hpp"

#include <ostream>
#include <istream>

namespace strategies
{

static int horizontal_compass(const Image& img)
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

    return (top > bottom) ? 1
        : (top < bottom) ? 2
        : 0;
}

compass::compass(const Image& img, const Image& img90)
: d(static_cast<cardinal_direction>(horizontal_compass(img) | horizontal_compass(img90) << 2))
{}

unsigned int compass::relationship(const compass& other) const
{ return mask_relationship(d, other.d, 0b11, 2, 25); }

std::istream& operator>>(std::istream& is, compass& d)
{ return is >> reinterpret_cast<std::underlying_type_t<compass::cardinal_direction>&>(d.d); }

std::ostream& operator<<(std::ostream& os, const compass& d)
{ return os << d.direction(); }

}
