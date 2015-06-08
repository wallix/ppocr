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

#include "loader2/datas_loader.hpp"

#include <ostream>
#include <istream>

namespace ppocr {

#define REGISTRY(name) \
    loader2::Strategy<strategies::name, loader2::PolicyLoader::img>
#define REGISTRY2(name) \
    loader2::Strategy<strategies::name, loader2::PolicyLoader::img>, \
    loader2::Strategy<strategies::name, loader2::PolicyLoader::img90>

namespace details_ {
    template<class... Strategies>
    struct pp_ocr_strategies
    {};

    template<class Strategies1, class Strategies2>
    struct pp_ocr_merge_strategies;

    template<class... Strategies1, class... Strategies2>
    struct pp_ocr_merge_strategies<pp_ocr_strategies<Strategies1...>, pp_ocr_strategies<Strategies2...>>
    { using type = pp_ocr_strategies<Strategies1..., Strategies2...>; };

    template<class Strategies>
    struct pp_ocr_to_datas;

    template<class... Strategies>
    struct pp_ocr_to_datas<pp_ocr_strategies<Strategies...>>
    { using type = loader2::Datas<Strategies...>; };
}

#ifdef IN_IDE_PARSER
using PpOcrDatas = loader2::Datas<
#else
using PpOcrSimpleDatas = details_::pp_ocr_strategies<
#endif
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

    REGISTRY2(dzdensity)
#ifdef IN_IDE_PARSER
,
#else
>;

using PpOcrComplexDatas = details_::pp_ocr_strategies<
#endif
    REGISTRY2(hbar),

    REGISTRY (alternations)
#ifdef IN_IDE_PARSER
,
#else
>;

using PpOcrExclusiveDatas = details_::pp_ocr_strategies<
#endif
    REGISTRY (zone),
    REGISTRY (proportionality_zone)
>;

#ifndef IN_IDE_PARSER
using PpOcrDatas = details_::pp_ocr_to_datas<
    details_::pp_ocr_merge_strategies<
        PpOcrSimpleDatas,
        details_::pp_ocr_merge_strategies<PpOcrComplexDatas, PpOcrExclusiveDatas>::type
    >::type
>::type;
#endif


// TODO other file

namespace details_ {
    struct WriteApplyData {
        std::ostream & os_;

        template<class Data>
        void operator()(Data const & data) const {
            if (!this->os_) {
                return;
            }

            for (auto & x : data.data()) {
                this->write(x);
            }
            this->os_ << '\n';
        }

    private:
        template<class T>
        void write(T const & x) const {
            this->os_ << x << ' ';
        }

        template<class T>
        void write(std::vector<T> const & cont) const {
            this->os_ << cont.size() << ' ';
            for (auto & x : cont) {
                this->os_ << x << ' ';
            }
        }

        template<class T, std::size_t N>
        void write(std::array<T, N> const & arr) const {
            for (auto & x : arr) {
                this->os_ << x << ' ';
            }
        }
    };

    struct ReadApplyData {
        std::istream & is_;
        std::size_t data_sz_;

        void operator()() const {
            if (this->is_) {
                std::istream::sentry s(this->is_);
            }
        }

        template<class Strategy, class... Strategies>
        void operator()(loader2::Data<Strategy> & data, loader2::Data<Strategies> & ... other) const {
            typename loader2::Data<Strategy>::container_type cont;

            if (this->is_) {
                cont.resize(this->data_sz_);
                for (auto & x : cont) {
                    this->read(x);
                }
            }

            data = loader2::Data<Strategy>(std::move(cont));

            (*this)(other...);
        }

    private:
        template<class T>
        void read(T & x) const {
            this->is_ >> x;
        }

        template<class T>
        void read(std::vector<T> & cont) const {
            std::size_t sz;
            if (this->is_ >> sz) {
                cont.resize(sz);
                for (auto & x : cont) {
                    this->is_ >> x;
                }
            }
        }

        template<class T, std::size_t N>
        void read(std::array<T, N> & arr) const {
            for (auto & x : arr) {
                this->is_ >> x;
            }
        }
    };
}

inline std::ostream & operator<<(std::ostream & os, PpOcrDatas const & datas) {
    os << datas.size() << '\n';
    loader2::apply_from_datas(datas, details_::WriteApplyData{os});
    return os;
}

template<class... Strategies>
std::istream & operator>>(std::istream & is, loader2::Datas<Strategies...> & datas) {
    std::size_t sz;
    is >> sz;
    using loader2::Data;
    struct Tuple : Data<Strategies>... {} t;
    details_::ReadApplyData read{is, sz};
    read(static_cast<Data<Strategies>&>(t)...);
    datas = PpOcrDatas(static_cast<Data<Strategies>&&>(t)...);
    return is;
}

}

#undef REGISTRY2
#undef REGISTRY

#endif
