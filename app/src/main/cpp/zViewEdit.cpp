//
// Created by User on 01.05.2022.
//

#include "zssh/zCommon.h"
#include "zssh/zViewText.h"
#include "zssh/zViewWidgets.h"

class zViewClear : public zViewButton {
public:
    explicit zViewClear(zViewEdit* _parent) : zViewButton(styles_z_editbutclear, 0, 0) {
        parent = _parent;
    }
    virtual i32 onEvent(zTouch* touch) {
        return onTouchEvent(touch);
    }
    [[nodiscard]] cstr typeName() const override { return "zViewEditClear"; }
};

class zFilterMail : public zFilterEdit {
public:
    int convertChar(czs& t, int ch, int pos) override {
        if(ch == '@' && t.indexOf("@") != -1) ch = 0;
        return ch;
    }
};

class zFilterPhone : public zFilterEdit {
public:
    int convertChar(czs& t, int ch, int pos) override {
        return ch * (pos ? isdigit(ch) : (ch == '+'));
    }
    [[nodiscard]] cstr getKeboardLayer() const override { return "digit123"; }
};

class zFilterPassword : public zFilterEdit {
public:
    czs& getText(const zStringUTF8 &_text) override {
        if(_text.count() != text.count()) text = zStringUTF8('*', _text.count());
        return text;
    }
    zStringUTF8 text;
};

class zFilterZx : public zFilterEdit {
public:
    int convertChar(czs& t, int ch, int pos) override {
        auto _count(t.count());
        switch(t[0]) {
            case '#': // hex
                if(_count >= 5) ch = 0;
                return (isdigit(ch) || (ch >= 'a' && ch <= 'f') || (ch >= 'A' && ch <= 'F')) * ch;
            case '%': // bin
                if(_count >= 17) ch = 0;
                return ((ch == '0' || ch == '1')) * ch;
            default: // dec
                if(_count >= 5) ch = 0;
                return ch * (pos == 0 ? (ch == '#' || ch == '%' || isdigit(ch)) : isdigit(ch));
        }
    }
    [[nodiscard]] cstr getKeboardLayer() const override { return "digit123"; }
};

class zFilterNumber : public zFilterEdit {
public:
    int convertChar(czs& t, int ch, int pos) override {
        return ch * (ch == '.' ? (t.indexOf(".") != -1) : isdigit(ch));
    }
    [[nodiscard]] cstr getKeboardLayer() const override { return "digit123"; }
};

zViewEdit::zViewEdit(zStyle *_stl, i32 _id, u32 _hint) : zViewText(_stl, _id, 0) {
    if(_hint) hintText = theme->findString(_hint);
    minMaxSize.x    = z_dp(z.R.dimen.editMinWidth);
    minMaxSize.w    = z_dp(z.R.dimen.editMinHeight);
    dropdown        = new zViewDropdown(styles_z_menupopup);
    popup           = new zViewPopup(styles_default, this, dropdown);
    dropdown->setAdapter(new zAdapterList(theme->findArray(z.R.array.menuEditor), new zFabricListItem(styles_z_edit_menu_item)));
    dropdown->measure({zMeasure(MEASURE_UNDEF, 0), zMeasure(MEASURE_UNDEF, 0)});
    dropdown->setOnClick([this](zView* v, int sel) {
        popup->dismiss();
        auto x(bkgSpan ? bkgSpan->s : caretIndex), l(bkgSpan ? bkgSpan->length() : 0);
        switch(sel) {
            // copy/cut/delete
            case 0: case 2: case 3:
                if(bkgSpan) {
                    if(sel != 3) manager->exchangeBuffer = realText.substr(x, l);
                    if(sel >= 2) clearSelected(true);
                }
                break;
            // paste
            case 1:
                clearSelected(true);
                setText(realText.insert(x, manager->exchangeBuffer), true);
                caretIndex += manager->exchangeBuffer.count();
                correctCaretPosition(caretIndex);
                break;
        }
    });
}

zViewEdit::~zViewEdit() {
    SAFE_DELETE(filter);
    SAFE_DELETE(but);
    SAFE_DELETE(popup);
}

void zViewEdit::setFilter(u32 inputMode, zFilterEdit* _filter) {
    if(!_filter) {
        switch(inputMode & ZS_MODE_EDIT_MASK) {
            case ZS_EDIT_PASSWORD:_filter = new zFilterPassword(); break;
            case ZS_EDIT_NUMERIC: _filter = new zFilterNumber(); break;
            case ZS_EDIT_ZX_DATA: _filter = new zFilterZx(); break;
            case ZS_EDIT_MAIL: 	  _filter = new zFilterMail(); break;
            case ZS_EDIT_PHONE:   _filter = new zFilterPhone(); break;
            default: 			  _filter = new zFilterEdit(); break;
        }
    }
    filter = _filter;
}

