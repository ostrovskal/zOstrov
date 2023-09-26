//
// Created by User on 06.05.2022.
//

#pragma once

#include "android/sensor.h"

class zSensor {
    friend int sensProc(int, int, void* _sens);
    friend class zSensorManager;
public:
    // конструкторы
    zSensor() { }
    zSensor(ASensor* _sensor, int _type, int _id, const std::function<int(zSensor*)>& act) : sensor(_sensor), type(_type), id(_id), onChange(std::move(act)) { }
    // деструктор
    ~zSensor() { ASensorEventQueue_enableSensor(queue, sensor); }
    // настройки
    void settings(int rate) const;
    // оператор сравнения
    bool operator == (int _id) const { return id == _id; }
    // идентификатор/тип
    int id{0}, type{0};
    // сенсор
    ASensor* sensor{nullptr};
    // очередь
    ASensorEventQueue* queue{nullptr};
    // данные
    ASensorEvent event{};
protected:
    // обработка
    virtual int process();
    // обработчик
    std::function<int(zSensor*)> onChange;
};

class zSensorManager {
public:
    // конструктор
    zSensorManager();
    // деструктор
    virtual ~zSensorManager() { sensors.clear(); }
    // подключение сенсора
    zSensor* enableSensor(int type, int id, int rate, ALooper* looper, const std::function<int(zSensor*)>& act);
    // получение сенсора
    zSensor* getSensor(int id) const;
    // отключение сенсора
    bool disableSensor(int id);
    // обработка сенсора
    void processSensor(int id) const;
protected:
    // движок
    ASensorManager* engine{nullptr};
    // массив сенсоров
    zArray<zSensor*> sensors{};
};
