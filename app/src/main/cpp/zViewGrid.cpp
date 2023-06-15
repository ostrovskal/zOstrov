//
// Created by User on 25.04.2022.
//

#include "zssh/zCommon.h"
#include "zssh/zViewRibbons.h"

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

void zViewGrid::onDraw() {
    zViewGroup::onDraw();
    if(div) {
        div->make(((div->size + div->padBegin + div->padEnd) - params[GRID_LINES_SPACE]) / 2, countItem, firstItem);
    }
}

void zViewGrid::fill(int _edge) {
    firstItem -= firstItem % linesGrid;
    auto view(atView(0));
    if(view) fillReverse(firstItem - linesGrid, view->edges(vert, false));
    auto count(children.size());
    view = atView(count - 1);
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
        int v(zTheme::value.u); attr |= _theme * ZTT_THM;
        switch(attr) {
            case Z_MODE:          params[GRID_MODE] = _params[GRID_MODE]                = v; break;
            case Z_LINES:         _params[GRID_LINES] = linesGrid                       = v; break;
            case Z_SPACING_CELL:  params[GRID_CELLS_SPACE] = _params[GRID_CELLS_SPACE]  = v; break;
            case Z_SPACING_LINE:  params[GRID_LINES_SPACE] = _params[GRID_LINES_SPACE]  = v; break;
            case Z_CELL_SIZE:     params[GRID_CELL_SIZE]   = _params[GRID_CELL_SIZE]    = v; break;
        }
    });
}

void zViewGrid::onMeasure(cszm& spec) {
    // Вычисление параметров ячеек
    zViewBaseRibbon::onMeasure(spec);
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
    for(i = 0 ; i < countItem; i++) {
        auto child(obtainView(i));
        if(!child) continue;
        // расчитать размер
        measureChild(child, spec);
        // добавить в кэш
        addViewCache(child);
        auto rv(child->rview);
        rv[3 - vert] += cellSpace; rv[vert + 2] += lineSpace;
        if(_max.isEmpty()) { if(spec[!vert].isNotExact()) lines = rv[3 - vert] * (linesGrid + 1); }
        auto val(_max[!vert] + rv[3 - vert]);
        if(val < cells) { _max[!vert] = val; continue; }
        val = _max[vert] + rv[vert + 2];
        if(val >= lines) break;
        _max[vert] = val;
    }
    _max[vert] += (div && div->resolve(i / linesGrid, false));
//    SZI_LOG("max", _max);
//    DLOG("2 lines: %i - %i %i %i %i %i", linesGrid, params[0], params[1], params[2], params[3], params[4]);
    return _max;
    /*
    int hmax(0), wmax(0), i;
    int limitCells(SIZE_MEASURE_SPEC(widthSpec));
    int limitLines(SIZE_MEASURE_SPEC(heightSpec));
    auto limC(vert ? &limitCells : &limitLines);
    auto limL(vert ? &limitLines : &limitCells);
    if(!lines) lines = (*limC - pad.extent(vert)) / (params[TABLE_CELL_SIZE] + params[TABLE_CELLS_SPACE] * 2);
    for(i = 0 ; i < countItem; i++) {
        auto child(obtainView(i));
        if(!child) continue;
        // расчитать размер
        measureChild(child, widthSpec, heightSpec);
        // добавить в кэш
        addViewCache(child);
        auto wc(child->rfull.w), hc(child->rfull.h);
        if(vert) {
            wc += cellSpacing; hc += lineSpacing;
        } else {
            wc += lineSpacing; hc += cellSpacing;
        }
        if(wmax == 0 && hmax == 0) {
            auto mode(MODE_MEASURE_SPEC((vert ? widthSpec : heightSpec)));
            if(mode != MEASURE_EXACT) {
                *limL = (vert ? wc : hc) * (lines + 1);
            }
        }
        if(vert) {
            // right, down
            auto _wmax(wmax + wc);
            if(_wmax < *limL) { wmax = _wmax; continue; }
            auto _hmax(hmax + hc);
            if(_hmax >= *limC) break;
            hmax = _hmax;
        } else {
            // down, right
            auto _hmax(hmax + hc);
            if(_hmax < *limL) { hmax = _hmax; continue; }
            auto _wmax(wmax + wc);
            if(_wmax >= *limC) break;
            wmax = _wmax;
        }
    }
    auto _size(resolveDivider(i / lines));
    if(vert) hmax += _size; else wmax += _size;
    return {wmax, hmax};
     *
     */
}

void zViewGrid::childMeasure(zView *child, zLayoutParams *lps) {
    szm s(zMeasure(MEASURE_UNDEF, 0), zMeasure(MEASURE_EXACT, params[GRID_CELL_SIZE]));
    auto wspec(makeChildMeasureSpec(s[vert], child->margin.extent(false), lps->w));
    auto hspec(makeChildMeasureSpec(s[!vert], child->margin.extent(true), lps->h));
    child->measure( { wspec, hspec } );
}
