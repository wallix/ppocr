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

    std::set<unsigned> external_loop_idx;

    for (size_t x = 0; x < img.width(); ++x) {
        external_loop_idx.emplace(mirror[img.to_size_t({x, 0})]);
        external_loop_idx.emplace(mirror[img.to_size_t({x, img.height()-1})]);
    }

    for (size_t y = 0; y < img.height(); ++y) {
        external_loop_idx.emplace(mirror[img.to_size_t({0, y})]);
        external_loop_idx.emplace(mirror[img.to_size_t({img.width()-1, y})]);
    }

    if (*external_loop_idx.begin() == 0) {
        external_loop_idx.erase(0);
    }

    out_ = static_cast<unsigned>(external_loop_idx.size());
    in_ = n - 1 - out_;
}

unsigned int loop::relationship(const loop& other) const
{ return *this == other ? 100 : 0; }

std::ostream& operator<<(std::ostream& os, const loop& loop)
{ return os << loop.loop_in() << ' ' << loop.loop_out(); }

std::istream& operator>>(std::istream& is, loop& loop)
{ return is >> loop.in_ >> loop.out_; }

}
