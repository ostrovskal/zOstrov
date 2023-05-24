//
// Created by User on 18.04.2023.
//

#pragma once

#include "zHandler.h"
#include "zDrawable.h"

class zMeasure {
public:
    zMeasure() { }
    zMeasure(int _val) : value(_val) { }
    zMeasure(const zMeasure& _val) : value(_val) { }
    zMeasure(int _mode, int _size) : value((_mode & MEASURE_MASK_MODE) | (_size & MEASURE_MASK_SIZE)) { }
    int size() const { return (value & MEASURE_MASK_SIZE); }
    int mode() const { return (value & MEASURE_MASK_MODE); }
    bool isExact() const { return mode() == MEASURE_EXACT; }
    bool isNotExact() const { return mode() != MEASURE_EXACT; }
    operator int() const { return value; }
    int value{0};
};

class zLayoutParams {
public:
    zLayoutParams() { }
    zLayoutParams(int _w, int _h) : size(_w, _h) { }
    zLayoutParams(int _x, int _y, int _w, int _h) : x(_x), y(_y), w(_w), h(_h) { }
    zLayoutParams(int* p) : x(p[0]), y(p[1]), w(p[2]), h(p[3]) { }
    zLayoutParams(const zLayoutParams& lp) : x(lp.x), y(lp.y), size(lp.size) { }
    zMeasure operator [](int index) const { return buf[index]; }
    union {
        struct {
            union { int x{0}, gravity; };
            union { int y{0}, weight; };
            union {
                zSize<zMeasure> size{0, 0};
                struct { zMeasure w, h; };
            };
        };
        zMeasure buf[4];
    };
};

using szm  = zSize<zMeasure>;
using cszm = const zSize<zMeasure>;

