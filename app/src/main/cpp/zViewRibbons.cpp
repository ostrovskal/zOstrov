
//
// Created by Sergo on 11.04.2022.
//

#include "zssh/zCommon.h"
#include "zssh/zViewRibbons.h"

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                                       БАЗОВАЯ ЛЕНТА                                                                    //
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

zViewBaseRibbon::zViewBaseRibbon(zStyle* _styles, i32 _id, bool _vert) : zViewGroup(_styles, _id), vert(_vert) {
    updateStatus(ZS_VORIENTATION, _vert);
    flyng = new zDrawableFlyng(this);
}

zViewBaseRibbon::~zViewBaseRibbon() {
    cacheViews.clear();
    if(adapter) {
        adapter->unregisterOwner(this);
        adapter = nullptr;
    }
}

void zViewBaseRibbon::onInflate(bool _theme) {
    zViewGroup::onInflate(_theme);
    // теперь специфичные
    zParamDrawable sl;
    styles->enumerate([&sl, _theme](u32 attr) {
        auto v(zTheme::value.value); attr |= _theme * ZTT_THM;
        switch(attr) {
            case ZTV_SELECTOR:          sl.texture  = v; break;
            case ZTV_SELECTOR_COLOR:    sl.color    = v; break;
            case ZTV_SELECTOR_TILE:     sl.tile     = (i32)v; break;
            case ZTV_SELECTOR_PADDING:  sl.padding  = v; break;
            default:                    break;
        }
    });
    // выделение
    if(sl.texture) setSelector(sl);
}

void zViewBaseRibbon::setSelector(const zParamDrawable& params) {
    if(!drs[DR_IDX_SEL]) drs[DR_IDX_SEL] = new zDrawable(this);
    drs[DR_IDX_SEL]->set(params); selectItem = -1;
    // создать селектор
    sizeSelector = drs[DR_IDX_SEL]->invalidate(10, 10, false);
}

void zViewBaseRibbon::setAdapter(zAdapter* _adapter) {
    reset();
    if(adapter) adapter->unregisterOwner(this);
    adapter = _adapter;
    if(_adapter) {
        _adapter->registerOwner(this);
        countItem = _adapter->getCount();
    }
    requestLayout();
}

void zViewBaseRibbon::reset() {
    cacheViews.clear();
    firstItem = countItem = deltaItem = 0;
    selectItem = clickItem = -1;
    removeAllViews();
    showSelector(false);
}

void zViewBaseRibbon::correctBegin(int used) {
    auto v(atView(0));
    if(v) {
        auto _delta(v->edges(vert, false) - edgeStart);
        if(firstItem) _delta -= used;
        if(_delta > 0) {
            offsetChildren(-_delta, vert);
        }
    }
}

void zViewBaseRibbon::correctFinish(int used) {
    auto count(children.size() - 1);
    if(count >= 0 && (firstItem + count) == (countItem - 1)) {
        auto pos(atView(count)->edges(vert, true));
        auto offset(edgeEnd - pos);
        auto first(atView(0)); auto start(first->edges(vert, false));
        if(offset > 0 && (firstItem > 0 || start < edgeStart)) {
            if(firstItem == 0) {
                offset = z_min(offset, (edgeStart - start));
            }
            offsetChildren(offset, vert);
            if(firstItem > 0) {
                fillReverse(firstItem - lines, first->edges(vert, false) - used);
            }
        }
    }
}

void zViewBaseRibbon::addViewCache(zView* view) {
    for(auto & cacheView : cacheViews) {
        if(!cacheView) {
            cacheView = view;
            return;
        }
    }
    cacheViews += view;
}

zView *zViewBaseRibbon::getViewCache(int pos) {
    // брать представление по его позиции(чтобы минимизировать обновление)
    zView* view(nullptr); int ii(-1);
    for(int i = 0 ; i < cacheViews.size(); i++) {
        view = cacheViews[i];
        if(!view) continue;
        ii = i;
        if(view->id == pos) break;
    }
    if(ii >= 0) view = cacheViews[ii], cacheViews[ii] = nullptr;
    return view;
}

zView* zViewBaseRibbon::obtainView(int position, bool* needToMeasure) {
    auto view(getViewCache(position));
    auto child(adapter ? adapter->getView(position, view, this) : nullptr);
    if(needToMeasure) *needToMeasure = (view != child);
    return child;
}

