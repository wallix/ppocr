#include "proportionality.hpp"
#include "utils/relationship.hpp"
#include "image.hpp"

#include <ostream>
#include <istream>


namespace strategies
{

unsigned int proportionality_traits::compute(const Image& img, const Image& img90)
{
    return img.width() < img.height()
    ? -static_cast<int>(img.width() * 100 / img.height())
    : static_cast<int>(img.height() * 100 / img.width());
}

unsigned int proportionality_traits::get_interval()
{ return 200; }

}
