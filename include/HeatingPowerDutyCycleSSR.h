//
// Created by Roman on 03.01.2020.
//

#ifndef ESPRESSIOT_HEATINGPOWERDUTYCYCLESSR_H
#define ESPRESSIOT_HEATINGPOWERDUTYCYCLESSR_H

#include <HeatingControl.h>

class HeatingPowerDutyCycleSSR : public HeatingControl {
public:
    HeatingPowerDutyCycleSSR();

    virtual ~HeatingPowerDutyCycleSSR();

    void initialize();

    void setHeaterPower(float power);

    float getHeaterPower();

    void update();


};


#endif //ESPRESSIOT_HEATINGPOWERDUTYCYCLESSR_H
