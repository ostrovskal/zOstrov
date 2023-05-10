//
// Created by User on 24.03.2022.
//

#pragma once

#include "sshCommon.h"

class sshApp : public zViewManager {
public:
    sshApp(android_app* android);
    // запуск
    void run();
    // инициализация
    virtual void setContent() override;
    // обработка событий ввода
    void processEvents(i32 event);
protected:
    // глабный объект
    android_app* android{nullptr};
};
