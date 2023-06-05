//
// Created by User on 05.06.2023.
//

#pragma once

#include "zCommonLayout.h"
#include "zViewRibbons.h"

enum MenuAction { actNever = 0, actAlways = 1, actIfRoom = 2 };
enum MenuFlags { menuItemText = 4, menuItemRadio = 8, menuItemCheck = 16,
                 menuItemGroup = 32, menuItemCollapse = 64, menuItemEnabled = 128,
                 menuItemChecked = 256, menuItemVisibled = 512 };

class zMenuGroup;
// класс элемента меню
class zMenuItem {
    friend class zActionBar;
public:
    // конструкторы
    zMenuItem() { }
    zMenuItem(i32 _id, i32 _img, czs& _txt, u32 _flg, zMenuGroup* _grp) : id(_id), img(_img), txt(_txt), flags(_flg), grp(_grp) { }
    ~zMenuItem();
    // оператор сравнения
    bool operator == (int _id) const { return id == _id; }
    // сброс
    void reset() { id = img = flags = 0; txt.empty(); }
    // установить текст
    void setText(czs& _text) { txt = _text; }
    // установить картинку
    void setImage(int _img) { img = _img; }
    // установить доступность
    void setEnabled(bool _set) { updateStatus(menuItemEnabled, _set); }
    // установить видимость
    void setVisibled(bool _set) { updateStatus(menuItemVisibled, _set); }
    // установить выделение
    void setChecked(bool _set) { updateStatus(menuItemChecked, _set); }
    // признак одиночного выбора
    bool isCheckBox() const { return flags & menuItemCheck; }
    // признак множественного выбора
    bool isRadioButton() const { return flags & menuItemRadio; }
    // призак текстового элемента
    bool isText() const { return flags & menuItemText; }
    // признак ссылки на группу
    bool isGroup() const { return flags & menuItemGroup; }
    // признак "свернутого"
    bool isCollapse() const { return flags & menuItemCollapse; }
    // "сворачивать" всегда
    bool isAlways() const { return (flags & 3) == actAlways; }
    // никогда не "сворачивать"
    bool isNever() const { return (flags & 3) == actNever; }
    // признак доступности
    bool isEnabled() const { return flags & menuItemEnabled; }
    // признак выделенного
    bool isChecked() const { return flags & menuItemChecked; }
    // признак видимости
    bool isVisibled() const { return flags & menuItemVisibled; }
    // вернуть идентификатор
    int getId() const { return id; }
    // вернуть картинку
    int getImage() const { return img; }
    // вернуть текст
    czs& getText() const { return txt; }
protected:
    // обновление статуса
    void updateStatus(int status, bool _set) { flags &= ~status; flags |= (status * _set); }
    // идентификатор/изображение
    i32 id{0}, img{0};
    // флаги
    u32 flags{0};
    // заголовок
    zStringUTF8 txt{};
    // указатель на группу, на которую указывает
    zMenuGroup* grp{nullptr};
};

// класс группы элементов меню
class zMenuGroup {
    friend class zActionBar;
public:
    // конструкторы
    zMenuGroup() { }
    zMenuGroup(int _id) : id(_id) { }
    ~zMenuGroup() { reset(); }
    // сброс
    void reset() { children.clear(); }
    // вернуть идентификатор
    int getId() const { return id; }
    // признак наличия элементов
    bool isNotEmpty() const { return children.size() > 1; }
    int count() const { return children.size(); }
    zMenuItem* add(zMenuItem* item) { children += item; return item; }
    zMenuItem* idFind(int _id) const { return atFind(children.indexOf(_id)); }
    zMenuItem* atFind(int _idx) const { return ((_idx >= 0 && _idx < count()) ? children[_idx] : nullptr); }
protected:
    int id{0};
    zArray<zMenuItem*> children{};
};

class zActionBar : public zLinearLayout {
public:
    enum MenuItem { menuEnd, menuPopupBegin, menuPopupEnd, menuItem };
    // структура меню из ресурсов
    struct MENUITEM {
        // тип - группа/элемент
        u32 type;
        // идентификатор элемента/идентификатор текста
        i32 id, title;
        // идентификатор изображения
        i32 image;
        // тип(текст/радио/флажок)|размещение(никогда/всегда/если есть место)
        u32 action;
    };
    enum { ACTION_DELAY = 300 };
    // конструктор
    zActionBar(zStyle* _styles, zStyle* _styles_buttons, zStyle* _styles_popup);
    // деструктор
    virtual ~zActionBar();
    // отобразить/скрыть
    virtual void show(bool _show);
    // проверка на режим блокировки
    virtual bool testLocked() const override;
    // установка содержимого
    virtual void setContent(zView* _view);
    // вернуть имя типа
    virtual cstr typeName() const override { return "zActionBar"; }
    // формирователь
    bool setMenu(int iconApp, MENUITEM* _menu);
    // установка адаптера
    zActionBar* setAdapter(zAdapterList* _adapter);
    // установка клика по элементу
    zActionBar* setOnClickMenuItem(std::function<void(zView*, zMenuItem*)> _click) { onClickMenuItem = std::move(_click); return this; }
    // установка клика по группе
    zActionBar* setOnClickMenuGroup(std::function<void(zView*, zMenuGroup*)> _click) { onClickMenuGroup = std::move(_click); return this; }
protected:
    class zAdapterMenu : public zAdapterArray<zMenuItem> {
    public:
        zAdapterMenu(zActionBar* _bar, zAdapterList* _list) : zAdapterArray<zMenuItem>(), list(_list), bar(_bar) {}
        void setGroup(zMenuGroup* _popup) { group = _popup; notify(); }
        virtual int getCount() const override { return group ? group->count() : 0; }
        virtual const zMenuItem& getItem(int position) const override { return *group->atFind(position); }
        virtual zView* getView(int position, zView *convert, zViewGroup *parent) override;
    protected:
        zActionBar* bar{nullptr};
        zAdapterList* list{nullptr};
        zMenuGroup* group{nullptr};
    };
    // вычисление габаритов
    virtual void onMeasure(cszm& spec) override;
    // вычисление кнопок действий
    int measureButton(int _id, int _image, int widthSize, int heightSize, zMenuGroup* _pop, bool isMeasure);
    // формирование меню
    void recursiveMenu(zMenuGroup* pop);
    // сброс кнопки overflow
    void resetOverflow();
    // клик
    void clickGroup(zView* view, zMenuGroup* pop);
    // корень меню
    zMenuGroup* root{nullptr};
    // кнопка вызова страндартного меню
    zMenuGroup overflow{};
    // адаптер
    zAdapterMenu* adapter{nullptr};
    // стили кнопок
    zStyle* styles_buttons{nullptr};
    // содержимое
    zFrameLayout* content{nullptr};
    // окно
    zViewPopup* popup{nullptr};
    // список
    zViewDropdown* dropdown{nullptr};
    // ожидание анимации
    int delay{ACTION_DELAY};
    // объект для рекурсии
    MENUITEM* _menu{nullptr};
    // нажатая кнопка
    zView* grpView{nullptr};
    // событие клика по элементу меню
    std::function<void(zView*, zMenuItem*)> onClickMenuItem;
    // событие клика по группе меню
    std::function<void(zView*, zMenuGroup*)> onClickMenuGroup;
};
