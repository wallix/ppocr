#include "ppocr/box.hpp"

#include <ostream>
// #include <istream>

namespace ppocr {

std::ostream & operator<<(std::ostream & os, Box const & box) {
    return os << box.index() << ' ' << box.bounds();
}

}
