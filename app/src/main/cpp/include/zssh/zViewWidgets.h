
#pragma once

#include "zViewText.h"

class zViewButton : public zViewText {
public:
    // конструктор
    zViewButton(zStyle *_styles, i32 _id, u32 _text, i32 _icon = -1);
    // событие загруки стилей
    virtual void onInit(bool theme) override;
    // вернуть им¤ типа
    virtual cstr typeName() const override { return "zViewButton"; }
    // номер иконки
    void setIcon(i32 _icon);
protected:
    // событие позиционировани¤
    virtual void onLayout(crti &position, bool changed) override;
    virtual void onDraw() override;
    // габариты иконки
    szi szIcon{};
    // тайл иконки
    int icon{-1};
};

class zViewCheck : public zViewText {
public:
    // конструктор
    zViewCheck(zStyle *_styles, i32 _id, u32 _text);
    // сохранение/восстановление
    virtual void stateView(STATE &state, bool save, int &index) override;
    // событие загруки стилей
    virtual void onInit(bool theme) override;
    // вернуть им¤ типа
    virtual cstr typeName() const override { return "zViewCheck"; }
protected:
    // событие касания
    virtual i32 onTouchEvent(zTouch *touch) override;
};

class zViewRadio : public zViewCheck {
public:
    // конструктор
    zViewRadio(zStyle *_styles, i32 _id, u32 _text, u32 _group);
    // вернуть им¤ типа
    virtual cstr typeName() const override { return "zViewRadio"; }
    // пометить
    virtual void checked(bool set) override;
protected:
    // событие касания
    virtual i32 onTouchEvent(zTouch *touch) override;
    // идентификатор группы
    u32 group{0};
};

class zViewSwitch : public zViewCheck {
public:
    // конструктор
    zViewSwitch(zStyle* _styles, i32 _id, u32 _text);
    virtual ~zViewSwitch() { delete trumb; }
    // событие загрузки
    virtual void onInit(bool theme) override;
    // вернуть им¤ типа
    virtual cstr typeName() const override { return "zViewSwitch"; }
    // пометить
    virtual void checked(bool set) override;
    // сохранение/восстановление
    virtual void stateView(STATE &state, bool save, int &index) override;
protected:
    // событие позиционировани¤
    virtual void onLayout(crti &position, bool changed) override;
    virtual void onDraw() override;
    // ползунок
    zDrawable* trumb{nullptr};
};

class zViewSlider : public zViewText {
public:
    // конструктор
    zViewSlider(zStyle* _styles, i32 _id, u32 _text, cszi& _range, int _pos, bool vert);
    // деструктор
    virtual ~zViewSlider() { SAFE_DELETE(trumb); }
    // установка позиции
    virtual bool setProgress(i32 _pos);
    // сохранение/восстановление
    virtual void stateView(STATE &state, bool save, int &index) override;
    // событие загрузки
    virtual void onInit(bool _theme) override;
    // установка диапазона
    void setRange(cszi& r) { range = r; invalidate(); }
    // установка режима
    void setMode(u32 _mode) { if(mode != _mode) { mode = _mode; invalidate(); } }
    // установка признака отображения подсказки(текущей позиции)
    void setTips(bool set) { updateStatus(ZS_TIPS, set); }
    // вернуть признак подсказки
    bool isTips() const { return (status & ZS_TIPS) != 0; }
    // вернуть режим
    u32 getMode() const { return mode; }
    // вернуть прогресс
    i32 getProgress() const { return pos; }
    // вернуть диапазон
    cszi& getRange() const { return range; }
    // вернуть имя типа
    virtual cstr typeName() const override { return "zViewSlider"; }
protected:
    virtual void onDraw() override;
    virtual void onLayout(crti &position, bool changed) override;
    virtual void onMeasure(cszm& spec) override;
    // обновление ползунка
    virtual void updateTrumb();
    // обновление макета
    virtual void updateLayout(bool changed);
    // отображение подсказки
    virtual void showTips();
    // обработка событий
    virtual i32 onTouchEvent(zTouch *touch) override;
    // текущая позиция
    int pos{0};
    // размер ползунка
    int sizeTrumb{1}, sizeTrumb2{1};
    // количество сегментов трека
    int segments{0}, posTrack{0};
    // дельта
    float delta{1.0f};
    // признак отрисовки
    bool isDraw{false};
    // внутренний отрисовщик ползунка
    zDrawable* trumb{nullptr};
    // режим анимации ползунка
    u32 mode{0};
    // скорость трека
    float speedTrack{0};
    // диапазон
    szi range{};
    // текст подсказки
    zStringUTF8 tips{};
    // матрица ползунка
    zMatrix mtxTrumb{};
};

class zViewProgress : public zViewSlider {
public:
    // конструктор
    zViewProgress(zStyle* _styles, i32 _id, u32 _text, cszi& _range, int _pos, bool vert);
    // событие загрузки
    virtual void onInit(bool _theme) override;
    // вернуть имя типа
    virtual cstr typeName() const override { return "zViewProgress"; }
protected:
    // обновление макета
    virtual void updateLayout(bool changed) override;
    virtual void onDraw() override;
    virtual void updateTrumb() override;
    // обработка
    virtual i32 onTouchEvent(zTouch *touch) override { return TOUCH_CONTINUE; }
    virtual void showTips() override;
};

