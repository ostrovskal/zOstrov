//
// Created by User on 18.04.2023.
//

#include "zostrov/zCommon.h"

static zParamDrawable drDef[DR_COUNT] = {
    // tex                      col     pad     tl      sh      sz      tp      sc
    {z.R.drawable.zssh,     0x0,    0,      -1,     0,          8,      0,      1.0f }, // DR_MSK
    {z.R.drawable.zssh,     0x0,    0,      -1,     0,          8,      0,      1.0f }, // DR_BK
    {z.R.drawable.zssh,     0x0,    0,      -1,     0,          8,      0,      1.0f }, // DR_FK
    {z.R.drawable.zssh,     0x0,    0,      -1,     0,          8,      0,      1.0f }, // DR_SEL
    {0,                     0x0,    0,      -1,     0,          8,      0,      1.0f }, // DR_TXT
    {z.R.drawable.zssh,    -1U,     0,      -1,     0,          8,      0,      1.0f }, // DR_ICON
    {0,                     0x0,    0,      -1,     0,          8,      ZS_DIVIDER_MIDDLE, 1.0f }, // DR_DIV
    {0xffffffff,            0x0,    0,      -1,     0,          -1,     -1,     1.0f }, // DR_SCL
    {0x01000000,            0x0,    0,      z.R.integer.rect,0, 8,      0,      1.0f }  // DR_BTXT
};

void zParamDrawable::set(u32 bs, u32 idx, int val) {
    if(scale == 0.0f) {
        memcpy(this, &drDef[(int)(this - zView::drParams)], sizeof(zParamDrawable));
    }
    params[(idx & ZTV_MASK) - (bs & ZTV_MASK)] = val;
}

void zParamDrawable::setDefaults() {
    memset(zView::drParams, 0, sizeof(zView::drParams));
    // прокрутка
    zView::drParams[DR_SCL].size = -1; zView::drParams[DR_SCL].type = -1; zView::drParams[DR_SCL].tiles = -1;
    // шрифт по умолчанию
    zView::drParams[DR_TXT].texture = theme->styles->_int(Z_THEME_FONT, z.R.drawable.font_def );
    drDef[DR_TXT].texture = zView::drParams[DR_TXT].texture;
    // цвет текста по умолчанию
    zView::drParams[DR_TXT].color = theme->styles->_int(Z_THEME_COLOR_TEXT_TEXT, 0xffaabbcc);
    drDef[DR_TXT].color = zView::drParams[DR_TXT].color;
    // divider
    drDef[DR_DIV].texture = theme->styles->_int(Z_THEME_COLOR_DIVIDER, 0xff7f7f7f);
    // цвет фона текста
    zView::drParams[DR_BTXT].texture = 0x01000000;
    zView::drParams[DR_BTXT].tiles = z.R.integer.rect;
}

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

