//
// Created by User on 18.04.2023.
//

#include "zssh/zCommon.h"

void zDrawable::release() {
    clear();
    if(texture) {
        if(texture->idFBO) {
            delete texture;
        } else {
            manager->cache->recycle(texture);
        }
        texture = nullptr;
    }
}

void zDrawable::make(i32 _count) {
    if(_count > count) clear();
    if(!vertices) {
        if(_count) {
            vertices = new zVertex2D[_count];
            memset(vertices, 0, _count * sizeof(zVertex2D));
        }
    }
    count = _count;
}

int zDrawable::makeQuad(crti& pos, crti& tex, zVertex2D* v, int _italic, bool _strip) const {
    auto _tex(texture->getReverseSize()); auto offs((float)_italic);
    auto px1((float)pos.x), py1((float)pos.y), px2(px1 + (float)pos.w), py2(py1 + (float)pos.h);
    auto tx1((float)tex.x * _tex.w), ty1((float)tex.y * _tex.h), tx2((float)tex.w * _tex.w), ty2((float)tex.h * _tex.h);
    v->x = px1 + offs;  v->y = py1; v->u = tx1; v->v = ty1; v++;
    v->x = px1;         v->y = py2; v->u = tx1; v->v = ty2; v++;
    v->x = px2 + offs;  v->y = py1; v->u = tx2; v->v = ty1; v++;
    v->x = px2;         v->y = py2; v->u = tx2; v->v = ty2;
    if(_strip) { v++; v->x = px2;  v->y = py2; }
    return 4 + _strip;
}

int zDrawable::makeTriangle(crti& pos, crti& tex, zVertex2D* v, int _italic) const {
    auto _tex(texture->getReverseSize()); auto offs((float)_italic);
    auto px1((float)pos.x), py1((float)pos.y), px2((float)px1 + (float)pos.w), py2((float)py1 + (float)pos.h);
    auto tx1((float)tex.x * _tex.w), ty1((float)tex.y * _tex.h), tx2((float)tex.w * _tex.w), ty2((float)tex.h * _tex.h);
    v->x = px1 + offs;  v->y = py1; v->u = tx1; v->v = ty1; v++;
    v->x = px1;         v->y = py2; v->u = tx1; v->v = ty2; v++;
    v->x = px2 + offs;  v->y = py1; v->u = tx2; v->v = ty1; v++;
    v->x = px2 + offs;  v->y = py1; v->u = tx2; v->v = ty1; v++;
    v->x = px1;         v->y = py2; v->u = tx1; v->v = ty2; v++;
    v->x = px2;         v->y = py2; v->u = tx2; v->v = ty2;
    return 6;
}

enum { VX, VY, VW, VH };

/** Карта для патч9 */
static int mapPatch[] = {VX,  0, VY,  0, VX,  1, VY,  1,// LEFT_TOP
                         VX,  1, VY,  0, VW, -1, VY,  1,// TOP
                         VW, -1, VY,  0, VW,  0, VY,  1,// RIGHT_TOP
                         VW, -1, VY,  1, VW,  0, VH, -1,// RIGHT
                         VW, -1, VH, -1, VW,  0, VH,  0,// RIGHT_BOTTOM
                         VX,  1, VH, -1, VW, -1, VH,  0,// BOTTOM
                         VX,  0, VH, -1, VX,  1, VH,  0,// LEFT_BOTTOM
                         VX,  0, VY,  1, VX,  1, VH, -1,// LEFT
                         VX,  1, VY,  1, VW, -1, VH, -1 // CENTER
};

static int x9, y9;

static void calcPatch9(int idx, crti& p9, crti& p, crti& t) {
    int v(mapPatch[idx + 1]);
    switch(mapPatch[idx]) {
        case VY: v *= p9.y; x9 = p.y + v;         y9 = t.y + v; break;
        case VW: v *= p9.w; x9 = p.x + p.w + v;   y9 = t.w + v; break;
        case VX: v *= p9.x; x9 = p.x + v;         y9 = t.x + v; break;
        case VH: v *= p9.h; x9 = p.y + p.h + v;   y9 = t.h + v; break;
    }
}

