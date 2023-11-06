
#pragma once

#include "zViewGroup.h"
#include "zViewWidgets.h"

class zViewKeyboard : public zViewGroup {
public:
    enum { KEYBOARD_NAME, KEYBOARD_SHIFT, KEYBOARD_DIGIT, KEYBOARD_LANG, KEYBOARD_INPUT, KEYBOARD_SPACE, KEYBOARD_ENTER, KEYBOARD_DELETE };
    // конструктор
    zViewKeyboard(cstr nameLayouts, zStyle* _styles_base = nullptr, zStyle* _styles_alt = nullptr, int _id = z.R.id.keyboard);
    // деструктор
    virtual ~zViewKeyboard();
    // загрузка стилей(отключено)
    virtual void onInit(bool theme) override;
    // обработка системной кнопки
    virtual i32 keyEvent(int key, bool sysKey) override;
    // имя типа
    virtual zString8 typeName() const override { return "zViewKeyboard"; }
    // отобразить/скрыть
    virtual void show(u32 _id, bool set);
    // установка текущего макета
    void setLayout(czs& name);
    // не обновлять
    virtual void requestLayout() override;
    // вернуть идентификатор владельца
    u32 getOwnerID() const { return owner ? owner->id : 0; }
protected:
    struct BUTTON {
        rti rview{};
        zString8 name[2]{};
        zString8 spec{};
        int icon{-1}, size{0};
        u32 color{0};
    };
    struct LAYOUT {
        LAYOUT() { }
        ~LAYOUT() { }
        bool operator == (cstr _name) const { return names[0] == _name; }
        // размер макета
        szi size{};
        // имена макетов
        zString8 names[5];
        // кнопки
        zArray<BUTTON> buttons{};
    };
    // внутренние события
    virtual void notifyEvent(HANDLER_MESSAGE *msg) override;
    // размеры
    virtual void onMeasure(cszm& spec) override;
    // позиция
    virtual void onLayout(crti &position, bool changed) override;
    // событие касания
    virtual i32 onTouchEvent() override;
    virtual bool touchSelf() override { return true; }
    // отрисовка FBO
    virtual void onDrawFBO() override;
    virtual void onDraw() override;
    // проверка на повторение нажатия
    virtual bool repeatPressed(BUTTON* but, int& code) const;
    // обработка специальных
    virtual bool processSpecPressed(BUTTON* but, int& code, zString8** _switch, bool dblClick);
    // смещение по высоте
    int offsetY{0};
    // дельты
    float deltaWidth{0}, deltaHeight{0};
    // минимальная высота
    int minHeight{0};
    // код кнопки
    int keyCode{0};
    // предыдущая клава/по умолчанию
    zString8 prevName, defName;
    // все макеты
    zArray<LAYOUT*> layouts;
    // текущий макет
    LAYOUT* current{nullptr};
    // владелец
    zView* owner{nullptr};
    // родитель владельца
    zView* parent{nullptr};
    // стили кнопок
    zStyle* styles_base{nullptr};
    zStyle* styles_alt{nullptr};
    // признак обновления макета
    bool isUpdate{false};
    // текущая кнопка
    int butIdx{-1};
    // признак нажатия спец. кнопки
    int nPressSpec{0};
    // признак актинового шифта
    bool activeShift{false};
    // признак рисования
    bool isDrawing{true};
    // нижняя граница владельца
    int scrollY{0};
};
