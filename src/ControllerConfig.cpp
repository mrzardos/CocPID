//
// ESPressIoT Controller for Espresso Machines
// 2016 by Roman Schmitz
//
// JSON Config File on SPIFFS
//

#include <Arduino.h>
#include <ArduinoJson.h>
#include "FS.h"

#define CONFIG_BUFFER_SIZE 1024
#define DEFAULT_FILENAME "/config.json"

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

    bool prepareFS();

    bool loadConfig(String fileName);

    double gTargetTemp = S_TSET;
    double gOvershoot = S_TBAND;
    double gP = S_P, gI = S_I, gD = S_D;
    double gaP = S_aP, gaI = S_aI, gaD = S_aD;

public:
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

};

ControllerConfig::ControllerConfig(String fileName = DEFAULT_FILENAME) {
    prepareFS();
    loadConfig(fileName);
}

ControllerConfig::~ControllerConfig() {
}


bool ControllerConfig::prepareFS() {
    if (!SPIFFS.begin()) {
        Serial.println("Failed to mount file system");
        return false;
    }
    return true;
}

bool ControllerConfig::loadConfig(String fileName) {
    File
    configFile = SPIFFS.open(fileName
    "/config.json", "r");
    if (!configFile) {
        Serial.println("Failed to open config file");
        return false;
    }

    StaticJsonDocument<CONFIG_BUFFER_SIZE> jsonDoc;

    DeserializationError parsingError = deserializeJson(jsonDoc, configFile);
    if (parsingError) {
        Serial.println("Failed to deserialize json config file");
        return false;
    }

    //wifi_ssid = jsonDoc["ssid"];
    //wifi_pass = jsonDoc["password"];
    gTargetTemp = jsonDoc["tset"];
    gOvershoot = jsonDoc["tband"];
    gP = jsonDoc["P"], gI = jsonDoc["I"], gD = jsonDoc["D"];
    gaP = jsonDoc["aP"], gaI = jsonDoc["aI"], gaD = jsonDoc["aD"];

    return true;
}

bool saveConfig() {
    StaticJsonBuffer<CONFIG_BUFFER_SIZE> jsonBuffer;
    JsonObject &json = jsonBuffer.createObject();
    //json["ssid"] = wifi_ssid;  json["password"] = wifi_pass;
    json["tset"] = gTargetTemp;
    json["tband"] = gOvershoot;
    json["P"] = gP, json["I"] = gI, json["D"] = gD;
    json["aP"] = gaP, json["aI"] = gaI, json["aD"] = gaD;

    File configFile = SPIFFS.open("/config.json", "w");
    if (!configFile) {
        Serial.println("Failed to open config file for writing");
        return false;
    }

    json.printTo(configFile);
    return true;
}

void resetConfig() {
    gP = S_P;
    gI = S_I;
    gD = S_D;
    gaP = S_aP;
    gaI = S_aI;
    gaD = S_aD;
    gTargetTemp = S_TSET;
    gOvershoot = S_TBAND;
}

double ControllerConfig::getTargetTemperature() {
    return gTargetTemp;
}

void ControllerConfig::setTargetTemperature(double targetTemperature) {
    gTargetTemp = targetTemperature;
}

double ControllerConfig::getOvershootBand() {
    return gOvershoot;
}

void ControllerConfig::setOvershootBand(double overshootBand) {
    gOvershoot = overshootBand;
}

double ControllerConfig::getP() {
    return gP;
}

void ControllerConfig::setP(double parameterValue) {
    gP = parameterValue;
}

double ControllerConfig::getI() {
    return gI;
}

void ControllerConfig::setI(double parameterValue) {
    gI = parameterValue;
}

double ControllerConfig::getD() {
    return gD;
}

void ControllerConfig::setD(double parameterValue) {
    gD = parameterValue;
}


double ControllerConfig::getAdaptiveP() {
    return gaP;
}

void ControllerConfig::setAdaptiveP(double parameterValue) {
    gaP = parameterValue;
}

double ControllerConfig::getAdaptiveI() {
    return gaI;
}

void ControllerConfig::setAdaptiveI(double parameterValue) {
    gaI = parameterValue;
}

double ControllerConfig::getAdaptiveD() {
    return gaD;
}

void ControllerConfig::setAdaptiveD(double parameterValue) {
    gaD = parameterValue;
}
