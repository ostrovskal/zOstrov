
//
// Created by Sergo on 11.04.2022.
//

#include "zssh/zCommon.h"
#include "zssh/zViewRibbons.h"

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                                       БАЗОВАЯ ЛЕНТА                                                                    //
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

zViewBaseRibbon::~zViewBaseRibbon() {
    cacheViews.clear();
    if(adapter) {
        adapter->unregistration(this);
        adapter = nullptr;
    }
}

void zViewBaseRibbon::setAdapter(zBaseAdapter* _adapter) {
    reset();
    if(adapter) adapter->unregistration(this);
    adapter = _adapter;
    if(_adapter) {
        _adapter->registration(this);
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
        auto _delta(v->edges(vert, false) - edge.w);
        if(firstItem) _delta -= used;
        if(_delta > 0) offsetChildren(-_delta, vert);
    }
}

void zViewBaseRibbon::correctFinish(int used) {
    auto count(children.size() - 1);
    if(count >= 0 && (firstItem + count) == (countItem - 1)) {
        auto pos(atView(count)->edges(vert, true));
        auto offset(edge.h - pos);
        auto first(atView(0)); auto start(first->edges(vert, false));
        if(offset > 0 && (firstItem > 0 || start < edge.w)) {
            if(firstItem == 0) offset = z_min(offset, (edge.w - start));
            offsetChildren(offset, vert);
            if(firstItem > 0) fillReverse(firstItem - linesGrid, first->edges(vert, false) - used);
        }
    }
}

void zViewBaseRibbon::addViewCache(zView* view) {
    for(auto& v : cacheViews) if(!v) { v = view; return; }
    cacheViews += view;
}

zView *zViewBaseRibbon::getViewCache(int pos) {
    // брать представление по его позиции(чтобы минимизировать обновление)
    zView* view(nullptr); int ii(-1);
    for(int i = 0 ; i < cacheViews.size(); i++) {
        if((view = cacheViews[i])) {
            ii = i; if(view->id == pos) break;
        }
    }
    if(ii >= 0) view = cacheViews[ii], cacheViews[ii] = nullptr;
    return view;
}

zView* zViewBaseRibbon::obtainView(int position, bool* needToMeasure) {
    auto view(getViewCache(position));
    auto child(adapter->getView(position, view, this));
    if(needToMeasure) *needToMeasure = (view != child);
    return child;
}

// Добавление видимого элемента из кэша или из адаптера
// where 0 - в начало, -1 в конец
zView* zViewBaseRibbon::addView(int x, int y, int pos, bool flow, int where) {
    bool needToMeasure(true);
    auto child(obtainView(pos, &needToMeasure));
    // привязать к родителю
    auto lps(&child->lps);
    attachInLayout(child, lps->w, lps->h, where);
    // вычислить размеры дочернего
    if(needToMeasure || !child->testFlags(ZS_MEASURE)) childMeasure(child, lps);
    // сдвинуть представление, в зависимости от требований списка
    auto childWidth(child->rview.w), childHeight(child->rview.h);
    auto childTop((vert ? (flow ? y : y - childHeight) : y));
    auto childLeft((vert ? x : (flow ? x : x - childWidth)));
    // определить позицию
    child->layout({ childLeft, childTop, childWidth, childHeight });
    return child;
}

int zViewBaseRibbon::itemFromPoint(cptf& p) const {
    auto x((int)p.x), y((int)p.y);
    for(int i = children.size() - 1; i >= 0; i--) {
        if(children[i]->rview.contains(x, y))
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
            auto isEdgeStart(pos >= edge.w);
            // последний выше нижней грани?
            pos = atView(count - 1)->edges(vert, true);
            auto isEdgeEnd(pos <= edge.h);
            auto isScrollToStart(!(isPosStart && isEdgeStart));
            auto isScrollToEnd(!(isPosEnd && isEdgeEnd));
            // проверить на возможность скролла, в зависимости от направления
            if((_delta > 0 && isScrollToStart) || (_delta < 0 && isScrollToEnd)) {
                auto posView(0);
                while(posView < children.size()) {
                    auto child(atView(posView));
                    // проверка на выход за пределы списка
                    // верхняя ниже нижней
                    auto isStart((child->edges(vert, false) + _delta) >= edge.h);
                    // нижняя выше верхней
                    auto isEnd((child->edges(vert, true) + _delta) <= edge.w);
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
                deltaItem = (view ? view->edges(vert, false) - edge.w : 0);
                invalidate();
                return false;
            }
            // Предел прокрутки. Запуск эффекта
            updateGlow(_delta);
        }
    }
    return true;
}

