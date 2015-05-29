/*
*   This program is free software; you can redistribute it and/or modify
*   it under the terms of the GNU General Public License as published by
*   the Free Software Foundation; either version 2 of the License, or
*   (at your option) any later version.
*
*   This program is distributed in the hope that it will be useful,
*   but WITHOUT ANY WARRANTY; without even the implied warranty of
*   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*   GNU General Public License for more details.
*
*   You should have received a copy of the GNU General Public License
*   along with this program; if not, write to the Free Software
*   Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
*
*   Product name: redemption, a FLOSS RDP proxy
*   Copyright (C) Wallix 2010-2015
*   Author(s): Jonathan Poelen
*/

#ifndef PPOCR_SRC_DEFINED_LOADER_HPP
#define PPOCR_SRC_DEFINED_LOADER_HPP

#include "loader2/datas_loader.hpp"

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

namespace ppocr {

#define REGISTRY(name, policy) \
    loader2::Strategy<strategies::name, loader2::PolicyLoader::policy>
#define REGISTRY2(name) \
    loader2::Strategy<strategies::name, loader2::PolicyLoader::img>, \
    loader2::Strategy<strategies::name, loader2::PolicyLoader::img90>

using PpOcrLoader = loader2::Loader<
    REGISTRY2(hdirection),
    REGISTRY2(hdirection2),

    //REGISTRY(direction),

    //REGISTRY(agravity),
    //REGISTRY(gravity),

    REGISTRY2(hgravity),
    REGISTRY2(hgravity2),

    //REGISTRY(compass),
    //REGISTRY(dcompass),

    REGISTRY (proportionality),
    //REGISTRY(gravity2),

    REGISTRY2(dvdirection),
    REGISTRY2(dvdirection2),

    REGISTRY2(dvgravity),
    REGISTRY2(dvgravity2),

    REGISTRY (density),

    REGISTRY2(dzdensity),

    REGISTRY2(hbar),


    REGISTRY (alternations),

    REGISTRY (zone),
    REGISTRY (proportionality_zone)
>;

}

#undef REGISTRY2
#undef REGISTRY

#endif
