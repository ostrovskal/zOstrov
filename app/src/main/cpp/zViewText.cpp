//
// Created by User on 27.04.2023.
//

#include "zssh/zCommon.h"
#include "zssh/zViewText.h"

zViewText::zViewText(zStyle *_styles, i32 _id, u32 _text) : zView(_styles, _id) {
    defSpan         = new zTextSpan();
    defPaint        = new zTextPaint();
    dr              = new zDrawable(this, DRW_BK);
    minMaxSize.x    = z_dp(z.R.dimen.textMinWidth);
    minMaxSize.w    = z_dp(z.R.dimen.textMinHeight);
    setText(_text);
    // значения по умолчанию
    setShadowOffset(2, 2);
}

zViewText::~zViewText() {
    SAFE_DELETE(defSpan);
    SAFE_DELETE(defPaint);
    SAFE_DELETE(dr);
}

void zViewText::setTextSpecial(czs &_text, cszm &spec) {
    realText = _text; measureSpec.set(0, 0);
    clearCacheSpans(false);
    measure(spec);
}

void zViewText::setText(czs& _text, bool force) {
    if(force || realText != _text) {
        auto changed(rview.isNotEmpty());
        realText = _text;
        clearCacheSpans(false);
        if(changed) {
            auto rv(rview);
            szm spec(zMeasure(lps.w == VIEW_WRAP ? MEASURE_UNDEF : MEASURE_EXACT, measureSpec.w.size()),
                     zMeasure(lps.h == VIEW_WRAP ? MEASURE_UNDEF : MEASURE_EXACT, measureSpec.h.size()));
            onMeasure(spec);
            changed = (rv != rview);
        }
        if(changed) requestLayout();
    }
}

