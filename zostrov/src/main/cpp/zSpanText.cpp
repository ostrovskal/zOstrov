//
// Created by User on 19.04.2023.
//

#include "zostrov/zCommon.h"
#include "zostrov/zSpanText.h"
#include "zostrov/zViewText.h"

zMatrix zTextSpanImage::m;

zTextPaint::zTextPaint() {
    font = manager->loadResourceTexture(theme->styles->_int(Z_THEME_FONT, z.R.drawable.font_def), nullptr);
    setStyle(ZS_TEXT_NORMAL);
    setSize((int)theme->styles->_int(Z_THEME_SIZE_TEXT_TEXT, 20_dp));
}

zTextPaint::~zTextPaint() {
    manager->cache->recycle(font);
}

void zTextPaint::init(zTextPaint* _paint) {
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

zString8 zTextPaint::info() const {
    return z_fmt("fk:%x bk:%x sts:%x sz:%i bs:%i wdt:%i", fkColor, bkColor, getStyle(), getSize(), baseLine, width).str();
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

zTexture::TILE_TTL* zTextPaint::getBoundChar(int ch, rti& tex, rti& bound) const {
    if(ch < '!') ch = ' ';
    auto glyph(font->paramGlyph(ch + bold));
    if(glyph) {
        tex = glyph->rect;
        bound.set(0, getFactor((float)(tex.y - yBold)), getFactor((float)tex.w), getFactor((float)tex.h));
        tex.w += tex.x; tex.h += tex.y;
    }
    return glyph;
}

int zTextPaint::correctBaseline(int _size) const {
    if(!(_size - getSize())) return 0;
    auto htex((float)getSizeFont());
    auto bs(z_round(((float)getBaseline() * ((float)_size / (float)htex)) + 0.5f));
    auto _bs(z_round(((float)getBaseline() * ((float)getSize() / (float)htex)) + 0.5f));
    return (_size - bs) - (getSize() - _bs);
}

int zTextPaint::getWidthChar(int ch) const {
    auto g(font->paramGlyph(ch + bold)); if(!g) return 0;
    auto l(g->getLeft()), r(g->getRight());
    if(style & ZS_TEXT_MONO) return (getFactor((float)(g->rect.w + l + r)));
    auto w(getFactor((float)g->rect.w));
    auto ll(getFactor((float)l)), rr(getFactor((float)r));
    l = l < 0 ? z_max(l, ll) : z_min(l, ll);
    r = r < 0 ? z_max(r, rr) : z_min(r, rr);
    return w + l + r;
}

int zTextPaint::widthText(cstr _text, int length) const {
    int len, _width(0);
    while(length-- > 0 && z_is8(_text)) {
        _width += getWidthChar(z_decode8(z_char8(_text, &len)));
        _text += len;
    }
    return _width;
}

int zTextPaint::indexOf(cstr _text, int length, int screenLimit, int screenX, bool exact) const {
    int wChar(0), _count(0), len;
    while(z_is8(_text) && length-- > 0) {
        // определить ширину символа
        wChar = getWidthChar(z_decode8(z_char8(_text, &len)));
        // проверить на лимит
        if((screenX + wChar) >= screenLimit) break;
        screenX += wChar; _text += len; _count++;
    }
    // определить по ширине символа куда мы ближе
    int delta(0);
    if(z_is8(_text)) delta = (exact ? (screenX + (wChar / 2) < screenLimit) : 1);
    return _count + delta;
}

int zTextPaint::indexReverseOf(cstr _text, int length, int screenLimit) const {
    int _width(0), _count(0);
    while(z_is8(_text) && length > 0) {
        // определить ширину символа
        _width += getWidthChar(z_decode8(z_char8(z_ptr8(_text, --length))));
        // проверить на лимит
        if(_width >= screenLimit) break;
        _count++;
    }
    return _count;
}

zTextSpanImage::zTextSpanImage(cstr image, cstr tile, float factor, u32 _color) {
    dr.init(0, _color, -1, 0, factor);
    auto tex(manager->cache->get(image, nullptr));
    if(tex) {
        dr.texture  = tex;
        dr.tile     = tex->getTile(tile);
    }
}

void zTextSpanImage::draw(int x, int y, int hmax, zTextPaint *paint, crti& clip) {
    rti r(x + 2_dp, y, paint->width - 4_dp, paint->getSize());
    m.translate((float)(r.x - manager->screen.x), (float)(r.y - manager->screen.y), 0);
    dr.measure(r.w, r.h, 0, false);
    dr.drawCommon(clip, m, true);
}

void zTextSpanImage::updateState(zTextPaint *paint) {
    auto t(dr.texture->getTile(dr.tile));
    if(t) {
        paint->width = (z_round((float)t->rect.w * dr.scale) + 4_dp);
        paint->setSize(z_round((float)t->rect.h * dr.scale));
    }
}

zTextSpanBullet::zTextSpanBullet(bool _ordered, int _index) : zTextSpanImage("zssh", "iconBullet", 0.5f, 0xffffffff), ordered(_ordered) {
    if(_ordered) {
        _idx = z_ntos(&_index, RADIX_DEC, true);
        _idx += ". "; dr.typeTri = GL_TRIANGLES;
    }
}

void zTextSpanBullet::updateState(zTextPaint *paint) {
    zTextSpanImage::updateState(paint);
    if(ordered) {
        // определить ширину
        paint->width = z_max(paint->widthText(_idx.str(), INT_MAX), paint->width);
        paint->setSize(12_dp);
        dr.texture = paint->getFont();
    }
    //paint->setMargin(25_dp);
}

void zTextSpanBullet::draw(int x, int y, int hmax, zTextPaint *paint, crti& clip) {
    if(ordered) {
        // выровнять по базовой линии
        auto len(_idx.count()); dr.make(len * 6); dr.makeText(_idx.str(), len, paint);
        auto screenX(manager->screen.x), screenY(manager->screen.y);
        m.translate((float)(x - screenX + 2), (float)(y - screenY + 2), 0);
        dr.color = 0xff000000; dr.drawCommon(clip, m, true);
        m.translate((float)(x - screenX), (float)(y - screenY), 0);
        dr.color = z.R.color.white; dr.drawCommon(clip, m, true);
    } else {
        // просто нарисовать значок
        zTextSpanImage::draw(x, y, hmax, paint, clip);
    }
}

void zTextSpanMask::draw(int x, int y, int hmax, zTextPaint *paint, crti &clip) {
    glBlendFuncSeparate(GL_DST_COLOR, GL_SRC_COLOR, GL_DST_COLOR, GL_SRC_COLOR);
    auto w(paint->width); paint->width += _width;
    zTextSpanImage::draw(x + w - paint->width, y, hmax, paint, clip);
    paint->width -= _width;
    glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
}

void zTextSpanParagraph::updateState(zTextPaint *paint) {
    paint->width = 14_dp;
}

void zTextSpanGravity::margin(zViewText* vt, zTextPaint* paint, cstr str) {
    auto w(paint->widthText(str, INT_MAX)), wc(vt->rclient.w);
    switch(grav) {
        case ZS_GRAVITY_START:  w = vt->ipad.x; break;
        case ZS_GRAVITY_END:    w = wc - vt->ipad.w - w; break;
        case ZS_GRAVITY_HCENTER:w = (wc - vt->ipad.extent(false) - w) / 2; break;
    }
    paint->width = w;
}

void zViewText::infoSpans() {
    DLOG("info spans");
    for(auto s : cacheSpans) { DLOG(s->info().str()); }
}

void zViewText::changeSpans(int pos, int length) {
    getSpan(SPAN_POSITION, pos, false, [this, length](int i, SPAN* s) {
        s->e += length; if(s->length() <= 0) spans.erase(i, 1);
    });
    spans.enumerate(false, [this, pos, length](int i, SPAN* s) {
        if(pos >= s->s) return false;
        for(; i < spans.size(); i++) { s = spans[i]; s->s += length; s->e += length; }
        return true;
    });
    clearCacheSpans(false);
}

bool zViewText::getMinRangePos(int mn, int& ret) const {
    ret = INT_MAX;
    for(auto s : spans) {
        if(s->s < ret && s->s > mn) ret = s->s;
        else if(s->e < ret && s->e > mn) ret = s->e;
    }
    return ret != INT_MAX;
}

void zViewText::mergeSpans(int count) {
    // если кэш уже есть - выйти
    if(cacheSpans.isNotEmpty()) return;
    int pos(0);
    if(spans.isNotEmpty()) {
        // 1. создаем диапазоны, начиная с минимального
        int minX1, minX2;
        if(!getMinRangePos(INT_MIN, minX1)) return;
        while(getMinRangePos(minX1, minX2)) {
            cacheSpans += new SPAN(defSpan, minX1, minX2, new zTextPaint(defPaint));
            minX1 = minX2;
        }
        // 2. заполняем созданные диапазоны параметрами
        for(auto real : cacheSpans) {
            auto s(real->s), e(real->e);
            for(auto sp : spans) {
                if(s < sp->s || e > sp->e) continue;
                sp->span->updateState(real->paint);
                // если в ссылке изображение
                real->span->setImage(sp->span);
                real->span = (real->span->typeId() > sp->span->typeId() ? real->span : sp->span);
            }
        }
        // добавить спан по умолчанию в разрывы
        for(int i = 0 ; i < cacheSpans.size(); i++) {
            auto sp(cacheSpans[i]);
            if(pos < sp->s) cacheSpans.insert(i, new SPAN(defSpan, pos, sp->s, new zTextPaint(defPaint)));
            pos = sp->e;
        }
    }
    // последний спан
    if(count && pos < count) cacheSpans += new SPAN(defSpan, pos, count, new zTextPaint(defPaint));
}

void zViewText::getSpan(int what, int arg, bool reverse, const std::function<void(int, SPAN*)>& act) {
    spans.enumerate(reverse, [what, arg, act](int i, SPAN* s) {
        switch(what) {
            case SPAN_FLAGS: if(s->f != arg) return false; break;
            case SPAN_POSITION: if(arg < s->s || arg >= s->e) return false; break;
            case SPAN_TYPE: if(s->span->typeId() != arg) return false; break;
        }
        act(i, s); return what != SPAN_POSITION;
    });
}

zViewText::SPAN* zViewText::setSpan(zTextSpan* _span, int start, int end, int flags) {
    start = z_max(0, start); end = z_min(realText.count(), end);
    auto len(end - start); if(len < 0) { delete _span; return nullptr; }
    if(flags == SPAN_FLAGS_DEFAULT && len == 0) {
        // найти последний c флагом SPAN_FLAGS_MARK
        getSpan(SPAN_FLAGS, SPAN_FLAGS_MARK, true, [end, &_span](int, SPAN* s) {
            s->f = SPAN_FLAGS_DEFAULT; s->e = end;
            SAFE_DELETE(_span); return true;
        });
        return nullptr;
    }
    // создать внутренню структуру
    auto tspan(new SPAN(_span, start, end, new zTextPaint(defPaint), flags));
    // инициализировать краску
    _span->updateState(tspan->paint);
    return spans += tspan;
}

void zViewText::delSpan(zTextSpan* _span) {
    auto idx(spans.indexOf(_span));
    if(idx != -1) spans.erase(idx, 1, true);
}

bool zViewText::setHtmlText(czs& text, const std::function<bool(cstr tag, bool end, zHtml* html)>& parser) {
    clearCacheSpans(true); realText = text;
    bool listOrdered(true); int listNum(1), listRev(1);
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
                case 0x3ee0e: case 0x79832919:
                    if(!end) {
                        pos += (_html->text[pos] == '\n');
                        _html->text += "\t";
                        span = new zTextSpanParagraph();
                        flags = SPAN_FLAGS_SPECIFIC;
                    }
                    break;
                // h1 - h6
                case 0xc7ada38: case 0xc7c22dd: case 0xc7d9a75:
                case 0xc7aa490: case 0xc7cf97d: case 0xc79a5b2:
                    if(_html->text[pos - 1] != '\n') { _html->text += "\n"; pos++; }
                    if(end) { _html->text += "\n", pos++; }
                    else {
                        _html->text += "\t";
                        setSpan(new zTextSpanGravity(ZS_GRAVITY_HCENTER), pos, pos + 1, SPAN_FLAGS_SPECIFIC);
                    }
                    span = new zTextSpanRelativeSize(htmlHeaderSize[tag[1] - '1']);
                    break;
                // b strong
                case 0x50e75: case 0x8e4b388b:
                    span = new zTextSpanStyle(ZS_TEXT_BOLD); break;
                // i em
                case 0x6b5: case 0xb073db:
                    span = new zTextSpanStyle(ZS_TEXT_ITALIC); break;
                // u
                case 0x6ae4b:
                    span = new zTextSpanUnderline(); break;
                // s strike
                case 0x51bdf: case 0xef748f41:
                    span = new zTextSpanStrikeline(); break;
                // img
                case 0x9ae16064:
                    _html->text += "\t";
                    span = new zTextSpanImage(_html->getStringAttr("src", "znull"), _html->getStringAttr("t", "rect"),
                                              _html->getFloatAttr("s", 1.0f), _html->getColorAttr("c", 0xffffffff));
                    flags = SPAN_FLAGS_SPECIFIC;
                    break;
                // a
                case 0x58de4:   span = new zTextSpanUrl(end ? "" : _html->getStringAttr("href", "self").str()); break;
                // title
                case 0x6f58b184:span = new zTextSpanAbsoluteSize(10); break;
                // sub
                case 0x80391c79:span = new zTextSpanSubscript(); break;
                // sup
                case 0x803c7044:span = new zTextSpanSuperscript(); break;
                // font
                case 0x89c2d575: break;
                // ul ol
                case 0x2c4b9944: case 0x1466284c:
                    if(_html->text[pos] != '\n') _html->text += "\n";
                    listOrdered = (hash == 0x1466284c);
                    listNum = _html->getIntegerAttr("start", RADIX_DEC, 1);
                    listRev = _html->getIntegerAttr("reversed", RADIX_DEC, 0);
                    break;
                // li
                case 0x13844afb:
                    if(end) {
                        if(_html->text[pos] != '\n') _html->text += "\n";
                    } else {
                        _html->text += "\t"; flags = SPAN_FLAGS_SPECIFIC;
                        setSpan(new zTextSpanParagraph(), pos, pos + 1, flags);
                        _html->text += "\t"; pos++;
                        span = new zTextSpanBullet(listOrdered, listNum);
                        listNum += !listRev * 2 - 1;
                    }
                    break;
                // bkg background
                case 0xc9b77622:
                case 0x1aaadb91: span = new zTextSpanBackgroundColor(end ? 0 : _html->getColorAttr("v", -1)); break;
                // c color
                case 0xdbf89687:
                case 0x4595e:   span = new zTextSpanForegrounColor(end ? 0 : _html->getColorAttr("v", -1)); break;
            }
            if(span) setSpan(span, pos, pos + (flags == SPAN_FLAGS_SPECIFIC), flags);
        }
        return true;
    });
    realText = html.text; updateText();
    // убрать все спаны с пометной mark
    for(int i = 0; i < spans.size(); i++) {
        if(spans[i]->f == SPAN_FLAGS_MARK) spans.erase(i--, 1, true);
    }
    return true;
}

