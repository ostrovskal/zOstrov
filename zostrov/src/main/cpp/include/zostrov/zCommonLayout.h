//
// Created by User on 19.04.2023.
//

#pragma once

class zFrameLayout : public zViewGroup {
public:
    zFrameLayout(zStyle* _styles, i32 _id) : zViewGroup(_styles, _id) { }
    virtual zString8 typeName() const override { return "zFrameLayout"; }
protected:
    virtual void onMeasure(cszm& spec) override;
    virtual void onLayout(crti &position, bool changed) override;
};

class zAbsoluteLayout : public zViewGroup {
public:
    zAbsoluteLayout(zStyle* _styles, i32 _id) : zViewGroup(_styles, _id) { }
    virtual zString8 typeName() const override { return "zAbsoluteLayout"; }
protected:
    virtual void onMeasure(cszm& spec) override;
    virtual void onLayout(crti &position, bool changed) override;
};

class zLinearLayout : public zViewGroup {
public:
    zLinearLayout(zStyle* _styles, i32 _id, bool _vert) : zViewGroup(_styles, _id) { vert = _vert; }
    virtual zString8 typeName() const override { return "zLinearLayout"; }
protected:
    virtual void onMeasure(cszm& spec) override;
    virtual void onLayout(crti &position, bool changed) override;
    virtual void onDraw() override;
    virtual pti applyGravity(crti& sizeParent, crti& sizeChild, u32 _gravity) override;
};

class zCellLayout : public zViewGroup {
public:
    zCellLayout(zStyle* _styles, i32 _id, cszi& _cell, i32 _space);
    virtual zString8 typeName() const override { return "zCellLayout"; }
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
    virtual int computeScrollOffset(bool _vert) const override { auto v(atView(0)); return v ? v->scroll[vert] : 0; }
    virtual int computeScrollRange(bool _vert) const override { return childSize; }
    virtual int computeScrollExtent(bool _vert) const override { return sizes(_vert) - pad.extent(_vert); }
    virtual zString8 typeName() const override { return "zScrollLayout"; }
protected:
    virtual pti applyGravity(crti& sizeParent, crti& sizeChild, u32 _gravity) override;
    virtual void onMeasure(cszm& spec) override;
    virtual void onLayout(crti &position, bool changed) override;
    virtual bool touchChildren() override;
    virtual i32 onTouchEvent() override;
    virtual bool touchSelf() override { return true; }
    int childSize{0};
};

class zLayoutEdit;
class zViewText;
class zEditLayout : public zViewGroup {
    friend class zLayoutEdit;
public:
    zEditLayout(zStyle* _styles, i32 _id) : zViewGroup(_styles, _id) { }
    virtual zView *attach(zView *v, int width, int height, int where = -1) override;
    // смена темы
    virtual void changeTheme() override;
    // имя типа
    virtual zString8 typeName() const override { return "zEditLayout"; }
protected:
    // внутренние события
    virtual void notifyEvent(HANDLER_MESSAGE *msg) override;
    // габариты
    virtual void onMeasure(cszm& spec) override;
    // позиционирование
    virtual void onLayout(crti &position, bool changed) override;
    // обновление
    void childUpdateText(bool _empty);
    void layoutChild();
    int frame{0};
    bool textEmpty{false};
    zViewText* hint{nullptr};
    zLayoutEdit* edit{nullptr};
};

class zTabWidget : public zFrameLayout {
public:
    zTabWidget(zStyle* _styles, i32 _id, u32 _text, i32 _icon) : zFrameLayout(_styles, _id), text(_text), icon(_icon) { }
    u32 getText() const { return text; }
    i32 getIcon() const { return icon; }
protected:
    u32 text{0};
    i32 icon{-1};
};

class zTabLayout : public zLinearLayout {
public:
    zTabLayout(zStyle* _styles, i32 _id, zStyle* _styles_capt, int _gravityCaption);
    // добавить страницу
    virtual zView* attach(zView* v, int width, int height, int where = -1) override;
    // сохранение/восстановление
    virtual void stateView(STATE &state, bool save, int &index) override;
    // установить активную страницу
    virtual void setActivePage(int _page);
    // загрузка стилей
    virtual void onInit(bool theme) override;
    // вернуть имя типа
    virtual zString8 typeName() const override { return "zTabLayout"; }
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

class zViewChart : public zFrameLayout {
public:
    // конструктор
    zViewChart(zStyle* _styles, i32 _id, u32 chartGravity);
    virtual ~zViewChart();
    // загрузка стилей
    virtual void onInit(bool _theme) override;
    // признак сохранения состояния дочерних
    virtual bool stateChildren() const override { return false; }
    // вернуть имя типа
    virtual zString8 typeName() const override { return "zViewChart"; }
    // установка режима
    void setMode(u32 _mode) { if(mode != _mode) { mode = _mode; requestLayout(); } }
    // установка данных
    bool addData(i32* value, int countValues, u32* colors, int countColors);
    void delData(int idx);
    // установка первого видимого
    void setFirstVisible(int _first);
    // вернуть первый видимый
    int getFirstVisible() const { return fChart; }
    // вернуть количество видимых
    int getCountVisible() const { return maxCharts; }
    // вернуть общее количество
    int getCount() const { return cCharts; }
    // вернуть вычисленный размер диаграммы
    float getSizeDiagramm() const { return sizeChart; }
    // вернуть значение по индексу из определенного набора
    int getValue(int idx, int kit) const;
protected:
    struct CHART_DATA {
        CHART_DATA() { }
        const CHART_DATA& set(i32* _data, i32 _count) { data = _data; count = _count; return *this; }
        ~CHART_DATA() { SAFE_DELETE(data); }
        i32* data{nullptr}; i32 count{0};
    };
    // событие габаритов
    virtual void onMeasure(cszm& spec) override;
    // событие позиционирования
    virtual void onLayout(crti &position, bool changed) override;
    // параметры прокруток
    virtual int computeScrollOffset(bool _vert) const override { return fChart * (int)sizeChart + delta; }
    virtual int computeScrollRange(bool _vert) const override { return cCharts * (int)sizeChart; }
    virtual int computeScrollExtent(bool _vert) const override { return rclient[_vert + 2]; }
    // событие касания
    i32 onTouchEvent() override;
    // прокрутка
    virtual bool scrolling(int _delta) override;
    // отрисовка
    virtual void onDraw() override;
    void makeDiagramm(float x, float y, float dy);
    void makeCircular(float x, float y, float dy);
    void makeGraph(float x, float y, float dy);
    int itemFromPoint(cptf& pt) const;
    // макс значение
    int maxVal{0};
    // первый видимый/всего/всего видимых
    int fChart{0}, cCharts{0}, maxCharts{0};
    // размер одной диаграммы
    float sizeChart{1};
    // центр
    ptf center{};
    // начальный угол
    float startAngle{0.0f};
    // входные данные - цвета/значения
    zArray<CHART_DATA> colors{};
    zArray<CHART_DATA> values{};
    // режим отображения
    u32 mode{ZS_CHART_DIAGRAMM};
    // массив ректов диаграмм
    zArray<rti> rects{};
    zArray<u32> cols{};
    // гравитация диаграмм
    u32 grav{ZS_GRAVITY_TOP};
    // дельта
    int delta{0}, clickItem{-1}, selectItem{-1};
    // отрисовщики
    zDrawable* dr[3]{nullptr};
};
