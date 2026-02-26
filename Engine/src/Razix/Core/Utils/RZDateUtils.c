#include "RZDateUtils.h"

#define YEAR_OFFSET  1970
#define YEAR_BITS    7
#define MONTH_BITS   4
#define DAY_BITS     5
#define HOUR_BITS    5
#define MINUTE_BITS  6
#define SECOND_BITS  6
#define MAX_YEAR     ((1 << YEAR_BITS) - 1 + YEAR_OFFSET)    // 2097
#define MAX_U32_BITS 32
#define MAX_U16_BITS 16

#define YEAR_MASK   ((1 << YEAR_BITS) - 1)
#define MONTH_MASK  ((1 << MONTH_BITS) - 1)
#define DAY_MASK    ((1 << DAY_BITS) - 1)
#define HOUR_MASK   ((1 << HOUR_BITS) - 1)
#define MINUTE_MASK ((1 << MINUTE_BITS) - 1)
#define SECOND_MASK ((1 << SECOND_BITS) - 1)

RAZIX_API RZPackedDate rz_date_pack(RZDate date)
{
    RZPackedDate packedDate = 0;
    // Pack year (offset by 1970)
    packedDate |= ((date.year - 1970) & YEAR_MASK) << (MAX_U16_BITS - YEAR_BITS);    // 7 bits for year
    // Pack month
    packedDate |= (date.month & MONTH_MASK) << (MAX_U16_BITS - YEAR_BITS - MONTH_BITS);    // 4 bits for month
    // Pack day
    packedDate |= (date.day & DAY_MASK) << (MAX_U16_BITS - YEAR_BITS - MONTH_BITS - DAY_BITS);    // 5 bits for day
    return packedDate;
}

RAZIX_API RZDate rz_date_unpack(RZPackedDate packedDate)
{
    RZDate date = {0};
    date.day    = (packedDate & DAY_MASK);                                                 // unpack day
    date.month  = (packedDate >> (MAX_U16_BITS - YEAR_BITS - MONTH_BITS)) & MONTH_MASK;    // unpack month
    date.year   = ((packedDate >> (MAX_U16_BITS - YEAR_BITS)) & 0x7F) + 1970;              // unpack year
    return date;
}

RAZIX_API RZPackedDateTime rz_datetime_pack(RZDateTime dateTime)
{
    RZPackedDate packedDateTime = 0;
    // Pack year (offset by 1970)
    packedDateTime |= ((dateTime.year - 1970) & YEAR_MASK) << (MAX_U32_BITS - YEAR_BITS);    // 7 bits for year
    // Pack month
    packedDateTime |= (dateTime.month & MONTH_MASK) << (MAX_U32_BITS - YEAR_BITS - MONTH_BITS);    // 4 bits for month
    // Pack day
    packedDateTime |= (dateTime.day & DAY_MASK) << (MAX_U32_BITS - YEAR_BITS - MONTH_BITS - DAY_BITS);    // 5 bits for day
    // Pack hour
    packedDateTime |= (dateTime.hour & HOUR_MASK) << (MAX_U32_BITS - YEAR_BITS - MONTH_BITS - DAY_BITS - HOUR_BITS);    // 5 bits for hour
    // Pack minute
    packedDateTime |= (dateTime.minute & MINUTE_MASK) << (MAX_U32_BITS - YEAR_BITS - MONTH_BITS - DAY_BITS - HOUR_BITS - MINUTE_BITS);    // 6 bits for minute
    // Pack second
    packedDateTime |= (dateTime.second & SECOND_MASK) << 0;    // 6 bits for second
    return packedDateTime;
}

RAZIX_API RZDateTime rz_datetime_unpack(RZPackedDateTime packedDateTime)
{
    RZDateTime dateTime = {0};
    dateTime.second     = (packedDateTime & SECOND_MASK);                                                                                    // unpack second
    dateTime.minute     = (packedDateTime >> (MAX_U32_BITS - YEAR_BITS - MONTH_BITS - DAY_BITS - HOUR_BITS - MINUTE_BITS)) & MINUTE_MASK;    // unpack minute
    dateTime.hour       = (packedDateTime >> (MAX_U32_BITS - YEAR_BITS - MONTH_BITS - DAY_BITS - HOUR_BITS)) & HOUR_MASK;                    // unpack hour
    dateTime.day        = (packedDateTime >> (MAX_U32_BITS - YEAR_BITS - MONTH_BITS - DAY_BITS)) & DAY_MASK;                                 // unpack day
    dateTime.month      = (packedDateTime >> (MAX_U32_BITS - YEAR_BITS - MONTH_BITS)) & MONTH_MASK;                                          // unpack month
    dateTime.year       = ((packedDateTime >> (MAX_U32_BITS - YEAR_BITS)) & YEAR_MASK) + 1970;                                               // unpack year
    return dateTime;
}