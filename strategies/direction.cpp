#include "direction.hpp"
#include "image.hpp"

#include "utils/relationship.hpp"
#include "utils/horizontal_zone.hpp"
#include "utils/horizontal_direction.hpp"

#include <ostream>
#include <istream>

#include <type_traits>


namespace strategies
{

direction::direction(const Image& img, const Image& img90)
: d(static_cast<cardinal_direction>(
    utils::horizontal_zone_1_2_3_4_5(img, utils::horizontal_direction(img))
 | (utils::horizontal_zone_1_2_3_4_5(img90, utils::horizontal_direction(img90)) << 3)
))
{}

unsigned direction::relationship(const direction& other) const
{ return utils::cardinal_relationship(d, other.d); }

std::istream& operator>>(std::istream& is, direction& d)
{ return is >> reinterpret_cast<std::underlying_type_t<decltype(d.d)>&>(d.d); }

std::ostream& operator<<(std::ostream& os, const direction& d)
{ return os << d.id(); }

}
