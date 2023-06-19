//
// Created by User on 25.04.2022.
//

#include "zssh/zCommon.h"
#include "zssh/zActionBar.h"

zMenuItem::~zMenuItem() { SAFE_DELETE(grp); }

void zMenuItem::setChecked(bool _set) {
    if(isCheckable()) {
        if(isRadioButton() && _set) manager->getActionBar()->resetRadio();
        updateStatus(menuItemChecked, _set);
    }
}

void zMenuItem::setText(czs& _txt) {
    txt = _txt;
    manager->getActionBar()->requestLayout();
}

void zMenuItem::setImage(int _img) {
    img = _img;
    manager->getActionBar()->requestLayout();
}

void zMenuItem::updateStatus(int status, bool _set) {
    flags &= ~status; flags |= (status * _set);
    manager->getActionBar()->requestLayout();
}

zActionBar::zActionBar(zStyle* _styles, zStyle* _styles_buttons, zStyle* _styles_group) :
                    zLinearLayout(_styles, z.R.id.actionBar, false), styles_buttons(_styles_buttons) {
    dropdown = new zViewDropdown(_styles_group);
    dropdown->setOnClick([this](zView* v, int sel) {
        auto item(const_cast<zMenuItem*>(&adapter->getItem(sel)));
        if(item->isEnabled()) {
            if(onClickMenuItem) onClickMenuItem(v, item);
            popup->dismiss();
            auto grp(item->getGroup()); if(grp) clickGroup(grpView, grp);
        }
    });
    setOnAnimation([this](zView*, int) {
        if(isChecked()) {
            // блокировка? - ждем
            if(testLocked()) { animator.frame = 0; return 1; }
        }
        // анимируем
        float v; auto cont(animator.update(v));
        if(manager->tpView<zViewPopup>(nullptr)) { animator.frame--; return 1; }
        if(!cont) { if(isChecked()) { show(!isChecked()); return 0; } }
        lps.y = -rview.h + (int)roundf(v * ((float)rview.h / 8.0f));
        requestPosition(); return updateVisible(cont);
    });
    content  = new zFrameLayout(styles_default, 0);
    popup    = new zViewPopup(styles_default, this, dropdown);
    minMaxSize.set(z_dp(z.R.dimen.actionMinWidth), 0, z_dp(z.R.dimen.actionMinHeight), 0);
}

void zActionBar::show(bool _show) {
    if(isChecked() != _show) {
        updateStatus(ZS_CHECKED, _show);
        animator.init(8 * !isChecked(), false);
        animator.add(8 * isChecked(), zInterpolator::LINEAR, 8);
        if(isChecked()) animator.add(8, zInterpolator::LINEAR, 128);
        post(MSG_ANIM, duration, 0);
    }
}

zActionBar::~zActionBar() {
    SAFE_DELETE(popup);
    if(adapter) {
        adapter->unregistration(this);
        SAFE_DELETE(adapter);
    }
}

void zActionBar::setContent(zView* _view) {
    if(content) {
        content->detachAllViews(false);
        content->attach(_view, VIEW_MATCH, VIEW_MATCH);
    }
}

void zActionBar::recursiveMenu(zMenuGroup* grp) {
    zMenuGroup* _grp;
    while(_menu->type != menuEnd) {
        auto flags(_menu->action | menuItemEnabled | menuItemVisibled);
        switch(_menu->type) {
            case menuGroupBegin:
                grp->add(new zMenuItem(_menu->id, _menu->image, theme->findString(_menu->title), flags | menuItemGroup, _grp = new zMenuGroup(_menu->id)));
                _menu++; recursiveMenu(_grp);
                break;
            case menuGroupEnd: return;
            default:
                grp->add(new zMenuItem(_menu->id, _menu->image, theme->findString(_menu->title), flags, nullptr));
                break;
        }
        _menu++;
    }
}

bool zActionBar::setMenu(int iconApp, MENUITEM* menu) {
    delete root; _menu = menu; int ii(1);
    root = new zMenuGroup();
    // изображение программы
    root->add(new zMenuItem(z.R.id.menuApp, iconApp, theme->findString(z.R.string.app_name), actAlways | menuItemEnabled | menuItemVisibled, nullptr));
    recursiveMenu(root);
    // отсортировать первый уровень по actAlways
    for(int i = 1 ; i < root->count(); i++) {
        auto item(root->atFind(i));
        if(item->isAlways()) root->swap(i, ii++);
    }
    return true;
}

int zActionBar::measureButton(int _id, int _image, int _flags, czs& _txt, int widthSize, int heightSize, zMenuGroup* grp) {
    auto but(new zViewImage(_id == z.R.id.menuOverflow ? styles_z_baroverflow : styles_buttons, _id, _image));
    if(_image == -1) { if(_txt.isNotEmpty()) but->setText(_txt, true); }
    but->disable(!(_flags & menuItemEnabled));
    but->tag = TAG{(char*)grp};
    but->setOnClick([this, grp](zView* v, int) { clickGroup(v, grp); });
    attach(but, VIEW_WRAP, VIEW_WRAP);
    auto ws(makeChildMeasureSpec(zMeasure(MEASURE_MOST, widthSize), but->pad.extent(false), VIEW_WRAP));
    auto hs(makeChildMeasureSpec(zMeasure(MEASURE_MOST, heightSize), but->pad.extent(true), VIEW_WRAP));
    but->measure( { ws, hs } );
    return but->rview.w;
}

