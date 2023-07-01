//
// Created by User on 14.06.2022.
//

#include "zssh/zCommon.h"
#include "zssh/zViewGroup.h"

zViewChart::zViewChart(zStyle* _styles, i32 _id, u32 _chartGravity) : zFrameLayout(_styles, _id), grav(_chartGravity) {
    minMaxSize.set(z_dp(z.R.dimen.chartMinWidth), 0, z_dp(z.R.dimen.chartMinHeight), 0);
    sizeTouch.set(3_dp, 3_dp); vert = (_chartGravity & ZS_GRAVITY_HORZ);
    flyng = new zFlyng(this);
}

void zViewChart::onInit(bool _theme) {
    zFrameLayout::onInit(_theme);
    setMode(styles->_int(Z_MODE, ZS_CHART_DIAGRAMM));
}

void zViewChart::delData(int idx) {
    if(idx >= 0 && idx < values.size()) {
        values.erase(idx, 1, true);
        requestLayout();
    }
}

bool zViewChart::addData(i32* _values, int _countValues, u32* _colors, int _countColors) {
    static CHART_DATA cd;
    if(values.size() > 3) {
        ILOG("Возможно маскимум 3 набора данных!!!");
        return false;
    }
    if(!_values || !_countValues) return false;
    auto cc(z_max(cCharts, _countValues));
    auto vdata(new i32[_countValues]); memcpy(vdata, _values, _countValues * 4);
    if(!_countColors) _countColors++;
    auto cdata(new i32[_countColors]);
    if(_colors) memcpy(cdata, _colors, _countColors * 4); else cdata[0] = -1;
    values += cd.set(vdata, _countValues);
    colors += cd.set(cdata, _countColors);
    maxVal = INT_MIN;
    for(auto& v : values) {
        int i(-1), count(v.count);
        if(count < cc) {
            auto data(new i32[cc]); memset(data, 0, cc * 4);
            memcpy(data, v.data, count * 4);
            delete v.data; v.data = data;
        }
        while(++i < count) maxVal = z_max(maxVal, v.data[i]);
    }
    cCharts = cc;
    requestLayout();
    return true;
}

void zViewChart::onMeasure(cszm& spec) {
    zFrameLayout::onMeasure(spec);
    auto ws(spec.w.size()), hs(spec.h.size());
    auto wm(spec.w.mode()), hm(spec.h.mode());
    int width(0), height(0), cc(z_max(1, cCharts)), cpad(ipad.extent(vert)), sz;
    auto _w(vert ? &height : &width), _h(vert ? &width : &height);
    auto _ws(vert ? hs : ws), _wm(vert ? hm : wm);
    auto _hs(vert ? ws : hs), _hm(vert ? wm : hm);
    if(_ws && _wm == MEASURE_EXACT) {
        *_w = _ws;
    } else {
        cc = z_min(cc, 20);
        sz = z_min(minMaxSize.x, _ws / cc) + cpad;
        *_w += sz * cc;
        *_w += pad.extent(vert);
        if(_ws) *_w = z_min(*_w, _ws);
    }
    sz = z_max(minMaxSize.x, (*_w - pad.extent(vert)) / cc);
    if(_hs && _hm == MEASURE_EXACT) {
        *_h = _hs;
    } else {
        *_h += pad.extent(!vert);
        *_h += sz * minMaxSize.w;
        if(_hs) *_h = z_min(*_h, _hs);
    }
    setMeasuredDimension(width, height);
    sizeChart = (float)sz;
    maxCharts = (cCharts ? (int)round((float)(vert ? rclient.h : rclient.w) / sizeChart) : 0);
    maxCharts = z_min(cCharts, maxCharts);
}

void zViewChart::onDraw() {
    int idx(0);
    switch(mode) {
        case ZS_CHART_DIAGRAMM:
            for(int j = 0 ; j < values.size(); j++) {
                auto c(colors[j].data); auto _count(colors[j].count);
                auto count(maxCharts + (delta != 0));
                for(int i = 0; i < count; i++) {
                    auto r(&rects[idx++]);
                    setScale(r->w / 100.0f, r->h / 100.0f);
//                    setTranslate((int) round(r->x), (int) round(r->y));
                    drw[DRW_FK]->color.set(i < _count ? c[i] : c[_count - 1]);
                    drw[DRW_FK]->draw(nullptr);
                }
            }
            break;
        case ZS_CHART_GRAPH:
            for(int i = 0 ; i < values.size(); i++) {
//                drw[DRW_IDX_BUT1 + i]->draw();
            }
            break;
    }
//    for(auto v : children) v->draw();
}

