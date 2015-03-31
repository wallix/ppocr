#include "strategies/loop.hpp"
#include "image/image.hpp"
#include "utils/count_zone.hpp"

#include <ostream>
#include <istream>


namespace strategies {

loop::loop(const Image& img, const Image& /*img90*/)
{
    utils::ZoneInfo zone_info = utils::count_zone(img);

    datas_[top_left_is_letter] = is_pix_letter(img[{0, 0}]);
    datas_[bottom_right_is_letter] = is_pix_letter(img[{img.width()-1, img.height()-1}]);
    datas_[number_top_alternations] = zone_info.top.size();
    datas_[number_right_alternations] = zone_info.right.size();
    datas_[number_bottom_alternations] = zone_info.bottom.size();
    datas_[number_left_alternations] = zone_info.left.size();

    for (auto x : zone_info.right) zone_info.top.insert(x);
    for (auto x : zone_info.bottom) zone_info.top.insert(x);
    for (auto x : zone_info.left) zone_info.top.insert(x);

    datas_[number_internal_alternations] = zone_info.count_zone - 1 - static_cast<unsigned>(zone_info.top.size());
}

unsigned int loop::relationship(const loop& other) const
{ return *this == other ? 100 : 0; }

std::ostream& operator<<(std::ostream& os, const loop& loop)
{
    for (auto & x : loop.datas()) {
        os << x << " ";
    }
    return os;
}

std::istream& operator>>(std::istream& is, loop& loop)
{
    for (auto & x : loop.datas_) {
        is >> x;
    }
    return is;
}

}
