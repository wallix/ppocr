#include "direction.hpp"
#include "image.hpp"

#include "utils/relationship.hpp"

#include <ostream>
#include <istream>

#include <type_traits>


namespace strategies
{

static inline unsigned normalize_positif_direction(const Image& img, unsigned d)
{
    if (d > img.height() / 4) {
        if (d > img.height()) {
            return 2;
        }
        return 1;
    }
    return 0;
}

static unsigned horizontal_direction(const Image& img)
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

    return (top > bottom) ? 3 + normalize_positif_direction(img, top - bottom)
        : (top < bottom) ? 3 - normalize_positif_direction(img, bottom - top)
        : 3;
}

direction::direction(const Image& img, const Image& img90)
: d(static_cast<cardinal_direction>(horizontal_direction(img) | (horizontal_direction(img90) << 3)))
{}

unsigned direction::relationship(const direction& other) const
{ return mask_relationship(d, other.d, 0b111, 3, 4); }

std::istream& operator>>(std::istream& is, direction& d)
{ return is >> reinterpret_cast<std::underlying_type_t<decltype(d.d)>&>(d.d); }

std::ostream& operator<<(std::ostream& os, const direction& d)
{ return os << d.id(); }

}
