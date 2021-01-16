#include "storage.hpp"

void CardPrepare(SDFS &card, uint8_t pin)
{
    card.begin(pin);

    if(!SD.exists("/station"))
    {
        SD.mkdir("/station");
    }
}