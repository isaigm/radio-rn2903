#include "radio.h"
#include <Sodaq_wdt.h>

Radio::Radio(Uart *lora)
{
    loraSerial = lora;
    inputBuffer = new char[DEFAULT_INPUT_BUFFER_SIZE];
    receiveBufer = new char[DEFAULT_RECEIVE_BUFFER_SIZE];
}
void Radio::setDebugSerial(Stream *serial)
{
    debugSerial = serial;
}
Radio::~Radio()
{
    delete inputBuffer;
    delete receiveBufer;
}
bool Radio::setSf(SpreadingFactor sf)
{
    String command = "radio set sf ";
    switch (sf)
    {
    case SpreadingFactor::SF7:
        command += "sf7";
        break;
    case SpreadingFactor::SF8:
        command += "sf8";
        break;
    case SpreadingFactor::SF9:
        command += "sf9";
        break;
    case SpreadingFactor::SF10:
        command += "sf10";
        break;
    case SpreadingFactor::SF11:
        command += "sf11";
        break;
    case SpreadingFactor::SF12:
        command += "sf12";
        break;
    }
    command += "\r\n";
    loraSerial->print(command);
    return expectOk();
}
bool Radio::setCr(CodingRate cr)
{
    String command = "radio set cr ";
    switch (cr)
    {
    case CodingRate::CR4_5:
        command += "4/5";
        break;
    case CodingRate::CR4_6:
        command += "4/6";
        break;
    case CodingRate::CR4_7:
        command += "4/7";
        break;
    case CodingRate::CR4_8:
        command += "4/8";
        break;
    }
    command += "\r\n";
    loraSerial->print(command);
    return expectOk();
}
bool Radio::setCrc(bool state)
{
    String command = state ? "radio set crc on" : "radio set crc off";
    command += "\r\n";
    loraSerial->print(command);
    return expectOk();
}
bool Radio::setIqi(bool state)
{
    String command = state ? "radio set iqi on" : "radio set iqi off";
    command += "\r\n";
    loraSerial->print(command);
    return expectOk();
}
bool Radio::setFreq(int freq)
{
    String command = "radio set freq " + String(freq) + "\r\n";
    loraSerial->print(command);
    return expectOk();
}
bool Radio::setPwr(int pwr)
{
    String command = "radio set pwr " + String(pwr) + "\r\n";
    loraSerial->print(command);
    return expectOk();
}
bool Radio::setBw(int bandwidth)
{
    String command = "radio set bw " + String(bandwidth) + "\r\n";
    loraSerial->print(command);
    return expectOk();
}
bool Radio::setSync(uint8_t syncWord)
{
    String command = "radio set sync ";
    char hex[5];
    snprintf(hex, sizeof(hex), "%.2X\r\n", syncWord);
    command += hex;
    loraSerial->print(command);
    return expectOk();
}
bool Radio::setMod(Mod mod)
{
    String command = "radio set mod ";
    switch (mod)
    {
    case Mod::LoRa:
        command += "lora";
        break;
    case Mod::FSK:
        command += "fsk";
        break;
    }
    command += "\r\n";
    loraSerial->print(command);
    return expectOk();
}
void Radio::allowReception()
{
    loraSerial->print("radio set wdt 0\r\n");
    expectStr("ok");
    debugSerial->println(inputBuffer);
}
void Radio::rcv(uint8_t *payload)
{
    loraSerial->print("radio rx 0\r\n");
    bool ret = expectStr("ok");

    while (true && ret)
    {
        sodaq_wdt_reset();
        if (readLine(receiveBufer, DEFAULT_RECEIVE_BUFFER_SIZE) > 0)
        {
            debugSerial->println(receiveBufer);
            break;
        }
    }
}
bool Radio::initLora()
{
    bool ret = false;
    loraSerial->setTimeout(DEFAULT_CHAR_TIMEOUT);
    loraSerial->print("mac pause\r\n");
    ret = !expectStr("0");

    ret &= setMod(Mod::LoRa);
    ret &= setFreq(923300000);
    ret &= setPwr(17);
    ret &= setSf(SpreadingFactor::SF7);
    ret &= setCrc(true);
    ret &= setIqi(false);
    ret &= setCr(CodingRate::CR4_7);
    ret &= setSync(0x12);
    ret &= setBw(250);
    return ret;
}
bool Radio::send(uint8_t *payload, int size)
{
    bool ret = false;
    if (size <= 255 && payload)
    {
        debugSerial->println(size);
        String command = "radio tx ";
        for(int i = 0; i < size; i++)
        {
            char hex[3];
            snprintf(hex, sizeof(hex), "%.2X", payload[i]);
            command += hex;
        }
        command += "\r\n";
        
        loraSerial->print(command);
        ret = expectStr("ok");
        debugSerial->println(inputBuffer);
        ret &= expectStr("radio_tx_ok");
        debugSerial->println(inputBuffer);
    }
    return ret;
}
bool Radio::expectStr(const char *str, uint16_t timeout)
{
    bool ret = false;
    unsigned long start = millis();
    while (millis() - start < timeout)
    {
        sodaq_wdt_reset();
        if (readLine(inputBuffer, DEFAULT_INPUT_BUFFER_SIZE) > 0)
        {    
            if (strstr(str, inputBuffer) != NULL)
            {
                ret = true;
            }
            break;
        }
    }
    return ret;
}
uint16_t Radio::readLine(char *buffer, size_t size)
{
    int len = loraSerial->readBytesUntil('\n', buffer, size);
    if (len > 0)
    {
        buffer[len - 1] = '\0';
    }
    return len;
}
bool Radio::expectOk()
{
    return expectStr("ok");
}