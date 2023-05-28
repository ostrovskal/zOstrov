//
// Created by User on 18.04.2023.
//

#include "zssh/zCommon.h"
#include "zssh/zViewGroup.h"

zViewGroup::zViewGroup(zStyle* _styles, i32 _id): zView(_styles, _id) {
    // не принимать фокус и клик
    status &= ~(ZS_FOCUSABLE | ZS_CLICKABLE);
    sizeTouch.set(5_dp, 5_dp);
}

void zViewGroup::offsetChildren(int delta, bool vert) {
    for(auto& child : children) {
        child->rview.offset(!vert * delta, vert * delta);
    }
}

zView* zViewGroup::attachInLayout(zView *v, int width, int height, int where) {
    if(v) {
        v->parent = this;
        v->lps.w = width;
        v->lps.h = height;
        children.insert(where == -1  ? children.size() : where, v);
        v->onAttach();
    }
    return v;
}

zView* zViewGroup::attach(zView *v, int width, int height, int where) {
    if(v) {
        attachInLayout(v, width, height, where);
        v->onInit(false);
        requestLayout();
    }
    return v;
}

zView* zViewGroup::attach(zView *v, int x, int y, int width, int height, int where) {
    if(attach(v, width, height, where)) {
        v->lps.x = x;
        v->lps.y = y;
    }
    return v;
}

void zViewGroup::removeAllViews() {
    for(auto& child : children) child->_detach();
    children.clear();
    requestLayout();
}

void zViewGroup::detachAllViews() {
    for(auto& child : children) child->_detach();
    children.free();
    requestLayout();
}

void zViewGroup::_remove(zView* v, bool _del) {
    auto idx(children.indexOf<zView*>(v));
    if(idx != -1) {
        // отсоединить
        v->_detach();
        // удалить из массива дочерних
        children.erase(idx, 1, _del);
    }
}

void zViewGroup::_detach() {
    for(auto& c : children) c->_detach();
    zView::_detach();
}

void zViewGroup::disable(bool set) {
    // дочерние
    for(auto v : children) {
        v->disable(set);
    }
    zView::disable(set);
}

void zViewGroup::draw() {
    if(isVisibled()) {
        // рисуем
        if(isDirty()) {
            drw[DRW_FBO]->drawFBO(fbo, [this] {
                drw[DRW_BK]->draw(&rview);
                // селектор
                drw[DRW_SEL]->draw(&rclient);
                onDraw();
                // дочерние
                for(auto v : children) v->draw();
                // рисуем декорации
                // прокрутка
                if(scrollBar && scrollBar->isVisibled()) scrollBar->draw();
                // глоу
                if(glow && glow->isVisibled()) glow->draw();
            });
            updateStatus(ZS_DIRTY_LAYER, !isFBO());
        }
        onDrawFBO();
        drawDebug();
    }
}

bool zViewGroup::intersectChildren(int x, int y) const {
    for(auto& c : children) {
        if(c->isVisibled()) {
            if(c->isTouchable()) {
                auto g(dynamic_cast<zViewGroup*>(c));
                if(g) {
                    if(g->intersectChildren(x, y))
                        return true;
                } else if(c->rclip.contains(x, y))
                    return true;
            }
        }
    }
    return false;
}

zView* zViewGroup::sysView() const {
    auto count(countChildren());
    for(int i = count - 1; i >= 0; i--) {
        auto g(atView<zViewGroup>(i));
        if(g && g->isVisibled()) {
            if(g->testFlags(ZS_SYSTEM)) return g;
            auto v(g->sysView()); if(v) return v;
        }
    }
    return nullptr;
}

i32 zViewGroup::touchEvent(AInputEvent* event) {
    // если текущий элемент видим
    if(isVisibled()) {
        auto count(children.size());
        bool ret(count != 0);
        // берем дочерние с конца
        for(int i = count - 1; i >= 0; i--) {
            auto child(children[i]);
            if((ret = child->touchEvent(event)) != TOUCH_CONTINUE) break;
        }
        // родительский
        ret |= zView::touchEvent(event);
        // если объект модальный - выход
        if(testFlags(ZS_MODAL) || ret) {
            return TOUCH_STOP;
        }
    }
    return TOUCH_CONTINUE;
}

void zViewGroup::onInit(bool _theme) {
    // сначало основные
    zView::onInit(_theme);
    // теперь специфичные
    int decor(0); zParamDrawable dv, sl;
    styles->enumerate([this, &dv, &sl, &decor, _theme](u32 attr) {
        auto v((int)zTheme::value.u); attr |= _theme * ZTT_THM;
        switch(attr) {
            case Z_SIZE_TOUCH:		    sizeTouch.set(v); break;
            case Z_DECORATE:            decor       = v; break;
            case Z_DIVIDER:             dv.texture  = v; break;
            case Z_DIVIDER_COLOR:       dv.color    = v; break;
            case Z_DIVIDER_TILES:       dv.tiles    = v; break;
            case Z_DIVIDER_PADDING:     dv.padding  = v; break;
            case Z_DIVIDER_SIZE:        dv.size     = v; break;
            case Z_DIVIDER_TYPE:        dv.type     = v; break;
            case Z_SELECTOR:            sl.texture  = v; break;
            case Z_SELECTOR_COLOR:      sl.color    = v; break;
            case Z_SELECTOR_TILES:      sl.tiles    = v; break;
            case Z_SELECTOR_PADDING:    sl.padding  = v; break;
        }
    });
    // глоу
    SAFE_DELETE(glow); if(decor & ZS_GLOW) glow = new zViewGlow(this);
    // прокрутка
    SAFE_DELETE(scrollBar);
    if(decor & ZS_SCROLLBAR) {
        if((decor & ZS_SCROLLBAR) == ZS_SCROLLBAR) decor = vert * ZS_VSCROLLBAR;
        scrollBar = new zViewScrollBar(this, (decor & ZS_SCROLLBAR) == ZS_VSCROLLBAR);
    }
    // селектор
    setDrawable(&sl, DRW_SEL);
    // разделитель
    setDrawable<zDrawableDivider>(&dv, DRW_DIV, &div);
}

