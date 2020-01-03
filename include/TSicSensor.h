//
// Created by Roman on 03.01.2020.
//

#ifndef ESPRESSIOT_TSICSENSOR_H
#define ESPRESSIOT_TSICSENSOR_H

#include <TemperatureSensor.h>
#include <TSIC.h>

// pins for power and signal
#define TSIC_SIG D4 // D4 pin of NodeMCU just in the right position
#define TSIC_SMP_TIME 100
#define ACCURACY 0.1

class TSicSensor : public TemperatureSensor {
private:
    TSIC *tSicSensor;
    float lastTemperature;
    float SumT;
    float lastErr;
    int CntT;
    uint16_t raw_temp;
    unsigned long lastSensTime;

    boolean isValidMeasurement(float measuredValue);

public:
    TSicSensor();
    virtual ~TSicSensor();

    void initialize();
    void readMeasurement();
    float getTemperature();
};


#endif //ESPRESSIOT_TSICSENSOR_H
