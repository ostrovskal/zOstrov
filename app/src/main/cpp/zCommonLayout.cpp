//
// Created by User on 25.04.2022.
//

#include "zssh/zCommon.h"
#include "zssh/zCommonLayout.h"

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                                         FRAME LAYOUT                                                                   //
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void zFrameLayout::onMeasure(cszm& spec) {
    auto sz(measureChildren(spec));
    defaultOnMeasure(spec, sz.w, sz.h);
}

void zFrameLayout::onLayout(crti &position, bool changed) {
    zView::onLayout(position, changed);
    for(auto& child : children) {
        if(child->isVisibled()) {
            // применить выравнивание
            auto width(child->rview.w), height(child->rview.h);
            auto pt(applyGravity(rclient, rti(0, 0, width + child->margin.extent(false), height + child->margin.extent(true)), child->lps.gravity));
            child->layout(rti(rclient.x + pt.x, rclient.y + pt.y, width, height));
        }
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                                        ABSOLUTE LAYOUT                                                                 //
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void zAbsoluteLayout::onMeasure(cszm& spec) {
    auto width(spec.w.size()), height(spec.h.size());
    // сначала расчитать габариты дочерних
    measureChildren(spec);
    if(spec.w.isNotExact() || spec.h.isNotExact()) {
        int maxHeight(0), maxWidth(0);
        // найти самый правый и самый нижний
        for(auto &child : children) {
            if(child->isVisibled()) {
                auto lps(&child->lps);
                maxWidth  = z_max(maxWidth, lps->x + child->sizes(false));
                maxHeight = z_max(maxHeight, lps->y + child->sizes(true));
            }
        }
        if(spec.w.isNotExact()) {
            maxWidth  += padMargin(false);
            maxWidth  = z_max(maxWidth, minMaxSize.x);
            width = maxWidth;
        }
        if(spec.h.isNotExact()) {
            maxHeight += padMargin(true);
            maxHeight = z_max(maxHeight, minMaxSize.y);
            height = maxHeight;
        }
    }
    setMeasuredDimension(width, height);
}

void zAbsoluteLayout::onLayout(crti &position, bool changed) {
    zView::onLayout(position, changed);
    for(auto& child : children) {
        if(child->isVisibled()) {
            auto lps(&child->lps);
            child->layout(rti(rclient.x + lps->x, rclient.y + lps->y, child->rview.w, child->rview.h));
        }
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                                        LINEAR LAYOUT                                                                   //
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void zLinearLayout::onMeasure(cszm& spec) {
    int width, height, allWeight(0); szm childSpec;
    int wmax(0), hmax(0), marginsChild(0); int* side; bool isWeights(false);
    bool vert(isVertical());
    // размер разделителя
    auto divSize(div ? div->resolve(countChildren()) : 0);
    if(vert) hmax = divSize; else wmax = divSize;
    // определяем габариты и веса дочерних
    for(auto child : children) {
        if(child->isVisibled()) {
            measureChild(child, spec);
            auto size(child->sizes(vert));
            marginsChild += child->margin.extent(vert);
            if(vert) {
                hmax += size;
                wmax = z_max(wmax, child->sizes(false));
            } else {
                wmax += size;
                hmax = z_max(hmax, child->sizes(true));
            }
            // весь вес
            auto weight(child->lps.weight);
            isWeights |= (weight != 0);
            allWeight += z_max(1, weight);
        }
    }
    if(vert) {
        if(spec.h.isExact()) hmax = spec.h.size() - pad.extent(true);
        height = hmax - divSize; width = wmax; side = &height;
    } else {
        if(spec.w.isExact()) wmax = spec.w.size()  - pad.extent(false);
        width = wmax - divSize; height = hmax; side = &width;
    }
    // снова определяем размеры дочерних с учетом веса
    for(auto child: children) {
        if(child->isVisibled()) {
            auto& lps(child->lps);
            auto weight(z_max<float>(1.0f, (float)child->lps.weight));
            if(isWeights) weight /= (float)allWeight;
            auto size(isWeights ? (zMeasure)roundf((float)(*side - marginsChild) * weight) : lps[vert + 2]), wh(lps[3 - vert]);
            childSpec.set(makeChildMeasureSpec(zMeasure(MEASURE_EXACT, width), 0, vert ? wh : size),
                          makeChildMeasureSpec(zMeasure(MEASURE_EXACT, height), 0, vert ? size : wh));
            child->measure(childSpec);
            if(!isWeights) *side -= child->sizes(vert);
        }
    }
    defaultOnMeasure(spec, wmax, hmax);
    // разделитель
    if(div) div->measure(0, 0, 3, false);
}

pti zLinearLayout::applyGravity(crti& sizeParent, crti& sizeChild, u32 _gravity) {
    auto ret(zView::applyGravity(sizeParent, sizeChild, _gravity));
    if(isVertical()) ret.y = 0; else ret.x = 0;
    return ret;
}

void zLinearLayout::onLayout(crti &position, bool changed) {
    zView::onLayout(position, changed);
    auto r(rclient); auto vert(isVertical()); auto pt(&r.buf[vert]);
    // размер разделителя
    auto divSize((div && (div->type & ZS_DIVIDER_MIDDLE)) ? (div->size + div->padEnd + div->padBegin) : 0);
    // начальная координата с учетом разделителя
    if(div && div->type & ZS_DIVIDER_BEGIN) *pt += div->size + div->padEnd;
    // позиционировать дочерние
    for(auto& child : children) {
        if(child->isVisibled()) { child->layout(r); *pt += child->sizes(vert) + divSize; }
    }
}

void zLinearLayout::onDraw() {
    zViewGroup::onDraw();
    // отрисовка разделителя
    if(div) div->make(0, countChildren(), 0);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                                        CELL LAYOUT                                                                     //
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static int z_clamp(int v, int mx) {
    return v > mx ? mx : (v < 0 ? 0 : v);
}

zCellLayout::zCellLayout(zStyle* _styles, i32 _id, cszi& _cell, i32 _space) : zViewGroup(_styles, _id) {
    cells = _cell; space = _space;
}

static void correctSize(zLayoutParams* lps, int cols, int rows) {
    lps->x = z_clamp(lps->x, cols - 1);
    lps->y = z_clamp(lps->y, rows - 1);
    if(lps->x + lps->w > cols) lps->w = cols - lps->x;
    if(lps->y + lps->h > rows) lps->h = rows - lps->y;
}

void zCellLayout::onMeasure(cszm& spec) {
    auto width(spec.w.size()), height(spec.h.size());
    auto wpad(pad.extent(false)), hpad(pad.extent(true));
    auto spc(space * 2), cols(z_max(1, cells.w)), rows(z_max(1, cells.h));
    // размеры по умолчанию
    auto wExact(spec.w.isExact()), hExact(spec.h.isExact());
    auto hCell((float)(hExact * (height - hpad)) / (float)rows);
    auto wCell((float)(wExact * (width - wpad))  / (float)cols);
    if(cells.w <= 0) wCell = hCell, cols = rows;
    else if(cells.h <= 0) hCell = wCell, rows = cols;
    if(!(wExact && hExact)) {
        // определяем максимальный размер ячейки
        for(auto &child: children) {
            if(child->isVisibled()) {
                // параметры макета
                auto lps(&child->lps);
                // корректируем габариты
                correctSize(lps, cols, rows);
                // определяем габариты дочернего
                szm childSpec(makeChildMeasureSpec(spec.w, padMargin(false), VIEW_WRAP),
                              makeChildMeasureSpec(spec.h, padMargin(true),  VIEW_WRAP));
                child->measure(childSpec);
                // определяем максимальный размер ячейки
                if(!wExact) wCell = z_max(wCell, (float)(child->rview.w + spc) / lps->w);
                if(!hExact) hCell = z_max(hCell, (float)(child->rview.h + spc) / lps->h);
            }
        }
    }
    wCell = (wCell ? wCell : 40_dp);
    hCell = (hCell ? hCell : 40_dp);
    // ставим фактические размеры
    for(auto &child: children) {
        if(child->isVisibled()) {
            // параметры макета
            auto lps(&child->lps);
            szm childSpec(zMeasure(MEASURE_EXACT, (int)(lps->w * wCell + space)),
                          zMeasure(MEASURE_EXACT, (int)(lps->h * hCell + space)));
            child->measure(childSpec);
        }
    }
    // определяем габариты макета
    if(!hExact) {
        height = hpad + hCell * rows;
        if(spec.h.mode() == MEASURE_MOST) {
            height = z_min(spec.h.size(), height);
        }
    }
    if(!wExact) {
        width = wpad + wCell * cols;
        if(spec.w.mode() == MEASURE_MOST) {
            width = z_min(spec.w.size(), width);
        }
    }
    cell.set(wCell, hCell);
    setMeasuredDimension(width, height);
}

void zCellLayout::onLayout(crti &position, bool changed) {
    zViewGroup::onLayout(position, changed);
    rti r;
    for(auto& child : children) {
        if(child->isVisibled()) {
            auto lps(&child->lps);
            r.x = (rclient.x + (int)((float)lps->x * cell.w + (float)space));
            r.y = (rclient.y + (int)((float)lps->y * cell.h + (float)space));
            r = child->rview.size();
            child->layout(r);
        }
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                                        SCROLL LAYOUT                                                                   //
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void zScrollLayout::onMeasure(cszm& spec) {
    int childWidth(0), childHeight(0);
    if(countChildren()) {
        auto child(atView(0));
        auto sw(spec.w.size()), sh(spec.h.size());
        auto lps(&child->lps);
        int wmms, hmms;
        if(atView<zScrollLayout>(0)) {
            wmms = vert * spec.w;
            hmms = !vert * spec.h;
        } else {
            if(vert) {
                wmms = zMeasure((sw != 0) * spec.w.mode(), sw);
                hmms = zMeasure(MEASURE_UNDEF, 0);
            } else {
                wmms = zMeasure(MEASURE_UNDEF, 0);
                hmms = zMeasure((sh != 0) * spec.h.mode(), sh);
            }
        }
        szm childSpec(makeChildMeasureSpec(wmms, padMargin(false), lps->w),
                      makeChildMeasureSpec(hmms, padMargin(true), lps->h));
        child->measure(childSpec);
        childWidth  = child->sizes(false);
        childHeight = child->sizes(true);
    }
    defaultOnMeasure(spec, childWidth, childHeight);
    childWidth  = z_max(rclient.w, childWidth);
    childHeight = z_max(rclient.h, childHeight);
    childSize   = vert ? childHeight : childWidth;
}

void zScrollLayout::onLayout(crti &position, bool changed) {
    zView::onLayout(position, changed);
    auto child(atView(0));
    // если он сам сдвинут
    if(child) child->layout(rclient - scroll);
}

zView *zScrollLayout::attach(zView *v, int width, int height, int where) {
    if(countChildren() == 0) {
        return zViewGroup::attach(v, width, height, where);
    }
    return nullptr;
}

i32 zScrollLayout::onTouchEvent(zTouch *touch) {
    bool drag(false);
    touch->drag(sizeTouch, [this, &drag, &touch](cszi& offs, bool event) {
        // если сдвинули - определяем дельту в зависимости от ориентации
        auto vert(isVertical());
        auto _delta(vert ? offs.h : offs.w);
        if(_delta) {
            // определяем время сдвига
            auto t((int)((touch->ctm - touch->btm) / 50000000));
            // если отпустили и время < 15(выбрано экспериментально)
            if(event && t < 1) {
                // запускаем флинг
                flyng->start((float)_delta / (float)sizes(vert));
            } else {
                // иначе - просто скроллим на дельту
                scrolling(_delta);
                // отправляем событие скролла
                if(onChangeScroll) onChangeScroll(this, _delta);
            }
            drag = true;
        }
    });
    return drag | touch->isCaptured();
}

bool zScrollLayout::scrolling(int _delta) {
    auto child(atView(0));
    if(child) {
        auto vert(isVertical());
        auto vEnd(vert ? rclient.h : rclient.w);
        auto delta1(delta); delta -= _delta;
        if(_delta < 0 && (delta + vEnd) >= childSize)
            delta = childSize - vEnd;
        if(_delta >= 0 && delta < 0)
            delta = 0;
        auto upd(delta1 != delta);
        child->scroll.buf[vert] = delta;
        if(upd) {
            child->requestLayout();
        } else if(glow) {
            updateGlow(_delta);
        }
        awakenScroll();
        return !upd;
    }
    return true;
}
