
#include "zssh/zCommon.h"
#include "zssh/zViewKeyboard.h"
#include "zssh/zViewWidgets.h"

zViewKeyboard::zViewKeyboard(cstr nameLayouts) : zViewGroup(styles_default, z.R.id.keyboard) {
    updateStatus(ZS_SYSTEM, true); updateStatus(ZS_VISIBLED, false);
//    setGravity(ZS_GRAVITY_HCENTER);
    setDrawable(nullptr, DRW_FBO); duration = 20;
    setOnAnimation([this] (zView*, int) {
        if(current && owner && nPressSpec) {
            auto but(&current->buttons[butIdx]); nPressSpec++;
            if(but->spec == "DELETE" && nPressSpec > 15 && !(nPressSpec & 1)) owner->keyEvent('\b', false);
            return true;
        }
        // сдвигаем родительский
        float v; auto visible(animator.update(v));
        auto heightScreen(zGL::instance()->getSizeScreen(true));
        parent->scroll.y = scrollY + ((int)round((float)offsetY / 6.0f) * v);
        lps.y = heightScreen - ((int)round((float)rview.h / 6.0f) * v);
        parent->requestPosition();
        updateStatus(ZS_VISIBLED, lps.y < heightScreen);
        return visible;
    });
    i32 idx(0);
    auto ptr(manager->assetFile(nameLayouts, &idx));
    zXml xml(ptr, idx);
    if(xml.isValid()) {
        auto root(xml.getRoot());
        if(root->getName() != "keyboards") {
            ILOG("Недопустимый XML формат клавиатуры!");
            return;
        }
        const zXml::zNode* nodeLyt, *nodeBut;
        BUTTON but; idx = 0;
        while((nodeLyt = root->getTag(idx++))) {
            if(nodeLyt->getName() != "layout") continue;
            static cstr _switch[] = { "name", "shift", "spec", "lang", "input" };
            LAYOUT lyt;
            // размер
            lyt.size.w = z_ston(nodeLyt->getAttrVal("width", "1"), RADIX_DEC);
            lyt.size.h = z_ston(nodeLyt->getAttrVal("height", "1"), RADIX_DEC);
            // переключатели
            for(int i = 0; i < 5; i++) lyt.names[i] = nodeLyt->getAttrVal(_switch[i], "");
            // кнопки
            int _idx(0);
            while((nodeBut = nodeLyt->getTag(_idx++))) {
                static cstr _coords[] = { "x", "y", "width", "height" };
                for(int i = 0 ; i < 4; i++) but.rview[i] = z_ston(nodeBut->getAttrVal(_coords[i], "0"), RADIX_DEC);
                but.icon = z_ston(nodeBut->getAttrVal("icon", "-1"), RADIX_DEC);
                but.color= z_ston(nodeBut->getAttrVal("color", "ffffffff"), RADIX_HEX);
                but.size = z_dp(z_ston(nodeBut->getAttrVal("size", "23"), RADIX_DEC));
                but.spec = nodeBut->getAttrVal("spec", "");
                but.name[0] = nodeBut->getAttrVal("com", "");
                if(but.spec.isEmpty()) {
                    but.name[1] = but.name[0].substr(1, 1);
                    but.name[0] = but.name[0].substr(0, 1);
                }
                lyt.buttons += but;
            }
            layouts += lyt;
        }
        // основные
        minHeight= z_ston(root->getAttrVal("minHeight", "40"), RADIX_DEC);
        idx      = layouts.indexOf(root->getAttrVal("default", ""));
        if(idx != -1) defName = layouts[idx].names[0];
    }
}

zViewKeyboard::~zViewKeyboard() {
    current = nullptr;
    layouts.clear();
}

void zViewKeyboard::onInit(bool _theme) {
    zViewGroup::onInit(_theme);
    if(countChildren()) removeAllViews(false);
    // основной текст
    attach(new zViewButton(styles_z_keyboardbase, 0, 0), 100, 100);
    // альтернативный текст
    attach(new zViewButton(styles_z_keyboardalt, 0, 0), 100, 100);
    // для нажатия кнопки
    zParamDrawable kp(0x90000000, 0, -1, 0, 1.0f);
    setDrawable(&kp, DRW_FK);
    // установить раскладку по умолчанию
    if(!current) setLayout(defName);
}