void zViewChart::onLayout(crti &position, bool changed) {
    zFrameLayout::onLayout(position, changed);
    setFirstVisible(fChart);
    auto x((float)rclient.x), y((float)rclient.y);
    auto dy((float)((vert ? rclient.w : rclient.h) - ipad.extent(vert)) / (float)maxVal);
    switch(mode) {
        case ZS_CHART_CIRCULAR: makeCircular(x, y, dy); break;
        case ZS_CHART_GRAPH:    makeGraph(x, y, dy); break;
        default:
        case ZS_CHART_DIAGRAMM: makeDiagramm(x, y, dy); break;
    }
    awakenScroll();
}

void zViewChart::makeDiagramm(float x, float y, float dy) {
    float yy, xx;
    auto w((float)rclient.w), h((float)rclient.h);
    rects.free();
    auto wpad((float)ipad.extent(false)), hpad((float)ipad.extent(true));
    for(auto& v : values) {
        auto data(v.data);
        auto px(x + (float)ipad.x), py(y + (float)ipad.y);
        if(vert) py -= (float)delta; else px -= (float)delta;
        auto count(maxCharts + (delta != 0));
        for(int i = 0 ; i < count; i++) {
            auto size(dy * (float)data[fChart + i]);
            if(vert) {
                switch(grav & ZS_GRAVITY_HORZ) {
                    default:
                    case ZS_GRAVITY_START:   xx = px + (w - size); break;
                    case ZS_GRAVITY_HCENTER: xx = px + ((w - size) / 2.0f); break;
                    case ZS_GRAVITY_END:     xx = px; break;
                }
                rects += rtf(xx, py, size, sizeChart - hpad);
                py += sizeChart;
            } else {
                switch(grav & ZS_GRAVITY_VERT) {
                    default:
                    case ZS_GRAVITY_TOP:     yy = py + (h - size); break;
                    case ZS_GRAVITY_VCENTER: yy = py + ((h - size) / 2.0f); break;
                    case ZS_GRAVITY_BOTTOM:  yy = py; break;
                }
                rects += rtf(px, yy, sizeChart - wpad, size);
                px += sizeChart;
            }
        }
    }
    drw[DRW_FK]->measure(100, 100, 0, false);
}

void zViewChart::makeCircular(float, float, float) {
}

void zViewChart::makeGraph(float x, float y, float dy) {
    float h((float)rclient.h), py1, py2;
    for(int j = 0 ; j < values.size(); j++) {
        //auto dr(drs[DR_IDX_BUT1 + j]);
/*
        if(!dr) {
            dr = new zDrawable(this, DRW_FK);
            dr->set(drw[DRW_FK], z.R.integer.rect);
            //drs[DR_IDX_BUT1 + j] = dr;
        }
*/
        auto diag(drw[DRW_FK]);//drs[DR_IDX_BUT1 + j]);
        auto data(values[j].data); auto x1(x);
        auto count(maxCharts + (delta != 0));
        diag->make(4 + count * 2);
        auto _tex(diag->texture->getReverseSize()); rti tex(diag->texture->getTile(z.R.integer.rect)->rect);
        tex.w += tex.x; tex.h += tex.y;
        auto tw(_tex.w), th(_tex.h), tx1((float)tex.x * tw), ty1((float)tex.y * th), tx2((float)tex.w * tw), ty2((float)tex.h * th);
        auto v(diag->vertices); bool top((grav & ZS_GRAVITY_VERT) == ZS_GRAVITY_TOP);
        for(int i = 0 ; i < count; i++) {
            auto h2(dy * (float)data[fChart + i]), x2(x1 + sizeChart);
            if(top) py2 = y + (h - h2); else py2 = (y + h2), h2 = -h2;
            if(i == 0) {
                auto h1(fChart ? dy * (float)data[fChart + i - 1] : 0);
                if(top) py1 = y + (h - h1); else py2 = (y + h1), h1 = -h1;
                v->x = x1; v->y = py1;     v->u = tx1; v->v = ty1; v++;
                v->x = x1; v->y = py1 + h1;v->u = tx1; v->v = ty2; v++;
                v->x = x2; v->y = py2;     v->u = tx2; v->v = ty1; v++;
                v->x = x2; v->y = py2 + h2;v->u = tx2; v->v = ty2; v++;
            }
            v->x = x2; v->y = py2;     v->u = tx2; v->v = ty1; v++;
            v->x = x2; v->y = py2 + h2;v->u = tx2; v->v = ty2; v++;
            x1 += sizeChart;
        }
        //diag->layout();
        //diag->setTranslate(rclient.x - delta, (int)py1);
        diag->color.set(colors[j].data[0]);
    }
}

