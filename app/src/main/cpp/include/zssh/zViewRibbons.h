
#pragma once

#include "zViewGroup.h"

class zViewBaseRibbon : public zViewGroup {
public:
    // конструктор
    zViewBaseRibbon(zStyle* _styles, i32 _id, bool _vert) : zViewGroup(_styles, _id) { vert = _vert; flyng = new zFlyng(this); }
    // деструктор
    virtual ~zViewBaseRibbon();
    // уведомление адаптера
    virtual void notifyAdapter(zBaseAdapter* _adapter) override;
    // сохранение/восстановление
    virtual void stateView(STATE &state, bool save, int &index) override;
    // прокрутить список
    virtual bool scrolling(int _delta) override;
    // сброс
    virtual void reset();
    // параметры прокрутки
    virtual int computeScrollOffset(bool _vert) const override;
    virtual int computeScrollRange(bool _vert) const override;
    virtual int computeScrollExtent(bool _vert) const override;
    // вернуть имя типа
    virtual cstr typeName() const override { return "zViewBaseRibbon"; }
    // установка ориентации
    virtual void setOrientation(bool vert) override;
    // установка адаптера
    virtual void setAdapter(zBaseAdapter* _adapter);
    // установка выделенного элемента
    void setItemSelected(int item);
    // вернуть выделенный элемент
    int getItemSelecter() const { return selectItem; }
    // установка события выделения
    void setOnChangeSelected(std::function<void(zView*, int)> _selected) { onChangeSelected = _selected; }
protected:
    // позиционирование
    virtual void onLayout(crti &position, bool changed) override;
    // размер
    virtual void onMeasure(cszm& spec) override;
    // отрисовка
    virtual void onDraw() override;
    // Заполнение списка
    virtual void fill(int edgePos) = 0;
    virtual void fillReverse(int pos, int start) = 0;
    virtual void fillForward(int pos, int end) = 0;
    // касание
    virtual i32 onTouchEvent(zTouch *touch) override;
    // вычисление размеров дочернего
    virtual void childMeasure(zView* child, zLayoutParams* lps) = 0;
    // Ликвидация расстояния между началом списка и первым элементом
    void correctBegin(int used);
    // Ликвидация расстояния между концом списка и последним элементом
    void correctFinish(int used);
    // показать/спрятать выделение
    void showSelector(bool show);
    // рассчитать габариты всех дочерних
    virtual szi measureChildrenSize(cszm& spec);
    // получить представление
    zView* obtainView(int position, bool* needToMeasure = nullptr);
    // вернуть индекс представления по координатам
    int itemFromPoint(cptf& p) const;
    // добавить представление из адаптера
    zView* addView(int x, int y, int pos, bool flow, int where);
    // получить представление из кэша
    zView* getViewCache(int pos);
    // добавить в кэш
    void addViewCache(zView* view);
    // количество линий(для сетки)
    int linesGrid{1};
    // количество элементов в адаптере
    int countItem{0};
    // выделенный элемент
    int selectItem{-1};
    // элемент на котором кликнули
    int clickItem{-1};
    // дельта элемента
    int deltaItem{0};
    // начальная/конечная грань
    szi edge{};
    // признак сдвига на последний элемент
    bool isOffsetEnd{false};
    // индекс первого видимого элемента
    i32 firstItem{0};
    // адаптер
    zBaseAdapter* adapter{nullptr};
    // кэш представлений
    zArray<zView*> cacheViews{};
    // событие выделения
    std::function<void(zView*, int)> onChangeSelected;
};

class zViewRibbon : public zViewBaseRibbon {
public:
    zViewRibbon(zStyle* _styles, i32 _id, bool _vert) : zViewBaseRibbon(_styles, _id, _vert) { }
    // вернуть имя типа
    virtual cstr typeName() const override { return "zViewRibbon"; }
protected:
    // Заполнение списка
    virtual void fill(int edgePos) override;
    virtual void fillReverse(int pos, int start) override;
    virtual void fillForward(int pos, int end) override;
    // создание дочернего
    virtual void childMeasure(zView *child, zLayoutParams *lps) override;
    // добавление представления
    zView* addView(int c, int pos, bool flow, bool end);
};

