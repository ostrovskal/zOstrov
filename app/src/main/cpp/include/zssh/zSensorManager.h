//
// Created by User on 06.05.2022.
//

#pragma once

#include "android/sensor.h"

class zSensor {
public:
    // конструкторы
    zSensor() { }
    zSensor(ASensor* _sensor, int _type, int _id) : sensor(_sensor), type(_type), id(_id) { }
    // деструктор
    ~zSensor() { ASensorEventQueue_enableSensor(queue, sensor); }
    // обработка
    void process();
    // настройки
    void settings() const;
    // оператор сравнения
    bool operator == (int _id) const { return id == _id; }
    // идентификатор/тип
    int id{0}, type{0};
    // сенсор
    ASensor* sensor{nullptr};
    // очередь
    ASensorEventQueue* queue{nullptr};
    // событие
    ASensorEvent event{};
};

class zSensorManager {
public:
    // конструктор
    zSensorManager();
    // деструктор
    virtual ~zSensorManager() { sensors.clear(); }
    // подключение сенсора
    zSensor* enableSensor(int type, int id, ALooper* looper);
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
