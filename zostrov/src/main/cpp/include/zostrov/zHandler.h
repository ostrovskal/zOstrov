
#pragma once

class zView;

struct HANDLER_MESSAGE {
    void init(const HANDLER_MESSAGE& m) {
        what = m.what; millis = m.millis; view = m.view;
        arg1 = m.arg1; arg2 = m.arg2; sarg = m.sarg;
    }
    // сообщение и аргументы
    int what{0};
    // задержка
    i64 millis{0};
    // владелец
    zView *view{nullptr};
    // целые аргументы
    int arg1{0}, arg2{0};
    // строковый аргумент
    zString8 sarg{};
};

class zHandler {
public:
    zHandler() { }
    // удалить определенные сообщения
    void erase(zView *view, int what);
    // очистить все
    void clear();
    // отобразить информацию
    void info();
    // оправить сообщение
    void send(zView* view, int what, u64 millis = 0, int arg1 = 0, int arg2 = 0, cstr s = "");
    // получить сообщение для модификации
    HANDLER_MESSAGE *get(zView *view, int what);
    // получить сообщение
    HANDLER_MESSAGE *obtain();
protected:
    int use{0};
    HANDLER_MESSAGE looper[64];
    // сообщение для отправки
    HANDLER_MESSAGE message{};
};
