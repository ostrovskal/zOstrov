//
// Created by User on 01.05.2022.
//

#include "zostrov/zCommon.h"
#include "zostrov/zViewText.h"
#include "zostrov/zViewWidgets.h"

zBaseAdapter* zViewEdit::adapter(nullptr);

class zViewClear : public zViewButton {
public:
    explicit zViewClear(zViewEdit* _parent) : zViewButton(styles_z_editbutclear, 0, 0) {
        parent = _parent;
    }
    i32 onEvent() { return onTouchEvent(); }
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
    czs& getText(const zString8 &_text) override {
        if(_text.count() != text.count()) text = zString8('*', _text.count());
        return text;
    }
    zString8 text;
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
    if(!adapter) {
        adapter = new zAdapterList(theme->findArray(z.R.array.menuEditor), new zFabricListItem(styles_z_edit_menu_item));
    }
    adapter->registration(this);
}

zViewEdit::~zViewEdit() {
    SAFE_DELETE(filter);
    SAFE_DELETE(but);
    manager->getPopup()->dismiss();
    if(adapter->unregistration(this))
        SAFE_DELETE(adapter);
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
    auto x(bound->x), y(bound->y), size(textCache.size()); _indexText -= visibleIndex;
    // определить вертикальную позицию
    for(int i = 0 ; i < size; i++) {
        auto c(textCache[i]); auto idx(_indexText - c->text.count());
        if(idx < 0 || (i + 1 >= size)) {
            getStringFromCache(i, bound, pos);
            x = pos.x + defPaint->widthText(c->text.str(), _indexText);
            break;
        }
        _indexText = idx; y += c->size;
    }
    return {x, y};
}

int zViewEdit::indexFromPosition(cpti& screen, bool exact) {
    auto bound(&drw[DRW_TXT]->bound); pti pos(0, bound->y);
    auto idx(0), size(textCache.size());
    for(int i = 0 ; i < size; i++) {
        auto c(textCache[i]);
        if(screen.y < (pos.y + c->size) || (i + 1 >= size)) {
            getStringFromCache(i, bound, pos);
            idx += visibleIndex + defPaint->indexOf(c->text.str(), INT_MAX, screen.x, pos.x, exact);
            break;
        }
        pos.y += c->size; idx += c->text.count();
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
            but = new zViewClear(this); but->onInit(false);
            but->drw[DRW_FK]->tile = drw[DRW_ICON]->tile;
        }
    }
}

void zViewEdit::disable(bool _set) {
    if(but) but->disable(_set);
    zViewText::disable(_set);
}

void zViewEdit::onDraw() {
    zViewText::onDraw();
    if(but) but->draw();
}

i32 zViewEdit::keyEvent(int key, bool sysKey) {
    // применить фильтр
    if(filter && key >= ' ') {
        if(!(key = filter->convertChar(realText, key, caretIndex)))
            return 0;
    }
    if(key == '\n') {
        // enter
        post(MSG_EDIT_FINISH, 10, 0);
        manager->changeFocus(nullptr);
        invalidate(); key = 0;
    }
    if(isFocus() && !testFlags(ZS_READ_ONLY) && key) {
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
            caretIndex = correct(caretPos);
            post(MSG_EDIT, 10, caretIndex);
        }
    }
    return key != 0;
}

cstr zViewEdit::getDrawText(bool _real) {
    return (_real || realText.isNotEmpty()) ? z_ptr8(filter->getText(realText), visibleIndex) : hintText.str();
}

bool zViewEdit::clearSelected(bool del_text) {
    bool ret(false);
    if(del_text && bkgSpan && bkgSpan->length()) {
        removeText(bkgSpan->s, bkgSpan->length());
        if(caretIndex >= bkgSpan->s) caretIndex = correct(bkgSpan->s);
        ret = true; post(MSG_EDIT, 10, caretIndex);
    }
    spans.clear(); cacheSpans.clear(); bkgSpan = nullptr;
    invalidate(); return ret;
}

void zViewEdit::initContextMenu() {
    auto dropdown(manager->getDropdown(this, styles_z_menupopup, adapter));
    dropdown->measure(ctxSpec);
    manager->getPopup()->show(caretScreen.x - rclient.x, caretScreen.y - 80 - rclient.y, this, dropdown);
}

void zViewEdit::processContextMenu(int sel) {
    manager->getPopup()->dismiss();
    auto x(bkgSpan ? bkgSpan->s : caretIndex), l(bkgSpan ? bkgSpan->length() : 0);
    switch(sel) {
        // copy/cut/delete
        case 0: case 2: case 3:
            if(bkgSpan) {
                if(sel != 3) manager->exchangeBuffer = realText.substr(x + visibleIndex, l);
                if(sel >= 2) clearSelected(true);
                correct(caretIndex);
            }
            break;
        // paste
        case 1:
            if((realText.count() + manager->exchangeBuffer.count()) >= maxLength) break;
            clearSelected(true);
            zViewText::setText(realText.insert(x, manager->exchangeBuffer), true);
            caretIndex += manager->exchangeBuffer.count();
            post(MSG_EDIT, 10, correct(caretIndex));
            break;
    }
}

