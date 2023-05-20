
#include "zssh/zCommon.h"
#include "zssh/zViewKeyboard.h"
#include "zssh/zViewWidgets.h"

// загрузить из активов XML - все макеты
// <keyboards default="" minHeight="">
//  <layout name="" shift="" spec="" lang="" input="" />
//  ...
// </keyboards>

static zStyle styles_z_keyboardBase[] = {
        { Z_FOREGROUND, z.R.drawable.zssh },
        { Z_FOREGROUND_TILES, z.R.integer.keybButSmall },
        { Z_FOREGROUND_GRAVITY, ZS_GRAVITY_CENTER },
        { Z_TEXT_FONT, z.R.drawable.font1 },
        { Z_TEXT_FOREGROUND_COLOR, 0xffffffff },
        { Z_TEXT_STYLE, ZS_TEXT_BOLD },
        { Z_TEXT_SIZE, 15 },
        { Z_GRAVITY, ZS_GRAVITY_CENTER },
        { Z_IPADDING, 0x03030303 },
        { Z_PADDING | ZT_END, 0x01010101 }
};

static zStyle styles_z_keyboardAlt[] = {
        { Z_FOREGROUND_GRAVITY, ZS_GRAVITY_CENTER },
        { Z_TEXT_FONT, z.R.drawable.font1 },
        { Z_TEXT_FOREGROUND_COLOR, 0xffafafaf },
        { Z_TEXT_STYLE, ZS_TEXT_BOLD },
        { Z_TEXT_SIZE, 12 },
        { Z_GRAVITY, ZS_GRAVITY_END | ZS_GRAVITY_BOTTOM },
        { Z_IPADDING, 0x04040404 },
        { Z_PADDING | ZT_END, 0x01010101 }
};