class zViewGroup;
// основной класс представления
class zView {
    friend class zViewManager;
    friend class zViewGroup;
    friend class zDrawable;
public:
    union TAG {
        char* str{nullptr};
        u32 u; float f;
    };
    struct STATE {
        void reset() { data.free(); string.empty(); }
        // идентификатор представления
        u32 id{0};
        // тип представления
        u32 type{0};
        // связанные данные
        zArray<u32> data;
        // строка текста
        zString string;
    };
    // конструкторы
    zView() { }
    zView(zStyle* _styles, i32 _id = 0);
    // деструктор
    virtual ~zView();
    // отрисовка
    virtual void draw();
    // нажата кнопка
    virtual i32 keyEvent(int key, bool sysKey) { return 1; }
    // проверка по идентификатору
    bool operator == (u32 _id) const { return id == _id; }
    // проверка по объекту
    bool operator == (zView* v) const { return this == v; }
    // установить стили
    virtual void setStyles(zStyle* styles);
    // касание
    virtual i32 touchEvent(AInputEvent* event);
    // выделить
    virtual void checked(bool set);
    // спрятать/отобразить
    virtual void visibility(bool _visible) { if(isVisibled() != _visible) { updateStatus(ZS_VISIBLED, _visible); requestLayout(); } }
    // блокировка/разблокировка
    virtual void disable(bool _disable) { updateStatus(ZS_DISABLED, _disable); }
    // запрос на полное обновление иерархии представлений
    virtual void requestLayout();
    // запрос на обновление координат иерархии представлений
    virtual void invalidate();
    // подсчитать габариты
    virtual void measure(cszm& spec);
    // позиционировать
    virtual void layout(crti& position);
    // изменение темы
    virtual void changeTheme();
    // сохранение/загрузка
    virtual void stateView(STATE &state, bool save, int &index);
    // клавиатура по умолчанию
    virtual cstr getDefaultKeyboardLayer() const { return ""; }
    // событие инициализации
    virtual void onInit(bool theme);
    // отправка сообщения
    virtual void post(int what, u64 millis, int arg);
    // требование фокуса
    virtual void requestFocus();
    // уведомление адаптера
//    virtual void notifyAdapter(zAdapter* adapter) { }
    // уведомление владельца
    virtual void notifyOwner(int what, zView *view, int arg) { }
    // фоновая обработка
    virtual bool backgroundProcess() { return false; }
    // вернуть имя типа
    virtual cstr typeName() const { return "zView"; }
    // установка альфы
    void setAlpha(float _alpha);
    // установка масштаба
    void setScale(float _x, float _y);
    // установка ротации
    void setRotation(float _x, float _y, float _z);
    // установка смещения
    void setTranslation(float _x, float _y);
    // обновление статуса
    u32 updateStatus(u32 msk, u32 val, bool set) { status &= ~msk; val = (val & msk) * set; status |= val; return val; }
    u32 updateStatus(u32 val, bool set) { status &= ~val; val *= set; status |= val; return val; }
    // установка паддинга
    void setPadding(crti& r) { pad = r; requestLayout(); }
    // удаление отображателя
    void removeDrawable(int index) { if(drw[index] != &fake) delete drw[index]; drw[index] = &fake; }
    // установка маргина
    void setMargin(crti& r) { margin = r; requestLayout(); }
    // вернуть паддинг + маргин
    int padMargin(bool vert) const { return pad.extent(vert) + margin.extent(vert); }
    // вернуть признак ориентации
    bool isVertical() const { return (status & ZS_VORIENTATION); }
    // вернуть признак доступности
    bool isEnabled() const { return !(status & ZS_DISABLED); }
    // вернуть признак видимости
    bool isVisibled() const { return (status & ZS_VISIBLED); }
    // вернуть признак выделения
    bool isChecked() const { return (status & ZS_CHECKED); }
    // вернуть признак FBO
    bool isFBO() const { return drw[DRW_FBO]->isValid(); }
    // вернуть признак перерисовки
    bool isDirty() const { return (status & ZS_DIRTY_LAYER); }
    // признак возможности клика
    bool isClickabled() const { return (status & ZS_CLICKABLE); }
    // признак возможности долгого клика
    bool isLongClickabled() const { return (status & ZS_LCLICKABLE); }
    // признак способности касания
    bool isTouchable() const { return status & ZS_TOUCHABLE; }
    // признак способности к фокусу
    bool isFocusable() const { return status & ZS_FOCUSABLE; }
    // признак способности фокуса при касании
    bool isFocusableInTouch() const { return status & ZS_FOCUSABLE_IN_TOUCHABLE; }
    // признак наличия фокуса
    bool isFocus() const;
    // вернуть грань
    int edges(bool vert, bool end) const { return rview[vert] + end * rview[vert + 2]; }
    // вернуть размер
    int sizes(bool vert) const { return rview[vert + 2] + margin.extent(vert); }
    // проверка статуса на флаг
    bool testFlags(int flags) const { return (status & flags) != 0; }
    // установить тэг
    void setTag(const TAG& _tag) { tag = _tag; }
    // установить отображателя
    template<typename T = zDrawable> void setDrawable(zParamDrawable* params, int index, T** var = nullptr) {
        if(var) *var = nullptr;
        if(params && !params->texture) {
            removeDrawable(index);
        } else {
            if(!drw[index]->isValid()) drw[index] = new T(this, index);
            if(params) drw[index]->init(*params);
            if(var) *var = (T *) drw[index];
            if(index == DRW_FBO) { updateStatus(ZS_DIRTY_LAYER, true); }
        }
    }
    // установка клика
    zView* setOnClick(std::function<void(zView*, bool)> _click) { onClick = std::move(_click); return this; }
    // установка касания
    zView* setOnTouch(std::function<int(zView*, zTouch*)> _touch) { onTouch = std::move(_touch); return this; }
    // установка анимации
    zView* setOnAnimation(std::function<int(zView*, int)> _anim) { onAnimation = std::move(_anim); return this; }
    // вернуть родителя
    template<typename T = zViewGroup> T* getParent() const { return dynamic_cast<T*>(parent); }
    // crc типа
    u32 crcType() const { return (u32)z_crc64(typeName()); }
    // установка фактора масштабирования
    void setForegroundScale(float _scale) { drw[DRW_FK]->scale = _scale; requestLayout(); }
    // вернуть фактор масштабирования
    float getForegroundScale() const { return drw[DRW_FK]->scale; }
    // установить гравитацию
    zView* setGravity(u32 _gravity) { _setGravity(_gravity & ZS_GRAVITY_MASK, &gravity); return this; }
    // установить гравитацию иконки
    zView* setIconGravity(u32 _gravity) { _setGravity(_gravity & ZS_GRAVITY_MASK, &icGravity); return this; }
    // установить гравитацию фореграунда
    zView* setForegroundGravity(u32 _gravity) { _setGravity(_gravity & ZS_GRAVITY_MASK, &fkGravity); return this; }
    // отрисовка отладочной информации
    virtual void drawDebug();
    // альфа
    float alpha{1.0f};
    // ротация/масштаб/смещение
    zVec3 rot{}, trans{};
    szf   scale{1, 1};
    // матрицы
    zMatrix mtxScale{}, mtxRot{}, mtx{};
    // задержка анимации
    u64 duration{50L};
    // привязанное некое значение
    TAG tag{0};
    // идентификатор
    i32 id{0};
    // состояние
    u32 status{ZS_VISIBLED | ZS_CLICKABLE | ZS_TOUCHABLE | ZS_FOCUSABLE | ZS_DIRTY_LAYER};
    // общая гравитация/гравитация фореграунда/гравитация иконки
    u8 gravity{0}, fkGravity{0}, icGravity{0};
    // позиция прокрутки
    pti scroll{};
    // минимальный/максимальный размер
    rti minMaxSize{};
    // клиентский рект/реальный рект/рект обрезки
    rti rclient{}, rview{}, rclip{};
    // параметры макета
    zLayoutParams lps{};
    // отступ/внутренний отступ
    rti pad{}, ipad{ 5, 5, 5, 5 };
    // внешний отступ
    rti margin{};
    // ячейки сетки
    szi cellDebug{};
    // интерполятор
    zInterpolator animator{};
    // буферы отрисовки
    zDrawable* drw[5]{};
    // стили
    zStyle* styles{nullptr};
    // событие клика
    std::function<void(zView*, bool)> onClick;
    // событие касания
    std::function<int(zView*, zTouch*)> onTouch;
    // событие анимации
    std::function<int(zView*, int)> onAnimation;
protected:
    // установка гравитации
    void _setGravity(u32 _g, u8* p) { if(_g != (*p & ZS_GRAVITY_MASK)) { *p &= ~ZS_GRAVITY_MASK; *p |= (u8)_g; requestLayout(); } }
    // область обрезки
    virtual rti drawableClip() const { return z_clipRect(parent->rclip, rview); }
    // проверка на режим блокировки
    virtual bool testLocked() const { return false; }
    // событие привязки к макету
    virtual void onAttach() { }
    // событие отключения от макета
    virtual void onDetach() { }
    // событие вычисления размеров
    virtual void onMeasure(cszm& spec) { }
    // событие позиционирования элементов
    virtual void onLayout(crti &position, bool changed);
    // событие изменение фокуса
    virtual void onChangeFocus(bool set) { }
    // событие отрисовки
    virtual void onDraw() { }
    // событие отрисовки fbo
    virtual void onDrawFBO() { drw[DRW_FBO]->draw(&rview); }
    // событие касания
    virtual i32 onTouchEvent(zTouch *touch);
    // применение гравитации
    virtual pti applyGravity(crti& sizeParent, crti& sizeChild, u32 _gravity);
    // отсоединение
    virtual void _detach();
    // установка
    void setMeasuredDimension(int width, int height);
    // вернуть объект касания
    HANDLER_TOUCH_MESSAGE* getTouchHandler(AInputEvent* event);
    // стереть событие касания
    void eraseTouchHandler();
    // установка габаритов по умолчанию
    void defaultOnMeasure(cszm& spec, int width, int height);
    // родитель
    zView* parent{nullptr};
    // буфер отрисовки при отладке
    zDrawable drwDebug{this, -1};
    // спецификаторы габаритов
    szm measureSpec{};
    // предыдущая позиция
    rti oldPos{INT_MAX, INT_MAX};
    // фейковый отображатель
    static zDrawableFake fake;
    // текущая FBO
    static zView* fbo;
};

