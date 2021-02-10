#ifndef STATSTRUCT_HPP
#define STATSTRUCT_HPP

struct statusStruct
{
    bool cardAvailable = true;
    bool bmeAvailable = true;
    bool rtcAvailable = true;
    bool spsAvailable = true;
    bool problemOccured = false;
};

#endif