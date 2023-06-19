//
// Created by User on 19.04.2023.
//

#include "zssh/zCommon.h"
#include "zssh/zSpanText.h"
#include "zssh/zViewText.h"

zTextPaint::zTextPaint() {
    font = manager->loadResourceTexture(theme->styles->_int(Z_THEME_FONT, z.R.drawable.fontDefault), nullptr);
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

class zViewImageSpan : public zViewImage {
public:
    zViewImageSpan(zTextSpanImage* _span, int tile) : zViewImage(styles_default, 0, tile), span(_span) {
        minMaxSize.set(10_dp, 0, 10_dp, 0);
    }
protected:
    zTextSpanImage* span{nullptr};
};

zTextSpanImage::zTextSpanImage(cstr nameImage, int tile, float factor, int pad) {
/*
    pad = z_dp(pad);
    image = new zViewImageSpan(this, tile);
    image->onInit(false);
    image->setBitmapFactor(factor);
    image->setPadding(rti(pad, pad, pad, pad));
    image->drs[zView::DR_IDX_FK]->texture = vmanager->cache.get(nameImage, nullptr);
    image->measure(MAKE_MEASURE_SPEC(MEASURE_UNDEF, 0), MAKE_MEASURE_SPEC(MEASURE_UNDEF, 0));
*/
}

zTextSpanImage::~zTextSpanImage() { delete image; }

bool zTextSpanImage::draw(int x, int y, int hmax, zTextPaint *paint) {
/*
    auto pad(image->pad);
    rti r(x + pad.x, y + (hmax - paint->height) / 2 + pad.y, paint->width, paint->height);
    image->drs[zView::DR_IDX_FK]->invalidate(paint->width, paint->height);
    image->drs[zView::DR_IDX_FK]->layout(&r);
    image->drs[zView::DR_IDX_FK]->specDraw(false);
*/
    return true;
}

void zTextSpanImage::updateState(zTextPaint *paint) {
/*
    auto pad(image->pad);
    paint->width = image->rfull.w + pad.extent(false);
    paint->height = image->rfull.h + pad.extent(true);
*/
}

zTextSpanBullet::zTextSpanBullet(bool _ordered, int _index) : zTextSpanImage("zssh", z.R.integer.iconBullet, 0.33f, !_ordered * 2), ordered(_ordered) {
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

bool zTextSpanBullet::draw(int x, int y, int hmax, zTextPaint *paint) {
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
    return true;
}

zTextSpanUrl::zTextSpanUrl(cstr _url) {
    color = theme->styles->_int(Z_THEME_COLOR_TEXT_URL, 0xffff0000);
    url = _url;
}
