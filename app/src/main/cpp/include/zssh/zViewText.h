//
// Created by User on 27.04.2023.
//

#pragma once

#include "zstandard/zHtml.h"
#include "zSpanText.h"

class zViewText : public zView {
public:
    enum { TEXT_COLOR_DEFAULT, TEXT_COLOR_HIGHLIGHT, TEXT_COLOR_SHADOW };
    // структура кэшированного текста, разбитого на подстроки
    struct CACHE {
        CACHE() { }
        CACHE(u32 _width, u32 _size, cstr _text, u32 _count) : width(_width), size(_size) { text = zStringUTF8(_text, _count); }
        // текст
        zStringUTF8 text{};
        // ширина в пикселях
        i32 width{0};
        // высота в пикселях
        i32 size{0};
    };
    // структура спана
    struct SPAN {
        // конструкторы
        SPAN() { }
        SPAN(zTextSpan* _span, int _s, int _e, zTextPaint* _paint, int _f = 0) : s(_s), e(_e), f(_f), span(_span), paint(_paint) { }
        ~SPAN() { delete paint; }
        // инфо
        zStringUTF8 info() const {
            static cstr names[] = { "def", "bk", "fk", "abs", "rel", "sub", "sup", "sts", "und", "str", "par", "url", "img", "lst", "msk" };
            return z_fmt("%i-%i %s(%s)", s, e, names[span->typeId()], paint->info().str()).str(); }
        // сброс
        void reset(zViewText* vt) { paint->reset(vt); }
        bool operator == (cszi& _s) const { return s == _s.w && e == _s.h; }
        // оператор сравнения
        bool operator == (zTextSpan* _span) const { return span == _span; }
        // начальная/конечная позиция/флаг
        int s{0}, e{0}, f{0};
        // реальный спан
        zTextSpan* span{nullptr};
        // объект хранящий параметры рисования
        zTextPaint* paint{nullptr};
    };
    // конструктор
    zViewText(zStyle *_styles, i32 _id, u32 _text);
    virtual ~zViewText();
    // событие загрузки стилей
    virtual void onInit(bool theme) override;
    // вернуть имя типа
    virtual cstr typeName() const override { return "zViewText"; }
    // установить текст по идентификатору из ресурсов
    void setText(u32 _text) { setText(theme->findString(_text), false); }
    // установить текст из строки
    void setText(czs& _text, bool force, bool eraseSpane = true);
    // специальный - для клавиатурных кнопок
    void setTextSpecial(czs& _text, cszm& spec);
    // установка текста парсингом из html
    bool setHtmlText(czs& html, const std::function<bool(cstr tag, bool end, zHtml* html)>& parser);
    // установка макс. линий
    void setLines(int _lines) { if(getLines() != _lines) { maxLines = _lines; requestLayout(); } }
    // вернуть макс. линий
    int getLines() const { return maxLines; }
    // установка размера шрифта
    void setTextSize(int _size) { if(defPaint->getSize() != _size) { defPaint->setSize(_size); requestLayout(); } }
    // установка цветов
    void setTextColorDefault(u32 _value) { colors[TEXT_COLOR_DEFAULT] = _value; }
    void setTextColorForeground(u32 _value) { if(defPaint->fkColor != _value) { defPaint->fkColor = _value; invalidate(); } }
    void setTextColorBackground(u32 _value) { if(defPaint->bkColor != _value) { defPaint->bkColor = _value; invalidate(); } }
    void setTextColorHighlight(u32 _value) { colors[TEXT_COLOR_HIGHLIGHT] = _value; }
    void setTextColorShadow(u32 _value) { if(colors[TEXT_COLOR_SHADOW] != _value) { colors[TEXT_COLOR_SHADOW] = _value; invalidate(); } }
    // получение цветов
    u32 getTextColorDefault() const { return colors[TEXT_COLOR_DEFAULT]; }
    u32 getTextColorForeground() const { return defPaint->fkColor; }
    u32 getTextColorBackground() const { return defPaint->bkColor; }
    u32 getTextColorHighlight() const { return colors[TEXT_COLOR_HIGHLIGHT]; }
    u32 getTextColorShadow() const { return colors[TEXT_COLOR_SHADOW]; }
    // вернуть размер шрифта
    int getTextSize() { return defPaint->getSize(); }
    // установить стиль текста
    void setTextStyle(int _style) { if(defPaint->getStyle() != _style) { defPaint->setStyle(_style); requestLayout(); } }
    // вернуть текст
    czs& getText() const { return realText; }
    // вернуть стиль текста
    u32 getTextStyle() const { return defPaint->getStyle(); }
    // вставка текста в некоторую позицию
    void insertText(int pos, cstr _text);
    // удаление некоторого количества символов
    void removeText(int pos, int count);
    // вернуть верхнее смещение шрифта
    int getAscent() const { return drw[DRW_TXT]->texture->ascent; }
    // вернуть базовую линию шрифта
    int getBaseline() const { return drw[DRW_TXT]->texture->descent; }
    // установка спана
    void setSpan(zTextSpan* _span, int start, int end, int flags = 0);
    // удаление спана
    void delSpan(zTextSpan* _span);
    // вернуть массив спанов
    const zArray<SPAN*>& getSpans() const { return spans; }
    // установка величины смещения тени текста
    void setShadowOffset(int x, int y) { shadow.set(x, y); invalidate(); }
protected:
    // изменить спаны на длину
    void changeSpans(int pos, int length);
    // вернуть минимальную позицию диапазона
    bool getMinRangePos(int mn, int& ret) const;
    // вывод информации о спанах
    void infoSpans();
    // вернуть спан по позиции
    SPAN* getSpan(int pos) const;
    // событие позиционирования
    virtual void onLayout(crti &position, bool changed) override;
    // событие определения габаритов
    virtual void onMeasure(cszm& spec) override;
    // событие отрисовки
    virtual void onDraw() override { drw[DRW_FK]->draw(nullptr); drawText(); }
    // вернуть текст для отображения
    virtual cstr getDrawText(bool _real) { return realText; }
    // вернуть цвет текста для отображения
    virtual u32 getDrawColorText(u32 color) { return color; }
    // отрисовка текста
    void drawText();
    // отрисовка текста со спанами
    void drawTextSpan(crti& clip);
    // отрисовка фрагмента текста
    int drawFragment(cstr txt, int count, zTextPaint* paint, const CACHE* cache, cpti& coord, crti& clip, int subH = 0);
    // добавление подстроки в кэш
    cstr addCacheSubString(cstr _stext, cstr _text, int& width, int height, bool isEdit);
    // разбивка текста
    szi textWrap(cstr _text, int widthRect);
    // разбивка текста с учетом спанов
    szi textWrapSpan(cstr _text, int widthRect);
    // очистка кэшей
    void clearCacheSpans(bool del_spans);
    // получить строку из кэша строк
    const CACHE* getStringFromCache(int index, rti* tbound, pti& pos);
    // слить все спаны
    void mergeSpans(czs& text);
    // дистанция между текстом и фореграундом
    int distance{0};
    // кэш картинки
    int fkW{0}, fkH{0};
    // максимальная длина текста(для редактора)
    int maxLength{INT_MAX};
    // максимальное количество линий текста
    int maxLines{1};
    // кэш иконки
    szi icSize{};
    // кэшированное значение ширины wrap текста
    int widthRectCache{0};
    // смещение тени
    pti shadow{};
    // массив спанов
    zArray<SPAN*> spans{};
    // кэш спанов
    zArray<SPAN*> cacheSpans{};
    // кэш разбитого на подстроки текста
    zArray<CACHE*> textCache{};
    // спан по умолчанию
    zTextSpan* defSpan{nullptr};
    // краска по умолчанию
    zTextPaint* defPaint{nullptr};
    // цвет текста/подсветки/тени
    u32 colors[3]{};
    // фон текста
    zDrawable* dr{nullptr};
    // реальный текст
    zStringUTF8 realText{};
};

