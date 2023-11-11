//
// Created by User on 18.04.2023.
//

#pragma once

#include "zHandler.h"
#include "zDrawable.h"
#include "zCommonAdapters.h"

class zMeasure {
public:
    zMeasure() { }
    zMeasure(int _val) : value(_val) { }
    zMeasure(const zMeasure& _val) : value(_val) { }
    zMeasure(int _mode, int _size) : value((_mode & MEASURE_MASK_MODE) | (_size & MEASURE_MASK_SIZE)) { }
    int size() const { return (value & MEASURE_MASK_SIZE); }
    int mode() const { return (value & MEASURE_MASK_MODE); }
    void setSize(int _size) { value = (mode() | _size); }
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
    const zLayoutParams& set(int _x, int _y, int _w, int _h) { x = _x; y = _y; w = _w; h = _h; return *this; }
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
    friend class zDrawable;
    friend class zDrawableFake;
    friend class zViewGroup;
    friend class zViewManager;
public:
    union TAG {
        char* str{nullptr};
        u32 u; double f;
    };
    struct STATE {
        STATE() { }
        STATE(u32 _id, u32 _tp) : id(_id), type(_tp) { }
        void reset(u32 _id, u32 _tp) { data.free(); string.empty(); id = _id; type = _tp; }
        // идентификатор представления
        u32 id{0};
        // тип представления
        u32 type{0};
        // связанные данные
        zArray<u32> data{};
        // строка текста
        zString8 string{};
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
    // блокировка/разблокировка
    virtual void disable(bool _disable);
    // запрос на полное обновление позиций представлений
    virtual void requestPosition();
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
    // изменение ориентации
    virtual void changeOrientation(bool _vert) { if(vert != _vert) { vert = _vert; requestLayout(); } }
    // сохранение/загрузка
    virtual void stateView(STATE &state, bool save, int &index) { }
    // клавиатура по умолчанию
    virtual cstr getDefaultKeyboardLayer() const { return nullptr; }
    // событие инициализации
    virtual void onInit(bool theme);
    // отправка сообщения
    virtual void post(int what, u64 millis = 0, int arg1 = 0, int arg2 = 0, cstr s = nullptr);
    // требование фокуса
    virtual void requestFocus();
    // уведомление об изменении параметров адаптера
    virtual void notifyAdapter(zBaseAdapter* _adapter) { }
    // вернуть имя типа
    virtual zString8 typeName() const { return "zView"; }
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
    // обновление видимости
    virtual int updateVisible(bool set);
    // установка паддинга
    void setPadding(crti& r) { pad = r; requestLayout(); }
    // удаление отображателя
    void removeDrawable(int index) { if(drw[index]->isValid()) delete drw[index]; drw[index] = &fake; }
    // установка маргина
    void setMargin(crti& r) { margin = r; requestLayout(); }
    // вернуть паддинг + маргин
    int padMargin(bool _vert) const { return pad.extent(_vert) + margin.extent(_vert); }
    // вернуть признак доступности
    bool isEnabled() const { return !(status & ZS_DISABLED); }
    // вернуть признак видимости
    bool isVisibled() const { return (status & ZS_VISIBLED); }
    // вернуть ориентацию
    bool isVertical() const { return vert; }
    // вернуть признак уничтожения
    bool isDestroy() const { return (status & ZS_DESTROY); }
    // вернуть признак выбора
    bool isChecked() const { return (status & ZS_CHECKED); }
    // вернуть признак FBO
    bool isFBO() const { return drw[DRW_FBO]->isValid(); }
    // вернуть признак возможности обновления
    bool isInvalidate() const { return !isDirty() || !isFBO(); }
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
    int edges(bool _vert, bool end) const { return rview[_vert] + end * rview[_vert + 2]; }
    // вернуть размер
    int sizes(bool _vert) const { return rview[_vert + 2] + margin.extent(_vert); }
    // проверка статуса на флаг
    bool testFlags(int flags) const { return (status & flags) != 0; }
    // установить тэг
    void setTag(const TAG& _tag) { tag = std::move(_tag); }
    // установить отображатель
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
    zView* setOnClick(std::function<void(zView*, int)> _click) { onClick = std::move(_click); return this; }
    // установка касания
    zView* setOnTouch(std::function<int(zView*, zTouch*)> _t) { onTouch = std::move(_t); return this; }
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
    // показать представления, которые не удалились из памяти
    static void showUnusedView();
    // отрисовка отладочной информации
    virtual void drawDebug();
    // альфа
    float alpha{1.0f};
    // ротация/масштаб/смещение
    zVec3 scale{1, 1, 1}, rot{}, trans{};
    // матрица
    zMatrix mtx{};
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
    rti pad{}, ipad{};
    // внешний отступ
    rti margin{};
    // ячейки сетки
    szi cellDebug{};
    // интерполятор
    zInterpolator animator{};
    // буферы отрисовки
    zDrawable* drw[DRW_COUNT]{};
    // параметры
    static zParamDrawable drParams[DR_COUNT];
    // стили
    zStyle* styles{nullptr};
    // объект касания
    static zTouch* touch;
protected:
    // установка гравитации
    void _setGravity(u32 _g, u8* p) { if(_g != (*p & ZS_GRAVITY_MASK)) { *p &= ~ZS_GRAVITY_MASK; *p |= (u8)_g; requestLayout(); } }
    // обновление OES текстуры
    virtual void updateTexture(cszi& size) { }
    // уведомление о событии
    virtual void notifyEvent(HANDLER_MESSAGE* msg);
    // диспетчер событий касания
    virtual i32 touchEventDispatch(AInputEvent *event) { return touchEvent(event); }
    // отображение формы обрезки
    virtual void drawShape();
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
    virtual i32 onTouchEvent();
    // применение гравитации
    virtual pti applyGravity(crti& sizeParent, crti& sizeChild, u32 _gravity);
    // освободить ресурсы
    virtual void cleanup();
    // обрабатывать касания дочерних?
    virtual bool touchChildren() { return true; }
    // продолжать обработку касания?
    virtual bool touchSelf() { return false; }
    // установка
    void setMeasuredDimension(int width, int height);
    bool initTouchHandler(AInputEvent* event) const;
    // установка габаритов по умолчанию
    void defaultOnMeasure(cszm& spec, szi size);
    // ориентация
    bool vert{false};
    // родитель
    zView* parent{nullptr};
    // спецификаторы габаритов
    szm measureSpec{};
    // предыдущая позиция
    rti oldPos{INT_MAX, INT_MAX};
    // событие клика
    std::function<void(zView*, int)> onClick;
    // событие касания
    std::function<int(zView*, zTouch*)> onTouch;
    // событие анимации
    std::function<int(zView*, int)> onAnimation;
    // отрисовщик отладки
    zDrawable drwDebug{this, -1};
    // фейковый отрисовщик
    static zDrawableFake fake;
    // текущая FBO
    static zView* fbo;
    // временная матрица
    static zMatrix m;
};

// глоу эффект
class zViewGlow : public zView {
public:
    // конструктор
    zViewGlow(zView* group);
    // запуск
    void start(float delta, bool vert, bool flow);
    // вернуть имя типа
    virtual zString8 typeName() const override { return "zViewGlow"; }
    // остановка
    void stop() { animator.clear(); updateVisible(false); }
protected:
    // внутренние события
    virtual void notifyEvent(HANDLER_MESSAGE *msg) override;
    // область обрезки
    virtual rti drawableClip() const override { return parent->rclip; }
    bool flow{false};
};

// прокрутка
class zViewScrollBar : public zView {
    friend class zViewGroup;
public:
    // конструктор
    zViewScrollBar(zView* group, bool _vert);
    // разбудить
    virtual void awaken();
    // вернуть имя типа
    virtual zString8 typeName() const override { return "zViewScrollBar"; }
    // вернуть размер
    int getSize() const { return size; }
protected:
    // внутренние события
    virtual void notifyEvent(HANDLER_MESSAGE *msg) override;
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
    virtual rti drawableClip() const override { return z_clipRect(parent->rclip, parent->rview); }
    // обновление
    void update(zView* own, int x, int y, int h);
protected:
    // внутренние события
    virtual void notifyEvent(HANDLER_MESSAGE *msg) override;
};

class zFlyng : public zView {
public:
    zFlyng(zView* _group);
    // вернуть имя типа
    virtual zString8 typeName() const override { return "zViewFlyng"; }
    // запуск
    bool start(zTouch* touch, int delta);
    // завершение
    void stop();
protected:
    // внутренние события
    virtual void notifyEvent(HANDLER_MESSAGE *msg) override;
};

static zStyle styles_z_view1[] = {
        { Z_BACKGROUND, 0xffffffff },
        { Z_PADDING | ZT_END, 0 }
};

class zView1 : public zView {
public:
    zView1() : zView(styles_z_view1, 0) { }
    virtual void onInit(bool _theme) override {
        zView::onInit(_theme);

    }
protected:
    virtual void onMeasure(cszm& spec) override {
        setMeasuredDimension(spec.w.size(), spec.h.size());
    }
    virtual void onLayout(crti& pos, bool changed) override {
        zView::onLayout(pos, changed);
    }
};