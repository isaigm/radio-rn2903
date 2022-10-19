#include <Arduino.h>
#include "radio.h"
#define loraSerial Serial1
#define debugSerial SERIAL_PORT_MONITOR
Radio *radio;
void setup()
{
	radio = new Radio(&loraSerial);
	delay(3000);
	while ((!SerialUSB) && (millis() < 10000))
	{
	}
	debugSerial.begin(57600);
	loraSerial.begin(57600);
	debugSerial.println("Starting...");
	radio->setDebugSerial(&debugSerial);

	if (radio->initLora())
	{
		debugSerial.println("LoRa started");
		radio->allowReception();
	}
}
void loop()
{
	radio->rcv(nullptr);
}