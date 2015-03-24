#include "gravity2.hpp"
#include "image/image.hpp"

#include "utils/relationship.hpp"
#include "utils/horizontal_gravity.hpp"

#include <ostream>
#include <istream>

#include <cassert>


namespace strategies
{

static unsigned horizontal_gravity2(const Image& img)
{
    utils::TopBottom g = utils::horizontal_gravity(img);

    size_t const hby2 = img.height()/2;
    int const gimg = ((hby2 * (hby2 + 1)) / 2) * img.width();
    unsigned ret = (gimg ? (int(g.top) - int(g.bottom)) * 100 / gimg : 0) + 100;

    assert(ret <= 200);
    return ret;
}

gravity2::gravity2(const Image& img, const Image& img90)
: d(horizontal_gravity2(img) | (horizontal_gravity2(img90) << 9))
{
    static_assert(sizeof(int) > 9*3/9, "short type");
}

unsigned gravity2::relationship(const gravity2& other) const
{ return utils::mask_relationship(d, other.d, 0b111111111, 9, 400); }

std::istream& operator>>(std::istream& is, gravity2& d)
{ return is >> d.d; }

std::ostream& operator<<(std::ostream& os, const gravity2& d)
{ return os << d.id(); }

}
