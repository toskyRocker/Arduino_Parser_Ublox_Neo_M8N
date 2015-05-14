/*
 Andrea Toscano
 U-BLOX NEO M8M Parser
*/

#ifndef UBLOX_H_INCLUDED
#define UBLOX_H_INCLUDED

#include <Arduino.h>


#include <stdint.h>
#include <string.h>
#include <math.h>
#include <stdlib.h>




class Ublox
{
public:

    class Tokeniser
    {
    public:
        Tokeniser(char* str, char token);

        bool next(char* out, int len);

    private:
        char* str;
        char token;
    };


    struct satellite
    {
        uint8_t prn;
        int16_t elevation;
        int16_t azimuth;
        uint8_t snr; //signal to noise ratio
    };

    struct _datetime
    {
        uint8_t day, month, year;
        uint8_t hours, minutes, seconds;
        uint16_t millis;
        bool valid; //1 = yes, 0 = no
    };

    enum _fixtype { FIX_TYPE_NONE, FIX_TYPE_GPS, FIX_TYPE_DIFF };
    enum _fix { FIX_NONE = 1, FIX_2D, FIX_3D };
    enum _op_mode { MODE_MANUAL, MODE_AUTOMATIC };


    bool encode(char c);

    float latitude, longitude, altitude, vert_speed;
    int latlng_age, alt_age;

    //these units are in hundredths
    //so a speed of 5260 means 52.60km/h
    uint16_t speed, course, knots;
    int speed_age, course_age, knots_age;

    _fixtype fixtype; //0 = no fix, 1 = satellite only, 2 = differential fix
    int fixtype_age;
    _fix fix;
    int fix_age;

    float pdop, hdop, vdop; //positional, horizontal and vertical dilution of precision
    int dop_age;

    int8_t sats_in_use;
    int8_t sats_in_view;

    satellite sats[12];
    int sats_age;

    _datetime datetime;
    int time_age, date_age;

    _op_mode op_mode;

private:

    bool check_checksum();

    uint8_t parse_hex(char h);
    bool process_buf();

    char buf[120];
    uint8_t pos;

    void read_gga();
    void read_gsa();
    void read_gsv();
    void read_rmc();
    void read_vtg();

};

//extern Ublox gps;

#endif // UBLOX_H_INCLUDED
