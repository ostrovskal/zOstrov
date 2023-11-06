
#include "zostrov/zCommon.h"
#include "zostrov/zViewKeyboard.h"
#include "zostrov/zViewWidgets.h"

zViewKeyboard::zViewKeyboard(cstr nameLayouts, zStyle* _styles_base, zStyle* _styles_alt, int _id) : zViewGroup(styles_default, _id) {
    static cstr _switch[] = { "name", "shift", "spec", "lang", "input" };
    styles_base = _styles_base ? _styles_base : styles_z_keyboardbase;
    styles_alt  = _styles_alt  ? _styles_alt  : styles_z_keyboardalt;
    updateStatus(ZS_SYSTEM, _id == z.R.id.keyboard);
    updateStatus(ZS_VISIBLED, false);
    setDrawable(nullptr, DRW_FBO); duration = 20;
    i32 idx(0); auto ptr((u8*)zFileAsset(nameLayouts, true, false).readn(&idx));
    zXml xml(ptr, idx); if(!xml.isValid()) return;
    auto root(xml.getRoot());
    if(root->getName() != "keyboards") { ILOG("Недопустимый XML формат клавиатуры!"); return; }
    // основные
    minHeight= root->getAttrVal("minHeight", "25").toNum();
    const zXml::zNode* nodeLyt, *nodeBut;
    BUTTON but; idx = 0; auto tex(manager->cache->get(theme->findString(styles_base->_int(Z_ICON, z.R.drawable.zssh)), nullptr));
    while((nodeLyt = root->getTag(idx++))) {
        if(nodeLyt->getName() != "layout") continue;
        auto lyt(new LAYOUT());
        // размер
        auto bs(z_dp(nodeLyt->getAttrVal("baseSize", "20").toNum()));
        lyt->size.w = nodeLyt->getAttrVal("width", "1").toNum();
        lyt->size.h = nodeLyt->getAttrVal("height", "1").toNum();
        // переключатели
        for(int i = 0; i < 5; i++) lyt->names[i] = nodeLyt->getAttrVal(_switch[i], "");
        // кнопки
        int _idx(0);
        while((nodeBut = nodeLyt->getTag(_idx++))) {
            static cstr _coords[] = { "x", "y", "width", "height" };
            for(int i = 0 ; i < 4; i++) but.rview[i] = nodeBut->getAttrVal(_coords[i], "0").toNum();
            but.icon = tex->getTile(nodeBut->getAttrVal("icon", ""));
            but.color= zColor(nodeBut->getAttrVal("color", "#fff")).toARGB();
            auto bsz(nodeBut->getAttrVal("size", ""));
            but.size = z_is8(bsz) ? z_dp(bsz.toNum()) : bs;
            but.spec = nodeBut->getAttrVal("spec", "");
            but.name[0] = nodeBut->getAttrVal("com", "");
            but.name[1] = nodeBut->getAttrVal("alt", "");
            lyt->buttons += but;
        }
        layouts += lyt;
    }
    manager->cache->recycle(tex);
    idx      = layouts.indexOf(root->getAttrVal("default", ""));
    if(idx != -1) defName = layouts[idx]->names[KEYBOARD_NAME];
}

zViewKeyboard::~zViewKeyboard() {
    current = nullptr;
    layouts.clear();
}

void zViewKeyboard::notifyEvent(HANDLER_MESSAGE *msg) {
    if(msg->what == MSG_ANIM) {
        u32 cont(1U);
        if(current && nPressSpec) {
            nPressSpec++; if(owner && nPressSpec > 15 && !(nPressSpec & 1))
                owner->keyEvent(keyCode, false);
        } else {
            // сдвигаем родительский
            float v; cont = animator.update(v);
            auto heightScreen(zGL::instance()->getSizeScreen(true));
            parent->scroll.y = scrollY + z_round((float)offsetY / 6.0f * v);
            lps.y = heightScreen - z_round((float)rview.h / 6.0f * v);
            parent->requestPosition(); updateStatus(ZS_VISIBLED, lps.y < heightScreen);
        }
        if(cont) post(MSG_ANIM, duration);
    }
    zViewGroup::notifyEvent(msg);
}

void zViewKeyboard::onInit(bool _theme) {
    zViewGroup::onInit(_theme);
    removeAllViews(false);
    // основной текст
    attach(new zViewButton(styles_base, 0, 0), 100, 100);
    // альтернативный текст
    attach(new zViewButton(styles_alt, 0, 0), 100, 100);
    // для нажатия кнопки
    zParamDrawable kp{ 0x90000000, 0x0, 0, -1, 0, 0, 0, 1.0f };
    setDrawable(&kp, DRW_FK);
    // установить раскладку по умолчанию
    if(!current) setLayout(defName);
}

void zViewKeyboard::setLayout(czs& _name) {
    if(current && current->names[0] == _name) return;
    auto idx(layouts.indexOf(_name));
    if(idx != -1) {
        if(current) prevName = current->names[0];
        current = layouts[idx]; requestLayout();
    } else {
        current = nullptr;
    }
}

