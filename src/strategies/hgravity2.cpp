#include "hgravity2.hpp"

#include "utils/horizontal_gravity.hpp"

#include "image.hpp"


namespace strategies {

static unsigned compute_hgravity2(const Image& img)
{
    auto const hby2 = img.height() / 2;
    if (!hby2) {
        return 50;
    }
    auto const top_bottom = utils::horizontal_gravity(img);
    auto const area = ((hby2 * (hby2 + 1)) / 2) * img.width() * 2;
    auto const sum = top_bottom.top + top_bottom.bottom;
    auto const ret = (sum + top_bottom.top - top_bottom.bottom) * 100 / area;
    return ret;
}

unsigned hgravity2_traits::compute(const Image&, const Image& img90)
{ return compute_hgravity2(img90); }

unsigned hgravity290_traits::compute(const Image& img, const Image&)
{ return compute_hgravity2(img); }

unsigned int hgravity2_traits::get_interval() { return 100; }
unsigned int hgravity290_traits::get_interval() { return 100; }

}
