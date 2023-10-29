//
// Created by User on 25.04.2022.
//

#include "zostrov/zCommon.h"
#include "zostrov/zCommonLayout.h"

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                                         FRAME LAYOUT                                                                   //
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void zFrameLayout::onMeasure(cszm& spec) {
    defaultOnMeasure(spec, measureChildren(spec));
}

void zFrameLayout::onLayout(crti &position, bool changed) {
    zView::onLayout(position, changed);
    for(auto& child : children) {
        if(child->isVisibled()) {
            // применить выравнивание
            auto width(child->rview.w), height(child->rview.h);
            auto pt(applyGravity(rclient, rti(0, 0, width + child->margin.extent(false), height + child->margin.extent(true)), child->lps.gravity));
            child->layout(rti(rclient.x + pt.x, rclient.y + pt.y, width, height));
        }
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                                        ABSOLUTE LAYOUT                                                                 //
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void zAbsoluteLayout::onMeasure(cszm& spec) {
    auto width(spec.w.size()), height(spec.h.size());
    // сначала расчитать габариты дочерних
    measureChildren(spec);
    if(spec.w.isNotExact() || spec.h.isNotExact()) {
        int maxHeight(0), maxWidth(0);
        // найти самый правый и самый нижний
        for(auto &child : children) {
            if(child->isVisibled()) {
                auto lps(&child->lps);
                maxWidth  = z_max(maxWidth, lps->x + child->sizes(false));
                maxHeight = z_max(maxHeight, lps->y + child->sizes(true));
            }
        }
        if(spec.w.isNotExact()) {
            maxWidth  += padMargin(false);
            maxWidth  = z_max(maxWidth, minMaxSize.x);
            width = maxWidth;
        }
        if(spec.h.isNotExact()) {
            maxHeight += padMargin(true);
            maxHeight = z_max(maxHeight, minMaxSize.y);
            height = maxHeight;
        }
    }
    setMeasuredDimension(width, height);
}

void zAbsoluteLayout::onLayout(crti &position, bool changed) {
    zView::onLayout(position, changed);
    for(auto& child : children) {
        if(child->isVisibled()) {
            auto lps(&child->lps);
            child->layout(rti(rclient.x + lps->x, rclient.y + lps->y, rclient.w, rclient.h));
        }
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                                        LINEAR LAYOUT                                                                   //
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void zLinearLayout::onMeasure(cszm& spec) {
    int sizeMatch(0), allWeight(0), marginsChild(0), countMatch(0); bool isWeights(false);
    szm cur(spec); szi _max, _wh;  szm childSpec;
    auto divSize(div ? div->resolve(countChildren(), true) : 0);
    _max[vert] = divSize + padMargin(vert);
    // определяем габариты и веса дочерних
    auto _undef(spec[vert].mode() == MEASURE_UNDEF);
    for(auto child : children) {
        if(child->isVisibled()) {
            // весь вес
            auto weight(child->lps.weight);
            isWeights |= (weight != 0);
            allWeight += z_max(1, weight);
            measureChild(child, cur);
            marginsChild += child->margin.extent(vert);
            // определение количества дочерних с VIEW_MATCH
            if(_undef || child->lps[vert + 2] != VIEW_MATCH) _max[vert] += child->sizes(vert); else countMatch++;
            _max[!vert] = z_max(_max[!vert], child->sizes(!vert));
            if(!_undef && !isWeights) cur[vert].setSize(z_max(0, cur[vert] - child->sizes(vert)));
        }
    }
    // определение габаритов для дочерних с VIEW_MATCH
    if(countMatch) {
        sizeMatch = (spec[vert].size() - _max[vert]) / countMatch;
        for(auto child : children) {
            if(child->isVisibled() && child->lps[vert + 2] == VIEW_MATCH)
                _max[vert] += sizeMatch;
        }
    }
    if(spec[vert].isExact()) _max[vert] = spec[vert].size() - padMargin(vert);
    _wh = _max; _wh[vert] -= (marginsChild + divSize);
    // снова определяем размеры дочерних с учетом веса
    for(auto child: children) {
        if(child->isVisibled()) {
            auto& lps(child->lps);
            if(lps[vert + 2] == VIEW_MATCH || isWeights) {
                auto weight(z_max<float>(1.0f, (float)child->lps.weight));
                if(isWeights) weight /= (float)allWeight;
                auto _size(lps[vert + 2] == VIEW_MATCH ? (sizeMatch - child->margin.extent(vert)) : (int)lps[vert + 2]);
                auto size(isWeights ? z_round((float)_wh[vert] * weight) : _size), wh((int)lps[3 - vert]);
                childSpec.set(makeChildMeasureSpec(zMeasure(MEASURE_EXACT, _wh.w), child->margin.extent(false), vert ? wh : size),
                              makeChildMeasureSpec(zMeasure(MEASURE_EXACT, _wh.h), child->margin.extent(true), vert ? size : wh));
                child->measure(childSpec);
            }
            if(!isWeights) _wh[vert] -= child->sizes(vert);
        }
    }
    defaultOnMeasure(spec, _max);
    // разделитель
    if(div) div->measure(0, 0, PIVOT_ALL | PIVOT_W | PIVOT_H, false);
}

pti zLinearLayout::applyGravity(crti& sizeParent, crti& sizeChild, u32 _gravity) {
    auto ret(zView::applyGravity(sizeParent, sizeChild, _gravity));
    ret[vert] = 0;
    return ret;
}

void zLinearLayout::onLayout(crti &position, bool changed) {
    zView::onLayout(position, changed);
    auto r(rclient); int sizeChildren(0);
    // размер разделителя
    auto divSize(div ? div->getSize(ZS_DIVIDER_MIDDLE) : 0);
    // начальная координата с учетом разделителя
    r[vert] += (div ? div->getSize(ZS_DIVIDER_BEGIN) : 0);
    // подсчитать размер всех дочерних вместе с разделителем
    for(auto& child : children) { if(child->isVisibled()) sizeChildren += child->sizes(vert); }
    sizeChildren += (div ? div->resolve(countChildren(), true) : 0);
    // выровнять все дочерние по верт/гор
    r[vert] += zView::applyGravity(r.size(), szi(sizeChildren, sizeChildren), gravity)[vert];
    // позиционировать дочерние
    for(auto& child : children) {
        if(child->isVisibled()) { child->layout(r); r[vert] += child->sizes(vert) + divSize; }
    }
}

void zLinearLayout::onDraw() {
    zViewGroup::onDraw();
    // отрисовка разделителя
    if(div) div->make(0, countChildren(), 0);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                                        CELL LAYOUT                                                                     //
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static int z_clamp(int v, int mx) {
    return v > mx ? mx : (v < 0 ? 0 : v);
}

zCellLayout::zCellLayout(zStyle* _styles, i32 _id, cszi& _cell, i32 _space) : zViewGroup(_styles, _id) {
    cells = _cell; space = _space; cellDebug = cells;
}

static void correctSize(zLayoutParams* lps, int cols, int rows) {
    lps->x = z_clamp(lps->x, cols - 1);
    lps->y = z_clamp(lps->y, rows - 1);
    if((lps->x + lps->w) > cols) lps->w = cols - lps->x;
    if((lps->y + lps->h) > rows) lps->h = rows - lps->y;
}

void zCellLayout::onMeasure(cszm& spec) {
    auto width(spec.w.size()), height(spec.h.size());
    auto wpad(pad.extent(false)), hpad(pad.extent(true));
    // 1. пространство между ячейками/количество ячеек
    auto spc(space * 2), cols(z_max(1, cells.w)), rows(z_max(1, cells.h));
    // 2. габариты ячеек
    auto hCell((float)(height - hpad) / (float)rows); if(hCell < Z_EPSILON) hCell = (float)48_dp;
    auto wCell((float)(width - wpad)  / (float)cols); if(wCell < Z_EPSILON) wCell = (float)48_dp;
    if(cells.w <= 0) wCell = hCell; else if(cells.h <= 0) hCell = wCell;
    cell.set(wCell, hCell);
    // 3. определяем габариты макета
    if(!spec.h.isExact()) {
        height = z_round((float)hpad + hCell * (float)rows);
        if(spec.h.mode() == MEASURE_MOST) height = z_min(spec.h.size(), height);
    }
    if(!spec.w.isExact()) {
        width = z_round((float)wpad + wCell * (float)cols);
        if(spec.w.mode() == MEASURE_MOST) width = z_min(spec.w.size(), width);
    }
    // 4. корректируем количество ячеек
    rows = z_round((float)(height - hpad) / hCell); height = z_round((float)hpad + hCell * (float)rows);
    cols = z_round((float)(width  - wpad) / wCell); width  = z_round((float)wpad + wCell * (float)cols);
    cellDebug.set(cols, rows);
    // 5. ставим фактические размеры
    for(auto &child: children) {
        if(child->isVisibled()) {
            // параметры макета
            auto lps(&child->lps);
            // корректируем габариты
            correctSize(lps, cols, rows);
            // определяем габариты дочернего
            szm childSpec(zMeasure(MEASURE_EXACT, (int)((float)lps->w * wCell - (float)spc)),
                          zMeasure(MEASURE_EXACT, (int)((float)lps->h * hCell - (float)spc)));
            child->measure(childSpec);
        }
    }
//    DLOG("wc:%f hc:%f", wCell, hCell);
//    DLOG("w:%i h:%i", width, height);
//    DLOG("col:%i row:%i", cols, rows);
    setMeasuredDimension(width, height);
}

void zCellLayout::onLayout(crti &position, bool changed) {
    zViewGroup::onLayout(position, changed); rti r;
    for(auto& child : children) {
        if(child->isVisibled()) {
            auto lps(&child->lps);
            r.x = (rclient.x + (int)((float)lps->x * cell.w + (float)space));
            r.y = (rclient.y + (int)((float)lps->y * cell.h + (float)space));
            r.w = child->rview.w; r.h = child->rview.h;
            child->layout(r);
        }
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                                        SCROLL LAYOUT                                                                   //
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void zScrollLayout::onMeasure(cszm& spec) {
    szi size;
    if(countChildren()) {
        auto child(atView(0));
        auto sw(spec.w.size()), sh(spec.h.size());
        int wmms, hmms;
        if(atView<zScrollLayout>(0)) {
            wmms = vert * spec.w;
            hmms = !vert * spec.h;
        } else {
            if(vert) {
                wmms = zMeasure((sw != 0) * spec.w.mode(), sw);
                hmms = zMeasure(MEASURE_UNDEF, sh);
            } else {
                wmms = zMeasure(MEASURE_UNDEF, sw);
                hmms = zMeasure((sh != 0) * spec.h.mode(), sh);
            }
        }
        measureChild(child, {wmms, hmms});
        size.set(child->sizes(false), child->sizes(true));
    }
    defaultOnMeasure(spec, size);
    childSize = z_max(size[vert], rclient[vert + 2]);
}

pti zScrollLayout::applyGravity(crti& sizeParent, crti& sizeChild, u32 _gravity) {
    auto ret(zView::applyGravity(sizeParent, sizeChild, _gravity));
    if(sizeChild[vert + 2] > sizeParent[vert + 2]) ret[vert] = 0;
    return ret;
}

void zScrollLayout::onLayout(crti &position, bool changed) {
    zView::onLayout(position, changed);
    auto child(atView(0));
    // если он сам сдвинут
    if(child) child->layout(rclient - scroll);
}

bool zScrollLayout::touchChildren() {
    // проверить - был ли драг?
    bool ret((z_round(touch->cpt[vert] - touch->bpt[vert]) / sizeTouch[vert]) == 0);
    return ret;
}

zView *zScrollLayout::attach(zView *v, int width, int height, int where) {
    if(!countChildren()) return zViewGroup::attach(v, width, height, where);
    return nullptr;
}

i32 zScrollLayout::onTouchEvent() {
    zViewGroup::onTouchEvent();
    bool drag(false);
    touch->drag(sizeTouch, [this, &drag](cszi& offs, bool event) {
        // если сдвинули - определяем дельту в зависимости от ориентации
        auto _delta(offs[vert] * sizeTouch[vert]);
        if(_delta) {
            flyng->stop();
            // определяем время сдвига
            if(!(event && flyng->start(touch, _delta))) {
                // иначе - просто скроллим на дельту
                scrolling(_delta);
            }
            drag = true;
        }
    });
    return drag | touch->isCaptured();
}

bool zScrollLayout::scrolling(int _delta) {
    auto child(atView(0));
    if(child) {
        auto vEnd(z_min(manager->getKeyboard()->lps.y, rclient[vert + 2]));
        auto delta(child->scroll[vert]);
        auto delta1(delta); delta -= _delta;
        if(_delta < 0 && (delta + vEnd) >= childSize) delta = childSize - vEnd;
        if(_delta >= 0 && delta < 0) delta = 0;
        auto upd(delta1 != delta);
        child->scroll[vert] = delta;
        if(upd) {
            child->requestPosition();
            // отправляем событие скролла
            post(MSG_SCROLLING, duration, child->scroll[vert]);
            awakenScroll();
        } else if(glow) {
            updateGlow(_delta);
        }
        return !upd;
    }
    return true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                                          EDIT LAYOUT                                                                   //
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class zLayoutEdit : public zViewEdit {
public:
    explicit zLayoutEdit(zViewEdit* _edit) : zViewEdit(_edit->styles, _edit->id, 0) { setHint(_edit->getHint()); }
protected:
    void notifyEvent(HANDLER_MESSAGE* msg) override {
        if(msg->what == MSG_EDIT) getParent<zEditLayout>()->childUpdateText(z_isempty(getText()));
        zViewEdit::notifyEvent(msg);
    }
};

static zStyle styles_z_hint[] = {
        { Z_FBO, true },
        { Z_IPADDING, 0x0 },
        { Z_BEHAVIOR, 0 },
        { Z_TEXT_LINES, 1 },
        { Z_DURATION | ZT_END, 20}
};

void zEditLayout::changeTheme() {
    zViewGroup::changeTheme();
    if(hint) hint->setTextColorForeground(theme->styles->_int(Z_THEME_COLOR_TEXT_HINT, 0x7f505050));
}

zView *zEditLayout::attach(zView *v, int width, int height, int where) {
    auto _edit(dynamic_cast<zViewEdit*>(v));
    if(!countChildren() && _edit) {
        edit = new zLayoutEdit(_edit);
        hint = new zViewText(styles_z_hint, 0, 0);
        zViewGroup::attach(edit, width, height, where);
        zViewGroup::attach(hint, VIEW_WRAP, VIEW_WRAP, where);
        hint->setTextSize(edit->getTextSize());
        hint->setTextStyle(edit->getTextStyle());
        hint->setText(_edit->getHint(), true);
        hint->setTextColorForeground(edit->getTextHintColor());
        hint->setGravity(edit->gravity);
        hint->updateStatus(ZS_VISIBLED, false);
        delete v; return edit;
    }
    return nullptr;
}

void zEditLayout::notifyEvent(HANDLER_MESSAGE *msg) {
    if(msg->what == MSG_ANIM) {
        if(countChildren()) {
            frame += !textEmpty * 2 - 1;
            auto cont(frame > 0 && frame < 4);
            if(!cont && textEmpty) {
                edit->setHint(hint->getText());
                hint->updateStatus(ZS_VISIBLED, false);
            }
            edit->requestLayout();
            if(cont) post(MSG_ANIM, duration);
        }
    }
    zViewGroup::notifyEvent(msg);
}

void zEditLayout::onMeasure(cszm& spec) {
    szi size;
    if(edit && hint) {
        measureChild(edit, spec);
        measureChild(hint, spec);
        // добавить высоту подсказки
        size.set(edit->rview.w, edit->rview.h + hint->rview.h);
    }
    defaultOnMeasure(spec, size);
}

void zEditLayout::onLayout(crti &position, bool changed) {
    zView::onLayout(position, changed);
    layoutChild();
}

void zEditLayout::layoutChild() {
    if(edit && hint) {
        auto esub((rview.h - edit->sizes(true)) / 2);
        auto hsub((rview.h - hint->sizes(true)) / 2);
        esub += (frame * esub / 4); hsub -= (frame * hsub / 4);
        auto wmax(edit->getWidthMax());
        edit->layout(rti(rclient.x, rclient.y + esub, edit->rview.w, edit->rview.h));
        // позиционировать текст по горизонтали
        int x(edit->rclient.x + edit->ipad.x);
        switch(hint->gravity & ZS_GRAVITY_HORZ) {
            case ZS_GRAVITY_END:     x += (wmax - hint->rview.w + edit->ipad.w); break;
            case ZS_GRAVITY_HCENTER: x += (wmax - hint->rview.w + edit->ipad.w) / 2; break;
        }
        hint->layout(rti(x, rclient.y + hsub, hint->rview.w, hint->rview.h));
    }
}

void zEditLayout::childUpdateText(bool _empty) {
    textEmpty = _empty;
    if((_empty && frame) || (!_empty && frame != 4)) {
        edit->setHint("");
        hint->updateStatus(ZS_VISIBLED, true);
        post(MSG_ANIM, duration, 0);
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                                        TABBED LAYOUT                                                                   //
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

zTabLayout::zTabLayout(zStyle* _styles, i32 _id, zStyle* _styles_capt, int _gravityCaption) :
        zLinearLayout(styles_default, _id, (_gravityCaption & ZS_GRAVITY_VERT) != 0), styles_caption(_styles_capt), gravCapt(_gravityCaption) {
    // макет заголовков
    caption = new zLinearLayout(_styles, 0, (_gravityCaption & ZS_GRAVITY_VERT) == 0);
    // макет контента
    content = new zFrameLayout(styles_z_tabcontent, 0);
}

void zTabLayout::onInit(bool _theme) {
    zViewGroup::onInit(_theme);
    if(countChildren()) return;
    zViewGroup::attach(content, VIEW_MATCH, VIEW_MATCH);
    zViewGroup::attach(caption, vert ? VIEW_MATCH : VIEW_WRAP, vert ? VIEW_WRAP : VIEW_MATCH, (gravCapt == ZS_GRAVITY_START || gravCapt == ZS_GRAVITY_TOP) - 1);
}

void zTabLayout::showPage(int _page) {
    if(_page >= 0 && _page < caption->countChildren()) {
        // спрятать активную страницу
        if(activePage != -1) content->atView(activePage)->updateVisible(false);
        // активная страница
        activePage = _page;
        // показать новую страницу
        content->atView(activePage)->updateVisible(true);
        // показать селектор
        showSelector();
    }
}

void zTabLayout::showSelector() {
    auto& r(caption->atView(activePage)->rview);
    caption->drw[DRW_SEL]->measure(vert ? r.w : 6, vert ? 6 : r.h, PIVOT_ALL, false);
    caption->drw[DRW_SEL]->bound.x = r.x + (gravCapt == ZS_GRAVITY_START) * (r[3 - !vert] - 8);
    caption->drw[DRW_SEL]->bound.y = r.y + (gravCapt == ZS_GRAVITY_TOP) * (r[vert + 2] - 8);
}

zView* zTabLayout::attach(zView* page, int width, int height, int where) {
    auto widget(dynamic_cast<zTabWidget*>(page));
    if(widget) {
        auto countPages(caption->countChildren());
        auto v(new zViewButton(styles_caption, countPages, widget->getText(), widget->getIcon()));
        v->setOnClick([this](zView *v, int) { setActivePage(v->id); });
        caption->attach(v, VIEW_WRAP, VIEW_WRAP);
        content->attach(page, VIEW_MATCH, VIEW_MATCH);
        page->updateVisible(false);
    }
    return page;
}

void zTabLayout::setActivePage(int _page) {
    if(_page != getActivePage()) {
        // показать новую
        showPage(_page);
        // вызвать событие
        if(onTabChange) onTabChange(this, _page);
    }
}

const zViewGroup* zTabLayout::getContentPage(int _page) const {
    return (_page >= 0 && _page < caption->countChildren()) ? content->atView<zViewGroup>(_page) : nullptr;
}

void zTabLayout::stateView(STATE &state, bool save, int &index) {
    zViewGroup::stateView(state, save, index);
    if(save) {
        state.data += activePage;
    } else {
        setActivePage((int)state.data[index++]);
    }
}

void zTabLayout::onLayout(crti &position, bool changed) {
    zLinearLayout::onLayout(position, changed);
    if(activePage == -1) setActivePage(0);
    showSelector();
}
