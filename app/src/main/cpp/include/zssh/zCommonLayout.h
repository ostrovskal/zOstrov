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
    zLinearLayout(zStyle* _styles, i32 _id, bool vert);
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
    zScrollLayout(zStyle* _styles, i32 _id, bool _vert);
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
    void updateChildPosition();
    int frame{0};
    bool textEmpty{false};
};