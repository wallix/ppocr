#include "proportionality_zone.hpp"
#include "image/image.hpp"
#include "utils/count_zone.hpp"
#include "utils/relationship.hpp"

#include <algorithm>
#include <ostream>
#include <istream>

namespace ppocr { namespace strategies {

proportionality_zone::proportionality_zone(const Image& img, const Image& /*img90*/)
{
    utils::ZoneInfo zone_info = utils::count_zone(img);

    for (auto & x : zone_info.right) zone_info.top.insert(x);
    for (auto & x : zone_info.bottom) zone_info.top.insert(x);
    for (auto & x : zone_info.left) zone_info.top.insert(x);

    unsigned area = 0;
    for (auto & x : zone_info.top) {
        area += x.second;
    }

    for (auto & x : zone_info.top) {
        this->datas_.push_back(x.second * 100 / area);
    }

    std::sort(this->datas_.begin(), this->datas_.end());
}

unsigned proportionality_zone::relationship(const proportionality_zone& other) const
{
    if (other.datas_.size() != this->datas_.size()) {
        return 0;
    }
    if (datas_.empty()) {
        return 100;
    }
    unsigned const total = std::inner_product(
        datas_.begin(), datas_.end(), other.datas_.begin(), 0u
      , std::plus<unsigned>()
      , [](unsigned a, unsigned b) { return a*100/(a+b); }
    ) / datas_.size();
    return utils::compute_relationship(total, 50u, 100u);
}

std::ostream& operator<<(std::ostream& os, const proportionality_zone& proportionality_zone)
{
    os << proportionality_zone.datas().size();
    for (auto & x : proportionality_zone.datas()) {
        os << ' ' << x;
    }
    return os;
}

std::istream& operator>>(std::istream& is, proportionality_zone& proportionality_zone)
{
    size_t n;
    is >> n;
    proportionality_zone.datas_.resize(n);
    for (auto & x : proportionality_zone.datas_) {
        is >> x;
    }
    return is;
}

} }