szi zViewGroup::measureChildren(cszm& spec) {
    int widthMax(0), heightMax(0);
    for(auto& child : children) {
        if(child->isVisibled()) {
            measureChild(child, spec);
            widthMax  = z_max(widthMax, child->sizes(false));
            heightMax = z_max(heightMax, child->sizes(true));
        }
    }
    return { widthMax, heightMax };
}

void zViewGroup::measureChild(zView* child, cszm& spec) {
    // 1. ѕолучить LayoutParams дочернего
    auto lps(&child->lps);
    // 2. —оздать MeasureSpec дочернего на основе makeChildMeasureSpec
    szm childMeasureSpec(makeChildMeasureSpec(spec.w, pad.extent(false) + child->margin.extent(false), lps->w),
                         makeChildMeasureSpec(spec.h, pad.extent(true) + child->margin.extent(true), lps->h));
    // 3. измерить дочерний
    child->measure(childMeasureSpec);
}

zMeasure zViewGroup::makeChildMeasureSpec(const zMeasure& spec, int padding, zMeasure childDimension) {
    int size(z_max(0, spec.size() - padding)); int resultSize, resultMode;
    switch(spec.mode()) {
        // родитель навязал нам точный размер
        case MEASURE_EXACT:
            switch(childDimension) {
                case VIEW_WRAP:
                    // дочерний хочет определить свой собственный размер. он не может быть больше нас.
                    resultSize = size;
                    resultMode = MEASURE_MOST;
                    break;
                case VIEW_MATCH:
                    // дочерний хочет быть нашего размера. пусть будет так.
                    childDimension = size;
                default:
                    resultSize = childDimension;
                    resultMode = MEASURE_EXACT;
                    break;
            }
            break;
            // родитель навязал нам максимальный размер
        case MEASURE_MOST:
            switch (childDimension) {
                case VIEW_WRAP:
                    // дочерний хочет определить свой собственный размер. он не может быть больше нас.
                case VIEW_MATCH:
                    // дочерний хочет быть нашего размера... узнайте, какого размера он должен быть
                    resultSize = size;
                    resultMode = MEASURE_MOST;
                    break;
                default:
                    // дочерний хочет определенный размер... пусть будет так.
                    resultSize = childDimension;
                    resultMode = MEASURE_EXACT;
                    break;
            }
            break;
            // родитель спросил, насколько большими мы хотим быть
        default:
        case MEASURE_UNDEF:
            switch (childDimension) {
                case VIEW_WRAP:
                    // дочерний хочет определить свой собственный размер.... выяснить, какого размера он должен быть
                case VIEW_MATCH:
                    // дочерний хочет быть нашего размера... узнайте, какого размера он должен быть
                    resultSize = 0;
                    resultMode = MEASURE_UNDEF;
                    break;
                default:
                    // дочерний хочет определенный размер... позвольте ему это.
                    resultSize = childDimension;
                    resultMode = MEASURE_EXACT;
                    break;
            }
            break;
    }
    return { resultMode, resultSize };
}

void zViewGroup::awakenScroll() {
    if(scrollBar) {
        auto vert(scrollBar->isVertical());
        auto offs((float)computeScrollOffset(vert)), ran((float)computeScrollRange(vert)), ext((float)computeScrollExtent(vert));
        auto showThumb(ext > 0 && ran > ext);
        auto ws(scrollBar->getSize());
        scrollBar->visibility(showThumb);
        if(showThumb) {
            auto sz(sizes(vert) - pad.extent(vert));
            auto length((int)roundf(sz * ext / ran)), offset((int)roundf((sz - length) * offs / (ran - ext)));
            ws = (ws > pad[vert + 2] ? pad[vert + 2] : ws);
            auto subW((pad[vert + 2] - ws) / 2), minLength(ws * 2);
            // если слишком маленький
            if(length < minLength) length = minLength;
            // если слишком большой
            if(offset + length > sz) offset = sz - length;
            auto _bound(&scrollBar->rview);
            if(vert) {
                _bound->set(rclient.w, offset, ws, length);
            } else {
                _bound->set(offset, rclient.h, length, ws);
            }
            _bound->offset(rclient.x + vert * subW, rclient.y + !vert * subW);
            scrollBar->awaken();
        }
    }
}

void zViewGroup::changeTheme() {
    for(auto& child : children) {
        child->changeTheme();
    }
    zView::changeTheme();
}

bool zViewGroup::testLocked() const {
    //return std::any_of(*children.begin(), *children.last(), isVisibled() && testLocked());
    return false;
}
