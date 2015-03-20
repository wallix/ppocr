#include "image_compare.hpp"
#include "image.hpp"

#include <algorithm>


int image_compare(Image const & a, Image const & b)
{
    if (a.width() < b.width()) {
        return -1;
    }
    if (a.width() > b.width()) {
        return 1;
    }
    if (a.height() < b.height()) {
        return -1;
    }
    if (a.height() > b.height()) {
        return 1;
    }

    auto const pair = std::mismatch(a.data(), a.data_end(), b.data());
    if (pair.first == a.data_end()) {
        return 0;
    }
    return (*pair.first < *pair.second) ? -1 : 1;
}
