//
// Created by User on 06.05.2022.
//

#include "zssh/zCommon.h"
#include "zssh/zSensorManager.h"

zSensorManager::zSensorManager() {
    if((engine = ASensorManager_getInstance())) {
        ASensorList sensorList;
        int numSensors(ASensorManager_getSensorList(engine, &sensorList));
        DLOG("************Sensors Available********************");
        for(int count = 0 ; count < numSensors ; count++ ) {
            auto sens(sensorList[count]);
            DLOG("%s(%s<%i>)", ASensor_getName(sens), ASensor_getVendor(sens), ASensor_getType(sens));
        }
    }
}

int sensProc(int, int, void* _sens) {
    auto sens((zSensor*)_sens);
    return sens->process();
}

zSensor* zSensorManager::enableSensor(int type, int id, int rate, ALooper* looper, const std::function<int(zSensor*)>& act) {
    zSensor* sens(nullptr);
    if(engine) {
        auto sensor((ASensor*)ASensorManager_getDefaultSensor(engine, type));
        if(sensor) {
            sens = new zSensor(sensor, type, id, act);
            if((sens->queue = ASensorManager_createEventQueue(engine, looper, id, sensProc, sens))) {
                sens->settings(rate);
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

int zSensor::process() {
    while(ASensorEventQueue_getEvents(queue, &event, 1) > 0) { }
    return (onChange ? onChange(this) : 0);
}

void zSensor::settings(int rate) const {
    ASensorEventQueue_enableSensor(queue, sensor);
    ASensorEventQueue_setEventRate(queue, sensor, rate);
}

/*
public void onSensorChanged(SensorEvent event) {
TextView tvX= (TextView)findViewById(R.id.x_axis);
TextView tvY= (TextView)findViewById(R.id.y_axis);
TextView tvZ= (TextView)findViewById(R.id.z_axis);
ImageView iv = (ImageView)findViewById(R.id.image);
float x = event.values[0];
float y = event.values[1];
float z = event.values[2];
if (!mInitialized) {
mLastX = x;
mLastY = y;
mLastZ = z;
tvX.setText("0.0");
tvY.setText("0.0");
tvZ.setText("0.0");
mInitialized = true;
} else {
float deltaX = Math.abs(mLastX - x);
float deltaY = Math.abs(mLastY - y);
float deltaZ = Math.abs(mLastZ - z);
if (deltaX < NOISE) deltaX = (float)0.0;
if (deltaY < NOISE) deltaY = (float)0.0;
if (deltaZ < NOISE) deltaZ = (float)0.0;
mLastX = x;
mLastY = y;
mLastZ = z;
tvX.setText(Float.toString(deltaX));
tvY.setText(Float.toString(deltaY));
tvZ.setText(Float.toString(deltaZ));
iv.setVisibility(View.VISIBLE);
if (deltaX > deltaY) {
iv.setImageResource(R.drawable.horizontal);
} else if (deltaY > deltaX) {
iv.setImageResource(R.drawable.vertical);
} else {
iv.setVisibility(View.INVISIBLE);
}
}
} *
 */