//
// Created by User on 14.06.2022.
//

#include "zostrov/zCommon.h"
#include "zostrov/zViewGroup.h"
#include <zstandard/zSpline.h>

zViewChart::zViewChart(zStyle* _styles, i32 _id, u32 _chartGravity) : zFrameLayout(_styles, _id), grav(_chartGravity) {
    minMaxSize.set(z_dp(z.R.dimen.chartMinWidth), 0, z_dp(z.R.dimen.chartMinHeight), 0);
    sizeTouch.set(4_dp, 4_dp); vert = (_chartGravity & ZS_GRAVITY_HORZ);
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
        // определить максимальный среди всех
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
    auto circ(mode == ZS_CHART_CIRCULAR);
    if(_ws && spec[vert].isExact()) {
        _max[vert] = _ws;
    } else {
        if(circ) {
            _max[vert] = z_max(256_dp, cc * 12_dp);
        } else {
            sz = z_max(minMaxSize.x, _ws / cc);
            _max[vert] = sz * cc + pad.extent(vert);
        }
        if(_ws) _max[vert] = z_min(_max[vert], _ws);
    }
    // размер одной диаграммы(ширина/высота в зависимости от ориентации)
    sz = z_max(minMaxSize.x, (_max[vert] - pad.extent(vert)) / cc);
    if(_hs && spec[!vert].isExact()) {
        _max[!vert] = _hs;
    } else {
        if(circ) {
            _max[!vert] = z_max(256_dp, cc * 12_dp);
        } else {
            _max[!vert] = z_max(sz * 3, minMaxSize.w + pad.extent(!vert));
        }
        if(_hs) _max[!vert] = z_min(_max[!vert], _hs);
    }
    if(circ) _max.w = z_min(_max.w, _max.h);
    setMeasuredDimension(_max.w, _max.h);
    sizeChart = (float)sz;
    maxCharts = (cCharts ? (int)round((float)rclient[vert + 2] / sizeChart) : 0);
    maxCharts = z_min(cCharts, maxCharts);
}

void zViewChart::onDraw() {
    auto scr(manager->screen);
    m.translate((float)(rclient.x - scr.x), (float)(rclient.y - scr.y), 0);
    switch(mode) {
        case ZS_CHART_DIAGRAMM:
            m.identity();
            for(int j = 0 ; j < cols.size(); j++) {
                auto r(&rects[j]);
                m.scale2((float)r->w / 100.0f, (float)r->h / 100.0f, 1.0f);
                m.translate2((float)(r->x - scr.x), (float)(r->y - scr.y), 0);
                dr[0]->color.set(cols[j]);
                dr[0]->drawCommon(rclip, m, true);
            }
            break;
        case ZS_CHART_GRAPH: {
            auto sc((float)rclient[vert + 2] / ((float)rclient[vert + 2] - sizeChart));
            m.scale2(vert ? 1.0f : sc, vert ? sc : 1.0f, 1.0f);
            for(int i = 0; i < values.size(); i++) {
                dr[i]->color.set(colors[i].data[0]);
                dr[i]->drawCommon(rclip, m, true);
            }
            break;
        }
        case ZS_CHART_CIRCULAR:
            if(values.isEmpty()) break;
            int offs(0);
            // устанавливаем текстуру
            glBindTexture(GL_TEXTURE_2D, dr[0]->texture->id);
            manager->prepareRender(0, dr[0]->vertices, rclip - manager->screen, m);
            for(int j = 0; j < cols.size(); j += 2) {
                // устанавливаем цветовой фильтр
                manager->setColorFilter(this, cols[j]);
                // запускаем отрисовку
                auto c((int)cols[j + 1]);
                // устанавливаем параметры шейдеров и обрезки
                glDrawArrays(GL_TRIANGLE_STRIP, offs, c + 1);
                offs += c;
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
        case ZS_CHART_CIRCULAR: makeCircular(((float)rclient.w / 2.0f), ((float)rclient.h / 2.0f), dy); break;
        case ZS_CHART_GRAPH:    makeGraph(0, 0, dy); break;
        default:
        case ZS_CHART_DIAGRAMM: makeDiagramm(x, y, dy); break;
    }
    awakenScroll();
}

void zViewChart::makeDiagramm(float x, float y, float dy) {
    rects.free(); cols.free(); rti r;
    auto wpad(ipad.extent(false)), hpad(ipad.extent(true));
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
                case 2: p[!vert] = pt[!vert] + ((float)rclient[3 - vert] - size); break;
                // center
                case 3: p[!vert] = pt[!vert] + (((float)rclient[3 - vert] - size) / 2.0f); break;
            }
            r.x = z_round(p.x + 0.5f + (float)(!vert * ipad.x));
            r.y = z_round(p.y + 0.5f + (float)(vert * ipad.y));
            r.w = vert ? (int)size : ((int)sizeChart - wpad);
            r.h = vert ? ((int)sizeChart - hpad) : (int)size;
            p[vert] += sizeChart;
            rects += r;
            cols += _cols[_count > (i + fChart) ? (i + fChart) : (_count - 1)];
        }
    }
    dr[0]->measure(100, 100, 0, false);
}