pti zViewEdit::positionFromIndex(int _indexText) {
    auto bound(&drw[DRW_TXT]->bound); pti pos;
    auto x(bound->x), y(bound->y), i(0); _indexText -= visibleIndex;
    // определить вертикальную позицию
    for(auto& c : textCache) {
        auto idx(_indexText - c->text.count());
        if(idx <= 0) {
            getStringFromCache(i, bound, pos);
            x = pos.x + defPaint->widthText(c->text.str(), _indexText);
            break;
        }
        _indexText = idx; y += c->size; i++;
    }
    return {x, y};
}

int zViewEdit::indexFromPosition(cpti& screen, bool exact) {
    auto bound(&drw[DRW_TXT]->bound); pti pos(0, bound->y);
    auto idx(0), i(0);
    for(auto& c : textCache) {
        if(screen.y < (pos.y + c->size)) {
            getStringFromCache(i, bound, pos);
            idx += visibleIndex + defPaint->indexOf(c->text.str(), INT_MAX, screen.x, pos.x, exact);
            break;
        }
        pos.y += c->size; idx += c->text.count(); i++;
    }
    return idx;
}

void zViewEdit::onInit(bool _theme) {
    zViewText::onInit(_theme);
    status          |= ZS_FOCUSABLE_IN_TOUCHABLE;
    maxLines        = (maxLines != 1) * 1023 + 1;
    colorHint       = theme->styles->_int(Z_THEME_COLOR_TEXT_HINT, 0x7f505050);
    setFilter(styles->_int(Z_MODE, ZS_EDIT_TEXT));
    // создать кнопку, если есть иконка
    if(drw[DRW_ICON]->isValid()) {
        if(!but) {
            but = new zViewClear(this); onInit(false);
            but->drw[DRW_FK]->tile = drw[DRW_ICON]->tile;
        }
    }
}

void zViewEdit::onDraw() {
    zViewText::onDraw();
    if(but) but->draw();
}

i32 zViewEdit::keyEvent(int key, bool sysKey) {
    if(testFlags(ZS_READ_ONLY) || !isFocus()) return 0;
    // применить фильтр
    if(filter && key >= ' ') {
        if(!(key = filter->convertChar(realText, key, caretIndex)))
            return 0;
    }
    if(key == '\n') {
        // enter
        post(MSG_EDIT_FINISH, 10, 0);
        manager->changeFocus(nullptr);
    } else if(key) {
        // если есть выделение - удалить его
        if(clearSelected(true)) {
            if(key == '\b') return 1;
        }
        auto caretPos(caretIndex);
        if(key == '\b') {
            if(caretPos > 0) removeText(--caretPos, 1);
        } else if(realText.count() < maxLength) {
            insertText(caretPos++, (char*)&key);
        }
        if(caretPos != caretIndex) {
            post(MSG_EDIT, 10, 0);
            caretIndex = correct(caretPos);
        }
    }
    return 1;
}

cstr zViewEdit::getDrawText(bool _real) {
    return (_real || realText.isNotEmpty()) ? z_ptrUTF8(filter->getText(realText), visibleIndex) : hintText.str();
}

bool zViewEdit::clearSelected(bool del_text) {
    bool ret(false);
    if(del_text && bkgSpan && bkgSpan->length()) {
        if(caretIndex > bkgSpan->s) {
            caretIndex = bkgSpan->s;
            correctCaretPosition(caretIndex);
        }
        removeText(bkgSpan->s, bkgSpan->length());
        ret = true;
    }
    spans.clear(); cacheSpans.clear(); bkgSpan = nullptr;
    invalidate();
    return ret;
}

void zViewEdit::notifyEvent(HANDLER_MESSAGE* msg) {
    zViewText::notifyEvent(msg);
    switch(msg->what) {
        case MSG_EDIT:
        case MSG_EDIT_FINISH:
            if(onChangeText) onChangeText(this, msg->what);
            break;
        case MSG_EDIT_CARET:
            if(onChangeCaretPos) onChangeCaretPos(this, msg->arg);
            break;
        case MSG_EDIT_MENU:
            popup->show({caretScreen.x, 50});
            break;
    }
}

