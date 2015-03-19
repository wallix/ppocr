#include "compass.hpp"
#include "image.hpp"

#include "utils/relationship.hpp"
#include "utils/horizontal_direction.hpp"

#include <ostream>
#include <istream>

namespace strategies
{

static int horizontal_compass(const Image& img)
{
    utils::TopBottom d = utils::horizontal_direction(img);

    return (d.top > d.bottom) ? 1
        : (d.top < d.bottom) ? 3
        : 2;
}

compass::compass(const Image& img, const Image& img90)
: d(static_cast<cardinal_direction>(horizontal_compass(img) | horizontal_compass(img90) << 2))
{}

unsigned int compass::relationship(const compass& other) const
{ return utils::mask_relationship(d, other.d, 0b11, 2, 2); }

std::istream& operator>>(std::istream& is, compass& d)
{ return is >> reinterpret_cast<std::underlying_type_t<compass::cardinal_direction>&>(d.d); }

std::ostream& operator<<(std::ostream& os, const compass& d)
{ return os << d.direction(); }

}
