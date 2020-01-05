//
// ESPressIoT Controller for Espresso Machines
// 2016 by Roman Schmitz
//
// Web Server with Options and Plotter
//

#include <ESP8266WebServer.h>
#include <ESP8266HTTPUpdateServer.h>
#include <ControllerStatus.h>
#include <PID_v1.h>
#include <ControllerConfig.h>

ESP8266WebServer server(80);
ESP8266HTTPUpdateServer httpUpdater;

ControllerStatus currentStatus;
ControllerConfig currentConfig;
PID *gPidInstance;

extern void tuning_on(PID *pidInstance, ControllerStatus &pidStatus);
extern void tuning_off(PID *pidInstance, ControllerStatus &pidStatus);

void handleNotFound() {
    String message = "File Not Found\n\n";
    message += "URI: ";
    message += server.uri();
    message += "\nMethod: ";
    message += (server.method() == HTTP_GET) ? "GET" : "POST";
    message += "\nArguments: ";
    message += server.args();
    message += "\n";

    for (uint8_t i = 0; i < server.args(); i++) {
        message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
    }

    server.send(404, "text/plain", message);
}

void handleRoot() {
    String message = "<head><meta http-equiv=\"refresh\" content=\"2\">\n<meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\" />\n<title>EspressIoT</title></head><h1>EspressIoT</h1>\n";
    message += "Measured Temperature: " + String(currentStatus.inputTemperature) + "<br/>\n";
    message += "Target Temperature: " + String(currentStatus.targetTemperature) + "<br/>\n";
    message += "Heater Power: " + String(currentStatus.outputPower) + "<br/>\n";
    message += "\n";
    message += "<hr/>\n";
    if (currentStatus.poweroffMode) message += "<a href=\"./toggleheater\"><button style=\"background-color:#FF0000\">Toggle Heater</button></a><br/>\n";
    else message += "<a href=\"./toggleheater\"><button style=\"background-color:#00FF00\">Toggle Heater</button></a><br/>\n";
    message += "<hr/>\n";
    message += "<a href=\"./config\"><button>Settings</button></a><br/>\n";
    server.send(200, "text/html", message);
}

void handleConfig() {
    String message = "<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\" /><title>EspressIoT Configuration</title></head><h1>EspressIoT</h1>\n";
    if (currentStatus.tuningMode) {
        message += "<h1> PID TUNING MODE RUNNING !</h1>";
        message += "<a href=\"/tuningstats\"><button>Stats</button></a><br/>\n";
        message += "<hr/>\n";
    }
    message += "<form action=\"set_config\">\nTarget Temperature:<br>\n";
    message +=
            "<input type=\"text\" name=\"tset\" value=\"" + String(currentStatus.targetTemperature) + "\"><br/><br/>\n";
    message += "<form action=\"set_config\">\nThreshold for adaptive PID:<br>\n";
    message += "<input type=\"text\" name=\"tband\" value=\"" + String(currentStatus.overshootMode) + "\"><br/><br/>\n";
    message += "normal PID:<br>\n P <input type=\"text\" name=\"pgain\" value=\"" + String(currentConfig.getP()) +
               "\"><br/>\n";
    message += "I <input type=\"text\" name=\"igain\" value=\"" + String(currentConfig.getI()) + "\"><br/>\n";
    message += "D <input type=\"text\" name=\"dgain\" value=\"" + String(currentConfig.getD()) + "\"><br><br>\n";
    message += "adaptive PID:<br>\n P <input type=\"text\" name=\"apgain\" value=\"" +
               String(currentConfig.getAdaptiveP()) + "\"><br/>\n";
    message += "I <input type=\"text\" name=\"aigain\" value=\"" + String(currentConfig.getAdaptiveI()) + "\"><br/>\n";
    message +=
            "D <input type=\"text\" name=\"adgain\" value=\"" + String(currentConfig.getAdaptiveD()) + "\"><br><br>\n";
    message += "<input type=\"submit\" value=\"Submit\">\n</form>";
    message += "<hr/>";
    message += "<a href=\"./loadconf\"><button>Load Config</button></a><br/>\n";
    message += "<a href=\"./saveconf\"><button>Save Config</button></a><br/>\n";
    message += "<a href=\"./resetconf\"><button>Reset Config to Default</button></a><br/>\n";
    message += "<a href=\"./update\"><button>Update Firmware</button></a><br/>\n";
    message += "<hr/>\n";
    //message += "<form action=\"set_tuning\">\nTuning Threshold (\°C):<br>\n";
    //message += "<input type=\"text\" name=\"tunethres\" value=\"" + String(aTuneThres) +"\"><br>\n";
    //message += "Tuning Power (heater)<br>\n";
    //message += "<input type=\"text\" name=\"tunestep\" value=\"" + String(aTuneStep) + "\"><br><br>\n";
    message += "<input type=\"submit\" value=\"Submit\">\n</form><br/>";
    if (!currentStatus.tuningMode) message += "<a href=\"./tuningmode\"><button style=\"background-color:#7070EE\">Start PID Tuning Mode</button></a><br/>\n";
    else message += "<a href=\"./tuningmode\"><button style=\"background-color:#7070EE\">Finish PID Tuning Mode</button></a><br/>\n";
    message += "<hr/>\n";
    message += "<a href=\"/\"><button>Back</button></a><br/>\n";
    server.send(200, "text/html", message);
}

