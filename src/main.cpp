#include <Arduino.h>
#include <MKRNB.h>
#include <elapsedMillis.h>

elapsedMillis timeElapsed;
unsigned int interval = 20000;
const char PINNUMBER[] = "";
unsigned int udpPort = 15683;
IPAddress udpServer(172, 27, 131, 100);

NBClient client;
GPRS gprs;
NB nbAccess;
NBUDP udp;

void setup() {
  
  Serial.begin(9600);
  while (!Serial) {;}

  Serial.println("Starting Arduino GPRS NTP client.");
  
  boolean connected = false;

  while (!connected) {
    if ((nbAccess.begin(PINNUMBER) == NB_READY) && (gprs.attachGPRS() == GPRS_READY)) {
      connected = true;
    } else {
      Serial.println("Not connected");
      delay(1000);
    }
  }

  Serial.println("\nStarting connection to server...");
  udp.begin(udpPort);
}

void array_to_string(byte array[], unsigned int len, char buffer[])
{
    for (unsigned int i = 0; i < len; i++)
    {
        byte nib1 = (array[i] >> 4) & 0x0F;
        byte nib2 = (array[i] >> 0) & 0x0F;
        buffer[i*2+0] = nib1  < 0xA ? '0' + nib1  : 'A' + nib1  - 0xA;
        buffer[i*2+1] = nib2  < 0xA ? '0' + nib2  : 'A' + nib2  - 0xA;
    }
    buffer[len*2] = '\0';
}

void loop() {

  if (timeElapsed > interval) {

    byte uplinkBuffer[4];

    //memset(uplinkBuffer, 0, 4);

    uplinkBuffer[0] = 0xDE;
    uplinkBuffer[1] = 0xAD;
    uplinkBuffer[2] = 0xBE;
    uplinkBuffer[3] = 0xEF;
    
    udp.beginPacket(udpServer, udpPort);
    udp.write(uplinkBuffer, 4);
    udp.endPacket();
    
    Serial.println("send message");

    timeElapsed = 0;

  }

  int downlinkPacketSize = udp.parsePacket();

  if (downlinkPacketSize > 0) {

    Serial.print("packet received: ");
    Serial.print(downlinkPacketSize);
    Serial.println("");

    char downlinkBytes[downlinkPacketSize];    
    udp.read(downlinkBytes, downlinkPacketSize);
    
    //for (int i = 0; i < sizeof(downlinkBytes); i++) {
    //  Serial.write(downlinkBytes[i]);
    //}

    Serial.println(downlinkBytes[0]);
    Serial.println(atoi(downlinkBytes));
    
    Serial.println("");

    //char str[32] = "";
    //array_to_string(downlinkBytes, downlinkPacketSize, str);
    //Serial.println(str);

    udp.flush();

  }
  
  delay(100);
}