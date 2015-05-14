#include "Ublox.h"

Ublox::Tokeniser::Tokeniser(char* _str, char _token)
{
    str = _str;
    token = _token;
}


bool Ublox::Tokeniser::next(char* out, int len)
{
    uint8_t count = 0;

    if(str[0] == 0)
        return false;

    while(true)
    {
        if(str[count] == '\0')
        {
            out[count] = '\0';
            str = &str[count];
            return true;
        }

        if(str[count] == token)
        {
            out[count] = '\0';
            count++;
            str = &str[count];
            return true;
        }

        if(count < len)
            out[count] = str[count];

        count++;
    }
    return false;
}


bool Ublox::encode(char c)
{
    buf[pos] = c;
    pos++;

    if(c == '\n') //linefeed
    {
        bool ret = process_buf();
        memset(buf, '\0', 120);
        pos = 0;
        return ret;
    }

    if(pos >= 120) //avoid a buffer overrun
    {
        memset(buf, '\0', 120);
        pos = 0;
    }
    return false;
}


bool Ublox::process_buf()
{
    if(!check_checksum()) //if checksum is bad
    {
        return false; //return
    }

    //otherwise, what sort of message is it
    if(strncmp(buf, "$GNGGA", 6) == 0)
    
    {
        read_gga();
    }
    if(strncmp(buf, "$GNGSA", 6) == 0)
    {
        read_gsa();
    }

    if(strncmp(buf, "$GPGSV", 6) == 0)
    {
        read_gsv();
    }

    if(strncmp(buf, "$GNRMC", 6) == 0)
    
    {
        read_rmc();
    }
    if(strncmp(buf, "$GNVTG", 6) == 0)
    {
        read_vtg();
    }
    return true;
}

// GNGGA 
void Ublox::read_gga()
{
    int counter = 0;
    char token[20];
    Tokeniser tok(buf, ',');

    while(tok.next(token, 20))
    {
        switch(counter)
        {
        case 1: //time
        {
            float time = atof(token);
            int hms = int(time);

            datetime.millis = time - hms;
            datetime.seconds = fmod(hms, 100);
            hms /= 100;
            datetime.minutes = fmod(hms, 100);
            hms /= 100;
            datetime.hours = hms;

            time_age = millis();
        }
        break;
        case 2: //latitude
        {
            float llat = atof(token);
            int ilat = llat/100;
            double mins = fmod(llat, 100);
            latitude = ilat + (mins/60);
        }
        break;
        case 3: //north/south
        {
            if(token[0] == 'S')
                latitude = -latitude;
        }
        break;
        case 4: //longitude
        {
            float llong = atof(token);
            int ilat = llong/100;
            double mins = fmod(llong, 100);
            longitude = ilat + (mins/60);
        }
        break;
        case 5: //east/west
        {
            if(token[0] == 'W')
                longitude = -longitude;
            latlng_age = millis();
        }
        break;
        case 6:
        {
            fixtype = _fixtype(atoi(token));
        }
        break;
        case 7:
        {
            sats_in_use = atoi(token);
        }
        break;
        case 8:
        {
            hdop = atoi(token);
        }
        break;
        case 9:
        {
            float new_alt = atof(token);
            vert_speed = (new_alt - altitude)/((millis()-alt_age)/1000.0);
            altitude = atof(token);
            alt_age = millis();
        }
        break;
        }
        counter++;
    }
}


void Ublox::read_gsa()
{
    int counter = 0;
    char token[20];
    Tokeniser tok(buf, ',');

    while(tok.next(token, 20))
    {
        switch(counter)
        {
        case 1: //operating mode
        {
            if(token[0] == 'A')
                op_mode = MODE_AUTOMATIC;
            if(token[0] == 'M')
                op_mode = MODE_MANUAL;
        }
        break;
        case 2:
        {
            fix = _fix(atoi(token));
            fix_age = millis();
        }
        break;
        case 14:
        {
            pdop = atof(token);
        }
        break;
        case 15:
        {
            hdop = atof(token);
        }
        break;
        case 16:
        {
            vdop = atof(token);
            dop_age = millis();
        }
        break;
        }
        counter++;
    }
}


