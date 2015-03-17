#include "proportionality.hpp"
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

bool proportionality::operator==(const proportionality& other) const
{ return this->proportion_ == other.proportion_; }

std::istream& operator>>(std::istream& is, proportionality& prop)
{ return is >> prop.proportion_; }

std::ostream& operator<<(std::ostream& os, const proportionality& prop)
{ return os << prop.proportion(); }

}