void zViewKeyboard::setLayout(czs& _name) {
    if(current && current->names[0] == _name) return;
    auto idx(layouts.indexOf(_name));
    if(idx != -1) {
        if(current) prevName = current->names[0];
        current = &layouts[idx]; requestLayout();
    } else {
        current = nullptr;
    }
}

i32 zViewKeyboard::onTouchEvent(zTouch *touch) {
    auto& buts(current->buttons);
    for(int i = 0 ; i < buts.size(); i++) {
        auto but(&buts[i]); auto r(but->rview);
        r.x = rview.x + r.x * deltaWidth; r.w *= deltaWidth;
        r.y = rview.y + r.y * deltaHeight; r.h *= deltaHeight;
        if(r.contains(touch->cpt.x, touch->cpt.y)) {
            if(touch->isCaptured()) {
                if(butIdx == -1) {
                    drw[DRW_FK]->measure(r.w, r.h, 0, false);
                    drw[DRW_FK]->bound = r;
                    butIdx = i;
                    if(but->spec == "DELETE") { nPressSpec = 1; post(MSG_ANIM, duration, 0); }
                }
                return TOUCH_STOP;
            }
            if(butIdx == i) {
                // кнопка отпущена
                zStringUTF8 *_switch(nullptr);
                int keyCode{0};
                if(nPressSpec) {
                    auto isFinish(nPressSpec > 15);
                    nPressSpec = 0; manager->eraseAllEventsView(this);
                    if(isFinish) { butIdx = -1; return TOUCH_STOP; }
                }
                if(but->spec == "DELETE") keyCode = '\b';
                else if(but->spec == "ENTER") keyCode = '\n';
                else if(but->spec == "SPACE") keyCode = ' ';
                else if(but->spec == "SHIFT") {
                    if(!(activeShift = touch->isDblClicked())) {
                        _switch = &current->names[KEYBOARD_SHIFT];
                    } else {
                        if(current->names[KEYBOARD_INPUT].isEmpty())
                            _switch = &current->names[KEYBOARD_SHIFT];
                        else {
                            isDrawing = true;
                            invalidate();
                        }
                    }
                }
                else if(but->spec == "LANG") _switch = &current->names[KEYBOARD_LANG];
                else if(but->spec == "SPEC") _switch = &current->names[KEYBOARD_DIGIT];
                else keyCode = but->name[touch->isLongClick()][0];
                if(!_switch && keyCode) {
                    if(owner) owner->keyEvent(keyCode, false);
                    if(owner) show(owner->id, true);
                    if(current->names[KEYBOARD_INPUT].isNotEmpty() && !activeShift) {
                        _switch = &current->names[KEYBOARD_INPUT];
                    }
                }
                if(_switch && _switch->isNotEmpty()) {
                    setLayout(*_switch == "back" ? prevName : *_switch);
                }
            }
        }
    }
    butIdx = -1;
    return TOUCH_STOP;
}

void zViewKeyboard::onMeasure(cszm& spec) {
    auto heightSize(z_percent(zGL::instance()->getSizeScreen(true), minHeight + 10 * manager->isLandscape()));
    auto widthSize(z_min(spec.w.size(), (int)roundf((float)heightSize * 2.5f)));
    auto sz(current ? current->size : szi(1, 1));
    deltaHeight = ((float)heightSize / (float)sz.h);
    deltaWidth = ((float)widthSize / (float)sz.w);
    setMeasuredDimension(widthSize, heightSize);
    isDrawing = true;
}

void zViewKeyboard::onLayout(crti &position, bool changed) {
    zViewGroup::onLayout(position, changed);
    rclip = rview; updateStatus(ZS_DIRTY_LAYER, isDrawing);
    if(isUpdate) {
        isUpdate = false;
        auto checked(updateStatus(ZS_CHECKED, owner != nullptr) != 0);
        if(owner) {
            auto hScreen(zGL::instance()->getSizeScreen(true));
            auto y(owner->edges(true, true) + owner->rview.h / 2);
            auto delta(hScreen - y);
            // проверить если нижняя граница вида меньше высоты клавы, то:
            offsetY = (delta < rview.h ? rview.h - delta : 0);
            scrollY = parent->scroll.y;
            owner->requestFocus();
        }
        animator.init(!checked * 6.0f - !checked, false);
        animator.add(checked * 6.0f, zInterpolator::EASEOUTCUBIC, 6 - !checked);
        post(MSG_ANIM, duration, 0);
    }
}

