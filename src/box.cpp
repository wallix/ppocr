#include "box.hpp"

#include <ostream>
// #include <istream>


std::ostream & operator<<(std::ostream & os, Box const & box) {
    return os << box.index() << ' ' << box.bounds();
}
