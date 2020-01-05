//
// Created by Roman on 05.01.2020.
//

#ifndef ESPRESSIOT_ESPRESSIOTSUBPROCESS_H
#define ESPRESSIOT_ESPRESSIOTSUBPROCESS_H


class EspressiotSubprocess {
public:
    EspressiotSubprocess() {}

    virtual ~EspressiotSubprocess() {}

public:
    virtual void setupModule() = 0;

    virtual void processingLoop() = 0;
};

};


#endif //ESPRESSIOT_ESPRESSIOTSUBPROCESS_H
