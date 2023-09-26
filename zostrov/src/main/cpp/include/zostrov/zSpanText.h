//
// Created by User on 19.04.2023.
//

#pragma once

BETTER_ENUM(spans, int, SPAN_DEFAULT, SPAN_BK_COLOR, SPAN_FK_COLOR, SPAN_ABSOLUTE_SIZE, SPAN_RELATIVE_SIZE,
                        SPAN_SUBSCRIPT, SPAN_SUPERSCRIPT, SPAN_STYLE, SPAN_UNDERLINE, SPAN_STRIKELINE,
                        SPAN_PARAGRAPH, SPAN_GRAVITY, SPAN_IMAGE, SPAN_BULLET, SPAN_MASK, SPAN_URL )

#define SPAN_FLAGS_DEFAULT  0
#define SPAN_FLAGS_SPECIFIC 1
#define SPAN_FLAGS_MARK     2

class zViewText;
class zTextPaint {
public:
    zTextPaint();
    zTextPaint(zViewText* _tv) { reset(_tv); }
    zTextPaint(zTextPaint* _paint) { init(_paint); }
    ~zTextPaint();
    void reset(zViewText* _tv);
    void init(zTextPaint* _paint);
    void setStyle(u32 _style);
    void setSize(i32 _size);
    void setBaseline(i32 _bs) { baseLine = _bs; }
    auto getBold() const { return bold; }
    auto getItalic() const { return z_round(italic * factor); }
    auto getSize() const { return size; }
    auto getStyle() const { return style; }
    auto getBaseline() const { return baseLine; }
    auto getAscent() const { return font->ascent; }
    auto getFactor(float _mul) const { return z_round(_mul * factor); }
    int getWidthChar(int ch) const;
    zTexture::TILE_TTL* getBoundChar(int ch, rti& tex, rti& bound) const;
    bool isUnderline() const { return style & ZS_TEXT_UNDERLINE; }
    bool isStrike() const { return style & ZS_TEXT_STRIKE; }
    zTexture* getFont() const { return font; }
    // корректировать baseline
    int correctBaseline(int size) const;
    int getSizeFont() const { return font->getSize().h / 2; }
    // вернуть ширину текста в пикселях
    int widthText(cstr _text, int length) const;
    // вернуть ближайщую позицию в тексте(screenLimit - предел/screenX - начальная коор. текста)
    int indexOf(cstr _text, int length, int screenLimit, int screenX, bool exact) const;
    int indexReverseOf(cstr _text, int length, int screenLimit) const;
    zString8 info() const;
    // цвет текста/фона
    u32 fkColor{0xffffffff}, bkColor{0};
    // ширина в пикселях/смешение высоты
    int width{0}, subH{0};
protected:
    // фактор масштабирования
    float factor{1.0f};
    i32 yBold{0};
    // размер
    i32 size{20};
    //
    i32 italic{0}, bold{0}, baseLine{0};
    // стиль
    u32 style{ZS_TEXT_NORMAL};
    // шрифт
    zTexture* font{nullptr};
};

class zTextSpan {
public:
    zTextSpan() { }
    virtual ~zTextSpan() { }
    // обновление состояния
    virtual void updateState(zTextPaint* paint) { }
    // вычисление
    virtual void margin(zViewText* vt, zTextPaint* paint, cstr str) { }
    // позиционирование
    virtual void draw(int x, int y, int hmax, zTextPaint *paint, crti& clip) { }
    // установка изображения(для ссылки)
    virtual void setImage(zTextSpan* _span) { }
    // клик по спану
    virtual bool click() { return false; }
    // вернуть тип спана
    virtual int typeId() const { return spans::SPAN_DEFAULT; }
};

// стиль текста
class zTextSpanStyle : public zTextSpan {
public:
    zTextSpanStyle(int _style) : style(_style) { }
    // обновление состояния
    void updateState(zTextPaint *paint) override { paint->setStyle(style | paint->getStyle()); }
    // вернуть тип спана
    virtual int typeId() const override { return spans::SPAN_STYLE; }
    int style{0};
};

// цвет текста
class zTextSpanForegrounColor : public zTextSpan {
public:
    zTextSpanForegrounColor(u32 _color) : color(_color) { }
    // вернуть тип спана
    virtual int typeId() const override { return spans::SPAN_FK_COLOR; }
    // обновление состояния
    void updateState(zTextPaint *paint) override { paint->fkColor = color; }
    u32 color{0};
};

// фоновый цвет текста
class zTextSpanBackgroundColor : public zTextSpan {
public:
    zTextSpanBackgroundColor(u32 _color) : color(_color) { }
    // вернуть тип спана
    virtual int typeId() const override { return spans::SPAN_BK_COLOR; }
    // обновление состояния
    void updateState(zTextPaint *paint) override { paint->bkColor = color; }
    u32 color{0};
};

// абсолютная высота текста
class zTextSpanAbsoluteSize : public zTextSpan {
public:
    zTextSpanAbsoluteSize(u32 _size) : size(_size) { }
    // вернуть тип спана
    virtual int typeId() const override { return spans::SPAN_ABSOLUTE_SIZE; }
    // обновление состояния
    void updateState(zTextPaint *paint) override { paint->setSize(size); }
    i32 size{0};
};

