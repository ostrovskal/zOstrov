
#pragma once

#include "zViewGroup.h"

class zViewBaseRibbon : public zViewGroup {
public:
    // конструктор
    zViewBaseRibbon(zStyle* _styles, i32 _id, bool _vert);
    // деструктор
    virtual ~zViewBaseRibbon();
    // уведомление адаптера
    virtual void notifyAdapter(zBaseAdapter* _adapter) override;
    // сохранение/восстановление
    virtual void stateView(STATE &state, bool save, int &index) override;
    // признак сохранения состояния дочерних
    virtual bool stateChildren() const override { return false; }
    // смена темы
    virtual void changeTheme() override;
    // прокрутить список
    virtual bool scrolling(int _delta) override;
    // сброс
    virtual void reset();
    // инициализация
    virtual void onInit(bool _theme) override;
    // параметры прокрутки
    virtual int computeScrollOffset(bool _vert) const override;
    virtual int computeScrollRange(bool _vert) const override;
    virtual int computeScrollExtent(bool _vert) const override;
    // отключить обновление размера
    virtual void requestLayout() override;
    // вернуть имя типа
    virtual zString8 typeName() const override { return "zViewBaseRibbon"; }
    // установка адаптера
    zViewBaseRibbon* setAdapter(zBaseAdapter* _adapter);
    // вернуть адаптер
    zAdapterList* getAdapter() const { return dynamic_cast<zAdapterList*>(adapter); }
    // установка выделенного элемента
    virtual void setItemSelected(int item);
    // вернуть выделенный элемент
    int getItemSelected() const { return selectItem; }
    // вернуть первый видимый
    int getFirstItem() const { return firstItem; }
protected:
    // внутренние события
    virtual void notifyEvent(HANDLER_MESSAGE *msg) override;
    // позиционирование
    virtual void onLayout(crti &position, bool changed) override;
    // размер
    virtual void onMeasure(cszm& spec) override;
    // отрисовка
    virtual void onDraw() override;
    // Заполнение списка
    virtual void fill(int edgePos, bool reverse) = 0;
    virtual void fillReverse(int pos, int start) = 0;
    virtual void fillForward(int pos, int end) = 0;
    // касание
    virtual i32 onTouchEvent() override;
    // продолжать обработку касания?
    virtual bool touchChildren() override;
    virtual bool touchSelf() override { return true; }
    // вычисление размеров дочернего
    virtual void childMeasure(zView* child, zLayoutParams* lps) = 0;
    // рассчитать габариты всех дочерних
    virtual szi measureChildrenSize(cszm& spec);
    // вернуть extra
    virtual int getExtra() const { return 0; }
    // Ликвидация расстояния между началом списка и первым элементом
    void correctBegin(int used);
    // Ликвидация расстояния между концом списка и последним элементом
    void correctFinish(int used);
    // показать/спрятать выделение
    void showSelector(bool show);
    // вернуть индекс представления по координатам
    int itemFromPoint(cptf& p) const;
    // добавить в кэш
    void addViewCache(zView* view);
    // получить представление
    zView* obtainView(int position, bool* needToMeasure = nullptr);
    // добавить представление из адаптера
    zView* addView(int x, int y, int pos, bool flow, int where);
    // получить представление из кэша
    zView* getViewCache(int pos);
    // предыдущее значение анимации
    float pprev{0.0f};
    // количество линий(для сетки)
    i32 linesGrid{1};
    // количество элементов в адаптере
    i32 countItem{0};
    // выделенный элемент
    i32 selectItem{-1};
    // элемент на котором кликнули
    i32 clickItem{-1};
    // дельта элемента
    i32 deltaItem{0};
    // индекс первого видимого элемента
    i32 firstItem{0};
    // начальная/конечная грань
    szi edge{};
    // адаптер
    zBaseAdapter* adapter{nullptr};
    // кэш представлений
    zArray<zView*> cacheViews{};
};

class zViewRibbon : public zViewBaseRibbon {
public:
    zViewRibbon(zStyle* _styles, i32 _id, bool _vert) : zViewBaseRibbon(_styles, _id, _vert) { }
    // вернуть имя типа
    virtual zString8 typeName() const override { return "zViewRibbon"; }
protected:
    // Заполнение списка
    virtual void fill(int edgePos, bool reverse) override;
    virtual void fillReverse(int pos, int start) override;
    virtual void fillForward(int pos, int end) override;
    // создание дочернего
    virtual void childMeasure(zView *child, zLayoutParams *lps) override;
    // добавление представления
    zView* addView(int c, int pos, bool flow, bool end);
};