void zViewKeyboard::onDrawFBO() {
    drw[DRW_FBO]->draw(&rview);
    // нарисовать нажатую кнопку
    if(butIdx != -1) drw[DRW_FK]->draw(nullptr);
}

void zViewKeyboard::onDraw() {
    // сформировать клавиатуру
    if(!current || !isDrawing) return;
    zStringUTF8 n1, n2; u32 bkColor;
    auto baseTxt(atView<zViewButton>(0)), altTxt(atView<zViewButton>(1));
    baseTxt->updateStatus(ZS_VISIBLED, true); altTxt->updateStatus(ZS_VISIBLED, true);
    for(auto& b : current->buttons) {
        if(b.spec.isNotEmpty()) {
            n1 = b.name[0]; n2.empty();
            baseTxt->setIcon(b.spec == "SHIFT" && activeShift ? z.R.integer.iconShiftFix : b.icon);
            bkColor = 0xff808080;

        } else {
            n1 = b.name[0]; n2 = b.name[1];
            bkColor = theme->themeColor;
            baseTxt->setIcon(-1);
        }
        auto r(b.rview); baseTxt->drw[DRW_FK]->color = bkColor;
        r.x = rview.x + r.x * deltaWidth; r.w *= deltaWidth;
        r.y = rview.y + r.y * deltaHeight; r.h *= deltaHeight;
        szm spec(zMeasure(MEASURE_EXACT, r.w), zMeasure(MEASURE_EXACT, r.h));
        baseTxt->setTextSize(b.size); baseTxt->setTextColorForeground(b.color);
        baseTxt->setTextSpecial(n1, spec); baseTxt->layout(r); baseTxt->draw();
        if(n2.isNotEmpty() && n1 != n2) { altTxt->setTextSpecial(n2, spec); altTxt->layout(r); altTxt->draw(); }
    }
    baseTxt->updateStatus(ZS_VISIBLED, false); altTxt->updateStatus(ZS_VISIBLED, false);
    isDrawing = false;
}

i32 zViewKeyboard::keyEvent(int key, bool sysKey) {
    if(sysKey && key == AKEYCODE_BACK) {
        if(owner && isChecked()) {
            if(owner) return owner->keyEvent('\n', false);
        }
    }
    return 0;
}

static zView* getParentOwner(zView* o) {
    if(o) {
        if(o == manager->getSystemView(true)) return o;
        auto sl(dynamic_cast<zScrollLayout*>(o));
        if(sl) { if(sl->isVertical()) return sl->atView(0); }
        return getParentOwner(o->getParent());
    }
    return nullptr;
}

void zViewKeyboard::show(u32 _id, bool set) {
    if(current) {
        if(set) {
            auto isOwner(owner != nullptr);
            owner = manager->idView(_id);
            parent = getParentOwner(owner);
            if(!isOwner) {
                cstr _defName(defName);
                if(owner) {
                    auto name(owner->getDefaultKeyboardLayer());
                    if(layouts.indexOf(name) != -1) _defName = name;
                }
                setLayout(_defName);
            }
            if(_id <= 0 || isChecked()) {
                // ставим объект в область экрана
                auto y(owner->rview.y - owner->rview.h / 2);
                if(y > 0) {
                    y = ((owner->edges(true, true) + owner->rview.h / 2) - rview.y);
                    if(y < 0) return;
                }
                //scrollY = parent->scroll.y;
                parent->scroll.y += y;// offsetY = y;
                owner->requestPosition();
                return;
            }
            updateStatus(ZS_VISIBLED, true);
        } else {
            if(!isChecked()) return;
            owner = nullptr;
            nPressSpec = 0; activeShift = false;
        }
        isUpdate = true;
        requestLayout();
    }
}

void zViewKeyboard::requestLayout() {
    if(countChildren() && !atView(0)->isVisibled())
        zViewGroup::requestLayout();
}
