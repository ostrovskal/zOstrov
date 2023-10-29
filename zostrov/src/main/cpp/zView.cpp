//
// Created by User on 18.04.2023.
//

#include "zostrov/zCommon.h"
#include "zostrov/zViewGroup.h"

// фейковый отображатель
zDrawableFake zView::fake;
zView* zView::fbo(nullptr);
zParamDrawable zView::drParams[DR_COUNT]{};
zMatrix zView::m;
zTouch* zView::touch(nullptr);

zView::zView(zStyle* _styles, i32 _id) : styles(_styles), id(_id) {
    for(auto& d : drw) d = &fake;
}

zView::~zView() {
    for(auto& dr : drw) { if(dr->isValid()) SAFE_DELETE(dr); }
}

pti zView::applyGravity(crti& sizeParent, crti& sizeChild, u32 _gravity) {
    pti ret;
    int sw((int)(_gravity & ZS_GRAVITY_HORZ));
    ret.x = (sizeParent.w - sizeChild.w) >> gravity_shift[sw & 3];
    int sh((int)(_gravity & ZS_GRAVITY_VERT));
    ret.y = (sizeParent.h - sizeChild.h) >> gravity_shift[(sh >> 2) & 3];
    return ret;
}

void zView::changeTheme() {
    drw[DRW_FBO]->release();
    status &= ~ZS_MEASURE; status |= ZS_DIRTY_LAYER;
    if(manager->isChangeTheme()) {
        onInit(true);
    } else {
        oldPos.set(INT_MAX, INT_MAX, 0, 0);
    }
}

void zView::setMinMaxSize(int val) {
    auto ptr((u8*)&val);
    for(int i = 0 ; i < 4; i++) {
        if(ptr[i]) minMaxSize[i] = ptr[i];
    }
}

void zView::onInit(bool _theme) {
    oldPos.set(INT_MAX, INT_MAX, 0, 0);
    zParamDrawable::setDefaults();
    styles->enumerate([this, _theme](u32 attr) {
        auto v(&zTheme::value); auto val((int)v->u);
        attr |= _theme * ZTT_THM;
        switch(attr) {
            case Z_DURATION:			duration    = (u64)val; break;
            case Z_SCALE_X:				scale.x     = v->f; break;
            case Z_SCALE_Y:				scale.y     = v->f; break;
            case Z_ROTATE:				rot.z       = v->f; break;
            case Z_ROTATE_X:			rot.x       = v->f; break;
            case Z_ROTATE_Y:			rot.y       = v->f; break;
            case Z_TRANSLATE_X:   		trans.x     = v->f; break;
            case Z_TRANSLATE_Y:   		trans.y     = v->f; break;
            case Z_ALPHA:				alpha 	    = v->f; break;
            case Z_BEHAVIOR:            updateStatus(ZS_BEHAVIOR_MASK, val, true); break;
            case Z_VISIBLED:            updateStatus(ZS_VISIBLED, val); break;
            case Z_TAP:			        updateStatus(ZS_TAP_MASK, val, true); break;
            case Z_GRAVITY:			    setGravity(val); break;
            case Z_SIZE:			    setMinMaxSize(val); break;
            case Z_IPADDING:		    ipad.set(val); break;
            case Z_PADDING:			    pad.set(val); break;
            case Z_MARGINS:             margin.set(val); break;
            case Z_DECORATE:            updateStatus(ZS_DECORATE_MASK, val, true); break;
            case Z_FOREGROUND_GRAVITY:  setForegroundGravity(val); fkGravity &= ~ZS_SCALE_MASK; fkGravity |= (val & ZS_SCALE_MASK); break;
            case Z_MASK_SHADER:         val = manager->getShader(v->s);
            case Z_MASK:
            case Z_MASK_COLOR:
            case Z_MASK_TILES:
            case Z_MASK_PADDING:
            case Z_MASK_GRAVITY:        drParams[DR_MSK].set(Z_MASK, attr, val); break;
            case Z_FOREGROUND_TILES:    attr = Z_FOREGROUND_TILE1;
            case Z_FOREGROUND_TILE1:
            case Z_FOREGROUND_TILE2:
            case Z_FOREGROUND_TILE3:
            case Z_FOREGROUND_TILE4:    ((u8*)&drParams[DR_FK].tiles)[(attr & ZTV_MASK) - (Z_FOREGROUND_TILE1 & ZTV_MASK)] = (val & 0xff);
                                        val = drParams[DR_FK].tiles; attr = Z_FOREGROUND_TILES;
            case Z_FOREGROUND:
            case Z_FOREGROUND_COLOR:
            case Z_FOREGROUND_PADDING:
            case Z_FOREGROUND_SCALE:
            case Z_FOREGROUND_SHADER:   if(attr == Z_FOREGROUND_SHADER) val = manager->getShader(v->s);
                                        drParams[DR_FK].set(Z_FOREGROUND, attr, val);
                                        if(attr == Z_FOREGROUND_SCALE) drParams[DR_FK].scale = v->f; break;
            case Z_BACKGROUND_SHADER:   val = manager->getShader(v->s);
            case Z_BACKGROUND:
            case Z_BACKGROUND_COLOR:
            case Z_BACKGROUND_TILES:
            case Z_BACKGROUND_PADDING:  drParams[DR_BK].set(Z_BACKGROUND, attr, val); break;
            case Z_FBO:                 if(val) setDrawable(nullptr, DRW_FBO); else removeDrawable(DRW_FBO); break;
        }
    });
    // базовые отображатели
    setDrawable(&drParams[DR_MSK], DRW_MSK);
    setDrawable(&drParams[DR_BK], DRW_BK);
    setDrawable(&drParams[DR_FK], DRW_FK);
    // ротация/масштаб
    setScale(scale.x, scale.y);
    setRotation(rot.x, rot.y, rot.z);
}

