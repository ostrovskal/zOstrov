//
// Created by User on 27.04.2023.
//

#include "zostrov/zCommon.h"
#include "zostrov/zViewText.h"

zViewText::zViewText(zStyle *_styles, i32 _id, u32 _text) : zView(_styles, _id) {
    defSpan         = new zTextSpan();
    defPaint        = new zTextPaint();
    dr              = new zDrawable(this, DRW_BK);
    setText(_text);
}

zViewText::~zViewText() {
    SAFE_DELETE(defSpan);
    SAFE_DELETE(defPaint);
    SAFE_DELETE(dr);
    clearCacheSpans(true);
}

void zViewText::setTextSpecial(czs &_text, cszm &spec) {
    realText = _text; measureSpec.set(0, 0);
    clearCacheSpans(true);
    measure(spec);
}

void zViewText::updateText() {
    clearCacheSpans(false);
    auto changed(rview.isNotEmpty());
    if(changed) {
        auto rv(rclient); auto count(drw[DRW_TXT]->bound.h);
        szm spec(zMeasure(lps.w == VIEW_WRAP ? MEASURE_UNDEF : MEASURE_EXACT, rview.w),
                 zMeasure(lps.h == VIEW_WRAP ? MEASURE_UNDEF : MEASURE_EXACT, rview.h));
        onMeasure(spec);
        changed = (rv != rclient || count != drw[DRW_TXT]->bound.h);
    }
    if(changed) {
//        DLOG("changed %s", realText.str());
        requestLayout();
    }
    invalidate();
}

zViewText* zViewText::setText(czs& _text, bool force) {
    if(force || realText != _text) {
        spans.clear(); if(realText.isEmpty()) requestLayout();
        realText = _text; updateText();
    }
    return this;
}

void zViewText::clearCacheSpans(bool del_spans) {
    urls.clear();
    textCache.clear();
    cacheSpans.clear();
    if(del_spans) {
        for(auto sp : spans) SAFE_DELETE(sp->span);
        spans.clear();
    }
}

void zViewText::onMeasure(cszm& spec) {
    auto widthSize(spec.w.size()), heightSize(spec.h.size());
    auto widthMode(spec.w.mode()), heightMode(spec.h.mode());
    int width, height;
    bool icg((icGravity & ZS_GRAVITY_HORZ) != ZS_GRAVITY_HCENTER), fkg((fkGravity & ZS_GRAVITY_HORZ) != ZS_GRAVITY_HCENTER), is(false);
    icSize.empty();
    // отступы
    auto wpad(pad.extent(false)), hpad(pad.extent(true));
    // определить габариты фореграунда/иконки
    auto fkSize(drw[DRW_FK]->resolveSize(widthSize, heightSize, fkGravity));
    if(drw[DRW_ICON]->isValid()) icSize = drw[DRW_ICON]->resolveSize(fkSize.w, fkSize.h, icGravity);
    fkW = fkSize.w + wpad; fkH = fkSize.h + hpad;
    auto icW(icSize.w + wpad), icH(icSize.h + hpad);
    // разбить текст и определить габариты текста в пикселях
    auto offsW(distance + (fkg * fkSize.w) + (icg * icSize.w));
    auto wh(textWrap(getDrawText(false), widthSize - (wpad + ipad.extent(false) + offsW)));
    // установить высоту текста
    drw[DRW_TXT]->bound.h = wh.h;
    // корректировать ширину текста
    if(widthSize && widthMode == MEASURE_EXACT) {
        width = widthSize;
        fkW = z_min(fkW, width);
    } else {
        // базовая ширина + отступ
        width  = wh.w + wpad + ipad.extent(false);
        // добавить ширину картинки/иконки(если она не по-центру)
        if(fkg) { is = true; width += fkW; }
        if(icSize.isNotEmpty() && icg) { is = true; width += icW; }
        if(is) width += (wh.w != 0) * distance - wpad;
        // макс. от ширины картинки
        width  = z_max(minMaxSize.x, z_max(width, fkW));
        width  = z_max(width, icW);
        // коррекция по макс. ширине
        if(minMaxSize.y) width = z_min(minMaxSize.y, width);
        // выполнить ограничение размера
        if(widthSize && widthMode == MEASURE_MOST) width = z_min(width, widthSize);
    }
    // определить высоту текста
    if(heightSize && heightMode == MEASURE_EXACT) {
        height = heightSize;
        fkH = z_min(fkH, height);
    } else {
        // высота текста + отступ
        height = z_max(minMaxSize.w, z_max(wh.h + hpad + ipad.extent(true), fkH));
        height = z_max(height, icH);
        // коррекция по макс. высоте
        if(minMaxSize.h) height = z_min(minMaxSize.h, height);
        // выполнить ограничение размера
        if(heightSize && heightMode == MEASURE_MOST) height = z_min(height, heightSize);
    }
    setMeasuredDimension(width, height);
    // габариты фореграунда
    if(fkg) {
        fkW -= wpad; fkH -= hpad;
    } else {
        fkW = rclient.w; fkH = rclient.h;
    }
}

