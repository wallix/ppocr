#include "alternation.hpp"
#include "image/image.hpp"

#include <ostream>
#include <istream>
#include <cassert>

#ifdef DEBUG_ALTERNATION
#include <iostream>
#define MAKE_SEQUENCE_ALTERNATION(name, img, x, y, B)                                          \
    (void(std::cout << name ":[" << img.bounds() << "][" << Index x, y << "] + [" << B << "]\n"), \
    make_alternations(img, x, y, B))
#else
#define MAKE_SEQUENCE_ALTERNATION(name, img, x, y, B) make_alternations(img, x, y, B)
#endif

namespace ppocr { namespace strategies {

namespace {
    template<class T>
    alternations::alternations_type
    make_alternations(const Image & img, Index const & idx, T const & bounds)
    {
        auto range = hrange(img, idx, bounds);
        auto it = range.begin();
        auto last = range.end();

        alternations::alternations_type alternations{*it, 1};
        while (rng::next_alternation(it, last)) {
            ++alternations.count;
        }

        return alternations;
    }
}

alternations::value_type alternations::load(const Image& img, const Image& img90) const
{
    alternations::value_type seq_alternations;
    auto it = seq_alternations.begin();

    {
        Bounds const & bnd = img.bounds();

        if (bnd.h() < 2) {
            *it++ = alternations_type{0, 0};
        } else {
            *it++ = MAKE_SEQUENCE_ALTERNATION("Hl1", img, {0, (bnd.h()-2)/3}, bnd.w());
        }
        *it++ = MAKE_SEQUENCE_ALTERNATION("Hl2", img, {0, (bnd.h()*2-1)/3}, bnd.w());
        Bounds const bnd_mask(bnd.w(), bnd.h()/3);
        *it++ = MAKE_SEQUENCE_ALTERNATION("Hm1", img, {0, 0}, bnd_mask);
        *it++ = MAKE_SEQUENCE_ALTERNATION("Hm2", img, {0, bnd.h()-bnd_mask.h()}, bnd_mask);
    }

    {
        Bounds const & bnd = img90.bounds();

        *it++ = MAKE_SEQUENCE_ALTERNATION("Vl1", img90, {0, (bnd.h()-1)/2}, bnd.w());
        Bounds const bnd_mask(bnd.w(), bnd.h()/3);
        *it++ = MAKE_SEQUENCE_ALTERNATION("Vm1", img90, {0, bnd.h()-bnd_mask.h()}, bnd_mask);
        *it++ = MAKE_SEQUENCE_ALTERNATION("Vm2", img90, {0, 0}, bnd_mask);
    }

    assert(it == seq_alternations.end());
    return seq_alternations;
}

unsigned alternations::best_difference() const
{ return 10; }

} }
