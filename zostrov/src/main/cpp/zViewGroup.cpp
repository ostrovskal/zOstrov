//
// Created by User on 18.04.2023.
//

#include "zostrov/zCommon.h"
#include "zostrov/zViewGroup.h"

zViewGroup::zViewGroup(zStyle* _styles, i32 _id): zView(_styles, _id) {
    // не принимать фокус и клик
    status &= ~(ZS_FOCUSABLE | ZS_CLICKABLE);
    sizeTouch.set(6_dp, 6_dp);
}

zViewGroup::~zViewGroup() {
    SAFE_DELETE(flyng);
    SAFE_DELETE(glow);
    SAFE_DELETE(scrollBar);
    zViewGroup::removeAllViews(false);
}

void zViewGroup::offsetChildren(int delta) {
    for(auto& child : children) {
        auto pos(child->oldPos);
        pos.offset(!vert * delta, vert * delta);
        child->layout(pos);
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
        v->requestLayout();
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

void zViewGroup::removeAllViews(bool _update) {
    while(countChildren()) remove(children[0]);
    if(_update) requestLayout();
}

void zViewGroup::detachAllViews(bool _update) {
    while(countChildren()) detach(children[0]);
    if(_update) requestLayout();
}

void zViewGroup::_remove(zView* v, bool _del) {
    if(v) {
        auto idx(children.indexOf<zView*>(v));
        if(idx != -1) {
            // отсоединить
            v->cleanup();
            // вызвать событие отсоединения
            v->onDetach();
            // убрать родителя
            v->parent = nullptr;
            // удалить из массива дочерних
            children.erase(idx, 1, _del);
            // удалить из касания
            if(_del && zView::touch && zView::touch->own == v)
                zView::touch->reset();//own = nullptr;
        }
    }
}

void zViewGroup::notifyEvent(HANDLER_MESSAGE* msg) {
    if(msg->what == MSG_SCROLLING && onChangeScroll)
        onChangeScroll(this, msg->arg1);
    else if(msg->what == MSG_SELECTED && onChangeSelected)
        onChangeSelected(this, msg->arg1);
    zView::notifyEvent(msg);
}

void zViewGroup::cleanup() {
    for(auto& c : children) c->cleanup();
    zView::cleanup();
}

void zViewGroup::disable(bool set) {
    // дочерние
    for(auto v : children) {
        v->disable(set);
    }
    zView::disable(set);
}

void zViewGroup::onDraw() {
    // селектор
    drw[DRW_SEL]->draw(nullptr);
    // дочерние
    for(auto v : children) v->draw();
    // прокрутка
    if(scrollBar && scrollBar->isVisibled()) scrollBar->draw();
    // глоу
    if(glow && glow->isVisibled()) glow->draw();
}

bool zViewGroup::intersectChildren(int x, int y) const {
    return std::any_of(std::begin(children), std::end(children), [x, y](const auto& c) {
        if(c->isVisibled() && c->isTouchable()) {
            auto g(dynamic_cast<zViewGroup*>(c));
            return ((g && g->intersectChildren(x, y)) || c->rclip.contains(x, y));
        }
        return false;
    });
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

static bool isOwn(zViewGroup* grp, zView* own) {
    if(grp == own) return true;
    auto parent(own->getParent());
    return (parent && own != parent && isOwn(grp, parent));
}

i32 zViewGroup::touchEvent(AInputEvent* event) {
    i32 result(TOUCH_CONTINUE);
    if(isVisibled()) {
        if(initTouchHandler(event)) {
            if(touch->own && !isOwn(this, touch->own))
                return result;
            // вызываем событие касания
            result = onTouchEvent();
            if(!touch->own && result) touch->own = this;
            // устанавливаем тап
            if(testFlags(ZS_TAP_MASK)) {
                updateStatus(ZS_TAP, touch->isCaptured());
                invalidate();
            }
        }
    }
    return result;
}

i32 zViewGroup::touchEventDispatch(AInputEvent *event) {
    // если текущий элемент видим
    int ret(TOUCH_CONTINUE);
    if(isVisibled()) {
        if(initTouchHandler(event)) {
            if(touchChildren()) {
                auto count(countChildren());
                for(int i = count - 1; i >= 0; i--) {
                    ret = children[i]->touchEventDispatch(event);
                    if(ret == TOUCH_STOP) return ret;
                    if(ret != TOUCH_CONTINUE) break;
                }
            }
            if((ret && touchSelf()) || ret == TOUCH_CONTINUE) {
                // родительский
                ret = touchEvent(event);
                // если объект модальный - выход
                if(testFlags(ZS_MODAL)) ret = TOUCH_STOP;
            }
        }
    }
    return ret;
}

void zViewGroup::onInit(bool _theme) {
    zView::onInit(_theme);
    // теперь специфичные
    int decor(0);
    styles->enumerate([this, &decor, _theme](u32 attr) {
        auto v((int)zTheme::value.u); attr |= _theme * ZTT_THM;
        switch(attr) {
            case Z_SIZE_TOUCH:		    sizeTouch.set(v);break;
            case Z_DECORATE:            decor       = v; break;
            case Z_DIVIDER_SHADER:      v = manager->getShader(theme->findString(v));
            case Z_DIVIDER:
            case Z_DIVIDER_COLOR:
            case Z_DIVIDER_TILES:
            case Z_DIVIDER_PADDING:
            case Z_DIVIDER_TYPE:
            case Z_DIVIDER_SIZE:        drParams[DR_DIV].set(Z_DIVIDER, attr, v); break;
            case Z_SELECTOR_SHADER:     v = manager->getShader(theme->findString(v));
            case Z_SELECTOR:
            case Z_SELECTOR_COLOR:
            case Z_SELECTOR_TILES:
            case Z_SELECTOR_PADDING:    drParams[DR_SEL].set(Z_SELECTOR, attr, v); break;
            case Z_SCROLLBAR_SHADER:    v = manager->getShader(theme->findString(v));
            case Z_SCROLLBAR:
            case Z_SCROLLBAR_SIZE:
            case Z_SCROLLBAR_FADE:
            case Z_SCROLLBAR_TILES:     drParams[DR_SCL].set(Z_SCROLLBAR, attr, v); break;
        }
    });
    // селектор
    setDrawable(&drParams[DR_SEL], DRW_SEL);
    // разделитель
    setDrawable<zDrawableDivider>(&drParams[DR_DIV], DRW_DIV, &div);
    // прокрутка
    SAFE_DELETE(scrollBar);
    if(decor & ZS_SCROLLBAR) {
        auto _decor(decor);
        if((_decor & ZS_SCROLLBAR) == ZS_SCROLLBAR) _decor = vert * ZS_VSCROLLBAR;
        scrollBar = new zViewScrollBar(this, (_decor & ZS_SCROLLBAR) == ZS_VSCROLLBAR);
    }
    // глоу
    SAFE_DELETE(glow);
    if(decor & ZS_GLOW) {
        glow = new zViewGlow(this);
    }
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
        scrollBar->updateStatus(ZS_VISIBLED, showThumb);
        if(showThumb) {
            auto sz(sizes(vert) - pad.extent(vert));
            auto length(z_round((float)sz * ext / ran)), offset(z_round(((float)(sz - length) * offs / (ran - ext))));
            ws = (ws > pad[vert + 2] ? pad[vert + 2] : ws);
            auto subW((pad[vert + 2] - ws) / 2), minLength(ws * 2);
            // если слишком маленький
            if(length < minLength) length = minLength;
            // если слишком большой
            if(offset + length > sz) offset = sz - length;
            auto _bound(&scrollBar->rview);
            if(vert) {
                _bound->set(rclient.w - ws, offset, ws, length);
            } else {
                _bound->set(offset, rclient.h - ws, length, ws);
            }
            _bound->offset(rclient.x + vert * subW, rclient.y + !vert * subW);
            scrollBar->awaken();
        }
    }
}

void zViewGroup::changeTheme() {
    for(auto child : children) {
        child->changeTheme();
    }
    zView::changeTheme();
}

bool zViewGroup::testLocked() const {
    return std::any_of(std::begin(children), std::end(children), [](const auto& c) {
        return c->isVisibled() && c->testLocked();
    });
}
