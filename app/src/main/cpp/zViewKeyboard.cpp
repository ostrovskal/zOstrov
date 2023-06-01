
#include "zssh/zCommon.h"
#include "zssh/zViewKeyboard.h"
#include "zssh/zViewWidgets.h"

zViewKeyboard::zViewKeyboard(cstr nameLayouts) : zViewGroup(styles_default, z.R.id.keyboard) {
    updateStatus(ZS_SYSTEM, true); updateStatus(ZS_VISIBLED, false);
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
        auto root(manager->getSystemView(true));
        root->scroll.y = ((int)round((float)offsetY / 6.0f) * v);
        lps.y = heightScreen - ((int)round((float)rview.h / 6.0f) * v);
        root->requestLayout();
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
                but.name = nodeBut->getAttrVal("com", "");
                but.spec = nodeBut->getAttrVal("spec", "");
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
    if(countChildren()) removeAllViews();
    // основной текст
    attach(new zViewText(styles_z_keyboardbase, 0, 0), 100, 100);
    // альтернативный текст
    attach(new zViewText(styles_z_keyboardalt, 0, 0), 100, 100);
    // для нажатия кнопки
    zParamDrawable kp(0x90000000, 0, -1, 0, 1.0f);
    setDrawable(&kp, DRW_FK);
    // установить раскладку по умолчанию
    if(!current) setLayout(defName);
}

void zViewKeyboard::setLayout(const zStringUTF8 &_name) {
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
                    if(but->spec == "DELETE"/* || but->spec == "SHIFT"*/) {
                        nPressSpec = 1; post(MSG_ANIM, duration, 0);
                    }
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
                else keyCode = but->name[touch->isLongClick()];
                if(!_switch && keyCode) {
                    if(owner) owner->keyEvent(keyCode, false);
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
    auto widthSize(spec.w.size()), heightSize(z_percent(zGL::instance()->getSizeScreen(true), minHeight));
    auto sz(current ? current->size : szi(1, 1));
    deltaWidth = ((float)widthSize / (float)sz.w);
    deltaHeight = ((float)heightSize / (float)sz.h);
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
            auto delta(hScreen - owner->edges(true, true));
            // проверить если нижняя граница вида меньше высоты клавы, то:
            offsetY = (delta < rview.h ? rview.h - delta : 0);
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
    zStringUTF8 n1, n2; u32 color;
    auto baseTxt(atView<zViewText>(0)), altTxt(atView<zViewText>(1));
    baseTxt->updateStatus(ZS_VISIBLED, true); altTxt->updateStatus(ZS_VISIBLED, true);
    for(auto& b : current->buttons) {
        auto& nm(b.name); auto isHighlight(false);
        if(b.spec.isNotEmpty()) {
            n1 = nm; n2.empty();
            color = zColor::gray.toARGB();
            isHighlight = (b.spec == "SHIFT" && activeShift);
        } else {
            n1 = nm.substr(0, 1);
            n2 = nm.substr(1, 1);
            color = theme->themeColor;
        }
        auto r(b.rview); baseTxt->drw[DRW_FK]->color = color;
        r.x = rview.x + r.x * deltaWidth; r.w *= deltaWidth;
        r.y = rview.y + r.y * deltaHeight; r.h *= deltaHeight;
        szm spec(zMeasure(MEASURE_EXACT, r.w), zMeasure(MEASURE_EXACT, r.h));
        baseTxt->setTextColorForeground(isHighlight ? 0xff0000ff : 0xffffffff);
        baseTxt->setText(n1, true); baseTxt->measure(spec); baseTxt->layout(r); baseTxt->draw();
        if(n2.isNotEmpty() && n1 != n2) { altTxt->setText(n2, true); altTxt->measure(spec); altTxt->layout(r); altTxt->draw(); }
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

void zViewKeyboard::show(u32 _id, bool set) {
    if(current) {
        auto root(manager->getSystemView(true));
        nPressSpec = 0; activeShift = false;
        if(set) {
            owner = manager->idView(_id);
            cstr _defName(defName);
            if(owner) {
                auto name(owner->getDefaultKeyboardLayer());
                if(layouts.indexOf(name) != -1) _defName = name;
            }
            setLayout(_defName);
            if(_id <= 0 || isChecked()) {
                // берем нижнюю координату владельца
                auto y(owner->edges(true, true));
                y -= rview.y;
                if(y > 0) root->lps.y -= y;
                return;
            }
            updateStatus(ZS_VISIBLED, true);
        } else {
            if(!isChecked()) return;
            owner = nullptr;
        }
        isUpdate = true;
        requestLayout();
    }
}

void zViewKeyboard::requestLayout() {
    if(countChildren() && !atView(0)->isVisibled())
        zViewGroup::requestLayout();
}
