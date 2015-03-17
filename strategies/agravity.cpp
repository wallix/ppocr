#include "agravity.hpp"
#include "image.hpp"

#include "math/almost_equal.hpp"

#include <ostream>
#include <istream>
#include <iomanip>

#include <cfloat>
#include <cmath>


namespace strategies
{

static inline int normalize_positif_agravity(const Image& img, unsigned d, int plus)
{
    if (d > img.height() / 4) {
        if (d > img.height()) {
            return plus+2;
        }
        return plus+1;
    }
    return 0;
}

static unsigned count_agravity(Bounds const & bnd, Pixel const * p, Pixel const * ep, bool is_top)
{
    long /*long*/ g = 0;
    unsigned h = is_top ? bnd.h() : 1;
    while (p != ep) {
        for (auto epl = p + bnd.w(); p != epl; ++p) {
            if (is_pix_letter(*p)) {
                g += h;
            }
        }

        if (is_top) {
            --h;
        }
        else {
            ++h;
        }
    }
    return g;
}

static long horizontal_agravity(const Image& img)
{
    Bounds const bnd(img.width(), img.height() / 2);
    auto p = img.data();
    auto ep = img.data({0, bnd.h()});
    long const top = count_agravity(bnd, p, ep, true);
    p = ep;
    if (img.height() & 1) {
        p += img.width();
    }
    long const bottom = count_agravity(bnd, p, img.data_end(), false);

    return top - bottom;
}

agravity::agravity(const Image& img, const Image& img90)
{
    auto const h1 = horizontal_agravity(img);
    auto const h2 = horizontal_agravity(img90);

    if (h1 || h2) {
        this->a = std::asin(double(h1) / std::sqrt(h1*h1+h2*h2));
    }
}

bool agravity::operator==(const agravity& other) const
{ return almost_equal(this->a, other.a, 2); }

bool agravity::operator<(agravity const & other) const
{ return a < other.a && !(*this == other); }


std::istream& operator>>(std::istream& is, agravity& ag)
{ return is >> ag.a; }

std::ostream& operator<<(std::ostream& os, const agravity& ag)
{
    auto const flags = os.setf(std::ios_base::fixed | std::ios_base::scientific, std::ios_base::floatfield);
    auto const precision = os.precision(std::numeric_limits<double>::max_exponent10);
    os << ag.angle();
    os.precision(precision);
    os.setf(flags, std::ios_base::floatfield);
    return os;
}

}
