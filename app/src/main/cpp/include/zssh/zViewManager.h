//
// Created by User on 18.04.2023.
//

#pragma once

#include "zstandard/zShader.h"
#include "zViewGroup.h"
#include "zActionBar.h"
#include "zCommonLayout.h"
#include "zViewForms.h"
#include "zViewKeyboard.h"
#include "zSensorManager.h"
#include "zSoundManager.h"

class zImageCache {
public:
    zImageCache() : maxSize(1024 * 1024 * 16) { }
    zImageCache(int size) : maxSize(size * 1024 * 1024) { }
    ~zImageCache();
    // получить
    zTexture* get(cstr _name, zTexture* t);
    // в корзину
    void recycle(zTexture* tx);
    // очистить
    void clear();
    // диагностика
    void info();
    // обновить
    void update();
protected:
    struct CACHE {
        CACHE() { }
        CACHE(zTexture* _tex, int _sz) : tex(_tex), sz(_sz) { millis = z_timeMillis(); }
        zTexture* tex{nullptr};
        int sz{0}; i64 millis{0};
        bool operator == (cstr n) const { return tex->name == n; }
    };
    static u8* load(cstr _name, i32& size) ;
    int findOlder();
    // текущий/максимальный размер
    int curSize{0}, maxSize{0};
    // количество
    int use{0};
    // массив текстур
    zArray<CACHE> tex{};
};