int zViewBaseRibbon::computeScrollOffset(bool _vert) const {
    if(children.isNotEmpty()) {
        auto view(atView(0));
        auto s((view->edges(_vert, false) - edge.w) * 100);
        return z_max(0, firstItem / linesGrid * 100 - s / view->sizes(_vert));
    }
    return 0;
}

int zViewBaseRibbon::computeScrollRange(bool _vert) const {
    return ((countItem + linesGrid - 1) / linesGrid) * 100;
}

int zViewBaseRibbon::computeScrollExtent(bool _vert) const {
    auto count(children.size());
    if(count > 0) {
        auto extent(((count + linesGrid - 1) / linesGrid) * 100);
        auto view(atView(0));
        auto s((view->edges(_vert, false) - edge.w) * 100);
        extent += s / view->sizes(_vert);
        view = atView(count - 1); s = view->edges(_vert, true) - edge.w;
        extent -= (s - (edge.h - edge.w)) * 100 / view->sizes(_vert);
        return extent;
    }
    return 0;
}

void zViewBaseRibbon::notifyAdapter(zBaseAdapter* _adapter) {
    countItem = _adapter->getCount();
    requestLayout();
}

void zViewBaseRibbon::stateView(STATE &state, bool save, int &index) {
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
    if(drw[DRW_SEL]->isValid()) {
        drw[DRW_SEL]->visible = show;
        if(show) {
            // обновляем параметры селектора
            auto view(atView(selectItem - firstItem));
            if(view) drw[DRW_SEL]->measure(view->rview.w, view->rview.h, 0, false);
        }
    }
}

void zViewBaseRibbon::setItemSelected(int item) {
    flyng->stop();
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
    int posEnd(0); edge.set(rclient[vert], rclient.extent(vert));
    // скорректировать первый видимый
    auto pos(firstItem), _count(countChildren());
    if(_count == 0) _count = countItem;
    if(pos + _count >= countItem) pos = countItem - _count;
    if(pos < 0) pos = 0;
    if(firstItem != pos) deltaItem = 0, firstItem = pos;
    if(isOffsetEnd && _count) {
        int ofsBegin((div && (div->type & ZS_DIVIDER_BEGIN) && firstItem == 0) ? div->size + div->padEnd : 0);
        auto posBegin(z_min(0, atView(0)->edges(vert, false) - ofsBegin - edge.w));
        posEnd = (z_min(0, edge.h - atView(_count - 1)->edges(vert, true)));
        posEnd += posBegin;
    }
    // переместить в кэш
    for(auto& child : children) addViewCache(child);
    // убрать все дочерние
    detachAllViews();
    // заполнить
    if(countItem > 0) fill(deltaItem + posEnd);
    isOffsetEnd = false;
    showSelector(selectItem != -1);
    // "будим" прокрутку
    awakenScroll();
}

szi zViewBaseRibbon::measureChildrenSize(cszm& spec) {
    int limit(spec[vert].mode()), hmax(0), wmax(0), i;
    auto hSize(vert ? &hmax : &wmax), wSize(vert ? &wmax : &hmax);
    for(i = 0 ; i < countItem; i++) {
        auto child(obtainView(i));
        if(!child) continue;
        // расчитать размер
        measureChild(child, spec);
        // добавить в кэш
        addViewCache(child);
        // определить макс. габариты
        auto rv(&child->rview);
        //auto wc(child->rview.w), hc(child->rview.h);
        if(wmax == 0 && hmax == 0) {
            if(spec[vert].isNotExact()) {
                // предел делаем на макс. 5
                auto _limit(rv->buf[vert + 2] * 5);//(vert ? hc : wc) * 5);
                if(_limit > limit) limit = _limit;
            }
        }
        *wSize = z_max(*wSize, rv->buf[3 - vert]);//(vert ? wc : hc));
        auto _size(*hSize + rv->buf[vert + 2]);//(vert ? hc : wc));
        if(_size >= limit) break;
        *hSize = _size;
    }
    auto _size(div && div->resolve(i));
    if(vert) hmax += _size; else wmax += _size;
    return {wmax, hmax};
}

