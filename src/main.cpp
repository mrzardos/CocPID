//
// ESPressIoT Controller for Espresso Machines
// 2016 by Roman Schmitz
//
// Uses PID library
//

#include <Arduino.h>
#include <PID_v1.h>
#include <ESP8266WiFi.h>
#include <ControllerConfig.h>
#include <ControllerStatus.h>
#include <TSicSensor.h>

// external definitions
extern void setupWebSrv();

extern void setupMQTT();

extern void setupHeater();

extern void setupSensor();

extern void updateTempSensor();

extern void setHeatPowerPercentage(float);

extern float getTemp(ControllerStatus &pidStatus);

extern void tuning_loop(ControllerStatus &pidStatus, unsigned long processingTimestamp);

extern void loopMQTT();

extern void updateHeater(unsigned long processingTimestamp);

extern void loopWebSrv();

// WIFI
#define WIFI_SSID "HomeNetwork"
#define WIFI_PASS "MyPassword"

// options for special modules
#define ENABLE_JSON
#define ENABLE_HTTP
#define ENABLE_MQTT

// use simulation or real heater and sensors
//#define SIMULATION_MODE

//
// Intervals for I/O
//
#define HEATER_INTERVAL 1000
#define DISPLAY_INTERVAL 1000
#define PID_INTERVAL 200

unsigned long processingTimestamp = 0;
unsigned long lastProcessingTimestamp = 0;

//
// gloabl variables and classes
//
ControllerStatus pidStatus;
ControllerConfig espressiotConfig;

PID ESPPID(&pidStatus.inputTemperature, &pidStatus.outputPower, &pidStatus.targetTemperature,
           pidStatus.P, pidStatus.I, pidStatus.D, DIRECT);

void setup() {
    pidStatus.outputPower = 0;
    pidStatus.tuningMode = false;
    pidStatus.overshootMode = false;
    pidStatus.poweroffMode = false;

    Serial.begin(115200);

    Serial.println("Mounting SPIFFS...");
    if (espressiotConfig.prepareFS()) {
        Serial.println("Failed to mount SPIFFS !");
    } else {
        Serial.println("Mounted.");
    }

    Serial.println("Loading config...");
    if (!espressiotConfig.loadConfig()) {
        Serial.println("Failed to load config. Using default values and creating config...");
        if (!espressiotConfig.saveConfig()) {
            Serial.println("Failed to save config");
        } else {
            Serial.println("Config saved");
        }
    } else {
        Serial.println("Config loaded");
    }

    Serial.println("Settin up PID...");

    WiFi.begin(WIFI_SSID, WIFI_PASS);
    Serial.println("");
    Serial.print("MAC address: ");
    Serial.println(WiFi.macAddress());

    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }

    Serial.println("");
    Serial.println("WiFi connected.");
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());

#ifdef ENABLE_HTTP
    setupWebSrv();
#endif

#ifdef ENABLE_MQTT
    setupMQTT();
#endif

    // setup components
    setupHeater();
    setupSensor();

    // start PID
    ESPPID.SetTunings(pidStatus.P, pidStatus.I, pidStatus.D);
    ESPPID.SetSampleTime(PID_INTERVAL);
    ESPPID.SetOutputLimits(0, 1000);
    ESPPID.SetMode(AUTOMATIC);

    processingTimestamp = millis();
    lastProcessingTimestamp = processingTimestamp;

}

void serialStatus() {
    Serial.print(pidStatus.inputTemperature, 2);
    Serial.print(" ");
    Serial.print(pidStatus.targetTemperature, 2);
    Serial.print(" ");
    Serial.print(pidStatus.outputPower, 2);
    Serial.print(" ");
    Serial.print(pidStatus.P, 2);
    Serial.print(" ");
    Serial.print(pidStatus.I, 2);
    Serial.print(" ");
    Serial.print(pidStatus.D, 2);
    Serial.print(" ");
    Serial.print(ESPPID.GetKp(), 2);
    Serial.print(" ");
    Serial.print(ESPPID.GetKi(), 2);
    Serial.print(" ");
    Serial.print(ESPPID.GetKd(), 2);
    Serial.println("");
}

void loop() {
    processingTimestamp = millis();

    updateTempSensor();
    pidStatus.inputTemperature = getTemp(pidStatus);

    if (abs(processingTimestamp - lastProcessingTimestamp) >= PID_INTERVAL or lastProcessingTimestamp > processingTimestamp) {
        if (pidStatus.poweroffMode == true) {
            pidStatus.outputPower = 0;
            setHeatPowerPercentage(0);
        } else if (pidStatus.tuningMode == true) {
            tuning_loop(pidStatus, processingTimestamp);
        } else {
            if (!pidStatus.overshootMode &&
                abs(pidStatus.targetTemperature - pidStatus.inputTemperature) >= espressiotConfig.getOvershootBand()) {
                ESPPID.SetTunings(espressiotConfig.getAdaptiveP(), espressiotConfig.getAdaptiveI(),
                                  espressiotConfig.getAdaptiveD());
                pidStatus.overshootMode = true;
            } else if (pidStatus.overshootMode && abs(pidStatus.targetTemperature - pidStatus.inputTemperature) <
                                 espressiotConfig.getOvershootBand()) {
                ESPPID.SetTunings(espressiotConfig.getP(), espressiotConfig.getI(), espressiotConfig.getD());
                pidStatus.overshootMode = false;
            }
            if (ESPPID.Compute() == true) {
                setHeatPowerPercentage(pidStatus.outputPower);
            }
        }

#ifdef ENABLE_MQTT
        loopMQTT();
#endif

        serialStatus();
        lastProcessingTimestamp = processingTimestamp;
    }

    updateHeater(processingTimestamp);

#ifdef ENABLE_HTTP
    loopWebSrv();
#endif

}

