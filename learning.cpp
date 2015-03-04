#include <functional>
#include <algorithm>
#include <iostream>
#include <vector>
#include <array>

#include <cstdint>
#include <cassert>
#include <cstring>

#include <stdexcept> // fix range.hpp
#include <falcon/range/range.hpp>
#include <falcon/iostreams/get_file_contents.hpp>

#include "mln/image/image2d.hh"
#include "mln/io/pbm/load.hh"
// #include "rgb8.hpp"

using std::size_t;

using component_type = char;

void print_image(std::ostream & os, component_type const * cs, size_t w, size_t h)
{
    os.fill(':');
    os.width(w+3);
    os << ":\n";
    while (h--) {
        os << ':';
        os.write(cs, w);
        os << ":\n";
        cs += w;
    }
    os.width(w+3);
    os << ":\n";
}

using vcomponent_type = std::vector<component_type>;

constexpr const component_type cc = ' ';
constexpr const component_type ncc = 'o';

bool isc(component_type c)
{ return c == ' '; }


struct box_type {
    size_t x;
    size_t y;
    size_t w;
    size_t h;

    explicit operator bool() const noexcept{
        return w && h;
    }
};

std::ostream & operator<<(std::ostream & os, box_type const & box) {
    return os << "box(" << box.x << ", " << box.y << ", " << box.w << ", " << box.h << ")";
}

bool horizontal_empty(component_type const * d, size_t w) {
    for (auto e = d+w; d != e; ++d) {
        if (isc(*d)) {
            return false;
        }
    }
    return true;
};
bool vertical_empty(component_type const * d, size_t w, size_t h) {
    for (auto e = d+w*h; d != e; d += w) {
        if (isc(*d)) {
            return false;
        }
    }
    return true;
};

box_type make_box_character(component_type const * cs, box_type const & cbox) {

    box_type box = cbox;

    auto d = cs + box.y*cbox.w + box.x;
    for (; box.x < cbox.w; ++box.x) {
        if (!vertical_empty(d, cbox.w, box.h)) {
            break;
        }
        ++d;
    }
    box.w = box.x;
    while (++box.w < cbox.w) {
        if (vertical_empty(d, cbox.w, box.h)) {
            break;
        }
        ++d;
    }
    box.w -= box.x + 1;

    d = cs + box.y*cbox.w + box.x;
    for (; box.y < cbox.h; ++box.y) {
        if (!horizontal_empty(d, box.w)) {
            break;
        }
        d += cbox.w;
    }
    d = cs + box.h*cbox.w + box.x;
    while (--box.h > box.y) {
        d -= cbox.w;
        if (!horizontal_empty(d, box.w)) {
            break;
        }
    }
    box.h -= box.y;

    ++box.h;
    return box;
}

vcomponent_type copy_from_box(box_type const & box, component_type const * cs, size_t w, size_t /*h*/) {
    vcomponent_type v(box.w*box.h);
    auto it = v.begin();
    cs += box.x + box.y * w;
    for (size_t y = 0; y < box.h; ++y) {
        it = std::copy(cs, cs + box.w, it);
        cs += w;
    }
    return v;
}

class vlight_type
{
    using internal_value_ = uint_fast16_t;

public:
    using value_type = bool;

    struct const_iterator
    {
        using iterator_category = std::forward_iterator_tag;

        const_iterator(internal_value_ const & v, unsigned offset = 0)
        : refv(v)
        , offset(offset)
        {}

        const_iterator & operator++() {
            ++offset;
            return *this;
        }

        bool operator*() const noexcept {
            return refv & (1 << offset);
        }

        bool operator<(const_iterator const & other) const noexcept {
            return offset < other.offset;
        }

        bool operator == (const_iterator const & other) const noexcept {
            return offset == other.offset;
        }

        bool operator != (const_iterator const & other) const noexcept {
            return offset != other.offset;
        }

    private:
        std::reference_wrapper<const internal_value_> refv;
        unsigned offset;
    };

    vlight_type() = default;

    vlight_type(size_t v, size_t offset) noexcept
    : v(v)
    , sz(offset)
    {
        assert(sz < 16);
    }


    void push_back(bool x) {
        assert(sz < 16);
        if (x) {
            v |= 1 << sz;
        }
        ++sz;
    }

    size_t size() const {
        return sz;
    }

    bool front() const {
        return v & 1;
    }

    bool back() const {
        return v & (1 << (sz-1));
    }

    const_iterator begin() const {
        return const_iterator(v, 0);
    }

    const_iterator end() const {
        return const_iterator(v, sz);
    }

    bool operator < (vlight_type const & other) const noexcept {
        return sz < other.sz && v < other.v;
    }

    bool operator == (vlight_type const & other) const noexcept {
        return v == other.v && sz == other.sz;
    }

    internal_value_ data() const noexcept {
        return v;
    }

private:
    internal_value_ v = 0;
    unsigned sz = 0;
};


vlight_type horizontal_light(vcomponent_type::const_iterator it, size_t w, size_t /*h*/) {
    vlight_type v;
    auto e = it + w;
    if (it != e) {
        v.push_back(isc(*it));
        ++it;
        for (; it != e; ++it) {
            if (isc(*it) != v.back()) {
                v.push_back(!v.back());
            }
        }
    }
    return v;
}