i32 zViewText::onTouchEvent() {
    int ret(TOUCH_CONTINUE);
   if(urls.isNotEmpty()) {
        if(touch->isCaptured()) {
            for(auto u: urls) {
                if(u->rect.contains((int)touch->cpt.x, (int)touch->cpt.y)) { urlRect = u->rect; break; }
            }
            ret = TOUCH_FINISH;
        }
        auto idx(urls.indexOf(urlRect));
        auto url(idx != -1 ? urls[idx] : nullptr);
        if(url) {
            auto sp(dynamic_cast<zTextSpanUrl*>(url->span->span));
            auto paint(url->span->paint);
            paint->setStyle((paint->getStyle() & ~ ZS_TEXT_UNDERLINE) | (ZS_TEXT_UNDERLINE * !touch->isCaptured()));
            paint->bkColor = (touch->isCaptured() ? bkgHighlight : sp->bkg);
            if(touch->click(urlRect)) {
                sp->click();
                if(onClickUrl) onClickUrl(this, sp->url);
                ret = TOUCH_ACTION;
            }
            if(touch->isReleased()) urlRect.empty();
            invalidate();
            return ret;
        }
    }
    return zView::onTouchEvent();
}

void zViewText::onLayout(crti &position, bool changed) {
    zView::onLayout(position, changed);
    // позиция картинки
    auto bound(&drw[DRW_FK]->bound);
    auto _grav(0), _width(0);
    if((fkGravity & ZS_GRAVITY_HORZ) == ZS_GRAVITY_HCENTER) {
        // габариты иконки
        if((icGravity & ZS_GRAVITY_HORZ) != ZS_GRAVITY_HCENTER) {
            _width = icSize.w + distance * (icSize.w != 0); _grav = icGravity;
        }
    } else {
        // габариты картинки
        _width = fkW + distance * (fkW != 0); _grav = fkGravity;
    }
    drw[DRW_FK]->measure(fkW, fkH, PIVOT_ALL, false);
    *bound = applyGravity(rclient, *bound, fkGravity); *bound += rclient;
    // позиция текста
    auto r(rclient - szi(ipad.extent(false), ipad.extent(true)));
    _grav = ((_grav & ZS_GRAVITY_HORZ) == ZS_GRAVITY_START);
    if((widthRectCache + _width) > r.w) {
        _width *= _grav * 2 - 1;
    } else {
        r.w -= _width; _width *= _grav;
    }
    bound = &drw[DRW_TXT]->bound; bound->w = r.w;
    *bound = applyGravity(r, *bound, gravity); *bound += r;
    bound->x += ipad.x + _width; bound->y += ipad.y;
}

const zViewText::CACHE* zViewText::getStringFromCache(int i, rti* tbound, pti& pos) {
    if(i >= 0 && i < textCache.size()) {
        auto cacheStr(textCache[i]);
        // применить гравитацию
        pos.x = tbound->x + ((tbound->w - cacheStr->width) >> gravity_shift[gravity & 3]);
        //DLOG("x: %i _x:%i w:%i cw:%i g:%X %s", pos.x, tbound->x, tbound->w, cacheStr->width, gravity, cacheStr->text.str());
        return cacheStr;
    }
    return nullptr;
}

int zViewText::drawFragment(cstr txt, int count, zTextPaint* paint, const CACHE* cache, cpti& coord, crti& clip, int subH) {
    // сформировать фрагмент
    auto wpix(paint->width ? paint->width : drw[DRW_TXT]->makeText(txt, count, paint));
    auto screenX(manager->screen.x), screenY(manager->screen.y);
    // отрисовка фона текста(если есть)
    if(dr && (paint->bkColor & 0xfe000000)) {
        dr->measure(wpix, cache->size, 0, false);
        dr->color.set(paint->bkColor);
        m.translate((float)(coord.x - screenX), (float)(coord.y - screenY - subH), 0);
        dr->drawCommon(clip, m, true);
    }
    if(!paint->width) {
        // отрисовка теневого текста
        if(shadow.isNotEmpty()) {
            drw[DRW_TXT]->color.set(colors[TEXT_COLOR_SHADOW]);
            m.translate((float)(coord.x - screenX + shadow.x), (float)(coord.y - screenY + shadow.y), 0);
            drw[DRW_TXT]->drawCommon(clip, m, true);
        }
        // отрисовка текста
        drw[DRW_TXT]->color.set(getDrawColorText(paint->fkColor));
        m.translate((float)(coord.x - screenX), (float)(coord.y - screenY), 0);
        drw[DRW_TXT]->drawCommon(clip, m, true);
    }
    return wpix;
}

