//
// Created by User on 25.04.2022.
//

#include "zostrov/zCommon.h"
#include "zostrov/zViewRibbons.h"

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                                               DROPDOWN                                                                 //
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void zViewDropdown::onDetach() {
    if(adapter) adapter->unregistration(this);
    styles = nullptr; adapter = nullptr; owner = nullptr;
    detachAllViews(false);
}

void zViewDropdown::notifyEvent(HANDLER_MESSAGE* msg) {
    if(msg->what == MSG_DROP_CLICK && owner) {
        owner->post(MSG_DROP_CLICK, 0, msg->arg1, msg->arg2, msg->sarg);
    }
    zViewRibbon::notifyEvent(msg);
}

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
    size.w += pad.extent(false);
    return size;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                                                SELECT                                                                  //
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

zViewSelect::zViewSelect(zStyle* _styles, i32 _id) : zViewGroup(_styles, _id) {
    minMaxSize.set(z_dp(z.R.dimen.selectMinWidth), 0, z_dp(z.R.dimen.selectMinHeight), 0);
    updateStatus(ZS_CLICKABLE, true);
}

zViewSelect::~zViewSelect() {
    manager->getPopup()->dismiss();
    if(adapter) {
        if(adapter->unregistration(this))
            SAFE_DELETE(adapter);
    }
}

void zViewSelect::onInit(bool _theme) {
    zViewGroup::onInit(_theme);
    if(!adapter) {
        // если нет адаптера
        auto si(theme->findStyles(styles->_int(Z_STYLES_ITEM, z.R.style.spin_item)));
        setAdapter(new zAdapterList(zString8(styles->_str(Z_ADAPTER_DATA, "no adapter!")).split("|"),
                                    new zFabricSelectItem(styles_z_spin_capt), new zFabricSelectItem(si)));
    }
}

void zViewSelect::reset() {
    selectItem = 0;
    removeAllViews(false);
    requestLayout();
}

zViewSelect* zViewSelect::setAdapter(zAdapterList *_adapter) {
    reset();
    if(adapter) {
        if(adapter->unregistration(this))
            SAFE_DELETE(adapter);
    }
    if(_adapter) {
        adapter = new zAdapterSelect(this, _adapter);
        adapter->registration(this);
    }
    return this;
}

void zViewSelect::stateView(STATE &state, bool save, int &index) {
    if(save) {
        state.data += selectItem;
    } else {
        setItemSelected((int)state.data[index++]);
    }
}

void zViewSelect::setItemSelected(int item) {
    auto countItem(adapter->getCount());
    if(item >= countItem) item = countItem - 1;
    if(item < 0) item = 0;
    auto view(adapter->list->getView(item, atView(0), this));
    if(!countChildren()) attachInLayout(view, VIEW_MATCH, VIEW_MATCH);
    if(selectItem != item) {
        selectItem = item;
        // вызов события
        post(MSG_SELECTED, duration, selectItem);
    }
    // скрыть окно
    manager->getPopup()->dismiss();
    // перерисовать
    invalidate();
}

void zViewSelect::notifyEvent(HANDLER_MESSAGE* msg) {
    if(msg->what == MSG_CLICK) {
        auto dropdown(manager->getDropdown(this, styles, adapter));
        dropdown->measure(dropMeasure);
        // скорректировать список
        dropdown->setItemSelected(selectItem);
        manager->getPopup()->show(0, rview.h, this, dropdown);
    } else if(msg->what == MSG_DROP_CLICK) {
        setItemSelected(msg->arg1);
        return;
    }
    zViewGroup::notifyEvent(msg);
}

void zViewSelect::onMeasure(cszm& spec) {
    // габариты по умолчанию
    int width(minMaxSize.x), height(minMaxSize.w);
    // если нет заголовка
    if(!countChildren()) setItemSelected(selectItem);
    auto view(atView(0));
    if(view) {
        // ширина/высота - неизвестно
        view->measure({zMeasure(MEASURE_UNDEF, 0), zMeasure(MEASURE_UNDEF, 0)});
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
        view->measure({zMeasure(MEASURE_EXACT, width), zMeasure(MEASURE_EXACT, height)});
        // теперь считаем габариты списка
        auto dropdown(manager->getDropdown(this, styles, adapter));
        // сначала его собственные
        dropMeasure.empty(); dropdown->measure(dropMeasure);
        dropMeasure.set(zMeasure(MEASURE_EXACT, z_max(width, dropdown->rview.w)), zMeasure(MEASURE_UNDEF, 0));
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
    manager->getPopup()->changeTheme();
    zViewGroup::changeTheme();
}

bool zViewSelect::testLocked() const {
    return manager->getPopup()->isVisibled();
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                                                 POPUP                                                                  //
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

zViewPopup::zViewPopup() : zViewGroup(styles_default, z.R.id.wndPopup) {
    updateStatus(ZS_VISIBLED, false);
    updateStatus(ZS_SYSTEM, true);
}

void zViewPopup::show(int x, int y, zView* _owner, zView* _content, zStyle* _styles) {
    offs.set(x, y); owner = _owner;
    if(_styles) setStyles(_styles);
    if(_content) {
        detachAllViews(false);
        attachInLayout(_content, VIEW_MATCH, VIEW_MATCH);
        content = _content;
    }
    updateStatus(ZS_VISIBLED, true);
    manager->getSystemView(false)->attach(this, VIEW_WRAP, VIEW_WRAP);
}

i32 zViewPopup::touchEventDispatch(AInputEvent *event) {
    if(isVisibled()) {
        auto action(zTouch::getEventAction(event));
        if(action == AMOTION_EVENT_ACTION_POINTER_DOWN || action == AMOTION_EVENT_ACTION_DOWN) {
            if(!zTouch::intersect(event, rview)) {
                dismiss(); return TOUCH_STOP;
            }
        }
        return zViewGroup::touchEventDispatch(event);
    }
    return TOUCH_CONTINUE;
}

void zViewPopup::onMeasure(cszm& spec) {
    setMeasuredDimension(content->rview.w + pad.extent(false),
                         content->rview.h + pad.extent(true));
}

void zViewPopup::onLayout(crti &position, bool changed) {
    rview.x = owner->rview.x + offs.x;
    rview.y = owner->edges(true, false) + offs.y;
    // проверка по гор.
    auto wScreen(zGL::instance()->getSizeScreen(false) - 2_dp);
    if(rview.extent(false) > wScreen) rview.x = wScreen - rview.w;
    // проверка по верт.
    auto hScreen(zGL::instance()->getSizeScreen(true) - 2_dp);
    if(rview.extent(true) > hScreen) {
        auto sub(rview.extent(true) - hScreen);
        // вниз - за пределы экрана/проверить вверх
        auto ry(owner->edges(true, false) - rview.h);
        if(ry < 0) {
            ry = -ry;
            if(sub > ry) sub = ry, rview.y = 0;
        } else rview.y = ry, sub = 0;
        content->rview.h -= sub; content->rclient.h -= sub;
        rview.h -= sub; rclient.h -= sub;
    }
    zView::onLayout(rview, changed);
    content->layout(rview);
//    RTI_LOG("popup", rview);
}

i32 zViewPopup::keyEvent(int key, bool sysKey) {
    if(sysKey && key == AKEYCODE_BACK) { dismiss(); return 1; }
    return 0;
}

void zViewPopup::dismiss() {
    detach(content); content = nullptr;
    manager->getSystemView(false)->detach(this);
    updateStatus(ZS_VISIBLED, false);
}