class zViewController : public zView {
public:
    // конструктор
    zViewController(zStyle* _styles, i32 _id, i32 _base, u32 _fileMap);
    // деструктор
    virtual ~zViewController() { SAFE_DELETE(map); }
    // вернуть имя типа
    virtual cstr typeName() const override { return "zViewController"; }
    // установка размера
    void setSize(cszi& _size) { lps.w = _size.w; lps.h = _size.h; requestLayout(); }
    // сброс
    void reset() { buttons = 0; }
    // вернуть нажатые кнопки
    u32 getButtons() const { return buttons; }
    // событие изменения статуса кнопок
    void setOnChangeButton(std::function<void(zView*, int)> _button) { onChangeButton = _button; }
protected:
    virtual void onDraw() override;
    virtual void onMeasure(cszm& spec) override { setMeasuredDimension(spec.w.size(), spec.h.size()); }
    virtual void onLayout(crti &position, bool changed) override;
    virtual i32 onTouchEvent(zTouch *touch) override;
    // нажатые кнопки
    int buttons{0};
    // отношение реальных размеров к размерам на экране
    szf sizeReverseMap{};
    // карта
    u8* map{nullptr};
    // базовый тайл
    i32 base{0};
    // событие
    std::function<void(zView*, int)> onChangeButton;
};

class zViewImage : public zViewText {
public:
    zViewImage(zStyle* _styles, i32 _id, i32 _image);
    // загрузка стилей
    virtual void onInit(bool theme) override;
    // вернуть имя типа
    virtual cstr typeName() const override { return "zViewImage"; }
    // установка изображения
    void setImage(i32 _image);
    // установка текстуры/номера тайла
    void setTexture(zTexture* tex, int _image);
    // вернуть изображение
    i32 getImage() const { return image; }
protected:
    // изображение
    int image{-1};
};

#include <OMXAL/OpenMAXAL.h>
#include <OMXAL/OpenMAXAL_Android.h>

class zViewVideo : public zView {
    friend XAresult AndroidBufferQueueCallback(XAAndroidBufferQueueItf caller, void *ctx, void *bufCtx, void *data,
                                               XAuint32 size, XAuint32 used, const XAAndroidBufferItem *items, XAuint32 itemsLength);
    friend void StreamChangeCallback(XAStreamInformationItf caller, XAuint32 id, XAuint32 idx, void* data, void* ctx);
public:
    enum {
        // количество буферов в нашей очереди буферов, произвольное число
        NB_BUFFERS = 8,
        // мы передаем данные транспортного потока MPEG-2, оперируем размером блока транспортного потока
        MPEG2_TS_PACKET_SIZE = 188,
        // количество блоков транспортного потока MPEG-2 в буфере, произвольное число
        PACKETS_PER_BUFFER = 10,
        // XAAndroidBufferQueueItf, XAStreamInformationItf and XAPlayItf
        NB_MAXAL_INTERFACES = 3,
        // определяет, сколько памяти мы выделяем для кэширования
        BUFFER_SIZE = PACKETS_PER_BUFFER * MPEG2_TS_PACKET_SIZE
    };
    // конструктор
    zViewVideo(zStyle* _styles, i32 _id, cstr name, bool isAsset);
    // деструктор
    virtual ~zViewVideo() { shutdown(); }
    // воспроизведение/пауза
    virtual void play(bool _play);
    // сначала
    virtual void rewind();
    // завершение
    void shutdown();
protected:
    // габариты
    virtual void onMeasure(cszm& spec) override;
    // позиционирование
    virtual void onLayout(crti &position, bool changed) override;
    // создание потока
    virtual bool createStream();
    // инициализация буферов
    bool initBuffers(bool _discontinuity);
    // подгрузка очередной порции
    XAresult callbackBuffer(XAAndroidBufferQueueItf caller, void *bufCtx, void *data, XAuint32 size, XAuint32 used,
                            const XAAndroidBufferItem *items, XAuint32 itemsLen);
    // изменение параметров потока
    static void callbackStream(XAStreamInformationItf caller, XAuint32 eventId, XAuint32 idx, void* data);
    // файл
    zFile* file{nullptr};
    // кэш в памяти для воспроизведения
    char dataCache[BUFFER_SIZE * NB_BUFFERS]{};
    // признак конца файла
    bool reachedEof{false};
    //
    bool discontinuity{false};
    // константа для идентификации контекста буфера, который является концом потока для декодирования
    int kEosBufferCntxt{1980};
    // движок
    XAObjectItf             outputMixObject{nullptr};
    XAObjectItf             engineObject{nullptr};
    XAEngineItf             engine{nullptr};
    // интерфейсы потока медиаплеера
    XAObjectItf             playerObject{nullptr};
    XAPlayItf               player{nullptr};
    XAAndroidBufferQueueItf playerBuffer{nullptr};
    XAStreamInformationItf  playerStream{nullptr};;
    XAVolumeItf             playerVolume{nullptr};
    EGLSurface              surface{nullptr};
    EGLContext              context{nullptr};
};

class zViewToast : public zViewText {
public:
    zViewToast(cstr _text, zStyle* styles = nullptr);
    // инициализация
    virtual void onInit(bool _theme) override;
    // тип представления
    virtual cstr typeName() const override { return "zViewToast"; }
};
