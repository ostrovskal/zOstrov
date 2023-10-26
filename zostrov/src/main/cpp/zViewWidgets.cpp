//#pragma clang diagnostic push
//#pragma ide diagnostic ignored "hicpp-multiway-paths-covered"

#include "zostrov/zCommon.h"
#include "zostrov/zViewWidgets.h"

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                                                BUTTON                                                                  //
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

zViewButton::zViewButton(zStyle *_styles, i32 _id, u32 _text, i32 _icon) : zViewText(_styles, _id, _text), icon(_icon) {
    minMaxSize.set(z_dp(z.R.dimen.butMinWidth), 0, z_dp(z.R.dimen.butMinHeight), 0);
}

void zViewButton::onInit(bool _theme) {
    zViewText::onInit(_theme);
    fkGravity = (fkGravity & ZS_SCALE_MASK) | ZS_GRAVITY_CENTER;
    auto ic(icon); icon = -1; setIcon(ic);
}

void zViewButton::onLayout(crti &position, bool changed) {
    zViewText::onLayout(position, changed);
    if(drw[DRW_ICON]->isValid()) {
        auto bound(&drw[DRW_ICON]->bound);
        drw[DRW_ICON]->measure(icSize.w, icSize.h, 0, false);
        *bound = applyGravity(rclient - szi(ipad.extent(false), ipad.extent(true)), icSize, icGravity);
        *bound += rclient; *bound += ipad;
    }
}

void zViewButton::setIcon(i32 _icon) {
    if(icon != _icon) {
        icon = _icon; drw[DRW_ICON]->tile = _icon;
        requestPosition();
    }
}