void zView::drawDebug() {
#ifndef NDEBUG
    // сетка элемента
    if(manager->isDebug()) {
        // создать сетку, если ее нет
        if(!drwDebug.vertices) {
            drwDebug.texture = manager->cache->get("znull", drwDebug.texture);
            drwDebug.makeDebug(cellDebug);
        }
        // установить текстуру
        glBindTexture(GL_TEXTURE_2D, drwDebug.texture->id);
        // параметры шейдера и обрезка
        manager->prepareRender(0, drwDebug.vertices, manager->screen, zMatrix::_identity);
        // фильтр
        manager->setColorFilter(this, zColor::white);
        glDisable(GL_SCISSOR_TEST);
        // нарисовать линии
        glLineWidth(2);
        glDrawArrays(GL_LINES, 0, drwDebug.count);
        // подсчет кол-во линий в кадре
        manager->countLn += drwDebug.count;
        glEnable(GL_SCISSOR_TEST);
    }
#endif
}

void zView::draw() {
    if(isDestroy()) { getParent()->remove(this); return; }
    if(isVisibled()) {
        // рисуем
        if(isDirty()) {
            drw[DRW_FBO]->drawFBO(this, fbo);
            updateStatus(ZS_DIRTY_LAYER, !isFBO());
        }
        onDrawFBO(); drawDebug();
        // каретка
        manager->drawCaret(this);
    }
}

bool zView::initTouchHandler(AInputEvent* event) const {
    if(!touch->isEmpty()) return true;
    return touch->event(event, rclip);
}

i32 zView::onTouchEvent() {
    if(!isEnabled()) return TOUCH_STOP;
    if(!isTouchable()) return TOUCH_CONTINUE;
    if(onTouch) {
        auto ret(onTouch(this, touch));
        if(ret != TOUCH_CONTINUE) return ret;
    }
    if(isClickabled()) {
        if(touch->click(rclip)) {
            requestFocus();
            post(MSG_CLICK, duration, isLongClickabled() && touch->isLongClick());
            return TOUCH_ACTION;
        }
    }
    return touch->isCaptured();
}

i32 zView::touchEvent(AInputEvent* event) {
    i32 result(TOUCH_CONTINUE);
    if(isVisibled()) {
        if(initTouchHandler(event)) {
            if(!touch->own || touch->own == this) {
                // вызываем событие касания
                if((result = onTouchEvent())) touch->own = this;
                // устанавливаем тап
                if(testFlags(ZS_TAP_MASK)) {
                    updateStatus(ZS_TAP, touch->isCaptured());
                    invalidate();
                }
            }
        }
    }
    return result;
}

