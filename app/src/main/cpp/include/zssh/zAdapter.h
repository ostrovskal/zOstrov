
#pragma once

class zViewGroup;
class zAdapterList;

class zAdapterFabric {
public:
	zAdapterFabric(zStyle* _styles) : styles(_styles) { }
	virtual ~zAdapterFabric() { }
	virtual zView* make(zViewGroup* parent) = 0;
protected:
	zStyle* styles{nullptr};
};

// Фабрика для создания элемента списка
class zAdapterFabricListItem : public zAdapterFabric {
public:
	zAdapterFabricListItem(zStyle* _styles) : zAdapterFabric(_styles) { }
	virtual zView* make(zViewGroup* parent) override;
};

// Фабрика для создания элемента селекта
class zAdapterFabricSpinItem : public zAdapterFabric {
public:
	zAdapterFabricSpinItem(zStyle* _styles) : zAdapterFabric(_styles) { }
	virtual zView* make(zViewGroup* parent) override;
};

class zAdapterFabricMenuItem : public zAdapterFabric {
public:
	zAdapterFabricMenuItem(zStyle* _styles) : zAdapterFabric(_styles) { }
	virtual zView* make(zViewGroup* parent) override;
};

class zAdapter {
public:
	// конструктор по умолчанию
	zAdapter() { }
	// деструктор
	virtual ~zAdapter() { owners.free(); }
	// зарегистрировать владельца
	virtual void registerOwner(zView* view);
	// отменить регистрирацию владельца
	virtual void unregisterOwner(zView* view);
	// вернуть количество элеметов
	virtual int getCount() const { return 0; }
	// вернуть идентификатор элемента
	virtual int getItemId(int position) const { return -1; }
	// вернуть тип элемента
	virtual int getItemViewType(int position) { return 0; }
	// вернуть количество типов
	virtual int getViewTypeCount() { return 1; }
	// вернуть признак пустого адаптера
	virtual bool isEmpty() const { return getCount() == 0; }
	// удалить все
	virtual void clear(bool _delete) {  }
	// получить представление
	virtual zView* getView(int position, zView* convert, zViewGroup* parent) { return nullptr; }
	// получить представление для выпадающего списка
	virtual zView* getDropDownView(int position, zView* convert, zViewGroup* parent) {
		return getView(position, convert, parent);
	}
protected:
	// уведомление владельцев
	void notifyOwners();
	// владельцы
	zArray<zView*> owners{};
};

template<typename T = zString> class zAdapterArray : public zAdapter {
public:
	// конструктор по умолчанию
	zAdapterArray() { }
	// конструктор по значениям
	zAdapterArray(const zArray<T>& _objects, zAdapterFabric* _fabric) : zAdapter(), fabric(_fabric), objects(_objects) { }
	// деструктор
	virtual ~zAdapterArray() { SAFE_DELETE(fabric); }
	// вернуть количество элеметов
	virtual int getCount() const override { return objects.size(); }
	// удалить все
	virtual void clear(bool _delete) override { if(_delete) objects.clear(); else objects.free(); notifyOwners(); }
	// удалить элемент
	virtual void erase(int position, bool _delete) { objects.erase(position, _delete); notifyOwners(); }
	// вернуть элемент
	virtual const T& getItem(int position) const { return objects[position]; }
	// вставить элемент(ы)
	virtual void insert(int position, const T& t) { objects.insert(position, t); notifyOwners(); }
	// добавить элемент
	virtual void add(const T& t) { objects += t; notifyOwners(); }
	// установка всех элементов сразу
	virtual void addAll(const zArray<T>& objs){ objects = objs; notifyOwners(); }
	// фабрика создания представления
	zAdapterFabric* fabric{nullptr};
protected:
	// массив элементов
	zArray<T> objects{};
};

class zAdapterList : public zAdapterArray<zString> {
public:
	// конструктор
	zAdapterList() : zAdapterArray<zString>({}, nullptr) { }
	zAdapterList(const zArray<zString>& array, zAdapterFabric* _caption, zAdapterFabric* _dropdown = nullptr) :
				zAdapterArray<zString>(array, _caption), dropdown(_dropdown) { }
	// деструктор
	virtual ~zAdapterList() { SAFE_DELETE(dropdown); }
	// получить представление
	virtual zView* getView(int position, zView* convert, zViewGroup* parent) override;
	virtual zView* getDropDownView(int position, zView* convert, zViewGroup* parent) override;
	// фабрика для выпадающего списка
	zAdapterFabric* dropdown{nullptr};
protected:
	// создать представление
	virtual zView* createView(int position, zView* convert, zViewGroup* parent, zAdapterFabric* _fabric, bool color);
};