// относительная высота текста
class zTextSpanRelativeSize : public zTextSpan {
public:
    zTextSpanRelativeSize(float _rel) : rel(_rel) { }
    // вернуть тип спана
    virtual int typeId() const override { return spans::SPAN_RELATIVE_SIZE; }
    // обновление состояния
    void updateState(zTextPaint *paint) override { paint->setSize(z_round((float)paint->getSize() * rel)); }
    float rel{0.0f};
};

// изображение
class zTextSpanImage : public zTextSpan {
public:
    zTextSpanImage(cstr _image, cstr _tile, float _factor, u32 _color = 0);
    // обновление состояния
    virtual void updateState(zTextPaint *paint) override;
    // отрисовка
    virtual void draw(int x, int y, int hmax, zTextPaint *paint, crti& clip) override;
    // вернуть тип спана
    virtual int typeId() const override { return spans::SPAN_IMAGE; }
    // изображение для спана
    zDrawable dr{ nullptr, DRW_FK };
    // временная матрица
    static zMatrix m;
};

class zTextSpanMask : public zTextSpanImage {
public:
    zTextSpanMask(cstr image, cstr tile, float factor, int _w) : zTextSpanImage(image, tile, factor, 0x7f7f7f7f), _width(_w) { }
    // отрисовка
    virtual void draw(int x, int y, int hmax, zTextPaint *paint, crti &clip) override;
    // вернуть тип спана
    virtual int typeId() const override { return spans::SPAN_MASK; }
    int _width{0};
};

class zTextSpanBullet : public zTextSpanImage {
public:
    zTextSpanBullet(bool _ordered, int _index);
    // обновление состояния
    virtual void updateState(zTextPaint *paint) override;
    // отрисовка
    virtual void draw(int x, int y, int hmax, zTextPaint *paint, crti& clip) override;
    // вернуть тип спана
    virtual int typeId() const override { return spans::SPAN_BULLET; }
    // признак нумерованного списка
    bool ordered{false};
    // текущий индекс нумерованного списка
    zString8 _idx{};
};

class zTextSpanParagraph : public zTextSpan {
public:
    // отступ
    virtual void updateState(zTextPaint* paint) override;
    // вернуть тип спана
    virtual int typeId() const override { return spans::SPAN_PARAGRAPH; }
};

class zTextSpanGravity : public zTextSpan {
public:
    zTextSpanGravity(int type) : grav(type) { }
    // отступ
    virtual void margin(zViewText* vt, zTextPaint* paint, cstr str) override;
    // вернуть тип спана
    virtual int typeId() const override { return spans::SPAN_GRAVITY; }
    int grav{ZS_GRAVITY_START};
};

class zTextSpanUrl : public zTextSpan {
public:
    zTextSpanUrl(cstr _url) : url(_url) { }
    // обновление состояния
    void updateState(zTextPaint *paint) override {
        paint->fkColor = theme->styles->_int(Z_THEME_COLOR_TEXT_URL, 0xff0000ff);
        paint->setStyle(paint->getStyle() | ZS_TEXT_UNDERLINE);
        bkg = paint->bkColor;
    }
    void setImage(zTextSpan* _span) override {
        if(dynamic_cast<zTextSpanImage*>(_span)) image = _span;
    }
    // отрисовка
    virtual void draw(int x, int y, int hmax, zTextPaint *paint, crti& clip) override {
        if(image) image->draw(x, y, hmax, paint, clip);
    }
    // вернуть тип спана
    virtual int typeId() const override { return spans::SPAN_URL; }
    // адрес ссылки
    zString8 url{};
    // предыдущий фон
    int bkg{0};
    // если ссылка на изображении
    zTextSpan* image{nullptr};
};

class zTextSpanSubscript : public zTextSpan {
public:
    // обновление состояния
    void updateState(zTextPaint *paint) override {
        paint->setSize(z_round((float)paint->getSize() * 0.66f));
        paint->setBaseline(paint->getBaseline() - paint->getSize());
    }
    // вернуть тип спана
    virtual int typeId() const override { return spans::SPAN_SUBSCRIPT; }
};

class zTextSpanSuperscript : public zTextSpan {
public:
    // обновление состояния
    void updateState(zTextPaint *paint) override {
        paint->setSize(z_round((float)paint->getSize() * 0.66f));
        paint->setBaseline(paint->getAscent() + paint->getSize() / 2);
    }
    // вернуть тип спана
    virtual int typeId() const override { return spans::SPAN_SUPERSCRIPT; }
};

class zTextSpanUnderline : public zTextSpan {
public:
    // обновление состояния
    void updateState(zTextPaint *paint) override { paint->setStyle(paint->getStyle() | ZS_TEXT_UNDERLINE); }
    // вернуть тип спана
    virtual int typeId() const override { return spans::SPAN_UNDERLINE; }
};

class zTextSpanStrikeline : public zTextSpan {
public:
    // обновление состояния
    void updateState(zTextPaint *paint) override { paint->setStyle(paint->getStyle() | ZS_TEXT_STRIKE); }
    // вернуть тип спана
    virtual int typeId() const override { return spans::SPAN_STRIKELINE; }
};