// Добавление видимого элемента из кэша или из адаптера
// where 0 - в начало, -1 в конец
zView* zViewBaseRibbon::addView(int x, int y, int pos, bool flow, int where) {
    bool needToMeasure(true);
    auto child(obtainView(pos, &needToMeasure));
    // привязать к родителю
    auto lyt(&child->lytParams);
    attachInLayout(child, lyt->w, lyt->h, where);
    if(needToMeasure || !child->testFlags(ZS_MEASURE)) {
        // вычислить размеры дочернего
        childMeasure(child, lyt);
    }
    // сдвинуть представление, в зависимости от требований списка
    auto childWidth(child->rfull.w), childHeight(child->rfull.h);
    auto childTop((vert ? (flow ? y : y - childHeight) : y));
    auto childLeft((vert ? x : (flow ? x : x - childWidth)));
    // определить позицию
    rti r(childLeft, childTop, childWidth, childHeight);
    child->layout(r);
    return child;
}

int zViewBaseRibbon::itemFromPoint(cptf& p) const {
    auto x((int)p.x), y((int)p.y);
    for(int i = children.size() - 1; i >= 0; i--) {
        if(children[i]->rfull.contains(x, y))
            return i + firstItem;
    }
    return -1;
}

bool zViewBaseRibbon::scrolling(int _delta) {
    auto count(children.size());
    if(count > 0) {
        if(_delta) {
            auto isPosStart(firstItem == 0), isPosEnd((firstItem + count) == countItem);
            // первый элемент ниже верхней грани?
            auto pos(atView(0)->edges(vert, false));
            auto isEdgeStart(pos >= edgeStart);
            // последний выше нижней грани?
            pos = atView(count - 1)->edges(vert, true);
            auto isEdgeEnd(pos <= edgeEnd);
            auto isScrollToStart(!(isPosStart && isEdgeStart));
            auto isScrollToEnd(!(isPosEnd && isEdgeEnd));
            // проверить на возможность скролла, в зависимости от направления
            if((_delta > 0 && isScrollToStart) || (_delta < 0 && isScrollToEnd)) {
                auto posView(0);
                while(posView < children.size()) {
                    auto child(atView(posView));
                    // проверка на выход за пределы списка
                    // верхняя ниже нижней
                    auto isStart((child->edges(vert, false) + _delta) >= edgeEnd);
                    // нижняя выше верхней
                    auto isEnd((child->edges(vert, true) + _delta) <= edgeStart);
                    if(isStart || isEnd) {
                        if(isEnd) firstItem++;
                        detach(child);
                        addViewCache(child);
                    } else {
                        posView++;
                    }
                }
                offsetChildren(_delta, vert);
                fill(0);
                auto view(atView(0));
                deltaItem = (view ? view->edges(vert, false) - edgeStart : 0);
                invalidate();
                return false;
            }
            // Предел прокрутки. Запуск эффекта
            updateGlow(_delta, vert);
        }
    }
    return true;
}

int zViewBaseRibbon::computeScrollOffset(bool _vert) const {
    if(children.isNotEmpty()) {
        auto view(atView(0));
        auto s((view->edges(_vert, false) - edgeStart) * 100);
        return z_max(0, firstItem / lines * 100 - s / view->sizes(_vert));
    }
    return 0;
}

int zViewBaseRibbon::computeScrollRange(bool _vert) const {
    return ((countItem + lines - 1) / lines) * 100;
}

int zViewBaseRibbon::computeScrollExtent(bool _vert) const {
    auto count(children.size());
    if(count > 0) {
        auto extent(((count + lines - 1) / lines) * 100);
        auto view(atView(0));
        auto s((view->edges(_vert, false) - edgeStart) * 100);
        extent += s / view->sizes(_vert);
        view = atView(count - 1); s = view->edges(_vert, true) - edgeStart;
        extent -= (s - (edgeEnd - edgeStart)) * 100 / view->sizes(_vert);
        return extent;
    }
    return 0;
}

void zViewBaseRibbon::notifyAdapter(zAdapter* _adapter) {
    countItem = _adapter->getCount();
    requestLayout();
}

