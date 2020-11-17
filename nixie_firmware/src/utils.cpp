#include "utils.hpp"

void activeDelay(unsigned long mills)
{
    elapsedMillis elapsed = 0;
    while (elapsed < mills)
        delay(0);
}