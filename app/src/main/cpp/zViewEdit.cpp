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
    but             = new zViewClear(this);
}

zViewEdit::~zViewEdit() {
    SAFE_DELETE(filter);
    SAFE_DELETE(but);
}

void zViewEdit::setFilter(int inputType, zFilterEdit* _filter) {
    if(!_filter) {
        switch(inputType & ZS_MODE_EDIT_MASK) {
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

int zViewEdit::indexFromPosition(int inScreen, bool exact, int *outScreen) {
    return drw[DRW_TXT]->indexOf(getDrawText(true), getTextSize(), inScreen, drw[DRW_TXT]->bound.x, exact, outScreen);
}

int zViewEdit::positionFromIndex(int _indexText) {
    return drw[DRW_TXT]->bound.x + drw[DRW_TXT]->sizeText(getDrawText(true), getTextSize(), _indexText);
}

void zViewEdit::onInit(bool _theme) {
    zViewText::onInit(_theme);
    status          |= ZS_NOWRAP | ZS_FOCUSABLE_IN_TOUCHABLE;
    colorHint       = styles->_int(Z_COLOR_HINT_TEXT, 0xff505050);
    setFilter((int)styles->_int(Z_MODE, ZS_EDIT_TEXT));
/*
    drw[DRW_ICON] = new zDrawable(this, DRW_ICON);
    drw[DRW_ICON]->init(z.R.drawable.zssh, 0xffffffff, z.R.integer.iconEdit, 0, 1.0f);
    setIconGravity(ZS_GRAVITY_END | ZS_GRAVITY_VCENTER);
*/
    but->onInit(false);
}

void zViewEdit::onDraw() {
    zViewText::onDraw();
    but->draw();
}

void zViewEdit::insertText(int pos, cstr _text) {
    setText(realText.insert(pos, _text), true);
}

void zViewEdit::removeText(int pos, int count) {
    setText(realText.remove(pos, count), true);
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
        updateText(0);
        manager->changeFocus(nullptr);
    } else {
        auto caretPos(caretIndex);
        if(key == '\b') {
            if(caretPos > 0) {
                removeText(visibleIndex + --caretPos, 1);
            }
        } else {
            insertText(visibleIndex + caretPos++, (char *) &key);
        }
        if(caretPos != caretIndex) {
            updateText(caretIndex);
            caretIndex = correct(caretPos);
            updateCaret();
        }
    }
    return 1;
}

i32 zViewEdit::onTouchEvent(zTouch *touch) {
    i32 result(touch->isCaptured());
    auto isButton(but->rview.contains((int)touch->cpt.x, (int)touch->cpt.y));
    if(isButton) {
        // вызываем событие касания дочернего
        result = but->onEvent(touch);
        if(result == TOUCH_ACTION) {
            clearText();
            result = TOUCH_CONTINUE;
        }
    }
    if(result == TOUCH_FINISH) {
        // определить позицию каретки в тексте и на экране
        auto pos(indexFromPosition((int)round(touch->cpt.x), true, &caretScreen.x));
        if(pos != caretIndex) {
            if(onChangeCaretPos) onChangeCaretPos(this, pos);
            caretIndex = pos;
        }
        correctCaretPosition(caretIndex);
        // обновить каретку
        updateCaret();
    } else isButton = true;
    if(isButton) {
        but->updateStatus(ZS_TAP, result == TOUCH_FINISH);
        invalidate();
    }
    return result;
}

void zViewEdit::correctCaretPosition(int _index) {
    caretScreen.x = positionFromIndex(_index);
}

void zViewEdit::stateView(STATE &state, bool save, int &index) {
    zViewText::stateView(state, save, index);
    if(save) {
        state.data   += (u32)caretIndex;
        state.string = realText;
    } else {
        caretIndex = (int)state.data[index++];
        setText(state.string, true);
        updateText(0);
//        post(MSG_EDIT, nullptr, 10);
    }
}

void zViewEdit::updateCaret() {
    auto caret(manager->getCaret());
    caret->update(this, caretScreen.x - rclient.x, ipad.y, rclient.h - ipad.extent(true));
}

int zViewEdit::correct(int _index) {
    int idx(0), size(getTextSize());
    // длина видимого текста в пикселях
    auto widthText(drw[DRW_TXT]->sizeText(getDrawText(true), size, INT_MAX));
    if(widthText >= wmax) {
        // сдвинуть текст вперед на величину, которая больше макс. ширины текста
        idx = drw[DRW_TXT]->indexOf(getDrawText(true), size, widthText, wmax);
    } else if(visibleIndex > 0) {
        // сдвинуть текст назад на величину разницы, макс. ширины текста и реальной ширины
        idx = -drw[DRW_TXT]->indexReverseOf(filter->getText(realText), size, wmax - widthText, visibleIndex);
    }
    if(idx) {
        // корректировать начальный индекс текста/позицию каретки
        visibleIndex += idx; _index -= idx;
        // установить новый текст(при возможности без обновления макета)
        setText(realText, true);
        // получить новую позицию каретки на экране
        correctCaretPosition(_index);
    }
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
    but->layout(rti(rclient.x + wmax + ipad.x, rclient.y + (rclient.h - but->rview.h) / 2, but->rview.w, but->rview.h));
    if(isFocus()) {
        correctCaretPosition(caretIndex);
        updateCaret();
    }
}

void zViewEdit::onMeasure(cszm& spec) {
    zViewText::onMeasure(spec);
    auto rh(rclient.h - ipad.extent(true));
    but->measure(szm(zMeasure(MEASURE_EXACT, rh), zMeasure(MEASURE_EXACT, rh)));
    wmax = rclient.w - rh - ipad.extent(false);
}

void zViewEdit::clearText() {
    if(realText.isNotEmpty()) {
        setText("", true);
        caretIndex = correct(0);
        visibleIndex = 0; updateCaret();
//        post(MSG_EDIT, nullptr, 100);
    }
}

void zViewEdit::onChangeFocus(bool set) {
    if(set) {
        caretIndex = correct(caretIndex);
        updateCaret();
    }
    invalidate();
}

cstr zViewEdit::getDrawText(bool _real) {
    if(_real) return z_ptrUTF8(filter->getText(realText), visibleIndex);
    return (realText.isNotEmpty() ? z_ptrUTF8(filter->getText(realText), visibleIndex) : hintText.str());
}

u32 zViewEdit::getDrawColorText(zTextPaint* paint) {
    return (realText.isNotEmpty() ? paint->fkColor : colorHint);
}
