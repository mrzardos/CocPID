//
// Created by Roman on 03.01.2020.
//

#ifndef ESPRESSIOT_HEATINGCONTROL_H
#define ESPRESSIOT_HEATINGCONTROL_H


class HeatingControl {
public:
    HeatingControl() {}

    virtual ~HeatingControl() {}

public:
    virtual void initialize() = 0;

    virtual void setHeaterPower(float power) = 0;

    virtual float getHeaterPower() = 0;

    virtual void update() = 0;
};


#endif //ESPRESSIOT_HEATINGCONTROL_H
