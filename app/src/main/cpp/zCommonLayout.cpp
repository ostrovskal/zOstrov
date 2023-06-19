//
// Created by User on 25.04.2022.
//

#include "zssh/zCommon.h"
#include "zssh/zCommonLayout.h"

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
    szi _max, _wh;  szm childSpec;
    auto divSize(div ? div->resolve(countChildren(), true) : 0);
    _max[vert] = divSize + padMargin(vert);
    // определяем габариты и веса дочерних
    auto _undef(spec[vert].mode() == MEASURE_UNDEF);
    for(auto child : children) {
        if(child->isVisibled()) {
            measureChild(child, spec);
            marginsChild += child->margin.extent(vert);
            // определение количества дочерних с VIEW_MATCH
            if(_undef || child->lps[vert + 2] != VIEW_MATCH)
                _max[vert] += child->sizes(vert); else countMatch++;
            _max[!vert] = z_max(_max[!vert], child->sizes(!vert));
//            DLOG("child %i %i %s", child->sizes(false), child->sizes(true), child->typeName());
            // весь вес
            auto weight(child->lps.weight);
            isWeights |= (weight != 0);
            allWeight += z_max(1, weight);
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
//    DLOG("");
    for(auto child: children) {
        if(child->isVisibled()) {
            auto& lps(child->lps);
            auto weight(z_max<float>(1.0f, (float)child->lps.weight));
            if(isWeights) weight /= (float)allWeight;
            auto oldSize(child->rview[vert + 2]);
            auto _size(lps[vert + 2] == VIEW_MATCH ? sizeMatch : (int)lps[vert + 2]);
            auto size(isWeights ? (int)roundf((float)_wh[vert] * weight) : _size), wh((int)lps[3 - vert]);
            childSpec.set(makeChildMeasureSpec(zMeasure(MEASURE_EXACT, _wh.w), child->margin.extent(false), vert ? wh : size),
                          makeChildMeasureSpec(zMeasure(MEASURE_EXACT, _wh.h), child->margin.extent(true), vert ? size : wh));
            child->measure(childSpec);
            if(!isWeights) {
                _wh[vert] -= child->sizes(vert);
                auto subSize(child->rview[vert + 2] - oldSize);
                _wh[vert] += subSize; _max[vert] += subSize;
            }
        }
    }
    defaultOnMeasure(spec, _max);
    // разделитель
    if(div) div->measure(0, 0, PIVOT_ALL, false);
}

pti zLinearLayout::applyGravity(crti& sizeParent, crti& sizeChild, u32 _gravity) {
    auto ret(zView::applyGravity(sizeParent, sizeChild, _gravity));
    ret[vert] = 0;
    return ret;
}

void zLinearLayout::onLayout(crti &position, bool changed) {
    zView::onLayout(position, changed);
    auto r(rclient);
    // размер разделителя
    auto divSize(div ? div->getSize(ZS_DIVIDER_MIDDLE) : 0);
    // начальная координата с учетом разделителя
    r[vert] += (div ? div->getSize(ZS_DIVIDER_BEGIN) : 0);
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
    auto spc(space * 2), cols(z_max(1, cells.w)), rows(z_max(1, cells.h));
    // размеры по умолчанию
    auto hCell((float)(height - hpad) / (float)rows);
    auto wCell((float)(width - wpad)  / (float)cols);
    if(cells.w <= 0) wCell = hCell, cols = (int)((float)(width - wpad) / wCell);
    else if(cells.h <= 0) hCell = wCell, rows = (int)((float)(height - hpad) / hCell);
    auto wExact(spec.w.isExact()), hExact(spec.h.isExact());
    if(!(wExact && hExact)) {
        // определяем максимальный размер ячейки
        for(auto &child: children) {
            if(child->isVisibled()) {
                // параметры макета
                auto lps(&child->lps);
                // определяем габариты дочернего
                child->measure({ makeChildMeasureSpec(spec.w, 0, VIEW_WRAP),
                                 makeChildMeasureSpec(spec.h, 0, VIEW_WRAP) });
                // определяем максимальный размер ячейки
                if(!wExact) wCell = z_max(wCell, (float)(child->rview.w - spc) / lps->w);
                if(!hExact) hCell = z_max(hCell, (float)(child->rview.h - spc) / lps->h);
            }
        }
    }
    wCell = (wCell ? wCell : 40_dp);
    hCell = (hCell ? hCell : 40_dp);
    // определяем габариты макета
    if(!hExact) {
        height = hpad + hCell * rows;
        if(spec.h.mode() == MEASURE_MOST) height = z_min(spec.h.size(), height);
    }
    if(!wExact) {
        width = wpad + wCell * cols;
        if(spec.w.mode() == MEASURE_MOST) width = z_min(spec.w.size(), width);
    }
    rows = (height - hpad) / hCell; height = hpad + hCell * rows;
    cols = (width - wpad) / wCell; width = wpad + wCell * cols;
    cellDebug.w = cols; cellDebug.h = rows;
    // ставим фактические размеры
    for(auto &child: children) {
        if(child->isVisibled()) {
            // параметры макета
            auto lps(&child->lps);
            // корректируем габариты
            correctSize(lps, cols, rows);
            // определяем габариты дочернего
            szm childSpec(zMeasure(MEASURE_EXACT, (int)(lps->w * wCell - spc)),
                          zMeasure(MEASURE_EXACT, (int)(lps->h * hCell - spc)));
            child->measure(childSpec);
        }
    }
    cell.set(wCell, hCell);
//    DLOG("wc:%f hc:%f", wCell, hCell);
//    DLOG("w:%i h:%i", width, height);
//    DLOG("col:%i row:%i", cols, rows);
    setMeasuredDimension(width, height);
}

void zCellLayout::onLayout(crti &position, bool changed) {
    zViewGroup::onLayout(position, changed);
    rti r;
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

void zScrollLayout::onLayout(crti &position, bool changed) {
    zView::onLayout(position, changed);
    auto child(atView(0));
    // если он сам сдвинут
    if(child) child->layout(rclient - scroll);
}

zView *zScrollLayout::attach(zView *v, int width, int height, int where) {
    if(!countChildren()) return zViewGroup::attach(v, width, height, where);
    return nullptr;
}

i32 zScrollLayout::onTouchEvent(zTouch *touch) {
    bool drag(false);
    touch->drag(sizeTouch, [this, &drag, &touch](cszi& offs, bool event) {
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
            if(onChangeScroll) onChangeScroll(this, child->scroll[vert]);
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
    zLayoutEdit(zEditLayout* _lyt, zViewEdit* _edit) : zViewEdit(_edit->styles, _edit->id, 0), lyt(_lyt) { setHint(_edit->getHint()); }
protected:
    void updateText(int _what) override {
        if(_what == MSG_EDIT) lyt->childUpdateText(z_isempty(getText()));
        zViewEdit::updateText(_what);
    }
    zEditLayout* lyt{nullptr};
};

static zStyle styles_z_hint[] = {
        { Z_FBO, true },
        { Z_IPADDING, 0x0 },
        { Z_BEHAVIOR, 0 },
        { Z_TEXT_NOWRAP, true },
        { Z_DURATION, 20},
        { Z_TEXT_FONT | ZT_END, z.R.drawable.font1 }
};

zView *zEditLayout::attach(zView *v, int width, int height, int where) {
    auto _edit(dynamic_cast<zViewEdit*>(v));
    if(!countChildren() && _edit) {
        setOnAnimation([this](zView*, int) {
            frame += !textEmpty * 2 - 1;
            auto cont(frame > 0 && frame < 4);
            if(!cont && textEmpty) {
                auto h(atView<zViewText>(1));
                atView<zViewEdit>(0)->setHint(h->getText());
                h->updateVisible(false);
            }
            requestLayout();
            return cont;
        });
        auto edit(new zLayoutEdit(this, _edit));
        auto hint(new zViewText(styles_z_hint, 0, 0));
        zViewGroup::attach(edit, width, height, where);
        zViewGroup::attach(hint, VIEW_WRAP, VIEW_WRAP, where);
        hint->setTextSize(edit->getTextSize());
        hint->setTextStyle(edit->getTextStyle());
        hint->setText(_edit->getHint(), true);
        hint->setTextColorForeground(0xffb8b8b8);//edit->getTextHintColor());
        hint->setGravity(edit->gravity);
        hint->updateStatus(ZS_VISIBLED, false);
        delete v; return edit;
    }
    return nullptr;
}

void zEditLayout::onMeasure(cszm& spec) {
    szi size;
    auto edit(atView<zViewEdit>(0));
    if(edit) {
        measureChild(edit, spec);
        auto hint(atView<zViewText>(1)); measureChild(hint, spec);
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
    auto edit(atView<zViewEdit>(0));
    if(edit) {
        auto hint(atView<zViewText>(1));
        auto esub((rview.h - edit->sizes(true)) / 2);
        auto hsub((rview.h - hint->sizes(true)) / 2);
        esub += (frame * esub / 4); hsub -= (frame * hsub / 4);
        auto wmax(edit->getWidthMax());
        edit->layout(rti(rclient.x, rclient.y + esub, edit->rview.w, edit->rview.h));
        // позиционировать текст по горизонтали
        int x(edit->rclient.x + edit->ipad.x);
        switch(hint->gravity & ZS_GRAVITY_HORZ) {
            case ZS_GRAVITY_END:     x += (wmax - hint->rview.w - edit->ipad.extent(false)); break;
            case ZS_GRAVITY_HCENTER: x += (wmax - hint->rview.w) / 2; break;
        }
        hint->layout(rti(x, rclient.y + hsub, hint->rview.w, hint->rview.h));
    }
}

void zEditLayout::childUpdateText(bool _empty) {
    textEmpty = _empty;
    if((_empty && frame) || (!_empty && frame != 4)) {
        atView<zViewEdit>(0)->setHint("");
        atView(1)->updateStatus(ZS_VISIBLED, true);
        post(MSG_ANIM, duration, 0);
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                                        TABBED LAYOUT                                                                   //
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

zTabLayout::zTabLayout(zStyle* _styles, i32 _id, zStyle* _styles_capt, int _gravityCaption) :
        zLinearLayout(styles_z_tabs, _id, (_gravityCaption & ZS_GRAVITY_VERT) != 0), styles_caption(_styles_capt), gravCapt(_gravityCaption) {
    // макет заголовков
    caption = new zLinearLayout(_styles, 0, (_gravityCaption & ZS_GRAVITY_VERT) == 0);
    // макет контента
    content = new zFrameLayout(styles_z_tabcontent, 0);
}

void zTabLayout::onInit(bool _theme) {
    zViewGroup::onInit(_theme);
    detachAllViews(true);
    attach(caption, vert ? VIEW_MATCH : VIEW_WRAP, vert ? VIEW_WRAP : VIEW_MATCH);
    attach(content, VIEW_MATCH, VIEW_MATCH);
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
    auto r(&caption->atView(activePage)->rview);
    caption->drw[DRW_SEL]->measure(r->w, r->h, 3, false);
    caption->drw[DRW_SEL]->bound = r->xy();
}

void zTabLayout::addPage(zViewGroup* group, u32 tabText, i32 tabIcon) {
    auto countPages(caption->countChildren());
    auto v(new zViewButton(styles_caption, countPages, tabText, tabIcon));
    v->setOnClick([this](zView* v, int) { setActivePage(v->id); });
    caption->attach(v, VIEW_WRAP, VIEW_WRAP);
    content->attach(group, VIEW_MATCH, VIEW_MATCH);
    if(countPages) group->updateVisible(false);
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
    zView::stateView(state, save, index);
    if(save) {
        state.data += activePage;
    } else {
        setActivePage((int)state.data[index++]);
    }
}

void zTabLayout::onLayout(crti &position, bool changed) {
    zLinearLayout::onLayout(position, changed);
    if(activePage == -1) setActivePage(0);
}