int zDrawable::makePatch9(crti& pos, crti& tex, crti& p9) const {
    int _count(0); rti p, t;
    static zVertex2D tmpVerts[36];
    static u8 vnum[] = {0, 1, 2, 3, 6, 7, 10, 11, 11, 12,15, 13, 19,
                        17, 17, 22, 21, 20, 25, 24, 24, 28, 31, 30, 35, 34};
    auto p9w(p9.extent(false)), p9h(p9.extent(true));
    rti _pos(0, 0, pos.w < p9w ? p9w : pos.w, pos.h < p9h ? p9h : pos.h);
    float sh((float)pos.h / (float)_pos.h), sw((float)pos.w / (float)_pos.w);
    for(int i = 0; i < 9 * 8; i += 8) {
        calcPatch9(i + 0, p9, _pos, tex); t.x = y9; p.x = x9;
        calcPatch9(i + 2, p9, _pos, tex); t.y = y9; p.y = x9;
        calcPatch9(i + 4, p9, _pos, tex); t.w = y9; p.w = x9 - p.x;
        calcPatch9(i + 6, p9, _pos, tex); t.h = y9; p.h = x9 - p.y;
        p.y = (int)round((float)p.y * sh); p.h = (int)round((float)p.h * sh);
        p.x = (int)round((float)p.x * sw); p.w = (int)round((float)p.w * sw);
        p.offset(pos.x, pos.y);
        _count += makeQuad(p, t, &tmpVerts[i / 2]);
    }
    // переделать в стрип
    for(int i = 0; i < 26; i++) vertices[i] = tmpVerts[vnum[i]];
    return 26;
}

void zDrawable::draw(rti *rect) {
    static zMatrix t, m;
    if(vertices && visible) {
        auto clip(view->drawableClip());
        // определение видимости
        if(texture && clip.isNotEmpty()) {
            // цвет
            auto _a(color.a);
            // установка смещения
            auto _bound = (rect ? *rect : bound) - manager->screen;
            ptf offs((float)_bound.x - vertices->x, (float)_bound.y - vertices->y);
            auto fbo(!view->isFBO() || index == DRW_FBO);
            if(fbo) {
                offs.x += view->trans.x; offs.y += view->trans.y;
                m = view->mtx; color.a *= view->alpha;
            } else {
                m.identity();
            }
            m *= t.translate(offs.x, offs.y, 1);
            drawCommon(clip, m, fbo);
            color.a = _a;
        }
    }
}

void zDrawable::drawCommon(crti& clip, const zMatrix& m, bool fbo) const {
    auto _clip(clip);
    // устанавливаем текстуру
    glBindTexture(GL_TEXTURE_2D, texture->id);
    // устанавливаем параметры шейдеров и обрезки
    manager->prepareRender(vertices, _clip - manager->screen, m);
    // устанавливаем цветовой фильтр
    manager->setColorFilter(fbo ? view : nullptr, color);
    // запускаем отрисовку
    glDrawArrays(typeTri, 0, count);
    // подсчет кол-во треугольников в кадре
    manager->countVertices += count;
    manager->countObjs++;
}

void zDrawable::measure(int width, int height, int pivot, bool isSave) {
    u16* ptr;
    if(index >= DRW_FBO) {
        // проверить на текстуру
        if(!(texture && texture->isEqual(width, height))) {
            if(!texture) texture = new zTexture();
            if(!texture->makeFBO(width, height)) {
                DLOG("error makeFBO %s(%i)", view->typeName(), view->id);
                release(); return;
            }
//            DLOG("fbo:%i %i %s", width, height, view->typeName());
        }
    }
    if(texture && (ptr = texture->getTile(tile))) {
        rti tex(ptr); auto isPatch9(false);
        // определение патча
        if(*(u32*)(ptr + 4)) {
            if(!(isPatch9 = (ptr[4] != 0xffff)))
                tex = texture->getSize();
        }
        // определение количества вершин
        make(isPatch9 * 22 + 4);
        // определение ректа
        if(!isSave) {
            if(!width) width = tex.w;
            if(!height) height = tex.h;
            bound.set((pivot & 1) * -(width >> 1), (pivot >> 1) * -(height >> 1), width, height);
        }
        // применить отступ к текстуре
        if(padding.isNotEmpty()) {
            tex += padding;
            tex.w -= padding.extent(false);
            tex.h -= padding.extent(true);
        }
        // текстуру в полный рект
        tex.w += tex.x; tex.h += tex.y;
        if(isPatch9) {
            // формирование патча
            makePatch9(bound, tex, (u8*)&ptr[4]);
        } else {
            // формирование квада
            makeQuad(bound, tex, vertices);
        }
    }
}

