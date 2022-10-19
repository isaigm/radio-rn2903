#ifndef RADIO_H
#define RADIO_H
#include <Stream.h>
#include <Arduino.h>
#define DEFAULT_CHAR_TIMEOUT 200
#define DEFAULT_TIMEOUT 500
#define DEFAULT_INPUT_BUFFER_SIZE 128
#define DEFAULT_RECEIVE_BUFFER_SIZE 1024
typedef Uart SerialType;
enum class Mod
{
    LoRa,
    FSK
};
enum class SpreadingFactor
{
    SF7,
    SF8,
    SF9,
    SF10,
    SF11,
    SF12
};
enum class CodingRate
{
    CR4_5,
    CR4_6,
    CR4_7,
    CR4_8
};
class Radio
{
public:
    Radio(Uart *lora);
    ~Radio();
    void setDebugSerial(Stream *stream);
    void allowReception();
    bool initLora();
    bool send(uint8_t *payload, int size);
    void rcv(uint8_t *payload);
    bool setMod(Mod mod);
    bool setFreq(int freq);
    bool setPwr(int pwr);
    bool setSf(SpreadingFactor sf);
    bool setCrc(bool state);
    bool setIqi(bool state);
    bool setCr(CodingRate rate);
    bool setSync(uint8_t syncWord);
    bool setBw(int bandwidth);
private:
    bool expectOk();
    uint16_t readLine(char *buffer, size_t size);
    bool expectStr(const char *str, uint16_t timeout = DEFAULT_TIMEOUT);
    Uart *loraSerial;
    Stream *debugSerial = nullptr;
    char *inputBuffer = nullptr, *receiveBufer = nullptr;
};
#endif