vlight_type vertical_light(vcomponent_type::const_iterator it, size_t w, size_t h) {
    vlight_type v;
    auto e = it + h * w;
    if (it != e) {
        v.push_back(isc(*it));
        it += w;
        for (; it != e; it += w) {
            if (isc(*it) != v.back()) {
                v.push_back(!v.back());
            }
        }
    }
    return v;
}

vlight_type horizontal_mask(vcomponent_type::const_iterator it, size_t hit, size_t w, size_t /*h*/) {
    vlight_type v;
    auto e = it + w;
    if (it != e) {
        v.push_back(!vertical_empty(it.base(), w, hit));
        ++it;
        for (; it != e; ++it) {
            if (vertical_empty(it.base(), w, hit) == v.back()) {
                v.push_back(!v.back());
            }
        }
    }
    return v;
}

vlight_type vertical_mask(vcomponent_type::const_iterator it, size_t wit, size_t w, size_t h) {
    vlight_type v;
    auto e = it + h * w;
    if (it != e) {
        v.push_back(!horizontal_empty(it.base(), wit));
        it += w;
        for (; it != e; it += w) {
            if (horizontal_empty(it.base(), wit) != v.back()) {
                v.push_back(!v.back());
            }
        }
    }
    return v;
}


void print_ligh(std::ostream & os, vlight_type const & v)
{
    for (auto && b : v) {
        os << (b  ? '.' : 'x');
    }
}


int main(int argc, char **argv)
{
    if (argc != 3) {
        return 1;
    }

    std::ifstream file(argv[2]);
    if (!file) {
        return 2;
    }

    mln::image2d<bool> img;
    if (!mln::io::pbm::load(img, argv[1])) {
        if (errno != 0) {
            std::cerr << strerror(errno) << std::endl;
        } else {
            std::cerr << "error" << std::endl;
        }
        return 4;
    }

    vcomponent_type vimg(img.ncols() * img.nrows());
    auto it = vimg.begin();
    for (unsigned y = 0; y < img.nrows(); y++) {
        for (unsigned j = 0; j < img.ncols(); j++) {
//             const auto & rgb = img.at(y, j);
//             unsigned char c
//             = ((511/*PPM_RED_WEIGHT*/   * rgb.red()   + 511)>>10)
//             + ((396/*PPM_GREEN_WEIGHT*/ * rgb.green() + 511)>>10)
//             + ((117/*PPM_BLUE_WEIGHT*/  * rgb.blue()  + 511)>>10);
            *it++ = img.at(y, j) ? ' ' : 'x';
//             *it++ = !(rgb.red() == 60 && rgb.green() == 64 && rgb.blue() == 72) ? ' ' : 'x';
//             *it++ = (rgb.red() == 255 && rgb.green() == 255 && rgb.blue() == 255) ? 'x' : ' ';
        }
    }

    size_t const h = img.nrows();
    size_t const w = img.ncols();
//     std::cout << vimg.size() << std::endl;

    box_type box{0, 0, w, h};

    std::string s;

    while (1) {
        auto const cbox = make_box_character(vimg.data(), box);
        if (!cbox) {
            break;
        }
//         std::cout << cbox << std::endl;

        auto vbox = copy_from_box(cbox, vimg.data(), w, h);

//         print_image(std::cout, vbox.data(), cbox.w, cbox.h);

        auto vlight1 = (cbox.h < 2)
          ? vlight_type()
          : horizontal_light(vbox.cbegin() + (cbox.h-2)/3*cbox.w,               cbox.w, cbox.h);
        auto vlight2 = horizontal_light(vbox.cbegin() + (cbox.h*2-1)/3*cbox.w,  cbox.w, cbox.h);
        auto vlight3 = vertical_light(vbox.cbegin() + (cbox.w/2),               cbox.w, cbox.h);

        auto vmask1 = horizontal_mask(vbox.cbegin(),                        cbox.h/3, cbox.w, cbox.h);
        auto vmask2 = horizontal_mask(vbox.cend() - (cbox.h/3) * cbox.w,    cbox.h/3, cbox.w, cbox.h);
        auto vmask3 = vertical_mask(vbox.cbegin(),                          cbox.w/3, cbox.w, cbox.h);
        auto vmask4 = vertical_mask(vbox.cbegin() + (cbox.w/3*2),           cbox.w/3, cbox.w, cbox.h);

        if (!(file >> s)) {
            return 10;
        }
        std::cout << s << ' ';
        std::cout << vlight1.size() << ' ' << vlight1.data() << ' ';
        std::cout << vlight2.size() << ' ' << vlight2.data() << ' ';
        std::cout << vlight3.size() << ' ' << vlight3.data() << ' ';
        std::cout << vmask1.size() << ' ' << vmask1.data() << ' ';
        std::cout << vmask2.size() << ' ' << vmask2.data() << ' ';
        std::cout << vmask3.size() << ' ' << vmask3.data() << ' ';
        std::cout << vmask4.size() << ' ' << vmask4.data() << '\n';

        box.x = cbox.x+cbox.w;
    }

    return 0;
}
