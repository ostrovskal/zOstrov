//
// Created by User on 19.04.2023.
//

#pragma once

#define SPAN_DEFAULT        0
#define SPAN_FK_COLOR       1   // задает цвет
#define SPAN_BK_COLOR       2   // задает фоновый цвет
#define SPAN_ABSOLUTE_SIZE  3   // размер
#define SPAN_RELATIVE_SIZE  4   // относительный размер
#define SPAN_STYLE          5   // стиль
#define SPAN_IMAGE          6   // картинка
#define SPAN_PARAGRAPH      7   // параграф
#define SPAN_BULLET         8   // список
#define SPAN_URL            9   // ссылка
#define SPAN_SUBSCRIPT      10  // текст снизу -
#define SPAN_SUPERSCRIPT    11  // текст сверху - степень
#define SPAN_UNDERLINE      12  //
#define SPAN_STRIKELINE     13  //

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
    auto getBold() const { return bold; }
    auto getItalic() const { return italic * factor; }
    auto getSize() const { return size; }
    auto getStyle() const { return style; }
    auto getBaseline() const { return baseLine; }
    auto getMargin() const { return margin; }
    auto getWidthChar(int ch) const { return font->widthGlyph(ch + bold, factor); }
    auto getFactor(float _mul) const { return (int)round(_mul * factor); }
    bool getBoundChar(int ch, rti& tex, rti& bound) const;
    bool isUnderline() const { return style & ZS_TEXT_UNDERLINE; }
    bool isStrike() const { return style & ZS_TEXT_STRIKE; }
    // вернуть ширину текста в пикселях
    int widthText(cstr _text, int length) const;
    // вернуть ближайщую позицию в тексте(screenLimit - предел/screenX - начальная коор. текста)
    int indexOf(cstr _text, int length, int screenLimit, int screenX, bool exact) const;
    int indexReverseOf(cstr _text, int length, int screenLimit) const;
    // цвет текста/фона
    u32 fkColor{0}, bkColor{0};
protected:
    // фактор масштабирования
    float factor{1.0f};
    i32 yBold{0};
    // размер/отступ
    i32 size{20}, margin{0};
    //
    i32 italic{0}, bold{0}, baseLine{0};
    // стиль
    u32 style{ZS_TEXT_NORMAL};
    // шрифт
    zTexture* font{nullptr};
    int idx{0};
};

class zTextSpan {
public:
    zTextSpan() { }
    // обновление состояния
    virtual void updateState(zTextPaint* paint) { }
    // позиционирование
    virtual bool draw(int x, int y, int hmax, zTextPaint *paint) { return false; }
    // клик по спану
    virtual bool click() { return false; }
    // вернуть тип спана
    virtual int typeId() const { return SPAN_DEFAULT; }
};