void handleTuningStats() {
    String message = "<head><meta http-equiv=\"refresh\" content=\"5\">\n<meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\" /><title>EspressIoT Configuration</title></head><h1>EspressIoT</h1>\n";
    message += "<h1> PID TUNING STATS </h1>";
//  message += "Total power-on-cycles: " + String(tune_count)+"<br/>\n";
//  message += "Time elapsed: " + String(tune_time-tune_start)+" ms<br/>\n";
//  message += "Average Period: " + String( float(tune_time-tune_start)/tune_count)+" ms<br/>\n";
//  message += "Upper Average: " + String(AvgUpperT/UpperCnt)+" °C<br/>\n";
//  message += "Lower Average: " + String(AvgLowerT/LowerCnt)+" °C<br/>\n";
    message += "<hr/>\n";
    message += "<a href=\"./tuningmode\"><button style=\"background-color:#7070EE\">Finish PID Tuning Mode</button></a><br/>\n";
    message += "<hr/>\n";
    message += "<a href=\"/config\"><button>Back</button></a><br/>\n";
    server.send(200, "text/html", message);
}

void handleSetConfig() {
    String message = "<head><meta http-equiv=\"refresh\" content=\"2;url=/config\">\n<meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\" /><title>EspressIoT</title></head><h1>Configuration changed !</h1>\n";
    for (uint8_t i = 0; i < server.args(); i++) {
        if (server.argName(i) == "tset") {
            message += "new tset: " + server.arg(i) + "<br/>\n";
            currentStatus.targetTemperature = ((server.arg(i)).toFloat());
        } else if (server.argName(i) == "tband") {
            message += "new tset: " + server.arg(i) + "<br/>\n";
            currentConfig.setOvershootBand((server.arg(i)).toFloat());
        } else if (server.argName(i) == "pgain") {
            message += "new pgain: " + server.arg(i) + "<br/>\n";
            currentConfig.setP((server.arg(i)).toFloat());
        } else if (server.argName(i) == "igain") {
            message += "new igain: " + server.arg(i) + "<br/>\n";
            currentConfig.setI((server.arg(i)).toFloat());
        } else if (server.argName(i) == "dgain") {
            message += "new pgain: " + server.arg(i) + "<br/>\n";
            currentConfig.setD((server.arg(i)).toFloat());
        } else if (server.argName(i) == "apgain") {
            message += "new pgain: " + server.arg(i) + "<br/>\n";
            currentConfig.setAdaptiveP((server.arg(i)).toFloat());
        } else if (server.argName(i) == "aigain") {
            message += "new igain: " + server.arg(i) + "<br/>\n";
            currentConfig.setAdaptiveI((server.arg(i)).toFloat());
        } else if (server.argName(i) == "adgain") {
            message += "new pgain: " + server.arg(i) + "<br/>\n";
            currentConfig.setAdaptiveD((server.arg(i)).toFloat());
        }

    }
    server.send(200, "text/html", message);

}

