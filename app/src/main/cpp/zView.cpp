//
// Created by User on 18.04.2023.
//

#include "zssh/zCommon.h"
#include "zssh/zViewGroup.h"

// фейковый отображатель
zDrawableFake zView::fake;
zView* zView::fbo(nullptr);

zView::zView(zStyle* _styles, i32 _id) : styles(_styles), id(_id) {
    for(auto& d : drw) d = &fake;
}

zView::~zView() {
    for(auto& dr : drw) { if(dr->isValid()) SAFE_DELETE(dr); }
    // удалить из событий
    manager->eraseAllEventsView(this);
}

pti zView::applyGravity(crti& sizeParent, crti& sizeChild, u32 _gravity) {
    static int shift[4] = { 31, 31, 0, 1 };
    pti ret;
    int sw((int)(_gravity & ZS_GRAVITY_HORZ));
    ret.x = (sizeParent.w - sizeChild.w) >> shift[sw & 3];
    int sh((int)(_gravity & ZS_GRAVITY_VERT));
    ret.y = (sizeParent.h - sizeChild.h) >> shift[(sh >> 2) & 3];
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

void zView::onInit(bool _theme) {
    oldPos.set(INT_MAX, INT_MAX, 0, 0); zParamDrawable fk, bk;
    styles->enumerate([this, &bk, &fk, _theme](u32 attr) {
        auto v(&zTheme::value); auto val((int)v->u);
        attr |= _theme * ZTT_THM;
        switch(attr) {
            case Z_DURATION:			duration    = (u64)val; break;
            case Z_SCALE_X:				scale.w     = v->f; break;
            case Z_SCALE_Y:				scale.h     = v->f; break;
            case Z_ROTATE:				rot.z       = v->f; break;
            case Z_ROTATE_X:			rot.x       = v->f; break;
            case Z_ROTATE_Y:			rot.y       = v->f; break;
            case Z_TRANSLATE_X:   		trans.x     = v->f; break;
            case Z_TRANSLATE_Y:   		trans.y     = v->f; break;
            case Z_ALPHA:				alpha 	    = v->f; break;
            case Z_FBO:                 setDrawable(nullptr, DRW_FBO); break;
            case Z_BEHAVIOR:            updateStatus(ZS_BEHAVIOR_MASK, val, true); break;
            case Z_DISPLAY:             updateStatus(ZS_DISPLAY_MASK, val, true); break;
            case Z_TAP:			        updateStatus(ZS_TAP_MASK, val, true); break;
            case Z_GRAVITY:			    setGravity(val); break;
            case Z_SIZE:			    minMaxSize.set(val); break;
            case Z_IPADDING:		    ipad.set(val); break;
            case Z_PADDING:			    pad.set(val); break;
            case Z_MARGINS:             margin.set(val); break;
            case Z_FOREGROUND:		    fk.texture  = val; break;
            case Z_FOREGROUND_COLOR:	fk.color    = val; break;
            case Z_FOREGROUND_TILES:	fk.tiles    = val; break;
            case Z_FOREGROUND_SCALE:	fk.scale    = v->f; break;
            case Z_FOREGROUND_GRAVITY:  setForegroundGravity(val); fkGravity &= ~ZS_SCALE_MASK; fkGravity |= (val & ZS_SCALE_MASK); break;
            case Z_FOREGROUND_PADDING:  fk.padding  = val; break;
            case Z_BACKGROUND:		    bk.texture 	= val; break;
            case Z_BACKGROUND_COLOR:	bk.color	= val; break;
            case Z_BACKGROUND_TILES:	bk.tiles 	= val; break;
            case Z_BACKGROUND_PADDING:  bk.padding	= val; break;
        }
    });
    // базовые отображатели
    setDrawable(&bk, DRW_BK);
    setDrawable(&fk, DRW_FK);
    // ротация/масштаб
    setRotation(rot.x, rot.y, rot.z); setScale(scale.w, scale.h);
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
        manager->prepareRender(drwDebug.vertices, manager->screen, zMatrix::_identity);
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
            drw[DRW_FBO]->drawFBO(fbo, [this] {
                drw[DRW_BK]->draw(&rview);
                onDraw();
            });
            updateStatus(ZS_DIRTY_LAYER, !isFBO());
        }
        onDrawFBO();
        drawDebug();
        // каретка
        manager->drawCaret(this);
    }
}

