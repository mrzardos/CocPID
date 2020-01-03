//
// Created by Roman on 03.01.2020.
//

#include "SimulatedSensor.h"

/*
#define SIM_TIME 100
#define SIM_T_START 20
#define SIM_T_LOSS 2.9e-2
#define SIM_T_HEAT 0.7787

void setupSensor() {
    curTemp = SIM_T_START;
}

void updateTempSensor() {
    if (abs(time_now - lastSensTime) >= SIM_TIME) {
        lastSensTime=time_now;
        curTemp = (curTemp<SIM_T_START)?(SIM_T_START):(curTemp+(heaterSavedState*SIM_T_HEAT*1e-3)-SIM_T_LOSS);
    }
}

return curTemp+((float)random(-10,10))/100;;
}
 */

SimulatedSensor::SimulatedSensor() {}

SimulatedSensor::~SimulatedSensor() {}

void SimulatedSensor::initialize() {

}

void SimulatedSensor::readMeasurement() {

}

float SimulatedSensor::getTemperature() {
    return 0;
}
