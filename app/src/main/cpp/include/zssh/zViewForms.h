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
    // тип
    virtual cstr typeName() const override { return "zViewForm"; }
    // событие кнопки
    virtual i32 keyEvent(int key, bool sysKey) override;
    virtual rti drawableClip() const override { return { 0, 0, 65535, 65535 }; }
    // инициализация
    virtual void onInit(bool _theme) override;
    // установка события закрытия формы
    void setOnClose(std::function<int(zView*, int code)> _close) { onClose = std::move(_close); }
    // закрыть форму
    void close(int code);
    // добавить элемент в контент
    zView* setContent(zView* view);
protected:
    // отрисовка fbo
    virtual void onDrawFBO() override;
    // заголовок
    u32 caption{0};
    // контент
    zFrameLayout* content{nullptr};
    // событие закрытия
    std::function<int(zView*, int)> onClose;
    // режим
    u32 mode{0};
    // сохраненные значения
    zStyle* _styles_button_footer{nullptr};
    zStyle* _styles_header{nullptr};
};

class zViewFormMessage : public zViewForm {
public:
    // конструктор
    zViewFormMessage(zStyle* _styles, i32 _id, zStyle* _styles_caption, zStyle* _styles_footer, u32 _caption, u32 _message, bool _modal);
    // загрузка стилей
    virtual void onInit(bool theme) override;
protected:
    // сообщение
    u32 message{0};
};