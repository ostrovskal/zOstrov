
#include "zssh/zCommon.h"
#include "zssh/zAdapter.h"
#include "zssh/zViewText.h"
#include "zssh/zViewWidgets.h"
#include "zssh/zViewRibbons.h"

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                             ОСНОВНЫЕ ФАБРИКИ                                                       //
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

zView* zAdapterFabricMenuItem::make(zViewGroup* parent) {
    auto v(new zLinearLayout(styles_default, 0, false));
    v->attach(new zViewImage(styles_z_menuimage, 0, 1), VIEW_WRAP, VIEW_MATCH);
    v->attach(new zViewCheck(styles, 0, 0), VIEW_MATCH, VIEW_MATCH);
    v->lytParams.set(0, 0, VIEW_MATCH, VIEW_WRAP);
    v->onInflate(false);
    return v;
}

zView* zAdapterFabricSpinItem::make(zViewGroup* parent) {
    auto v(new zViewText(styles, 0, 0));
    v->lytParams.set(0, 0, VIEW_MATCH, VIEW_WRAP);
    v->onInflate(false);
    return v;
}

zView* zAdapterFabricListItem::make(zViewGroup* parent) {
    auto v(new zViewText(styles, 0, 0));
    auto tbl(dynamic_cast<zViewTable*>(parent));
    int cell(100_dp);
    if(tbl) {
        i32 params[5]; tbl->getParameters(params);
        auto tmp(params[zViewTable::TABLE_CELL_SIZE]);
        if(tmp) cell = tmp;
    }
    v->lytParams.set(0, 0, cell, cell);
    v->onInflate(false);
    return v;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                              БАЗОВЫЙ АДАПТЕР                                                       //
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void zAdapter::registerOwner(zView* view) {
    if(owners.indexOf(view) == -1) {
        owners += view;
    }
}

void zAdapter::unregisterOwner(zView* view) {
    auto idx(owners.indexOf(view));
    if(idx != -1) {
        owners.erase(idx, 1, false);
    }
}

void zAdapter::notifyOwners() {
    for(auto v : owners) {
        v->notifyAdapter(this);
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                         ПРОСТОЙ АДАПТЕР СПИСКА                                                     //
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

zView *zAdapterList::getView(int position, zView *convert, zViewGroup *parent) {
    return createView(position, convert, parent, fabric, false);
}

zView *zAdapterList::getDropDownView(int position, zView *convert, zViewGroup *parent) {
    return createView(position, convert, parent, dropdown, true);
}

zView *zAdapterList::createView(int position, zView *convert, zViewGroup *parent, zAdapterFabric *_fabric, bool color) {
    auto nv(convert);
    if(!nv) nv = _fabric->make(parent);
    nv->id = position;
    auto tv(dynamic_cast<zViewText*>(nv));
    if(tv) {
        tv->setText(getItem(position), false);
        auto sv(dynamic_cast<zViewSelect*>(parent));
        if(sv && color) {
            tv->setTextForegroundColor(sv->getSelectedItem() == position ?
                                             tv->getTextHighlightColor() : tv->getTextDefaultColor());
        }
    }
    return nv;
}
