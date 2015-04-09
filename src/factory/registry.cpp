#include "data_loader.hpp"

#include "strategies/alternation.hpp"
// #include "strategies/direction.hpp"
#include "strategies/hdirection.hpp"
#include "strategies/hdirection2.hpp"
// #include "strategies/agravity.hpp"
// #include "strategies/gravity.hpp"
// #include "strategies/compass.hpp"
// #include "strategies/dcompass.hpp"
#include "strategies/proportionality.hpp"
// #include "strategies/gravity2.hpp"
#include "strategies/hgravity.hpp"
#include "strategies/hgravity2.hpp"

#include "strategies/dvdirection.hpp"
#include "strategies/dvdirection2.hpp"
#include "strategies/dvgravity.hpp"
#include "strategies/dvgravity2.hpp"

#include "strategies/dzdensity.hpp"

#include "strategies/density.hpp"

#include "strategies/zone.hpp"
#include "strategies/proportionality_zone.hpp"

#include "strategies/hbar.hpp"


void all_registy(DataLoader& loader)
{
#define REGISTRY(name) loader.registry<strategies::name>(#name)
    //REGISTRY(direction);

    REGISTRY(hdirection);
    REGISTRY(hdirection90);
    REGISTRY(hdirection2);
    REGISTRY(hdirection290);

    //REGISTRY(agravity);
    //REGISTRY(gravity);

    REGISTRY(hgravity);
    REGISTRY(hgravity90);
    REGISTRY(hgravity2);
    REGISTRY(hgravity290);

    //REGISTRY(compass);
    //REGISTRY(dcompass);

    REGISTRY(proportionality);
    //REGISTRY(gravity2);

    REGISTRY(dvdirection);
    REGISTRY(dvdirection90);
    REGISTRY(dvdirection2);
    REGISTRY(dvdirection290);

    REGISTRY(dvgravity);
    REGISTRY(dvgravity90);
    REGISTRY(dvgravity2);
    REGISTRY(dvgravity290);

    REGISTRY(density);

    REGISTRY(dzdensity);
    REGISTRY(dzdensity90);

    REGISTRY(hbar);
    REGISTRY(hbar90);


    REGISTRY(alternations);

    REGISTRY(zone);
    REGISTRY(proportionality_zone);
#undef REGISTRY
}
