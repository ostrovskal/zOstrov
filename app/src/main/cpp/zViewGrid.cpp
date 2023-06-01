//
// Created by User on 25.04.2022.
//

#include "zssh/zCommon.h"
#include "zssh/zViewRibbons.h"

/*
void zViewGrid::setParameters(const i32* _params) {
    if(!_params) return;
    memcpy(params, _params, sizeof(params));
    lines       = params[TABLE_LINES];
    cellSpacing = params[TABLE_CELLS_SPACE];
    lineSpacing = params[TABLE_LINES_SPACE];
    cellSize    = params[TABLE_CELL_SIZE];
    requestLayout();
}

void zViewGrid::getParameters(i32* _params) const {
    if(_params) {
        memcpy(_params, params, sizeof(params));
    }
}

void zViewGrid::fill(int _edge) {
    firstItem -= firstItem % linesGrid;
    auto view(atView(0));
    if(view) fillReverse(firstItem - linesGrid, view->edges(vert, false));
    auto count(children.size());
    view = atView(count - 1);
    if(firstItem == 0 && (divType & ZS_DIVIDER_BEGIN)) _edge += divSize + divPadEnd;
    fillForward(firstItem + count, (view ? view->edges(vert, true) + lineSpacing : edge.w) + _edge);
}

void zViewGrid::fillReverse(int pos, int next) {
    auto div((divType & ZS_DIVIDER_MIDDLE) ? divSize + divPadBegin + divPadEnd : 0);
    while(next > edgeStart && pos >= 0) {
        next = makeLine(pos, next, false)->edges(vert, false) - lineSpacing;
        firstItem = pos; pos -= linesGrid; next -= div;
    }
    correctBegin(lineSpacing + div);
}

void zViewGrid::fillForward(int pos, int next) {
    auto _div((divType & ZS_DIVIDER_MIDDLE) ? divSize + divPadBegin + divPadEnd : 0);
    while(next < edge.h&& pos < countItem) {
        next = makeLine(pos, next, true)->edges(vert, true) + lineSpacing;
        next += _div; pos += linesGrid;
    }
    correctFinish(lineSpacing + _div);
}

zView* zViewGrid::makeLine(int position, int coord1, bool flow) {
    auto coord2((vert ? rclient.x : rclient.y) + (params[TABLE_MODE] == ZS_TABLE_STRETCH_UNIFORM) * cellSpacing);
    auto last(z_min(countItem, position + lines));
    zView* child(nullptr);
    for(int pos = position; pos < last; pos++ ) {
        auto where(flow ? -1 : pos - position);
        child = addView(vert ? coord2 : coord1, vert ? coord1 : coord2, pos, flow, where);
        coord2 += cellSize + (pos < last - 1) * cellSpacing;
    }
    return child;
}

void zViewGrid::onInflate(bool _theme) {
    zViewBaseRibbon::onInflate(_theme);
    styles->enumerate([this, _theme](u32 attr) {
        int v((int)zTheme::value.value); attr |= _theme * ZTT_THM;
        switch(attr) {
            case ZTV_MODE:          params[TABLE_MODE]          = v; break;
            case ZTV_LINES:         params[TABLE_LINES]         = v; break;
            case ZTV_SPACING_CELL:  params[TABLE_CELLS_SPACE]   = v; break;
            case ZTV_SPACING_LINE:  params[TABLE_LINES_SPACE]   = v; break;
            case ZTV_CELL_SIZE:     params[TABLE_CELL_SIZE]     = v; break;
            default: 				break;
        }
    });
    setParameters(params);
}

void zViewGrid::onMeasure(int widthSpec, int heightSpec) {
    zViewBaseRibbon::onMeasure(widthSpec, heightSpec);
    // Вычисление параметров ячеек
    auto cell(params[TABLE_CELL_SIZE]), space(params[TABLE_CELLS_SPACE]);
    if(cell <= 0) return;
    auto wh(vert ? rclient.w : rclient.h), ln(z_max(1, lines - 1));
    switch(params[TABLE_MODE]) {
        case ZS_TABLE_STRETCH_CELL:
            ln = z_max(1, lines - 2);
            cellSize = (int)round((float)(wh - space * ln) / (float)lines);
            break;
        case ZS_TABLE_STRETCH_SPACING:
            wh -= cell * lines;
            cellSpacing = (int)round((float)wh / (float)ln);
            break;
        case ZS_TABLE_STRETCH_UNIFORM:
            wh -= (cell * lines + space * (lines + 1));
            cellSpacing = space + (int)round((float)wh / (float)(lines + 1));
            break;
    }
//    DLOG("cell:%i space:%i cellSize:%i cellSpacing:%i", cell, space, cellSize, cellSpacing);
}

szi zViewGrid::measureChildrenSize(int widthSpec, int heightSpec) {
    int hmax(0), wmax(0), i;
    int limitCells(SIZE_MEASURE_SPEC(widthSpec));
    int limitLines(SIZE_MEASURE_SPEC(heightSpec));
    auto limC(vert ? &limitCells : &limitLines);
    auto limL(vert ? &limitLines : &limitCells);
    if(!linesGrid) linesGrid = (*limC - pad.extent(vert)) / (params[TABLE_CELL_SIZE] + params[TABLE_CELLS_SPACE] * 2);
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
    auto _size(div && div->resolve(i / linesGrid));
    if(vert) hmax += _size; else wmax += _size;
    return {wmax, hmax};
}

void zViewGrid::childMeasure(zView *child, zLayoutParams *lps) {
    auto spec1(zMeasure(MEASURE_UNDEF, 0));
    auto spec2(zMeasure(MEASURE_EXACT, cellSize));
    auto wspec(makeChildMeasureSpec(vert ? spec2 : spec1, padMargin(false), lps->w));
    auto hspec(makeChildMeasureSpec(vert ? spec1 : spec2, padMargin(true), lps->h));
    child->measure( { wspec, hspec } );
}
*/
