//
// Created by User on 25.04.2022.
//

#include "zssh/zCommon.h"
#include "zssh/zViewRibbons.h"

zViewSelect::zViewSelect(zStyle* _styles, i32 _id) : zViewGroup(_styles, _id) {
    SAFE_DELETE(drs[DR_IDX_FK]);
    dropdown = new zViewDropdown(this, _styles);
    popup    = new zViewPopup(styles_default, this, dropdown);
    minSize.set(z_dp(z.R.dimen.selectMinWidth), z_dp(z.R.dimen.selectMinHeight));
}

zViewSelect::~zViewSelect() {
    vmanager->getCommonView()->detach(popup);
    SAFE_DELETE(popup);
    if(adapter) {
        adapter->unregisterOwner(this);
        SAFE_DELETE(adapter);
    }
}

i32 zViewSelect::onTouch(zTouch *touch) {
    if(touch->click()) {
        // показать попап
        popup->show(0, 0);
    }
    return TOUCH_STOP;
}

void zViewSelect::setAdapter(zAdapterList *_adapter) {
    if(adapter) {
        adapter->unregisterOwner(this);
        SAFE_DELETE(adapter);
    }
    if(_adapter) {
        adapter = new zAdapterSelect(this, _adapter);
        dropdown->setAdapter(adapter);
        adapter->registerOwner(this);
    }
}

void zViewSelect::stateView(VIEW_STATE &state, bool save, int &index) {
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
}

void zViewSelect::onMeasure(int widthSpec, int heightSpec) {
    // габариты по умолчанию
    int height(minSize.h), width(minSize.w);
    // подсчитать габариты списка
    // ширина - как у селекта, высота - неизвестно
    if(!countChildren()) setItemSelected(selectItem);
    dropdown->measure(makeChildMeasureSpec(0, 0, VIEW_WRAP),
                      makeChildMeasureSpec(0, 0, VIEW_WRAP));
    auto view(atView(0));
    if(view) {
        view->measure(makeChildMeasureSpec(widthSpec, padMargin(false), lytParams.w),
                                makeChildMeasureSpec(heightSpec, padMargin(true), VIEW_WRAP));
        // подсчитать габариты селекта
        auto hspec(MODE_MEASURE_SPEC(heightSpec)), heightSize(SIZE_MEASURE_SPEC(heightSpec));
        if(heightSize && hspec == MEASURE_EXACT) {
            height = heightSize;
        } else {
            height = z_max(minSize.h, view->rfull.h);
            if(heightSize && hspec == MEASURE_MOST) {
                height = z_min(height, heightSize);
            }
        }
        auto wspec(MODE_MEASURE_SPEC(widthSpec)), widthSize(SIZE_MEASURE_SPEC(widthSpec));
        if(widthSize && wspec == MEASURE_EXACT) {
            width = widthSize;
        } else {
            width = view->rfull.w;
            width = z_max(width, dropdown->rfull.w);
            width = z_max(minSize.w, width);
            if(widthSize && wspec == MEASURE_MOST) {
                width = z_min(width, widthSize);
            }
        }
        view->measure(makeChildMeasureSpec(MAKE_MEASURE_SPEC(MEASURE_EXACT, width), padMargin(false), VIEW_MATCH),
                      makeChildMeasureSpec(MAKE_MEASURE_SPEC(MEASURE_EXACT, height), padMargin(true), VIEW_MATCH));
        // если селект стал больше по ширине, пересчитать ширину списка
        if(width > dropdown->rfull.w) {
            dropdown->measure(makeChildMeasureSpec(MAKE_MEASURE_SPEC(MEASURE_EXACT, width), 0, VIEW_MATCH),
                              makeChildMeasureSpec(0, 0, VIEW_WRAP));
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

void zViewSelect::notifyOwner(int what, zView *view, int arg) {
    if(what == MSG_CLICK) {
        setItemSelected(arg);
    }
}
