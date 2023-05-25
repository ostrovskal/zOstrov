//
// Created by User on 25.04.2022.
//

#include "zssh/zCommon.h"
#include "zssh/zViewRibbons.h"

zViewPopup::zViewPopup(zStyle* _styles, zView* _owner, zView* _content) : zViewGroup(_styles, z.R.id.wndPopup), owner(_owner) {
    setContent(_content);
    updateStatus(ZS_VISIBLED, false);
    updateStatus(ZS_SYSTEM, true);
}

void zViewPopup::setContent(zView* _content) {
    detachAllViews();
    attach(_content, VIEW_MATCH, VIEW_MATCH);
    content = _content;
}

void zViewPopup::show(int xOffs, int yOffs) {
    offs.set(xOffs, yOffs);
    vmanager->getCommonView()->attach(this, VIEW_WRAP, VIEW_WRAP);
    updateStatus(ZS_VISIBLED, true);
    content->requestLayout();
}

void zViewPopup::dismiss() {
    if(owner) owner->notifyOwner(MSG_POPUP_DISMISS, this, 0);
    vmanager->getCommonView()->detach(this);
}

i32 zViewPopup::touchEvent(AInputEvent *event) {
    auto action(zTouch::getEventAction(event));
    if(action == AMOTION_EVENT_ACTION_POINTER_DOWN || action == AMOTION_EVENT_ACTION_DOWN) {
        if(!zTouch::intersect(event, rfull)) {
            dismiss();
            return TOUCH_STOP;
        }
    }
    return zViewGroup::touchEvent(event);
}

void zViewPopup::onLayout(crti &position, bool changed) {
    rfull.x = owner->rfull.x + offs.x - pad.x;
    rfull.y = owner->edges(true, true) + offs.y;
    // проверка по гор.
    auto wScreen(vmanager->getScreen(false));
    if(rfull.extent(false) > wScreen) rfull.x = wScreen - rfull.w;
    // проверка по верт.
    auto hScreen(vmanager->getScreen(true));
    if(rfull.extent(true) > hScreen) {
        auto sub(rfull.extent(true) - hScreen);
        // вниз - за пределы экрана
        // проверить вверх
        auto ry(owner->edges(true, false) - offs.y - rfull.h);
        if(ry < 0) {
            ry = -ry;
            if(sub > ry) sub = ry, rfull.y = 0;
        } else rfull.y = ry, sub = 0;
        content->rfull.h -= sub; content->rclient.h -= sub;
        rfull.h -= sub; rclient.h -= sub;
    }
    zView::onLayout(rfull, changed);
    rti r(rclient.x, rclient.y, content->rfull.w, content->rfull.h);
    content->layout(r);
}

void zViewPopup::onMeasure(int widthSpec, int heightSpec) {
    setMeasuredDimension(content->rfull.w + pad.extent(false), content->rfull.h + pad.extent(true));
}

i32 zViewPopup::onKey(int key, bool sysKey) {
    if(sysKey && key == AKEYCODE_BACK) {
        dismiss();
        return 1;
    }
    return 0;
}

szi zViewDropdown::measureChildrenSize(int widthSpec, int heightSpec) {
    auto wSize(0), hSize(resolveDivider(countItem));
    for(int i = 0 ; i < countItem; i++) {
        auto child(obtainView(i));
        if(!child) continue;
        // расчитать размер
        measureChild(child, widthSpec, heightSpec);
        // добавить в кэш
        addViewCache(child);
        // определить макс. габариты
        wSize = z_max(wSize, child->rfull.w);
        hSize += child->rfull.h;
    }
    return {wSize, hSize};
}