class zViewGrid : public zViewBaseRibbon {
public:
    enum { TABLE_LINES, TABLE_CELLS_SPACE, TABLE_LINES_SPACE, TABLE_CELL_SIZE, TABLE_MODE };
    // конструктор
    zViewGrid(zStyle* _styles, i32 _id, bool _vert) : zViewBaseRibbon(_styles, _id, _vert) { }
    // загрузка стилей
    virtual void onInit(bool _theme) override;
    // вернуть имя типа
    virtual cstr typeName() const override { return "zViewTable"; }
    // смена ориентации
    virtual void setOrientation(bool _vert) override;
    // установка параметров
    //void setParameters(const i32* params);
    // возврат параметров
    //void getParameters(i32* params) const;
protected:
    virtual void childMeasure(zView *child, zLayoutParams *lps) override;
    // вычисление размера
    virtual void onMeasure(cszm& spec) override;
    // Заполнение таблицы
    virtual void fill(int edgePos) override;
    virtual void fillReverse(int pos, int start) override;
    virtual void fillForward(int pos, int end) override;
    // подсчитать габариты дочерних
    virtual szi measureChildrenSize(cszm& spec) override;
    // Создание линии
    zView* makeLine(int position, int coord, bool flow);
    // установленные параметры
    i32 _params[5]{};
    // Простанство между линиями/Пространство между ячейками/Размер ячейки
    i32 params[3]{};
};

class zViewPopup : public zViewGroup {
public:
    // конструктор
	zViewPopup(zStyle* _styles, zView* _owner, zView* _content);
    // отобразить
    virtual void show(cpti& _offs);
    // обработка касания
    virtual i32 touchEvent(AInputEvent *event) override;
    // отставка
    virtual void dismiss();
    // нажатие клавиши
    virtual i32 keyEvent(int key, bool sysKey) override;
    // вернуть имя типа
    virtual cstr typeName() const override { return "zViewPopup"; }
    // установка контента
    void setContent(zView* content);
protected:
    virtual void onLayout(crti &position, bool changed) override;
    virtual void onMeasure(cszm& spec) override;
    // владелец
    zView* owner{ nullptr };
    // содержимое
    zView* content{ nullptr };
    // смещение от владельца
    pti offs{};
};

class zViewDropdown : public zViewRibbon {
public:
    zViewDropdown(zView* _owner, zStyle* _styles) : zViewRibbon(_styles, -1000, true), owner(_owner) { }
protected:
/*
    virtual void messageReceive(HANDLER_MESSAGE *msg) override {
        owner->notifyOwner(msg->what, atView(msg->arg), msg->arg);
        zViewRibbon::messageReceive(msg);
    }
*/
    virtual szi measureChildrenSize(cszm& spec) override;
    // владелец
    zView* owner{ nullptr };
};

class zViewSelect : public zViewGroup {
public:
    /*
    // конструктор
    zViewSelect(zStyle* _styles, i32 id);
    // деструктор
    virtual ~zViewSelect();
    // сохранение/восстановление состояния
    virtual void stateView(STATE &state, bool save, int &index) override;
    // смена темы
    virtual void changeTheme() override;
    // проверка на режим блокировки
    virtual bool testLocked() const override { return popup->isVisibled(); }
    // уведомление владельца
    virtual void notifyOwner(int what, zView *view, int arg) override;
    // вернуть имя типа
    virtual cstr typeName() const override { return "zViewSelect"; }
    // установка адаптера
    void setAdapter(zAdapterList* _adapter);
    // установка выбранного элемента
    void setItemSelected(int item);
    // выбранный
    int getSelectedItem() const { return selectItem; }
    // установка события выделения
    void setOnChangeSelected(std::function<void(zView*, int)> _select) { onChangeSelected = _select; }
protected:
    class zAdapterSelect : public zAdapterList {
    public:
        zAdapterSelect(zViewSelect* _select, zAdapterList* _list) : zAdapterList(), list(_list), select(_select) {}
        virtual void registration(zView* owner) override { return list->registration(owner); }
        virtual void unregistration(zView* owner) override { return list->unregistration(owner); }
        virtual void insert(int position, const zStringUTF8& t) override { list->insert(position, t); }
        virtual void erase(int position, bool _delete) override { list->erase(position, _delete); }
        virtual void clear(bool _delete) override { list->clear(_delete); }
        virtual int getCount() const override { return list->getCount(); }
        virtual const zStringUTF8& getItem(int position) const override { return list->getItem(position); }
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
    // касание
    virtual i32 onTouchEvent(zTouch *touch) override;
    // адаптер
    zAdapterSelect* adapter{nullptr};
    // выпадающий список
    zViewDropdown* dropdown{nullptr};
    // всплывающее окно
    zViewPopup* popup{nullptr};
    // выбранный элемент
    int selectItem{0};
    // событие выделения
    std::function<void(zView*, int)> onChangeSelected;
     */
};
