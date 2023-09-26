
#pragma once

class zViewGroup;
class zAdapterList;

class zBaseFabric {
public:
	zBaseFabric(zStyle* _styles) : styles(_styles) { }
	virtual ~zBaseFabric() { }
	virtual zView* make(zViewGroup* parent) = 0;
	zStyle* getStyles() const { return styles; }
protected:
	zStyle* styles{nullptr};
};

// Фабрика для создания элемента списка
class zFabricListItem : public zBaseFabric {
public:
	zFabricListItem(zStyle* _styles) : zBaseFabric(_styles) { }
	virtual zView* make(zViewGroup* parent) override;
};

// Фабрика для создания элемента селекта
class zFabricSelectItem : public zBaseFabric {
public:
	zFabricSelectItem(zStyle* _styles) : zBaseFabric(_styles) { }
	virtual zView* make(zViewGroup* parent) override;
};

// Фабрика для создания элемента меню
class zFabricMenuItem : public zBaseFabric {
public:
	zFabricMenuItem(zStyle* _styles) : zBaseFabric(_styles) { }
	virtual zView* make(zViewGroup* parent) override;
};

// Фабрика для создания макета с текстом
class zFabricLayoutItem : public zBaseFabric {
public:
	zFabricLayoutItem(zStyle* _styles, int _count) : zBaseFabric(_styles), count(_count) { }
	virtual zView* make(zViewGroup* parent) override;
	int count{0};
};

class zBaseAdapter {
public:
	// конструктор по умолчанию
	zBaseAdapter() { }
	// деструктор
	virtual ~zBaseAdapter() { owners.free(); }
	// зарегистрировать владельца
	virtual void registration(zView* view);
	// отменить регистрацию владельца
	virtual bool unregistration(zView* view);
	// вернуть количество элеметов
	virtual int getCount() const = 0;
	// вернуть идентификатор элемента
	virtual int getItemId(int position) const { return -1; }
	// вернуть тип элемента
	virtual int getItemViewType(int position) { return 0; }
	// вернуть количество типов
	virtual int getViewTypeCount() { return 1; }
	// вернуть признак пустого адаптера
	virtual bool isEmpty() const { return getCount() == 0; }
	// удалить все
	virtual void clear(bool _delete) = 0;
    // удалить элемент
    virtual void erase(int position, bool _delete) = 0;
	// получить представление
	virtual zView* getView(int position, zView* convert, zViewGroup* parent) { return nullptr; }
	// получить представление для выпадающего списка
	virtual zView* getDropDownView(int position, zView* convert, zViewGroup* parent) {
		return getView(position, convert, parent);
	}
protected:
	// уведомление владельцев об изменении адаптера
	void notify();
	// владельцы
	zArray<zView*> owners{};
};

template<typename T = zString8> class zAdapterArray : public zBaseAdapter {
public:
	// конструктор по умолчанию
	zAdapterArray() { }
	// конструктор по значениям
	zAdapterArray(const zArray<T>& _objects, zBaseFabric* _fabric) : zBaseAdapter(), fabricBase(_fabric), objects(_objects) { }
	// деструктор
	virtual ~zAdapterArray() { SAFE_DELETE(fabricBase); }
	// вернуть количество элеметов
	virtual int getCount() const override { return objects.size(); }
	// удалить все
	virtual void clear(bool _delete) override { if(_delete) objects.clear(); else objects.free(); notify(); }
	// удалить элемент
	virtual void erase(int position, bool _delete) override { objects.erase(position, _delete); notify(); }
	// вернуть элемент
	virtual const T& getItem(int position) const { return objects[position]; }
	// вставить элемент(ы)
	virtual void insert(int position, const T& t) { objects.insert(position, t); notify(); }
	// добавить элемент
	virtual void add(const T& t) { objects += t; notify(); }
	// установка всех элементов сразу
	virtual void addAll(const zArray<T>& objs) { objects = objs; notify(); }
	// фабрика создания представления
	zBaseFabric* fabricBase{nullptr};
protected:
	// массив элементов
	zArray<T> objects{};
};

class zAdapterList : public zAdapterArray<zString8> {
public:
	// конструктор
	zAdapterList() : zAdapterArray<zString8>({}, nullptr) { }
	zAdapterList(const zArray<zString8>& array, zBaseFabric* _caption, zBaseFabric* _dropdown = nullptr) :
				zAdapterArray<zString8>(array, _caption), fabricDropdown(_dropdown) { }
	// деструктор
	virtual ~zAdapterList() { SAFE_DELETE(fabricDropdown); }
	// получить представление
	virtual zView* getView(int position, zView* convert, zViewGroup* parent) override;
	virtual zView* getDropDownView(int position, zView* convert, zViewGroup* parent) override;
	// фабрика для выпадающего списка
	zBaseFabric* fabricDropdown{nullptr};
protected:
	// создать представление
	virtual zView* createView(int position, zView* convert, zViewGroup* parent, zBaseFabric* _fabric, bool color);
};