i32 zViewEdit::onTouchEvent(zTouch *touch) {
    if(!isFocus()) zViewText::onTouchEvent(touch);
    i32 result(touch->isCaptured());
    auto isButton(but && but->rview.contains((int)touch->cpt.x, (int)touch->cpt.y));
    if(isButton) {
        // вызываем событие касания дочернего
        result = but->onEvent(touch);
        if(result == TOUCH_ACTION) { clearText(); result = TOUCH_CONTINUE; }
    }
    if(result == TOUCH_FINISH) {
        // определить позицию каретки в тексте и на экране
        auto pos(indexFromPosition(pti(touch->cpt.x, touch->cpt.y), true));
        if(touch->isDblClicked()) {
            posSel.x = 0;  pos = posSel.y = realText.count();
        }
        manager->event.erase(this, 0);
        if(posSel.x == INT_MIN) posSel.x = pos;
        // нажали в выделение?
        bool isMove(bkgSpan != nullptr);
        if(bkgSpan) {
            if(pos >= bkgSpan->s && pos < bkgSpan->e) isMove = (touch->isDragging(true) || touch->isDragging(false));
            if(isMove) clearSelected(false);
        }
        if(!isMove) post(MSG_EDIT_MENU, 1000, 0);
        posSel.y = pos;
        if(posSel.x != posSel.y) {
            int p1(z_min(posSel.x, posSel.y)); int p2(z_max(posSel.x, posSel.y));
            if(bkgSpan) {
                bkgSpan->s = p1; bkgSpan->e = p2;
            } else { bkgSpan = setSpan(new zTextSpanBackgroundColor(bkgHighlight), p1, p2); }
            setText(realText, true);
        }
        if(pos != caretIndex) {
            post(MSG_EDIT_CARET, 3, pos);
            caretIndex = pos;
        }
        // обновить каретку
        correctCaretPosition(caretIndex);
    } else {
        if(!popup->isVisibled()) {
            if(bkgSpan && (caretIndex > bkgSpan->s && caretIndex < bkgSpan->e)) clearSelected(false);
            manager->event.erase(this, 0);
        }
        posSel.x = posSel.y = INT_MIN;
        isButton = true;
    }
    if(isButton && but) {
        but->updateStatus(ZS_TAP, result == TOUCH_FINISH);
        invalidate();
    }
    return result;
}

void zViewEdit::correctCaretPosition(int _index) {
    caretScreen = positionFromIndex(_index);
    if(realText.isEmpty()) {
        caretScreen.x = rclient.x + ipad.x; auto x(0);
        caretScreen.y = rclient.y + ((rclient.h - getTextSize()) >> gravity_shift[((gravity & ZS_GRAVITY_VERT) >> 2) & 3]);
        switch(gravity & ZS_GRAVITY_HORZ) {
            case ZS_GRAVITY_HCENTER: x = wmax / 2; break;
            case ZS_GRAVITY_END:     x = wmax; break;
        }
        caretScreen.x += x;
        caretIndex = 0;
    }
    updateCaret();
}

void zViewEdit::stateView(STATE &state, bool save, int &index) {
    zViewText::stateView(state, save, index);
    if(save) {
        state.data   += (u32)caretIndex;
        state.string = realText;
    } else {
        caretIndex = (int)state.data[index++];
        setText(state.string, true);
        post(MSG_EDIT, 10, 0);
    }
}

void zViewEdit::updateCaret() {
    auto caret(manager->getCaret());
    caret->update(this, caretScreen.x - rclient.x, caretScreen.y - rclient.y, getTextSize());
}

int zViewEdit::correct(int _index) {
    bool update(getLines() != 1);
    if(!update) {
        int idx(0);
        // длина видимого текста в пикселях
        auto widthText(defPaint->widthText(getDrawText(true), INT_MAX));
        if(widthText >= wmax) {
            // сдвинуть текст вперед на величину, которая больше макс. ширины текста
            idx = defPaint->indexOf(getDrawText(true), INT_MAX, widthText, wmax, false);
        } else if(visibleIndex > 0) {
            // сдвинуть текст назад на величину разницы макс. ширины текста и реальной ширины
            idx = -defPaint->indexReverseOf(filter->getText(realText), visibleIndex, wmax - widthText);
        }
        if(idx) {
            // корректировать начальный индекс текста/позицию каретки
            visibleIndex += idx;
            update = true;
        }
    } else {
        oldPos.empty();
    }
    if(update) {
        // установить новый текст
        setText(realText, true);
    }
    // получить новую позицию каретки на экране
    correctCaretPosition(_index);
    return _index;
}

void zViewEdit::changeTheme() {
    if(textCache.size()) caretIndex = correct(caretIndex);
    zView::changeTheme();
}

void zViewEdit::onLayout(crti &position, bool changed) {
    zViewText::onLayout(position, changed);
    if(but) but->layout(rti(rclient.x + wmax + 8 + distance + ipad.x, rclient.y + (rclient.h - but->rview.h) / 2, but->rview.w, but->rview.h));
    if(isFocus()) correctCaretPosition(caretIndex);
}

void zViewEdit::onMeasure(cszm& spec) {
    zViewText::onMeasure(spec);
    auto butW(icSize.w);
    if(but) but->measure({zMeasure(MEASURE_EXACT, butW), zMeasure(MEASURE_EXACT, icSize.h)}); else butW = 0;
    wmax = rclient.w - distance - (butW + 4) - ipad.extent(false);
}

void zViewEdit::clearText() {
    if(realText.isNotEmpty()) {
        clearSelected(false);
        setText("", true);
        caretIndex = correct(0);
        visibleIndex = 0;
        post(MSG_EDIT, 10, 0);
    }
}

void zViewEdit::onChangeFocus(bool set) {
    if(set) caretIndex = correct(caretIndex);
    invalidate();
}
