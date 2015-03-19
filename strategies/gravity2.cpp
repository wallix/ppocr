#include "gravity2.hpp"
#include "image.hpp"

#include "utils/relationship.hpp"

#include <ostream>
#include <istream>


namespace strategies
{

static int count_gravity2(Bounds const & bnd, Pixel const * p, Pixel const * ep, bool is_top)
{
    int g = 0;
    int coef = is_top ? bnd.h() : 1;
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

static unsigned horizontal_gravity2(const Image& img)
{
    Bounds const bnd(img.width(), img.height() / 2);
    auto p = img.data();
    auto ep = img.data({0, bnd.h()});
    int const top = count_gravity2(bnd, p, ep, true);
    p = ep;
    if (img.height() & 1) {
        p += img.width();
    }
    int const bottom = count_gravity2(bnd, p, img.data_end(), false);

    size_t const hby2 = img.height()/2;
    int const gimg = ((hby2 * (hby2 + 1)) / 2) * img.width();
    int const g = top - bottom;
    unsigned ret = (gimg ? g * 100 / gimg : 0) + 100;

    assert(ret <= 200);
    return ret;
//     return gimg ? g * 100 / gimg : (g ? 100 : 0);
}

gravity2::gravity2(const Image& img, const Image& img90)
: d(horizontal_gravity2(img) | (horizontal_gravity2(img90) << 9))
{
    static_assert(sizeof(int) > 9*3/9, "short type");
}

unsigned gravity2::relationship(const gravity2& other) const
{ return mask_relationship(d, other.d, 0b111111111, 9, 400); }

std::istream& operator>>(std::istream& is, gravity2& d)
{ return is >> d.d; }

std::ostream& operator<<(std::ostream& os, const gravity2& d)
{ return os << d.id(); }

}
