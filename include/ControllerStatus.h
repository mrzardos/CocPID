//
// Created by Roman on 25.12.2019.
//

#ifndef ESPRESSIOT_CONTROLLERSTATUS_H
#define ESPRESSIOT_CONTROLLERSTATUS_H

struct ControllerStatus {
    double inputTemperature;
    double outputPower;
    double targetTemperature;
    double P;
    double I;
    double D;
    bool tuningMode;
    bool overshootMode;
    bool poweroffMode;
};

#endif //ESPRESSIOT_CONTROLLERSTATUS_H
