#include "basic_proportionality.hpp"
#include "relationship.hpp"
#include "sassert.hpp"

#include <ostream>
#include <istream>


namespace strategies
{

void details_::check_interval(unsigned x, unsigned interval) { sassert(x <= interval); }

unsigned proportionality_base::relationship(const proportionality_base& other, unsigned interval) const
{ return utils::compute_relationship(value(), other.value(), interval); }

std::istream& operator>>(std::istream& is, proportionality_base & prop)
{
    decltype(prop.value()) proportion;
    is >> proportion;
    prop = proportionality_base(proportion);
    return is;
}

std::ostream& operator<<(std::ostream& os, const proportionality_base & prop)
{ return os << prop.value(); }

}
