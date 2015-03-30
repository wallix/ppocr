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

    struct Cap {
        size_t position(Box const & box) const { return box.top(); }
        size_t position(line::data_line const & data) const { return data.capline; }
        line_position get_line_position(line::Data const & data) const { return data.capline; }
    };
    struct Ascent {
        size_t position(Box const & box) const { return box.top(); }
        size_t position(line::data_line const & data) const { return data.ascentline; }
        line_position get_line_position(line::Data const & data) const { return data.ascentline; }
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
        size_t line = invalid_result;
        for (CharInfo const & info : char_infos) {
            if (info.ok) {
                Box const & box = info.box;
                auto p = line_impl::find_data(datas, info.ref_definitions.front()->c);
                if (p != datas.end()) {
                    if (f.get_line_position(*p) == line_position::below) {
                        line = f.position(box);
                        break;
                    }
                }
            }
        }
        return line;
    }

    template<class F>
    static ptr_def_list
    filter(
        std::vector<line::Data> const & datas, Box const & box,
        ptr_def_list const & ptr_defs, line::data_line pos_line, F f
    ) {
        ptr_def_list ret;
        line_position pos
            = f.position(pos_line) == f.position(box)
            ? line_position::below
            : f.position(pos_line) < f.position(box)
            ? line_position::above
            : line_position::upper
        ;
        for (Definition const * def : ptr_defs) {
            auto p = line_impl::find_data(datas, def->c);
            if (p == datas.end()
             || f.get_line_position(*p) == line_position::unspecified
             || f.get_line_position(*p) == pos
            ) {
                ret.push_back(def);
            }
        }
        return ret;
    }
};

line::data_line line::search(const std::vector< CharInfo >& char_infos) const
{
    return {
        line_impl::search_impl(datas_, char_infos, line_impl::Cap()),
        line_impl::search_impl(datas_, char_infos, line_impl::Ascent()),
        line_impl::search_impl(datas_, char_infos, line_impl::Mean()),
        line_impl::search_impl(datas_, char_infos, line_impl::Base())
    };
}

ptr_def_list line::operator()(const CharInfo& info, line::data_line const & pos_line) const
{
    ptr_def_list
    ret = line_impl::filter(datas_, info.box, info.ref_definitions, pos_line, line_impl::Cap());
    ret = line_impl::filter(datas_, info.box, ret, pos_line, line_impl::Ascent());
    ret = line_impl::filter(datas_, info.box, ret, pos_line, line_impl::Mean());
    ret = line_impl::filter(datas_, info.box, ret, pos_line, line_impl::Base());
    return ret;
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
    while (is >> data.c >> data.capline >> data.ascentline >> data.meanline >> data.baseline) {
        x.datas_.push_back(std::move(data));
    }
    return is;
}

std::ostream& operator<<(std::ostream& os, line_position pos)
{ return utils::write_enum(os, pos); }

std::istream& operator>>(std::istream& os, line_position & pos)
{ return utils::read_enum(os, pos); }

}