static zVertex2D* makeDebugRect(float x, float y, float w, float h, zVertex2D* v) {
    v->x = x;	  v->y = y;     v++;
    v->x = x + w; v->y = y;     v++;	// top
    v->x = x;	  v->y = y;     v++;
    v->x = x;	  v->y = y + h; v++;	// left
    v->x = x + w; v->y = y;     v++;
    v->x = x + w; v->y = y + h; v++;	// right
    v->x = x;     v->y = y + h; v++;
    v->x = x + w; v->y = y + h; v++;    // bottom
    return v;
}

void zDrawable::makeDebug(cszi &cell) {
    SAFE_DELETE(vertices);
    // определить размер массива линий
    count = 8;
    // rclient
    auto rclip(view->rclient - manager->screen);
    float wc((float)rclip.w - 1.5f), hc((float)rclip.h - 1.5f), xc((float)rclip.x + 0.5f), yc((float)rclip.y + 0.5f);
    if(!cell.isEmpty()) count += (cell.h - 1) * 2 + (cell.w - 1) * 2;
    auto v(new zVertex2D[count]); vertices = v;
    memset(v, 0, count * sizeof(zVertex2D));
    // клиент элемента
    v = makeDebugRect(xc, yc, wc, hc, v);
    // сетка
    if(!cell.isEmpty()) {
        float cw((float)wc / (float)cell.w), ch((float)hc / (float)cell.h); yc += ch;
        for(int y = 0; y < cell.h - 1; y++) {
            v->x = xc; v->y = yc; v++; v->x = xc + wc; v->y = yc; v++;
            yc = round(yc + ch);
        }
        xc += cw; yc = (float)rclip.y + 0.5f;
        for(int x = 0; x < cell.w - 1; x++) {
            v->x = xc; v->y = yc; v++; v->x = xc; v->y = yc + hc; v++;
            xc = round(xc + cw);
        }
    }
}

int zDrawable::makeText(cstr text, int len, zTextPaint* paint) {
    auto ht(paint->size), idx(0), stx(0), sty(0), style((int)paint->getStyle());
    auto offsetBold(224 * ((style & ZS_TEXT_BOLD) != 0)), offsetY(((texture->getSize().h / 2) * (offsetBold != 0)));
    auto factor(scaleFactor(ht, true)); rti _pos, tex;
    while(len-- > 0) {
        auto ch(z_decodeUTF8(z_charUTF8(text))); text += z_charLengthUTF8(text);
        if(ch < '!') ch = ' ';
        auto glyph(texture->paramGlyph(ch + offsetBold));
        if(!glyph) continue;
        tex.set(glyph); tex.w += tex.x; tex.h += tex.y;
        auto cw((int)round((float)glyph[2] * factor));
        if(ch == ' ') cw = 10.0f;
        _pos.set(stx, sty + (int)round((float)(glyph[1] - offsetY) * factor), cw, (int)round((float)glyph[3] * factor));
        // нарисовать
        idx += makeQuad(_pos, tex, &vertices[idx], paint->italic, true);
        stx += cw;
    }
    auto glyph(texture->paramGlyph('_' + offsetBold));
    tex.set(glyph);
    tex = tex.padding(4, 0); tex.w += tex.x; tex.h += tex.y;
    stx += paint->preWidth + paint->italic * factor;
    // strike
    if(style & ZS_TEXT_STRIKE) {
        _pos.set(0, 1 + ht / 2, stx - 2, (int)round(2.0f * factor));
        idx += makeQuad(_pos, tex, &vertices[idx], 0, true);
    }
    // underline
    if(style & ZS_TEXT_UNDERLINE) {
        auto bs((int)round((float)texture->descent * factor + 0.5f));
        _pos.set(0, ht - bs, stx - 2, (int)round(2.0f * factor));
        idx += makeQuad(_pos, tex, &vertices[idx], 0, true);
    }
    count = idx;
    return stx;
}

