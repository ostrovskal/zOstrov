
//
// Created by Sergo on 11.04.2022.
//

#include "zostrov/zCommon.h"
#include "zostrov/zViewRibbons.h"

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                                       БАЗОВАЯ ЛЕНТА                                                                    //
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

zViewBaseRibbon::zViewBaseRibbon(zStyle* _styles, i32 _id, bool _vert) : zViewGroup(_styles, _id) {
    vert = _vert; flyng = new zFlyng(this);
}

void zViewBaseRibbon::notifyEvent(HANDLER_MESSAGE *msg) {
    if(msg->what == MSG_ANIM) {
        float p; auto cont(animator.update(p));
        auto pp(z_round(p - pprev)); pprev = p;
        if(pp) offsetChildren(pp);
        if(p < 0) {
            fill(0, false);
        } else if(firstItem > 0) {
            fillReverse(firstItem - linesGrid, atView(0)->edges(vert, false));
        }
        if(!cont) {
            auto v(atView(0));
            deltaItem = (v ? v->edges(vert, false) - edge.w : 0);
        } else post(MSG_ANIM, duration);
        showSelector(clickItem != -1);
        invalidate();
    } else if(msg->what == MSG_ITEMS && countChildren()) {
        // чтобы не вызывать до бесконечности(если нет элементов)
        setItemSelected(msg->arg1);
    }
    zViewGroup::notifyEvent(msg);
}

zViewBaseRibbon::~zViewBaseRibbon() {
    cacheViews.clear();
    if(adapter) {
        if(adapter->unregistration(this))
            SAFE_DELETE(adapter);
    }
}

bool zViewBaseRibbon::touchChildren() {
    // проверить - был ли драг?
    bool ret((z_round(touch->cpt[vert] - touch->bpt[vert]) / sizeTouch[vert]) == 0);
    return ret;

}

zViewBaseRibbon* zViewBaseRibbon::setAdapter(zBaseAdapter* _adapter) {
    reset();
    if(adapter) {
        if(adapter->unregistration(this))
            SAFE_DELETE(adapter);
    }
    if(_adapter) {
        _adapter->registration(this);
        countItem = _adapter->getCount();
        adapter = _adapter;
    }
    return this;
}

void zViewBaseRibbon::reset() {
    cacheViews.clear();
    firstItem = countItem = deltaItem = 0;
    selectItem = clickItem = -1;
    removeAllViews(false);
    requestPosition();
//    zViewGroup::requestLayout();
}

void zViewBaseRibbon::correctBegin(int used) {
    auto v(atView(0));
    if(v) {
        auto _div(div ? div->getSize(ZS_DIVIDER_BEGIN) : 0);
        auto _delta(v->edges(vert, false) - edge.w);
        if(firstItem) _delta -= used; else _delta -= _div;
        if(_delta > 0) {
            if(glow || !testFlags(ZS_OVERSCROLL)) {
                offsetChildren(-_delta);
            } else {/* if(animator.frame >= animator.frames)*/
                animator.init(0, false);
                animator.add((float)-_delta, zInterpolator::EASEINQUAD, 10);
                pprev = 0; post(MSG_ANIM, duration / 2, 0);
            }
        }
    }
}

void zViewBaseRibbon::correctFinish(int used) {
    auto count(children.size() - 1);
    if(count >= 0 && (firstItem + count) == (countItem - 1)) {
        auto _div(div ? div->getSize(ZS_DIVIDER_END) : 0);
        auto pos(atView(count)->edges(vert, true) + _div);
        auto offset(edge.h - pos);
        auto first(atView(0)); auto start(first->edges(vert, false));
        if(offset > 0 && (firstItem > 0 || start < edge.w)) {
            if(firstItem == 0) offset = z_min(offset, (edge.w - start));
            if(glow || !testFlags(ZS_OVERSCROLL)) {
                offsetChildren(offset);
                if(firstItem > 0) fillReverse(firstItem - linesGrid, first->edges(vert, false) - used);
            } else {/* if(animator.frame >= animator.frames)*/
                animator.init(0, false);
                animator.add((float)offset, zInterpolator::EASEINQUAD, 10);
                pprev = 0; post(MSG_ANIM, duration / 2, 0);
            }
        }
    }
}

