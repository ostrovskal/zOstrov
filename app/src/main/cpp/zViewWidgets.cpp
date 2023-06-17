//#pragma clang diagnostic push
//#pragma ide diagnostic ignored "hicpp-multiway-paths-covered"

#include "zssh/zCommon.h"
#include "zssh/zViewWidgets.h"

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                                                BUTTON                                                                  //
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

zViewButton::zViewButton(zStyle *_styles, i32 _id, u32 _text, i32 _icon) : zViewText(_styles, _id, _text), icon(_icon) {
    minMaxSize.set(z_dp(z.R.dimen.butMinWidth), 0, z_dp(z.R.dimen.butMinHeight), 0);
}

void zViewButton::onInit(bool _theme) {
    zViewText::onInit(_theme);
    fkGravity = (fkGravity & ZS_SCALE_MASK) | ZS_GRAVITY_CENTER;
    setIcon(icon);
}

void zViewButton::onLayout(crti &position, bool changed) {
    zViewText::onLayout(position, changed);
    if(changed && drw[DRW_ICON]->isValid()) {
        auto bound(&drw[DRW_ICON]->bound);
        drw[DRW_ICON]->measure(icSize.w, icSize.h, 0, false);
        *bound = applyGravity(rclient - szi(ipad.extent(false), ipad.extent(true)), icSize, icGravity);
        *bound += rclient; *bound += ipad;
    }
}

