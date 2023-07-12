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
    // сформировать значения
    auto vdata(new i32[_countValues]); memcpy(vdata, _values, _countValues * 4);
    // сформировать цвет
    if(!_countColors) _countColors++;
    auto cdata(new i32[_countColors]);
    if(_colors) memcpy(cdata, _colors, _countColors * 4); else cdata[0] = -1;
    // добавить в набор
    values += cd.set(vdata, _countValues);
    colors += cd.set(cdata, _countColors);
    // определить максимальный/все наборы сделать одного размера
    maxVal = INT_MIN;
    for(auto& v : values) {
        int i(-1), count(v.count);
        // если текущий меньше максимального
        if(count < cc) {
            // создать временный по макс. из нулей
            auto data(new i32[cc]); memset(data, 0, cc * 4);
            // скопировать в него текущий набор значений
            memcpy(data, v.data, count * 4);
            delete v.data; v.data = data;
        }
        // определить максимыльный среди всех
        while(++i < count) maxVal = z_max(maxVal, v.data[i]);
    }
    cCharts = cc;
    requestLayout();
    return true;
}

void zViewChart::onMeasure(cszm& spec) {
    zFrameLayout::onMeasure(spec);
    szi _max; int cc(z_max(5, z_max(1, cCharts))), sz;
    auto _ws(spec[vert].size()), _hs(spec[!vert].size());
    if(_ws && spec[vert].isExact()) {
        _max[vert] = _ws;
    } else {
        sz = z_max(minMaxSize[vert * 2], _ws / cc);
        _max[vert] = sz * cc + pad.extent(vert);
        if(_ws) _max[vert] = z_min(_max[vert], _ws);
    }
    // размер одной диаграммы(ширина/высота в зависимости от ориентации)
    sz = z_max(minMaxSize[vert * 2], (_max[vert] - pad.extent(vert)) / cc);
    if(_hs && spec[!vert].isExact()) {
        _max[!vert] = _hs;
    } else {
        _max[!vert] = z_max(sz * 3, minMaxSize[2 - vert * 2] + pad.extent(!vert));
        if(_hs) _max[!vert] = z_min(_max[!vert], _hs);
    }
    setMeasuredDimension(_max.w, _max.h);
    sizeChart = (float)sz;
    maxCharts = (cCharts ? (int)round((float)rclient[vert + 2] / sizeChart) : 0);
    maxCharts = z_min(cCharts, maxCharts);
}

void zViewChart::onDraw() {
    switch(mode) {
        case ZS_CHART_DIAGRAMM:
            for(int j = 0 ; j < cols.size(); j++) {
                auto r(&rects[j]);
                setScale(r->w / 100.0f, r->h / 100.0f);
                drw[DRW_FK]->color.set(cols[j]);
                drw[DRW_FK]->draw(r);
            }
            setScale(1, 1);
            break;
        case ZS_CHART_GRAPH:
            for(int i = 0 ; i < values.size(); i++) dr[i].draw(nullptr);
            break;
    }
    zViewGroup::onDraw();
}

void zViewChart::onLayout(crti &position, bool changed) {
    zFrameLayout::onLayout(position, changed);
    setFirstVisible(fChart);
    auto x((float)rclient.x), y((float)rclient.y);
    auto dy((float)rclient[3 - vert] / (float)maxVal);
    switch(mode) {
        case ZS_CHART_CIRCULAR: makeCircular(x, y, dy); break;
        case ZS_CHART_GRAPH:    makeGraph(x, y, dy); break;
        default:
        case ZS_CHART_DIAGRAMM: makeDiagramm(x, y, dy); break;
    }
//    awakenScroll();
}

int zViewChart::getMaxValue(int _j, int _i, int& c) {
    static int mx[3]; static int cl[3];
    // перенести во временный массив
    for(int j = 0 ; j < values.size(); j++) {
        mx[j] = values[j].data[_i];
        auto col(colors[j].data); auto count(colors[j].count);
        cl[j] = col[_i < count ? _i : count - 1];
    }
/*
    // отсортировать 3 значения по убывающей
    for(int j = 0 ; j < 2; j++) {
        if(mx[j] < mx[j + 1]) {
            std::swap(mx[j], mx[j + 1]);
            std::swap(cl[j], cl[j + 1]);
        }
    }
*/
    c = cl[_j];
    return mx[_j];
}

void zViewChart::makeDiagramm(float x, float y, float dy) {
    rects.free(); cols.free(); rti r; int c;
    auto wpad((float)ipad.extent(false)), hpad((float)ipad.extent(true));
    for(int j = 0 ; j < values.size(); j++) {
        ptf pt(x, y); auto p(pt);
        pt[vert] -= (float)delta;
        auto count(maxCharts + (delta != 0));
        for(int i = 0 ; i < count; i++) {
            auto val(getMaxValue(j, fChart + i, c));
            auto size(dy * (float)val);
            switch(grav >> (!vert * 2)) {
                default:
                // start/top
                case 1: p[!vert] = pt[!vert]; break;
                // end/bottom
                case 2: p[!vert] = pt[!vert] + (rclient[3 - vert] - size); break;
                // center
                case 3: p[!vert] = pt[!vert] + ((rclient[3 - vert] - size) / 2.0f); break;
            }
            r.x = (int)roundf(p.x + 0.5f + !vert * ipad.x);
            r.y = (int)roundf(p.y + 0.5f + vert * ipad.y);
            r.w = vert ? size : sizeChart - wpad;
            r.h = vert ? sizeChart - hpad : size;
            p[vert] += sizeChart;
            rects += r; cols += c;
        }
    }
    drw[DRW_FK]->measure(100, 100, 0, false);
}

void zViewChart::makeCircular(float, float, float) {
}

void zViewChart::makeGraph(float x, float y, float dy) {
    float h((float)rclient.h), py1, py2;
    for(int j = 0 ; j < values.size(); j++) {
        auto diag(&dr[j]);
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
        diag->measure(100, 100, 0, false);
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
        auto _delta(offs[vert] * sizeTouch[vert]);
        if(_delta) {
            flyng->stop();
            // определяем время сдвига
            if(event && flyng->start(touch, _delta)) clickItem = -1;
                // иначе - просто скроллим на дельту
            else scrolling(_delta);
            // признак перетаскивания
            drag = true;
        }
    });
    // если не было перетаскивания
    if(!drag) {
        // если тап
        if(touch->isCaptured()) {
            // останавливаем флинг
            flyng->stop();
            if(clickItem == -1) {
                // определяем индекс куда тапнули
                if((clickItem = itemFromPoint(touch->cpt)) != -1) {
                    // вызов события
                    post(MSG_SELECTED, duration, clickItem + fChart);
                    // запоминаем выделенный
                    selectItem = clickItem;
                }
            }
        } else {
            if(touch->isReleased() && selectItem == itemFromPoint(touch->cpt)) {
                // отправляем клик
                if(onClick) onClick(atView(selectItem), selectItem + fChart);
            }
            clickItem = -1;
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
    if(!upd) updateGlow(_delta); else requestPosition();
    return !upd;
}

int zViewChart::itemFromPoint(cptf& pt) const {
    szf _sz(rclient.x, rclient.y);
    int idx(fChart), count(maxCharts + (delta != 0));
    _sz[vert] -= (float)delta;
    for(int i = 0; i < count; i++) {
        auto s(_sz[vert] + sizeChart);
        if(pt[vert] > _sz[vert] && pt[vert] < s) return idx;
        _sz[vert] = s; idx++;
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
    return INT_MIN;
}
