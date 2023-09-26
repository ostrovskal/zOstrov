//
// Created by User on 25.04.2022.
//

#include "zostrov/zCommon.h"
#include "zostrov/zViewRibbons.h"

void zViewGrid::setParameters(i32 param, i32 value) {
    if(param >= GRID_CELLS_SPACE && param <= GRID_LINES) {
        _params[param] = value;
        if(param == GRID_LINES) linesGrid = value; else params[param] = value;
        requestLayout();
    }
}

i32 zViewGrid::getParameters(i32 param) const {
    if(param >= GRID_CELLS_SPACE && param <= GRID_LINES) {
        return params[param];
    }
    return -1;
}

int zViewGrid::getExtra() const {
    return (params[GRID_LINES_SPACE] - div->size) / 2;
}

void zViewGrid::fill(int _edge, bool reverse) {
    if(reverse) {
        firstItem -= firstItem % linesGrid;
        auto view(atView(0));
        if(view) fillReverse(firstItem - linesGrid, view->edges(vert, false));
    }
    auto count(children.size());
    auto view(atView(count - 1));
    if(firstItem == 0 && div) {
        auto begin(div->getSize(ZS_DIVIDER_BEGIN));
        if(_edge < begin && _edge >= 0) _edge += begin;
    }
    fillForward(firstItem + count, (view ? view->edges(vert, true) : edge.w) + _edge);
}

void zViewGrid::fillReverse(int pos, int next) {
    auto _div(div ? div->getSize(ZS_DIVIDER_MIDDLE) : 0);
    _div += params[GRID_LINES_SPACE];
    while(next > edge.w && pos >= 0) {
        next = makeLine(pos, next - _div, false)->edges(vert, false) - _div;
        firstItem = pos; pos -= linesGrid;
    }
    correctBegin(_div);
}

void zViewGrid::fillForward(int pos, int next) {
    auto _div(div ? div->getSize(ZS_DIVIDER_MIDDLE) : 0);
    _div += params[GRID_LINES_SPACE];
    while(next < edge.h&& pos < countItem) {
        next = makeLine(pos, next, true)->edges(vert, true) + _div;
        pos += linesGrid;
    }
    correctFinish(_div);
}

zView* zViewGrid::makeLine(int position, int coord1, bool flow) {
    auto coord2(rclient[!vert] + (params[GRID_MODE] == ZS_GRID_UNIFORM) * params[GRID_CELLS_SPACE]);
    auto last(z_min(countItem, position + linesGrid));
    zView* child(nullptr);
    for(int pos = position; pos < last; pos++ ) {
        auto where(flow ? -1 : pos - position);
        child = addView(vert ? coord2 : coord1, vert ? coord1 : coord2, pos, flow, where);
        coord2 += params[GRID_CELL_SIZE] + (pos < last - 1) * params[GRID_CELLS_SPACE];
    }
    return child;
}

void zViewGrid::onInit(bool _theme) {
    zViewBaseRibbon::onInit(_theme);
    styles->enumerate([this, _theme](u32 attr) {
        auto v((int)zTheme::value.u); attr |= _theme * ZTT_THM;
        switch(attr) {
            case Z_MODE:          params[GRID_MODE]        = _params[GRID_MODE]         = v; break;
            case Z_LINES:         params[GRID_LINES]       = _params[GRID_LINES]        = linesGrid = v; break;
            case Z_SPACING_CELL:  params[GRID_CELLS_SPACE] = _params[GRID_CELLS_SPACE]  = v; break;
            case Z_SPACING_LINE:  params[GRID_LINES_SPACE] = _params[GRID_LINES_SPACE]  = v; break;
            case Z_CELL_SIZE:     params[GRID_CELL_SIZE]   = _params[GRID_CELL_SIZE]    = v; break;
        }
    });
}

void zViewGrid::onMeasure(cszm& spec) {
    zViewBaseRibbon::onMeasure(spec);
    // Вычисление параметров ячеек
    auto cell(_params[GRID_CELL_SIZE]), space(_params[GRID_CELLS_SPACE]);
    auto wh(rclient[3 - vert]), ln(z_max(1, linesGrid - 1));
    switch(_params[GRID_MODE]) {
        case ZS_GRID_CELL:
            ln = z_max(1, linesGrid - 2);
            params[GRID_CELL_SIZE] = (int)round((float)(wh - space * ln) / (float)linesGrid);
            break;
        case ZS_GRID_SPACING:
            wh -= cell * linesGrid;
            params[GRID_CELLS_SPACE] = (int)round((float)wh / (float)ln);
            break;
        case ZS_GRID_UNIFORM:
            wh -= (cell * linesGrid + space * (linesGrid + 1));
            params[GRID_CELLS_SPACE] = space + (int)round((float)wh / (float)(linesGrid + 1));
            break;
    }
}

szi zViewGrid::measureChildrenSize(cszm& spec) {
    int i; szi _max;
    auto cells(spec[!vert].size()), lines(spec[vert].size());
    auto cell(_params[GRID_CELL_SIZE]), cellSpace(_params[GRID_CELLS_SPACE]), lineSpace(_params[GRID_LINES_SPACE]);
    if(cell <= 0) cell = 100_dp;
    if(!cells) { cells = (cell + cellSpace) * 5; }
    if(!linesGrid) linesGrid = (cells - pad.extent(!vert)) / (cell + cellSpace);
    _params[GRID_LINES] = linesGrid; _params[GRID_CELL_SIZE] = cell;
    auto _div((div ? div->resolve(z_min(countItem, (linesGrid - 1)), false) : 0));
    for(i = 0 ; i < countItem; i++) {
        auto child(obtainView(i));
        if(!child) continue;
        // расчитать размер
        measureChild(child, spec);
        // добавить в кэш
        addViewCache(child);
        auto& rv(child->rview);
        if(_max.isEmpty()) {
            if(spec[!vert].isNotExact()) cells = (rv[3 - vert] + cellSpace) * (linesGrid + 1);
            if(spec[vert].isNotExact()) lines = (rv[vert + 2] + lineSpace + _div) * (linesGrid);
        }
        auto val(_max[!vert] + rv[3 - vert] + cellSpace);
        if(val < cells) { _max[!vert] = val; continue; }
        val = _max[vert] + rv[vert + 2] + lineSpace;
        if(val >= lines) break;
        _max[vert] = val;
    }
    _max[vert] += _div;
    return _max;
}

void zViewGrid::childMeasure(zView *child, zLayoutParams *lps) {
    szm s(zMeasure(MEASURE_UNDEF, 0), zMeasure(MEASURE_EXACT, params[GRID_CELL_SIZE]));
    auto wspec(makeChildMeasureSpec(s[vert], child->margin.extent(false), lps->w));
    auto hspec(makeChildMeasureSpec(s[!vert], child->margin.extent(true), lps->h));
    child->measure( { wspec, hspec } );
}
