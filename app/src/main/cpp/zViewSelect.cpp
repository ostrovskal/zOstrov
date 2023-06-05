//
// Created by User on 25.04.2022.
//

#include "zssh/zCommon.h"
#include "zssh/zViewRibbons.h"

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
        popup->show(pti());
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
    invalidate();
}

void zViewSelect::onMeasure(cszm& spec) {
    // габариты по умолчанию
    int width(minMaxSize.x), height(minMaxSize.w);
    // подсчитать габариты списка
    // ширина - как у селекта, высота - неизвестно
    if(!countChildren()) setItemSelected(selectItem);
    dropdown->measure({makeChildMeasureSpec(0, 0, VIEW_WRAP), makeChildMeasureSpec(0, 0, VIEW_WRAP)});
    auto view(atView(0));
    if(view) {
        view->measure({makeChildMeasureSpec(spec.w, padMargin(false), lps.w), makeChildMeasureSpec(spec.h, padMargin(true), VIEW_WRAP)});
        // подсчитать габариты селекта
        auto widthSize(spec.w.size()), heightSize(spec.h.size());
        if(spec.h.isExact()) {
            height = heightSize;
        } else {
            height = z_max(minMaxSize.w, view->rview.h);
            if(heightSize && spec.h.mode() == MEASURE_MOST) height = z_min(height, heightSize);
        }
        if(spec.w.isExact()) {
            width = widthSize;
        } else {
            width = z_max(minMaxSize.x, z_max(view->rview.w, dropdown->rview.w));
            if(widthSize && spec.w.mode() == MEASURE_MOST) width = z_min(width, widthSize);
        }
        view->measure({makeChildMeasureSpec(zMeasure(MEASURE_EXACT, width), padMargin(false), VIEW_MATCH),
                      makeChildMeasureSpec(zMeasure(MEASURE_EXACT, height), padMargin(true), VIEW_MATCH)});
        // если селект стал больше по ширине, пересчитать ширину списка
        if(width > dropdown->rview.w) {
            dropdown->measure({makeChildMeasureSpec(zMeasure(MEASURE_EXACT, width), 0, VIEW_MATCH), makeChildMeasureSpec(0, 0, VIEW_WRAP)});
        }
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