void zViewChart::makeGraph(float x, float y, float dy) {
    ptf pt; float size;
    for(int j = 0 ; j < values.size(); j++) {
        auto _vals(values[j].data);
        pt.set(x, y); auto p(pt);
        zArray<ptf> path;
        pt[vert] -= (float)delta;
        auto count(maxCharts + (delta != 0));
        for(int i = 0 ; i < count; i++) {
            size = (dy * (float)_vals[i + fChart]);
            switch(grav >> (!vert * 2)) {
                default:
                // start/top
                case 1: p[!vert] = pt[!vert] + size; break;
                // end/bottom
                case 2: p[!vert] = pt[!vert] + ((float)rclient[3 - vert] - size); break;
                // center
                case 3: p[!vert] = pt[!vert] + (((float)rclient[3 - vert] - size) / 2.0f); break;
            }
            path += ptf(p.x + (float)(!vert * ipad.x), p.y + (float)(vert * ipad.y));
            p[vert] += sizeChart;
        }
        smoothPath(path, 5, true);
        path.reverse(); size = (float)rclient[3 - vert];
        switch(grav >> (!vert * 2)) {
            default:
            case 1: pt.set(0, 0); break;
            case 2: pt.set((float)vert * size, (float)!vert * size); break;
            case 3: pt.set((float)vert * (size / 2.0f), (float)!vert * (size / 2.0f));
                auto v(!vert); auto c(path.size()); path += path[c - 1];
                for(int n = c - 1 ; n >= 0; n--) { p = path[n]; p[v] = pt[v] + (pt[v] - p[v]); path += p; }
        }
        dr[j]->makePath(path, dr[j]->getRectTile(true).padding(5, 5), pt, vert + 1);
    }
}

static float d2r(float a) { return a * (Z_PI / 180.0f); }

void zViewChart::makeCircular(float x, float y, float) {
    ptf pt(x, y); cols.free(); zArray<ptf> path;
    float radius((float)z_min(rclient.h, rclient.w) / 2.0f);
    if(values.isEmpty()) return;
    auto _vals(values[0].data), _cols(colors[0].data);
    auto count(values[0].count), _count(colors[0].count);
    // подсчитать сумму
    float sum(0), angle1(startAngle); for(int i = 0 ; i < count; i++) sum += (float)_vals[i];
    sum = (360.0f / sum);
    for(int i = 0 ; i < count; i++) {
        auto angle2(angle1 + (float)_vals[i] * sum);
        if(i + 1 >= count) angle2 = startAngle + 360.5f;
        auto c(z_round((angle2 - angle1) / 5.0f)); c = z_max(1, c);
        auto a1(angle1); cols += _cols[i < _count ? i : _count - 1]; cols += c * 2;
        while(c-- > 0) {
            auto angle(d2r(a1));
            path += ptf(x + sinf(angle) * radius, y - cosf(angle) * radius);
            a1 += 5.0f;
        }
        angle1 = angle2;
    }
    path += ptf(x + sinf(d2r(angle1)) * radius, y - cosf(d2r(angle1)) * radius);
    dr[0]->makePath(path, dr[0]->getRectTile(true), pt, 0);
    center.set(pt.x + (float)rclient.x, pt.y + (float)rclient.y);
}

void zViewChart::setFirstVisible(int _first) {
    if((_first + maxCharts) > cCharts) _first = cCharts - maxCharts;
    else if(_first < 0) _first = 0;
    if(fChart != _first) { fChart = _first; invalidate(); }
}

i32 zViewChart::onTouchEvent() {
    bool drag(false);
    if(mode == ZS_CHART_CIRCULAR) {
        touch->rotate(sizeTouch, center, [this, &drag](float, float a, bool event) {
            startAngle += a;
            requestPosition();
            drag = true;
        });
    } else {
        touch->drag(sizeTouch, [this, &drag](cszi &offs, bool event) {
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
    }
    // если не было перетаскивания
    if(!drag && mode != ZS_CHART_CIRCULAR) {
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
    szf _sz((float)rclient.x, (float)rclient.y);
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