void zViewText::clearCacheSpans(bool del_spans) {
    cacheSpans.clear();
    cachePaints.clear();
    textCache.clear();
    if(del_spans) {
        for(auto &sp : spans) {
            SAFE_DELETE(sp->paint);
            SAFE_DELETE(sp->span);
        }
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

void zViewText::onLayout(crti &position, bool changed) {
    zView::onLayout(position, changed);
    if(changed) {
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
        drw[DRW_FK]->measure(fkW, fkH, 3, false);
        *bound = applyGravity(rclient, *bound, fkGravity); *bound += rclient;
        // позиция текста
        if(textCache.isNotEmpty()) {
            auto r(rclient - szi(ipad.extent(false), ipad.extent(true)));
            _grav = ((_grav & ZS_GRAVITY_HORZ) == ZS_GRAVITY_START);
            if((widthRectCache + _width) > r.w) {
                _width *= _grav * 2 - 1;
            } else {
                r.w -= _width;
                _width *= _grav;
            }
            bound = &drw[DRW_TXT]->bound; bound->w = r.w;
            *bound = applyGravity(r, *bound, gravity); *bound += r;
            bound->x += ipad.x + _width; bound->y += ipad.y;
        }
    }
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

void zViewText::drawText() {
    static zMatrix m;
    if(textCache.isNotEmpty()) {
        auto tbound(&drw[DRW_TXT]->bound);
        auto screenX(manager->screen.x), screenY(manager->screen.y);
        // длина текста в символах
        int countChars(2); for(auto &t: textCache) countChars += t->text.count();
        // создать буферы вершин(с учетом strike|underline)
        drw[DRW_TXT]->make(countChars * 6);
        // высота текстуры
        auto htex(drw[DRW_TXT]->texture->getSize().h / 2);
        // отрисовка
        pti coord(tbound->x, tbound->y);
        int indexCache(0), indexText(0), wpix, subH;
        rti clip(z_clipRect(rclip, rti(rclient.x + ipad.x, rclient.y + ipad.y, rclient.w - ipad.extent(false), rclient.h - ipad.extent(true))));
        // взять текущий текст из кэша
        auto cacheStr(getStringFromCache(indexCache++, tbound, coord));
        for(auto &sp: cacheSpans) {
            auto posSpan(sp->s); auto paint(sp->paint);
            auto baseLine((int)round((float)paint->getBaseline() * ((float)cacheStr->size / (float)htex) + 0.5f));
            coord.x += paint->getMargin();
            auto _subH(cacheStr->size - paint->getSize());
            // проверка - если спан сам себя рисует
            if(sp->span->draw(coord.x, coord.y, cacheStr->size, paint))
                coord.x += 0;//paint->width;
            while(posSpan < sp->e) {
                // корректировать вертикальную позицию, относительно базовой линии шрифта
                if(_subH) {
                    auto scaleSize((float)paint->getSize() / (float)htex);
                    auto _baseLine((int)round((float)paint->getBaseline() * scaleSize + 0.5f));
                    subH = ((cacheStr->size - baseLine) - (paint->getSize() - _baseLine));
                } else subH = 0;
                // сдвинуть по верт. относительно общей высоты строки
                coord.y += subH;
                // lenText -> либо весь sp, либо весь cacheStr
                auto lenText(z_min(cacheStr->text.count() - indexText, sp->e - posSpan));
                auto txt(cacheStr->text.ptr(indexText));
                // следующая позиция
                indexText += lenText; posSpan += lenText;
                // сформировать фрагмент
                wpix = drw[DRW_TXT]->makeText(txt, lenText, paint);
                // отрисовка фона текста(если есть)
                if(dr) {
                    dr->measure(wpix, cacheStr->size, 0, false);
                    dr->color.set(paint->bkColor);
                    m.translate(coord.x - screenX, coord.y - screenY - subH, 0);
                    dr->drawCommon(clip, m, true);
                }
                // отрисовка теневого текста
                drw[DRW_TXT]->color.set(colors[TEXT_COLOR_SHADOW]);
                m.translate(coord.x - screenX + shadow.x, coord.y - screenY + shadow.y, 0);
                drw[DRW_TXT]->drawCommon(clip, m, true);
                // отрисовка текста
                drw[DRW_TXT]->color.set(getDrawColorText(paint->fkColor));
                m.translate(coord.x - screenX, coord.y - screenY, 0);
                drw[DRW_TXT]->drawCommon(clip, m, true);
                // корректировать позицию для следующего спана
                coord.x += wpix; coord.y -= subH;
                // проверка на конец строки из кэша
                if(indexText >= cacheStr->text.count()) {
                    // корректировать позицию для следующей подстроки
                    coord.x = tbound->x; coord.y += cacheStr->size;
                    if(!(cacheStr = getStringFromCache(indexCache++, tbound, coord))) break;
                    baseLine = (int)round((float)paint->getBaseline() * ((float)cacheStr->size / (float)htex) + 0.5f);
                    indexText = 0;
                }
            }
        }
    }
}

szi zViewText::textWrap(cstr _text, int widthRect) {
    static CACHE tcache; static zStringUTF8 ellText;
    int _ellipsis(INT_MAX), pt3(0), width(0), maxHeight(0), maxWidth(0), sepPos(0), sepWidth(0), height(0), _count;
    // проверить на кэшированные значения
    if(textCache.isNotEmpty() && widthRect >= widthRectCache) {
//        DLOG("from cache %s", textCache[0]->text.str());
        for(auto& cache : textCache) maxHeight += cache->size;
        return { widthRectCache, maxHeight };
    }
    // разбить текст по спец. символам по ширине ректа
    if(testFlags(ZS_ELLIPSIS)) {
        _ellipsis = widthRect;
        ellText = _text; _text = ellText.str();
    }
    if(widthRect <= 0 || isWrap()) widthRect = INT_MAX;
    textCache.clear();
    auto isEdit(dynamic_cast<zViewEdit*>(this));
    // адрес последнего разделителя/начало подстроки/текущий символ
    cstr separator(nullptr), _stext(_text); int ch;
    // массив спанов
    mergeSpans(_text);
    // идти по всем спанам
    for(auto& sp : cacheSpans) {
        auto paint(sp->paint); auto _pos(sp->s), _end(sp->e);
        if(testFlags(ZS_ELLIPSIS)) pt3 = paint->getWidthChar('.') * 3;
        height = z_max(height, paint->getSize());
        width += paint->getMargin() + paint->getItalic();
        // корректировать длину в пикселях, если длина нулевая
        _end += (_pos == _end);
        while(_pos < _end) {
            if(!(ch = z_decodeUTF8(z_charUTF8(_text, &_count)))) break;
            // если это не начало подстроки и есть разделитель - запоминаем его
            if(_stext != _text && z_delimiter(ch)) separator = _text, sepWidth = width, sepPos = _pos;
            if(isWrap() || ch != '\n') {
                // определить новую ширину строки
                auto ln(paint->getWidthChar(ch) + width);
                // проверка на многоточие
                if((ln + pt3) >= _ellipsis) {
                    width += pt3; ellText = ellText.left(z_sizeCountUTF8(_stext, _text)) + "...";
                    _stext = ellText.str(); _text = ellText.ptr();
//                    DLOG("ellipsis %i %i %i %s", _ellipsis, width, sz, ellText.str());
                    break;
                }
                // если длина подстроки меньше ширины ректа и символ не "новая строка" = дальше
                if(_stext == _text || ln < widthRect) { width = ln; _text += _count; _pos++; continue; }
            } else { separator = nullptr; }
            // добавить подстроку в массив
            if(separator) _text = separator, width = sepWidth, _pos = sepPos, separator = nullptr;
            // убрать конечные пробелы
            _count = z_sizeCountUTF8(_stext, _text);
            if(!isEdit) while(z_isspace(z_charUTF8(z_ptrUTF8(_stext, _count - 1)))) _count--, width -= 10;
            if(width) textCache += new CACHE(width, height, _stext, _count);
            maxWidth = z_max(width, maxWidth); maxHeight += height;
            // пропустить начальные пробелы
            if(!isEdit) { while(z_isspace(z_charUTF8(_text, &_count))) _text += _count; }
            _stext = _text; width = paint->getItalic();
        }
    }
    // последняя подстрока
    if(_stext < _text) {
        textCache += new CACHE(width, height, _stext, z_sizeCountUTF8(_stext, _text));
        maxWidth = z_max(width, maxWidth); maxHeight += height;
    }
    widthRectCache = maxWidth;
    return { maxWidth, maxHeight };
}

void zViewText::onInit(bool _theme) {
    zView::onInit(_theme);
    setTextColorHighlight(theme->styles->_int(Z_THEME_COLOR_TEXT_HIGHLIGHT, 0xffa0a0a0));
    setTextColorShadow(theme->styles->_int(Z_THEME_COLOR_TEXT_SHADOW, 0xff000000));
    styles->enumerate([this, _theme](u32 attr) {
        auto v(&zTheme::value); auto val(v->u);
        attr |= _theme * ZTT_THM;
        switch(attr) {
            case Z_TEXT_HIGHLIGHT_COLOR:    setTextColorHighlight(val); break;
            case Z_TEXT_SHADOW_COLOR:       setTextColorShadow(val); break;
            case Z_TEXT_SIZE:               setTextSize(val); break;
            case Z_TEXT_STYLE:              setTextStyle(val & ZS_MODE_TEXT_MASK); break;
            case Z_TEXT_NOWRAP: 		    setWrap(val != 0); break;
            case Z_TEXT_SHADOW_OFFS:        setShadowOffset(val & 0xff, (val >> 8) & 0xff); break;
            case Z_TEXT_DISTANCE:           distance    = (int)val; break;
            case Z_TEXT_LENGTH:             maxLength   = val; break;
            case Z_ICON_GRAVITY:            setIconGravity(v->u); icGravity &= ~ZS_SCALE_MASK; icGravity |= (v->u & ZS_SCALE_MASK); break;
            case Z_TEXT_FONT:
            case Z_TEXT_FOREGROUND_COLOR:   drParams[DR_TXT].set(Z_TEXT_FONT, attr, val); break;
            case Z_TEXT_BACKGROUND:
            case Z_TEXT_BACKGROUND_COLOR:
            case Z_TEXT_BACKGROUND_TILES:   drParams[DR_BTXT].set(Z_TEXT_BACKGROUND, attr, val);
            case Z_ICON:
            case Z_ICON_COLOR:
            case Z_ICON_TILES:
            case Z_ICON_PADDING:
            case Z_ICON_SCALE:              drParams[DR_ICON].set(Z_ICON, attr, val);
                                            if(attr == Z_ICON_SCALE) drParams[DR_ICON].scale = v->f; break;
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

void zViewText::mergeSpans(czs& _text) {
    // если кэш уже есть - выйти
    if(cacheSpans.isNotEmpty()) return;
    int pos(0);
    if(spans.isNotEmpty()) {
        // переписать все спаны в кэш
        for(auto& s : spans) cacheSpans += new SPAN(s->span, s->s, s->e, addCache(s->paint));
        // слить диапазоны, со слиянием характеристик
        for(int i = 0 ; i < cacheSpans.size(); i++) {
            auto tsp1(cacheSpans[i]); auto s1(tsp1->s), e1(tsp1->e);
            //if(tsp->f) continue;
            for(int j = i + 1 ; j < cacheSpans.size(); j++) {
                if(i == j) continue;
                auto tsp2(cacheSpans[j]); auto s2(tsp2->s), e2(tsp2->e);
                // не пересекаются
                if(s1 >= e2 || e1 <= s2) continue;
                auto p(tsp1->paint); auto sp(tsp1->span);
                if(s1 == s2 && e1 == e2) {
                    // идентичны
                    sp->updateState(tsp2->paint);
                    cacheSpans.erase(i--, 1); break;
                }
                auto is1(s1 > s2 && s1 < e2), is2(e1 > s2 && e1 < e2);
                auto ns(0), ne(0), upd(1); bool url(tsp2->span->typeId() == SPAN_URL);
                if(is1 && is2) {
                    // меньше
                    if(url) { cacheSpans.erase(i--, 1, false); break; }
                    tsp2->e = s1; if(s2 != e2) tsp2->span->updateState(p);
                    ns = e1; ne = e2; upd = 0; p = tsp2->paint; sp = tsp2->span;
                } else if(is1) {
                    // начало
                    if(url) {
                        if(e1 != e2) { s1 = e2; tsp1->s = s1; continue; }
                        else { cacheSpans.erase(i--, 1); break; }
                    } else {
                        tsp2->e = s1;
                        if(e1 != e2) { ns = s1; ne = e2; s1 = e2; tsp1->s = s1; }
                    }
                } else if(is2) {
                    // конец
                    if(url) {
                        if(s1 != s2) { e1 = s2; tsp1->e = e1; continue; }
                        else { cacheSpans.erase(i--, 1); break; }
                    } else {
                        tsp2->s = e1;
                        if(s1 != s2) { ns = s2; ne = e1; e1 = s2; tsp1->e = e1; }
                    }
                } else {
                    // больше
                    if(s2 != e2) tsp1->span->updateState(tsp2->paint);
                    cacheSpans.erase(i--, 1, true);
                    if(s1 != s2) cacheSpans.insert(j++, new SPAN(sp, s1, s2, addCache(p)));
                    if(e1 != e2) cacheSpans.insert(j, new SPAN(sp, e2, e1, addCache(p)));
                    break;
                }
                if(ns || ne) {
                    auto tmp(new SPAN(sp, ns, ne, addCache(p)));
                    cacheSpans.insert(j, tmp); p = tmp->paint;
                    j = 0;
                }
                if(upd) tsp2->span->updateState(p);
            }
        }
        // отсортировать по возрастанию
        for(int i = 0; i < cacheSpans.size(); i++) {
            bool isStop(true);
            for(int j = 0; j < cacheSpans.size() - 1; j++) {
                auto s1(cacheSpans[j]), s2(cacheSpans[j + 1]);
                if(s1->s > s2->s) {
                    isStop = false;
                    cacheSpans[j] = s2; cacheSpans[j + 1] = s1;
                }
            }
            if(isStop) break;
        }
        // добавить спан по умолчанию в разрывы
        for(int i = 0; i < cacheSpans.size(); i++) {
            auto sp(cacheSpans[i]);
            if(pos < sp->s) cacheSpans.insert(i, new SPAN(defSpan, pos, sp->s, defPaint));
            pos = sp->e;
        }
    }
    // последний спан
    auto len(_text.count());
    if(len && pos < len) cacheSpans += new SPAN(defSpan, pos, len, defPaint);
}

zViewText::SPAN* zViewText::getSpan(int start, int end) const {
/*
    return std::any_of<zViewText::SPAN, zViewText::SPAN>(std::begin(spans), std::end(spans), [start, end](const auto &s) {
        return ((s->s >= start && s->e <= end) ? s : nullptr);
    });
*/
    for(auto& s : spans) { if(s->s >= start && s->e <= end) return s; }
    return nullptr;
}

void zViewText::setSpan(zTextSpan* _span, int start, int end, int flags) {
    auto len(realText.count());
    start = z_max(0, start); end = z_min(len ? len : INT_MAX, end);
    len = end - start;
    if(len < 0) { delete _span; return; }
    if(flags == SPAN_FLAGS_DEFAULT && len == 0) {
        // найти последний c флагом SPAN_FLAGS_MARK
        for(int i = spans.size() - 1; i >= 0; i--) {
            auto sp(spans[i]);
            if(sp->f != SPAN_FLAGS_MARK) continue;
            if(sp->span->typeId() != _span->typeId()) continue;
            sp->f = flags; sp->e = end;
            delete _span;
            return;
        }
    }
    auto tspan(new SPAN(_span, start, end, new zTextPaint(defPaint), flags));
    _span->updateState(tspan->paint);
    spans += tspan;
}

void zViewText::delSpan(zTextSpan* _span) {
    auto idx(spans.indexOf(_span));
    if(idx != -1) spans.erase(idx, 1, true);
}

bool zViewText::setHtmlText(czs& text, const std::function<bool(cstr tag, bool end, zHtml* html)>& parser) {
    clearCacheSpans(true);
    realText.empty(); bool listOrdered(true); int listNum(1), listRev(1);
    zHtml html(text.str(), [this, &parser, &listOrdered, &listNum, &listRev](cstr tag, bool end, zHtml* _html) {
        static float htmlHeaderSize[] = { 1.7f, 1.6f, 1.5f, 1.4f, 1.3f, 1.2f };
        zTextSpan* span(nullptr);
        if(!parser(tag, end, _html)) {
            // идентификация спанов
            auto hash(z_hash(tag, z_strlen(tag)));
            auto flags(end ? SPAN_FLAGS_DEFAULT : SPAN_FLAGS_MARK);
            auto pos(_html->text.count());
            switch(hash) {
                // big
                case 0xb79e14f3: span = new zTextSpanRelativeSize(1.5f); break;
                // small
                case 0xbf4847de: span = new zTextSpanRelativeSize(0.8f); break;
                // br
                case 0x221bed08: _html->text += "\n"; break;
                // p div
                case 0x3ee0e:
                case 0x79832919:
                    if(_html->text.isNotEmpty() && !end) {
                        if(_html->text[pos] != '\n') { _html->text += "\n"; pos++; }
                        span = new zTextSpanParagraph();
                        flags = SPAN_FLAGS_SPECIFIC;
                    }
                    break;
                // h1 - h6
                case 0xc7ada38:
                case 0xc7c22dd:
                case 0xc7d9a75:
                case 0xc7aa490:
                case 0xc7cf97d:
                case 0xc79a5b2:
                    if(_html->text.isNotEmpty() && _html->text[pos] != '\n') { _html->text += "\n"; pos++; }
                    _html->text += "\n"; pos++;
                    span = new zTextSpanRelativeSize(htmlHeaderSize[tag[1] - '1']);
                    break;
                // b strong
                case 0x50e75:
                case 0x8e4b388b:span = new zTextSpanStyle(ZS_TEXT_BOLD); break;
                // i em
                case 0x6b5:
                case 0xb073db:  span = new zTextSpanStyle(ZS_TEXT_ITALIC); break;
                // u
                case 0x6ae4b:   span = new zTextSpanUnderline(); break;
                // s strike
                case 0x51bdf:
                case 0xef748f41:span = new zTextSpanStrikeline(); break;
                // img
                case 0x9ae16064:
//                    _html->text += '!';
                    span = new zTextSpanImage(_html->getStringAttr("src", "znull"),
                                              _html->getIntegerAttr("tile", RADIX_DEC, 0),
                                              _html->getFloatAttr("factor", 1.0f),
                                              _html->getIntegerAttr("pad", RADIX_DEC, 0));
                    flags = SPAN_FLAGS_SPECIFIC;
                    break;
                // a
                case 0x58de4:   span = new zTextSpanUrl(_html->getStringAttr("href", "self")); break;
                // title
                case 0x6f58b184:span = new zTextSpanAbsoluteSize(10); break;
                // sub
                case 0x80391c79:span = new zTextSpanSubscript(); break;
                // sup
                case 0x803c7044:span = new zTextSpanSuperscript(); break;
                // font
                case 0x89c2d575:
                    break;
                // ul ol
                case 0x2c4b9944:
                case 0x1466284c:
                    listOrdered = (hash == 0x1466284c);
                    listNum = _html->getIntegerAttr("start", RADIX_DEC, 1);
                    listRev = _html->getIntegerAttr("reversed", RADIX_DEC, 0);
                    break;
                // li
                case 0x13844afb:
                    if(!end) {
                        if(_html->text.isNotEmpty() && _html->text[pos] != '\n') { _html->text += "\n"; pos++; }
                        span = new zTextSpanBullet(listOrdered, listNum);
                        flags = SPAN_FLAGS_SPECIFIC;
                        listNum += !listRev * 2 - 1;
                    }
                    break;
                // bkg background
                case 0xc9b77622:
                case 0x1aaadb91: span = new zTextSpanBackgroundColor(_html->getColorAttr("value", -1)); break;
                // c color
                case 0xdbf89687:
                case 0x4595e:   span = new zTextSpanForegrounColor(_html->getColorAttr("value", -1)); break;
                default: break;
            }
            if(span) setSpan(span, pos, pos, flags);
        }
        return span != nullptr;
    });
    setText(html.text.str(), true);
    // убрать все спаны с пометной mark
    for(int i = 0; i < spans.size(); i++) {
        if(spans[i]->f == SPAN_FLAGS_MARK) {
            spans.erase(i--, 1, true);
        }
    }
    return true;
}
