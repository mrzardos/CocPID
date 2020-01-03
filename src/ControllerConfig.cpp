//
// ESPressIoT Controller for Espresso Machines
// 2016 by Roman Schmitz
//
// JSON Config File on SPIFFS
//

#include <Arduino.h>
#include <ArduinoJson.h>
#include "ControllerConfig.h"
#include "FS.h"

#define CONFIG_BUFFER_SIZE 1024
#define DEFAULT_FILENAME "/config.json"

ControllerConfig::ControllerConfig() {
    ControllerConfig(DEFAULT_FILENAME);
}

ControllerConfig::ControllerConfig(String fileName) {
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

bool ControllerConfig::loadConfig() {
    loadConfig("/config.json");
}

bool ControllerConfig::loadConfig(String fileName = "/config.json") {
    File configFile = SPIFFS.open(fileName, "r");
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

    wifiSsid = jsonDoc["ssid"];
    wifiPassword = jsonDoc["password"];
    gTargetTemp = jsonDoc["tset"];
    gOvershoot = jsonDoc["tband"];
    gP = jsonDoc["P"], gI = jsonDoc["I"], gD = jsonDoc["D"];
    gaP = jsonDoc["aP"], gaI = jsonDoc["aI"], gaD = jsonDoc["aD"];

    return true;
}

bool ControllerConfig::saveConfig() {
    StaticJsonDocument<CONFIG_BUFFER_SIZE> jsonDoc;
    jsonDoc["ssid"] = wifiSsid;
    jsonDoc["password"] = wifiPassword;
    jsonDoc["tset"] = gTargetTemp;
    jsonDoc["tband"] = gOvershoot;
    jsonDoc["P"] = gP, jsonDoc["I"] = gI, jsonDoc["D"] = gD;
    jsonDoc["aP"] = gaP, jsonDoc["aI"] = gaI, jsonDoc["aD"] = gaD;

    File configFile = SPIFFS.open("/config.json", "w");
    if (!configFile) {
        Serial.println("Failed to open config file for writing");
        return false;
    }

    if (serializeJson(jsonDoc, configFile) == 0) {
        Serial.println(F("Failed to write to file"));
        return false;
    }

    configFile.close();
    return true;
}

void ControllerConfig::resetConfig() {
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

const char *ControllerConfig::getWifiSsid() const {
    return wifiSsid;
}

void ControllerConfig::setWifiSsid(char *wifiSsid) {
    ControllerConfig::wifiSsid = wifiSsid;
}

const char *ControllerConfig:: getWifiPassword() const {
    return wifiPassword;
}

void ControllerConfig::setWifiPassword(char *wifiPassword) {
    ControllerConfig::wifiPassword = wifiPassword;
}
