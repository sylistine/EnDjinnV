#include "EnDjinnV.h"
#include "Time.h"

using namespace std;
using namespace Djn;

Time::Time() : startTime(chrono::high_resolution_clock::now()), currTime(startTime) {}

void Time::tick()
{
    lastTime = currTime;
    currTime = chrono::high_resolution_clock::now();
}