int zDrawable::makePath(const zArray<ptf>& pth, crti& tex, ptf b, int pivot) {
    make(pth.size() * 2 + 1);
    auto _tex(texture->getReverseSize());
    rtf t((float)tex.x * _tex.w, (float)tex.y * _tex.h, (float)tex.w * _tex.w, (float)tex.h * _tex.h);
    auto v(vertices);
    for(int i = 0 ; i < pth.size() ; i++) {
        auto p(pth[i]); auto tx((i & 1) ? t.w : t.x);
        v->x = p.x; v->y = p.y; v->u = tx; v->v = t.y; v++;
        v->x = b.x; v->y = b.y; v->u = tx; v->v = t.h; v++;
        if(pivot & PIVOT_X) b.x = p.x;
        if(pivot & PIVOT_Y) b.y = p.y;
    }
    v->x = b.x; v->y = b.y; v->u = t.x; v->v = t.h;
    return count;
}
int zDrawable::makeQuad(crti& pos, crti& tex, zVertex2D* v, int _italic) const {
    auto _tex(texture->getReverseSize()); auto offs((float)_italic);
    auto px1((float)pos.x), py1((float)pos.y), px2(px1 + (float)pos.w), py2(py1 + (float)pos.h);
    auto tx1((float)tex.x * _tex.w), ty1((float)tex.y * _tex.h), tx2((float)tex.w * _tex.w), ty2((float)tex.h * _tex.h);
    v->x = px1 + offs;  v->y = py1; v->u = tx1; v->v = ty1; v++;
    v->x = px1;         v->y = py2; v->u = tx1; v->v = ty2; v++;
    v->x = px2 + offs;  v->y = py1; v->u = tx2; v->v = ty1; v++;
    v->x = px2;         v->y = py2; v->u = tx2; v->v = ty2;
    return 4;
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

static zVertex2D* makeDebugRect(float x, float y, float w, float h, zVertex2D* v) {
    auto x1(x), x2(x + w), y1(y), y2(y + h);
    v->x = x1; v->y = y1; v++;
    v->x = x2; v->y = y1; v++;	// top
    v->x = x1; v->y = y1; v++;
    v->x = x1; v->y = y2; v++;	// left
    v->x = x2; v->y = y1; v++;
    v->x = x2; v->y = y2; v++;	// right
    v->x = x1; v->y = y2; v++;
    v->x = x2; v->y = y2; v++;  // bottom
    return v;
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

int zDrawable::makePatch9(crti& pos, crti& tex, rti p9) const {
    if(pos.isEmpty()) return 0;
    int _count(0); rti p, t;
    static zVertex2D tmpVerts[36];
    static u8 vnum[] = {0, 1, 2, 3, 6, 7, 10, 11, 11, 12,15, 13, 19, 17, 17, 22, 21, 20, 25, 24, 24, 28, 31, 30, 35, 34};
    float sh((float)pos.h / (float)z_max(pos.h, tex.h - tex.y)), sw((float)pos.w / (float)z_max(pos.w, tex.w - tex.x));
    if((1.0f - sw) > Z_EPSILON) { p9.x = z_round((float)p9.x * sw + 0.5f); p9.w = z_round((float)p9.w * sw + 0.5f); }
    if((1.0f - sh) > Z_EPSILON) { p9.y = z_round((float)p9.y * sh + 0.5f); p9.h = z_round((float)p9.h * sh + 0.5f); }
    for(int i = 0; i < 9 * 8; i += 8) {
        calcPatch9(i + 0, p9, pos, tex); t.x = y9; p.x = x9;
        calcPatch9(i + 2, p9, pos, tex); t.y = y9; p.y = x9;
        calcPatch9(i + 4, p9, pos, tex); t.w = y9; p.w = x9 - p.x;
        calcPatch9(i + 6, p9, pos, tex); t.h = y9; p.h = x9 - p.y;
        _count += makeQuad(p, t, &tmpVerts[i / 2]);
    }
    // переделать в стрип
    for(int i = 0; i < 26; i++) vertices[i] = tmpVerts[vnum[i]];
    return 26;
}

void zDrawable::draw(rti *rect) {
    static zMatrix t, m; static rti clip;
    if(vertices && visible) {
        switch(index) {
            case DRW_BK: clip = view->drawableClip(); break;
            case DRW_FBO: clip = view->parent->rclip; break;
            default: clip = view->rclip; break;
        }
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
            m *= t.translate(offs.x, offs.y, 0);
            drawCommon(clip, m, fbo);
            color.a = _a;
        }
    }
}

void zDrawable::drawCommon(crti& clip, const zMatrix& m, bool fbo) const {
    auto _clip(clip);
    // устанавливаем текстуру
    glBindTexture(texture->type, texture->id);
    // устанавливаем параметры шейдеров и обрезки
    manager->prepareRender(shader, vertices, _clip - manager->screen, m);
    // устанавливаем цветовой фильтр
    manager->setColorFilter(fbo ? view : nullptr, color);
    // запускаем отрисовку
    glDrawArrays(typeTri, 0, count);
    // подсчет кол-во треугольников в кадре
    manager->countVertices += count;
    manager->countObjs++;
}

void zDrawable::measure(int width, int height, int pivot, bool isSave) {
    zTexture::TILE_TTL* _tile;
    if(index >= DRW_FBO) {
        // проверить на текстуру
        if(!(texture && texture->isEqual(width, height))) {
            if(!texture) texture = new zTexture();
            else {
                auto sz(texture->getSize());
                DLOG("resize makeFBO %s(%i) %i(%i)x%i(%i)", view->typeName(), view->id, sz.w, width, sz.h, height);
            }
            if(!texture->makeFBO(width, height)) {
                ILOG("error makeFBO %s(%i) %ix%i", view->typeName(), view->id, width, height);
                release(); return;
            }
            manager->volumeVideoMemory(width * height * 4, true);
            DLOG("ok makeFBO %s(%i) %ix%i", view->typeName(), view->id, width, height);
        }
    }
    if(texture && (_tile = texture->getTile(tile))) {
        rti tex(_tile->rect);
        // определение патча
        auto isPatch9(_tile->patch9 != 0 && !(pivot & PIVOT_QUAD));
        // определение количества вершин
        make(isPatch9 * 22 + 4);
        // определение ректа
        if(!isSave) {
            if(pivot & PIVOT_W) width = tex.w;
            if(pivot & PIVOT_H) height = tex.h;
            bound.set((pivot & PIVOT_X) * -(width >> 1), ((pivot & PIVOT_Y) != 0) * -(height >> 1), width, height);
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
            makePatch9(bound, tex, (u8*)&_tile->patch9);
        } else {
            // формирование квада
            makeQuad(bound, tex, vertices);
        }
    } else {
        clear();
    }
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
    auto tmp(0), stx(0); rti _pos, tex; count = 0;
    zTexture::TILE_TTL* glp;
    while(len-- > 0 && z_is8(text)) {
        auto ch(z_decode8(z_char8(text, &tmp)));
        if((glp = paint->getBoundChar(ch, tex, _pos))) {
            auto l(glp->getLeft()), r(glp->getRight()); int w;
            if(paint->getStyle() & ZS_TEXT_MONO) {
                w = paint->getFactor((float)(glp->rect.w + r + l)); l = 0;
            } else {
                auto ll(paint->getFactor((float)l)), rr(paint->getFactor((float)r));
                l = l < 0 ? z_max(l, ll) : z_min(l, ll);
                r = r < 0 ? z_max(r, rr) : z_min(r, rr);
                w = l + _pos.w + r;
            }
            _pos.x += stx + l;
            // нарисовать
            count += makeTriangle(_pos, tex, &vertices[count], paint->getItalic());
            stx += w; text += tmp;
        }
    }
    paint->getBoundChar('_', tex, _pos); tex = tex.padding(4, 0);
    stx += paint->getItalic();
    // strike
    if(paint->isStrike()) {
        _pos.set(0, paint->getSize() / 2 + 1, stx, paint->getFactor(2.0f));
        count += makeTriangle(_pos, tex, &vertices[count], 0);
    }
    // underline
    if(paint->isUnderline()) {
        auto bs(paint->getFactor((float)paint->getBaseline()));
        _pos.set(0, paint->getSize() - bs + 1, stx, paint->getFactor(2.0f));
        count += makeTriangle(_pos, tex, &vertices[count], 0);
    }
    return stx;
}

ptf zDrawable::offsetBound() const {
    auto rv(&manager->screen);
    auto x(vertices ? vertices->x : 0), y(vertices ? vertices->y : 0);
    return { (float)(bound.x - rv->x) - x, (float)(bound.y - rv->y) - y };
}

float zDrawable::scaleFactor(u32 value, bool text) const {
    auto h((float)(texture ? texture->getSize().h / (text + 1) : 1));
    return ((float)value / h);
}

void zDrawable::info() const {
    DLOG("%s", z_fmt("bound(%i-%i:%i-%i) col:%x tex:%s count:%i tile:%i",
                     bound.x, bound.y, bound.w, bound.h, color.toABGR(), color.g, color.b, color.a,
                     texture ? texture->name.str() : "null", count, tile).str());
}

void zDrawable::init(const zDrawable* drw, int _tile) {
    texture     = manager->cache->get(drw->texture->name, texture);
    padding     = drw->padding;
    color       = drw->color;
    tile        = _tile;
    shader      = drw->shader;
}

void zDrawable::init(u32 _tx, u32 _cl, i32 _tl, u32 _pd, float _sc, int sh) {
    if(_tx & 0xff000000) {
        if(!_cl) _cl = _tx; if(_tl == -1) _tl = z.R.integer.rect;
        _tx = z.R.drawable.zssh;
    } else if(!_cl) _cl = theme->themeColor;
    padding.set(_pd); color.set(_cl);
    scale = _sc; shader = sh;
    texture = manager->loadResourceTexture(_tx, texture);
    tiles = _tl; setTileNum(0);
}

void zDrawable::drawFBO(zView* vw, zView* prev) {
    if(texture) {
        // установить целевую текстуру
        setFBO(true, true);
        zView::fbo = vw;
        manager->screen = vw->rview;
        // нарисовать в текстуру
        vw->drw[DRW_BK]->draw(&vw->rview);
        vw->onDraw();
        // форма обрезки
        if(vw->drw[DRW_MSK]->isValid()) vw->drawShape();
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
        auto _tile(texture->getTile(tile));
        if(_tile) {
            w = _tile->rect.w * 1_dp, h = _tile->rect.h * 1_dp;
            auto rel((float)w / (float)h);
            switch(gravity & ZS_SCALE_MASK) {
                case ZS_SCALE_HEIGHT:
                    h = w = z_round((float)z_min(h, hmax) * scale);
                    break;
                case ZS_SCALE_WIDTH:
                    w = h = z_round((float)z_min(w, wmax) * scale);
                    break;
                case ZS_SCALE_MIN:
                    h = z_round((float)z_min(w, h) * scale);
                    w = z_round((float)h * rel);
                    break;
                case ZS_SCALE_MAX:
                    h = z_round((float)z_max(w, h) * scale);
                    w = z_round((float)h * rel);
                    break;
                default:
                    w = z_round((float)w * scale);
                    h = z_round((float)h * scale);
                    break;
            }
        }
    }
    return { w, h };
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                                        DIVIDER                                                                         //
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

int zDrawableDivider::getSize(int flags) const {
    int ret(size);
    switch(type & flags) {
        case ZS_DIVIDER_MIDDLE: ret += padBegin + padEnd; break;
        case ZS_DIVIDER_END: ret += padBegin; break;
        case ZS_DIVIDER_BEGIN: ret += padEnd; break;
        default: ret = 0; break;
    }
    return ret;
}

int zDrawableDivider::resolve(int count, bool _visible) const {
    int _size(0), middle(0); count--;
    if(type & ZS_DIVIDER_BEGIN) _size += size + padEnd;
    if(type & ZS_DIVIDER_END) _size += size + padBegin;
    if((type & ZS_DIVIDER_MIDDLE) && count > 0) {
        if(_visible) {
            auto parent((zViewGroup*)view);
            for(int i = 0; i < count; i++) middle += (parent->atView(i)->isVisibled());
        } else middle = count;
        _size += (size + padBegin + padEnd) * middle;
    }
    return _size;
}

void zDrawableDivider::measure(int width, int height, int pivot, bool isSave) {
    if(view->isVertical()) width = view->rclient.w, height = size; else width  = size, height = view->rclient.h;
    zDrawable::measure(width, height, PIVOT_ALL, isSave);
}

void zDrawableDivider::init(const zParamDrawable& p) {
    auto sz((u8*)&p.size);
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

void zDrawableDivider::make(int extra, int countItems, int idx) {
    auto _parent((zViewGroup*)view); auto vert(view->isVertical());
    auto childCount(_parent->countChildren());
    auto alles(childCount && (idx + childCount) == countItems);
    if(!childCount) return;
    // если первый и нужен первый
    if(idx == 0 && (type & ZS_DIVIDER_BEGIN)) {
        auto v(_parent->atView(0)); auto p(-(padEnd + size)); if(!v->isVisibled()) v = view, p = 0;
        set(v->edges(vert, false) - v->margin[vert], p);
    }
    if(type & ZS_DIVIDER_MIDDLE) {
        for(int i = 0; i < childCount - alles; i++) {
            auto v(_parent->atView(i));
            if(v->isVisibled()) set(v->edges(vert, true) + extra + v->margin[vert + 2], padBegin);
        }
    }
    if(alles && (type & ZS_DIVIDER_END)) {
        auto v(_parent->atView(childCount - 1)); if(!v->isVisibled()) v = view;
        set(v->edges(vert, true) + v->margin[vert + 2], padBegin);
    }
}

void zDrawableFake::drawFBO(zView* vw, zView* prev) {
    vw->drw[DRW_BK]->draw(&vw->rview);
    vw->onDraw();
    // форма обрезки
    if(vw->drw[DRW_MSK]->isValid()) vw->drawShape();
}