void zViewText::drawText() {
    if(textCache.isNotEmpty()) {
        rti clip(z_clipRect(rclip, rti(rclient.x + ipad.x, rclient.y + ipad.y, rclient.w - ipad.extent(false), rclient.h - ipad.extent(true))));
        // длина текста в символах
        int countChars(2); for(auto &t: textCache) countChars += t->text.count();
        // создать буферы вершин(с учетом strike|underline)
        drw[DRW_TXT]->make(countChars * 6);
        // если есть спаны - на специальную отрисовку
        if(spans.isNotEmpty()) drawTextSpan(clip);
        else {
            // отрисовка
            auto tbound(&drw[DRW_TXT]->bound); pti coord(tbound->xy());
            const CACHE* cacheStr; int indexCache(0);
            // взять текущий текст из кэша
            while((cacheStr = getStringFromCache(indexCache++, tbound, coord))) {
                drawFragment(cacheStr->text.str(), cacheStr->text.count(), defPaint, cacheStr, coord, clip);
                // корректировать позицию для следующей подстроки
                coord.x = tbound->x; coord.y += cacheStr->size;
            }
        }
    }
}

cstr zViewText::addCacheSubString(cstr _stext, cstr _text, int& width, int height, bool isEdit) {
    // убрать конечные пробелы
    auto _count(z_sizeCount8(_stext, _text)); int ch;
    if(!isEdit) {
        while(_count &&z_isspace((char)(ch = z_char8(z_ptr8(_stext, _count - 1)))))
            width -= defPaint->getWidthChar(ch), _count--;
    }
    if(_count) textCache += new CACHE(width, height, _stext, _count);
    // пропустить начальные пробелы
    if(!isEdit) { while(z_isspace((char)z_char8(_text, &_count))) _text += _count; }
    return _text;
}

szi zViewText::textWrap(cstr _text, int widthRect) {
    widthRectCache = 0; if(spans.isNotEmpty()) return textWrapSpan(_text, widthRect);
    int maxHeight(0), width(0), lines(getLines()), sepPos(0), sepWidth(0), height(0), _count, _pos;
    // разбить текст по спец. символам по ширине ректа
    auto _width(widthRect);
    if(lines == 1 || widthRect <= 0) widthRect = INT_MAX;
    textCache.clear();
    auto isEdit(dynamic_cast<zViewEdit*>(this));
    // адрес последнего разделителя/начало подстроки/текущий символ
    cstr separator(nullptr), _stext(_text); int ch('A');
    height = z_max(height, defPaint->getSize());
    width += shadow.x + defPaint->getItalic();
    while(true) {
        auto _ch(ch); if(!(ch = z_char8(_text, &_count))) break;
        // если это не начало подстроки и есть разделитель - запоминаем его
        if(_stext != _text && z_delimiter(_ch)) separator = _text, sepWidth = width, sepPos = _pos;
        if(lines == 1 || ch != '\n') {
            // определить новую ширину строки
            auto ln(defPaint->getWidthChar(z_decode8(ch)) + width);
            // если длина подстроки меньше ширины ректа
            if(_stext == _text || lines == 1 || ln < widthRect) { width = ln; _text += _count; _pos++; continue; }
        } else { separator = nullptr; _text++; }
        // добавить подстроку в массив
        if(separator) _text = separator, width = sepWidth, _pos = sepPos, separator = nullptr;
        _text = addCacheSubString(_stext, _text, width, height, isEdit);
        widthRectCache = z_max(width, widthRectCache); maxHeight += height;
        _stext = _text; width = defPaint->getItalic(); lines--;
    }
    // последняя подстрока
    if(_stext < _text) {
        addCacheSubString(_stext, _text, width, height, isEdit);
        widthRectCache = z_max(width, widthRectCache); maxHeight += height;
    }
    // данные ...
    if(ellipsis != ZS_ELLIPSIS_NONE && textCache.isNotEmpty() && lines == 1) {
        auto pt3(defPaint->widthText("...", 3));
        auto c(textCache[0]); auto w(c->width - _width);
        if(w > 0) {
            w += pt3;
            zString8 tmp;
            auto _c(c->text.count()), _c1(_c / 2), l(0), ll(0);
            auto _t(c->text.str()), _t1(z_ptr8(_t, _c1));
            while(w >= 0) {
                switch(ellipsis) {
                    case ZS_ELLIPSIS_END:   ch = z_char8(z_ptr8(_t, --_c)); break;
                    case ZS_ELLIPSIS_START: ch = z_char8(_t, &l); _t += l; break;
                    default:                if(ll++ & 1) ch = z_char8(z_ptr8(_t, --_c1)); else { ch = z_char8(_t1, &l); _t1 += l; } break;
                }
                l = defPaint->getWidthChar(z_decode8(ch));
                c->width -= l; w -= l;
            }
            c->width += pt3;
            switch(ellipsis) {
                case ZS_ELLIPSIS_END:   tmp = c->text.left(_c) + "..."; break;
                case ZS_ELLIPSIS_START: tmp = "..."; tmp += _t; break;
                default:                tmp = c->text.left(_c1) + "..."; tmp += _t1; break;
            }
            c->text = tmp;
        }
    }
    return { widthRectCache, z_max(maxHeight, getTextSize()) };
}