ptf zDrawable::offsetBound() const {
    auto rv(&manager->screen);
    return { (float)(bound.x - rv->x) - vertices->x, (float)(bound.y - rv->y) - vertices->y };
}

int zDrawable::sizeText(cstr _text, u32 heightText, int lengthText) const {
    int len, width(0); auto factor(scaleFactor(heightText, true));
    while (lengthText-- > 0 && z_isUTF8(_text)) {
        // определить ширину символа
        width += texture->widthGlyph(z_decodeUTF8(z_charUTF8(_text, &len)), factor);
        _text += len;
    }
    return width;
}

int zDrawable::indexOf(cstr _text, u32 heightText, int screenLimit, int screenX, bool exact, int *posScreen) const {
    int wGlyph(0), _count(0), len; auto factor(scaleFactor(heightText, true));
    while(z_isUTF8(_text)) {
        // определить ширину символа
        wGlyph = texture->widthGlyph(z_decodeUTF8(z_charUTF8(_text, &len)), factor);
        // проверить на лимит
        if((screenX + wGlyph) >= screenLimit) break;
        screenX += wGlyph; _text += len; _count++;
    }
    // определить по ширине символа куда мы ближе
    int delta(0);
    if(z_isUTF8(_text)) delta = (exact ? ((screenX + (wGlyph / 2)) < screenLimit) : 1);
    screenX += wGlyph * delta;
    if(posScreen) *posScreen = screenX;
    return _count + delta;
}

float zDrawable::scaleFactor(u32 value, bool text) const {
    auto h((float)(texture ? texture->getSize().h / (text + 1) : 1));
    return ((float)value / h);
}

int zDrawable::indexReverseOf(cstr _text, u32 heightText, int limitPix, int lengthText) const {
    int posPix(0), _count(0);
    auto factor(scaleFactor(heightText, true));
    while(z_isUTF8(_text) && lengthText-- > 0) {
        // определить ширину символа
        auto wGlyph(texture->widthGlyph(z_decodeUTF8(z_charUTF8(z_ptrUTF8(_text, lengthText))), factor));
        // проверить на лимит
        if((posPix + wGlyph) >= limitPix) break;
        posPix += wGlyph; _count++;
    }
    return _count;
}

void zDrawable::info() const {
    DLOG("%s", z_fmt("bound(%i-%i:%i-%i) col:%x tex:%s count:%i tile:%i",
                     bound.x, bound.y, bound.w, bound.h, color.toABGR(), color.g, color.b, color.a,
                     texture ? texture->name.str() : "null", count, tile).str());
}

void zDrawable::init(const zDrawable* drw, int _tile) {
    texture     = manager->cache->get(drw->texture->name, texture);
    padding     = drw->padding;
    tile        = _tile;
    color       = drw->color;
}

void zDrawable::init(u32 _tx, u32 _cl, i32 _tl, u32 _pd, float _sc) {
    if(_tx & 0xff000000) {
        _cl = _tx; _tx = z.R.drawable.zssh;
        if(_tl == -1) _tl = z.R.integer.rect;
    } else if(!_cl) _cl = theme->themeColor;
    padding.set(_pd); color.set(_cl); scale = _sc;
    texture = manager->loadResourceTexture(_tx, texture);
    tiles = _tl; setTileNum(0);
//    if(texture) bound = rectTile(tile);
}

