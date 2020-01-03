//
// Created by Roman on 25.12.2019.
//

#ifndef ESPRESSIOT_CONTROLLERCONFIG_H
#define ESPRESSIOT_CONTROLLERCONFIG_H

#include <Arduino.h>
#include "FS.h"

//
// STANDARD values based on Gaggia CC
//
#define S_P 115.0
#define S_I 4.0
#define S_D 850.0
#define S_aP 100.0
#define S_aI 0.0
#define S_aD 0.0
#define S_TSET 94.0
#define S_TBAND 1.5

class ControllerConfig {
private:
    File configFile;

    char *wifiSsid;
    char *wifiPassword;

    double gTargetTemp;
    double gOvershoot;
    double gP;
    double gI;
    double gD;
    double gaP = S_aP, gaI = S_aI, gaD = S_aD;

public:
    ControllerConfig();

    char *getWifiSsid() const;

    void setWifiSsid(char *wifiSsid);

    char *getWifiPassword() const;

    void setWifiPassword(char *wifiPassword);

    ControllerConfig(String fileName);

    ~ControllerConfig();

    double getTargetTemperature();

    void setTargetTemperature(double targetTemperature);

    double getOvershootBand();

    void setOvershootBand(double overshootBand);

    double getP();

    void setP(double parameterValue);

    double getI();

    void setI(double parameterValue);

    double getD();

    void setD(double parameterValue);

    double getAdaptiveP();

    void setAdaptiveP(double parameterValue);

    double getAdaptiveI();

    void setAdaptiveI(double parameterValue);

    double getAdaptiveD();

    void setAdaptiveD(double parameterValue);

    void resetConfig();

    bool prepareFS();
    bool loadConfig();
    bool loadConfig(String fileName);
    bool saveConfig();

};
#endif //ESPRESSIOT_CONTROLLERCONFIG_H
