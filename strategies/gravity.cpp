#include "gravity.hpp"
#include "image.hpp"

#include "utils/relationship.hpp"

#include <ostream>
#include <istream>

#include <type_traits>

namespace strategies
{

static inline unsigned normalize_positif_gravity(const Image& img, unsigned d)
{
    if (d > img.height() / 4) {
        if (d > img.height()) {
            return 2;
        }
        return 1;
    }
    return 0;
}

static unsigned count_gravity(Bounds const & bnd, Pixel const * p, Pixel const * ep, bool is_top)
{
    unsigned g = 0;
    unsigned coef = is_top ? bnd.h() : 1;
    while (p != ep) {
        for (auto epl = p + bnd.w(); p != epl; ++p) {
            if (is_pix_letter(*p)) {
                g += coef;
            }
        }

        if (is_top) {
            --coef;
        }
        else {
            ++coef;
        }
    }
    return g;
}

static unsigned horizontal_gravity(const Image& img)
{
    Bounds const bnd(img.width(), img.height() / 2);
    auto p = img.data();
    auto ep = img.data({0, bnd.h()});
    unsigned const top = count_gravity(bnd, p, ep, true);
    p = ep;
    if (img.height() & 1) {
        p += img.width();
    }
    unsigned const bottom = count_gravity(bnd, p, img.data_end(), false);

    return (top > bottom) ? 3 + normalize_positif_gravity(img, top - bottom)
        : (top < bottom) ? 3 - normalize_positif_gravity(img, bottom - top)
        : 3;
}

gravity::gravity(const Image& img, const Image& img90)
: d(static_cast<cardinal_direction>(horizontal_gravity(img) | (horizontal_gravity(img90) << 3)))
{}

unsigned gravity::relationship(const gravity& other) const
{ return mask_relationship(d, other.d, 0b111, 3, 4); }

std::istream& operator>>(std::istream& is, gravity& d)
{ return is >> reinterpret_cast<std::underlying_type_t<decltype(d.d)>&>(d.d); }

std::ostream& operator<<(std::ostream& os, const gravity& d)
{ return os << d.id(); }

}
