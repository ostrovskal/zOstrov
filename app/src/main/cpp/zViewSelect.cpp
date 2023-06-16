//
// Created by User on 25.04.2022.
//

#include "zssh/zCommon.h"
#include "zssh/zViewRibbons.h"

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                                               DROPDOWN                                                                 //
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

szi zViewDropdown::measureChildrenSize(cszm& spec) {
    szi size(0, div ? div->resolve(countItem, false) : 0);
    for(int i = 0 ; i < countItem; i++) {
        auto child(obtainView(i));
        if(!child) continue;
        // расчитать размер
        measureChild(child, spec);
        // добавить в кэш
        addViewCache(child);
        // определить макс. габариты
        size.w = z_max(size.w, child->rview.w);
        size.h += child->rview.h;
    }
    return size;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                                                SELECT                                                                  //
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

zViewSelect::zViewSelect(zStyle* _styles, i32 _id) : zViewGroup(_styles, _id) {
    dropdown = new zViewDropdown(_styles);
    popup    = new zViewPopup(styles_default, this, dropdown);
    dropdown->setOnClick([this](zView* v, int sel) { setItemSelected(sel); });
    minMaxSize.set(z_dp(z.R.dimen.selectMinWidth), 0, z_dp(z.R.dimen.selectMinHeight), 0);
}

zViewSelect::~zViewSelect() {
    SAFE_DELETE(popup);
    SAFE_DELETE(dropdown);
    if(adapter) {
        adapter->unregistration(this);
        SAFE_DELETE(adapter);
    }
}

i32 zViewSelect::onTouchEvent(zTouch *touch) {
    if(touch->click()) {
        // показать попап
        popup->show(pti(0, rview.h));
    }
    return TOUCH_STOP;
}

zViewSelect* zViewSelect::setAdapter(zAdapterList *_adapter) {
    if(adapter) {
        adapter->unregistration(this);
        SAFE_DELETE(adapter);
    }
    if(_adapter) {
        adapter = new zAdapterSelect(this, _adapter);
        dropdown->setAdapter(adapter);
        adapter->registration(this);
    }
    return this;
}

void zViewSelect::stateView(STATE &state, bool save, int &index) {
    if(save) {
        state.data += selectItem;
    } else {
        selectItem = (int)state.data[index++];
    }
}

void zViewSelect::setItemSelected(int item) {
    auto view(adapter ? adapter->list->getView(item, atView(0), this) : nullptr);
    if(!countChildren()) attachInLayout(view, VIEW_MATCH, VIEW_MATCH);
    selectItem = item;
    // скорректировать список
    dropdown->setItemSelected(item);
    // скрыть окно
    popup->dismiss();
    // вызов события
    if(onChangeSelected) onChangeSelected(this, selectItem);
    // перерисовать
    invalidate();
}

void zViewSelect::onMeasure(cszm& spec) {
    // габариты по умолчанию
    int width(minMaxSize.x), height(minMaxSize.w);
    // если нет заголовка
    if(!countChildren()) setItemSelected(selectItem);
    // ширина/высота - неизвестно
    dropdown->measure({zMeasure(MEASURE_UNDEF, 0), zMeasure(MEASURE_UNDEF, 0)});
    auto view(atView(0));
    if(view) {
        view->measure({zMeasure(MEASURE_EXACT, dropdown->rclient.w), zMeasure(MEASURE_UNDEF, 0)});
        // подсчитать габариты селекта
        auto heightSize(spec.h.size()), widthSize(spec.w.size());
        if(spec.w.isExact()) width = widthSize;
        else {
            width = z_max(minMaxSize.x, view->rview.w + pad.extent(false));
            if(widthSize && spec.w.mode() == MEASURE_MOST) width = z_min(width, widthSize);
        }
        if(spec.h.isExact()) height = heightSize;
        else {
            height = z_max(minMaxSize.w, view->rview.h + pad.extent(true));
            if(heightSize && spec.h.mode() == MEASURE_MOST) height = z_min(height, heightSize);
        }
        view->measure({zMeasure(MEASURE_EXACT, width - pad.extent(false)), zMeasure(MEASURE_EXACT, height - pad.extent(true))});
        dropdown->measure({zMeasure(MEASURE_EXACT, width - pad.extent(false)), zMeasure(MEASURE_UNDEF, 0)});
    }
    setMeasuredDimension(width, height);
}

void zViewSelect::onLayout(crti& position, bool changed) {
    zView::onLayout(position, changed);
    // позиционируем заголовок
    auto view(atView(0));
    if(view) view->layout(rclient);
}

void zViewSelect::changeTheme() {
    popup->changeTheme();
    zViewGroup::changeTheme();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                                                 POPUP                                                                  //
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

zViewPopup::zViewPopup(zStyle* _styles, zView* _owner, zView* _content) : zViewGroup(_styles, z.R.id.wndPopup), owner(_owner) {
    setContent(_content);
    updateStatus(ZS_VISIBLED, false);
    updateStatus(ZS_SYSTEM, true);
}

void zViewPopup::setContent(zView* _content) {
    detach(content);
    if(_content) {
        attach(_content, VIEW_MATCH, VIEW_MATCH);
        content = _content;
    }
}

void zViewPopup::show(cpti& _offs) {
    offs = _offs;
    manager->getSystemView(false)->attach(this, VIEW_WRAP, VIEW_WRAP);
    updateStatus(ZS_VISIBLED, true);
    if(content) content->requestLayout();
}

i32 zViewPopup::touchEvent(AInputEvent *event) {
    auto action(zTouch::getEventAction(event));
    if(action == AMOTION_EVENT_ACTION_POINTER_DOWN || action == AMOTION_EVENT_ACTION_DOWN) {
        if(!zTouch::intersect(event, rview)) {
            dismiss(); return TOUCH_STOP;
        }
    }
    return zViewGroup::touchEvent(event);
}

void zViewPopup::onLayout(crti &position, bool changed) {
    rview.x = owner->rview.x + offs.x;
    rview.y = owner->edges(true, false) + offs.y;
    // проверка по гор.
    auto wScreen(zGL::instance()->getSizeScreen(false));
    if(rview.extent(false) > wScreen) rview.x = wScreen - rview.w;
    // проверка по верт.
    auto hScreen(zGL::instance()->getSizeScreen(true));
    if(rview.extent(true) > hScreen) {
        auto sub(rview.extent(true) - hScreen);
        // вниз - за пределы экрана
        // проверить вверх
        auto ry(owner->edges(true, false) - offs.y - rview.h);
        if(ry < 0) {
            ry = -ry;
            if(sub > ry) sub = ry, rview.y = 0;
        } else rview.y = ry, sub = 0;
        content->rview.h -= sub; content->rclient.h -= sub;
        rview.h -= sub; rclient.h -= sub;
    }
    zView::onLayout(rview, changed);
    rti r(rclient.x, rclient.y, content->rview.w, content->rview.h);
    content->layout(r);
}

i32 zViewPopup::keyEvent(int key, bool sysKey) {
    if(sysKey && key == AKEYCODE_BACK) { dismiss(); return 1; }
    return 0;
}

void zViewPopup::dismiss() {
    manager->getSystemView(false)->detach(this);
    updateStatus(ZS_VISIBLED, false);
}
