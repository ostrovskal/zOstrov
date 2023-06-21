//
// Created by User on 19.04.2023.
//

#include "zssh/zCommon.h"
#include "zssh/zSpanText.h"
#include "zssh/zViewText.h"

zTextPaint::zTextPaint() {
    font = manager->loadResourceTexture(theme->styles->_int(Z_THEME_FONT, z.R.drawable.fontDefault), nullptr);
    setStyle(ZS_TEXT_NORMAL);
    setSize(theme->styles->_int(Z_THEME_SIZE_TEXT_TEXT, 20_dp));
}

zTextPaint::~zTextPaint() {
    manager->cache->recycle(font);
}

void zTextPaint::init(zTextPaint* _paint) {
    margin  = _paint->margin;
    fkColor = _paint->fkColor;
    bkColor = _paint->bkColor;
    italic  = _paint->italic;
    baseLine= _paint->baseLine;
    font    = manager->cache->get(_paint->font->name, font);
    setStyle(_paint->style);
    setSize(_paint->size);
}

void zTextPaint::reset(zViewText* _tv) {
    fkColor = _tv->getTextColorForeground();
    bkColor = _tv->getTextColorBackground();
    baseLine= _tv->getBaseline();
    font    = manager->cache->get(_tv->drw[DRW_TXT]->texture->name, font);
    setStyle(_tv->getTextStyle());
    setSize(_tv->getTextSize());
}

zStringUTF8 zTextPaint::info() const {
    return z_fmt("fk:%x bk:%x sts:%x sz:%i marg:%i bs:%i wdt:%i", fkColor, bkColor, getStyle(), getSize(), getMargin(), baseLine, width).str();
}

void zTextPaint::setSize(i32 _size) {
    size   = _size;
    factor = ((float)_size / (float)getSizeFont());
}

void zTextPaint::setStyle(u32 _style) {
    style   = _style;
    italic  = (int)((style & ZS_TEXT_ITALIC) * 4);
    bold    = 224 * ((style & ZS_TEXT_BOLD) != 0);
    yBold   = getSizeFont() * (bold != 0);
}

bool zTextPaint::getBoundChar(int ch, rti& tex, rti& bound) const {
    if(ch < '!') ch = ' ';
    auto glyph(font->paramGlyph(ch + bold));
    if(glyph) {
        tex.set(glyph); tex.w += tex.x; tex.h += tex.y;
        auto cw((int)round((float)glyph[2] * factor));
        if(ch == ' ') cw = 10.0f;
        bound.set(0, (int)round((float)(glyph[1] - yBold) * factor), cw, (int)round((float)glyph[3] * factor));
    }
    return glyph != nullptr;
}

int zTextPaint::correctBaseline(int _size) const {
    if(!(_size - getSize())) return 0;
    auto htex((float)getSizeFont());
    auto bs = (float)getBaseline() * ((float)_size / (float)(htex + 0.5f));
    auto _bs((float)getBaseline() * ((float)getSize() / (float)(htex + 0.5f)));
    return ((_size - bs) - (getSize() - _bs));
}

int zTextPaint::widthText(cstr _text, int length) const {
    int len, _width(0);
    while(length-- > 0 && z_isUTF8(_text)) {
        _width += getWidthChar(z_decodeUTF8(z_charUTF8(_text, &len)));
        _text += len;
    }
    return _width;
}

int zTextPaint::indexOf(cstr _text, int length, int screenLimit, int screenX, bool exact) const {
    int wChar(0), _count(0), len;
    while(z_isUTF8(_text) && length-- > 0) {
        // определить ширину символа
        wChar = getWidthChar(z_decodeUTF8(z_charUTF8(_text, &len)));
        // проверить на лимит
        if((screenX + wChar) >= screenLimit) break;
        screenX += wChar; _text += len; _count++;
    }
    // определить по ширине символа куда мы ближе
    int delta(0);
    if(z_isUTF8(_text)) delta = (exact ? (screenX + (wChar / 2) < screenLimit) : 1);
    return _count + delta;
}

