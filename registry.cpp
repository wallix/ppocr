#include "data_loader.hpp"

#include "strategies/alternation.hpp"
#include "strategies/direction.hpp"

void all_registy(DataLoader& loader)
{
    loader.registry<strategies::alternations>("alternations");
    loader.registry<strategies::direction>("direction");
}
