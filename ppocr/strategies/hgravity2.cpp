#include "hgravity2.hpp"

#include "utils/horizontal_gravity.hpp"

#include "image/image.hpp"


namespace ppocr { namespace strategies {

hgravity2::value_type hgravity2::load(Image const & img, Image const & /*img90*/) const
{
    auto const hby2 = img.height()/2;
    if (!hby2) {
        return 50;
    }
    auto const area = ((hby2 * (hby2 + 1)) / 2) * img.width();
    auto const top_bottom = utils::horizontal_gravity(img);
    auto const ret = (area + top_bottom.top - top_bottom.bottom) * 100 / (area * 2);
    return ret;
}

hgravity2::relationship_type hgravity2::relationship() const
{ return {100}; }

unsigned hgravity2::best_difference() const
{ return 5; }

} }
