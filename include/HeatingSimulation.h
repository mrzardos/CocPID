//
// Created by Roman on 03.01.2020.
//

#ifndef ESPRESSIOT_HEATINGSIMULATION_H
#define ESPRESSIOT_HEATINGSIMULATION_H

#include <HeatingControl.h>

class HeatingSimulation : public HeatingControl {
private:
    float heatCycles;

public:
    void initialize();

    void setHeaterPower(float power);

    float getHeaterPower();

    void update();

};


#endif //ESPRESSIOT_HEATINGSIMULATION_H