void zViewBaseRibbon::onMeasure(cszm& spec) {
    if(!adapter) {
        ILOG("Не установлен адаптер в ленте!!!");
        return;
    }
    // расчитать габариты всех дочерних
    auto size(measureChildrenSize(spec));
    cacheViews.clear();
    defaultOnMeasure(spec, size.w, size.h);
    // дивидер
    if(div) div->measure(0, 0, 0, false);
}

i32 zViewBaseRibbon::onTouchEvent(zTouch *touch) {
    bool drag(false);
    touch->drag(sizeTouch, [this, &drag, &touch](cszi& offs, bool event) {
        // если сдвинули - определяем дельту в зависимости от ориентации
        auto _delta(vert ? offs.h : offs.w);
        if(_delta) {
            // определяем время сдвига
            auto t((int)((touch->ctm - touch->btm) / 50000000));
            // если отпустили и время < 10(выбрано экспериментально)
            if(event && t < 15) {
                // запускаем флинг
                //flyng->pull((float)_delta / (float)sizes(vert), _delta > 0, vert);
                //flyng->visible = true;
                // отправляем событие флинга
                //post(MSG_FLYNG, nullptr);
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
                //post(MSG_CLICK, nullptr, 50, selectItem);
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
    if(div) div->make(0, countItem, firstItem);
}

void zViewBaseRibbon::setOrientation(bool _vert) {
    zViewGroup::setOrientation(_vert);
    reset(); countItem = adapter->getCount();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                                              ЛЕНТА                                                                     //
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void zViewRibbon::fill(int _edge) {
    auto view(atView(0));
    if(view) fillReverse(firstItem - 1, view->edges(vert, false));
    int count(children.size());
    view = atView( count - 1);
    count += firstItem;
    if(firstItem == 0 && div && div->type & ZS_DIVIDER_BEGIN) _edge += div->size + div->padEnd;
    fillForward(count, (view ? view->edges(vert, true) : edge.w) + _edge);
}

// заполнять с конца к началу
void zViewRibbon::fillReverse(int pos, int next) {
    auto _div(div && (div->type & ZS_DIVIDER_MIDDLE) ? div->size + div->padBegin + div->padEnd : 0);
    while(next > edge.w && pos >= 0) {
        next = addView(next, pos, false, false)->edges(vert, false);
        next -= _div; pos--;
    }
    firstItem = pos + 1;
    correctBegin(_div);
}

// заполнять с начала до конца
void zViewRibbon::fillForward(int pos, int next) {
    auto _div(div && (div->type & ZS_DIVIDER_MIDDLE) ? div->size + div->padBegin + div->padEnd : 0);
    while(next < edge.h && pos < countItem && pos >= 0) {
        next = addView(next, pos, true, true)->edges(vert, true);
        next += _div; pos++;
    }
    correctFinish(_div);
}

zView* zViewRibbon::addView(int coord, int pos, bool flow, bool end) {
    auto x(vert ? rclient.x : coord);
    auto y(vert ? coord: rclient.y);
    return zViewBaseRibbon::addView(x, y, pos, flow, end ? -1 : 0);
}

void zViewRibbon::childMeasure(zView* child, zLayoutParams* lps) {
    auto wSpec(makeChildMeasureSpec((vert ? zMeasure(MEASURE_EXACT, rview.w) : measureSpec.w), padMargin(false), lps->w));
    auto hSpec(makeChildMeasureSpec((vert ? measureSpec.h : zMeasure(MEASURE_EXACT, rview.h)), padMargin(true), lps->h));
    child->measure({ wSpec, hSpec });
}

