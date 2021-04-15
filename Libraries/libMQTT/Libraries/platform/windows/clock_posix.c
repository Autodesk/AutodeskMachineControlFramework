#include "clock.h"
#include <windows.h>

/*
 * Time conversion constants.
 */
#define NANOSECONDS_PER_MILLISECOND    ( 1000000L )    /**< @brief Nanoseconds per millisecond. */
#define MILLISECONDS_PER_SECOND        ( 1000L )       /**< @brief Milliseconds per second. */

/*-----------------------------------------------------------*/

long ticks_to_nanos(LONGLONG subsecond_time, LONGLONG frequency)
{
    return (long)((1e9 * subsecond_time) / frequency);
}

uint32_t Clock_GetTimeMs()
{
    //https://docs.microsoft.com/en-us/windows/win32/sysinfo/acquiring-high-resolution-time-stamps

    int64_t timeMs;
    LARGE_INTEGER time;
    LARGE_INTEGER frequency;
    QueryPerformanceCounter(&time);
    QueryPerformanceFrequency(&frequency);
    timeMs =((time.QuadPart / frequency.QuadPart) * MILLISECONDS_PER_SECOND) + ( (ticks_to_nanos(time.QuadPart % frequency.QuadPart, frequency.QuadPart))/ NANOSECONDS_PER_MILLISECOND );
    return (uint32_t)timeMs;
}

uint32_t Clock_GetTimeNs()
{
    //https://docs.microsoft.com/en-us/windows/win32/sysinfo/acquiring-high-resolution-time-stamps

    int64_t timeNs;
    LARGE_INTEGER time;
    LARGE_INTEGER frequency;
    QueryPerformanceCounter(&time);
    QueryPerformanceFrequency(&frequency);
    timeNs = ((time.QuadPart / frequency.QuadPart) * MILLISECONDS_PER_SECOND * NANOSECONDS_PER_MILLISECOND) + ((ticks_to_nanos(time.QuadPart % frequency.QuadPart, frequency.QuadPart)));
    return (uint32_t)timeNs;
}



/*-----------------------------------------------------------*/

void Clock_SleepMs( uint32_t sleepTimeMs )
{
    Sleep(sleepTimeMs);
}