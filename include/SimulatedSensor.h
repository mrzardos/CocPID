//
// Created by Roman on 03.01.2020.
//

#ifndef ESPRESSIOT_SIMULATEDSENSOR_H
#define ESPRESSIOT_SIMULATEDSENSOR_H

#include <TemperatureSensor.h>

class SimulatedSensor : public TemperatureSensor {
public:
    SimulatedSensor();

    virtual ~SimulatedSensor();

public:
    void initialize();

    void readMeasurement();

    float getTemperature();
};


#endif //ESPRESSIOT_SIMULATEDSENSOR_H
