#include "data_loader.hpp"

#include "strategies/alternation.hpp"
#include "strategies/direction.hpp"
#include "strategies/agravity.hpp"
#include "strategies/gravity.hpp"
#include "strategies/compass.hpp"
#include "strategies/dcompass.hpp"

void all_registy(DataLoader& loader)
{
    loader.registry<strategies::alternations>("alternations");
    loader.registry<strategies::direction>("direction");
    loader.registry<strategies::agravity>("agravity");
    loader.registry<strategies::gravity>("gravity");
    loader.registry<strategies::compass>("compass");
    loader.registry<strategies::dcompass>("dcompass");
}
