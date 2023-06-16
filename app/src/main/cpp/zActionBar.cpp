//
// Created by User on 25.04.2022.
//

#include "zssh/zCommon.h"
#include "zssh/zActionBar.h"

zMenuItem::~zMenuItem() { SAFE_DELETE(grp); }

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
        if(!item->isEnabled()) return;
        if(onClickMenuItem) onClickMenuItem(v, item);
        popup->dismiss();
        if(item->grp) clickGroup(grpView, item->grp);
    });
    setOnAnimation([this](zView*, int) {
        if(isChecked()) {
            // видима - блокировка? - ждем
            if(testLocked()) return true;
        }
        // анимируем
        float v; auto cont(animator.update(v));
        if(rview.h) lps.y = -rview.h + (int)roundf(v * ((float)rview.h / 8.0f));
        requestLayout();
        if(!cont) { if(isChecked()) show(!isChecked()); return false; }
        updateStatus(ZS_VISIBLED, cont);
        return cont;
    });
    content  = new zFrameLayout(styles_default, 0);
    popup    = new zViewPopup(styles_default, this, dropdown);
    //updateStatus(ZS_VISIBLED, false);
    minMaxSize.set(z_dp(z.R.dimen.actionMinWidth), 0, z_dp(z.R.dimen.actionMinHeight), 0);
}

zActionBar::~zActionBar() {
    resetOverflow();
    SAFE_DELETE(popup);
    SAFE_DELETE(dropdown);
    SAFE_DELETE(content);
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

void zActionBar::resetOverflow() {
/*
    for(int i = 0 ; i < overflow.count(); i++) {
        overflow.children[i]->grp = nullptr;
    }
*/
//    overflow.reset();
    overflow.children.free();
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
        if(item->isAlways()) root->children.swap(i, ii++);
    }
    return true;
}

int zActionBar::measureButton(int _id, int _image, int _flags, czs& _txt, int widthSize, int heightSize, zMenuGroup* grp) {
    auto but(new zViewImage(_id == z.R.id.menuOverflow ? styles_z_baroverflow : styles_buttons, _id, _image));
    if(_image == -1) { if(_txt.isNotEmpty()) but->setText(_txt, true); }
    if(!(_flags & menuItemEnabled)) but->disable(true);
    but->setOnClick([this, grp](zView* v, int) { clickGroup(v, grp); });
    attach(but, VIEW_WRAP, VIEW_WRAP);
    auto ws(makeChildMeasureSpec(zMeasure(MEASURE_MOST, widthSize), but->pad.extent(false), VIEW_WRAP));
    auto hs(makeChildMeasureSpec(zMeasure(MEASURE_MOST, heightSize), but->pad.extent(true), VIEW_WRAP));
    but->measure( { ws, hs } );
    return but->rview.w;
}

void zActionBar::clickGroup(zView *view, zMenuGroup *grp) {
    if(grp && grp->isNotEmpty()) {
        if(adapter) adapter->setGroup(grp);
        dropdown->measure( { zMeasure(MEASURE_UNDEF, 0), zMeasure(MEASURE_UNDEF, 0) } );
        popup->show(pti(view->rview.x, rview.h));
        if(onClickMenuGroup) onClickMenuGroup(view, grp);
    } else {
        popup->dismiss();
        if(onClickMenuItem) onClickMenuItem(view, root->idFind(view->id));
    }
    grpView = view;
}

void zActionBar::onMeasure(cszm& spec) {
    auto widthSize(spec.w.size()), heightSize(spec.h.size());
    int wContent(z_percent(widthSize, 33));
    int width(wContent), i, count(root ? root->count() : 0);
    detach(content); resetOverflow(); removeAllViews(false);
    for(i = 0 ; i < count; ) {
        auto item(root->atFind(i++)); auto grp(item->grp);
        if(!item->isVisibled()) continue;
        if(item->isNever()) {
            if(grp && !grp->isNotEmpty()) grp = nullptr;
            overflow.add(new zMenuItem(item->id, item->img, item->txt, item->flags, grp));
        } else {
            auto w(measureButton(item->id, item->img, item->flags, item->txt, widthSize, heightSize, grp));
            width += w; if((width + w * 2) >= widthSize) break;
        }
    }
    // overflow
    for(; i < count; i++) {
        auto item(root->atFind(i)); auto grp(item->grp);
        if(!item->isVisibled()) continue;
        if(grp && !grp->isNotEmpty()) grp = nullptr;
        overflow.add(item);
        //overflow.add(new zMenuItem(item->id, item->img, item->txt, item->flags, grp));
    }
    if(overflow.isNotEmpty() || count == 0) {
        width += measureButton(z.R.id.menuOverflow, z.R.integer.iconMenu, menuItemEnabled, "", widthSize, heightSize, &overflow);
    }
    // контент
    wContent += z_max(0, widthSize - width);
    attach(content, wContent, VIEW_MATCH, count != 0);
    zLinearLayout::onMeasure(spec);
}

void zActionBar::show(bool _show) {
    if(isChecked() != _show) {
        updateStatus(ZS_CHECKED, _show);
        updateStatus(ZS_VISIBLED, true);
        delay = 0;
        animator.init(8 * !isChecked(), false);
        animator.add(8 * isChecked(), zInterpolator::LINEAR, 4);
        if(isChecked()) animator.add(8 * isChecked(), zInterpolator::LINEAR, 128);
        post(MSG_ANIM, duration, 0);
    }
}

zActionBar* zActionBar::setAdapter(zAdapterList* _adapter) {
    if(adapter) {
        adapter->unregistration(this);
        SAFE_DELETE(adapter);
    }
    if(_adapter) {
        adapter = new zAdapterMenu(this, _adapter);
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
    auto nv((zLinearLayout*)list->getView(position, convert, bar));
    auto item(group->atFind(position));
    nv->id = item->id;
    nv->disable(!item->isEnabled());
//    if(!nv->updateStatus(ZS_VISIBLED, item->isVisibled())) return nv;
    // иконка
    auto iv(nv->atView<zViewImage>(0));
    if(iv) iv->setImage(item->img);
    // текст/check/radio
    auto tv(nv->atView<zViewText>(1));
    if(tv) {
        int tl1(z.R.integer.iconGroup), tl2(z.R.integer.iconGroup), _gravity(ZS_GRAVITY_END | ZS_GRAVITY_VCENTER);
        if(!item->isGroup()) {
            if(item->isText()) _gravity = ZS_GRAVITY_CENTER;
            else {
                tl1 = item->isCheckBox() ? z.R.integer.checkOff : z.R.integer.radioOff;
                tl2 = item->isCheckBox() ? z.R.integer.checkOn : z.R.integer.radioOn;
            }
        }
        // тайлы
        tv->drw[DRW_FK]->tiles = (tl1 | (tl2 >> 8));
        // гравитация
        tv->setForegroundGravity(_gravity);
        // показывать ли?
        tv->drw[DRW_FK]->visible = (_gravity != ZS_GRAVITY_CENTER);
        // признак выделения
        tv->checked(item->isChecked());
        // установить текст
        tv->setText(item->getText(), false);
        nv->requestLayout();
    }
    return nv;
}