void zViewChart::setFirstVisible(int _first) {
    if((_first + maxCharts) > cCharts) _first = cCharts - maxCharts;
    else if(_first < 0) _first = 0;
    if(fChart != _first) { fChart = _first; invalidate(); }
}

i32 zViewChart::onTouchEvent(zTouch *touch) {
    bool drag(false);
    touch->drag(sizeTouch, [this, &drag, &touch](cszi& offs, bool event) {
        // если сдвинули - определяем дельту в зависимости от ориентации
        auto _delta(offs[vert]);
        if(_delta) {
            // определяем время сдвига
            auto t((int)((touch->ctm - touch->btm) / 50000000));
            // если отпустили и время < 10(выбрано экспериментально)
            if(event && t < 15) {
                // запускаем флинг
//                flyng->pull((float)_delta / (float)sizes(vert), _delta > 0, vert);
  //              flyng->setVisible(true);
                // отправляем событие флинга
    //            post(MSG_FLYNG, nullptr);
            } else {
                // иначе - просто скроллим на дельту
                scrolling(_delta);
                // отправляем событие скролла
                if(onChangeScroll) onChangeScroll(this, _delta);
            }
            drag = true;
            clickItem = selectItem = -1;
        }
    });
    if(!drag) {
        if(touch->isCaptured()) {
            flyng->stop();
            if(clickItem == -1) {
                // определяем индекс куда тапнули
                clickItem = itemFromPoint(touch->cpt);
                // вызов события
                if(clickItem != -1 && selectItem == -1) {
                    if(onChangeSelected) onChangeSelected(this, clickItem);
                }
                // запоминаем выделенный
                selectItem = clickItem;
            }
        } else {
            if(touch->isReleased() && selectItem != -1) {
                // отправляем клик
                post(MSG_CLICK, 50, selectItem);
            }
            // если тапа нет - сбрасываем
            selectItem = clickItem = -1;
        }
    }
    return drag | touch->isCaptured();
}

bool zViewChart::scrolling(int _delta) {
    auto end(computeScrollRange(vert)), rc(maxCharts * (int)sizeChart), dx(computeScrollOffset(vert));
    auto d(delta), f(fChart);
    dx -= _delta;
    if(_delta < 0 && (dx + rc) >= end) dx = end - rc;
    if(_delta >= 0 && dx < 0) dx = 0;
    setFirstVisible(dx / (int)sizeChart);
    auto sz((int)sizeChart); delta = dx % sz;
    auto upd(d != delta || f != fChart);
    if(!upd) updateGlow(_delta); else invalidate();
    return !upd;
}

int zViewChart::itemFromPoint(cptf& pt) const {
    float s;
    auto x((float)rclient.x), y((float)rclient.y);
    int idx(fChart), count(maxCharts + (delta != 0));
    if(vert) y -= (float)delta; else x -= (float)delta;
    auto xx(pt.x), yy(pt.y);
    for(int i = 0; i < count; i++) {
        if(vert) {
            s = y + sizeChart;
            if(yy >= y && yy < s) return idx;
            y = s;
        } else {
            s = x + sizeChart;
            if(xx >= x && xx < s) return idx;
            x = s;
        }
        idx++;
    }
    return -1;
}

int zViewChart::getValue(int idx, int kit) const {
    if(idx >= 0 && idx < cCharts) {
        if(kit >= 0 && kit < values.size()) {
            auto data(values[kit].data);
            return data[idx];
        }
    }
    return -1;
}
