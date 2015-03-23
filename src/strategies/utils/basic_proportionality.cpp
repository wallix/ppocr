#include "basic_proportionality.hpp"
#include "relationship.hpp"

#include <ostream>
#include <istream>


namespace strategies
{

unsigned proportionality_base::relationship(const proportionality_base& other, unsigned interval) const
{ return utils::compute_relationship(proportion(), other.proportion(), interval); }

std::istream& operator>>(std::istream& is, proportionality_base & prop)
{
    decltype(prop.proportion()) proportion;
    is >> proportion;
    prop = proportionality_base(proportion);
    return is;
}

std::ostream& operator<<(std::ostream& os, const proportionality_base & prop)
{ return os << prop.proportion(); }

}
