//
// Created by User on 06.05.2022.
//

#include "zssh/zCommon.h"
#include "zssh/zSensorManager.h"

zSensorManager::zSensorManager() {
    if((engine = ASensorManager_getInstance())) {
        ASensorList sensorList;
        int numSensors(ASensorManager_getSensorList(engine, &sensorList));
        for(int count = 0 ; count < numSensors ; count++ ){
            DLOG("************Sensors Available********************");
            DLOG("%s(%s)", ASensor_getName(sensorList[count]), ASensor_getVendor(sensorList[count]));
        }
    }
}

zSensor* zSensorManager::enableSensor(int type, int id, ALooper* looper) {
    zSensor* sens(nullptr);
    if(engine) {
        auto sensor((ASensor*)ASensorManager_getDefaultSensor(engine, type));
        if(sensor) {
            sens = new zSensor(sensor, type, id);
            if((sens->queue = ASensorManager_createEventQueue(engine, looper, id, nullptr, nullptr))) {
                sens->settings();
                sensors += sens;
            } else {
                SAFE_DELETE(sens);
            }
        }
    }
    return sens;
}

bool zSensorManager::disableSensor(int id) {
    auto idx(sensors.indexOf(id));
    if(idx != -1) sensors.erase(idx, 1);
    return idx != -1;
}

zSensor* zSensorManager::getSensor(int id) const {
    auto idx(sensors.indexOf(id));
    return (idx != -1 ? sensors[idx] : nullptr);
}

void zSensorManager::processSensor(int id) const {
    auto sens(getSensor(id));
    if(sens) sens->process();
}

void zSensor::process() {
    while(ASensorEventQueue_getEvents(queue, &event, 1) > 0) { }
/*
    if(type == ASENSOR_TYPE_ACCELEROMETER) {
        DLOG("accelerometer: x=%f y=%f z=%f", event.acceleration.x, event.acceleration.y, event.acceleration.z);
    }
*/
}

void zSensor::settings() const {
    ASensorEventQueue_enableSensor(queue, sensor);
    ASensorEventQueue_setEventRate(queue, sensor, 20000);
}