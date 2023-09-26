//
// Created by User on 18.04.2023.
//

#pragma once

#include "zTexture.h"

struct zParamDrawable {
    void set(u32 bs, u32 idx, int val);
    static void setDefaults();
    union {
        struct { u32 texture{0}, color{0}, padding{0};
                 i32 tiles{-1}, shader{0}, size{8}, type{0}; float scale{1.0f};
        };
        int params[10];
    };
};

class zView;
class zDrawable {
public:
    // конструктор
    zDrawable() { }
    zDrawable(zView* _view, int _index) : view(_view), index(_index) { }
    // деструктор
    virtual ~zDrawable() { release(); }
    // габариты
    virtual void measure(int width, int height, int pivot, bool isSave);
    // отрисовка
    virtual void draw(rti *rect);
    // инициализация
    virtual void init(const zParamDrawable& p) {
        init(p.texture, p.color, p.tiles, p.padding, p.scale, p.shader);
    }
    // событие анимации
    virtual int onAnimation() { return 0; }
    // отрисовка в FBO
    virtual void drawFBO(zView* prev, const std::function<void(void)>& _draw);
    virtual void drawFBO(zView* view, zView* prev);
    // признак валидности
    virtual bool isValid() const { return true; }
    // непосредственно отрисовка
    void drawCommon(crti& clip, const zMatrix& m, bool fbo) const;
    // вернуть тайл по индексу
    int getTileNum(int idx) const { return (tiles >> (idx * 8)) & 0xff; }
    // установить тайл
    zDrawable* setTileNum(int idx) { tile = getTileNum(idx); return this; }
    // очистка
    void clear() { SAFE_DELETE(vertices); count = 0; }
    // установка fbo
    void setFBO(bool set, bool clear) { texture->setFBO(set, clear); }
    // формирование
    void make(i32 _count);
    // установить параметры
    void init(u32 _tx, u32 _cl, i32 _tl, u32 _pd, float _sc, int sh = 0);
    void init(const zDrawable* drw, int _tiles);
    // сформировать координаты для патч9
    int makePatch9(crti& pos, crti& tex, rti patch9) const;
    // сформировать два треугольника
    int makeTriangle(crti& pos, crti& tex, zVertex2D* v, int _italic = 0) const;
    // сформировать квад
    int makeQuad(crti& pos, crti& tex, zVertex2D* v, int _italic = 0) const;
    // сформировать путь
    int makePath(const zArray<ptf>& pth, crti& tex, ptf b, int pivot);
    // сформировать отладочную сетку
    void makeDebug(cszi& cell);
    // реализация
    void release();
    // сформировать текст
    int makeText(cstr text, int len, zTextPaint* paint);
    ptf offsetBound() const;
    // вычислить размер, с учетом масштабирования
    virtual szi resolveSize(int wmax, int hmax, u32 gravity) const;
    // вычислить фактор масштабирования
    float scaleFactor(u32 value, bool text) const;
    // вернуть рекст тайла
    rti getRectTile(bool tex) const {
        auto r(texture->getTile(tile)->rect);
        if(tex) r.w += r.x, r.h += r.y;
        return r;
    }
    // информация о объекте
    void info() const;
    // фактор масштабирования
    float scale{1.0f};
    // индекс шейдера
    i32 shader{0};
    // тип треугольников
    i32 typeTri{GL_TRIANGLE_STRIP};
    // количество вершин
    i32 count{0};
    // тайл
    i32 tile{0}, tiles{0};
    // индекс
    i32 index{0};
    // рект
    rti bound{};
    // отступ текстуры
    rti padding{};
    // цвет
    zColor color{};
    // владелец
    zView* view{nullptr};
    // массив вершин
    zVertex2D* vertices{nullptr};
    // текстура
    zTexture *texture{nullptr};
    // признак видимости
    bool visible{true};
};

// фейковый отображатель
class zDrawableFake : public zDrawable {
public:
    zDrawableFake() { }
    // габариты
    virtual void measure(int width, int height, int pivot, bool isSave) override { }
    // отрисовка
    virtual void draw(rti *rect) override { }
    // отрисовка в FBO
    virtual void drawFBO(zView* prev, const std::function<void(void)>& _draw) override { _draw(); }
    virtual void drawFBO(zView* view, zView* prev) override;
    // признак валидности
    virtual bool isValid() const override { return false; }
    // габариты тайла
    virtual szi resolveSize(int wmax, int hmax, u32 gravity) const override { return  { 0, 0 }; }
};

// разделитель
class zDrawableDivider : public zDrawable {
public:
    // конструктор
    zDrawableDivider(zView* _view, int _index) : zDrawable(_view, _index) { }
    // размер
    virtual void measure(int width, int height, int pivot, bool isSave) override;
    // инициализация
    virtual void init(const zParamDrawable& p) override;
    // формирование
    void make(int extra, int count, int idx);
    // определение размера всех разделителей
    int resolve(int count, bool _visible) const;
    // вернуть полный размер по типу разделителя
    int getSize(int flags) const;
    // тип отображения/размер
    int type{0}, size{1}, padBegin{0}, padEnd{0};
protected:
    // установка элемента
    void set(int pos, int pad);
};
