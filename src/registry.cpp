#include "data_loader.hpp"

#include "strategies/alternation.hpp"
#include "strategies/direction.hpp"
#include "strategies/agravity.hpp"
#include "strategies/gravity.hpp"
#include "strategies/compass.hpp"
#include "strategies/dcompass.hpp"
#include "strategies/proportionality.hpp"
#include "strategies/gravity2.hpp"

void all_registy(DataLoader& loader)
{
    loader.registry<strategies::alternations>("alternations"); // good
    loader.registry<strategies::direction>("direction");
    loader.registry<strategies::agravity>("agravity"); // good
    loader.registry<strategies::gravity>("gravity");
    loader.registry<strategies::compass>("compass");
    loader.registry<strategies::dcompass>("dcompass"); // good
    loader.registry<strategies::proportionality>("proportionality"); // good
    loader.registry<strategies::gravity2>("gravity2"); // ?
}
