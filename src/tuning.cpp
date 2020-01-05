//
// ESPressIoT Controller for Espresso Machines
// 2017 by Roman Schmitz
//
// Simple PID tuning procedure
// based on the blog entry http://brettbeauregard.com/blog/2012/01/arduino-pid-autotune-library/
//

#include <PID_v1.h>
#include <ControllerStatus.h>

// external definitions
extern void setHeatPowerPercentage(float);
extern void updateHeater();

double aTuneStep = 100.0, aTuneThres = 0.2;
double AvgUpperT = 0, AvgLowerT = 0;
int UpperCnt = 0, LowerCnt = 0;
int tune_count = 0;

unsigned long tune_time = 0;
unsigned long tune_start = 0;

void tuning_on(PID *pidInstance, ControllerStatus &pidStatus) {
    tune_time = 0;
    tune_start = 0;
    tune_count = 0;
    UpperCnt = 0;
    LowerCnt = 0;
    AvgUpperT = 0;
    AvgLowerT = 0;
    pidInstance->SetMode(MANUAL);
    pidStatus.tuningMode = true;
}

void tuning_off(PID *pidInstance, ControllerStatus &pidStatus) {
    pidInstance->SetMode(AUTOMATIC);
    pidStatus.tuningMode = false;

    double dt = float(tune_time - tune_start) / tune_count;
    double dT = ((AvgUpperT / UpperCnt) - (AvgLowerT / LowerCnt));

    double Ku = 4 * (2 * aTuneStep) / (dT * 3.14159);
    double Pu = dt / 1000; // units of seconds

    pidStatus.P = 0.6 * Ku;
    pidStatus.I = 1.2 * Ku / Pu;
    pidStatus.D = 0.075 * Ku * Pu;
}

void tuning_loop(ControllerStatus &pidStatus, unsigned long processingTimestamp) {
    //
    // count seconds between power-on-cycles
    //
    //
    if (pidStatus.inputTemperature < (pidStatus.targetTemperature - aTuneThres)) { // below lower threshold -> power on
        if (pidStatus.outputPower == 0) { // just fell below threshold
            if (tune_count == 0) tune_start = processingTimestamp;
            tune_time = processingTimestamp;
            tune_count++;
            AvgLowerT += pidStatus.inputTemperature;
            LowerCnt++;
        }
        pidStatus.outputPower = aTuneStep;
        setHeatPowerPercentage(aTuneStep);
    } else if (pidStatus.inputTemperature >
               (pidStatus.targetTemperature + aTuneThres)) { // above upper threshold -> power off
        if (pidStatus.outputPower == aTuneStep) { // just crossed upper threshold
            AvgUpperT += pidStatus.inputTemperature;
            UpperCnt++;
        }
        pidStatus.outputPower = 0;
        setHeatPowerPercentage(0);
    }
}

