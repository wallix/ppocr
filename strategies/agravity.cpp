#include "agravity.hpp"
#include "image.hpp"

#include "math/almost_equal.hpp"
#include "utils/relationship.hpp"
#include "utils/horizontal_gravity.hpp"

#include <ostream>
#include <istream>
#include <iomanip>

#include <cfloat>
#include <cmath>


namespace strategies
{

agravity::agravity(const Image& img, const Image& img90)
{
    utils::TopBottom const g1 = utils::horizontal_gravity(img);
    utils::TopBottom const g2 = utils::horizontal_gravity(img90);

    using Signed = long;

    auto const h1 = Signed(g1.top) - Signed(g1.bottom);
    auto const h2 = Signed(g2.top) - Signed(g2.bottom);

    if (h1 || h2) {
        this->a = std::asin(double(h1) / std::sqrt(h1*h1+h2*h2));
    }
}

bool agravity::operator==(const agravity& other) const
{ return almost_equal(this->a, other.a, 2); }

bool agravity::operator<(agravity const & other) const
{ return a < other.a && !(*this == other); }

unsigned agravity::relationship(const agravity& other) const
{
    // disable -Wfloat-equal
    std::equal_to<> const eq;
    if (eq(angle(), null_angle()) || eq(other.angle(), null_angle())) {
        return eq(other.angle(), angle()) ? 100 : 0;
    }
    return utils::compute_relationship(angle(), other.angle(), M_PI);
}

std::istream& operator>>(std::istream& is, agravity& ag)
{ return is >> ag.a; }

std::ostream& operator<<(std::ostream& os, const agravity& ag)
{
    auto const flags = os.setf(std::ios_base::fixed | std::ios_base::scientific, std::ios_base::floatfield);
    auto const precision = os.precision(std::numeric_limits<double>::max_exponent10);
    os << ag.angle();
    os.precision(precision);
    os.setf(flags, std::ios_base::floatfield);
    return os;
}

}
