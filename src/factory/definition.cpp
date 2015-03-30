#include "definition.hpp"

#include <ostream>
#include <istream>


Definition::Definition(std::string c, Image img, DataLoader::Datas data)
: c(std::move(c))
, img(std::move(img))
, datas(std::move(data))
{}

Definition::Definition(std::string c, Image img, const DataLoader& loader)
: c(std::move(c))
, img(std::move(img))
, datas(loader.new_data(this->img, this->img.rotate90()))
{}

void write_definition(std::ostream& os, Definition const & def, const DataLoader& loader)
{
    os << def.c << " " << def.img.width() << " " << def.img.height() << " ";
    os.write(def.img.data(), def.img.area()) << "\n" << loader.writer(def.datas) << "\n";
}

void read_definition(std::istream& is, Definition & def, const DataLoader& loader)
{
    is >> def.c;
    Bounds bnd;
    if (is >> bnd) {
        std::unique_ptr<Pixel[]> p(new Pixel[bnd.area()]);
        //decltype(file)::sentry sentry(file);
        is.rdbuf()->snextc();
        is.read(p.get(), bnd.area());
        def.datas = loader.new_data(is);
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
