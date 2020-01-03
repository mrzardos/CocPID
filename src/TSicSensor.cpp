//
// Created by Roman on 03.01.2020.
//

#include "Arduino.h"
#include <TSIC.h>
#include <TSicSensor.h>

TSicSensor::TSicSensor() {
    if (!tSicSensor) {
        tSicSensor = new TSIC(TSIC_SIG);
    }
    initialize();
}

TSicSensor::~TSicSensor() {

}

void TSicSensor::initialize() {
    lastTemperature = 0.0;
    SumT = 0.0;
    lastErr = 0.0;
    CntT = 0;
    raw_temp = 0;
    lastSensTime = millis();
}

// very simple selection of noise hits/invalid values: sometimes problems in readout/cabling cause negative values or HUGE jumps
boolean TSicSensor::isValidMeasurement(float measuredValue) {
    return (abs(measuredValue - lastTemperature) < 1.0 || lastTemperature < 1);
}

void TSicSensor::readMeasurement() {
    if (abs(millis() - lastSensTime) >= TSIC_SMP_TIME) {
        if (tSicSensor->getTemperature(&raw_temp)) {
            float measuredTemperature = tSicSensor->calc_Celsius(&raw_temp);

            if (isValidMeasurement(measuredTemperature)) {
                SumT += measuredTemperature;
                CntT++;
            }
            lastSensTime = millis();
        }
    }
}

float TSicSensor::getTemperature() {

    // fallback: if this method is called before a new readout was performed, we will have no new temperature measured. Return last measured temperature.
    float retVal = lastTemperature;

    if (CntT >= 1) {
        retVal = (SumT / CntT);
        SumT = 0.;
        CntT = 0;
    }

    return retVal;
}