void handleSetTuning() {
    String message = "<head><meta http-equiv=\"refresh\" content=\"2;url=/config\">\n<meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\" /><title>EspressIoT</title></head><h1>Configuration changed !</h1>\n";
    for (uint8_t i = 0; i < server.args(); i++) {
        if (server.argName(i) == "tunethres") {
            message += "new tuning threshold: " + server.arg(i) + "<br/>\n";
//            aTuneThres = ((server.arg(i)).toFloat());
        } else if (server.argName(i) == "tunestep") {
            message += "new tuning power: " + server.arg(i) + "<br/>\n";
//            aTuneStep = ((server.arg(i)).toFloat());
        }
    }
    server.send(200, "text/html", message);

}

void handleLoadConfig() {
    String message = "<head><meta http-equiv=\"refresh\" content=\"2;url=/config\">\n<meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\" /><title>EspressIoT</title></head>";
    if (currentConfig.loadConfig()) message += "<h1>Configuration loaded !</h1>\n";
    else message += "<h1>Error loading configuration !</h1>\n";
    server.send(200, "text/html", message);
}

void handleSaveConfig() {
    String message = "<head><meta http-equiv=\"refresh\" content=\"2;url=/config\">\n<meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\" /><title>EspressIoT</title></head>";
    if (currentConfig.saveConfig()) message += "<h1>Configuration saved !</h1>\n";
    else message += "<h1>Error saving configuration !</h1>\n";
    server.send(200, "text/html", message);
}

void handleResetConfig() {
    String message = "<head><meta http-equiv=\"refresh\" content=\"2;url=/config\">\n<meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\" /><title>EspressIoT</title></head>";
    currentConfig.resetConfig();
    message += "<h1>Configuration set to default !</h1>\n";
    server.send(200, "text/html", message);
}

void handleToggleHeater() {
    String message = "<head><meta http-equiv=\"refresh\" content=\"2;url=/\">\n<meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\" /><title>EspressIoT</title></head>";
    message += "<h1> Done ! </h1>";
    currentStatus.poweroffMode = (!currentStatus.poweroffMode);
    server.send(200, "text/html", message);
}

void handleTuningMode() {
    String message = "<head><meta http-equiv=\"refresh\" content=\"2;url=/config\">\n<meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\" /><title>EspressIoT</title></head>";
    if (!currentStatus.tuningMode) {
        tuning_on(gPidInstance, currentStatus);
        message += "<h1> Started ! </h1>";
    } else {
        message = "<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\" /><title>EspressIoT</title></head>";
        tuning_off(gPidInstance, currentStatus);
        message += "<h1> Finished and new parameters calculated ! </h1>";
//        message += "Total power-on-cycles: " + String(tune_count) + "<br/>\n";
//        message += "Average Period: " + String(float(tune_time - tune_start) / tune_count) + " ms<br/>\n";
//        message += "Average Peak-To-Peak Temperature: " + String((AvgUpperT / UpperCnt) - (AvgLowerT / LowerCnt)) +
//                   " °C<br/>\n";
        message += "<a href=\"/config\"><button>Back</button></a><br/>\n";
    }

    server.send(200, "text/html", message);
}

void setupWebSrv() {

    httpUpdater.setup(&server);
    Serial.print("Updater running !");
    server.on("/", handleRoot);
    server.on("/config", handleConfig);
    server.on("/loadconf", handleLoadConfig);
    server.on("/saveconf", handleSaveConfig);
    server.on("/resetconf", handleResetConfig);
    server.on("/set_config", handleSetConfig);
    server.on("/tuningmode", handleTuningMode);
    server.on("/tuningstats", handleTuningStats);
    server.on("/set_tuning", handleSetTuning);
    server.on("/toggleheater", handleToggleHeater);
    server.onNotFound(handleNotFound);
    server.begin();
    Serial.println("HTTP server started");
}

void loopWebSrv(PID &pidInstance, ControllerConfig &pidConfig, ControllerStatus &pidStatus) {
    currentStatus = pidStatus;
    currentConfig = pidConfig;
    gPidInstance = &pidInstance;

    server.handleClient();
}
