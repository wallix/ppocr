

template<class F>
vcomponent_type horizontal_projection(component_type const * cs, size_t w, size_t h, F f)
{
    vcomponent_type v(w*h);
    auto it = v.begin();
    while (h--) {
        bool is_find = false;
        for (auto & c : falcon::range(cs, w)) {
            f(it, c, is_find);
            ++it;
        }
        cs += w;
    }
    return v;
}

vcomponent_type left_projection(component_type const * cs, size_t w, size_t h)
{
    return horizontal_projection(cs, w, h, [](auto it, auto & c, bool & is_find) {
        if (isc(c)) {
            if (is_find) {
                *it = ncc;
            }
            else {
                *it = c;
                is_find = true;
            }
        }
        else {
            *it = c;
            is_find = false;
        }
    });
}

vcomponent_type right_projection(component_type const * cs, size_t w, size_t h)
{
    return horizontal_projection(cs, w, h, [](auto it, auto & c, bool & is_find) {
        if (isc(c)) {
            *it = ncc;
            is_find = true;
        }
        else {
            if (is_find) {
                *(it-1) = cc;
                *it = c;
                is_find = false;
            }
            else {
                *it = c;
            }
        }
    });
}

template<class F>
vcomponent_type vertical_projection(component_type const * cs, size_t w, size_t h, F f)
{
    vcomponent_type v(w*h);
    auto it = v.begin();
    for (auto tmpw = w; tmpw--; ) {
        auto vit = it;
        bool is_find = false;
        for (auto & c : falcon::range(cs, w*h, w)) {
            f(vit, c, is_find);
            vit += w;
        }
        ++cs;
        ++it;
    }
    return v;
}

vcomponent_type top_projection(component_type const * cs, size_t w, size_t h)
{
    return vertical_projection(cs, w, h, [](auto it, auto & c, bool & is_find) {
        if (isc(c)) {
            if (is_find) {
                *it = ncc;
            }
            else {
                *it = c;
                is_find = true;
            }
        }
        else {
            *it = c;
            is_find = false;
        }
    });
}

vcomponent_type bottom_projection(component_type const * cs, size_t w, size_t h)
{
    return vertical_projection(cs, w, h, [w](auto it, auto & c, bool & is_find) {
        if (isc(c)) {
            *it = ncc;
            is_find = true;
        }
        else {
            if (is_find) {
                *(it-w) = cc;
                *it = c;
                is_find = false;
            }
            else {
                *it = c;
            }
        }
    });
}

void projection(std::ostream & os, component_type const * components, size_t w, size_t h)
{
    auto vleft = left_projection(components, w, h);
//     print_image(os, vleft.data(), w, h);

    auto vright = right_projection(components, w, h);
//     print_image(os, vright.data(), w, h);

    auto vtop = top_projection(components, w, h);
//     print_image(os, vtop.data(), w, h);

    auto vbottom = bottom_projection(components, w, h);
//     print_image(os, vbottom.data(), w, h);

    vcomponent_type vsum = vleft;
    auto sum = [&](vcomponent_type & vsum, vcomponent_type const & v) {
        auto it = vsum.begin();
        for (auto & c : v) {
            if (isc(c)) {
                *it = c;
            }
            ++it;
        }
    };
    sum(vsum, vright);
    sum(vsum, vtop);
    sum(vsum, vbottom);

    const size_t count_projection = 5;
    vcomponent_type v(h*w*count_projection);
    for (auto y : falcon::range(size_t{}, h)) {
        std::copy(vleft.begin() + y*w, vleft.begin() + (y+1)*w,     v.begin()+(y*count_projection+0)*w);
        std::copy(vright.begin() + y*w, vright.begin() + (y+1)*w,   v.begin()+(y*count_projection+1)*w);
        std::copy(vtop.begin() + y*w, vtop.begin() + (y+1)*w,       v.begin()+(y*count_projection+2)*w);
        std::copy(vbottom.begin() + y*w, vbottom.begin() + (y+1)*w, v.begin()+(y*count_projection+3)*w);
        std::copy(vsum.begin() + y*w, vsum.begin() + (y+1)*w,       v.begin()+(y*count_projection+4)*w);
    }
    print_image(os, v.data(), w*count_projection, h);
}