// глоу эффект
class zViewGlow : public zView {
public:
    // конструктор
    zViewGlow(zView* group);
    // запуск
    void start(float delta, bool vert, bool flow);
    // вернуть имя типа
    virtual cstr typeName() const override { return "zViewGlow"; }
    // остановка
    void stop() { animator.clear(); updateStatus(ZS_VISIBLED, false); }
protected:
    // область обрезки
    virtual rti drawableClip() const override { return parent->rclip; }
};

// прокрутка
class zViewScrollBar : public zView {
public:
    // конструктор
    zViewScrollBar(zView* group, bool _vert);
    // разбудить
    virtual void awaken();
    // вернуть имя типа
    virtual cstr typeName() const override { return "zViewScrollBar"; }
    // вернуть размер
    int getSize() const { return size; }
protected:
    // область обрезки
    virtual rti drawableClip() const override { return z_clipRect(parent->rview, rview); }
    // размер
    int size{6};
    // признак затенения
    bool fade{false};
};

class zViewCaret : public zView {
public:
    // конструктор
    zViewCaret();
    // обновление
    void update(zView* own, int x, int y, int h);
protected:
    // период
    int blink{0};
};

class zFlyng : public zView {
public:
    zFlyng(zView* _group);
    // вернуть имя типа
    virtual cstr typeName() const override { return "zViewFlyng"; }
    void start(float delta);
    void stop();
};
