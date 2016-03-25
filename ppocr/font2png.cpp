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
*   Copyright (C) Wallix 2010-2015
*   Author(s): Jonathan Poelen
*/

#include <QApplication>
#include <QPainter>
#include <QLabel>
//#include <QDebug>

#include <iostream>
#include <cstring>
#include <cerrno>

int main(int argc, char **argv)
{
    if (argc < 4) {
        std::cout << argv[0] << " text-utf8 out-basefile font_files [...]\n";
        return 1;
    }
    QApplication app(argc, argv);

    QString text = ' ' + QString::fromUtf8(argv[1]) + ' ';

    QLabel label(text);
    label.setStyleSheet("background: #000; color: #fff");

    auto mkimg = [&](QFont const & font, QString const & name) {
        label.setFont(font);
        QFontMetrics fm(font);
        label.setGeometry(0, 0, fm.width(text), fm.height() + 10);
        QImage img(label.size(), QImage::Format_RGB32);
        QPainter painter(&img);
        label.render(&painter);
        if (!img.save(name)) {
            std::cout << "write " << name.toStdString() << " " << strerror(errno) << "\n";
        }
    };

    QString font_name;
    QString outfile = argv[2];
    int const lenbasefile = outfile.size();

    for (int i = 2; i < argc; ++i) {
        std::cout << argv[i] << "\n";
        font_name = argv[i];
        for (int sz = 8; sz < 13; ++sz) {
            QFont font(font_name, sz);

            // filename
            {
                int const index = font_name.lastIndexOf('/');
                if (index != -1) {
                    font_name.remove(0, index+1);
                }
            }
            // remove extension
            {
                int const index = font_name.lastIndexOf('.');
                if (index != -1) {
                    font_name.resize(index);
                }
            }
            outfile += font_name;
            outfile += '-';
            outfile += QString::number(sz);

            outfile += "-aliasing.png";

            mkimg(font, outfile);
            font.setStyleStrategy(QFont::NoAntialias);
            outfile.remove(outfile.size() - 13, 9);
            mkimg(font, outfile);

            outfile.resize(lenbasefile);
        }
    }

    app.quit();
    return 0;
}
