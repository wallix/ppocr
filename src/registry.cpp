#include "data_loader.hpp"

#include "strategies/alternation.hpp"
// #include "strategies/direction.hpp"
#include "strategies/hdirection.hpp"
#include "strategies/hdirection2.hpp"
#include "strategies/agravity.hpp"
#include "strategies/gravity.hpp"
#include "strategies/compass.hpp"
#include "strategies/dcompass.hpp"
#include "strategies/proportionality.hpp"
#include "strategies/gravity2.hpp"


void all_registy(DataLoader& loader)
{
#define REGISTRY(name) loader.registry<strategies::name>(#name)
    REGISTRY(alternations); // good
    //REGISTRY(direction);
    REGISTRY(hdirection);
    REGISTRY(hdirection2);
    REGISTRY(hdirection90);
    REGISTRY(hdirection290);
    REGISTRY(agravity); // good
    REGISTRY(gravity);
    REGISTRY(compass);
    REGISTRY(dcompass); // good
    REGISTRY(proportionality); // good
    REGISTRY(gravity2);
#undef REGISTRY
}
