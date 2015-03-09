#include "alternation.hpp"

#include <cassert>

#ifdef DEBUG_ALTERNATION
#include <iostream>
#define MAKE_SEQUENCE_ALTERNATION(name, img, x, y, B)                                          \
    (void(std::cout << name ":[" << img.bounds() << "][" << Index x, y << "] + [" << B << "]\n"), \
    make_sequence_alternation(img, x, y, B))
#else
#define MAKE_SEQUENCE_ALTERNATION(name, img, x, y, B) make_sequence_alternation(img, x, y, B)
#endif

namespace strategies {

std::array<alternation_seq_t, 7>
all_sequence_alternation(const Image& img, const Image& img90)
{
    std::array<alternation_seq_t, 7> alternations;

    auto it = alternations.begin();

    {
        Bounds const & bnd = img.bounds();

        if (bnd.h() < 2) {
            ++it;
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

    assert(it == alternations.end());

    return alternations;
}

}
