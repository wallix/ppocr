#include "ppocr/loader2/glyphs_loader.hpp"
#include "ppocr/utils/image_compare.hpp"
#include "ppocr/utils/read_file.hpp"
#include "ppocr/utils/reindex.hpp"
#include "ppocr/defined_loader.hpp"

#include <iostream>
#include <fstream>
#include <algorithm>

#include <cstring>
#include <cerrno>

using namespace ppocr;

using Glyphs = loader2::Glyphs;
using Glyph = loader2::Glyph;
using Views = loader2::Views;

template<class Glyphs, class Cmp, class Eq, class GetViews>
void reduce(Glyphs & cont, Cmp cmp, Eq eq, GetViews get_views) {
    auto last = cont.end();
    std::sort(cont.begin(), last, cmp);

    auto first = std::adjacent_find(cont.begin(), last, eq);

    auto first2 = first;
    while (first != last) {
        auto pos = std::upper_bound(first+1, last, *first, cmp);
        using value_type = typename Glyphs::value_type;
        //NOTE The implementation may assume that this parameter is a unique reference to this argument.
        //NOTE Self-move-assignment is not possible
        *first2 = value_type(std::move(*first));
        Views & views = get_views(*first2);
        while (++first < pos) {
            views.insert(views.end(), get_views(*first).begin(), get_views(*first).end());
        }
        std::sort(views.begin(), views.end());
        views.erase(std::unique(views.begin(), views.end()), views.end());
        ++first2;
    }
    cont.erase(first2, last);
}

struct ReindexDatas {
    std::vector<unsigned> const & indexes;
    template<class Strategy>
    void operator()(loader2::Data<Strategy> & data) const {
        auto cont = data.release();
        utils::reindex(indexes, cont);
        data = loader2::Data<Strategy>(cont);
    }
};

template<class Data>
typename Data::value_type const & get_value(Data const & data, size_t i) {
    return data[i];
}


template<class T>
std::ostream & operator<<(std::ostream & os, std::vector<T> const & cont) {
    os << cont.size() << ' ';
    for (auto & x : cont) {
        os << x << ' ';
    }
    return os;
}

template<class T, std::size_t N>
std::ostream & operator<<(std::ostream & os, std::array<T, N> const & arr) {
    for (auto & x : arr) {
        os << x << ' ';
    }
    return os;
}

template<class... Ts>
void print(loader2::Datas<Ts...> const & datas) {
    for (size_t i = 0; i < datas.size(); ++i) {
        (void)std::initializer_list<char>{(void(
            std::cout << get_value(datas.template get<Ts>(), i) << " "
        ), char())...};
        std::cout << "\n";
    }
}

int main(int ac, char **av)
{
    if (ac < 4) {
        std::cerr << av[0] << "output_datas output_glyphs glyph_file [...]\n";
        return 1;
    }

    using Glyphs = loader2::Glyphs;
    using Glyph = loader2::Glyph;
    using Views = loader2::Views;

    Glyphs glyphs;

    for (int i = 3; i < ac; ++i) {
        utils::read_file(glyphs, av[i]);
    }

    reduce(
        glyphs,
        [](Glyph const & g1, Glyph const & g2) { return image_compare(g1.img, g2.img) < 0; },
        [](Glyph const & g1, Glyph const & g2) { return g1.img == g2.img; },
        [](Glyph & g) -> Views & { return g.views; }
    );

    std::cerr << "Glyphs: " << (glyphs.size()) << std::endl;

    PpOcrDatas datas;
    for (auto & glyph : glyphs) {
        datas.load(glyph.img);
    }

    using index_type = unsigned;
    struct integer_iterator
    {
        using iterator_category = std::random_access_iterator_tag;
        using value_type = index_type;
        using difference_type = std::ptrdiff_t;
        using pointer = void;
        using reference = value_type;

        index_type n;
        integer_iterator & operator++() { ++n; return *this; }
        index_type operator*() { return n; }
        difference_type operator-(integer_iterator const & other) const { return this->n - other.n; }
    };
    std::vector<index_type> indexes(
        integer_iterator{index_type{0}},
        integer_iterator{static_cast<index_type>(datas.size())}
    );

//     std::sort(indexes.begin(), indexes.end(), [&](index_type i1, index_type i2) { return datas.lt(i1, i2); });
//     std::cout << (std::unique(indexes.begin(), indexes.end(), [&](index_type i1, index_type i2) { return datas.eq(i1, i2); }) - indexes.begin()) << "\n";

print(datas);

    reduce(
        indexes,
        [&](index_type i1, index_type i2) { return datas.lt(i1, i2); },
        [&](index_type i1, index_type i2) { return datas.eq(i1, i2); },
        [&](index_type i) -> Views & { return glyphs[i].views; }
    );

    utils::reindex(indexes, glyphs);
    glyphs.resize(indexes.size());
    loader2::apply_from_datas(datas, ReindexDatas{indexes});

    std::cerr << "Glyphs: " << glyphs.size() << std::endl;

    throw std::runtime_error("unimplemented");

//     {std::ofstream file(av[0]); file << datas;}
//     {std::ofstream file(av[1]); file << glyphs;}
}