void zViewBaseRibbon::onInit(bool _theme) {
    zViewGroup::onInit(_theme);
    if(!adapter) {
        // если нет адаптера
        auto si(theme->findStyles(styles->_int(Z_STYLES_ITEM, z.R.style.list_item)));
        setAdapter(new zAdapterList(zString8(styles->_str(Z_ADAPTER_DATA, "no adapter!")).split("|"), new zFabricListItem(si)));
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
        if((view = cacheViews[i])) { ii = i; if(view->id == pos) break; }
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
        if(children[i]->rview.contains(x, y)) return i + firstItem;
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
            auto _div(div && (div->type & ZS_DIVIDER_END) ? div->size + div->padBegin : 0);
            pos = atView(count - 1)->edges(vert, true) + _div;
            auto isEdgeEnd(pos <= edge.h);
            auto isScrollToStart(!(isPosStart && isEdgeStart));
            auto isScrollToEnd(!(isPosEnd && isEdgeEnd));
            // проверить на возможность скролла, в зависимости от направления
            if((_delta > 0 && isScrollToStart) || (_delta < 0 && isScrollToEnd)) {
                auto posView(0);
                // проверка на выход за пределы списка
                while(posView < children.size()) {
                    auto child(atView(posView++));
                    // верхняя ниже нижней
                    auto isEnd((child->edges(vert, false) + _delta) >= edge.h);
                    // нижняя выше верхней
                    auto isStart((child->edges(vert, true) + _delta) <= edge.w);
                    if(isStart || isEnd) {
                        if(isStart) firstItem++;
                        detach(child); addViewCache(child);
                        posView--;
                    }
                }
                offsetChildren(_delta);
                fill(0, true);
                auto v(atView(0));
                deltaItem = (v ? v->edges(vert, false) - edge.w : 0);
                requestPosition();
                // отправляем событие скролла
                post(MSG_SCROLLING, duration, firstItem);
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
        auto s((view->edges(_vert, false) - edge.w) * 1000);
        return z_max(0, firstItem / linesGrid * 1000 - s / view->sizes(_vert));
    }
    return 0;
}

void zViewBaseRibbon::requestLayout() {
    auto drop(dynamic_cast<zViewDropdown*>(this));
    if(drop) zViewGroup::requestLayout();
}

int zViewBaseRibbon::computeScrollRange(bool _vert) const {
    return ((countItem + linesGrid - 1) / linesGrid) * 1000;
}

int zViewBaseRibbon::computeScrollExtent(bool _vert) const {
    auto count(children.size());
    if(count > 0) {
        auto extent(((count + linesGrid - 1) / linesGrid) * 1000);
        auto view(atView(0));
        auto s((view->edges(_vert, false) - edge.w) * 1000);
        extent += s / view->sizes(_vert);
        view = atView(count - 1); s = view->edges(_vert, true) - edge.w;
        extent -= (s - (edge.h - edge.w)) * 1000 / view->sizes(_vert);
        return extent;
    }
    return 0;
}

void zViewBaseRibbon::notifyAdapter(zBaseAdapter* _adapter) {
    reset(); countItem = _adapter->getCount();
}

void zViewBaseRibbon::changeTheme() {
    cacheViews.clear();
    zViewGroup::changeTheme();
}

void zViewBaseRibbon::stateView(STATE &state, bool save, int &index) {
    zViewGroup::stateView(state, save, index);
    if(save) {
        state.data += firstItem;
        state.data += deltaItem;
        state.data += selectItem;
    } else {
        firstItem = (int)state.data[index++];
        deltaItem = (int)state.data[index++];
        selectItem= (int)state.data[index++];
        requestLayout();
    }
}

void zViewBaseRibbon::showSelector(bool show) {
    if(drw[DRW_SEL]->isValid()) {
        if(show && selectItem >= 0) {
            // обновляем параметры селектора
            auto view(atView(selectItem - firstItem));
            if(view) {
                drw[DRW_SEL]->measure(view->rview.w, view->rview.h, 0, false);
                drw[DRW_SEL]->bound = view->rview;
            } else show = false;
        }
        drw[DRW_SEL]->visible = show;
        invalidate();
    }
}

void zViewBaseRibbon::setItemSelected(int item) {
    flyng->stop();
    auto _count(countChildren() - 1);
    if(_count < 0) {
        post(MSG_ITEMS, 20, item);
    } else {
        if(item >= countItem) item = countItem - 1;
        if(item < 0) item = 0;
        firstItem = item - _count / 2;
        if(selectItem != item) {
            selectItem = item;
            // вызов события
            post(MSG_SELECTED, duration, selectItem);
        }
        requestPosition();
    }
}

void zViewBaseRibbon::onLayout(crti &position, bool changed) {
    zView::onLayout(position, changed);
    edge.set(rclient[vert], rclient.extent(vert));
    // скорректировать первый видимый
    auto pos(firstItem), _count(countChildren() - 1);
    if(_count < 0) _count = countItem;
    if(pos + _count >= countItem) pos = countItem - _count;
    if(pos < 0) pos = 0;
    if(firstItem != pos) deltaItem = 0, firstItem = pos;
    // переместить в кэш и убрать все дочерние
    for(auto child : children) addViewCache(child);
    detachAllViews(false);
    // заполнить
    if(countItem > 0) fill(deltaItem, true);
    // показать прокрутку
    awakenScroll();
    // отображаем/скрываем выделение
    showSelector(clickItem != -1);
}

szi zViewBaseRibbon::measureChildrenSize(cszm& spec) {
    int limit(spec[vert].size()), i; szi _max;
    for(i = 0 ; i < countItem; i++) {
        auto child(obtainView(i));
        if(!child) continue;
        // расчитать размер
        measureChild(child, spec);
        // добавить в кэш
        addViewCache(child);
        // определить макс. габариты
        auto& rv(child->rview);
        if(_max.isEmpty()) {
            if(spec[vert].isNotExact()) {
                // предел делаем на макс. 5
                auto _limit(rv[vert + 2] * 6);
                if(_limit < limit) limit = _limit;
            }
        }
        _max[!vert] = z_max(_max[!vert], rv[3 - vert]);
        auto _size(_max[vert] + rv[vert + 2]);
        if(_size >= limit) break;
        _max[vert] = _size;
    }
    _max[vert] += (div ? div->resolve(i, false) : 0);
    return _max;
}

void zViewBaseRibbon::onMeasure(cszm& spec) {
    // расчитать габариты всех дочерних
    defaultOnMeasure(spec, measureChildrenSize(spec));
    // дивидер
    if(div) div->measure(0, 0, PIVOT_W | PIVOT_H, false);
}

i32 zViewBaseRibbon::onTouchEvent() {
    int ret(TOUCH_CONTINUE);
    touch->drag(sizeTouch, [this, &ret](cszi& offs, bool event) {
        // если сдвинули - определяем дельту в зависимости от ориентации
        auto _delta(offs[vert] * sizeTouch[vert]);
        if(_delta) {
            flyng->stop();
            // определяем время сдвига
            if(event && flyng->start(touch, _delta)) {
                // иначе - просто скроллим на дельту
            } else scrolling(_delta);
            // признак перетаскивания
            ret = TOUCH_FINISH;
            if(!testFlags(ZS_ALWAYS_SEL)) clickItem = -1;
        }
    });
    // если не было перетаскивания
    if(ret == TOUCH_CONTINUE) {
        // если тап
        if(touch->isCaptured()) {
            // останавливаем флинг
            flyng->stop();
            if(touch->just()) {
                if(clickItem == -1 || testFlags(ZS_ALWAYS_SEL)) {
                    // определяем индекс куда тапнули
                    if((clickItem = itemFromPoint(touch->cpt)) != -1) {
                        // запоминаем выделенный
                        post(MSG_SELECTED, duration, clickItem);
                        selectItem = clickItem;
                    }
                }
                ret = TOUCH_FINISH;
            }
        } else if(touch->isReleased()) {
            if(clickItem != -1 && selectItem == itemFromPoint(touch->cpt)) {
                // отправляем клик
                post(MSG_DROP_CLICK, 0, selectItem);
            }
            if(!testFlags(ZS_ALWAYS_SEL)) clickItem = -1;
            ret = TOUCH_ACTION;
        }
    }
    // отображаем/скрываем выделение
    showSelector(clickItem != -1);
    return ret;
}

void zViewBaseRibbon::onDraw() {
    if(div) div->make(getExtra(), countItem, firstItem);
    zViewGroup::onDraw();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                                              ЛЕНТА                                                                     //
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void zViewRibbon::fill(int _edge, bool reverse) {
    if(reverse) {
        auto view(atView(0));
        if(view) fillReverse(firstItem - 1, view->edges(vert, false));
    }
    int count(children.size());
    auto view(atView(count - 1));
    count += firstItem;
    if(firstItem == 0 && div) {
        auto begin(div->getSize(ZS_DIVIDER_BEGIN));
        if(_edge < begin && _edge >= 0) _edge += begin;
    }
    fillForward(count, (view ? view->edges(vert, true) : edge.w) + _edge);
}

// заполнять от конца к началу
void zViewRibbon::fillReverse(int pos, int next) {
    auto _div(div ? div->getSize(ZS_DIVIDER_MIDDLE) : 0);
    while(next > edge.w && pos >= 0) {
        next = addView(next - _div, pos--, false, false)->edges(vert, false) - _div;
    }
    firstItem = pos + 1;
    correctBegin(_div);
}

// заполнять с начала до конца
void zViewRibbon::fillForward(int pos, int next) {
    auto _div(div ? div->getSize(ZS_DIVIDER_MIDDLE) : 0);
    while(next < edge.h && pos < countItem && pos >= 0) {
        next = addView(next, pos++, true, true)->edges(vert, true) + _div;
    }
    correctFinish(_div);
}

zView* zViewRibbon::addView(int coord, int pos, bool flow, bool end) {
    auto x(vert ? rclient.x : coord), y(vert ? coord: rclient.y);
    return zViewBaseRibbon::addView(x, y, pos, flow, !end - 1);
}

void zViewRibbon::childMeasure(zView* child, zLayoutParams* lps) {
    auto wSpec(makeChildMeasureSpec((vert ? zMeasure(MEASURE_EXACT, rclient.w) : measureSpec.w), child->margin.extent(false), lps->w));
    auto hSpec(makeChildMeasureSpec((vert ? measureSpec.h : zMeasure(MEASURE_EXACT, rclient.h)), child->margin.extent(true), lps->h));
    child->measure({ wSpec, hSpec });
}