class zFilterEdit {
public:
    // обработать поступивший символ
    virtual int convertChar(czs& _text, int ch, int pos) { return ch; }
    // вернуть текст
    virtual czs& getText(czs& _text) { return _text; }
    // клава по умолчанию
    virtual cstr getKeboardLayer() const { return nullptr; }
};

class zViewClear;
class zViewEdit : public zViewText {
public:
    // конструктор
    zViewEdit(zStyle* _stl, i32 _id, u32 _hint);
    // деструктор
    virtual ~zViewEdit();
    // уведомление о нажатой кнопке
    virtual i32 keyEvent(int key, bool sysKey) override;
    // изменение темы
    virtual void changeTheme() override;
    // сохранение/восстановление состояния
    virtual void stateView(STATE &state, bool save, int &index) override;
    // событие загрузки стилей
    virtual void onInit(bool theme) override;
    // раскладка клавы по умолчанию
    virtual cstr getDefaultKeyboardLayer() const override { return filter ? filter->getKeboardLayer() : nullptr; }
    // вернуть имя типа
    virtual cstr typeName() const override { return "zViewEdit"; }
    // установка фильтра
    void setFilter(u32 inputMode, zFilterEdit* _filter = nullptr);
    // вернуть фильтр
    zFilterEdit* getFilter() const { return filter; }
    // очистка текста
    void clearText();
    // вернуть текст подсказки
    czs& getHint() const { return hintText; }
    // установить текст подсказки
    void setHint(czs& _hint) { hintText = _hint; invalidate(); }
    // максимальная ширина текста
    int getWidthMax() const { return wmax; }
    // установка события редактирования
    void setOnChangeText(std::function<void(zView*, int)> _edit) { onChangeText = std::move(_edit); }
    // установка события каретки
    void setOnChangeCaretPosition(std::function<void(zView*, int)> _pos) { onChangeCaretPos = std::move(_pos); }
    // вернуть цвет подсказки
    u32 getTextHintColor() const { return colorHint; }
    // установить цвет подсказки
    void setTextHintColor(u32 _color) { colorHint = _color; setText(realText, true); }
    // обновление текста
    virtual void updateText(int _what) { if(onChangeText) onChangeText(this, _what); }
protected:
    // отрисовка
    virtual void onDraw() override;
    // позиционирование
    virtual void onLayout(crti &position, bool changed) override;
    // размеры
    virtual void onMeasure(cszm& spec) override;
    // смена фокуса
    virtual void onChangeFocus(bool set) override;
    // касание
    virtual i32 onTouchEvent(zTouch *touch) override;
    // вернуть текст для отображения
    virtual cstr getDrawText(bool _real) override {
        return (_real || realText.isNotEmpty()) ? z_ptrUTF8(filter->getText(realText), visibleIndex) : hintText.str();
    }
    // вернуть цвет текста для отображения
    virtual u32 getDrawColorText(u32 color) override { return (realText.isNotEmpty() ? color : colorHint); }
    // при удалении и вставке символа - начальная позиция текста/позиция каретки
    int correct(int index);
    // коррекция позиции каретки на экране
    void correctCaretPosition(int index);
    // индес в тексте из позиции
    int indexFromPosition(cpti& screen, bool exact);
    // позиция из индекса
    pti positionFromIndex(int _indexText);
    // установка каретки
    void updateCaret();
    // позиция каретки в тексте
    int caretIndex{-1};
    // начальная позиция видимого текста
    int visibleIndex{0};
    // ширина рабочей области редактора
    int wmax{0};
    // позиция каретки на экране
    pti caretScreen{};
    // фейковый текст(hint)
    zStringUTF8 hintText;
    // цвет hint
    u32 colorHint{0};
    // фильтр
    zFilterEdit* filter{nullptr};
    // кнопка отмены
    zViewClear* but{nullptr};
    // событие редактирования
    std::function<void(zView*, int)> onChangeText;
    // событие установки каретки
    std::function<void(zView*, int)> onChangeCaretPos;
};
