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
    zTextPaint() { }
    zTextPaint(zViewText* _tv) { reset(_tv); }
    zTextPaint(zTextPaint* _paint) { init(_paint); }
    void reset(zViewText* _tv);
    void init(zTextPaint* _paint);
    void setStyle(u32 _style);
    u32 getStyle() const { return style; }
    // цвет текста/фона
    u32 fkColor{0}, bkColor{0};
    // размер
    i32 size{0}, preWidth{0};
    i32 italic{0}, baseLine{0};
    zTexture* texture{nullptr};
protected:
    u32 style{ZS_TEXT_NORMAL};
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