szi zViewText::textWrapSpan(cstr _text, int widthRect) {
    int maxHeight(0), sepPos(0), sepWidth(0), _count;
    SPAN* _sp(nullptr);
    // разбить текст по спец. символам по ширине ректа
    if(widthRect <= 0) widthRect = INT_MAX;
    textCache.clear();
    // адрес последнего разделителя/начало подстроки/текущий символ
    cstr separator(nullptr), _stext(_text); int ch('A');
    // массив спанов
    mergeSpans(z_count8(_text));
    // идти по всем спанам
    int width(0), height(0), _i(0);
    for(int i = 0; i < cacheSpans.size(); i++) {
        auto sp(cacheSpans[i]); auto paint(sp->paint); auto _pos(sp->s);
        height = z_max(height, paint->getSize());
        width += shadow.x + paint->getItalic() + paint->width;
        while(_pos < sp->e) {
            auto _ch(ch); if(!(ch = z_decode8(z_char8(_text, &_count)))) break;
            // если это не начало подстроки и есть разделитель - запоминаем его
            if(_stext != _text && z_delimiter(_ch)) separator = _text, sepWidth = width, sepPos = _pos, _sp = sp, _i = i;
            if(ch != '\n') {
                // определить новую ширину строки
                auto ln(paint->getWidthChar(ch) + width);
                // если длина подстроки меньше ширины ректа и символ не "новая строка" = дальше
                if(_stext == _text || ln <= widthRect) { width = ln; _text += _count; _pos++; continue; }
            } else { separator = nullptr; _text++; _pos++; }
            // добавить подстроку в массив
            if(separator) _text = separator, width = sepWidth, _pos = sepPos, sp = _sp, i = _i, separator = nullptr;
            _text = addCacheSubString(_stext, _text, width, height, true);
            widthRectCache = z_max(width, widthRectCache); maxHeight += height;
            _stext = _text; height = defPaint->getSize(); width = paint->getItalic();
        }
        if(_stext == _text) width = 0;
    }
    // последняя подстрока
    if(_stext < _text) {
        addCacheSubString(_stext, _text, width, height, true);
        widthRectCache = z_max(width, widthRectCache); maxHeight += height;
    }
    return { widthRectCache, maxHeight };
}

