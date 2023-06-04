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
    detachAllViews(false);
    attach(_content, VIEW_MATCH, VIEW_MATCH);
    content = _content;
}

void zViewPopup::show(cpti& _offs) {
    offs = _offs;
    manager->getSystemView(false)->attach(this, VIEW_WRAP, VIEW_WRAP);
    updateStatus(ZS_VISIBLED, true);
    content->requestLayout();
}

void zViewPopup::dismiss() {
    //if(owner) owner->notifyOwner(MSG_POPUP_DISMISS, this, 0);
    manager->getSystemView(false)->detach(this);
}

i32 zViewPopup::touchEvent(AInputEvent *event) {
    auto action(zTouch::getEventAction(event));
    if(action == AMOTION_EVENT_ACTION_POINTER_DOWN || action == AMOTION_EVENT_ACTION_DOWN) {
        if(!zTouch::intersect(event, rview)) {
            dismiss();
            return TOUCH_STOP;
        }
    }
    return zViewGroup::touchEvent(event);
}

void zViewPopup::onLayout(crti &position, bool changed) {
    rview.x = owner->rview.x + offs.x - pad.x;
    rview.y = owner->edges(true, true) + offs.y;
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

void zViewPopup::onMeasure(cszm& spec) {
    setMeasuredDimension(content->rview.w + pad.extent(false), content->rview.h + pad.extent(true));
}

i32 zViewPopup::keyEvent(int key, bool sysKey) {
    if(sysKey && key == AKEYCODE_BACK) { dismiss(); return 1; }
    return 0;
}

szi zViewDropdown::measureChildrenSize(cszm& spec) {
    int wSize(0), hSize(div && div->resolve(countItem, true));
    for(int i = 0 ; i < countItem; i++) {
        auto child(obtainView(i));
        if(!child) continue;
        // расчитать размер
        measureChild(child, spec);
        // добавить в кэш
        addViewCache(child);
        // определить макс. габариты
        wSize = z_max(wSize, child->rview.w);
        hSize += child->rview.h;
    }
    return {wSize, hSize};
}