void zViewText::onInit(bool _theme) {
    zView::onInit(_theme);
    setTextColorHighlight(theme->styles->_int(Z_THEME_COLOR_TEXT_HIGHLIGHT, 0xffa0a0a0));
    setTextColorShadow(theme->styles->_int(Z_THEME_COLOR_TEXT_SHADOW, 0xff000000));
    bkgHighlight = theme->styles->_int(Z_THEME_COLOR_SELECTOR, 0xff909090);
    styles->enumerate([this, _theme](u32 attr) {
        auto v(&zTheme::value); auto val(v->u);
        attr |= _theme * ZTT_THM;
        switch(attr) {
            case Z_TEXT_HIGHLIGHT_COLOR:    setTextColorHighlight(val); break;
            case Z_TEXT_SHADOW_COLOR:       setTextColorShadow(val); break;
            case Z_TEXT_SIZE:               setTextSize((int)val); break;
            case Z_TEXT_STYLE:              setTextStyle(val & ZS_MODE_TEXT_MASK); break;
            case Z_TEXT_LINES: 		        setLines((int)val); break;
            case Z_TEXT_SHADOW_OFFS:        setShadowOffset((int)(val & 0xffff), (int)((val >> 16) & 0xffff)); break;
            case Z_TEXT_DISTANCE:           distance    = (int)val; break;
            case Z_TEXT_LENGTH:             maxLength   = (int)val; break;
            case Z_TEXT_ELLIPSIS:           ellipsis    = (int)val; break;
            case Z_ICON_GRAVITY:            setIconGravity(v->u); icGravity &= ~ZS_SCALE_MASK; icGravity |= (v->u & ZS_SCALE_MASK); break;
            case Z_TEXT_SHADER:             val = manager->getShader(theme->findString(val));
            case Z_TEXT_FONT:
            case Z_TEXT_FOREGROUND_COLOR:   drParams[DR_TXT].set(Z_TEXT_FONT, attr, (int)val); break;
            case Z_TEXT_BACKGROUND:
            case Z_TEXT_BACKGROUND_COLOR:
            case Z_TEXT_BACKGROUND_TILES:   drParams[DR_BTXT].set(Z_TEXT_BACKGROUND, attr, (int)val);
            case Z_ICON_SHADER:             val = manager->getShader(theme->findString(val));
            case Z_ICON:
            case Z_ICON_COLOR:
            case Z_ICON_TILES:
            case Z_ICON_PADDING:
            case Z_ICON_SCALE:              drParams[DR_ICON].set(Z_ICON, attr, (int)val);
                                            if(attr == Z_ICON_SCALE) drParams[DR_ICON].scale = v->f; break;
            case Z_ASSET:                   setHtmlText((cstr)zFileAsset(zTheme::value.s, true).readn(), [](cstr, bool, zHtml*) { return false; }); break;
        }
    });
    // шрифт
    setDrawable(&drParams[DR_TXT], DRW_TXT);
    drw[DRW_TXT]->typeTri = GL_TRIANGLES;
    setTextColorDefault(drParams[DR_TXT].color);
    setTextColorForeground(drParams[DR_TXT].color);
    // фон шрифта
    dr->init(drParams[DR_BTXT]);
    defPaint->bkColor = dr->color.toARGB();
    // иконка
    setDrawable(&drParams[DR_ICON], DRW_ICON);
    // сброс краски
    defPaint->reset(this);
    // сброс кэша спанов
    clearCacheSpans(false);
}