int zTextPaint::indexReverseOf(cstr _text, int length, int screenLimit) const {
    int _width(0), _count(0);
    while(z_isUTF8(_text) && length > 0) {
        // определить ширину символа
        _width += getWidthChar(z_decodeUTF8(z_charUTF8(z_ptrUTF8(_text, --length))));
        // проверить на лимит
        if(_width >= screenLimit) break;
        _count++;
    }
    return _count;
}

zTextSpanImage::zTextSpanImage(int image, int tile, float factor, int _color) {
    dr.init(image, _color, tile, 0, factor);
}

void zTextSpanImage::draw(int x, int y, int hmax, zTextPaint *paint, crti& clip) {
    static zMatrix m;
    rti r(x + 2_dp, y, paint->width - 4_dp, paint->getSize());
    auto screenX(manager->screen.x), screenY(manager->screen.y);
    m.translate(r.x - screenX, r.y - screenY, 0);
    dr.measure(r.w, r.h, 0, false);
    dr.drawCommon(clip, m, true);
}

void zTextSpanImage::updateState(zTextPaint *paint) {
    rti t(dr.texture->getTile(dr.tile));
    paint->width = ((int)round(t.w * dr.scale) + 4_dp);
    paint->setSize((int)round(t.h * dr.scale));
//    paint->setBaseline(paint->getBaseline() + paint->getSize());
}

zTextSpanBullet::zTextSpanBullet(bool _ordered, int _index) : zTextSpanImage(z.R.drawable.zssh, z.R.integer.iconBullet, 0.33f, !_ordered * 2), ordered(_ordered) {
/*
    if(_ordered) {
        strIdx = z_ntos(&_index, RADIX_DEC, true);
        strIdx += ". ";
    }
*/
}

void zTextSpanBullet::updateState(zTextPaint *paint) {
/*
    if(!ordered) {
        zTextSpanImage::updateState(paint);
    } else {
        // определить ширину
        auto tex(paint->texture); auto _text(strIdx.str());
        int width(0); auto factor((float)paint->height / (float)tex->getSize().h);
        while(*_text) width += tex->widthGlyph((u8)*_text++, factor);
        width = z_max(width, image->rfull.w);
        paint->width = width;
    }
    paint->preWidth = 40;
*/
}

void zTextSpanMask::draw(int x, int y, int hmax, zTextPaint *paint, crti &clip) {

}

void zTextSpanBullet::draw(int x, int y, int hmax, zTextPaint *paint, crti& clip) {
/*
    if(!ordered) return zTextSpanImage::draw(x, y, hmax, paint);
    auto htex(paint->texture->getSize().h);
    // выровнять по базовой линии
    auto sc1((float)paint->height / (float)htex), sc2((float)hmax / (float)htex);
    auto bs1((int)round((float)paint->baseLine * sc2 + 0.5f));
    auto bs2((int)round((float)paint->baseLine * sc1 + 0.5f));
    auto subH((hmax - bs1) - (paint->height - bs2));
    rti r(x, y + subH, paint->width, paint->height);
    auto len(strIdx.length()), width(paint->width); paint->width = 0;
    image->drs[zView::DR_IDX_FK]->make(len * 6);
    image->drs[zView::DR_IDX_FK]->texture = vmanager->cache.get(paint->texture->name, image->drs[zView::DR_IDX_FK]->texture);
    image->drs[zView::DR_IDX_FK]->clip = r;
    image->drs[zView::DR_IDX_FK]->color = zColor::shadow;
    image->drs[zView::DR_IDX_FK]->makeText(strIdx, len, r, paint, true);
    image->drs[zView::DR_IDX_FK]->specDraw(false);
    image->drs[zView::DR_IDX_FK]->color.set(paint->fkColor);
    image->drs[zView::DR_IDX_FK]->makeText(strIdx, len, r, paint, false);
    image->drs[zView::DR_IDX_FK]->specDraw(false);
    paint->width = width;
*/
}

void zTextSpanParagraph::updateState(zTextPaint *paint) {
    paint->setMargin(15_dp);
}

zTextSpanUrl::zTextSpanUrl(cstr _url) {
    color = theme->styles->_int(Z_THEME_COLOR_TEXT_URL, 0xffff0000);
    url = _url;
}