class zViewGrid : public zViewBaseRibbon {
public:
    enum { GRID_CELLS_SPACE, GRID_LINES_SPACE, GRID_CELL_SIZE, GRID_MODE, GRID_LINES };
    // конструктор
    zViewGrid(zStyle* _styles, i32 _id, bool _vert) : zViewBaseRibbon(_styles, _id, _vert) { }
    // загрузка стилей
    virtual void onInit(bool _theme) override;
    // вернуть имя типа
    virtual zString8 typeName() const override { return "zViewGrid"; }
    // установка параметра
    void setParameters(i32 param, i32 value);
    // возврат параметра
    i32 getParameters(i32 param) const;
protected:
    virtual int getExtra() const override;
    virtual void childMeasure(zView *child, zLayoutParams *lps) override;
    // вычисление размера
    virtual void onMeasure(cszm& spec) override;
    // Заполнение таблицы
    virtual void fill(int edgePos, bool reverse) override;
    virtual void fillReverse(int pos, int start) override;
    virtual void fillForward(int pos, int end) override;
    // подсчитать габариты дочерних
    virtual szi measureChildrenSize(cszm& spec) override;
    // Создание линии
    zView* makeLine(int position, int coord, bool flow);
    // установленные параметры
    i32 _params[GRID_LINES + 1]{};
    // текущие параметры
    i32 params[GRID_LINES + 1]{};
};

class zViewPopup : public zViewGroup {
    friend class zViewManager;
public:
    // отобразить
    void show(int x, int y, zView* _owner, zView* _content, zStyle* _styles = nullptr);
    // признак сохранения состояния дочерних
    virtual bool stateChildren() const override { return false; }
    // обработка касания
    virtual i32 touchEventDispatch(AInputEvent *event) override;
    // отставка
    void dismiss();
    // нажатие клавиши
    virtual i32 keyEvent(int key, bool sysKey) override;
    // вернуть имя типа
    virtual zString8 typeName() const override { return "zViewPopup"; }
protected:
    // конструктор
    zViewPopup();
    // позиционирование
    virtual void onLayout(crti &position, bool changed) override;
    // размер
    virtual void onMeasure(cszm& spec) override;
    // владелец
    zView* owner{nullptr};
    // содержимое
    zView* content{nullptr};
    // смещение от владельца
    pti offs{};
};

class zViewDropdown : public zViewRibbon {
    friend class zViewManager;
public:
    // инициализация
    virtual void onInit(bool _theme) override { minMaxSize.empty(); zViewRibbon::onInit(_theme); }
    // вернуть имя типа
    virtual zString8 typeName() const override { return "zViewDropdown"; }
protected:
    // конструктор
    zViewDropdown() : zViewRibbon(styles_default, 0, true) {  }
    // уведомление о событии
    virtual void notifyEvent(HANDLER_MESSAGE* msg) override;
    // рассчитать габариты всех дочерних
    virtual szi measureChildrenSize(cszm& spec) override;
    // вызывается при отсоединении от родителя
    virtual void onDetach() override;
    // владелец
    zView* owner{nullptr};
};

class zViewSelect : public zViewGroup {
public:
    // конструктор
    zViewSelect(zStyle* _styles, i32 id);
    // деструктор
    virtual ~zViewSelect();
    // сохранение/восстановление состояния
    virtual void stateView(STATE &state, bool save, int &index) override;
    // признак сохранения состояния дочерних
    virtual bool stateChildren() const override { return false; }
    // смена темы
    virtual void changeTheme() override;
    // инициализация
    virtual void onInit(bool _theme) override;
    // проверка на режим блокировки
    virtual bool testLocked() const override;
    // вернуть имя типа
    virtual zString8 typeName() const override { return "zViewSelect"; }
    // уведомление адаптера
    virtual void notifyAdapter(zBaseAdapter* _adapter) override { reset(); }
    // установка адаптера
    zViewSelect* setAdapter(zAdapterList* _adapter);
    // вернуть адаптер
    zAdapterList* getAdapter() const { return adapter; }
    // установка выбранного элемента
    void setItemSelected(int item);
    // выбранный
    int getItemSelected() const { return selectItem; }
    // вернуть строку
    zString8 getStringItemSelected(int item) const { return adapter->getItem(item); }
protected:
    class zAdapterSelect : public zAdapterList {
    public:
        zAdapterSelect(zViewSelect* _select, zAdapterList* _list) : zAdapterList(), list(_list), select(_select) {}
        virtual void registration(zView* owner) override { list->registration(owner); }
        virtual bool unregistration(zView* owner) override { return list->unregistration(owner); }
        virtual void insert(int position, czs& t) override { list->insert(position, t); }
        virtual void add(czs& t) override { list->add(t); }
        virtual void addAll(const zArray<zString8>& objs) override { list->addAll(objs); }
        virtual void erase(int position, bool _delete) override { list->erase(position, _delete); }
        virtual void clear(bool _delete) override { list->clear(_delete); }
        virtual int getCount() const override { return list->getCount(); }
        virtual czs& getItem(int position) const override { return list->getItem(position); }
        virtual zView* getView(int position, zView *convert, zViewGroup *parent) override {
            return list->getDropDownView(position, convert, select);
        }
        zAdapterList* list{nullptr};
        zViewSelect* select{nullptr};
    };
    // позиционирование
    virtual void onLayout(crti &position, bool changed) override;
    // вычисление размера
    virtual void onMeasure(cszm& spec) override;
    // уведомление о событии
    virtual void notifyEvent(HANDLER_MESSAGE* msg) override;
    // проверка на качание на себе
    virtual bool touchSelf() override { return true; }
    // сброс
    void reset();
    // адаптер
    zAdapterSelect* adapter{nullptr};
    // габариты dropdown
    szm dropMeasure{};
    // выбранный элемент
    int selectItem{0};
    // событие выделения
    std::function<void(zView*, int)> onChangeSelected;
};
