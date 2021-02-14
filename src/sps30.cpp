#include "sps30.hpp"
#include "debug.hpp"
#include "communication.hpp"

#define BUFFER_SIZE_MEASURE 80
#define BUFFER_SIZE_INFO 60
#define BUFFER_SIZE_FW 32

bool sps30Prepare()
{
    sensirion_i2c_init();
    if(sps30_probe() != 0)
        return false;
    else
        log(sps30ModuleInfo());

    return true;
}

bool sps30ReadNewData(sps30_measurement &data)
{
    struct sps30_measurement val;
    uint16_t data_ready;
    int16_t ret;
    int counter = 50;

    do
    {
        ret = sps30_read_data_ready(&data_ready);
        if(ret < 0)
        {
            return false;
        }
        else if (!data_ready)
        {
            counter++;
        }
        else break;
        
        delay(100);
    }
    while(counter < 50);

    ret = sps30_read_measurement(&val);
    if(ret < 0)
    {
        return false;
    }

    data = val;

    return true;
}

const char* sps30ModuleInfo()
{
    uint8_t major, minor;
    static char buffer[BUFFER_SIZE_INFO];
    char serialNumber[BUFFER_SIZE_FW];

    sps30_get_serial(serialNumber);
    sps30_read_firmware_version(&major, &minor);
    snprintf(buffer, BUFFER_SIZE_INFO, "Serial number: %s firmware version: %d.%d", serialNumber, major, minor);

    return buffer;
}