void zView::eraseTouchHandler() {
    manager->touch.erase(this);
}

HANDLER_TOUCH_MESSAGE* zView::getTouchHandler(AInputEvent* event) {
    HANDLER_TOUCH_MESSAGE* touchObject;
    // берем из массива для нашего вида
    touchObject = manager->touch.get(this);
    if(!touchObject) {
        // проверяем на рект
        if(!zTouch::intersect(event, rclip))
            return nullptr;
        // если прошли проверку - получаем новый
        if(!(touchObject = manager->touch.obtain())) {
            ILOG("В системе больше нет объектов касания!!!");
            manager->touch.clear();
            return nullptr;
        }
        // инициализируем
        touchObject->view = this;
        // установка фокуса
        requestFocus();
    }
    return touchObject;
}

i32 zView::touchEvent(AInputEvent* event) {
    if(isVisibled()) {
        // инициируем объект касания
        auto touchChild(getTouchHandler(event));
        if(!touchChild) return TOUCH_CONTINUE;
        auto touch(&touchChild->touch);
        // проверить - это приемлемое касание?
        if(!touch->event(event, rclip)) return TOUCH_CONTINUE;
        // вызываем событие касания
        auto result(onTouchEvent(touch));
        // устанавливаем тап
        if(testFlags(ZS_TAP_MASK)) {
            updateStatus(ZS_TAP, touch->isCaptured());
            invalidate();
        }
        // если отпустили - стираем
        if(touch->isReleased()) eraseTouchHandler();
        return result;
    }
    return TOUCH_CONTINUE;
}

void zView::setStyles(zStyle* _styles) {
    if(styles != _styles) {
        styles = _styles;
        onInit(false);
        requestLayout();
    }
}

void zView::checked(bool _check) {
    updateStatus(ZS_CHECKED, _check);
    drw[DRW_FK]->setTileNum(_check)->measure(0, 0, 0, true);
    invalidate();
}

i32 zView::onTouchEvent(zTouch *touch) {
    if(!isTouchable()) {
        return TOUCH_CONTINUE;
    }
    if(onTouch) {
        auto ret(onTouch(this, touch));
        if(ret != TOUCH_CONTINUE) return ret;
    }
    if(isClickabled()) {
        if(touch->click()) {
            if(onClick) onClick(this, isLongClickabled() && touch->isLongClick());
            return TOUCH_ACTION;
        }
    }
    return touch->isCaptured();
}

void zView::requestLayout() {
    if(isVisibled()) {
        status &= ~ZS_MEASURE;
        if(parent && parent->testFlags(ZS_MEASURE))
            parent->requestLayout();
    }
}

void zView::invalidate() {
    if(isVisibled()) {
        status |= ZS_DIRTY_LAYER;
        if(parent && !parent->testFlags(ZS_DIRTY_LAYER))
            parent->invalidate();
    }
}

