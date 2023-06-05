//
// Created by User on 19.04.2023.
//

#pragma once

class zFrameLayout : public zViewGroup {
public:
    zFrameLayout(zStyle* _styles, i32 _id) : zViewGroup(_styles, _id) { }
    virtual cstr typeName() const override { return "zFrameLayout"; }
protected:
    virtual void onMeasure(cszm& spec) override;
    virtual void onLayout(crti &position, bool changed) override;
};

class zAbsoluteLayout : public zViewGroup {
public:
    zAbsoluteLayout(zStyle* _styles, i32 _id) : zViewGroup(_styles, _id) { }
    virtual cstr typeName() const override { return "zAbsoluteLayout"; }
protected:
    virtual void onMeasure(cszm& spec) override;
    virtual void onLayout(crti &position, bool changed) override;
};

class zLinearLayout : public zViewGroup {
public:
    zLinearLayout(zStyle* _styles, i32 _id, bool _vert) : zViewGroup(_styles, _id) { vert = _vert; }
    virtual cstr typeName() const override { return "zLinearLayout"; }
protected:
    virtual void onMeasure(cszm& spec) override;
    virtual void onLayout(crti &position, bool changed) override;
    virtual void onDraw() override;
    virtual pti applyGravity(crti& sizeParent, crti& sizeChild, u32 _gravity) override;
};

class zCellLayout : public zViewGroup {
public:
    zCellLayout(zStyle* _styles, i32 _id, cszi& _cell, i32 _space);
    virtual cstr typeName() const override { return "zCellLayout"; }
protected:
    virtual void onMeasure(cszm& spec) override;
    virtual void onLayout(crti &position, bool changed) override;
    // расстояние внутри ячеек
    i32 space{0};
    // размер ячейки
    szf cell{1.0f, 1.0f};
    // количество ячеек
    szi cells{1, 1};
};

class zScrollLayout : public zViewGroup {
public:
    zScrollLayout(zStyle* _styles, i32 _id, bool _vert) : zViewGroup(_styles, _id) { flyng = new zFlyng(this); vert = _vert; }
    // прив¤зка элемента
    virtual zView *attach(zView *v, int width, int height, int where = -1) override;
    // прокрутка на дельту
    virtual bool scrolling(int _delta) override;
    // параметры прокруток
    virtual int computeScrollOffset(bool _vert) const override { return delta; }
    virtual int computeScrollRange(bool _vert) const override { return childSize; }
    virtual int computeScrollExtent(bool _vert) const override { return sizes(_vert) - pad.extent(_vert); }
    virtual cstr typeName() const override { return "zScrollLayout"; }
protected:
    virtual void onMeasure(cszm& spec) override;
    virtual void onLayout(crti &position, bool changed) override;
    virtual i32 onTouchEvent(zTouch *touch) override;
    int childSize{0}, delta{0};
};

class zEditLayout : public zViewGroup {
    friend class zLayoutEdit;
public:
    zEditLayout(zStyle* _styles, i32 _id) : zViewGroup(_styles, _id) { }
    virtual zView *attach(zView *v, int width, int height, int where = -1) override;
    virtual cstr typeName() const override { return "zEditLayout"; }
protected:
    virtual void onMeasure(cszm& spec) override;
    virtual void onLayout(crti &position, bool changed) override;
    void childUpdateText(bool _empty);
    void layoutChild();
    int frame{0};
    bool textEmpty{false};
};

class zTabLayout : public zLinearLayout {
public:
    zTabLayout(zStyle* _styles, i32 _id, zStyle* _styles_capt, int _gravityCaption);
    // добавить страницу
    virtual void addPage(zViewGroup* page, u32 tabText, i32 tabIcon = -1);
    // сохранение/восстановление
    virtual void stateView(STATE &state, bool save, int &index) override;
    // установить активную страницу
    virtual void setActivePage(int _page);
    // загрузка стилей
    virtual void onInit(bool theme) override;
    // вернуть имя типа
    virtual cstr typeName() const override { return "zTabLayout"; }
    // вернуть активную страницу
    int getActivePage() const { return activePage; }
    // вернуть содержимое активной страницы
    const zViewGroup* getContentPage(int page) const;
    // установка события смены вкладки
    void setOnTabChange(std::function<void(zView*, int)> _change) { onTabChange = _change; }
protected:
    // показать страницу
    virtual void showPage(int _page);
    // позиционирование
    virtual void onLayout(crti &position, bool changed) override;
    // показать селектор
    void showSelector();
    // макет для заголовков
    zLinearLayout* caption{nullptr};
    // макет содержимого
    zFrameLayout* content{nullptr};
    // активная вкладка
    int activePage{-1};
    // гравитация заголовка
    u32 gravCapt{0};
    // стили заголовков
    zStyle* styles_caption{nullptr};
    // событие смены вкладки
    std::function<void(zView*, int)> onTabChange{nullptr};
};

class zActionBar : public zLinearLayout {
public:
    enum { ACTION_DELAY = 300 };
    // конструктор
    zActionBar(zStyle* _styles, zStyle* _styles_buttons, zStyle* _styles_popup);
    // деструктор
    virtual ~zActionBar();
    // отобразить/скрыть
    virtual void show(bool _show);
    // проверка на режим блокировки
    virtual bool testLocked() const override;
    // установка содержимого
    virtual void setContent(zView* _view);
    // вернуть имя типа
    virtual cstr typeName() const override { return "zActionBar"; }
    // формирователь
    bool setMenu(int iconApp, zMenuItem* _menu);
    // установка адаптера
    zActionBar* setAdapter(zAdapterList* _adapter);
protected:
    class zAdapterMenu : public zAdapterArray<MENUITEM> {
    public:
        zAdapterMenu(zActionBar* _bar, zAdapterList* _list) : zAdapterArray<MENUITEM>(), list(_list), bar(_bar) {}
        void setPopup(POPUPMENU* _popup) { popupmenu = _popup; notify(); }
        virtual int getCount() const override { return popupmenu ? popupmenu->size() : 0; }
        virtual const MENUITEM& getItem(int position) const override { return *popupmenu->getItem(position); }
        virtual zView* getView(int position, zView *convert, zViewGroup *parent) override;
    protected:
        zActionBar* bar{nullptr};
        zAdapterList* list{nullptr};
        POPUPMENU* popupmenu{nullptr};
    };
    // вычисление габаритов
    virtual void onMeasure(cszm& spec) override;
    // вычисление кнопок действий
    int measureButton(int _id, int _image, int widthSize, int heightSize, POPUPMENU* _pop, bool isMeasure);
    // формирование меню
    void recursiveMenu(POPUPMENU* pop);
    // сброс кнопки overflow
    void resetOverflow();
    // клик
    void clickPopup(zView* view, POPUPMENU* pop);
    // корень меню
    POPUPMENU* root{nullptr};
    // кнопка вызова страндартного меню
    POPUPMENU overflow{};
    // адаптер
    zAdapterMenu* adapter{nullptr};
    // стили кнопок
    zStyle* styles_buttons{nullptr};
    // содержимое
    zFrameLayout* content{nullptr};
    // окно
    zViewPopup* popup{nullptr};
    // список
    zViewDropdown* dropdown{nullptr};
    // ожидание анимации
    int delay{ACTION_DELAY};
    // объект для рекурсии
    zMenuItem* _menu{nullptr};
    // нажатая кнопка
    zView* buttonView{nullptr};
};
