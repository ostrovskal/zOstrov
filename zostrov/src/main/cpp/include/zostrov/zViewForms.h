//
// Created by User on 13.04.2022.
//

#pragma once

#include "zViewGroup.h"
#include "zViewWidgets.h"

class zViewForm : public zLinearLayout {
    friend class zViewManager;
public:
    // конструктор
    zViewForm(zStyle* _styles, i32 _id, zStyle* _styles_caption, zStyle* _styles_footer, u32 _caption, bool _modal);
    virtual ~zViewForm() { delete shade; }
    // тип
    virtual cstr typeName() const override { return "zViewForm"; }
    // событие кнопки
    virtual i32 keyEvent(int key, bool sysKey) override;
    // сохранение/восстановление
    virtual void stateView(STATE& state, bool save, int& index) override;
    // инициализация
    virtual void onInit(bool _theme) override;
    // обновление видимости
    virtual int updateVisible(bool set) override;
    // добавление дочерних
    virtual zView* attach(zView *v, int width, int height, int where = -1) override;
    // установка события закрытия формы
    void setOnClose(std::function<int(zViewForm*, int code)> _close) { onClose = std::move(_close); }
    // закрыть форму
    void close(int code);
    // добавить элемент в контент
    zView* setContent(zView* view);
protected:
    virtual void onLayout(crti& pos, bool changed) override;
    // обработка касания
    virtual i32 touchEventDispatch(AInputEvent *event) override;
    // область обрезки
    virtual rti drawableClip() const override { return { 0, 0, 65535, 65535 }; }
    // отрисовка fbo
    virtual void onDrawFBO() override;
    // заголовок
    u32 caption{0};
    // контент
    zFrameLayout* content{nullptr};
    // событие закрытия
    std::function<int(zViewForm*, int)> onClose;
    // режим
    u32 mode{0};
    // признак инициализации
    bool isOpen{false};
    // сохраненные значения
    zStyle* _styles_button_footer{nullptr};
    zStyle* _styles_header{nullptr};
    // затемнение
    zDrawable* shade{nullptr};
};

class zViewFormMessage : public zViewForm {
public:
    // конструктор
    zViewFormMessage(zStyle* _styles, i32 _id, zStyle* _styles_caption, zStyle* _styles_footer, u32 _caption, const zString8& _message);
    // загрузка стилей
    virtual void onInit(bool theme) override;
    // установка сообщения
    void setMessage(zString8 _message) { message = std::move(_message); }
protected:
    // сообщение
    zString8 message{};
};

class zViewFormWait : public zViewForm {
public:
    // конструктор
    zViewFormWait(i32 _id, int width, int height);
    virtual ~zViewFormWait() {
        auto _parent(getParent());
        if(_parent) _parent->detach(this);
    }
    // установка прогресса
    void setProgress(cszi& sz, int _pos) {
        auto w(idView<zViewProgress>(z.R.id.wait));
        w->setRange(sz); w->setProgress(_pos);
    }
protected:
    virtual i32 touchEventDispatch(AInputEvent *event) override {
        return TOUCH_CONTINUE;
    }
};
