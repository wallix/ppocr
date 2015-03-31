#include "line.hpp"
#include "utils/enum_io.hpp"

#include <algorithm>

namespace filters {

static size_t const invalid_result = ~size_t{};

struct line_impl
{
    static std::vector<line::Data>::const_iterator
    find_data(std::vector<line::Data> const & datas, std::string const & c)
    {
        return std::find_if(datas.begin(), datas.end(), [&](line::Data const & data) {
            return data.c == c;
        });
    }

    struct Ascent {
        size_t position(Box const & box) const { return box.top(); }
        size_t position(line::data_line const & data) const { return data.ascentline; }
        line_position get_line_position(line::Data const & data) const { return data.ascentline; }
    };
    struct Cap {
        size_t position(Box const & box) const { return box.top(); }
        size_t position(line::data_line const & data) const { return data.capline; }
        line_position get_line_position(line::Data const & data) const { return data.capline; }
    };
    struct Mean {
        size_t position(Box const & box) const { return box.top(); }
        size_t position(line::data_line const & data) const { return data.meanline; }
        line_position get_line_position(line::Data const & data) const { return data.meanline; }
    };
    struct Base {
        size_t position(Box const & box) const { return box.bottom(); }
        size_t position(line::data_line const & data) const { return data.baseline; }
        line_position get_line_position(line::Data const & data) const { return data.baseline; }
    };

    template<class F>
    static size_t
    search_impl(std::vector<line::Data> const & datas, const std::vector< CharInfo >& char_infos, F f)
    {
        /// NOTE simple impolementation
        size_t line = invalid_result;
        for (CharInfo const & info : char_infos) {
            if (info.ok) {
                Box const & box = info.box;
                auto p = line_impl::find_data(datas, info.ref_definitions.front()->c);
                if (p != datas.end()) {
                    if ((f.get_line_position(*p) & line_position::below) == line_position::below) {
                        line = f.position(box);
                        break;
                    }
                }
            }
        }
        return line;
    }

    template<class F>
    static void filter(
        std::vector<line::Data> const & datas, Box const & box,
        ptr_def_list & ptr_defs, line::data_line const & pos_line, F f
    ) {
        line_position pos
            = f.position(pos_line) == f.position(box)
            ? line_position::below
            : f.position(pos_line) < f.position(box)
            ? line_position::above
            : line_position::upper
        ;
        ptr_defs.erase(std::remove_if(ptr_defs.begin(), ptr_defs.end(), [&](Definition const * def) {
            auto p = line_impl::find_data(datas, def->c);
            return !(p == datas.end()
                || f.get_line_position(*p) == line_position::unspecified
                || (f.get_line_position(*p) & pos) != line_position::unspecified
            );
        }), ptr_defs.end());
    }
};

line::data_line line::search(const std::vector< CharInfo >& char_infos) const
{
    return {
        line_impl::search_impl(datas_, char_infos, line_impl::Ascent()),
        line_impl::search_impl(datas_, char_infos, line_impl::Cap()),
        line_impl::search_impl(datas_, char_infos, line_impl::Mean()),
        line_impl::search_impl(datas_, char_infos, line_impl::Base())
    };
}

void line::operator()(ptr_def_list & ptr_defs, Box const & box, line::data_line const & pos_line) const
{
    if (pos_line.ascentline != invalid_result) {
        line_impl::filter(datas_, box, ptr_defs, pos_line, line_impl::Ascent());
    }
    if (pos_line.capline != invalid_result) {
        line_impl::filter(datas_, box, ptr_defs, pos_line, line_impl::Cap());
    }
    if (pos_line.meanline != invalid_result) {
        line_impl::filter(datas_, box, ptr_defs, pos_line, line_impl::Mean());
    }
    if (pos_line.baseline != invalid_result) {
        line_impl::filter(datas_, box, ptr_defs, pos_line, line_impl::Base());
    }
}

std::ostream& operator<<(std::ostream& os, const line& x)
{
    for (auto & data : x.datas_) {
        os << data.c << " "
           << data.capline << " "
           << data.ascentline << " "
           << data.meanline << " "
           << data.baseline;
    }
    return os;
}

std::istream& operator>>(std::istream& is, line& x)
{
    line::Data data;
    while (is >> data.c >> data.ascentline >> data.capline >> data.meanline >> data.baseline) {
        x.datas_.push_back(std::move(data));
    }
    return is;
}

std::ostream& operator<<(std::ostream& os, line_position pos)
{ return utils::write_enum(os, pos); }

std::istream& operator>>(std::istream& os, line_position & pos)
{ return utils::read_enum(os, pos); }

}
