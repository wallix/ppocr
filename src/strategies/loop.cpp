#include "strategies/loop.hpp"
#include "image/image.hpp"

#include <ostream>
#include <istream>

#include <vector>
#include <set>

namespace {
    bool propagation_loop(
        std::vector<unsigned> & mirror, Image const & img,
        unsigned current, Index const & idx
    ) {
        {
            auto & n = mirror[img.to_size_t(idx)];
            if (n || is_pix_letter(img[idx])) {
                return false;
            }
            n = current;
        }

        if (idx.x() != 0) {
            propagation_loop(mirror, img, current, {idx.x()-1, idx.y()});
        }
        if (idx.x() + 1 != img.width()) {
            propagation_loop(mirror, img, current, {idx.x()+1, idx.y()});
        }
        if (idx.y() != 0) {
            propagation_loop(mirror, img, current, {idx.x(), idx.y()-1});
        }
        if (idx.y() + 1 != img.height()) {
            propagation_loop(mirror, img, current, {idx.x(), idx.y()+1});
        }

        return true;
    }
}

namespace strategies {

loop::loop(const Image& img, const Image& /*img90*/)
{
    std::vector<unsigned> mirror(img.area(), 0);
    unsigned n = 1;

    for (size_t x = 0; x < img.width(); ++x) {
        for (size_t y = 0; y < img.height(); ++y) {
            if (propagation_loop(mirror, img, n, {x, y})) {
                ++n;
            }
        }
    }

    std::set<unsigned> top, bottom, right, left;

    auto insert = [&](std::set<unsigned> & set, size_t x, size_t y) {
        auto i = img.to_size_t({x, y});
        if (mirror[i]) {
            set.insert(mirror[i]);
        }
    };

    for (size_t x = 0; x < img.width(); ++x) {
        insert(top, x, 0);
        insert(bottom, x, img.height()-1);
    }

    for (size_t y = 0; y < img.height(); ++y) {
        insert(left, 0, y);
        insert(right, img.width()-1, y);
    }

    datas_[0] = top.size();
    datas_[1] = right.size();
    datas_[2] = bottom.size();
    datas_[3] = left.size();

    for (auto x : right) top.insert(x);
    for (auto x : bottom) top.insert(x);
    for (auto x : left) top.insert(x);

    datas_[4] = n - 1 - static_cast<unsigned>(top.size());
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