void zViewText::infoSpans() {
    DLOG("info spans");
    for(auto s : cacheSpans) DLOG(s->info().str());
}

void zViewText::changeSpans(int pos, int length) {
    for(int i = 0 ; i < cacheSpans.size(); i++) {
        auto s(cacheSpans[i]);
        if(pos >= s->s && pos <= s->e) {
            s->e += length; if(s->e <= 0) cacheSpans.erase(i--, 1);
            for(int j = i + 1 ; j < cacheSpans.size(); j++) {
                s = cacheSpans[j];
                s->s += length; s->e += length;
                if(s->e <= 0) cacheSpans.erase(j--, 1);
            }
            break;
        }
    }
}

bool zViewText::getMinRangePos(int mn, int& ret) const {
    ret = INT_MAX;
    for(auto s : spans) {
        if(s->s < ret && s->s > mn) ret = s->s;
        else if(s->e < ret && s->e > mn) ret = s->e;
    }
    return ret != INT_MAX;
}

void zViewText::mergeSpans(czs& _text) {
    // если кэш уже есть - выйти
    if(cacheSpans.isNotEmpty()) return;
    int pos(0);
    if(spans.isNotEmpty()) {
        // 1. создаем диапазоны, начиная с минимального
        int minX1, minX2;
        if(!getMinRangePos(INT_MIN, minX1)) return;
        while(getMinRangePos(minX1, minX2)) {
            cacheSpans += new SPAN(new zTextSpan(), minX1, minX2, new zTextPaint(defPaint));
            minX1 = minX2;
        }
//        DLOG("1. make range");
//        infoSpans();
        // 2. заполняем созданные диапазоны параметрами
        for(auto real : cacheSpans) {
            auto s(real->s), e(real->e);
            for(auto sp : spans) {
                if(s < sp->s || e > sp->e) continue;
                sp->span->updateState(real->paint);
                auto t1(sp->span->typeId()), t2(real->span->typeId()); auto r(real->span);
                real->span = (t1 > t2 ? sp->span : real->span);
                if(t1 == SPAN_DEFAULT) delete r;
            }
        }
//        DLOG("1. fill range");
//        infoSpans();
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
//    DLOG("3. add default span");
    infoSpans();
}

zViewText::SPAN* zViewText::getSpan(int pos) const {
    for(auto& s : spans) { if(s->s >= pos && s->e <= pos) return s; }
    return nullptr;
}

void zViewText::setSpan(zTextSpan* _span, int start, int end, int flags) {
    auto len(realText.count()); if(!len) return;
    start = z_max(0, start); end = z_min(len, end);
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
    // создать внутренню структуру
    auto tspan(new SPAN(_span, start, end, new zTextPaint(defPaint), flags));
    // инициализировать краску
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
                    span = new zTextSpanImage(/*_html->getStringAttr("src", "znull")*/ 0,
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
    setText(html.text, true, false);
    // убрать все спаны с пометной mark
    for(int i = 0; i < spans.size(); i++) {
        if(spans[i]->f == SPAN_FLAGS_MARK) {
            spans.erase(i--, 1, true);
        }
    }
    return true;
}

szi zViewText::textWrapSpan(cstr _text, int widthRect) {
    int lines(1), maxHeight(0), maxWidth(0), sepPos(0), sepWidth(0), _count;
    SPAN* _sp(nullptr);
    // проверить на кэшированные значения
    if(textCache.isNotEmpty() && widthRect >= widthRectCache) {
        //DLOG("from cache %s", textCache[0]->text.str());
        for(auto& cache : textCache) maxHeight += cache->size;
        return { widthRectCache, maxHeight };
    }
    // разбить текст по спец. символам по ширине ректа
    if(widthRect <= 0 || getLines() == 1) widthRect = INT_MAX;
    textCache.clear();
    // адрес последнего разделителя/начало подстроки/текущий символ
    cstr separator(nullptr), _stext(_text); int ch('A');
    // массив спанов
    mergeSpans(_text);
    // идти по всем спанам
    int width(0), height(0), _i(0), offs(0);
    for(int i = 0; i < cacheSpans.size(); i++) {
        auto sp(cacheSpans[i]); auto paint(sp->paint); auto _pos(sp->s);
        height = z_max(height, paint->getSize());
        width += paint->getMargin() + paint->getItalic() + paint->width;
        while(_pos < sp->e) {
            auto _ch(ch); if(!(ch = z_decodeUTF8(z_charUTF8(_text, &_count)))) break;
            // если это не начало подстроки и есть разделитель - запоминаем его
            if(_stext != _text && z_delimiter(_ch)) separator = _text, sepWidth = width, sepPos = _pos, _sp = sp, _i = i;
            if(getLines() == 1 || ch != '\n') {
                // определить новую ширину строки
                auto ln(paint->getWidthChar(ch) + width);
                // если длина подстроки меньше ширины ректа и символ не "новая строка" = дальше
                if(_stext == _text || lines >= getLines() || ln < widthRect) { width = ln; _text += _count; _pos++; continue; }
            } else { separator = nullptr; }
            // добавить подстроку в массив
            if(separator) _text = separator, width = sepWidth, _pos = sepPos, sp = _sp, i = _i, separator = nullptr;
            _text = addCacheSubString(_stext, _text, width, height, true);
            maxWidth = z_max(width, maxWidth); maxHeight += height + offs;
            _stext = _text; lines++;
            width = paint->getItalic(); height = paint->getSize();
        }
    }
    // последняя подстрока
    if(_stext < _text) {
        addCacheSubString(_stext, _text, width, height, true);
        maxWidth = z_max(width, maxWidth); maxHeight += height + offs;
    }
    widthRectCache = maxWidth;
    return { maxWidth, maxHeight };
}

void zViewText::drawTextSpan(crti& clip) {
    static bool isShow = true;
    // отрисовка
    auto tbound(&drw[DRW_TXT]->bound); pti coord(tbound->xy());
    int indexCache(0), indexText(0);
    // взять текущий текст из кэша
    auto cacheStr(getStringFromCache(indexCache++, tbound, coord));
    for(auto sp: cacheSpans) {
        auto paint(sp->paint);
        coord.x += paint->getMargin();
        if(sp->span->isSkip()) { indexText++; continue; }
        auto posSpan(sp->s);
        // корректировать вертикальную позицию, относительно базовой линии шрифта
        auto subH(paint->correctBaseline(cacheStr->size));
//        if(isShow) DLOG("size %i subH %i string %s", cacheStr->size, subH, cacheStr->text.ptr(indexText));
        while(posSpan < sp->e) {
            // сдвинуть по верт. относительно общей высоты строки
            coord.y += subH;
            // lenText -> либо весь sp, либо весь cacheStr
            auto lenText(z_min(cacheStr->text.count() - indexText, sp->e - posSpan));
            auto txt(cacheStr->text.ptr(indexText));
            // следующая позиция
            indexText += lenText; posSpan += lenText;
            auto wpix(drawFragment(txt, lenText, paint, cacheStr, coord, clip, subH));
            // проверка - если спан сам себя рисует
            sp->span->draw(coord.x, coord.y, cacheStr->size, paint, clip);
            // корректировать позицию для следующего спана
            coord.x += wpix; coord.y -= subH;
            // проверка на конец строки из кэша
            if(indexText >= cacheStr->text.count()) {
                // корректировать позицию для следующей подстроки
                coord.x = tbound->x; coord.y += cacheStr->size;
                if(!(cacheStr = getStringFromCache(indexCache++, tbound, coord))) break;
                // корректировать вертикальную позицию, относительно базовой линии шрифта
                subH = paint->correctBaseline(cacheStr->size);
//                if(isShow) DLOG("size %i subH %i string %s", cacheStr->size, subH, cacheStr->text.str());
                indexText = 0;
            }
        }
    }
    isShow = false;
}

void zViewText::insertText(int pos, cstr _text) {
    changeSpans(pos, z_countUTF8(_text));
    setText(realText.insert(pos, _text), true, false);
    DLOG("t %s", realText.str());
}

void zViewText::removeText(int pos, int count) {
    changeSpans(pos, -count);
    setText(realText.remove(pos, count), true, false);
}