void zView::measure(cszm& spec) {
    auto changed(measureSpec.w != spec.w || measureSpec.h != spec.h);
    if(!testFlags(ZS_MEASURE) || changed) {
        status &= ~ZS_MEASURE_SET;
        onMeasure(spec);
        if(!testFlags(ZS_MEASURE_SET)) {
            DLOG("setMeasureDimension not called! Error!");
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
        if(drwDebug.count) {
            drwDebug.release();
        }
#endif
    }
}

void zView::defaultOnMeasure(cszm& spec, int width, int height) {
    auto ws(spec.w.size()), hs(spec.h.size());
    auto wm(spec.w.mode()), hm(spec.h.mode());
    if(ws && wm == MEASURE_EXACT) {
        width = ws;
    } else {
        width += pad.extent(false);
        width = z_max(minMaxSize.x, width);
        // коррекция по макс. ширине
        if(minMaxSize.y) width = z_min(minMaxSize.y, width);
        if(ws && wm == MEASURE_MOST) width = z_min(width, ws);
    }
    if(hs && hm == MEASURE_EXACT) {
        height = hs;
    } else {
        height += pad.extent(true);
        height = z_max(minMaxSize.w, height);
        // коррекция по макс. высоте
        if(minMaxSize.h) height = z_min(minMaxSize.h, height);
        if(hs && hm == MEASURE_MOST) height = z_min(height, hs);
    }
    setMeasuredDimension(width, height);
}

void zView::onLayout(crti &position, bool changed) {
    rview.x = position.x + margin.x; rview.y = position.y + margin.y;
    // учитывать внешний отступ при выравнивании
    rview += parent->applyGravity(position, rview + margin.size(), lps.gravity ? lps.gravity : parent->gravity);
    rview -= scroll;
    // вычислить позицию клиента
    rclient.x = rview.x + pad.x; rclient.y = rview.y + pad.y;
    rclip = z_clipRect(parent->rclip, rclient);
    if(changed) {
        // позиционирование базовых отображателей
        drw[DRW_BK]->measure(rview.w, rview.h, 3, false);
        drw[DRW_FBO]->measure(rview.w, rview.h, 3, false);
    }
}

void zView::setMeasuredDimension(int width, int height) {
    rview.w = width; rview.h = height;
    rclient.w = rview.w - pad.extent(false); rclient.h = rview.h - pad.extent(true);
    status |= ZS_MEASURE_SET;
}

bool zView::isFocus() const {
    return manager->isCheckFocus(this);
}

void zView::stateView(STATE &state, bool save, int &index) {
    if(save) {
        state.data += (status & ZS_STATES);
        state.data += (u32)animator.frame;
        state.data += (u32)(trans.x * 65535.0f);
        state.data += (u32)(trans.y * 65535.0f);
        state.data += (u32)(alpha * 65535.0f);
        state.data += (u32)(scale.w * 65535.0f);
        state.data += (u32)(scale.h * 65535.0f);
        state.data += (u32)(rot.x * 65535.0f);
        state.data += (u32)(rot.y * 65535.0f);
        state.data += (u32)(rot.z * 65535.0f);
        state.data += scroll.x; state.data += scroll.y;
    } else {
        status  &= ~ZS_STATES; status |= (state.data[index++] & ZS_STATES);
        animator.frame = state.data[index++];
        trans.x = (float)state.data[index++] / 65535.0f;
        trans.y = (float)state.data[index++] / 65535.0f;
        alpha   = (float)state.data[index++] / 65535.0f;
        setScale((float)state.data[index] / 65535.0f, (float)state.data[index + 1] / 65535.0f);
        setRotation((float)state.data[index + 2] / 65535.0f, (float)state.data[index + 3] / 65535.0f, (float)state.data[index + 4] / 65535.0f);
        scroll.x = (int)state.data[index + 5]; scroll.y = (int)state.data[index + 6];
        if(drw[DRW_FBO]->isValid()) status |= ZS_DIRTY_LAYER; index += 7;
    }
}

void zView::post(int what, u64 millis, int arg) {
    manager->event.send(this, what, millis, arg);
}

void zView::requestFocus() {
    if(isFocusable()) {
        manager->changeFocus(this);
    }
}

void zView::setAlpha(float _alpha) {
    alpha = _alpha;
    invalidate();
}

void zView::setScale(float _x, float _y) {
    // установка масштаба
    mtxScale.scale2(_x, _y, 1);
    mtx = mtxScale * mtxRot;
    scale.set(_x, _y);
    invalidate();
}

void zView::setRotation(float _x, float _y, float _z) {
    // установка угла
    rot.set(_x, _y, _z);
    mtx = mtxScale * mtxRot.rotate(deg2rad(_x), deg2rad(_y), deg2rad(_z));
    invalidate();
}

void zView::setTranslation(float _x, float _y) {
    trans.set(_x, _y, 0);
    invalidate();
}

void zView::_detach() {
    // убрать фокус
    if(isFocus()) manager->changeFocus(nullptr);
    // убрать тап
    updateStatus(ZS_TAP, ZS_TAP, false);
    // вызвать событие отсоединения
    onDetach();
    // удалить связанные события
    manager->eraseAllEventsView(this);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                                        GLOW                                                                            //
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

zViewGlow::zViewGlow(zView* group) : zView(styles_z_glow, 0) {
    parent = group; zView::onInit(false);
    updateStatus(ZS_VISIBLED, false);
    setOnAnimation([this](zView*, int) {
        float v; bool cont;
        if((cont = animator.update(v))) {
            auto vert(isVertical());
            // посчитать альфу
            alpha = v / 3.0f;
            // посчитать размер
            mtxScale.scale(vert ? 1 : v, vert ? v : 1, 1);
            mtx = mtxScale * mtxRot;
        } else {
            updateStatus(ZS_VISIBLED, false);
        }
        return cont;
    });
}

void zViewGlow::start(float _delta, bool _vert, bool _flow) {
    // параметры отображения
    if(isVisibled()) return;
    updateStatus(ZS_VISIBLED, true); vert = _vert;
    alpha = 0.0f; mtxRot.rotate(0, 0, _flow * deg2rad(180.0f)); mtxScale.identity();
    // базовый тайл
    drw[DRW_BK]->tile = _vert ? z.R.integer.horzGlow : z.R.integer.vertGlow;
    drw[DRW_BK]->measure(_vert * parent->rclient.w, !_vert * parent->rclient.h, !_vert + 1, false);
    // габариты и позиция
    rview.w = drw[DRW_BK]->bound.w; rview.h = drw[DRW_BK]->bound.h;
    rview[!_vert] = parent->rclient[!_vert];
    rview[_vert]  = parent->edges(_vert, _flow);
    mtx = mtxScale * mtxRot;
    // анимация
    _delta = fabs(_delta);
    animator.init(0.0f, false);
    animator.add(z_min(3.0f, _delta / 10.0f), zInterpolator::LINEAR, z_min<int>(8, _delta / 2));
    animator.add(0.0f, zInterpolator::EASEINCUBIC, 10);
    // запуск анимации
    post(MSG_ANIM, duration, 0);
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                                        SCROLLBAR                                                                       //
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

zViewScrollBar::zViewScrollBar(zView* group, bool _vert) : zView(styles_z_scrollbar, 0) {
    parent = group; vert = _vert;
    zView::onInit(false);
    size = styles->_int(Z_SCROLLBAR_SIZE, 4);
    fade = styles->_int(Z_SCROLLBAR_FADE, 0);
    setOnAnimation([this](zView*, int) { return updateStatus(ZS_VISIBLED, animator.update(alpha)); });
}

void zViewScrollBar::awaken() {
    updateStatus(ZS_VISIBLED, true);
    drw[DRW_BK]->measure(rview.w, rview.h, 0, false);
    animator.init(3.0f, false);
    animator.add(0.2f, zInterpolator::LINEAR, 20);
    // запустить фейдинг
    if(fade) post(MSG_ANIM, duration, 0);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                                        FLYNG                                                                           //
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

zFlyng::zFlyng(zView* group) : zView(styles_default, 0) {
    parent = group;
    setOnAnimation([this](zView*, int) {
        float p(0.0f);
        if(animator.update(p)) {
            ((zViewGroup*)parent)->scrolling(p * 5.0f);
            return 1;
        }
        updateStatus(ZS_VISIBLED, false);
        return 0;
    });
}

void zFlyng::start(float delta) {
    updateStatus(ZS_VISIBLED, true);
    animator.init(0, false);
    animator.add(delta * 2, zInterpolator::EASEINOUTCUBIC, delta);
    post(MSG_ANIM, duration, 0);
}

void zFlyng::stop() {
    animator.clear();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                                        CARET                                                                           //
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

zViewCaret::zViewCaret() : zView(styles_z_caret, 0) {
    zView::onInit(false);
    drw[DRW_BK]->view = this;
    drw[DRW_BK]->measure(3, 10, 0, false);
    setOnAnimation([this](zView*, int) {
        updateStatus(ZS_VISIBLED, (blink++ & 1));
        return drw[DRW_BK]->view != nullptr;
    });
}

void zViewCaret::update(zView* own, int x, int y, int h) {
    parent = own; blink = 0;
    updateStatus(ZS_VISIBLED, own != nullptr);
    if(own) {
        rview.set(x + own->rclient.x, y + own->rclient.y, 3, h);
        drw[DRW_BK]->bound = rview;
        setScale(1.0f, (float) h / 10.0f);
        post(MSG_ANIM, duration, 0);
    }
}