void zViewEdit::notifyEvent(HANDLER_MESSAGE* msg) {
    switch(msg->what) {
        case MSG_EDIT:
        case MSG_EDIT_FINISH:
            if(onChangeText) onChangeText(this, msg->what);
            break;
        case MSG_EDIT_CARET:
            if(onChangeCaretPos) onChangeCaretPos(this, msg->arg1);
            break;
        case MSG_EDIT_MENU:
            initContextMenu();
            break;
        case MSG_DROP_CLICK:
            processContextMenu(msg->arg1);
            break;
    }
    zViewText::notifyEvent(msg);
}

void zViewEdit::setSelected(int start, int end) {
    auto c(realText.count());
    start = z_min(c, z_max(0, start)); end = z_min(c, z_max(0, end));
    int p1(z_min(start, end)), p2(z_max(start, end));
    if(bkgSpan) {
        bkgSpan->s = p1; bkgSpan->e = p2;
    } else {
        bkgSpan = setSpan(new zTextSpanBackgroundColor(bkgHighlight), p1, p2);
    }
    updateText();
}

i32 zViewEdit::onTouchEvent() {
    auto result(zViewText::onTouchEvent());
    if(but && (but->testFlags(ZS_TAP) || but->rview.contains((int)touch->cpt.x, (int)touch->cpt.y))) {
        // вызываем событие касания дочернего
        result = but->onEvent();
        if(result == TOUCH_ACTION) setText("", true);
        but->updateStatus(ZS_TAP, result == TOUCH_FINISH);
        invalidate(); return result;
    }
    if(result == TOUCH_FINISH) {
        if(!touch->delta(sizeTouch) && !touch->just()) return result;
        // определить позицию каретки в тексте и на экране
        auto pos(indexFromPosition(pti((int)touch->cpt.x, (int)touch->cpt.y), true));
        if(touch->isDblClicked()) { posSel.x = 0;  pos = posSel.y = realText.count(); }
        if(posSel.x == INT_MIN) posSel.x = pos - visibleIndex;
        // нажали в выделение?
        if(bkgSpan) {
            bool isMove(true);
            if(posSel.x > bkgSpan->s && posSel.x < bkgSpan->e) isMove = (touch->isDragging(true) || touch->isDragging(false));
            if(isMove) clearSelected(false);
        }
        post(MSG_EDIT_MENU, 1000, 0);
        posSel.y = pos - visibleIndex;
        if(posSel.x != posSel.y) setSelected(posSel.x, posSel.y);
        if(pos != caretIndex) {
            post(MSG_EDIT_CARET, 3, pos);
            caretIndex = pos;
        }
        // обновить каретку
        correctCaretPosition(caretIndex);
    } else {
        if(!manager->getPopup()->isVisibled()) {
            if(bkgSpan && (caretIndex > bkgSpan->s && caretIndex < bkgSpan->e))
                clearSelected(false);
            manager->event.erase(this, MSG_EDIT_MENU);
        }
        posSel.x = posSel.y = INT_MIN;
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
    }
}

void zViewEdit::updateCaret() {
    if(isFocus()) {
        auto caret(manager->getCaret());
        caret->update(this, caretScreen.x - rclient.x, caretScreen.y - rclient.y, getTextSize());
    }
}

int zViewEdit::correct(int _index) {
    bool update(getLines() != 1);
    if(!update && wmax) {
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
    // обновить текст
    if(update) updateText();
    // получить новую позицию каретки на экране
    correctCaretPosition(_index);
    return _index;
}

void zViewEdit::changeTheme() {
    if(textCache.size()) caretIndex = correct(caretIndex);
    manager->getPopup()->changeTheme(); zView::changeTheme();
}

void zViewEdit::onLayout(crti &position, bool changed) {
    zViewText::onLayout(position, true);
    if(but) {
        but->layout(rti(rclient.x + wmax + distance, rclient.y + (rclient.h - but->rview.h) / 2,
                        but->rview.w, but->rview.h));
    }
    if(isFocus()) correctCaretPosition(caretIndex);
}

void zViewEdit::onMeasure(cszm& spec) {
    zViewText::onMeasure(spec);
    auto butW(icSize.w);
    if(but) but->measure({zMeasure(MEASURE_EXACT, butW), zMeasure(MEASURE_EXACT, icSize.h)}); else butW = 0;
    wmax = rclient.w - distance - butW - ipad.extent(false);
}

zViewText* zViewEdit::setText(czs& _text, bool force) {
    clearSelected(false);
    zViewText::setText(_text, force);
    visibleIndex = 0; caretIndex = correct(_text.count());
    post(MSG_EDIT, 10, caretIndex);
    return this;
}

void zViewEdit::onChangeFocus(bool set) {
    if(set) {
        correct(caretIndex);
    } else {
        auto caret(manager->getCaret());
        caret->update(nullptr, 0, 0, 0);
    }
}
