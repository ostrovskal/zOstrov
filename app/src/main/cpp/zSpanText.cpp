//
// Created by User on 19.04.2023.
//

#include "zssh/zCommon.h"
#include "zssh/zSpanText.h"
#include "zssh/zViewText.h"

zTextPaint::zTextPaint() {
    font = manager->loadResourceTexture(z.R.drawable.fontDefault, nullptr);
    setStyle(ZS_TEXT_NORMAL);
    setSize(20_dp);
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

void zTextPaint::setSize(i32 _size) {
    size   = _size;
    factor = ((float)_size / (float)(font->getSize().h / 2.0f));
}

void zTextPaint::setStyle(u32 _style) {
    style   = _style;
    italic  = (int)((style & ZS_TEXT_ITALIC) * 4);
    bold    = 224 * ((style & ZS_TEXT_BOLD) != 0);
    yBold   = (font->getSize().h / 2) * (bold != 0);
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

int zTextPaint::widthText(cstr _text, int length) const {
    int len, width(0);
    while(length-- > 0 && z_isUTF8(_text)) {
        width += getWidthChar(z_decodeUTF8(z_charUTF8(_text, &len)));
        _text += len;
    }
    return width;
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
    int width(0), _count(0);
    while(z_isUTF8(_text) && length > 0) {
        // определить ширину символа
        width += getWidthChar(z_decodeUTF8(z_charUTF8(z_ptrUTF8(_text, --length))));
        // проверить на лимит
        if(width >= screenLimit) break;
        _count++;
    }
    return _count;
}