void zView::setStyles(zStyle* _styles) {
    if(_styles && styles != _styles) {
        styles = _styles;
        onInit(false);
        requestLayout();
    }
}

void zView::disable(bool _disable) {
    if(isEnabled() == _disable) {
        updateStatus(ZS_DISABLED, _disable);
        if(_disable) cleanup();
        invalidate();
    }
}

void zView::checked(bool _check) {
    updateStatus(ZS_CHECKED, _check);
    drw[DRW_FK]->setTileNum(_check)->measure(0, 0, PIVOT_H | PIVOT_W, true);
    invalidate();
}

void zView::requestLayout() {
    status &= ~ZS_MEASURE;
    if(parent && parent->testFlags(ZS_MEASURE))
        parent->requestLayout();
}

void zView::requestPosition() {
    status &= ~ZS_LAYOUT;
    if(parent && parent->testFlags(ZS_LAYOUT))
        parent->requestPosition();
}

void zView::invalidate() {
    status |= ZS_DIRTY_LAYER;
    if(parent && parent->isInvalidate())
        if(parent != this) parent->invalidate();
}

void zView::measure(cszm& spec) {
    auto changed(measureSpec.w != spec.w || measureSpec.h != spec.h);
    if(!testFlags(ZS_MEASURE) || changed) {
        status &= ~ZS_MEASURE_SET;
        onMeasure(spec);
        if(!testFlags(ZS_MEASURE_SET)) {
            ILOG("setMeasureDimension not called! Error!");
            throw std::exception();
        }
        status &= ~ZS_LAYOUT;
        measureSpec = spec;
    }
}

void zView::layout(crti& position) {
    auto changed(position.x != oldPos.x || position.y != oldPos.y);
    if(!testFlags(ZS_LAYOUT) || changed) {
        status |= (ZS_LAYOUT | ZS_MEASURE | ZS_DIRTY_LAYER);
        onLayout(position, changed);
        oldPos = position;
#ifndef NDEBUG
        // убрать сетку
        if(drwDebug.count) drwDebug.release();
#endif
    }
}

void zView::defaultOnMeasure(cszm& spec, szi size) {
    auto ws(spec.w.size()), hs(spec.h.size());
    auto wm(spec.w.mode()), hm(spec.h.mode());
    if(ws && wm == MEASURE_EXACT) {
        size.w = ws;
    } else {
        size.w = z_max(minMaxSize.x, size.w + pad.extent(false));
        // коррекция по макс. ширине
        if(minMaxSize.y) size.w = z_min(minMaxSize.y, size.w);
        if(ws && wm == MEASURE_MOST) size.w = z_min(size.w, ws);
    }
    if(hs && hm == MEASURE_EXACT) {
        size.h = hs;
    } else {
        size.h = z_max(minMaxSize.w, size.h + pad.extent(true));
        // коррекция по макс. высоте
        if(minMaxSize.h) size.h = z_min(minMaxSize.h, size.h);
        if(hs && hm == MEASURE_MOST) size.h = z_min(size.h, hs);
    }
    setMeasuredDimension(size.w, size.h);
}

void zView::drawShape() {
    glBlendFuncSeparate(GL_DST_COLOR, GL_SRC_COLOR, GL_DST_COLOR, GL_SRC_COLOR);
    glDisable(GL_SCISSOR_TEST);
    drw[DRW_MSK]->draw(&rview);
    glEnable(GL_SCISSOR_TEST);
    glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
}

void zView::onLayout(crti &position, bool changed) {
    rview.x = position.x + margin.x; rview.y = position.y + margin.y;
    rti _position(position); _position.w -= margin.extent(false); _position.h -= margin.extent(true);
    // учитывать внешний отступ при выравнивании
    rview += parent->applyGravity(_position, rview, lps.gravity ? lps.gravity : parent->gravity);
    rview -= scroll;
    // вычислить позицию клиента
    rclient.x = rview.x + pad.x; rclient.y = rview.y + pad.y;
    rclip = z_clipRect(parent->rclip, rclient);
    updateStatus(ZS_DRAWING, rclip.isNotEmpty());
    // позиционирование базовых отображателей
    drw[DRW_BK]->measure(rview.w, rview.h, PIVOT_ALL, false);
    drw[DRW_FBO]->measure(rview.w, rview.h, PIVOT_ALL, false);
    drw[DRW_MSK]->measure(rview.w, rview.h, PIVOT_ALL, false);
}

