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
    int convertChar(const zStringUTF8& t, int ch, int pos) override {
        if(ch == '@' && t.indexOf("@") != -1) ch = 0;
        return ch;
    }
};

class zFilterPhone : public zFilterEdit {
public:
    int convertChar(const zStringUTF8& t, int ch, int pos) override {
        return ch * (pos ? isdigit(ch) : (ch != '+'));
    }
    [[nodiscard]] cstr getKeboardLayer() const override { return "digit123"; }
};

class zFilterPassword : public zFilterEdit {
public:
    const zStringUTF8& getText(const zStringUTF8 &_text) override {
        if(_text.count() != text.count()) text = zStringUTF8('*', _text.count());
        return text;
    }
    zStringUTF8 text;
};

class zFilterZx : public zFilterEdit {
public:
    int convertChar(const zStringUTF8& t, int ch, int pos) override {
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
    int convertChar(const zStringUTF8& t, int ch, int pos) override {
        return ch * (ch == '.' ? (t.indexOf(".") != -1) : isdigit(ch));
    }
    [[nodiscard]] cstr getKeboardLayer() const override { return "digit123"; }
};

zViewEdit::zViewEdit(zStyle *_stl, i32 _id, u32 _hint) : zViewText(_stl, _id, 0) {
    if(_hint) hintText = theme->findString(_hint);
    minMaxSize.x    = z_dp(z.R.dimen.editMinWidth);
    minMaxSize.w    = z_dp(z.R.dimen.editMinHeight);
}

zViewEdit::~zViewEdit() {
    SAFE_DELETE(filter);
    SAFE_DELETE(but);
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
            x = pos.x + drw[DRW_TXT]->sizeText(c->text, c->size, _indexText);
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
            idx += visibleIndex + drw[DRW_TXT]->indexOf(c->text, c->size, screen.x, pos.x, exact, &pos.x);
            break;
        }
        pos.y += c->size; idx += c->text.count(); i++;
    }
    return idx;
}

void zViewEdit::onInit(bool _theme) {
    zViewText::onInit(_theme);
    status          |= ZS_FOCUSABLE_IN_TOUCHABLE;
    colorHint       = styles->_int(Z_COLOR_HINT_TEXT, 0xff505050);
    setFilter(styles->_int(Z_MODE, ZS_EDIT_TEXT));
    // создать кнопку, если есть иконка
    if(drw[DRW_ICON]->isValid()) {
        if(!but) {
            but = new zViewClear(this);
            but->onInit(false);
            but->drw[DRW_FK]->tile = drw[DRW_ICON]->tile;
        }
    }
}

void zViewEdit::onDraw() {
    zViewText::onDraw();
    if(but) but->draw();
}

i32 zViewEdit::keyEvent(int key, bool sysKey) {
    if(testFlags(ZS_READ_ONLY)) return 0;
    // применить фильтр
    if(filter && key >= ' ') {
        if(!(key = filter->convertChar(realText, key, caretIndex)))
            return 0;
    }
    if(key == '\n') {
        // enter
        updateText(MSG_EDIT_FINISH);
        manager->changeFocus(nullptr);
    } else if(key) {
        auto caretPos(caretIndex);
        if(key == '\b') {
            if(caretPos > 0) removeText(--caretPos, 1);
        } else insertText(caretPos++, (char *) &key);
        if(caretPos != caretIndex) {
            updateText(MSG_EDIT);
            caretIndex = correct(caretPos);
            updateCaret();
        }
    }
    return 1;
}

i32 zViewEdit::onTouchEvent(zTouch *touch) {
    i32 result(touch->isCaptured());
    auto isButton(but && but->rview.contains((int) touch->cpt.x, (int) touch->cpt.y));
    if(isButton) {
        // вызываем событие касания дочернего
        result = but->onEvent(touch);
        if(result == TOUCH_ACTION) { clearText(); result = TOUCH_CONTINUE; }
    }
    if(result == TOUCH_FINISH) {
        // определить позицию каретки в тексте и на экране
        auto pos(indexFromPosition(pti(touch->cpt.x, touch->cpt.y), true));
        if(pos != caretIndex) {
            if(onChangeCaretPos) onChangeCaretPos(this, pos);
            caretIndex = pos;
        }
        correctCaretPosition(caretIndex);
        // обновить каретку
        updateCaret();
    } else isButton = true;
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
}

void zViewEdit::stateView(STATE &state, bool save, int &index) {
    zViewText::stateView(state, save, index);
    if(save) {
        state.data   += (u32)caretIndex;
        state.string = realText;
    } else {
        caretIndex = (int)state.data[index++];
        setText(state.string, true);
        updateText(MSG_EDIT);
    }
}

void zViewEdit::updateCaret() {
    auto caret(manager->getCaret());
    caret->update(this, caretScreen.x - rclient.x, caretScreen.y - rclient.y, getTextSize());
}

int zViewEdit::correct(int _index) {
    bool update(!isWrap());
    if(!update) {
        int idx(0), size(getTextSize());
        // длина видимого текста в пикселях
        auto widthText(drw[DRW_TXT]->sizeText(getDrawText(true), size, INT_MAX));
        if(widthText >= wmax) {
            // сдвинуть текст вперед на величину, которая больше макс. ширины текста
            idx = drw[DRW_TXT]->indexOf(getDrawText(true), size, widthText, wmax);
        } else if(visibleIndex > 0) {
            // сдвинуть текст назад на величину разницы макс. ширины текста и реальной ширины
            idx = -drw[DRW_TXT]->indexReverseOf(filter->getText(realText), size, wmax - widthText, visibleIndex);
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
    if(textCache.size()) {
        caretIndex = correct(caretIndex);
        if(isFocus()) updateCaret();
    }
    zView::changeTheme();
}

void zViewEdit::onLayout(crti &position, bool changed) {
    zViewText::onLayout(position, changed);
    if(but) but->layout(rti(rclient.x + wmax + distance + ipad.x, rclient.y + (rclient.h - but->rview.h) / 2, but->rview.w, but->rview.h));
    if(isFocus()) {
        correctCaretPosition(caretIndex);
        updateCaret();
    }
}

void zViewEdit::onMeasure(cszm& spec) {
    zViewText::onMeasure(spec);
    auto butW(icSize.w);
    if(but) but->measure(szm(zMeasure(MEASURE_EXACT, butW), zMeasure(MEASURE_EXACT, icSize.h))); else butW = 0;
    wmax = rclient.w - distance - butW - ipad.extent(false);
}

void zViewEdit::clearText() {
    if(realText.isNotEmpty()) {
        setText("", true);
        caretIndex = correct(0);
        visibleIndex = 0; updateCaret();
        updateText(MSG_EDIT);
    }
}

void zViewEdit::onChangeFocus(bool set) {
    if(set) {
        caretIndex = correct(caretIndex);
        updateCaret();
    }
    invalidate();
}
