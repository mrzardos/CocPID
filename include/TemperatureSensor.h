//
// Created by Roman on 03.01.2020.
//

#ifndef ESPRESSIOT_TEMPERATURESENSOR_H
#define ESPRESSIOT_TEMPERATURESENSOR_H

class TemperatureSensor {
public:
    TemperatureSensor() {}

    virtual ~TemperatureSensor() {}

public:
    virtual void initialize() = 0;

    virtual void readMeasurement() = 0;

    virtual float getTemperature() = 0;
};


#endif //ESPRESSIOT_TEMPERATURESENSOR_H