void zDrawable::drawFBO(zView* prev, const std::function<void(void)>& _draw) {
    if(texture) {
        // установить целевую текстуру
        setFBO(true, true);
        zView::fbo = view;
        manager->screen = view->rview;
        // нарисовать в текстуру
        _draw();
        // восстановить предыдущую цель
        zView::fbo = prev;
        if(prev) {
            manager->screen = prev->rview;
            prev->drw[DRW_FBO]->setFBO(true, false);
        } else {
            auto sz(zGL::instance()->getSizeScreen());
            manager->screen.set(0, 0, sz.w, sz.h);
            setFBO(false, false);
        }
    }
}

szi zDrawable::resolveSize(int wmax, int hmax, u32 gravity) const {
    int w(0), h(0);
    // определить габариты картинки
    if(texture) {
        auto rect(rectTile(tile));
        w = rect.w, h = rect.h; auto rel((float)w / (float)h);
        switch(gravity & ZS_SCALE_MASK) {
            case ZS_SCALE_HEIGHT: h = hmax; w = (int)roundf(h * rel); break;
            case ZS_SCALE_WIDTH:  w = wmax; h = (int)roundf(w * rel); break;
            case ZS_SCALE_MIN: h = z_min(w, h); w = (int)roundf(h * rel); break;
            case ZS_SCALE_MAX: h = z_max(w, h); w = (int)roundf(h * rel); break;
            default: w *= 1_dp; h *= 1_dp; break;
        }
        w = (int)roundf((float)w * scale);
        h = (int)roundf((float)h * scale);
    }
    return { w, h };

}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                                        DIVIDER                                                                         //
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

int zDrawableDivider::resolve(int count) const {
    int _size(0), middle(0); count--;
    if(type & ZS_DIVIDER_BEGIN) _size += size + padEnd;
    if(type & ZS_DIVIDER_END) _size += size + padBegin;
    if((type & ZS_DIVIDER_MIDDLE) && count > 0) {
        auto parent((zViewGroup*)view);
        for(int i = 0; i < count; i++) middle += (parent->atView(i)->isVisibled());
        _size += (size + padBegin + padEnd) * middle;
    }
    return _size;
}

void zDrawableDivider::measure(int width, int height, int pivot, bool isSave) {
    if(view->isVertical()) width = view->rclient.w, height = size; else width  = size, height = view->rclient.h;
    zDrawable::measure(width, height, 3, isSave);
}

void zDrawableDivider::init(const zParamDrawable& p) {
    auto sz(&p.size);
    type     = p.type; size     = sz[3];
    padBegin = sz[0];  padEnd   = sz[2];
    zDrawable::init(p);
}

void zDrawableDivider::set(int pos, int pad) {
    static rti r; auto v(view->isVertical());
    r[!v] = view->rclient[!v]; r[v] = pos + pad;
    r[v + 2] = size; r[!v + 2] = view->rclient[!v + 2];
    draw(&r);
}

void zDrawableDivider::make(int extra, int count, int idx) {
    auto _parent((zViewGroup*)view); auto vert(view->isVertical());
//    extra += ((params[zViewTable::TABLE_LINES_SPACE] * 2 - (size + padBegin + padEnd) / 2);
    // если первый и нужен первый
    if(idx == 0 && (type & ZS_DIVIDER_BEGIN)) {
        auto v(_parent->atView(0)); auto p(-(padEnd + size)); if(!v->isVisibled()) v = view, p = 0;
        set(v->edges(vert, false) - v->margin[vert], p);
    }
    if(type & ZS_DIVIDER_MIDDLE) {
        for(int i = 0; i < count - 1; i++) {
            auto v(_parent->atView(i));
            if(v->isVisibled()) set(v->edges(vert, true) + extra + v->margin[vert + 2], padBegin);
        }
    }
    if(count && (type & ZS_DIVIDER_END)) {
        auto v(_parent->atView(count - 1)); if(!v->isVisibled()) v = view;
        set(v->edges(vert, true) + v->margin[vert + 2], padBegin);
    }
}