class zViewManager {
    friend class zViewForm;
public:
    enum { Z_NONE, Z_ACTIVE, Z_QUIT, Z_SAVE, Z_LOAD, Z_RESUME, Z_ANIM = 16, Z_CHANGE_THEME = 32 };
    // тип переменных шейдера
    enum { ZSH_APOS, ZSH_ATEX, ZSH_UTEX, ZSH_PMTX, ZSH_WMTX, ZSH_UFLT, ZSH_UCOL };
    // тип цветовых фильтров
    enum { ZCF_NORMAL = 0, ZCF_FOCUSED = 4, ZCF_DISABLED = 5 };
    // конструктор
    zViewManager(ANativeActivity* activity, int size_cache);
    // деструктор
    virtual ~zViewManager();
    // подсчет использования видео памяти
    void volumeVideoMemory(int size, bool _add) { videoMemory += size * (_add * 2 - 1); }
    // привязка формы
    virtual zViewForm* attachForm(zViewForm* form, int width, int height);
    // обработка сенсоров
    virtual void processSensors(int id) const { sensor.processSensor(id); }
    // отображение тоста
    static void showToast(cstr _text, zStyle* _styles = nullptr);
    // обработка события кнопок
    virtual int keyEvent(int key);
    // подготовка к визуализации
    virtual void prepareRender(zVertex2D* vertices, crti& scissor, const zMatrix& wmtx);
    // установка цвета и фильтра
    virtual void setColorFilter(zView* view, const zColor& color);
    // установка матрицы и вьюпорта
    virtual void setMainMatrix(int ww, int hh, bool invert);
    // вернуть базовое окно
    ANativeWindow* getNativeWindow() const { return window; }
    // отрисовка каретки
    void drawCaret(zView* _view);
    // вернуть каретку
    zViewCaret* getCaret() const { return caret; }
    // вернуть панель действий
    zActionBar* getActionBar() const { return actionBar; }
    // вернуть клавиатуру
    zViewKeyboard* getKeyboard() const { return keyboard; }
    // вернуть менеджер активов
    AAssetManager* getAsset() const { return assets; }
    // изменение фокуса
    void changeFocus(zView* view);
    // признак отображение области
    bool isDebug() const { return debug; }
    // вернуть признак портретной ориентации экрана
    bool isLandscape() const { return landscape; }
    // признак активности
    bool isActive() const { return (status & Z_ACTIVE) != 0; }
    // признак выхода
    bool isQuit() const { return status == Z_QUIT; }
    // признак анимаций
    bool isAnim() const { return (status & (Z_ACTIVE | Z_ANIM)) == (Z_ACTIVE | Z_ANIM); }
    // проверка вида на наличие фокуса
    bool isCheckFocus(const zView* view) const { return view == focus; }
    // проверка на смену темы оформления
    bool isChangeTheme() const { return status & Z_CHANGE_THEME; }
    // обработка входящих событий ввода
    virtual i32 processInputEvens(AInputEvent *event);
    // спрятать/отобразить клавиатуру
    virtual void showSoftKeyboard(u32 idOwner, bool _show);
    // преобразовать в dip
    i32 dp(i32 v) const { return (i32)round((float)v * scaleScreen); }
    // прочитать файл из активов
    u8* assetFile(cstr src, i32* plen = nullptr, u8* ptr = nullptr, i32 len = -1, i32 pos = -1);
    // загрузка текстуры
    zTexture* loadResourceTexture(u32 _id, zTexture *_t) const;
    // вернуть корневое/базовое представление
    zViewGroup* getSystemView(bool _root) const { return (_root ? (zViewGroup*)root : (zViewGroup*)common); }
    // удалить все события связанные с определенным представлением
    virtual void eraseAllEventsView(zView* view) { touch.erase(view); event.erase(view, 0); }
    // вернуть представление по ID
    template<typename T = zView> T* idView(u32 _id) const { return common->idView<T>(_id); }
    // вернуть представление по типу
    template<typename T = zView> T* tpView(int* idx) const { return common->tpView<T>(idx); }
    // базовые пути приложения
    czs& getBasePath(int idx) const { return basePaths[idx]; }
    // хэндлер касаний
    zTouchHandler touch;
    // хэндлер событий
    zHandler event;
    // рект текущего экрана
    rti screen{};
    // кэш текстур
    zImageCache* cache{nullptr};
    // менеджер звука
    zSoundManager sound{};
    // менеджер датчиков
    zSensorManager sensor{};
    // буфер обмена
    zStringUTF8 exchangeBuffer{};
    // количество треугольников в кадре
    i32 countVertices{0};
    // количество линий в кадре
    i32 countLn{0};
    // количество объектов в кадре
    i32 countObjs{0};
    // язык системы
    char languages[3]{0};
protected:
    // установка темы оформления
    virtual void setTheme(zStyle* _styles, zResource** _user, zStyles* _user_styles);
    // пересоздание главного окна
    virtual void updateNativeWindow(AConfiguration* config, zStyle* _styles, zResource** _user, zStyles* _user_styles);
    // перерисовка главного окна
    virtual void redrawNativeWindow();
    // установка/потеря фокуса главного окна
    virtual void focusNativeWindow(bool focus);
    // отрисовка представлений
    virtual void drawViews();
    // инициализация дисплея
    virtual bool displayInit();
    // уничтожение дисплея
    void displayDestroy();
    // инициализация
    virtual void setContent() = 0;
    // активация/деактивация
    virtual void appActivate(bool activate, ANativeWindow *_window);
    // обновить параметры конфигурации
    virtual void refreshConfig(AConfiguration* config);
    // восстановить/сохранить состояние всех представлений
    virtual void stateAllViews(u32 action, u8** ptr, u32* size);
    // рекурсивное сохранение состояния представлений
    void saveStateView(zViewGroup* view, zView::STATE &state, int &index);
    // удаление связанных ресуров
    void destroyResources();
    // признак отладки
    bool debug{false};
    // ориентация(портретная/ландшафтная)
    bool landscape{false};
    // направление
    bool leftLayout{false};
    // коэффицент масштабирования
    float scaleScreen{0.0f};
    // статус
    u32 status{Z_NONE};
    // размер буфера сохраненных данных
    u32 sizeViewStates{0};
    // спецификация главного макета
    szm measureCommon{};
    // главное окно
    ANativeWindow *window{nullptr};
    // менеджер активов
    AAssetManager* assets{nullptr};
    // окружение java
    JNIEnv* env{nullptr};
    // текущий цвет
    zColor color{};
    // буфер сохраненных данных
    u8* bufViewStates{nullptr};
    // корневой макет
    zFrameLayout* root{nullptr};
    // представление с фокусом
    zView* focus{nullptr};
private:
    // базовый(скрытый) родительский макет
    zAbsoluteLayout* common{nullptr};
    // текстура для сетки
    zTexture* debugTexture{nullptr};
    // клавиатура
    zViewKeyboard* keyboard{nullptr};
    // блоки структур для сохранения
    zArray<zView::STATE> viewStates;
    // каретка
    zViewCaret* caret{nullptr};
    // панель действий
    zActionBar* actionBar{nullptr};
    // шейдер
    zShader* program{nullptr};
    zShader* programOES{nullptr};
    // переменные в шейдере
    i32 shaderVars[7]{};
    i32 shaderVarsOES[7]{};
    // объем видео памяти
    i32 videoMemory{0};
    // основные пути приложения
    zStringUTF8 basePaths[3];
};
