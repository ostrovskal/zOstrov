//
// Created by User on 25.04.2022.
//

#include "zostrov/zCommon.h"
#include "zostrov/zActionBar.h"

zMenuItem::~zMenuItem() { SAFE_DELETE(grp); }

zMenuItem* zMenuGroup::idFind(int _id) const {
    for(auto& mi : children) {
        auto grp(mi->getGroup());
        if(grp) {
            auto ret(grp->idFind(_id));
            if(ret) return ret;
        }
        if(mi->getId() == _id) return mi;
    }
    return nullptr;
}

zMenuItem* zMenuGroup::atFind(int _idx) const {
    return ((_idx >= 0 && _idx < count()) ? children[_idx] : nullptr);
}

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
                    zLinearLayout(_styles, z.R.id.actionBar, false), styles_buttons(_styles_buttons), styles_group(_styles_group) {
    content  = new zFrameLayout(styles_default, 0);
    minMaxSize.set(z_dp(z.R.dimen.actionMinWidth), 0, z_dp(z.R.dimen.actionMinHeight), 0);
}

int zActionBar::processAnimation() {
    if(isChecked()) {
        // блокировка? - ждем
        if(testLocked()) { animator.frame = 0; return 1; }
    }
    // анимируем
    float v; auto cont(animator.update(v));
    if(manager->tpView<zViewPopup>(nullptr)) { animator.frame--; return 1; }
    if(!cont) { if(isChecked()) { show(!isChecked()); return 0; } }
    lps.y = -rview.h + z_round(v * ((float)rview.h / 8.0f));
    requestPosition(); return (i32)updateStatus(ZS_VISIBLED, cont);
}

void zActionBar::notifyEvent(HANDLER_MESSAGE *msg) {
    if(msg->what == MSG_ANIM) {
        if(processAnimation())
            post(MSG_ANIM, duration);
    } else if(msg->what == MSG_DROP_CLICK) {
        auto drop(manager->getPopup()->atView<zViewDropdown>(0));
        auto item(const_cast<zMenuItem*>(&adapter->getItem(msg->arg1)));
        manager->getPopup()->dismiss();
        if(item->isEnabled()) {
            if(onClickMenuItem) onClickMenuItem(drop->atView(msg->arg1), item);
            auto grp(item->getGroup());
            if(grp) clickGroup(grpView, grp);
        }
    }
    zLinearLayout::notifyEvent(msg);
}

void zActionBar::show(bool _show) {
    if(isChecked() != _show) {
        updateStatus(ZS_CHECKED, _show);
        animator.init((float)(8 * !isChecked()), false);
        animator.add((float)(8 * isChecked()), zInterpolator::LINEAR, 8);
        if(isChecked()) animator.add(8, zInterpolator::LINEAR, 128);
        post(MSG_ANIM, duration, 0);
    }
}

zActionBar::~zActionBar() {
    manager->getPopup()->dismiss();
}

void zActionBar::changeTheme() {
    manager->getPopup()->changeTheme();
    zViewGroup::changeTheme();
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
            case menuGroupEnd:
                return;
            default:
                grp->add(new zMenuItem(_menu->id, _menu->image, theme->findString(_menu->title), flags, nullptr));
                break;
        }
        _menu++;
    }
}

zMenuGroup* zActionBar::setMenu(int iconApp, MENUITEM* menu) {
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
    return root;
}

int zActionBar::measureButton(int _id, int _image, u32 _flags, czs& _txt, int widthSize, int heightSize, zMenuGroup* grp) {
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
        grp = &currentGrp; if(adapter) adapter->setGroup(grp);
        if(onClickMenuGroup) onClickMenuGroup(view, grp);
        auto dropdown(manager->getDropdown(this, styles_group, adapter));
        dropdown->measure(menuSpec);
        manager->getPopup()->show(view->rview.x, rview.h, this, dropdown);
    } else {
        manager->getPopup()->dismiss();
        grp = nullptr;
        if(onClickMenuItem) onClickMenuItem(view, root->idFind(view->id));
    }
    cgrp = grp;
    grpView = view;
}

void zActionBar::onMeasure(cszm& spec) {
    auto widthSize(spec.w.size()), heightSize(spec.h.size());
    int wContent(z_percent(widthSize, 40));
    int width(wContent), i, count(root ? root->count() : 0);
    detach(content); overflow.reset(z.R.id.menuOverflow);
    removeAllViews(false);
    for(i = 0 ; i < count; ) {
        auto item(root->atFind(i++));
        if(item->isVisibled()) {
            if(item->isNever()) { overflow.add(item); continue; }
            auto w(measureButton(item->getId(), item->getImage(), item->getFlags(), item->getText(), widthSize, heightSize, item->getGroup()));
            width += w; if((width + w * 2) >= widthSize) break;
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
        if(adapter->unregistration(this))
            SAFE_DELETE(adapter);
    }
    if(_adapter) {
        adapter = new zAdapterMenu(_adapter);
        adapter->registration(this);
    }
    return this;
}

bool zActionBar::testLocked() const {
    if(manager->getPopup()->isVisibled()) return true;
    return zViewGroup::testLocked();
}

zView* zActionBar::zAdapterMenu::getView(int position, zView *convert, zViewGroup *parent) {
    auto item(group->atFind(position));
    auto v((zLinearLayout*)list->getView(position, convert, manager->getActionBar()));
    v->updateStatus(ZS_MEASURE, false);
    v->id = item->getId();
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
    // признак блокировки
    v->disable(!item->isEnabled());
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
