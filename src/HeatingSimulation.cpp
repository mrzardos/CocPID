//
// Created by Roman on 03.01.2020.
//

#include "HeatingSimulation.h"

void HeatingSimulation::initialize() {
    heatCycles = 0.0;
}

void HeatingSimulation::setHeaterPower(float power) {
    if (power < 0.0) {
        power = 0.0;
    }
    if (power > 1000.0) {
        power = 1000.0;
    }
    heatCycles = power;
}

float HeatingSimulation::getHeaterPower() {
    return heatCycles;
}

void HeatingSimulation::update() {
    /*void updateHeater() {
        heatCurrentTime = processingTimestamp;
        if(heatCurrentTime - heatLastTime >= 1000 or heatLastTime > heatCurrentTime) {
            heaterSavedState = getHeatCycles();
            heatLastTime = heatCurrentTime;
        }
    }*/
}