void zViewBaseRibbon::stateView(VIEW_STATE &state, bool save, int &index) {
    zView::stateView(state, save, index);
    if(save) {
        state.data += firstItem;
        state.data += deltaItem;
    } else {
        firstItem = (int)state.data[index++];
        deltaItem = (int)state.data[index++];
    }
}

void zViewBaseRibbon::showSelector(bool show) {
    if(drs[DR_IDX_SEL]) {
        // проверяем на видимость селектор
        drs[DR_IDX_SEL]->setVisible(show);
        if(show) {
            auto view(atView(selectItem - firstItem));
            if(view) {
                // обновляем параметры селектора
                auto r(view->rfull);
                drs[DR_IDX_SEL]->mtxScale.scale((float)r.w / sizeSelector.w, (float)r.h / sizeSelector.h, 0);
                drs[DR_IDX_SEL]->layout(&r);
            }
        }
    }
}

void zViewBaseRibbon::setItemSelected(int item) {
    flyng->finish();
    auto _count(countChildren() - 1);
    if(item >= countItem) item = countItem - 1;
    if(item < 0) item = 0;
    firstItem = item - _count / 2;
    isOffsetEnd = (firstItem + _count >= countItem);
    selectItem = clickItem = -1;
    invalidate();
}

void zViewBaseRibbon::onLayout(crti &position, bool changed) {
    zView::onLayout(position, changed);
    int posEnd(0);
    edgeStart       = vert ? rclient.y : rclient.x;
    edgeEnd         = rclient.extent(vert);
    // скорректировать первый видимый
    auto pos(firstItem), _count(countChildren());
    if(_count == 0) _count = countItem;
    if(pos + _count >= countItem) pos = countItem - _count;
    if(pos < 0) pos = 0;
    if(firstItem != pos) deltaItem = 0, firstItem = pos;
    if(isOffsetEnd && _count) {
        int ofsBegin(0);
        if((divType & ZS_DIVIDER_BEGIN) && firstItem == 0) ofsBegin = divSize + divPadEnd;
        auto posBegin(z_min(0, atView(0)->edges(vert, false) - ofsBegin - edgeStart));
        posEnd = (z_min(0, edgeEnd - atView(_count - 1)->edges(vert, true)));
        posEnd += posBegin;
    }
    // переместить в кэш
    for(auto& child : children) addViewCache(child);
    // убрать все дочерние
    detachAllViews();
    // заполнить
    if(countItem > 0) {
        fill(deltaItem + posEnd);
    }
    // "будим" скролбар
    awakenScroll();
    isOffsetEnd = false;
    showSelector(selectItem != -1);
}

szi zViewBaseRibbon::measureChildrenSize(int widthSpec, int heightSpec) {
    int limit(SIZE_MEASURE_SPEC((vert ? heightSpec : widthSpec)));
    int hmax(0), wmax(0), i;
    auto hSize(vert ? &hmax : &wmax);
    auto wSize(vert ? &wmax : &hmax);
    for(i = 0 ; i < countItem; i++) {
        auto child(obtainView(i));
        if(!child) continue;
        // расчитать размер
        measureChild(child, widthSpec, heightSpec);
        // добавить в кэш
        addViewCache(child);
        // определить макс. габариты
        auto wc(child->rfull.w), hc(child->rfull.h);
        if(wmax == 0 && hmax == 0) {
            if(MODE_MEASURE_SPEC((vert ? heightSpec : widthSpec)) != MEASURE_EXACT) {
                // предел делаем на макс. 4
                auto _limit((vert ? hc : wc) * 5);
                if(_limit > limit) limit = _limit;
            }
        }
        *wSize = z_max(*wSize, (vert ? wc : hc));
        auto _size(*hSize + (vert ? hc : wc));
        if(_size >= limit) break;
        *hSize = _size;
    }
    auto _size(resolveDivider(i));
    if(vert) hmax += _size; else wmax += _size;
    return {wmax, hmax};
}

void zViewBaseRibbon::onMeasure(int widthSpec, int heightSpec) {
    // расчитать габариты всех дочерних
    auto size(measureChildrenSize(widthSpec, heightSpec));
    cacheViews.clear();
    defaultOnMeasure(widthSpec, heightSpec, size.w, size.h);
    // дивидер
    if(drs[DR_IDX_DIV]) {
        drs[DR_IDX_DIV]->invalidate(0, 0, false);
    }
}

