#include "definition.hpp"

#include <ostream>
#include <istream>

namespace ppocr {

Definition::Definition(std::string c, std::string font_name, Image img, DataLoader::Datas data)
: c(std::move(c))
, font_name(std::move(font_name))
, img(std::move(img))
, datas(std::move(data))
{}

Definition::Definition(std::string c, std::string font_name, Image img, const DataLoader& loader)
: c(std::move(c))
, font_name(std::move(font_name))
, img(std::move(img))
, datas(loader.new_datas(this->img, this->img.rotate90()))
{}

void write_definition(std::ostream& os, Definition const & def, const DataLoader& loader)
{
    os << def.c << " " << def.font_name << " " << def.img.bounds() << " ";
    os.write(def.img.data(), def.img.area()) << "\n" << loader.writer(def.datas) << "\n";
}

void read_definition(std::istream& is, Definition & def, const DataLoader& loader)
{
    Bounds bnd;
    if (is >> def.c >> def.font_name >> bnd) {
        std::unique_ptr<Pixel[]> p(new Pixel[bnd.area()]);
        //decltype(file)::sentry sentry(file);
        is.rdbuf()->snextc();
        is.read(p.get(), bnd.area());
        def.datas = loader.new_datas(is);
        def.img = Image(bnd, std::move(p));
    }
}

std::vector<Definition> read_definitions(std::istream& is, DataLoader const & loader)
{
    std::vector<Definition> definitions;
    Definition def;
    while (is) {
        read_definition(is, def, loader);
        if (def) {
            definitions.push_back(std::move(def));
        }
    }
    return definitions;
}

}
