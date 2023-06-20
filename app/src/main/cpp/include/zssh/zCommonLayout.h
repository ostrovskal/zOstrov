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
    virtual int computeScrollOffset(bool _vert) const override { auto v(atView(0)); return v ? v->scroll[vert] : 0; }
    virtual int computeScrollRange(bool _vert) const override { return childSize; }
    virtual int computeScrollExtent(bool _vert) const override { return sizes(_vert) - pad.extent(_vert); }
    virtual cstr typeName() const override { return "zScrollLayout"; }
protected:
    virtual void onMeasure(cszm& spec) override;
    virtual void onLayout(crti &position, bool changed) override;
    virtual i32 onTouchEvent(zTouch *touch) override;
    int childSize{0};
};

class zLayoutEdit;
class zEditLayout : public zViewGroup {
    friend class zLayoutEdit;
public:
    zEditLayout(zStyle* _styles, i32 _id) : zViewGroup(_styles, _id) { }
    virtual zView *attach(zView *v, int width, int height, int where = -1) override;
    // смена темы
    virtual void changeTheme() override;
    // имя типа
    virtual cstr typeName() const override { return "zEditLayout"; }
protected:
    virtual void onMeasure(cszm& spec) override;
    virtual void onLayout(crti &position, bool changed) override;
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