void zViewButton::setIcon(i32 _icon) {
    if(drw[DRW_ICON]->tile != _icon) {
        drw[DRW_ICON]->tile = _icon;
        requestLayout();
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

i32 zViewCheck::onTouchEvent(zTouch *touch) {
    i32 ret;
    if((ret = zView::onTouchEvent(touch)) == TOUCH_ACTION) checked(!isChecked());
    return ret;
}

void zViewCheck::onInit(bool _theme) {
    zViewText::onInit(_theme);
    checked((status & ZS_CHECKED) != 0);
}

void zViewCheck::stateView(STATE &state, bool save, int &index) {
    zViewText::stateView(state, save, index);
    if(!save) checked(testFlags(ZS_CHECKED));
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                                               RADIO                                                                    //
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

zViewRadio::zViewRadio(zStyle *_styles, i32 _id, u32 _text, u32 _group) : zViewCheck(_styles, _id, _text), group(_group) {
    minMaxSize.set(z_dp(z.R.dimen.radioMinWidth), 0, z_dp(z.R.dimen.radioMinHeight), 0);
}

i32 zViewRadio::onTouchEvent(zTouch *touch) {
    i32 ret;
    if((ret = zViewCheck::onTouchEvent(touch)) == TOUCH_ACTION) checked(true);
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
    setOnAnimation([this](zView*, int) {
        float pos; auto visible(animator.update(pos));
        trumb->bound.x = drw[DRW_FK]->bound.x + ((float)fkW / 8.0f) * pos;
        invalidate();
        return visible;
    });
}

void zViewSwitch::checked(bool set) {
    // применить checked
    if(isChecked() != set) {
        animator.clear();
        animator.init(!set * 4.0f, false);
        animator.add(set * 4.0f, zInterpolator::LINEAR, 4);
        updateStatus(ZS_CHECKED, set);
        post(MSG_ANIM, duration, 0);
    }
}

void zViewSwitch::onLayout(crti &position, bool changed) {
    zViewText::onLayout(position, changed);
    if(changed) {
        auto bound(&drw[DRW_FK]->bound);
        trumb->measure(fkW, fkH, 0, false);
        trumb->bound.x = bound->x + ((float) fkW / 8.0f) * (isChecked() * 4);
        trumb->bound.y = bound->y;
    }
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
        updateStatus(ZS_CHECKED, !check); checked(check);
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
    setOnAnimation([this](zView*, int) {
        if(isDraw) {
            float value; static zMatrix rot;
            animator.update(value);
            if(mode & ZS_SLIDER_ROTATE) rot.rotate(0, 0, deg2rad(value * 360.0f)); else rot.identity();
            int v(-(int)trunc(value * drw[DRW_FK]->bound[vert + 2]));
            drw[DRW_FK]->bound[vert] = posTrack + (v % drw[DRW_FK]->bound[vert + 2]);
            if(value >= 0.5f) value = 1.0f - value; value += 0.5f;
            if(mode & ZS_SLIDER_SCALE) mtxTrumb.scale(value, value, 0.0f); else mtxTrumb.identity();
            mtxTrumb = rot * mtxTrumb;
            invalidate(); isDraw = false;
        }
        return 1;
    });
}

bool zViewSlider::setProgress(i32 _pos) {
    _pos = range.clamp(_pos);
    auto ret(_pos != pos);
    if(ret) { pos = _pos; updateTrumb(); showTips(); }
    return ret;
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
    animator.add(1.0f, zInterpolator::LINEAR, (int)roundf(12 * speedTrack));
    if(speedTrack || mode) post(MSG_ANIM, duration, 0);
}

void zViewSlider::showTips() {
    if(isTips()) setText(zStringUTF8(z_fmt("%s%i", tips.str(), pos).str()), false);
}

void zViewSlider::onDraw() {
    // нарисовать трек
    static zMatrix m, r;
    auto clip(drawableClip());
    if(clip.isNotEmpty()) {
        ptf offs;
        auto _clip(z_clipRect(parent->rclip, rclient.padding(!vert * sizeTrumb2, vert * sizeTrumb2)));
        if(_clip.isNotEmpty()) {
            auto w(drw[DRW_FK]->bound.w); offs = drw[DRW_FK]->offsetBound();
            if(vert) r.rotate(0, 0, deg2rad(90)); else r.identity();
            for(int i = 0; i < segments; i++) {
                drw[DRW_FK]->drawCommon(_clip, r * m.translate(offs.x, offs.y, 0.0f), true);
                offs[vert] += w;
            }
        }
        // нарисовать текст
        zViewText::drawText();
        // нарисовать ползунок
        offs = trumb->offsetBound();
        trumb->drawCommon(clip, mtxTrumb * m.translate(offs.x, offs.y, 0.0f), true);
        isDraw = true;
    }
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
    auto xy(drw[DRW_FK]->bound[vert]);
    zViewText::onLayout(position, changed);
    updateLayout(changed);
    drw[DRW_FK]->bound[vert] = xy;
}

void zViewSlider::updateLayout(bool changed) {
    if(changed) {
        // размер ползунка
        sizeTrumb = z_min(z_min(minMaxSize[1 + vert * 2], rclient.w), z_min(minMaxSize[3 - vert * 2], rclient.h)); sizeTrumb2 = sizeTrumb / 2;
        // длина трэка(в зависимости от ориентации)
        auto lenTrack(rclient[vert + 2]);
        // количество сегментов трека
        segments = (int)roundf((float)lenTrack / (float)sizeTrumb) + 2;
        // пересоздать трек
        drw[DRW_FK]->measure(sizeTrumb, sizeTrumb, 3, false);
        auto bound(&drw[DRW_FK]->bound);
        *bound = rclient.xy();
        (*bound)[1 - vert] += applyGravity(rclient, szi(sizeTrumb, sizeTrumb), fkGravity)[!vert];
        posTrack = (*bound)[vert];
        // дельта ползунка
        delta = (float) (lenTrack - sizeTrumb) / (float) range.interval();
        // cформировать ползунок
        trumb->measure(sizeTrumb, sizeTrumb, 3, false);
    }
    updateTrumb();
}

i32 zViewSlider::onTouchEvent(zTouch *touch) {
    auto ret(touch->isCaptured());
    if(ret) {
        //вычислить ближайшую позицию, исходя из диапазона слайдера
        auto dir((int)touch->cpt[vert] - sizeTrumb2 - rclient[vert]);
        // установить новый прогресс
        setProgress(range.w + (int)round((float)dir / delta));
        // обновить позицию
        updateTrumb();
        ret = TOUCH_ACTION;
    }
    return ret;
}

void zViewSlider::updateTrumb() {
    trumb->bound.set(rclient.x, rclient.y, sizeTrumb, sizeTrumb);
    trumb->bound[vert] += (int)roundf((float)(pos - range.w) * delta);
    trumb->bound[1 - vert] += applyGravity(rclient, szi(sizeTrumb, sizeTrumb), fkGravity)[!vert];
    invalidate();
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
    static zMatrix m;
    // фон
    if(!mode) drw[DRW_FK]->draw(nullptr);
    // ползунок
    auto offs(trumb->offsetBound());
    trumb->drawCommon(rclip, mtxTrumb * m.translate(offs.x, offs.y, 0.0f), true);
    // текст
    drawText();
}

void zViewProgress::onInit(bool _theme) {
    zViewSlider::onInit(_theme);
    trumb->color.set(styles->_int(Z_PRIMARY, 0xffa00000));
    if(mode == ZS_SLIDER_ROTATE) post(MSG_ANIM, duration, 0);
}

void zViewProgress::updateLayout(bool changed) {
    if(changed) {
        delta = (float)rclient[vert + 2] / (float)range.interval();
        sizeTrumb = z_min(z_min(minMaxSize[1 + vert * 2], rclient.w), z_min(minMaxSize[3 - vert * 2], rclient.h)); sizeTrumb2 = sizeTrumb / 2;
        if(mode == ZS_SLIDER_ROTATE) {
            auto center(rclient.center());
            trumb->measure(sizeTrumb, sizeTrumb, 3, false);
            trumb->bound.x = center.x - sizeTrumb2; trumb->bound.y = center.y - sizeTrumb2;
        } else updateTrumb();
    }
}

void zViewProgress::updateTrumb() {
    if(mode != ZS_SLIDER_ROTATE) {
        auto v((int)roundf((float)pos * delta));
        auto pt(applyGravity(rclient, szi(sizeTrumb, sizeTrumb), fkGravity)[!vert]);
        trumb->measure(vert ? sizeTrumb : v, vert ? v : sizeTrumb, PIVOT_ALL, false);
        drw[DRW_FK]->measure(vert ? sizeTrumb : rclient.w, vert ? rclient.h : sizeTrumb , PIVOT_ALL, false);
        drw[DRW_FK]->bound = rclient.xy(); trumb->bound = rclient.xy();
        trumb->bound[1 - vert] += pt; drw[DRW_FK]->bound[1 - vert] += pt;
        invalidate();
    }
}

void zViewProgress::showTips() {
    if(isTips()) {
        auto _pos((int)round((float)pos / (float)range.interval() * 100.0f));
        setText(zStringUTF8(z_fmt("%s%i%%", tips.str(), _pos).str()), false);
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                                              КОНТРОЛЛЕР                                                                //
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

zViewController::zViewController(zStyle *_styles, i32 _id, i32 _base, u32 _fileMap) : zView(_styles, _id), base(_base) {
    minMaxSize.set(z_dp(z.R.dimen.controllerMinWidth), 0, z_dp(z.R.dimen.controllerMinHeight), 0);
    int size(0); auto ptr(manager->assetFile(theme->findString(_fileMap), &size));
    if(!ptr) return;
    zXml xml(ptr, size);
    if(xml.isValid()) {
        auto root(xml.getRoot());
        if(root->getName() == "controller") {
            auto count(root->getCountNodes() / 2);
            for(int i = 0; i < count; i++) {
                auto s1(root->getTag(i * 2)->getVal()), s2(root->getTag(i * 2 + 1)->getVal());
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
            setSize(szi(lps.w, lps.h));
        }
    }
}

i32 zViewController::onTouchEvent(zTouch *touch) {
    auto but(buttons);
    if(touch->isCaptured()) {
        auto x((int)((touch->cpt.x - (float)rview.x) * sizeReverseMap.w));
        auto y((int)((touch->cpt.y - (float)rview.y) * sizeReverseMap.h));
        if(x >= 0 && y >= 0 && x < cellDebug.w && y < cellDebug.h) {
            but = map[y * cellDebug.w + x];
        }
    } else if(touch->isReleased()) {
        but = 0;
    }
    if(but != buttons) {
        buttons = but;
        // вызываем событие
        if(onChangeButton) onChangeButton(this, buttons);
        invalidate();
        return TOUCH_STOP;
    }
    return TOUCH_CONTINUE;
}

void zViewController::onDraw() {
    // базовый тайл
    auto but(buttons);
    auto fk(drw[DRW_FK]);
    fk->tile = base; fk->measure(0, 0, 3, true); fk->draw(nullptr);
    // нажатые кнопки
    for(int i = 0; i < 4; i++) {
        if(but & 1) {
            fk->setTileNum(i); fk->measure(0, 0, 3, true); fk->draw(nullptr);
        }
        but >>= 1;
    }
}

void zViewController::onLayout(crti &position, bool changed) {
    zView::onLayout(position, changed);
    auto width(rview.w), height(rview.h);
    sizeReverseMap.w = (float)cellDebug.w / (float)width;
    sizeReverseMap.h = (float)cellDebug.h / (float)height;
    rclient = rview;
    drw[DRW_FK]->measure(width, height, 3, false); drw[DRW_FK]->bound = rview.xy();
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
        requestLayout();
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

zViewToast::zViewToast(cstr _text, zStyle* styles) : zViewText(styles ? styles : styles_z_toast, z.R.id.toast, 0) {
    minMaxSize.set(z_dp(z.R.dimen.toastMinWidth), 0, z_dp(z.R.dimen.toastMinHeight), 0);
    // удалить старый
    auto common(manager->getSystemView(true));
    auto v(common->idView(z.R.id.toast));
    common->remove(v);
    animator.clear();
    animator.init(0.0f, false);
    animator.add(5.0f, zInterpolator::LINEAR, 128);
    animator.add(0.0f, zInterpolator::LINEAR, 64);
    if(!z_isempty(_text)) {
        // добавить новый
        common->attach(this, ZS_GRAVITY_HCENTER | ZS_GRAVITY_BOTTOM, 0, VIEW_WRAP, VIEW_WRAP);
        setText(_text, true);
    }
    setOnAnimation([this, common](zView*, int) {
        invalidate();
        auto ret(animator.update(alpha));
        if(!ret) common->remove(this);
        return ret;
    });
}

void zViewToast::onInit(bool _theme) {
    zViewText::onInit(_theme);
    setAlpha(0);
    post(MSG_ANIM, duration, 0);
}
