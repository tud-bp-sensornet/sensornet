/**
 * \file
 *         Clock implementation for Tests
 * \author
 *         tud-bp-sensornet
 */

#include "sys/clock.h"
#include <time.h>
#include <sys/time.h>

unsigned long tme = 0x0000;

/*---------------------------------------------------------------------------*/
clock_time_t
clock_time(void)
{
	struct timeval tv;

	gettimeofday(&tv, NULL);

	return tv.tv_sec * 1000 + tv.tv_usec / 1000;
}
/*---------------------------------------------------------------------------*/
unsigned long
clock_seconds(void)
{
	return tme;
}
/*---------------------------------------------------------------------------*/
void
clock_set_seconds(unsigned long sec)
{
	tme = sec;
}
/*---------------------------------------------------------------------------*/
