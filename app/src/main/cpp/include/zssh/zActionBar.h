//
// Created by User on 05.06.2023.
//

#pragma once

#include "zCommonLayout.h"
#include "zViewRibbons.h"

class zMenuGroup;
// класс элемента меню
class zMenuItem {
//    friend class zActionBar;
public:
    // конструкторы
    zMenuItem() { }
    zMenuItem(i32 _id, i32 _img, czs& _txt, u32 _flg, zMenuGroup* _grp) : id(_id), img(_img), txt(_txt), flags(_flg), grp(_grp) { }
    virtual ~zMenuItem();
    // оператор сравнения
    bool operator == (int _id) const { return id == _id; }
    // установить текст
    void setText(czs& _txt);
    // установить картинку
    void setImage(int _img);
    // установить доступность
    void setEnabled(bool _set) { updateStatus(menuItemEnabled, _set); }
    // установить видимость
    void setVisibled(bool _set) { updateStatus(menuItemVisibled, _set); }
    // установить выделение
    void setChecked(bool _set);
    void setFlags(int _flags) { flags = _flags; }
    // возможно чекить
    bool isCheckable() const { return isCheckBox() || isRadioButton(); }
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
    auto getText() const { return txt; }
    // вернуть группу
    auto getGroup() const { return grp; }
    // вернуть флаги
    auto getFlags() const { return flags; }
protected:
    // обновление статуса
    void updateStatus(int status, bool _set);
    // идентификатор/изображение
    i32 id{0}, img{-1};
    // флаги
    u32 flags{0};
    // заголовок
    zStringUTF8 txt{};
    // указатель на группу
    zMenuGroup* grp{nullptr};
};

// класс группы элементов меню
class zMenuGroup : public zMenuItem {
public:
    // конструкторы
    zMenuGroup() { }
    zMenuGroup(int _id) : zMenuItem(_id, -1, "", menuItemGroup, nullptr) { }
    virtual ~zMenuGroup() { reset(0); }
    // сброс
    void reset(int _id) { children.free(); id = _id; }
    void swap(int _1, int _2) { children.swap(_1, _2); }
    zMenuItem** begin() const { return &children[0]; }
    zMenuItem** end() const { return &children[count()]; }
    // признак наличия элементов
    bool isNotEmpty() const { return children.size() > 1; }
    bool isEmpty() const { return children.size() == 0; }
    int count() const { return children.size(); }
    zMenuItem* add(zMenuItem* item) { children += item; return item; }
    zMenuItem* idFind(int _id) const { return atFind(children.indexOf(_id)); }
    zMenuItem* atFind(int _idx) const { return ((_idx >= 0 && _idx < count()) ? children[_idx] : nullptr); }
protected:
    zArray<zMenuItem*> children{};
};

class zActionBar : public zLinearLayout {
public:
    // конструктор
    zActionBar(zStyle* _styles, zStyle* _styles_buttons, zStyle* _styles_popup);
    // деструктор
    virtual ~zActionBar();
    // изменение темы
    virtual void changeTheme() override { popup->changeTheme(); zViewGroup::changeTheme(); }
    // проверка на режим блокировки
    virtual bool testLocked() const override;
    // установка содержимого
    virtual void setContent(zView* _view);
    // сохранение/загрузка
    virtual void stateView(STATE &state, bool save, int &index) override;
    // вернуть имя типа
    virtual cstr typeName() const override { return "zActionBar"; }
    // отобразить/скрыть
    void show(bool _show);
    // формирователь
    bool setMenu(int iconApp, MENUITEM* _menu);
    // сброс переключателей
    void resetRadio();
    // установка адаптера
    zActionBar* setAdapter(zAdapterList* _adapter);
    // установка клика по элементу
    zActionBar* setOnClickMenuItem(std::function<void(zView*, zMenuItem*)> _click) { onClickMenuItem = std::move(_click); return this; }
    // установка клика по группе
    zActionBar* setOnClickMenuGroup(std::function<void(zView*, zMenuGroup*)> _click) { onClickMenuGroup = std::move(_click); return this; }
protected:
    class zAdapterMenu : public zAdapterArray<zMenuItem> {
    public:
        zAdapterMenu(zAdapterList* _list) : zAdapterArray<zMenuItem>(), list(_list) {}
        void setGroup(zMenuGroup* _popup) { group = _popup; notify(); }
        virtual int getCount() const override { return group ? group->count() : 0; }
        virtual const zMenuItem& getItem(int position) const override { return *group->atFind(position); }
        virtual zView* getView(int position, zView *convert, zViewGroup *parent) override;
    protected:
        zAdapterList* list{nullptr};
        zMenuGroup* group{nullptr};
    };
    // вычисление габаритов
    virtual void onMeasure(cszm& spec) override;
    // вычисление кнопок действий
    int measureButton(int _id, int _image, int _flags, czs& _txt, int widthSize, int heightSize, zMenuGroup* _grp);
    // формирование меню
    void recursiveMenu(zMenuGroup* pop);
    // клик
    void clickGroup(zView* view, zMenuGroup* pop);
    // рекурсивное сохранение/восстановление
    void _stateView(zMenuGroup* grp, STATE &state, bool save, int &index);
    // корень меню
    zMenuGroup* root{nullptr};
    // кнопка вызова страндартного меню
    zMenuGroup overflow{};
    zMenuGroup currentGrp{};
    zMenuGroup* cgrp{nullptr};
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
    // объект для рекурсии
    MENUITEM* _menu{nullptr};
    // кнопка группы
    zView* grpView{nullptr};
    // событие клика по элементу меню
    std::function<void(zView*, zMenuItem*)> onClickMenuItem;
    // событие клика по группе меню
    std::function<void(zView*, zMenuGroup*)> onClickMenuGroup;
};