void zView::setMeasuredDimension(int width, int height) {
    rview.w = width; rview.h = height;
    rclient.w = rview.w - pad.extent(false); rclient.h = rview.h - pad.extent(true);
    status |= ZS_MEASURE_SET;
}

bool zView::isFocus() const {
    return manager->isCheckFocus(this);
}

void zView::post(int what, u64 millis, int arg1, int arg2, cstr s) {
    manager->event.send(this, what, millis, arg1, arg2, s);
}

void zView::notifyEvent(HANDLER_MESSAGE* msg) {
    switch(msg->what) {
        case MSG_ANIM:
            if(onAnimation) {
                if(onAnimation(this, msg->arg1))
                    post(MSG_ANIM, duration, msg->arg1);
            }
            break;
        case MSG_DROP_CLICK:
        case MSG_CLICK:
            if(onClick) onClick(this, msg->arg1);
            break;
    }
}

void zView::requestFocus() {
    if(isFocusableInTouch()) {
        manager->changeFocus(this);
    }
}

void zView::setAlpha(float _alpha) {
    alpha = _alpha;
    invalidate();
}

void zView::setScale(float _x, float _y) {
    // установка масштаба
    scale.set(_x, _y, 1); mtx.scale(scale);
    mtx *= m.rotate(rot);
    invalidate();
}

void zView::setRotation(float _x, float _y, float _z) {
    // установка угла
    rot.set(deg2rad(_x), deg2rad(_y), deg2rad(_z));
    setScale(scale.x, scale.y);
}

int zView::updateVisible(bool set) {
    if(isVisibled() != set) {
        updateStatus(ZS_VISIBLED, set);
        if(!set) cleanup();
        requestLayout();
    }
    return (int)(set * ZS_VISIBLED);
}

void zView::setTranslation(float _x, float _y) {
    trans.set(_x, _y, 0);
    invalidate();
}