void zActionBar::clickGroup(zView *view, zMenuGroup *grp) {
    if(grp && grp->isNotEmpty()) {
        currentGrp.reset(grp->getId());
        for(auto& i : *grp) { if(i->isVisibled()) currentGrp.add(i); }
        grp = &currentGrp;
        if(adapter) adapter->setGroup(grp);
        dropdown->measure( { zMeasure(MEASURE_UNDEF, 0), zMeasure(MEASURE_UNDEF, 0) } );
        popup->show(pti(view->rview.x, rview.h));
        if(onClickMenuGroup) onClickMenuGroup(view, grp);
    } else {
        popup->dismiss(); grp = nullptr;
        if(onClickMenuItem) onClickMenuItem(view, root->idFind(view->id));
    }
    cgrp = grp;
    grpView = view;
}

void zActionBar::onMeasure(cszm& spec) {
    auto widthSize(spec.w.size()), heightSize(spec.h.size());
    int wContent(z_percent(widthSize, 40));
    int width(wContent), i, count(root ? root->count() : 0);
    detach(content); overflow.reset(z.R.id.menuOverflow); removeAllViews(false);
    for(i = 0 ; i < count; ) {
        auto item(root->atFind(i++));
        if(item->isVisibled()) {
            if(item->isNever()) overflow.add(item);
            else {
                auto w(measureButton(item->getId(), item->getImage(), item->getFlags(), item->getText(), widthSize, heightSize, item->getGroup()));
                width += w; if((width + w * 2) >= widthSize) break;
            }
        }
    }
    // overflow
    for(; i < count; i++) {
        auto item(root->atFind(i));
        if(item->isVisibled()) overflow.add(item);
    }
    if(overflow.isNotEmpty() || count == 0) {
        width += measureButton(z.R.id.menuOverflow, z.R.integer.iconMenu, menuItemEnabled, "", widthSize, heightSize, &overflow);
    }
    // контент
    wContent += z_max(0, widthSize - width);
    attach(content, wContent, VIEW_MATCH, count != 0);
    zLinearLayout::onMeasure(spec);
}

zActionBar* zActionBar::setAdapter(zAdapterList* _adapter) {
    if(adapter) {
        adapter->unregistration(this);
        SAFE_DELETE(adapter);
    }
    if(_adapter) {
        adapter = new zAdapterMenu(_adapter);
        dropdown->setAdapter(adapter);
        adapter->registration(this);
    }
    return this;
}

bool zActionBar::testLocked() const {
    if(popup->isVisibled()) return true;
    return zViewGroup::testLocked();
}

zView* zActionBar::zAdapterMenu::getView(int position, zView *convert, zViewGroup *parent) {
    auto item(group->atFind(position));
    auto v((zLinearLayout*)list->getView(position, convert, manager->getActionBar()));
    v->id = item->getId(); v->disable(!item->isEnabled());
    // изображение
    auto iv(v->atView<zViewImage>(0));
    iv->setImage(item->getImage());
    // текст/check/radio
    auto tv(v->atView<zViewText>(1));
    tv->setText(item->getText(), false);
    int tl1(z.R.integer.iconGroup), tl2(z.R.integer.iconGroup), grav(ZS_GRAVITY_END | ZS_GRAVITY_VCENTER);
    if(!item->isGroup()) {
        if(item->isText()) {
            grav = ZS_GRAVITY_CENTER;
        } else {
            // тайлы
            tl1 = item->isCheckBox() ? z.R.integer.checkOff : z.R.integer.radioOff;
            tl2 = item->isCheckBox() ? z.R.integer.checkOn : z.R.integer.radioOn;
        }
    }
    // гравитация
    tv->setForegroundGravity(grav);
    // тайлы
    tv->drw[DRW_FK]->tiles = (tl1 | (tl2 << 8));
    // признак
    tv->checked(item->isChecked());
    // показывать ли?
    tv->drw[DRW_FK]->visible = grav != ZS_GRAVITY_CENTER;
    v->requestLayout();
    return v;
}

// сброс переключателей
void zActionBar::resetRadio() {
    if(cgrp) {
        for(auto& i : *cgrp) {
            if(i->isRadioButton()) i->setChecked(false);
        }
    }
}

void zActionBar::_stateView(zMenuGroup* grp, zView::STATE &state, bool save, int &index) {
    if(save) {
        state.data += grp->getId();
    } else {
        auto _id(state.data[index++]);
        if(grp->getId() != _id) return;
    }
    for(auto& i : *grp) {
        if(save) {
            state.data += i->getFlags();
            state.data += i->getImage();
        } else {
            i->setFlags(state.data[index++]);
            i->setImage(state.data[index++]);
        }
        if(i->getGroup()) _stateView(i->getGroup(), state, save, index);
    }
}
