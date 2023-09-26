//
// Created by User on 18.04.2023.
//

#pragma once

#include "zView.h"

// группа представлений
class zViewGroup : public zView {
    friend class zViewManager;
public:
    enum { ANIM_SCROLL, ANIM_GLOW, ANIM_FLYNG };
    // конструкторы
    zViewGroup() { }
    zViewGroup(zStyle* _styles, i32 _id = 0);
    virtual ~zViewGroup();
    // блокировка/разблокировка
    virtual void disable(bool set) override;
    // пропускать
    virtual void stateView(STATE& state, bool save, int& index) override { }
    // признак сохранения состояния дочерних
    virtual bool stateChildren() const { return true; }
    // касание
    virtual i32 touchEvent(AInputEvent* event) override;
    // вернуть имя типа
    virtual cstr typeName() const override { return "zViewGroup"; }
    // проверка на режим блокировки
    virtual bool testLocked() const override;
    // управление прокруткой
    virtual void awakenScroll();
    // параметры прокруток
    // смещение
    virtual int computeScrollOffset(bool _vert) const{ return 4; }
    // размер всех страниц
    virtual int computeScrollRange(bool _vert) const { return 32; }
    // размер одной страницы
    virtual int computeScrollExtent(bool _vert) const { return 8; }
    // загрузка стилей
    virtual void onInit(bool theme) override;
    // смена темы оформления
    virtual void changeTheme() override;
    // прокрутка содержимого
    virtual bool scrolling(int _delta) { return false; }
    // проверка на пересечение с дочерними
    virtual bool intersectChildren(int x, int y) const;
    // сдвинуть дочерние
    void offsetChildren(int delta);
    // вернуть системное представление
    zView* sysView() const;
    // привязка дочернего
    virtual zView* attach(zView *v, int width, int height, int where = -1);
    virtual zView* attach(zView *v, int x, int y, int width, int height, int where = -1);
    virtual zView* attachInLayout(zView *v, int width, int height, int where = -1);
    // убрать дочернее представление
    virtual void detach(zView *v) { _remove(v, false); }
    // удалить дочернее представление
    virtual void remove(zView* v) { _remove(v, true); }
    // удаление всех дочерних
    virtual void removeAllViews(bool _update);
    // отсоеденить все дочерние представления
    virtual void detachAllViews(bool _update);
    // вернуть представление по типу
    template<typename T = zView> T* tpView(int* idx) const {
        T* v(nullptr); int i;
        for(i = 0 ; i < countChildren(); i++) {
            auto child(children[i]);
            v = dynamic_cast<T*>(child);
            if(v) break;
            auto g(dynamic_cast<zViewGroup*>(child));
            if(g) { if((v = g->tpView<T>(idx))) break; }
        }
        if(idx) *idx = (v ? i : -1);
        return v;
    }
    // вернуть представление по индексу
    template<typename T = zView> T* atView(i32 idx) const {
        if(idx >= 0 && idx < children.size()) {
            return dynamic_cast<T*>(children[idx]);
        }
        return nullptr;
    }
    // поиск представление по его ID
    template<typename T = zView> T* idView(u32 _id) const {
        for(auto& child : children) {
            if(child->id == _id) return dynamic_cast<T*>(child);
            auto group(dynamic_cast<zViewGroup*>(child));
            if(group) {
                auto v(group->idView<T>(_id));
                if(v) return v;
            }
        }
        return nullptr;
    }
    // вернуть количество дочерних представлений
    i32 countChildren() const { return children.size(); }
    // создание параметров для вычисления дочерних
    static zMeasure makeChildMeasureSpec(const zMeasure& spec, int padding, zMeasure childDimension);
    // установка события прокрутки
    zViewGroup* setOnChangeScrolling(std::function<void(zView*, int)> _scroll) { onChangeScroll = _scroll; return this; }
    // установка события выделения
    zViewGroup* setOnChangeSelected(std::function<void(zView*, int)> _selected) { onChangeSelected = _selected; return this; }
protected:
    // диспетчер событий касания
    virtual i32 touchEventDispatch(AInputEvent *event) override;
    // отрисовка
    void onDraw() override;
    // уведомление о событии
    virtual void notifyEvent(HANDLER_MESSAGE* msg) override;
    // отсоединение
    virtual void cleanup() override;
    // вычисление всех дочерних
    virtual szi measureChildren(cszm& spec);
    // вычисление одного дочернего
    virtual void measureChild(zView* child, cszm& spec);
    // обновить глоу
    void updateGlow(int _delta) { if(glow) glow->start((float)_delta / (float)sizes(vert), vert, _delta < 0); }
    // удалить/отсоединить
    void _remove(zView* v, bool _del);
    // разделитель
    zDrawableDivider* div{nullptr};
    // прокрутка
    zViewScrollBar* scrollBar{nullptr};
    // глоу
    zViewGlow* glow{nullptr};
    // флинг
    zFlyng* flyng{nullptr};
    // массив дочерних
    zArray<zView*> children{};
    // чувствительность прокрутки
    szi sizeTouch{5, 5};
    // событие прокрутки
    std::function<void(zView*, int)> onChangeScroll;
    // событие выделения
    std::function<void(zView*, int)> onChangeSelected;
};