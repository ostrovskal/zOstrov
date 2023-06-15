//
// Created by User on 25.04.2022.
//

#include "zssh/zCommon.h"
#include "zssh/zActionBar.h"

zMenuItem::~zMenuItem() { SAFE_DELETE(grp); }

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
    updateStatus(ZS_VISIBLED, false);
    minMaxSize.set(z_dp(z.R.dimen.actionMinWidth), 0, z_dp(z.R.dimen.actionMinHeight), 0);
}

zActionBar::~zActionBar() {
    resetOverflow();
    manager->getSystemView(false)->detach(popup);
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
    for(int i = 0 ; i < overflow.count(); i++) {
        overflow.children[i]->grp = nullptr;
    }
    overflow.reset();
}

void zActionBar::recursiveMenu(zMenuGroup* grp) {
    zMenuGroup* _grp;
    while(_menu->type != menuEnd) {
        switch(_menu->type) {
            case menuGroupBegin:
                grp->add(new zMenuItem(_menu->id, _menu->image, theme->findString(_menu->title), _menu->action | menuItemGroup, _grp = new zMenuGroup(_menu->id)));
                _menu++; recursiveMenu(_grp);
                break;
            case menuGroupEnd: return;
            default:
                grp->add(new zMenuItem(_menu->id, _menu->image, theme->findString(_menu->title), _menu->action, nullptr));
                break;
        }
        _menu++;
    }
}

bool zActionBar::setMenu(int iconApp, MENUITEM* menu) {
    delete root; _menu = menu; int ii(1);
    root = new zMenuGroup();
    // изображение программы
    root->add(new zMenuItem(z.R.id.menuApp, iconApp, "", actAlways, nullptr));
    recursiveMenu(root);
    // отсортировать первый уровень по actAlways
    for(int i = 1 ; i < root->count(); i++) {
        auto item(root->atFind(i));
        if(item->isAlways()) root->children.swap(i, ii++);
    }
    return true;
}

int zActionBar::measureButton(int _id, int _image, int widthSize, int heightSize, zMenuGroup* grp, bool isMeasure) {
    int width(0);
    auto but(new zViewImage(_id == z.R.id.menuOverflow ? styles_z_baroverflow : styles_buttons, _id, _image));
    but->setTag(TAG{(char*)grp});
    but->setOnClick([this](zView* v, int) {
        auto grp((zMenuGroup*)v->tag.str);
        clickGroup(v, grp);
    });
    attach(but, VIEW_WRAP, VIEW_WRAP);
    if(isMeasure) {
        auto ws(makeChildMeasureSpec(zMeasure(MEASURE_MOST, widthSize), but->pad.extent(false), VIEW_WRAP));
        auto hs(makeChildMeasureSpec(zMeasure(MEASURE_MOST, heightSize), but->pad.extent(true), VIEW_WRAP));
        but->measure( { ws, hs } ); width = but->rview.w;
    }
    return width;
}

void zActionBar::clickGroup(zView *view, zMenuGroup *grp) {
    if(grp && grp->isNotEmpty()) {
        if(adapter) adapter->setGroup(grp);
        dropdown->measure( { zMeasure(MEASURE_UNDEF, 0), zMeasure(MEASURE_UNDEF, 0) } );
        popup->show(pti(view->rview.x, rview.h));
        if(onClickMenuGroup) onClickMenuGroup(this, grp);
    } else popup->dismiss();
    grpView = view;
}

void zActionBar::onMeasure(cszm& spec) {
    auto widthSize(spec.w.size()), heightSize(spec.h.size());
    int wContent(z_percent(widthSize, 33)), wBut(0);
    int width(wContent), i, count(root ? root->count() : 0);
    detach(content); resetOverflow(); removeAllViews(false);
    for(i = 0 ; i < count; ) {
        auto item(root->atFind(i++)); auto grp(item->grp);
        if(!item->isVisibled()) continue;
        if(!item->isNever()) {
            auto w(measureButton(item->id, item->img, widthSize, heightSize, grp, wBut == 0));
            if(w) wBut = w;
            width += wBut;
            if((width + wBut * 2) >= widthSize) break;
        } else {
            if(grp && !grp->isNotEmpty()) grp = nullptr;
            overflow.add(new zMenuItem(item->id, item->img, item->txt, item->flags, grp));
        }
    }
    // overflow
    for(; i < count; i++) {
        auto item(root->atFind(i)); auto grp(item->grp);
        if(grp && !grp->isNotEmpty()) grp = nullptr;
        overflow.add(new zMenuItem(item->id, item->img, item->txt, item->flags, grp));
    }
    if(overflow.isNotEmpty() || count == 0) {
        width += measureButton(z.R.id.menuOverflow, z.R.integer.iconMenu, widthSize, heightSize, &overflow, wBut == 0);
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
    // иконка
    auto iv(nv->atView<zViewImage>(0));
    if(iv) iv->setImage(item->img);
    // текст/check/radio
    auto tv(nv->atView<zViewText>(1));
    if(tv) {
        int tl1(z.R.integer.iconGroup), tl2(z.R.integer.iconGroup), _gravity(ZS_GRAVITY_END);
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
