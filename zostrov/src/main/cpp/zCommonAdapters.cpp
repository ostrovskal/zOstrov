
#include "zostrov/zCommon.h"
#include "zostrov/zCommonAdapters.h"
#include "zostrov/zViewText.h"
#include "zostrov/zViewWidgets.h"
#include "zostrov/zViewRibbons.h"

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                             ОСНОВНЫЕ ФАБРИКИ                                                       //
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

zView* zFabricLayoutItem::make(zViewGroup* parent) {
    auto v(new zLinearLayout(styles_default, 0, false));
    for(int i = 0; i < count; i++) v->attach(new zViewText(styles, 0, 0), VIEW_MATCH, VIEW_MATCH);
    v->lps.set(0, 0, VIEW_MATCH, VIEW_WRAP);
    v->onInit(false);
    return v;
}

zView* zFabricMenuItem::make(zViewGroup* parent) {
    auto v(new zLinearLayout(styles_default, 0, false));
    v->attach(new zViewImage(styles_z_menuimage, 0, -1), VIEW_WRAP, VIEW_MATCH);
    v->attach(new zViewCheck(styles, 0, 0), VIEW_MATCH, VIEW_MATCH);
    v->lps.set(0, 0, VIEW_MATCH, VIEW_WRAP);
    v->onInit(false);
    return v;
}

zView* zFabricSelectItem::make(zViewGroup* parent) {
    auto v(new zViewButton(styles, 0, 0));
    v->lps.set(0, 0, VIEW_MATCH, VIEW_WRAP);
    v->onInit(false);
    return v;
}

zView* zFabricListItem::make(zViewGroup* parent) {
    int w(VIEW_MATCH), h(VIEW_WRAP);
    if(!parent->isVertical()) std::swap(w, h);
    auto v(new zViewText(styles, 0, 0));
    auto tbl(dynamic_cast<zViewGrid*>(parent));
    if(tbl) {
        auto tmp(tbl->getParameters(zViewGrid::GRID_CELL_SIZE));
        if(tmp) w = tmp, h = tmp;
    }
    v->lps.set(0, 0, w, h);
    v->onInit(false);
    return v;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                              БАЗОВЫЙ АДАПТЕР                                                       //
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void zBaseAdapter::registration(zView* view) {
    if(owners.indexOf(view) == -1) owners += view;
}

bool zBaseAdapter::unregistration(zView* view) {
    owners.erase(owners.indexOf(view), 1, false);
    return owners.isEmpty();
}

void zBaseAdapter::notify() {
    for(auto v : owners) v->notifyAdapter(this);
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                         ПРОСТОЙ АДАПТЕР СПИСКА                                                     //
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

zView *zAdapterList::getView(int position, zView *convert, zViewGroup *parent) {
    return createView(position, convert, parent, fabricBase, false);
}

zView *zAdapterList::getDropDownView(int position, zView *convert, zViewGroup *parent) {
    return createView(position, convert, parent, fabricDropdown, true);
}

zView *zAdapterList::createView(int position, zView *convert, zViewGroup *parent, zBaseFabric *_fabric, bool color) {
    auto nv(convert);
    if(!nv) nv = _fabric->make(parent);
    nv->id = position; nv->updateStatus(ZS_TAP, false);
    auto tv(dynamic_cast<zViewText*>(nv));
    if(tv) {
        auto but(dynamic_cast<zViewButton*>(nv));
        zString8 txt(getItem(position)); int ic(-1);
        if(but) {
            auto icon(tv->drw[DRW_ICON]);
            if(icon->isValid()) ic = icon->texture->getTile(txt.substrAfter("\b"));
            txt = (ic != -1 ? "" : std::move(txt.substrBefore("\b", txt)));
        }
        // поставим иконку/текст
        if(but) but->setIcon(ic);
        tv->setText(txt);
        auto sv(dynamic_cast<zViewSelect*>(parent));
        if(sv && color) tv->setTextColorForeground(sv->getItemSelected() == position ? tv->getTextColorHighlight() : tv->getTextColorDefault());
    }
    return nv;
}
