
#include "zssh/zCommon.h"
#include "zssh/zCommonAdapters.h"
#include "zssh/zViewText.h"
#include "zssh/zViewWidgets.h"
#include "zssh/zViewRibbons.h"

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                             ОСНОВНЫЕ ФАБРИКИ                                                       //
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

zView* zFabricMenuItem::make(zViewGroup* parent) {
    auto v(new zLinearLayout(styles_default, 0, false));
    v->attach(new zViewImage(styles_z_menuimage, 0, 1), VIEW_WRAP, VIEW_MATCH);
    v->attach(new zViewCheck(styles, 0, 0), VIEW_MATCH, VIEW_MATCH);
    v->lps.set(0, 0, VIEW_MATCH, VIEW_WRAP);
    v->onInit(false);
    return v;
}

zView* zFabricSelectItem::make(zViewGroup* parent) {
    auto v(new zViewText(styles, 0, 0));
    v->lps.set(0, 0, VIEW_MATCH, VIEW_WRAP);
    v->onInit(false);
    return v;
}

zView* zFabricListItem::make(zViewGroup* parent) {
    int cell(100_dp);
    auto v(new zViewText(styles, 0, 0));
/*
    auto tbl(dynamic_cast<zViewGrid*>(parent));
    if(tbl) {
        i32 params[5]; tbl->getParameters(params);
        auto tmp(params[zViewGrid::TABLE_CELL_SIZE]);
        if(tmp) cell = tmp;
    }
*/
    v->lps.set(0, 0, cell, cell);
    v->onInit(false);
    return v;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                              БАЗОВЫЙ АДАПТЕР                                                       //
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void zBaseAdapter::registration(zView* view) {
    if(owners.indexOf(view) == -1) owners += view;
}

void zBaseAdapter::unregistration(zView* view) {
    owners.erase(owners.indexOf(view), 1, false);
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
    nv->id = position;
/*
    auto tv(dynamic_cast<zViewText*>(nv));
    if(tv) {
        tv->setText(getItem(position), false);
        auto sv(dynamic_cast<zViewSelect*>(parent));
        if(sv && color) tv->setTextColorForeground(sv->getSelectedItem() == position ? tv->getTextColorHighlight() : tv->getTextColorDefault());
    }
*/
    return nv;
}