void zView::cleanup() {
    // убрать фокус
    if(isFocus()) manager->changeFocus(nullptr);
    // убрать тап
    updateStatus(ZS_TAP, false);
    // удалить связанные события
    manager->eraseAllEventsView(this);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                                        GLOW                                                                            //
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

zViewGlow::zViewGlow(zView* group) : zView(styles_z_glow, 0) {
    parent = group; zView::onInit(false);
    updateStatus(ZS_VISIBLED, false);
}

void zViewGlow::notifyEvent(HANDLER_MESSAGE *msg) {
    if(msg->what == MSG_ANIM) {
        float v; bool cont;
        if((cont = animator.update(v))) {
            // посчитать альфу
            alpha = v / 2.0f;
            // посчитать размер
            setScale(vert ? 1 : v, vert ? v : 1);
            rview[!vert] = parent->rclient[!vert];
            rview[vert]  = parent->edges(vert, flow);
        }
        invalidate();
        if(updateStatus(ZS_VISIBLED, cont))
            post(MSG_ANIM, duration);
    }
    zView::notifyEvent(msg);
}

void zViewGlow::start(float _delta, bool _vert, bool _flow) {
    // параметры отображения
    if(isVisibled()) return;
    updateStatus(ZS_VISIBLED, true); vert = _vert; flow = _flow;
    alpha = 0.0f; setRotation(0, 0, (float)_flow * 180.0f);
    // базовый тайл
    drw[DRW_BK]->tile = _vert ? z.R.integer.horzGlow : z.R.integer.vertGlow;
    auto pivot(_vert ? (PIVOT_H | PIVOT_X) : (PIVOT_W | PIVOT_Y));
    drw[DRW_BK]->measure(_vert * parent->rclient.w, !_vert * parent->rclient.h, pivot, false);
    // габариты и позиция
    rview.w = drw[DRW_BK]->bound.w; rview.h = drw[DRW_BK]->bound.h;
    // анимация
    _delta = z_min(2.1f, z_max(0.75f, fabs(_delta * 5.0f)));
    animator.init(0.0f, false);
    animator.add(_delta, zInterpolator::LINEAR, 6);
    animator.add(0.0f, zInterpolator::EASEINCUBIC, 8);
    // запуск анимации
    post(MSG_ANIM, duration);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                                        FLYNG                                                                           //
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

zFlyng::zFlyng(zView* group) : zView(styles_default, 0) {
    parent = group;
}

bool zFlyng::start(zTouch* touch, int delta) {
    auto t((int)((touch->ctm - touch->btm) / 1350000));
    // если отпустили и время < 15(выбрано экспериментально)
    if(t < 15) {
        delta *= (24 - t) / 4;
//        DLOG("fling d: %i t: %i", delta, t);
        animator.init((float)delta, false);
        animator.add(0.0f, zInterpolator::EASEOUTQUAD, abs(delta));
        post(MSG_ANIM, duration, 0);
    }
    return t < 15;
}

void zFlyng::notifyEvent(HANDLER_MESSAGE *msg) {
    if(msg->what == MSG_ANIM) {
        float p; bool cont;
        if((cont = animator.update(p))) {
            auto v(dynamic_cast<zViewGroup*>(parent));
            if(v) cont = !v->scrolling((int)p);
        }
        if(cont) post(MSG_ANIM, duration);
    }
    zView::notifyEvent(msg);
}

void zFlyng::stop() {
    animator.clear();
    manager->eraseAllEventsView(this);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                                        SCROLLBAR                                                                       //
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

zViewScrollBar::zViewScrollBar(zView* group, bool _vert) : zView(styles_z_scrollbar, 0) {
    auto tl(drParams[DR_SCL].tiles); parent = group; vert = _vert;
    fade = (drParams[DR_SCL].type == -1 ? styles->_int(Z_SCROLLBAR_FADE, 0) : drParams[DR_SCL].type);
    size = (int)(drParams[DR_SCL].size == -1 ? styles->_int(Z_SCROLLBAR_SIZE, 2_dp) : drParams[DR_SCL].size);
    zView::onInit(false); if(tl != -1) drw[DRW_BK]->tile = tl;
}

void zViewScrollBar::notifyEvent(HANDLER_MESSAGE *msg) {
    if(msg->what == MSG_ANIM) {
        invalidate();
        if(updateStatus(ZS_VISIBLED, animator.update(alpha)))
            post(MSG_ANIM, duration);
    }
    zView::notifyEvent(msg);
}

void zViewScrollBar::awaken() {
    updateStatus(ZS_VISIBLED, true); alpha = 1.0f;
    drw[DRW_BK]->measure(rview.w, rview.h, 0, false);
    animator.init(3.0f, false);
    animator.add(0.2f, zInterpolator::LINEAR, 20);
    // запустить фейдинг
    if(fade) post(MSG_ANIM, duration, 0);
    invalidate();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                                        CARET                                                                           //
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

zViewCaret::zViewCaret() : zView(styles_z_caret, 0) {
    zView::onInit(false);
    drw[DRW_BK]->view = this;
}

void zViewCaret::notifyEvent(HANDLER_MESSAGE *msg) {
    if(msg->what == MSG_ANIM) {
        auto _parent(parent != nullptr); invalidate();
        updateStatus(ZS_VISIBLED, (animator.frame++ & _parent));
        if(parent) post(MSG_ANIM, duration);
    }
    zView::notifyEvent(msg);
}

void zViewCaret::update(zView* own, int x, int y, int h) {
    parent = own; updateStatus(ZS_VISIBLED, own != nullptr);
    if(own) {
        int italic(0);
        if(dynamic_cast<zViewEdit*>(own)) {
            italic = (int)(((zViewEdit*)own)->getTextStyle() & ZS_TEXT_ITALIC);
            setRotation(0, 0, (float)italic * 8.0f);
        }
        rview.set(x + own->rclient.x + italic, y + own->rclient.y, 2, h);
        drw[DRW_BK]->measure(4, h, PIVOT_ALL, false);
        animator.frame = 0;
    }
    post(MSG_ANIM, duration, 0);
}
