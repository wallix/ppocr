#include "hdirection2.hpp"

#include "utils/horizontal_direction.hpp"

#include "image.hpp"


namespace strategies {

static unsigned compute_hdirection2(const Image& img)
{
    auto const top_bottom = utils::horizontal_direction(img);
    auto const area = img.height() / 2 * img.width() * 2;
    if (!area) {
        return 50;
    }
    auto const sum = top_bottom.top + top_bottom.bottom;
    auto const ret = (sum + top_bottom.top - top_bottom.bottom) * 100 / area;
    return ret;
}

unsigned hdirection2_traits::compute(const Image&, const Image& img90)
{ return compute_hdirection2(img90); }

unsigned hdirection290_traits::compute(const Image& img, const Image&)
{ return compute_hdirection2(img); }

unsigned int hdirection2_traits::get_interval() { return 100; }
unsigned int hdirection290_traits::get_interval() { return 100; }

}