void Ublox::read_gsv()
{
    char token[20];
    Tokeniser tok(buf, ',');

    tok.next(token, 20);
    tok.next(token, 20);

    tok.next(token, 20);
    int mn = atoi(token); //msg number

    tok.next(token, 20);
    sats_in_view = atoi(token); //number of sats

    int8_t j = (mn-1) * 4;
    int8_t i;

    for(i = 0; i <= 3; i++)
    {
        tok.next(token, 20);
        sats[j+i].prn = atoi(token);

        tok.next(token, 20);
        sats[j+i].elevation = atoi(token);

        tok.next(token, 20);
        sats[j+i].azimuth = atoi(token);

        tok.next(token, 20);
        sats[j+i].snr = atoi(token);
    }
    sats_age = millis();
}


void Ublox::read_rmc()
{
    int counter = 0;
    char token[20];
    Tokeniser tok(buf, ',');

    while(tok.next(token, 20))
    {
        switch(counter)
        {
        case 1: //time
        {
            float time = atof(token);
            int hms = int(time);

            datetime.millis = time - hms;
            datetime.seconds = fmod(hms, 100);
            hms /= 100;
            datetime.minutes = fmod(hms, 100);
            hms /= 100;
            datetime.hours = hms;

            time_age = millis();
        }
        break;
        case 2:
        {
            if(token[0] == 'A')
                datetime.valid = true;
            if(token[0] == 'V')
                datetime.valid = false;
        }
        break;
        /*
        case 3:
        {
            float llat = atof(token);
            int ilat = llat/100;
            double latmins = fmod(llat, 100);
            latitude = ilat + (latmins/60);
        }
        break;
        case 4:
        {
            if(token[0] == 'S')
                latitude = -latitude;
        }
        break;
        case 5:
        {
            float llong = atof(token);
            float ilat = llong/100;
            double lonmins = fmod(llong, 100);
            longitude = ilat + (lonmins/60);
        }
        break;
        case 6:
        {
             if(token[0] == 'W')
                longitude = -longitude;
            latlng_age = millis();
        }
        break;
        */
        case 8:
        {
            course = atof(token);
            course_age = millis();
        }
        break;
        case 9:
        {
            uint32_t date = atoi(token);
            datetime.year = fmod(date, 100);
            date /= 100;
            datetime.month = fmod(date, 100);
            datetime.day = date / 100;
            date_age = millis();
        }
        break;
        }
        counter++;
    }
}

void Ublox::read_vtg()
{
    int counter = 0;
    char token[20];
    Tokeniser tok(buf, ',');

    while(tok.next(token, 20))
    {
        switch(counter)
        {
        case 1:
        {
            course = (atof(token)*100);
            course_age = millis();
        }
        break;
        case 5:
        {
            knots = (atof(token)*100);
            knots_age = millis();
        }
        break;
        case 7:
        {
            speed = (atof(token)*100);
            speed_age = millis();
        }
        break;
        }
        counter++;
    }
}


bool Ublox::check_checksum()
{
    if (buf[strlen(buf)-5] == '*')
    {
        uint16_t sum = parse_hex(buf[strlen(buf)-4]) * 16;
        sum += parse_hex(buf[strlen(buf)-3]);

        for (uint8_t i=1; i < (strlen(buf)-5); i++)
            sum ^= buf[i];
        if (sum != 0)
            return false;

        return true;
    }
    return false;
}


uint8_t Ublox::parse_hex(char c)
{
    if (c < '0')
        return 0;
    if (c <= '9')
        return c - '0';
    if (c < 'A')
        return 0;
    if (c <= 'F')
        return (c - 'A')+10;
    return 0;
}


