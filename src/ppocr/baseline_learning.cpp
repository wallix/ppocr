#include "ppocr/image/image.hpp"
#include "ppocr/image/image_from_file.hpp"
#include "ppocr/box_char/make_box_character.hpp"
#include "ppocr/filters/best_baseline.hpp"

#include <iostream>
#include <fstream>
#include <vector>

#include <cerrno>
#include <cstring>

using namespace ppocr;

static int search_baseline(const char * imagefilename) {
    Image img = image_from_file(imagefilename);
    //std::cerr << img << '\n';

    std::string s;

    struct Iterator : std::iterator<std::forward_iterator_tag, size_t> {
        Box box_;
        size_t x_;
        Image const * img_;

        Iterator() = default;
        Iterator(Image const & img)
        : box_(make_box_character(img, {0, 0}, img.bounds()))
        , x_(box_.x() + box_.w())
        , img_(&img)
        {}

        bool operator == (Iterator const & other) { return box_ == other.box_; }
        bool operator != (Iterator const & other) { return !(*this == other); }

        Iterator & operator++() {
            //std::cout << img_->section(box_.index(), box_.bounds()) << **this << "\n";
            box_ = make_box_character(*img_, {x_, 0}, img_->bounds());
            x_ = box_.x() + box_.w();
            return *this;
        }

        size_t operator*() const { return box_.bottom(); }
    };

    auto baseline = filters::best_baseline(Iterator{img}, Iterator{});

    std::cout << "baseline: " << baseline << '\n';

    return 0;
}

int main(int ac, char ** av)
{
    if (ac < 1) {
        std::cerr << av[0] << " datas\n";
        return 1;
    }

    std::ifstream file(av[1]);
    if (!file) {
        std::cerr << strerror(errno) << '\n';
        return 2;
    }

    int return_code = 0;

    for (int i = 1; i < ac; ++i) {
        int const code = search_baseline(av[i]);
        if (code) {
            return_code = code;
        }
    }

    return return_code;
}