i32 zViewBaseRibbon::onTouch(zTouch *touch) {
    bool drag(false);
    touch->drag(dragging, [this, &drag, &touch](cszi& offs, bool event) {
        // если сдвинули - определяем дельту в зависимости от ориентации
        auto _delta(vert ? offs.h : offs.w);
        if(_delta) {
            // определяем время сдвига
            auto t((int)((touch->ctm - touch->btm) / 50000000));
            // если отпустили и время < 10(выбрано экспериментально)
            if(event && t < 15) {
                // запускаем флинг
                flyng->pull((float)_delta / (float)sizes(vert), _delta > 0, vert);
                flyng->setVisible(true);
                // отправляем событие флинга
                post(MSG_FLYNG, nullptr);
                // сбрасываем клик и выделение
                clickItem = selectItem = -1;
            } else {
                // иначе - просто скроллим на дельту
                scrolling(_delta);
                // отправляем событие скролла
                if(onChangeScroll) onChangeScroll(this, _delta);
            }
            // признак перетаскивания
            drag = true;
        }
        if(!event) selectItem = -1;
    });
    // если не было перетаскивания
    if(!drag) {
        // если тап
        if(touch->isCaptured()) {
            // останавливаем флинг
            flyng->finish();
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
                post(MSG_CLICK, nullptr, 50, selectItem);
            }
            // если тапа нет - сбрасываем
            selectItem = clickItem = -1;
        }
    }
    // отображаем/скрываем выделение
    showSelector(selectItem != -1);
    return drag | touch->isCaptured();
}

void zViewBaseRibbon::onDraw() {
    zViewGroup::onDraw();
    auto div((zDrawableDivider*)drs[DR_IDX_DIV]);
    if(div) div->draw(countItem, firstItem);
}

void zViewBaseRibbon::setOrientation(bool _vert) {
    zViewGroup::setOrientation(_vert);
    vert = _vert; reset();
    if(adapter) countItem = adapter->getCount();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                                              ЛЕНТА                                                                     //
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

zViewRibbon::zViewRibbon(zStyle* _styles, i32 _id, bool _vert) : zViewBaseRibbon(_styles, _id, _vert) { }

void zViewRibbon::fill(int edge) {
    auto view(atView(0));
    if(view) fillReverse(firstItem - 1, view->edges(vert, false));
    int count(children.size());
    view = atView( count - 1);
    count += firstItem;
    if(firstItem == 0 && (divType & ZS_DIVIDER_BEGIN)) edge += divSize + divPadEnd;
    fillForward(count, (view ? view->edges(vert, true) : edgeStart) + edge);
}

// заполнять с конца к началу
void zViewRibbon::fillReverse(int pos, int next) {
    auto div((divType & ZS_DIVIDER_MIDDLE) ? divSize + divPadBegin + divPadEnd : 0);
    while(next > edgeStart && pos >= 0) {
        next = addView(next, pos, false, false)->edges(vert, false);
        next -= div; pos--;
    }
    firstItem = pos + 1;
    correctBegin(div);
}

// заполнять с начала до конца
void zViewRibbon::fillForward(int pos, int next) {
    auto div((divType & ZS_DIVIDER_MIDDLE) ? divSize + divPadBegin + divPadEnd : 0);
    while(next < edgeEnd && pos < countItem && pos >= 0) {
        next = addView(next, pos, true, true)->edges(vert, true);
        next += div; pos++;
    }
    correctFinish(div);
}

zView* zViewRibbon::addView(int coord, int pos, bool flow, bool end) {
    auto x(vert ? rclient.x : coord);
    auto y(vert ? coord: rclient.y);
    return zViewBaseRibbon::addView(x, y, pos, flow, end ? -1 : 0);
}

void zViewRibbon::childMeasure(zView* child, rti* lyt) {
    auto wSpec(makeChildMeasureSpec((vert ? MAKE_MEASURE_SPEC(MEASURE_EXACT, rfull.w) : measureSpec.w),
                                    padMargin(false), lyt->w));
    auto hSpec(makeChildMeasureSpec((vert ? measureSpec.h : MAKE_MEASURE_SPEC(MEASURE_EXACT, rfull.h)),
                                    padMargin(true), lyt->h));
    child->measure(wSpec, hSpec);
}

