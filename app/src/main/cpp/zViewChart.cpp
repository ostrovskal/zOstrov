//
// Created by User on 14.06.2022.
//

#include "zssh/zCommon.h"
#include "zssh/zViewGroup.h"

zViewChart::zViewChart(zStyle* _styles, i32 _id, u32 _chartGravity) : zFrameLayout(_styles, _id), grav(_chartGravity) {
    minMaxSize.set(z_dp(z.R.dimen.chartMinWidth), 0, z_dp(z.R.dimen.chartMinHeight), 0);
    sizeTouch.set(3_dp, 3_dp); vert = (_chartGravity & ZS_GRAVITY_HORZ);
    flyng = new zFlyng(this);
    dr[0] = new zDrawable(this, DRW_FK);
    dr[1] = new zDrawable(this, DRW_FK);
    dr[2] = new zDrawable(this, DRW_FK);
}

zViewChart::~zViewChart() {
    values.clear();
    colors.clear();
    SAFE_DELETE(dr[0]);
    SAFE_DELETE(dr[1]);
    SAFE_DELETE(dr[2]);
}

void zViewChart::onInit(bool _theme) {
    zFrameLayout::onInit(_theme);
    setMode(styles->_int(Z_MODE, ZS_CHART_DIAGRAMM));
    dr[0]->init(drw[DRW_FK], drw[DRW_FK]->tile);
    dr[1]->init(drw[DRW_FK], drw[DRW_FK]->tile);
    dr[2]->init(drw[DRW_FK], drw[DRW_FK]->tile);
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
        sz = z_max(minMaxSize.x, _ws / cc);
        _max[vert] = sz * cc + pad.extent(vert);
        if(_ws) _max[vert] = z_min(_max[vert], _ws);
    }
    // размер одной диаграммы(ширина/высота в зависимости от ориентации)
    sz = z_max(minMaxSize.x, (_max[vert] - pad.extent(vert)) / cc);
    if(_hs && spec[!vert].isExact()) {
        _max[!vert] = _hs;
    } else {
        _max[!vert] = z_max(sz * 5, minMaxSize.w + pad.extent(!vert));
        if(_hs) _max[!vert] = z_min(_max[!vert], _hs);
    }
    setMeasuredDimension(_max.w, _max.h);
    sizeChart = (float)sz;
    maxCharts = (cCharts ? (int)round((float)rclient[vert + 2] / sizeChart) : 0);
    maxCharts = z_min(cCharts, maxCharts);
}

void zViewChart::onDraw() {
    auto scr(manager->screen);
    switch(mode) {
        case ZS_CHART_DIAGRAMM:
            m.identity();
            for(int j = 0 ; j < cols.size(); j++) {
                auto r(&rects[j]);
                m.scale2(r->w / 100.0f, r->h / 100.0f, 1.0f);
                m.translate2(r->x - scr.x, r->y - scr.y, 0);
                dr[0]->color.set(cols[j]);
                dr[0]->drawCommon(rclip, m, true);
            }
            break;
        case ZS_CHART_GRAPH:
            for(int i = 0 ; i < values.size(); i++) {
                // установить текстуру
                glBindTexture(GL_TEXTURE_2D, dr[i]->texture->id);
                // параметры шейдера и обрезка
                manager->prepareRender(dr[i]->vertices, manager->screen, zMatrix::_identity);
                // фильтр
                manager->setColorFilter(this, colors[i].data[0]);
                // нарисовать линии
                glLineWidth(5);
                glDrawArrays(GL_LINE_STRIP, 0, dr[i]->count);
            }
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
    awakenScroll();
}

void zViewChart::makeDiagramm(float x, float y, float dy) {
    rects.free(); cols.free(); rti r;
    auto wpad((float)ipad.extent(false)), hpad((float)ipad.extent(true));
    for(int j = 0 ; j < values.size(); j++) {
        auto _vals(values[j].data);
        auto _cols(colors[j].data);
        auto _count(colors[j].count);
        ptf pt(x, y); auto p(pt);
        pt[vert] -= (float)delta;
        auto count(maxCharts + (delta != 0));
        for(int i = 0 ; i < count; i++) {
            auto size(dy * (float)_vals[i + fChart]);
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
            rects += r;
            cols += _cols[_count > (i + fChart) ? (i + fChart) : (_count - 1)];
        }
    }
    dr[0]->measure(100, 100, 0, false);
}

void zViewChart::makeCircular(float, float, float) {
}

#include <zstandard/zSpline.h>

void zViewChart::makeGraph(float x, float y, float dy) {
    for(int j = 0 ; j < values.size(); j++) {
        auto _vals(values[j].data);
        ptf pt(x, y); auto p(pt);
        zArray<ptf> path;
        pt[vert] -= (float)delta;
        auto count(maxCharts);
        for(int i = 0 ; i < count; i++) {
            auto size(dy * (float)_vals[i + fChart]);
            switch(grav >> (!vert * 2)) {
                default:
                // start/top
                case 1: p[!vert] = pt[!vert]; break;
                // end/bottom
                case 2: p[!vert] = pt[!vert] + (rclient[3 - vert] - size); break;
                // center
                case 3: p[!vert] = pt[!vert] + ((rclient[3 - vert] - size) / 2.0f); break;
            }
            path += ptf(p.x + !vert * ipad.x, p.y + vert * ipad.y);
            p[vert] += sizeChart;
        }
        path += ptf(p.x, p.y);
        smoothPath(path, 5, true);
        dr[j]->make(path.size());
        //dr[j]->typeTri = GL_LINE_STRIP;
        dr[j]->texture = manager->cache->get("znull", dr[j]->texture);
        auto vert(dr[j]->vertices);
        for(int ii = 0; ii < path.size(); ii++) {
            auto& _p(path[ii]);
            //PTF_LOG("_p", _p);
            vert->x = _p.x; vert->y = _p.y;
            vert++;
        }
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
                    post(MSG_SELECTED, duration, clickItem);
                    // запоминаем выделенный
                    selectItem = clickItem;
                }
            }
        } else {
            if(touch->isReleased() && selectItem == itemFromPoint(touch->cpt)) {
                // отправляем клик
                if(onClick) onClick(atView(selectItem), selectItem);
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
