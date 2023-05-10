
#pragma once

class zView;

struct HANDLER_MESSAGE {
    // сообщение и аргументы
    int what{0};
    // задержка
    i64 millis{0};
    // владелец
    zView *view{nullptr};
    // аргумент
    int arg{0};
};

struct HANDLER_TOUCH_MESSAGE {
    // владелец
    zView *view{nullptr};
    // признак валидности
    bool valid{false};
    // связанные данные
    zTouch touch;
};

class zHandler {
public:
    zHandler() {}
    // удалить определенные сообщения
    void erase(zView *view, int what);
    // очистить все
    void clear();
    // отобразить информацию
    void info();
    // оправить сообщение
    void send(zView* view, int what, u64 millis = 0, int arg = 0);
    // получить сообщение для модификации
    HANDLER_MESSAGE *get(zView *view, int what);
    // получить сообщение
    HANDLER_MESSAGE *obtain();
protected:
    int use{0};
    HANDLER_MESSAGE looper[32];
};

class zTouchHandler {
public:
    // конструкторы
    zTouchHandler() { }
    // показать информацию
    void info();
    // удалить определенные сообщения
    void erase(zView *view);
    // очистить все
    void clear();
    // получить сообщение для модификации
    HANDLER_TOUCH_MESSAGE *get(zView *view);
    // получить сообщение
    HANDLER_TOUCH_MESSAGE *obtain();
protected:
    int use{0};
    HANDLER_TOUCH_MESSAGE looper[10];
};