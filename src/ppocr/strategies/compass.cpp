#include "ppocr/compass.hpp"
#include "ppocr/image/image.hpp"

#include "ppocr/strategies/utils/relationship.hpp"
#include "ppocr/strategies/utils/horizontal_direction.hpp"
#include "ppocr/strategies/utils/cardinal_direction_io.hpp"

#include <ostream>
#include <istream>

namespace ppocr { namespace strategies {

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
{ return utils::cardinal_relationship(d, other.d); }

std::istream& operator>>(std::istream& is, compass& d)
{ return is >> d.d; }

std::ostream& operator<<(std::ostream& os, const compass& d)
{ return os << d.direction(); }

} }
