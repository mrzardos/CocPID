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

unsigned long time_now=0;
unsigned long time_last=0;

boolean tuning = false;
boolean osmode = false;
boolean poweroffMode = false;

//
// gloabl variables and classes
//
ControllerStatus pidStatus;
PID ESPPID(&pidStatus.inputTemperature, &pidStatus.outputPower, &pidStatus.targetTemperature, pidStatus.P, pidStatus.I, pidStatus.D, DIRECT);
ControllerConfig espressiotConfig;

void setup()
{
  pidStatus.outputPower=0;

  Serial.begin(115200);

  Serial.println("Mounting SPIFFS...");
  if(espressiotConfig.prepareFS()) {
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
  ESPPID.SetTunings(gP, gI, gD);
  ESPPID.SetSampleTime(PID_INTERVAL);
  ESPPID.SetOutputLimits(0, 1000);
  ESPPID.SetMode(AUTOMATIC);
 
  time_now=millis();
  time_last=time_now;
    
}

void serialStatus() {
  Serial.print(pidStatus.inputTemperature, 2); Serial.print(" ");
  Serial.print(pidStatus.targetTemperature, 2); Serial.print(" ");
  Serial.print(pidStatus.outputPower, 2); Serial.print(" ");
  Serial.print(pidStatus.P, 2); Serial.print(" ");
  Serial.print(pidStatus.I, 2); Serial.print(" ");
  Serial.print(pidStatus.D, 2); Serial.print(" ");
  Serial.print(ESPPID.GetKp(), 2); Serial.print(" ");
  Serial.print(ESPPID.GetKi(), 2); Serial.print(" ");
  Serial.print(ESPPID.GetKd(), 2);
  Serial.println("");
}

void loop() {
  time_now=millis();

  updateTempSensor(); 
  pidStatus.inputTemperature=getTemp();

  if(abs(time_now-time_last)>=PID_INTERVAL or time_last > time_now) {
    if(poweroffMode==true) {
      pidStatus.outputPower=0;
      setHeatPowerPercentage(0);
    }
    else if(tuning==true)
    {
      tuning_loop();
    }
    else  {
      if( !osmode && abs(pidStatus.targetTemperature-pidStatus.inputTemperature)>=espressiotConfig.getOvershootBand()) {
        ESPPID.SetTunings(espressiotConfig.getAdaptiveP(), espressiotConfig.getAdaptiveI(), espressiotConfig.getAdaptiveD());
        osmode=true;
      }
      else if( osmode && abs(pidStatus.targetTemperature-pidStatus.inputTemperature)<espressiotConfig.getOvershootBand() ) {
        ESPPID.SetTunings(espressiotConfig.getP(), espressiotConfig.getI(), espressiotConfig.getD());
        osmode=false;
      }
      if(ESPPID.Compute()==true) {   
        setHeatPowerPercentage(pidStatus.outputPower);
      }
    }        
    
    #ifdef ENABLE_MQTT
    loopMQTT();
    #endif
    
    serialStatus();
    time_last=time_now;
  }

  updateHeater();
  
  #ifdef ENABLE_HTTP
  loopWebSrv();
  #endif 
  
}

