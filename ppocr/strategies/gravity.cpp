#include "gravity.hpp"
#include "image/image.hpp"

#include "utils/relationship.hpp"
#include "utils/horizontal_gravity.hpp"
#include "utils/horizontal_zone.hpp"
#include "utils/cardinal_direction_io.hpp"

#include <ostream>
#include <istream>

#include <type_traits>

namespace ppocr { namespace strategies {

gravity::gravity(const Image& img, const Image& img90)
: d(static_cast<cardinal_direction>(
    utils::horizontal_zone_1_2_3_4_5(img, utils::horizontal_gravity(img))
 | (utils::horizontal_zone_1_2_3_4_5(img90, utils::horizontal_gravity(img90)) << 3)
))
{}

unsigned gravity::relationship(const gravity& other) const
{ return utils::cardinal_relationship(d, other.d); }

std::istream& operator>>(std::istream& is, gravity& d)
{ return is >> d.d; }

std::ostream& operator<<(std::ostream& os, const gravity& d)
{ return os << d.id(); }

} }