bool zViewKeyboard::repeatPressed(BUTTON* but, int& code) const {
    if(but->spec == "DELETE") code = '\b';
    else code = but->name[0][0];
    return (but->name[0] == but->name[1] || but->name[1].isEmpty());
}

bool zViewKeyboard::processSpecPressed(BUTTON* but, int& code, zString8** _switch, bool dblClick) {
    auto name(but->spec);
    if(name == "DELETE") code = '\b';
    else if(name == "ENTER") code = '\n';
    else if(name == "SPACE") code = ' ';
    else if(name == "SHIFT") {
        if(!(activeShift = dblClick)) {
            *_switch = &current->names[KEYBOARD_SHIFT];
        } else {
            if(current->names[KEYBOARD_INPUT].isEmpty())
                *_switch = &current->names[KEYBOARD_SHIFT];
            else {
                isDrawing = true;
                invalidate();
            }
        }
    }
    else if(name == "LANG") *_switch = &current->names[KEYBOARD_LANG];
    else if(name == "SPEC") *_switch = &current->names[KEYBOARD_DIGIT];
    else return false;
    return true;
}

i32 zViewKeyboard::onTouchEvent() {
    auto& buts(current->buttons);
    for(int i = 0 ; i < buts.size(); i++) {
        auto but(&buts[i]); auto r(but->rview);
        r.x = rview.x + z_round((float)r.x * deltaWidth);  r.w = z_round((float)r.w * deltaWidth);
        r.y = rview.y + z_round((float)r.y * deltaHeight); r.h = z_round((float)r.h * deltaHeight);
        if(r.contains((int)touch->cpt.x, (int)touch->cpt.y)) {
            if(touch->isCaptured()) {
                if(butIdx == -1) {
                    drw[DRW_FK]->measure(r.w, r.h, 0, false);
                    drw[DRW_FK]->bound = r;
                    butIdx = i;
                    if(repeatPressed(but, keyCode)) { nPressSpec = 1; post(MSG_ANIM, duration, 0); }
                }
                return TOUCH_STOP;
            }
            if(butIdx == i) {
                // кнопка отпущена
                zString8 *_switch(nullptr); keyCode = 0;
                if(nPressSpec) {
                    auto isFinish(nPressSpec > 15);
                    nPressSpec = 0; manager->eraseAllEventsView(this);
                    if(isFinish) { butIdx = -1; return TOUCH_STOP; }
                }
                if(!processSpecPressed(but, keyCode, &_switch, touch->isDblClicked())) {
                    keyCode = but->name[touch->isLongClick()][0];
                }
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
    auto heightSize(z_percent(zGL::instance()->getSizeScreen(true), minHeight + 17 * manager->isLandscape()));
    auto widthSize(z_min(spec.w.size(), z_round((float)heightSize * 2.5f)));
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
        animator.init((float)(!checked * 6 - !checked), false);
        animator.add((float)(checked * 6), zInterpolator::EASEOUTCUBIC, 6 - !checked);
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
    zString8 n1, n2; u32 bkColor;
    auto baseTxt(atView<zViewButton>(0)), altTxt(atView<zViewButton>(1));
    baseTxt->updateStatus(ZS_VISIBLED, true); altTxt->updateStatus(ZS_VISIBLED, true);
    drw[DRW_FK]->measure(rview.w, rview.h, 0, false);
    drw[DRW_FK]->color.a = 0.9f; drw[DRW_FK]->draw(&rview); drw[DRW_FK]->color.a = 0.5f;
    for(auto& b : current->buttons) {
        if(b.spec.isNotEmpty()) {
            n1 = b.name[0]; n2.empty();
            bkColor = 0xff808080;
        } else {
            n1 = b.name[0]; n2 = b.name[1];
            bkColor = theme->themeColor;
        }
        auto r(b.rview); baseTxt->drw[DRW_FK]->color = bkColor;
        r.x = rview.x + z_round((float)r.x * deltaWidth);  r.w = z_round((float)r.w * deltaWidth);
        r.y = rview.y + z_round((float)r.y * deltaHeight); r.h = z_round((float)r.h * deltaHeight);
        baseTxt->setIcon((b.spec == "SHIFT" && activeShift) ? z.R.integer.iconShiftFix : b.icon);
        baseTxt->setTextSize(b.size);
        baseTxt->setTextColorForeground(b.color);
        baseTxt->setTextColorIcon(b.color);
        szm spec(zMeasure(MEASURE_EXACT, r.w), zMeasure(MEASURE_EXACT, r.h));
        baseTxt->setTextSpecial(n1, spec); baseTxt->layout(r); baseTxt->draw();
        if(n2.isNotEmpty() && n1 != n2) { altTxt->setTextSpecial(n2, spec); altTxt->layout(r); altTxt->draw(); }
    }
    baseTxt->updateStatus(ZS_VISIBLED, false); altTxt->updateStatus(ZS_VISIBLED, false);
    isDrawing = false;
}

i32 zViewKeyboard::keyEvent(int key, bool sysKey) {
    if(sysKey && key == AKEYCODE_BACK) {
        if(owner && owner->isEnabled() && isChecked()) {
            return owner->keyEvent('\n', false);
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
