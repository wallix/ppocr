#include "hdirection2.hpp"

#include "utils/horizontal_direction.hpp"

#include "image/image.hpp"


namespace ppocr { namespace strategies {

hdirection2::value_type hdirection2::load(Image const & img, Image const & /*img90*/) const
{
    auto const area = img.height() / 2 * img.width();
    if (!area) {
        return 50;
    }
    auto const top_bottom = utils::horizontal_direction(img);
    auto const ret = (area + top_bottom.top - top_bottom.bottom) * 100 / (area * 2);
    return ret;
}

hdirection2::relationship_type hdirection2::relationship() const
{ return {100}; }

unsigned hdirection2::best_difference() const
{ return 5; }

} }
