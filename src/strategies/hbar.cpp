#include "hbar.hpp"
#include "image/image.hpp"

#include <algorithm>
#include <ostream>
#include <istream>

namespace ppocr { namespace strategies { namespace details_ {

basic_hbar::basic_hbar(const Image& img)
{
    auto p = img.data();
    auto const e = img.data_end();
    bool previous_is_plain = false;
    for (; p != e; p += img.width()) {
        if (std::all_of(p, p+img.width(), [](Pixel const & pix) { return is_pix_letter(pix); })) {
            if (!previous_is_plain) {
                previous_is_plain = true;
                ++n_;
            }
        }
        else {
            previous_is_plain = false;
        }
    }
}

std::istream& operator>>(std::istream& is, basic_hbar& hbar)
{ return is >> hbar.n_; }

std::ostream& operator<<(std::ostream& os, const basic_hbar& hbar)
{ return os << hbar.count(); }

} } }