void zViewText::drawTextSpan(crti& clip) {
    urls.clear();
    // отрисовка
    auto tbound(&drw[DRW_TXT]->bound); pti coord(tbound->xy());
    int indexCache(0), indexText(0);
    // взять текущий текст из кэша
    auto cacheStr(getStringFromCache(indexCache++, tbound, coord));
    if(!cacheStr) return;
    for(auto sp: cacheSpans) {
        auto paint(sp->paint); auto posSpan(sp->s);
        // корректировать вертикальную позицию, относительно базовой линии шрифта
        auto subH(paint->correctBaseline(cacheStr->size));
        sp->span->margin(this, paint, cacheStr->text);
        while(posSpan < sp->e) {
            // сдвинуть по верт. относительно общей высоты строки
            coord.y += subH;
            // lenText -> либо весь sp, либо весь cacheStr
            auto lenText(z_min(cacheStr->text.count() - indexText, sp->e - posSpan));
            auto txt(cacheStr->text.ptr(indexText));
            // следующая позиция
            indexText += lenText; posSpan += lenText;
            auto wpix(drawFragment(txt, lenText, paint, cacheStr, coord, clip, subH));
            // проверить на ссылку
            if(sp->span->typeId() == spans::SPAN_URL) {
                urls += new URL(rti(coord.x, coord.y, wpix, cacheStr->size), sp);
                status |= ZS_CLICKABLE;
            }
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
                indexText = 0;
            }
        }
    }
}

void zViewText::insertText(int pos, cstr _text) {
    changeSpans(pos, z_count8(_text));
    realText.insert(pos, _text);
    updateText();
}

void zViewText::removeText(int pos, int count) {
    changeSpans(pos, -count);
    realText.remove(pos, count);
    updateText();
}
