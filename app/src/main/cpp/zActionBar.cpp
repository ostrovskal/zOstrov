//
// Created by User on 25.04.2022.
//

#include "zssh/zCommon.h"
#include "zssh/zViewGroup.h"
#include "zssh/zViewWidgets.h"
#include "zssh/zViewRibbons.h"

/*
zActionBar::zActionBar(zStyle* _styles, zStyle* _styles_buttons, zStyle* _styles_popup) :
                    zLinearLayout(_styles, z.R.id.actionBar, false), styles_buttons(_styles_buttons) {
    content  = new zFrameLayout(styles_default, 0);
    dropdown = new zViewDropdown(this, _styles_popup);
    popup    = new zViewPopup(styles_default, this, dropdown);
    status   &= ~ZS_VISIBLED;
    minSize.set(z_dp(z.R.dimen.actionMinWidth), z_dp(z.R.dimen.actionMinHeight));
}

zActionBar::~zActionBar() {
    resetOverflow();
    vmanager->getCommonView()->detach(popup);
    SAFE_DELETE(popup);
    if(adapter) {
        adapter->unregisterOwner(this);
        SAFE_DELETE(adapter);
    }
}

void zActionBar::setContent(zView* _view) {
    if(content) {
        content->removeAllViews();
        content->attach(_view, VIEW_MATCH, VIEW_MATCH);
    }
}

void zActionBar::resetOverflow() {
    for(int i = 0 ; i < overflow.size(); i++) {
        overflow.children[i]->pop = nullptr;
    }
    overflow.reset();
}

void zActionBar::recursiveMenu(POPUPMENU* pop) {
    while(_menu->type != menuEnd) {
        switch(_menu->type) {
            case menuPopupBegin: {
                auto _pop(new POPUPMENU(_menu->id));
                pop->add(new MENUITEM(_menu->id, _menu->image, _menu->title, _menu->action | menuPopup, _pop));
                _menu++; recursiveMenu(_pop);
                break;
            }
            case menuPopupEnd: return;
            default:
                pop->add(new MENUITEM(_menu->id, _menu->image, _menu->title, _menu->action, nullptr));
                break;
        }
        _menu++;
    }
}

bool zActionBar::setMenu(int iconApp, zMenuItem* menu) {
    delete root; _menu = menu;
    // изображение программы
    root = (new POPUPMENU()); root->add(new MENUITEM(z.R.id.menuApp, iconApp, 0, actAlways, nullptr));
    recursiveMenu(root);
    // отсортировать первый уровень по actAlways
    int ii(1);
    for(int i = 1 ; i < root->size(); i++) {
        auto item(root->getItem(i));
        if(!item->isAlways()) continue;
        root->children.swap(i, ii);
        ii++;
    }
    return true;
}

int zActionBar::measureButton(int _id, int _image, int widthSize, int heightSize, POPUPMENU* pop, bool isMeasure) {
    int width(0);
    auto but(new zViewImage(_id == z.R.id.menuOverflow ? styles_z_baroverflow : styles_buttons, _id, _image));
    but->setTag((u64)pop); but->setOnClick([this](zView* v, HANDLER_MESSAGE*) {
        clickPopup(v, (POPUPMENU*)v->tag);
        vmanager->messagePost(v, MSG_MENU_CLICK, nullptr, 50);
    });
    attach(but, VIEW_WRAP, VIEW_WRAP);
    if(isMeasure) {
        auto ws(makeChildMeasureSpec(MAKE_MEASURE_SPEC(MEASURE_MOST, widthSize), padMargin(false), VIEW_WRAP));
        auto hs(makeChildMeasureSpec(MAKE_MEASURE_SPEC(MEASURE_MOST, heightSize), padMargin(true), VIEW_WRAP));
        but->measure(ws, hs);
        width = but->rfull.w;
    }
    return width;
}

void zActionBar::onMeasure(int widthSpec, int heightSpec) {
    auto widthSize(SIZE_MEASURE_SPEC(widthSpec)), heightSize(SIZE_MEASURE_SPEC(heightSpec));
    int wContent(z_percent(widthSize, 33)), wBut(0);
    int width(wContent), i, count(root ? root->size() : 0);
    detach(content); resetOverflow(); removeAllViews();
    for(i = 0 ; i < count; ) {
        auto item(root->getItem(i++)); auto pop(item->pop);
        if(!item->isVisibled()) continue;
        if(!item->isNever()) {
            auto w(measureButton(item->id, item->image, widthSize, heightSize, pop, wBut == 0));
            if(w) wBut = w;
            width += wBut;
            if((width + wBut * 2) >= widthSize) break;
        } else {
            if(pop && !pop->isNotEmpty()) pop = nullptr;
            overflow.add(new MENUITEM(item->id, item->image, item->text, item->flags, pop));
        }
    }
    // overflow
    for(; i < count; i++) {
        auto item(root->getItem(i)); auto pop(item->pop);
        if(pop && !pop->isNotEmpty()) pop = nullptr;
        overflow.add(new MENUITEM(item->id, item->image, item->text, item->flags, pop));
    }
    if(overflow.isNotEmpty() || count == 0) {
        width += measureButton(z.R.id.menuOverflow, z.R.integer.iconMenu, widthSize, heightSize, &overflow, wBut == 0);
    }
    // контент
    wContent += z_max(0, widthSize - width);
    attach(content, wContent, VIEW_MATCH, count != 0);
    zLinearLayout::onMeasure(widthSpec, heightSpec);
}

bool zActionBar::onRedraw() {
    bool isdelay(false);
    if(!delay) {
        if(frame >= 0 && frame < 9) {
            if(rfull.h) {
                lytParams.y = -rfull.h + frame * (rfull.h / 8);
                frame += isChecked() * 2 - 1;
                if(frame <= 0) updateStatus(ZS_VISIBLED, false);
                invalidate();
            }
            return false;
        }
        isdelay = isChecked();
        delay = 1;
    }
    if(testLocked() || isdelay) {
        // подождем
        post(MSG_REDRAW, nullptr, 20 * ACTION_DELAY);
    } else {
        // спратать
        show(false);
    }
    return true;
}

void zActionBar::show(bool _show) {
    if(isChecked() != _show) {
        updateStatus(ZS_CHECKED, _show);
        updateStatus(ZS_VISIBLED, true);
        delay = 0; frame = 8 * !isChecked();
        post(MSG_REDRAW);
        requestLayout();
        onRedraw();
    }
}

void zActionBar::setAdapter(zAdapterList* _adapter) {
    if(adapter) {
        adapter->unregisterOwner(this);
        SAFE_DELETE(adapter);
    }
    if(_adapter) {
        adapter = new zAdapterMenu(this, _adapter);
        dropdown->setAdapter(adapter);
        adapter->registerOwner(this);
    }
}

void zActionBar::notifyOwner(int what, zView *view, int arg) {
    if(what == MSG_CLICK) {
        auto item(&adapter->getItem(arg));
        if(!item->isEnabled()) return;
        vmanager->messagePost(view, MSG_MENU_CLICK, nullptr, 20, arg);
        popup->dismiss();
        if(item->pop) clickPopup(buttonView, item->pop);
    }
}

void zActionBar::clickPopup(zView* view, POPUPMENU* pop) {
    if(pop && pop->isNotEmpty()) {
        vmanager->initializeMenuItem(pop);
        if(adapter) adapter->setPopup(pop);
        dropdown->measure(zViewGroup::makeChildMeasureSpec(MAKE_MEASURE_SPEC(MEASURE_UNDEF, 0), 0, VIEW_WRAP),
                          zViewGroup::makeChildMeasureSpec(measureSpec.h, 0, VIEW_WRAP));
        popup->show(view->rfull.x, 0);
    } else {
        popup->dismiss();
    }
    buttonView = view;
}

bool zActionBar::testLocked() const {
    if(popup->isVisibled()) return true;
    return zViewGroup::testLocked();
}

zView* zActionBar::zAdapterMenu::getView(int position, zView *convert, zViewGroup *parent) {
    auto nv((zLinearLayout*)list->getView(position, convert, bar));
    auto item(popupmenu->getItem(position));
    nv->id = item->id;
    nv->disable(!item->enable);
    // иконка
    auto iv(nv->atView<zViewImage>(0));
    if(iv) iv->setImage(item->image);
    // текст/check/radio
    auto tv(nv->atView<zViewText>(1));
    if(tv) {
        int tl1(z.R.integer.iconPopup), tl2(z.R.integer.iconPopup), pos(ZS_POSITION_END);
        if(!item->isPopup()) {
            if(item->isText()) {
                pos = ZS_POSITION_NONE;
            } else {
                tl1 = item->isCheckBox() ? z.R.integer.checkOff : z.R.integer.radioOff;
                tl2 = item->isCheckBox() ? z.R.integer.checkOn : z.R.integer.radioOn;
            }
        }
        tv->setTileNum(0, tl1); tv->setTileNum(1, tl2);
        tv->setBitmapPoistion(pos);
        tv->drs[DR_IDX_FK]->tile = tl1;
        tv->drs[DR_IDX_FK]->enable = (pos != ZS_POSITION_NONE);
        tv->checked(item->checked);
        tv->setText(item->title, false);
        nv->requestLayout();
    }
    return nv;
}

*/