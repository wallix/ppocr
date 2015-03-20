#include "proportionality.hpp"
#include "utils/relationship.hpp"
#include "image.hpp"

#include <ostream>
#include <istream>


namespace strategies
{

proportionality::proportionality(const Image& img, const Image& /*img90*/)
: proportion_(
    img.width() < img.height()
  ? -static_cast<int>(img.width() * 100 / img.height())
  : static_cast<int>(img.height() * 100 / img.width())
)
{}

unsigned proportionality::relationship(const proportionality& other) const
{ return utils::compute_relationship(proportion(), other.proportion(), 200); }

std::istream& operator>>(std::istream& is, proportionality& prop)
{ return is >> prop.proportion_; }

std::ostream& operator<<(std::ostream& os, const proportionality& prop)
{ return os << prop.proportion(); }

}