zViewKeyboard::zViewKeyboard(cstr nameLayouts) : zViewGroup(styles_default, z.R.id.keyboard) {
    //updateStatus(ZS_SYSTEM, true); updateStatus(ZS_VISIBLED, false);
    setDrawable(nullptr, DRW_FBO); duration = 20;
    setOnAnimation([this] (zView*, int) {
/*
        // начальная координата
        auto frame(drs[DR_IDX_FK]->frame);
        // сдвигаем корневой
        auto root(vmanager->getRootView());
        auto rf(&root->lytParams);
        rf->y = parentY + ((int)round((float)offsetY / 5.0f) * frame);
        auto bound(rfull);
        bound.y = keybY + ((int)round((float)rfull.h / 5.0f) * frame);
        lytParams.y = bound.y;
        frame += (!isChecked() * 2) - 1;
        drs[DR_IDX_FK]->frame = frame;
        invalidate();
        // скрыть клаву
        if(frame == 6) updateStatus(ZS_VISIBLED, false);
        return(frame == -1 || frame == 6);
*/
       return 0;
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
                for(int i = 0 ; i < 4; i++) but.rview.buf[i] = z_ston(nodeBut->getAttrVal(_coords[i], "0"), RADIX_DEC);
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
    removeAllViews();
    // основной текст
    attach(new zViewText(styles_z_keyboardBase, 0, 0), 100, 100);
    // альтернативный текст
    attach(new zViewText(styles_z_keyboardAlt, 0, 0), 100, 100);
    //atView(0)->removeDrawable(DRW_FBO); atView(1)->removeDrawable(DRW_FBO);
    // для нажатия кнопки
    drw[DRW_FK] = new zDrawable(this, DRW_FK);
    drw[DRW_FK]->init(0x90000000, 0, -1, 0, 1.0f);
    // установить раскладку по умолчанию
    if(!current) setLayout(defName);
}

void zViewKeyboard::setLayout(const zStringUTF8 &_name) {
    if(current && current->names[0] == _name) return;
    auto idx(layouts.indexOf(_name));
    if(idx != -1) {
        if(current) prevName = current->names[0];
        current = &layouts[idx];
        updateStatus(ZS_MEASURE | ZS_LAYOUT, false);
        parent->requestLayout();
        invalidate();
    } else {
        current = nullptr;
    }
}

void zViewKeyboard::onMeasure(cszm& spec) {
    auto widthSize(spec.w.size()), heightSize(z_percent(zGL::instance()->getSizeScreen(true), minHeight));
    auto sz(current ? current->size : szi(1, 1));
    deltaWidth = ((float)widthSize / (float)sz.w);
    deltaHeight = ((float)heightSize / (float)sz.h);
    setMeasuredDimension(widthSize, heightSize);
}

i32 zViewKeyboard::onTouchEvent(zTouch *touch) {
    auto& buts(current->buttons);
    for(int i = 0 ; i < buts.size(); i++) {
        auto but(&buts[i]); auto r(but->rview);
        r.x *= deltaWidth; r.w *= deltaWidth;
        r.y *= deltaHeight; r.h *= deltaHeight;
        if(r.contains(touch->cpt.x, touch->cpt.y)) {
            if(touch->isCaptured()) {
                if(butIdx == -1) {
                    drw[DRW_FK]->measure(r.w, r.h, 0, false);
                    drw[DRW_FK]->bound = r;
                    butIdx = i;
                }
                return TOUCH_STOP;
            }
            if(butIdx == i) {
                // кнопка отпущена
                zStringUTF8 *_switch(nullptr);
                int keyCode{0};
                if(but->spec == "DELETE") keyCode = '\b';
                else if(but->spec == "ENTER") keyCode = '\n';
                else if(but->spec == "SHIFT") _switch = &current->names[1];
                else if(but->spec == "LANG") _switch = &current->names[3];
                else if(but->spec == "SPEC") _switch = &current->names[2];
                else keyCode = but->name[touch->isLongClick()];
                if(!_switch) {
                    if(owner) owner->keyEvent(keyCode, false);
                    if(current->names[4].isNotEmpty()) {
                        _switch = &current->names[4];
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

void zViewKeyboard::onLayout(crti &position, bool changed) {
    zViewGroup::onLayout(position, changed);
    drw[DRW_FBO]->bound = rview;
/*
    if(isUpdate) {
        isUpdate = false;
        updateStatus(ZS_CHECKED, owner != nullptr);
        if(owner) {
            auto hScreen(zGL::instance()->getSizeScreen(true));
            auto delta(hScreen - owner->edges(true, true));
            // проверить если нижняя граница вида меньше высоты клавы, то:
            offsetY = (delta < rview.h ? rview.h - delta : 0);
            keybY = hScreen - rview.h;
            auto root(manager->getSystemView(true));
            parentY = root->lps.y - offsetY;
            owner->requestFocus();
        }
        //drs[DR_IDX_FK]->frame = owner ? 5 : 0;
        post(MSG_ANIM, duration, 0);
    }
*/
}

void zViewKeyboard::onDrawFBO() {
    drw[DRW_FBO]->draw(nullptr);
    // нарисовать нажатую кнопку
    if(butIdx != -1) drw[DRW_FK]->draw(nullptr);
}

void zViewKeyboard::onDraw() {
    // сформировать клавиатуру
    if(!current) return;
    zStringUTF8 n1, n2;
    auto baseTxt(atView<zViewText>(0)), altTxt(atView<zViewText>(1));
    baseTxt->updateStatus(ZS_VISIBLED, true); altTxt->updateStatus(ZS_VISIBLED, true);
    for(auto& b : current->buttons) {
        auto& nm(b.name);
        if(b.spec.isNotEmpty()) {
            n1 = nm; n2.empty();
            baseTxt->drw[DRW_FK]->color = zColor::gray;
        } else {
            n1 = nm.substr(0, 1);
            n2 = nm.substr(1, 1);
            baseTxt->drw[DRW_FK]->color = theme->themeColor;
        }
        auto r(b.rview);
        r.x *= deltaWidth; r.w *= deltaWidth;
        r.y *= deltaHeight; r.h *= deltaHeight;
        szm spec(zMeasure(MEASURE_EXACT, r.w), zMeasure(MEASURE_EXACT, r.h));
        baseTxt->setText(n1, true); baseTxt->measure(spec); baseTxt->layout(r); baseTxt->draw();
        if(n2.isNotEmpty() && n1 != n2) { altTxt->setText(n2, true); altTxt->measure(spec); altTxt->layout(r); altTxt->draw(); }
    }
    baseTxt->updateStatus(ZS_VISIBLED, false); altTxt->updateStatus(ZS_VISIBLED, false);
}

i32 zViewKeyboard::keyEvent(int key, bool sysKey) {
    if(sysKey && key == AKEYCODE_BACK) {
        if(owner && isChecked()) {
            if(owner) owner->keyEvent('\n', false);
        }
    }
    return 0;
}

void zViewKeyboard::show(u32 _id, bool set) {
    if(current) {
        if(set) {
            owner = manager->idView(_id);
            cstr _defName(defName);
            if(owner) {
                auto name(owner->getDefaultKeyboardLayer());
                if(name) _defName = name;
            }
            setLayout(_defName);
            if(_id <= 0 || isChecked()) {
                auto y(owner->edges(true, true));
                y -= rview.y;
                if(y > 0) {
                    manager->getSystemView(true)->lps.y -= y;
                    invalidate();
                }
                return;
            }
            updateStatus(ZS_VISIBLED, true);
        } else {
            if(!isChecked()) return;
            owner = nullptr;
        }
        isUpdate = true;
        invalidate();
    }
}
