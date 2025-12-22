#ifndef _RZ_DATE_UTILS_H_
#define _RZ_DATE_UTILS_H_

#include "Razix/Core/RZCore.h"
#include "Razix/Core/RZDataTypes.h"

typedef struct RZDate
{
    u16 year;
    u8  month;
    u8  day;
} RZDate;

typedef struct RZDateTime
{
    u16 year;
    u8  month;
    u8  day;
    u8  hour;
    u8  minute;
    u8  second;
} RZDateTime;

// Compact bit-packed date (2 bytes)
// Year:  1970-2097 (7 bits offset), Month: 1-12 (4 bits), Day: 1-31 (5 bits)
typedef u16 RZPackedDate;

// Compact bit-packed datetime (4 bytes)
// Year: 1970-2097 (7 bits), Month: 1-12 (4 bits), Day: 1-31 (5 bits)
// Hour: 0-23 (5 bits), Minute: 0-59 (6 bits), Second: 0-59 (6 bits)
typedef u32 RZPackedDateTime;

RAZIX_API RZPackedDate     rz_date_pack(RZDate date);
RAZIX_API RZDate           rz_date_unpack(RZPackedDate packedDate);
RAZIX_API RZPackedDateTime rz_datetime_pack(RZDateTime dateTime);
RAZIX_API RZDateTime       rz_datetime_unpack(RZPackedDateTime packedDateTime);

#endif    // _RZ_DATE_UTILS_H_