void zViewButton::onDraw() {
    zViewText::onDraw();
    drw[DRW_ICON]->draw(nullptr);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                                             CHECKBOX                                                                   //
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

zViewCheck::zViewCheck(zStyle *_styles, i32 _id, u32 _text) : zViewText(_styles, _id, _text) {
    minMaxSize.set(z_dp(z.R.dimen.checkMinWidth), 0, z_dp(z.R.dimen.checkMinHeight), 0);
}

i32 zViewCheck::onTouchEvent() {
    i32 ret; if((ret = zViewText::onTouchEvent()) == TOUCH_ACTION) checked(!isChecked());
    return ret;
}

void zViewCheck::onInit(bool _theme) {
    zViewText::onInit(_theme);
    checked((status & ZS_CHECKED) != 0);
}

void zViewCheck::stateView(STATE &state, bool save, int &index) {
    if(save) {
        state.data += (status & ZS_CHECKED);
    } else {
        status |= state.data[index++] | ZS_DIRTY_LAYER;
        checked(testFlags(ZS_CHECKED));
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                                               RADIO                                                                    //
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

zViewRadio::zViewRadio(zStyle *_styles, i32 _id, u32 _text, u32 _group) : zViewCheck(_styles, _id, _text), group(_group) {
    minMaxSize.set(z_dp(z.R.dimen.radioMinWidth), 0, z_dp(z.R.dimen.radioMinHeight), 0);
}

i32 zViewRadio::onTouchEvent() {
    i32 ret;
    if((ret = zViewCheck::onTouchEvent()) == TOUCH_ACTION) checked(true);
    return ret;
}

void zViewRadio::checked(bool set) {
    auto grp(getParent());
    if(grp && set) {
        for (int i = 0; i < grp->countChildren(); i++) {
            auto v(grp->atView<zViewRadio>(i));
            if(v && v->group == group) {
                v->checked(false);
            }
        }
    }
    zView::checked(set);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                                               SWITCH                                                                   //
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

zViewSwitch::zViewSwitch(zStyle* _styles, i32 _id, u32 _text) : zViewCheck(_styles, _id, _text) {
    trumb = new zDrawable(this, 0);
    minMaxSize.set(z_dp(z.R.dimen.switchMinWidth), 0, z_dp(z.R.dimen.switchMinHeight), 0);
}

void zViewSwitch::notifyEvent(HANDLER_MESSAGE *msg) {
    if(msg->what == MSG_ANIM) {
        float pos; auto visible(animator.update(pos));
        trumb->bound.x = z_round((float)drw[DRW_FK]->bound.x + ((float)fkW / 8.0f) * pos);
        invalidate(); if(visible) post(MSG_ANIM, duration);
    }
    zViewText::notifyEvent(msg);
}

void zViewSwitch::checked(bool set) {
    // применить checked
    if(isChecked() != set) {
        animator.clear();
        animator.init((float)(!set * 4), false);
        animator.add((float)(set * 4), zInterpolator::LINEAR, 4);
        updateStatus(ZS_CHECKED, set);
        post(MSG_ANIM, duration, 0);
    }
}

void zViewSwitch::onLayout(crti &position, bool changed) {
    zViewText::onLayout(position, changed);
    auto bound(&drw[DRW_FK]->bound);
    trumb->measure(fkW, fkH, 0, false);
    trumb->bound.x = z_round((float)bound->x + ((float)fkW / 8.0f) * (float)(isChecked() * 4));
    trumb->bound.y = bound->y;
}

void zViewSwitch::onDraw() {
    zViewText::onDraw();
    trumb->draw(nullptr);
}

void zViewSwitch::onInit(bool _theme) {
    zViewCheck::onInit(_theme);
    // инициализируем отрисовшик ползунка
    trumb->init(drw[DRW_FK], drw[DRW_FK]->getTileNum(1));
}

void zViewSwitch::stateView(zView::STATE &state, bool save, int &index) {
    zViewCheck::stateView(state, save, index);
    if(!save) {
        auto check(testFlags(ZS_CHECKED));
        updateStatus(ZS_CHECKED, !check);
        checked(check);
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                                             СЛАЙДЕР                                                                    //
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

zViewSlider::zViewSlider(zStyle* _styles, i32 _id, u32 _text, cszi& _range, int _pos, bool _vert) :
        zViewText(_styles, _id, 0), range(_range), pos(_pos) {
    trumb = new zDrawable(this, 0); vert = _vert;
    minMaxSize.set(z_dp(z.R.dimen.sliderMinWidth), z_dp(z.R.dimen.sliderMaxWidth),
                   z_dp(z.R.dimen.sliderMinHeight), z_dp(z.R.dimen.sliderMaxHeight));
    if(_text > 0) tips = theme->findString(_text) + " ";
}

void zViewSlider::notifyEvent(HANDLER_MESSAGE *msg) {
    if(msg->what == MSG_ANIM) {
        if(testFlags(ZS_DRAWING) && isEnabled()) {
            float value; static zMatrix rot;
            animator.update(value);
            if(mode & ZS_SLIDER_ROTATE) rot.rotate(0, 0, deg2rad(value * 360.0f)); else rot.identity();
            int v(-(int)trunc(value * (float)drw[DRW_FK]->bound[vert + 2]));
            drw[DRW_FK]->bound[vert] = posTrack + (v % drw[DRW_FK]->bound[vert + 2]);
            if(value >= 0.5f) value = 1.0f - value; value += 0.5f;
            if(mode & ZS_SLIDER_SCALE) mtxTrumb.scale(value, value, 0.0f); else mtxTrumb.identity();
            mtxTrumb = rot * mtxTrumb; invalidate();
            updateStatus(ZS_DRAWING, false);
        }
        post(MSG_ANIM, duration);
    } else if(msg->what == MSG_SELECTED && onChangeSelected) {
        onChangeSelected(this, msg->arg1);
    }
    zViewText::notifyEvent(msg);
}

bool zViewSlider::setProgress(i32 _pos) {
    _pos = range.clamp(_pos);
    auto ret(_pos != pos);
    if(ret) {
        pos = _pos;
        post(MSG_SELECTED, 50, _pos);
        updateTrumb(); invalidate();
    }
    return ret;
}

void zViewSlider::setRange(cszi& r) {
    if(r.w != range.w || r.h != range.h) {
        range = r; if(range.interval() == 0) ++range.h;
        requestPosition();
    }
}

void zViewSlider::onInit(bool _theme) {
    zViewText::onInit(_theme);
    // режим
    mode = styles->_int(Z_MODE, 0);
    speedTrack = styles->_float(Z_SPEED_TRACK, 0);
    // установить величину прогресса
    auto _pos(pos); pos = -1; setProgress(_pos);
    // инициализировать ползунок
    trumb->init(drw[DRW_FK], drw[DRW_FK]->getTileNum(1));
    animator.init(0.0f, true);
    animator.add(1.0f, zInterpolator::LINEAR, z_round(12.0f * speedTrack));
}

void zViewSlider::showTips() {
    if(isTips()) setText(z_fmt8("%s%i", tips.str(), pos), false);
}

void zViewSlider::onDraw() {
    // нарисовать трек
    static zMatrix r;
    auto clip(drawableClip());
    updateStatus(ZS_DRAWING, clip.isNotEmpty());
    ptf offs;
    auto _clip(z_clipRect(parent->rclip, rclient.padding(!vert * sizeTrumb2, vert * sizeTrumb2)));
    if(_clip.isNotEmpty()) {
        auto w(drw[DRW_FK]->bound.w); offs = drw[DRW_FK]->offsetBound();
        if(vert) r.rotate(0, 0, deg2rad(90)); else r.identity();
        for(int i = 0; i < segments; i++) {
            drw[DRW_FK]->drawCommon(_clip, r * m.translate(offs.x, offs.y, 0.0f), true);
            offs[vert] += (float)w;
        }
    }
    // нарисовать текст
    zViewText::drawText();
    // нарисовать ползунок
    offs = trumb->offsetBound();
    trumb->drawCommon(clip, mtxTrumb * m.translate(offs.x, offs.y, 0.0f), true);
}

void zViewSlider::onMeasure(cszm& spec) {
    auto minMax(minMaxSize);
    if(vert) {
        std::swap(minMaxSize.x, minMaxSize.w);
        std::swap(minMaxSize.y, minMaxSize.h);
    }
    zViewText::onMeasure(spec);
    minMaxSize = minMax;
}

void zViewSlider::onLayout(crti &position, bool changed) {
    zViewText::onLayout(position, changed);
    updateLayout(changed);
}

void zViewSlider::updateLayout(bool changed) {
    auto bound(&drw[DRW_FK]->bound);
    // размер ползунка
    sizeTrumb = z_min(z_min(minMaxSize[1 + vert * 2], rclient.w), z_min(minMaxSize[3 - vert * 2], rclient.h)); sizeTrumb2 = sizeTrumb / 2;
    // длина трэка(в зависимости от ориентации)
    auto lenTrack(rclient[vert + 2]);
    // количество сегментов трека
    segments = z_round((float)lenTrack / (float)sizeTrumb) + 2;
    // пересоздать трек
    drw[DRW_FK]->measure(sizeTrumb, sizeTrumb, PIVOT_ALL, false);
    // дельта ползунка
    delta = (float)(lenTrack - sizeTrumb) / (float)range.interval();
    // cформировать ползунок
    trumb->measure(sizeTrumb, sizeTrumb, PIVOT_ALL, false);
    *bound = rclient.xy();
    (*bound)[1 - vert] += applyGravity(rclient, szi(sizeTrumb, sizeTrumb), fkGravity)[!vert];
    posTrack = (*bound)[vert];
    updateTrumb();
    if(speedTrack != 0 || mode) {
        post(MSG_ANIM, duration, 0);
    }
}

i32 zViewSlider::onTouchEvent() {
    auto ret(zViewText::onTouchEvent());
    if(ret == TOUCH_FINISH) {
        //вычислить ближайшую позицию, исходя из диапазона слайдера
        auto dir((int)touch->cpt[vert] - sizeTrumb2 - rclient[vert]);
        // установить новый прогресс
        setProgress(range.w + (int)round((float)dir / delta));
        ret = TOUCH_ACTION;
    }
    return ret;
}

void zViewSlider::updateTrumb() {
    trumb->bound.set(rclient.x, rclient.y, sizeTrumb, sizeTrumb);
    trumb->bound[vert] += z_round((float)(pos - range.w) * delta);
    trumb->bound[1 - vert] += applyGravity(rclient, szi(sizeTrumb, sizeTrumb), fkGravity)[!vert];
    showTips();
}

void zViewSlider::stateView(STATE &state, bool save, int &index) {
    zViewText::stateView(state, save, index);
    if(save) {
        state.data += (u32)pos;
    } else {
        setProgress((int)state.data[index++]);
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                                             ПРОГРЕСС                                                                   //
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

zViewProgress::zViewProgress(zStyle *_styles, i32 _id, u32 _text, cszi &_range, int _pos, bool vert) :
                            zViewSlider(_styles, _id, _text, _range, _pos, vert) {
    status &= ~ZS_CLICKABLE;
    minMaxSize.set(z_dp(z.R.dimen.progressMinWidth), z_dp(z.R.dimen.progressMaxWidth),
                   z_dp(z.R.dimen.progressMinHeight), z_dp(z.R.dimen.progressMaxHeight));
}

void zViewProgress::onDraw() {
    // фон
    if(mode != ZS_SLIDER_ROTATE)
        drw[DRW_FK]->draw(nullptr);
    // ползунок
    auto offs(trumb->offsetBound());
    trumb->drawCommon(rclip, mtxTrumb * m.translate(offs.x, offs.y, 0.0f), true);
    // текст
    drawText(); updateStatus(ZS_DRAWING, rclip.isNotEmpty());
}

bool zViewProgress::setProgress(int _pos) {
    auto ret(zViewSlider::setProgress(_pos));
    if(ret) requestPosition();
    return ret;
}

void zViewProgress::onInit(bool _theme) {
    zViewSlider::onInit(_theme);
    trumb->color.set(styles->_int(Z_PRIMARY, 0xffffffff));
}

void zViewProgress::updateLayout(bool changed) {
    updateTrumb();
    if(mode != ZS_SLIDER_ROTATE) {
        manager->eraseAllEventsView(this);
    }
}

void zViewProgress::updateTrumb() {
    if(mode != ZS_SLIDER_ROTATE) {
        delta = (float)rclient[vert + 2] / (float)range.interval();
        sizeTrumb = z_min(z_min(minMaxSize[1 + vert * 2], rclient.w), z_min(minMaxSize[3 - vert * 2], rclient.h));
        auto v(z_round((float)pos * delta));
        auto pt(applyGravity(rclient, szi(sizeTrumb, sizeTrumb), fkGravity)[!vert]);
        trumb->measure(vert ? sizeTrumb : v, vert ? v : sizeTrumb, PIVOT_ALL, false);
        drw[DRW_FK]->measure(vert ? sizeTrumb : rclient.w, vert ? rclient.h : sizeTrumb , PIVOT_ALL, false);
        drw[DRW_FK]->bound = rclient.xy(); trumb->bound = rclient.xy();
        trumb->bound[1 - vert] += pt; drw[DRW_FK]->bound[1 - vert] += pt;
    } else {
        sizeTrumb = z_min(z_min(z_dp(z.R.dimen.waitMaxWidth), rclient.w),
                          z_min(z_dp(z.R.dimen.waitMaxHeight), rclient.h));
        auto center(rclient.center()); sizeTrumb2 = sizeTrumb / 2;
        trumb->measure(sizeTrumb, sizeTrumb, PIVOT_ALL, false);
        trumb->bound.x = center.x - sizeTrumb2; trumb->bound.y = center.y - sizeTrumb2;
        post(MSG_ANIM, duration, 0);
    }
    showTips();
}

void zViewProgress::showTips() {
    if(isTips()) {
        auto _pos(z_round((float)pos / (float)range.interval() * 100.0f));
        setText(z_fmt8("%s%i%%", tips.str(), _pos), false);
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                                              КОНТРОЛЛЕР                                                                //
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

zViewController::zViewController(zStyle *_styles, i32 _id, i32 _base, u32 _fileMap) : zFrameLayout(_styles, _id), base(_base) {
    minMaxSize.set(z_dp(z.R.dimen.controllerMinWidth), 0, z_dp(z.R.dimen.controllerMinHeight), 0);
    int size(0); auto ptr(manager->assetFile(theme->findString(_fileMap), &size));
    if(!ptr) return;
    zXml xml(ptr, size);
    if(xml.isValid()) {
        auto root(xml.getRoot());
        if(root->getName() == "controller") {
            // габариты кнопок + массив декораций
            auto keys(root->getTag("keys"));
            if(keys) {
                auto count(keys->getCountNodes());
                for(int i = 0 ; i < count; i++) {
                    auto k(keys->getTag(i));
                    if(k->getName() != "key") continue;
                    decors += new DECORATE(
                            z_ston(k->getAttrVal("x", "0"), RADIX_DEC),
                            z_ston(k->getAttrVal("y", "0"), RADIX_DEC),
                            z_ston(k->getAttrVal("w", "32"), RADIX_DEC),
                            z_ston(k->getAttrVal("h", "32"), RADIX_DEC));
                }
            }
            // карта контроллера
            auto maps(root->getTag("map"));
            if(maps) {
                auto count(maps->getCountNodes() / 2);
                for(int i = 0; i < count; i++) {
                    auto s1(maps->getTag(i * 2)->getVal()), s2(maps->getTag(i * 2 + 1)->getVal());
                    if(!map) {
                        cellDebug.set(s1.count() / 2, count);
                        map = new u8[count * cellDebug.w];
                    }
                    for(int j = 0; j < cellDebug.w; j++) {
                        u8 bits(0);
                        auto _11(s1[j * 2]), _12(s1[j * 2 + 1]);
                        auto _21(s2[j * 2]), _22(s2[j * 2 + 1]);
                        if(_11 != '_') bits |= 1 << (_11 - '0');
                        if(_12 != '_') bits |= 1 << (_12 - '0');
                        if(_21 != '_') bits |= 1 << (_21 - '0');
                        if(_22 != '_') bits |= 1 << (_22 - '0');
                        map[i * cellDebug.w + j] = bits;
                    }
                }
            }
            setSize(szi(lps.w, lps.h));
        }
    }
    for(auto& d : dr) d = new zDrawable(this, DRW_FK);
}

zViewController::~zViewController() {
    SAFE_DELETE(map);
//    SAFE_DELETE(button);
    for(auto& d : dr) SAFE_DELETE(d);
}

i32 zViewController::onTouchEvent() {
    auto but(buttons);
    if(touch->isCaptured()) {
        auto x((int)((touch->cpt.x - (float)rview.x) * sizeReverseMap.w));
        auto y((int)((touch->cpt.y - (float)rview.y) * sizeReverseMap.h));
        if(x >= 0 && y >= 0 && x < cellDebug.w && y < cellDebug.h) but = map[y * cellDebug.w + x];
    } else if(touch->isReleased()) but = 0;
    if(but != buttons) {
        buttons = but;
        // вызываем событие
        if(onChangeButton) onChangeButton(this, buttons);
        invalidate(); return TOUCH_STOP;
    }
    return TOUCH_CONTINUE;
}

void zViewController::setDecorateKey(int idx, cstr text, int icon) {
    if(idx >= 0 && idx < decors.size()) {
        decors[idx]->icon = icon;
        decors[idx]->text = text;
    }
    invalidate();
}

void zViewController::onDraw() {
    // базовый тайл
    auto but(buttons);
    drw[DRW_FK]->draw(&rview);
    // нарисовать надписи/иконки
    float xx((float)rview.w / 128.0f), yy((float)rview.h / 128.0f);
    button->updateStatus(ZS_VISIBLED, true);
    for(int i = 0 ; i < decors.size(); i++) {
        auto& d(decors[i]); auto pressed((but & 1) << 1);
        auto color(pressed ? z.R.color.red : z.R.color.white);
        if(pressed) dr[i]->draw(&rview);
        auto r(d->rect); r.offset(pressed, pressed);
        r.x = rview.x + z_round((float)r.x * xx); r.w = z_round((float)r.w * xx);
        r.y = rview.y + z_round((float)r.y * yy); r.h = z_round((float)r.h * yy);
        szm spec(zMeasure(MEASURE_EXACT, r.w), zMeasure(MEASURE_EXACT, r.h));
        button->setIcon(d->icon); button->setTextSpecial(d->text, spec);
        button->setTextColorForeground(color);
        button->setTextColorIcon(color);
        button->setTextSize(z_round((float)22_dp * xx));
        button->layout(r); button->draw(); but >>= 1;
    }
    button->updateStatus(ZS_VISIBLED, false);
}

void zViewController::onInit(bool _theme) {
    zViewGroup::onInit(_theme);
    removeAllViews(false);
    // основной текст
    attach(button = new zViewButton(styles_z_butcontroller, 0, 0), 100, 100);
}

void zViewController::requestLayout() {
    if(countChildren() && !atView(0)->isVisibled())
        zViewGroup::requestLayout();
}

void zViewController::onLayout(crti &position, bool changed) {
    zFrameLayout::onLayout(position, changed);
    auto width(rview.w), height(rview.h); rclient = rview;
    sizeReverseMap.w = (float)cellDebug.w / (float)width;
    sizeReverseMap.h = (float)cellDebug.h / (float)height;
    auto fk(drw[DRW_FK]); fk->tile = base;
    fk->measure(width, height, 0, false);
    for(int i = 0 ; i < 4; i++) {
        auto d(dr[i]); d->init(fk, 0); d->tiles = fk->tiles;
        d->setTileNum(i); d->measure(width, height, 0, false);
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                                             ИЗОБРАЖЕНИЕ                                                                //
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

zViewImage::zViewImage(zStyle* _styles, i32 _id, i32 _image) : zViewText(_styles, _id, 0), image(_image) {
    minMaxSize.set(z_dp(z.R.dimen.imageMinWidth), 0, z_dp(z.R.dimen.imageMinHeight), 0);
}

void zViewImage::setTexture(zTexture* tex, int _image) {
    manager->cache->get(tex->name, drw[DRW_FK]->texture);
    setImage(_image);
}

void zViewImage::setImage(i32 _image) {
    if(image != _image) {
        image = _image; drw[DRW_FK]->tile = _image;
        requestPosition();
    }
}

void zViewImage::onInit(bool _theme) {
    zViewText::onInit(_theme);
    fkGravity = ZS_GRAVITY_CENTER;
    auto img(image); image = -1;
    setImage(img);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                                                 ТОСТ                                                                   //
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

zViewToast::zViewToast(cstr _text, zStyle* styles) : zViewText(styles ? styles : styles_z_toast, z.R.id.toast, 0), text(_text) {
    minMaxSize.set(z_dp(z.R.dimen.toastMinWidth), 0, z_dp(z.R.dimen.toastMinHeight), 0);
    // удалить старый
    auto common(manager->getSystemView(true));
    common->remove(common->idView(z.R.id.toast));
    animator.init(0.0f, false);
    animator.add(5.0f, zInterpolator::LINEAR, 128);
    animator.add(0.0f, zInterpolator::LINEAR, 64);
    // добавить новый
    common->attach(this, ZS_GRAVITY_HCENTER | ZS_GRAVITY_BOTTOM, 0, VIEW_WRAP, VIEW_WRAP);
}

void zViewToast::onInit(bool _theme) {
    zViewText::onInit(_theme);
    setAlpha(0); setText(text, true);
    post(MSG_ANIM, duration, 0);
}

void zViewToast::notifyEvent(HANDLER_MESSAGE *msg) {
    if(msg->what == MSG_ANIM) {
        invalidate();
        if(!animator.update(alpha)) {
            manager->getSystemView(true)->remove(this);
        } else {
            post(MSG_ANIM, duration);
        }
    }
    zViewText::notifyEvent(msg);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                                              ПОВЕРХНОСТЬ                                                               //
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void zViewSurface::onInit(bool _theme) {
    zFrameLayout::onInit(_theme);
    auto texture(new zTexture());
    texture->name = z_guid8();
    texture->makeTexture(size.w, size.h, GL_TEXTURE_2D);
    drw[DRW_FK]->texture = manager->cache->add(texture, drw[DRW_FK]->texture);
}

void zViewSurface::onLayout(crti& position, bool changed) {
    zFrameLayout::onLayout(position, changed);
    drw[DRW_FK]->measure(rclient.w, rclient.h, PIVOT_ALL, false);
}

void zViewSurface::onDraw() {
    glBindTexture(GL_TEXTURE_2D, drw[DRW_FK]->texture->id);
    updateTexture();
    drw[DRW_FK]->draw(&rclient);
    zFrameLayout::onDraw();
}
