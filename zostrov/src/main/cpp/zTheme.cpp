//
// Created by User on 18.04.2023.
//

#include "zostrov/zCommon.h"

// менеджер стилей(тема)
zTheme* theme(nullptr);
zTheme::data zTheme::value{nullptr};

zTheme::zTheme() {
    theme = this;
}

static zResource* find_res(zResource** res, czs& lng, u32 attr) {
    if(res) {
        zResource* r;
        while((r = (*res++)) != nullptr) {
            if(r->v == lng) {
                r++;
                do {
                    if(attr == (r->a & ZTA_MASK))
                        return r;
                } while(!(r++->a & ZT_END));
                break;
            }
        }
    }
    return nullptr;
}

 cstr zTheme::getResource(u32 _value) const {
    zResource* res;
    // ищем ресурсы с языком системы
    if(!(res = find_res(user_resources, manager->languages, _value))) {
        res = find_res(z_resources_ptr_arrays, manager->languages, _value);
    }
    if(!res) {
        // ищем ресусры по умолчанию
        if(!(res = find_res(user_resources, "", _value))) {
            res = find_res(z_resources_ptr_arrays, "", _value);
        }
    }
    return (res ? res->v : "");
}

zStyle* zTheme::findStyles(u32 _value) const {
    auto _styles((_value & 0x00100000) ? (((_value & ZTV_MASK) < (sizeof(z_styles) / sizeof(zResource))) ? z_styles : nullptr) : user_styles);
    return (_styles ? _styles[_value & ZTV_MASK].s : nullptr);
}

const zStyle* zStyle::findAttribute(u32 _attr) const {
    auto cur(this); u32 m(_attr & ZTA_MASK); const zStyle* ret(nullptr);
    do {
        auto _a(cur->a & ZTA_MASK);
        if(_a == Z_STYLES) {
            auto styles(theme->findStyles(cur->v));
            if(styles) ret = styles->findAttribute(_attr);
        } else {
            if(m == _a) { ret = cur; break; }
        }
    } while(!(cur++->a & ZT_END));
    return (ret ? resolveAttribute(ret) : nullptr);
}

const zStyle* zStyle::resolveAttribute(const zStyle* _style) {
    auto _attr(_style->a);
    // сбросить все значение в ноль
    zTheme::value.u = 0;
    if(_attr & ZT_THEME) {
        // ищем в теме
        return theme->findAttribute(_style->v);
    } else if(_attr & ZTT_STR) {
        // ищем в ресурсах
        zTheme::value.s = theme->getResource(_style->v);
        return _style;
    }
    // инициализировать атрибут
    zTheme::fillValue(_style);
    return _style;
}

void zStyle::enumerate(const std::function<void(u32)>& act) const {
    auto _style(this);
    do {
        auto _a(_style->a & ZTA_MASK);
        if(_a == Z_STYLES) {
            auto styles(theme->findStyles(_style->v));
            if(styles) styles->enumerate(act);
        } else {
            resolveAttribute(_style);
            act(_a);
        }
    } while(!(_style++->a & ZT_END));
}

const zStyle* zTheme::findAttribute(u32 attr) const {
    return styles->findAttribute(attr);
}

void zTheme::fillValue(const zStyle* _style) {
    static rti rect;
    auto val(_style->v), attr(_style->a & ZTT_MASK); auto v(&value);
    switch(attr) {
        case ZTT_INT: v->u  = val; break;
        case ZTT_DMN: v->u  = z_dp((int)val); break;
        case ZTT_FLT: v->f = (float)val / 65535.0f; break;
        case ZTT_VEC: rect.set(val); v->u = z_dp(rect.h) | (z_dp(rect.w) << 8) | (z_dp(rect.y) << 16) | (z_dp(rect.x) << 24);
    }
}

bool zTheme::setTheme(zStyle* _styles, czs& lang, zResource** _user_resources, zStyles* _user_styles) {
    auto ret(false);
    if(_styles) {
        prefixes = lang.split("-");
        styles = _styles; user_resources = _user_resources; user_styles = _user_styles;
        auto val(_styles->_str(Z_THEME_NAME));
        if(val) {
            ret = (themeName != val);
            themeName = val;
            themeColor = _styles->_int(Z_THEME_COLOR, 0xffffffff);
        } else {
            themeName.empty();
            ILOG("Недопустимая структура темы!");
        }
    }
    return ret;
}

u32 zStyle::_int(u32 _attr, u32 def) const {
    if(_attr & (ZTT_INT | ZTT_DMN | ZTT_VEC)) {
        enumerate([_attr, &def](u32 _a) {
            if(_a == _attr) def = zTheme::value.u;
        });
    }
    return def;
}

float zStyle::_float(u32 _attr, float def) const {
    if(_attr & ZTT_FLT) {
        enumerate([_attr, &def](u32 _a) {
            if(_a == _attr) def = zTheme::value.f;
        });
    }
    return def;
}

cstr zStyle::_str(u32 _attr, cstr def) const {
    if(_attr & ZTT_STR) {
        enumerate([_attr, &def](u32 _a) {
            if(_a == _attr) def = zTheme::value.s;
        });
    }
    return def;
}
