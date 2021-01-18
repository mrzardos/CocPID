//
// ESPressIoT Controller for Espresso Machines
// 2016 by Roman Schmitz
//
// Telnet Server 
//

#ifdef ENABLE_TELNET

#include <ESP8266WiFi.h>

WiFiServer telnetServer(23);
WiFiClient telnetClient;

void telnetStatus() {
  telnetClient.print(time_now); telnetClient.print(" ");
  telnetClient.print(gInputTemp, 2); telnetClient.print(" ");
  telnetClient.print(gTargetTemp, 2); telnetClient.print(" ");
  telnetClient.print(gOutputPwr, 2); telnetClient.print(" ");
  telnetClient.print(gP, 2); telnetClient.print(" ");
  telnetClient.print(gI, 2); telnetClient.print(" ");
  telnetClient.print(gD, 2); telnetClient.print(" ");
  telnetClient.print(ESPPID.GetKp(), 2); telnetClient.print(" ");
  telnetClient.print(ESPPID.GetKi(), 2); telnetClient.print(" ");
  telnetClient.print(ESPPID.GetKd(), 2);
  telnetClient.println("");
}

void setupTelnet() {
  telnetServer.begin();
  telnetServer.setNoDelay(true);
  Serial.println("Please connect Telnet Client, exit with ^] and 'quit'");

  Serial.print("Free Heap[B]: ");
  Serial.println(ESP.getFreeHeap());
}

void loopTelnet() {
  if (telnetServer.hasClient()){
    if (!telnetClient || !telnetClient.connected()){
      if(telnetClient) telnetClient.stop();
      telnetClient = telnetServer.available();
    } else {
      telnetServer.available().stop();
    }
  }
  if (telnetClient && telnetClient.connected() && telnetClient.available()){
    //while(telnetClient.available())
    //  Serial.write(telnetClient.read());
    telnetStatus();
  }
}

#endif
