//
// Created by User on 19.04.2023.
//

#include "zssh/zCommon.h"
#include "zssh/zSpanText.h"
#include "zssh/zViewText.h"

void zTextPaint::init(zTextPaint* _paint) {
    size    = _paint->size;
    preWidth= _paint->preWidth;
    fkColor = _paint->fkColor;
    bkColor = _paint->bkColor;
    style   = _paint->style;
    italic  = _paint->italic;
    baseLine= _paint->baseLine;
    texture = _paint->texture;
}

void zTextPaint::reset(zViewText* _tv) {
/*
    height  = _tv->getTextSize();
    fkColor = _tv->getTextColorForeground();
    bkColor = _tv->getTextBackgroundColor();
    style   = _tv->getTextStyle();
    baseLine= _tv->getDescent();
    texture = _tv->dr[0]->texture;
*/
}

void zTextPaint::setStyle(u32 _style) {
    if(style != _style) {
        style  = _style;
        italic = (int)((style & ZS_TEXT_ITALIC) * 4);
    }
}
